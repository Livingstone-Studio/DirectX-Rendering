#pragma once

class Mesh;

class AssetManager
{
public:
	static Mesh LoadObj(const wchar_t* filePath);
};

