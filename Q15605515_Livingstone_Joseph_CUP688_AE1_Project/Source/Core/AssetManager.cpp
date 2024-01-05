#include "AssetManager.h"

#include "../Rendering/Mesh.h"
#include "../Rendering/Renderer.h"
#include <fstream>
#include <string>
#include <corecrt_wstring.h>
#include <sstream>

std::vector<Material> AssetManager::m_mats;
std::map<std::string, int> AssetManager::m_mat_ids;
std::map<std::string, ID3D11ShaderResourceView*> AssetManager::m_textures;
int AssetManager::m_next_id = 0;

struct Face
{
	Face(std::string material,
		int vertexOne, int vertexTwo, int vertexThree, int vertexFour,
		int uvOne, int uvTwo, int uvThree, int uvFour,
		int normalOne, int normalTwo, int normalThree, int normalFour)
		: MaterialName{ material },
		  VertexIndex{ vertexOne, vertexTwo, vertexThree, vertexFour },
		  UVIndex{ uvOne, uvTwo, uvThree, uvFour },
		  NormalIndex{ normalOne, normalTwo, normalThree, normalFour }
	{}

	std::string MaterialName = "";
	int VertexIndex[4] = { 0,0,0, 0 };
	int UVIndex[4] = { 0, 0, 0, 0 };
	int NormalIndex[4] = { 0, 0, 0, 0 };
};

void AssetManager::Cleanup()
{
	for (auto pair : m_textures)
	{
		if (pair.second)
			pair.second->Release();
	}
	m_textures.empty();
}

