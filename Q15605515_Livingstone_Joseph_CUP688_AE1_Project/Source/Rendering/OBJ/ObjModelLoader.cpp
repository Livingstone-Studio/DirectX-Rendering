#include "ObjModelLoader.h"

#include "Model.h"
#include "../Renderer.h"

#include <sstream>
#include <WICTextureLoader.h>

std::unordered_map<std::string, Model*> ObjModelLoader::m_model_atlas;
std::unordered_map<std::string, Material> ObjModelLoader::m_material_atlas;
std::unordered_map<std::string, ID3D11ShaderResourceView*> ObjModelLoader::m_texture_atlas;

HRESULT ObjModelLoader::LoadModel(std::string relative_path, std::string texture_path, Model** model)
{
	if (!model)
	{
		OutputDebugString(L"Failed to find model.");
		return E_FAIL;
	}
	else if (*model)
	{
		OutputDebugString(L"Model already exists.");
		return E_FAIL;
	}

	ModelData model_data;
	HRESULT result = LoadModelData(relative_path, texture_path, &model_data);
	if (FAILED(result))
	{
		OutputDebugString(L"Failed to load model.");
		return E_FAIL;
	}

	// Create model
	*model = new Model(model_data);
	m_model_atlas.insert(std::make_pair(relative_path, *model));

	return S_OK;
}

const Material& ObjModelLoader::GetMaterial(std::string name)
{
	std::unordered_map<std::string, Material>::iterator it;
	it = m_material_atlas.find(name);
	if (it != m_material_atlas.end())
		return m_material_atlas[name];
	return Material();
}

ID3D11ShaderResourceView* ObjModelLoader::GetTexture(std::string name)
{
	std::unordered_map<std::string, ID3D11ShaderResourceView*>::iterator it;
	it = m_texture_atlas.find(name);
	if (it != m_texture_atlas.end())
		return m_texture_atlas[name];
	return NULL;
}

HRESULT ObjModelLoader::LoadModelData(std::string relative_path, std::string texture_path, ModelData* mesh_data)
{
	if (!mesh_data)
	{
		OutputDebugString(L"Failed to find mesh data.");
		return E_FAIL;
	}

	GeometryData geometry_data;
	std::string last_keyword;
	std::string line;
	std::string material = "";
	std::ifstream file(relative_path);
	while (getline(file, line))
	{
		std::istringstream line_stream(line);
		std::string keyword;
		line_stream >> keyword;
		if (keyword == "mtllib")
		{
			line_stream >> keyword;
			LoadModelMaterial("Assets/" + keyword, &mesh_data->materials);
		}
		else if (keyword == "usemtl")
		{
			line_stream >> keyword;
			material = keyword;
		}
		else if (keyword == "v")
		{
			XMFLOAT3 vertex_position;
			LoadModelGeometryData(line, &vertex_position);
			geometry_data.vertices.push_back(vertex_position);
		}
		else if (keyword == "vn")
		{
			XMFLOAT3 vertex_normal;
			LoadModelGeometryData(line, &vertex_normal);
			geometry_data.normals.push_back(vertex_normal);
		}
		else if (keyword == "vt")
		{
			XMFLOAT2 vertex_uv;
			LoadModelGeometryData(line, &vertex_uv);
			geometry_data.uvs.push_back(vertex_uv);
		}
		else if (keyword == "f")
		{
			XMINT3 line_values[3] = { { 0,0,0 } ,{ 0,0,0 } ,{ 0,0,0 } };
			LoadModelTriangleData(line, &line_values[0], &line_values[1], &line_values[2]);
			for (int i = 0; i < 3; ++i)
			{
				geometry_data.indices.push_back(line_values[i]);
				geometry_data.indices_mats.push_back(material);
			}
		}
	}

	material = "";
	int i = 0;
	for (const XMINT3& face : geometry_data.indices)
	{
		Vertex vertex;
		vertex.Pos = geometry_data.vertices[face.x - 1];
		vertex.Norm = geometry_data.normals[face.z - 1];
		vertex.UV = geometry_data.uvs[face.y - 1];
		mesh_data->vertices.push_back(vertex);
		mesh_data->indices.push_back(mesh_data->vertices.size() - 1);

		if (material != geometry_data.indices_mats[i])
		{
			material = geometry_data.indices_mats[i];
			mesh_data->materials.PositionUseData.insert(std::make_pair(mesh_data->vertices.size() - 1, material));
			mesh_data->materials.PositionData.push_back(mesh_data->vertices.size() - 1);
		}
		i++;
	}

	if (texture_path.size() > 0)
		LoadTexture(texture_path, &mesh_data->texture);

	return S_OK;
}

HRESULT ObjModelLoader::LoadModelGeometryData(std::string line, XMFLOAT3* data)
{
	if (!data)
		return E_FAIL;

	std::string contents = line.substr(2, line.size()-1);
	std::istringstream string_stream(contents);
	std::string current;
	float info[3] = { 0,0,0 };

	for (int i = 0; i < 3; ++i)
	{
		string_stream >> current;
		if (current.size() > 0)
			info[i] = std::stof(current);
	}

	*data = { info[0], info[1], info[2] };

	return S_OK;
}

