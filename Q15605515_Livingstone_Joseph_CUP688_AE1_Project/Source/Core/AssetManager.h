#pragma once
#include <d3d11.h>
#include <WICTextureLoader.h>

#include <string>
#include <vector>
#include <map>

struct Mesh;
struct Material;

class AssetManager
{
public:
	static void Cleanup();

	static Mesh LoadObj(const wchar_t* filePath);	

	static void LoadMaterialFile(std::string file);
	static int GetMaterialID(std::string name);

	static void LoadTexture(std::string texturePath);

	static std::vector<Material> GetMaterials() { return m_mats; }
	static ID3D11ShaderResourceView* GetTexture(std::string name) { return m_textures[name]; }

private:
	static std::vector<Material> m_mats;
	static std::map<std::string, int> m_mat_ids;
	static std::map<std::string, ID3D11ShaderResourceView*> m_textures;
	static int m_next_id;
};