Mesh AssetManager::LoadObj(const wchar_t* filePath)
{
	// Mesh Initialisation
	Mesh mesh;

	std::string current_mat = "";

	// Initializing vectors that store obj info
	std::vector<XMFLOAT3> vertices;
	std::vector<XMFLOAT2> uvs;
	std::vector<XMFLOAT3> normals;
	std::vector<Face> faces;

	// Reading Obj 
	std::ifstream file(filePath);
	std::string line;
	const char* s;
	while (getline(file, line))
	{
		std::stringstream line_stream(line);
		float x, y, z;
		switch (line[0])
		{
		case 'v':
		{
			switch (line[1])
			{
			case ' ':
				sscanf_s(line.c_str(), "v %f %f %f", &x, &y, &z);
				vertices.push_back({ x,y,z });
				break;
			case 'n':
				sscanf_s(line.c_str(), "vn %f %f %f", &x, &y, &z);
				normals.push_back({ x,y,z });
				break;
			case 't':
				sscanf_s(line.c_str(), "vt %f %f", &x, &y);
				uvs.push_back({ x,y });
				break;
			}
			break;
		}
		case 'u':
		{
			std::istringstream iss(line);
			std::string keyword;
			std::string name;
			iss >> keyword;
			iss >> name;
			current_mat = name;
			break;
		}
		case 'f':
		{
			int v0 = 0, v1 = 0, v2 = 0, v3 = 0, t0 = 0, t1 = 0, t2 = 0, t3 = 0, n0 = 0, n1 = 0, n2 = 0, n3 = 0;
			sscanf_s(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", &v0, &t0, &n0, &v1, &t1, &n1, &v2, &t2, &n2, &v3, &t3, &n3);
			Face face(current_mat, v0 - 1, v1 - 1, v2 - 1, v3 - 1, t0 - 1, t1 - 1, t2 - 1, t3 - 1, n0 - 1, n1 - 1, n2 - 1, n3 - 1);
			faces.push_back(face);
			break;
		}
		case 'm':
		{
			std::istringstream iss(line);
			std::string keyword;
			std::string matName;

			iss >> keyword;
			iss >> matName;

			LoadMaterialFile("Assets/" + matName);
			break;
		}
		}
	}

	// Mesh Building
	std::string material = "";
	//int count = 0;
	for (Face face : faces)
	{
		bool four = true;
		for (int i = 0; i < 4; ++i)
		{
			if (face.VertexIndex[i] < 0)
			{
				four = false;
				continue;
			}

			XMFLOAT3 position = vertices[face.VertexIndex[i]];
			XMFLOAT2 uv = uvs[face.UVIndex[i]];
			XMFLOAT3 normal = normals[face.NormalIndex[i]];
			mesh.Vertices.push_back({ position, {1,1,1,1}, uv, normal});
			mesh.Indices.push_back(mesh.Vertices.size() - 1);
			
			if (material != face.MaterialName)
			{
				material = face.MaterialName;
				mesh.Materials.insert(std::make_pair(mesh.Vertices.size() - 1, GetMaterialID(face.MaterialName)));
				mesh.MaterialIndexs.push_back(mesh.Vertices.size() - 1);
			}
			//count++;
		}
	}

	// Return Built Mesh
	return mesh;
}

void AssetManager::LoadMaterialFile(std::string file)
{
	Material material;

	std::string keyword;

	std::ifstream matfile(file);
	std::string line;
	const char* s;
	while (getline(matfile, line))
	{
		std::stringstream line_stream(line);
		switch (line[0])
		{
		case 'i':
		{
			int i;
			sscanf_s(line.c_str(), "illum %i", &i);
			material.Illumination = { i };
			break;
		}
		case 'n':
		{
			if ((material.Id != -1))
			{
				m_mats.push_back(material);

				material.Id = -1;
				material.Illumination = 1;
				material.Ambient = { 1.0f, 1.0f, 1.0f };
				material.Diffuse = { 1.0f, 1.0f, 1.0f };
				material.Specular = { 1.0f, 1.0f, 1.0f };
				material.Emissive = { 1.0f, 1.0f, 1.0f };			
				material.SpecularExponent = 0.0f;
			}
			std::string name;
			std::istringstream iss(line);
			iss >> keyword;
			iss >> name;

			material.Id = GetMaterialID(name);

			break;
		}
		case 'K':
		{
			switch (line[1])
			{
			case 'a':
			{
				float x, y, z;
				sscanf_s(line.c_str(), "Ka %f %f %f", &x, &y, &z);
				material.Ambient = { x,y,z };
				break;
			}
			case 'd':
			{
				float x, y, z;
				sscanf_s(line.c_str(), "Kd %f %f %f", &x, &y, &z);
				material.Diffuse = { x,y,z };
				break;
			}
			case 's':
			{
				float x, y, z;
				sscanf_s(line.c_str(), "Ks %f %f %f", &x, &y, &z);
				material.Specular = { x,y,z };
				break;
			}
			case 'e':
			{
				float x, y, z;
				sscanf_s(line.c_str(), "Ke %f %f %f", &x, &y, &z);
				material.Emissive = { x,y,z };
				break;
			}
			}
			break;
		}
		case 'N':
		{
			switch (line[1])
			{
			case 's':
				sscanf_s(line.c_str(), "Ns %f", &material.SpecularExponent);
				break;
			}
			break;
		}
		case 'm':
		{
			std::istringstream iss(line);
			iss >> keyword;
			if (keyword == "map_Kd")
			{
				std::string name;
				iss >> name;
				material.DiffuseMap = "Assets/" + name;
				LoadTexture(material.DiffuseMap.c_str());
			}
			break;
		}
		}
	}

	if ((material.Id != -1))
	{
		m_mats.push_back(material);

		material.Id = -1;
		material.Illumination = 1;
		material.Ambient = { 1.0f, 1.0f, 1.0f };
		material.Diffuse = { 1.0f, 1.0f, 1.0f };
		material.Specular = { 1.0f, 1.0f, 1.0f };
		material.Emissive = { 1.0f, 1.0f, 1.0f };
		material.SpecularExponent = 0.0f;
	}
}

int AssetManager::GetMaterialID(std::string name)
{
	std::map<std::string, int>::iterator it;
	it = m_mat_ids.find(name);
	if (it != m_mat_ids.end())
		return m_mat_ids[name];
	m_mat_ids.insert(std::make_pair(name, m_next_id));
	m_next_id++;
	return m_mat_ids[name];
}

void AssetManager::LoadTexture(std::string texturePath)
{
	std::map<std::string, ID3D11ShaderResourceView*>::iterator it;
	it = m_textures.find(texturePath);
	if (it != m_textures.end())
		return;

	std::wstring widestr = std::wstring(texturePath.begin(), texturePath.end());
	ID3D11ShaderResourceView* texture = NULL;
	HRESULT result = DirectX::CreateWICTextureFromFile(
		Renderer::Instance->GetDevice(),
		Renderer::Instance->GetDeviceContext(),
		widestr.c_str(), NULL, &texture);
	if (FAILED(result))
	{
		OutputDebugString(L"Failed to create texture.");
		return;
	}

	m_textures.insert(std::make_pair(texturePath, texture));
}