HRESULT ObjModelLoader::LoadModelGeometryData(std::string line, XMFLOAT2* data)
{
	if (!data)
		return E_FAIL;

	std::string contents = line.substr(2, line.size() - 1);
	std::istringstream string_stream(contents);
	std::string current;
	float info[2] = { 0,0 };

	for (int i = 0; i < 2; ++i)
	{
		string_stream >> current;
		if (current.size() > 0)
			info[i] = std::stof(current);
	}

	*data = { info[0], info[1] };

	return S_OK;
}

HRESULT ObjModelLoader::LoadModelTriangleData(std::string line, XMINT3* one, XMINT3* two, XMINT3* three)
{
	if (!one || !two || !three)
		return E_FAIL;

	std::string edited_line = line;
	for (int i = 0; i < edited_line.size(); ++i)
		if (edited_line[i] == '/')
			edited_line.replace(i, 1, " ");

	std::istringstream string_stream(edited_line);
	std::string current;

	int count = 0;
	XMINT3 info[3] = { { 0,0,0 } ,{ 0,0,0 } ,{ 0,0,0 } };
	while (string_stream >> current)
	{
		if (current == "f")
			continue;
		info[count].x = std::stoi(current);
		string_stream >> current;
		info[count].y = std::stoi(current);
		string_stream >> current;
		info[count].z = std::stoi(current);
		count++;
	}



	//int count = 0;
	//int info[3] = { 0,0,0 };
	//std::istringstream section_stream(current);
	//std::string section;
	//while (section_stream >> section)
	//{
	//	info[count] = std::stoi(section);
	//	count++;
	//}

	*one = info[0];
	*two = info[1];
	*three = info[2];

	return S_OK;
}

HRESULT ObjModelLoader::LoadModelMaterial(std::string relative_path, MaterialData* material_data)
{
	Material material;
	std::string line;
	std::ifstream file(relative_path);
	while (getline(file, line))
	{
		std::istringstream line_stream(line);
		std::string keyword;
		line_stream >> keyword;
		if (keyword == "newmtl")
		{
			if (material.Name.size() > 0)
			{
				m_material_atlas.insert(std::make_pair(material.Name, material));
				material.Name = "";
			}

			line_stream >> keyword;
			material.Name = keyword;
		}
		else if (keyword == "Ns")
		{
			XMFLOAT2 info;
			LoadModelGeometryData(line, &info);
			material.SpecularExponent = info.x;
		}
		else if (keyword == "Ka")
		{
			LoadModelGeometryData(line, &material.Ambient);
		}
		else if (keyword == "Kd")
		{
			LoadModelGeometryData(line, &material.Diffuse);
		}
		else if (keyword == "Ks")
		{
			LoadModelGeometryData(line, &material.Specular);
		}
		else if (keyword == "Ke")
		{
			LoadModelGeometryData(line, &material.Emissive);
		}
		else if (keyword == "map_Kd")
		{
			line_stream >> keyword;
			HRESULT result = LoadMaterialTexture("Assets/" + keyword, &material);
			if (!FAILED(result))
			{
				material.TextureId = 1;
				material.Diffuse = { 1,1,1 };
			}
		}
	}

	if (material.Name.size() > 0)
	{
		m_material_atlas.insert(std::make_pair(material.Name, material));
		material.Name = "";
	}

	return S_OK;
}

HRESULT ObjModelLoader::LoadMaterialTexture(std::string relative_path, Material* material)
{
	std::string texture_name = "";
	HRESULT result = LoadTexture(relative_path, &texture_name);
	if (FAILED(result))
	{
		OutputDebugString(L"Failed to create texture.");
		return E_FAIL;
	}
	material->DiffuseMap = texture_name;

	return S_OK;
}

HRESULT ObjModelLoader::LoadTexture(std::string relative_path, std::string* data_store)
{
	if (relative_path.size() == 0)
		return E_NOTIMPL;

	std::unordered_map<std::string, ID3D11ShaderResourceView*>::iterator it;
	it = m_texture_atlas.find(relative_path);
	if (it != m_texture_atlas.end())
		return E_FAIL;

	HRESULT result;
	ID3D11ShaderResourceView* texture = NULL;
	std::wstring widestr = std::wstring(relative_path.begin(), relative_path.end());
	if (relative_path.substr(relative_path.size() - 4, 4) == ".dds")
	{
		result = DirectX::CreateDDSTextureFromFile(
			Renderer::Instance->GetDevice(),
			Renderer::Instance->GetDeviceContext(),
			widestr.c_str(), NULL, &texture);
	}
	else 
	{
		result = DirectX::CreateWICTextureFromFile(
			Renderer::Instance->GetDevice(),
			Renderer::Instance->GetDeviceContext(),
			widestr.c_str(), NULL, &texture);
	}

	if (FAILED(result))
	{
		OutputDebugString(L"Failed to create texture.");
		return E_FAIL;
	}

	size_t start_index = relative_path.find("/", 0);
	*data_store = relative_path.substr(start_index + 1, relative_path.size() - 1 - start_index + 1);
	m_texture_atlas.insert(std::make_pair(*data_store, texture));

	return S_OK;
}

void ObjModelLoader::Cleanup()
{
	for (auto& pair : m_texture_atlas)
	{
		if (pair.second)
			pair.second->Release();
	}
	for (auto& pair : m_model_atlas)
	{
		if (pair.second)
			delete pair.second;
	}
}