#include "AssetManager.h"

#include "../Rendering/Mesh.h"
#include <fstream>
#include <string>
#include <sstream>

struct Face
{
	Face(int vertexOne, int vertexTwo, int vertexThree, int vertexFour,
		int uvOne, int uvTwo, int uvThree, int uvFour,
		int normalOne, int normalTwo, int normalThree, int normalFour)
		: VertexIndex{ vertexOne, vertexTwo, vertexThree, vertexFour },
		  UVIndex{ uvOne, uvTwo, uvThree, uvFour },
		  NormalIndex{ normalOne, normalTwo, normalThree, normalFour }
	{}

	int VertexIndex[4] = { 0,0,0, 0 };
	int UVIndex[4] = { 0, 0, 0, 0 };
	int NormalIndex[4] = { 0, 0, 0, 0 };
};

Mesh AssetManager::LoadObj(const wchar_t* filePath)
{
	// Mesh Initialisation
	Mesh mesh;

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
		case 'f':
			int v0 = 0, v1 = 0, v2 = 0, v3 = 0, t0 = 0, t1 = 0, t2 = 0, t3 = 0, n0 = 0, n1 = 0, n2 = 0, n3 = 0;
			sscanf_s(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", &v0, &t0, &n0, &v1, &t1, &n1, &v2, &t2, &n2, &v3, &t3, &n3);
			Face face(v0 - 1, v1 - 1, v2 - 1, v3 - 1, t0 - 1, t1 - 1, t2 - 1, t3 - 1, n0 - 1, n1 - 1, n2 - 1, n3 - 1);
			faces.push_back(face);
			break;
		}
	}

	// Mesh Building
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
			mesh.Vertices.push_back({ position, {1,1,1,1}, uv, normal });
			mesh.Indices.push_back(mesh.Vertices.size() - 1);
		}
	}

	int i = 0;

	// Return Built Mesh
	return mesh;
}
