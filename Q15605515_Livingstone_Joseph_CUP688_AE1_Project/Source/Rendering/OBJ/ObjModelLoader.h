#pragma once

#include "ModelDataStructs.h"

#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <unordered_map>
#include <string>
#include <fstream>

class Model;

class ObjModelLoader
{
public:
	// Loads model for game object
	static HRESULT LoadModel(std::string relative_path, std::string texture_path, Model** model);

	static const Material& GetMaterial(std::string name);
	static ID3D11ShaderResourceView* GetTexture(std::string name);

private:
	static HRESULT LoadModelData(std::string relative_path, std::string texture_path, ModelData* mesh_data);

	// Attempts to load the models geometry
	static HRESULT LoadModelGeometryData(std::string line, XMFLOAT3* data);
	static HRESULT LoadModelGeometryData(std::string line, XMFLOAT2* data);
	static HRESULT LoadModelTriangleData(std::string line, XMINT3* one, XMINT3* two, XMINT3* three);

	// Attempts to loads model material data
	static HRESULT LoadModelMaterial(std::string relative_path, MaterialData* material_data);
	static HRESULT LoadMaterialTexture(std::string relative_path, Material* material);

	static HRESULT LoadTexture(std::string relative_path, std::string* data_store);
	

private:
	static std::unordered_map<std::string, Model*> m_model_atlas;
	static std::unordered_map<std::string, Material> m_material_atlas;
	static std::unordered_map<std::string, ID3D11ShaderResourceView*> m_texture_atlas;

public:
	// Application end cleanup
	static void Cleanup();

};