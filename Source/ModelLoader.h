#pragma once
#include <vector>

#include "Mesh.h"
#include <openFBX/ofbx.h>

// frontend class that has multiple functions depending on the implementation
// so this will always give the same result but the way to do it *has* to be different
// so we can change library without the worst headache, just a migraine.

class ModelLoader
{
public:
	struct ModelLoaderReturnData
	{
		int VERTEX_COUNT;
		int INDEX_COUNT;
		std::vector<Mesh::Vertex> myVertices{};
		std::vector<unsigned int> myIndices{};

	};
	static bool GetModel(const char* filepath, ModelLoaderReturnData& returnData);

	// implementations go here
private:
	// returns false if failed
	static bool OpenFBX(const char* filepath, ModelLoaderReturnData& returnData);
};

inline bool ModelLoader::GetModel(const char* filepath, ModelLoaderReturnData& returnData)
{
	return OpenFBX(filepath, returnData);
}

inline bool ModelLoader::OpenFBX(const char* filepath, ModelLoaderReturnData& returnData)
{
	static char s_TimeString[256];
	FILE* fp = fopen(filepath, "rb");

	if (!fp)
	{
		return false;
	}

	fseek(fp, 0, SEEK_END);
	long file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	auto* content = new ofbx::u8[file_size];
	fread(content, 1, file_size, fp);

	// Start Timer
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);
	LARGE_INTEGER start;
	QueryPerformanceCounter(&start);

	// Ignoring certain nodes will only stop them from being processed not tokenised (i.e. they will still be in the tree)
	ofbx::LoadFlags flags =
		//		ofbx::LoadFlags::IGNORE_MODELS |
		ofbx::LoadFlags::IGNORE_BLEND_SHAPES |
		ofbx::LoadFlags::IGNORE_CAMERAS |
		ofbx::LoadFlags::IGNORE_LIGHTS |
		//		ofbx::LoadFlags::IGNORE_TEXTURES |
		ofbx::LoadFlags::IGNORE_SKIN |
		ofbx::LoadFlags::IGNORE_BONES |
		ofbx::LoadFlags::IGNORE_PIVOTS |
		//		ofbx::LoadFlags::IGNORE_MATERIALS |
		ofbx::LoadFlags::IGNORE_POSES |
		ofbx::LoadFlags::IGNORE_VIDEOS |
		ofbx::LoadFlags::IGNORE_LIMBS |
		//		ofbx::LoadFlags::IGNORE_MESHES |
		ofbx::LoadFlags::IGNORE_ANIMATIONS;

	ofbx::IScene* g_scene = ofbx::load((ofbx::u8*)content, file_size, (ofbx::u16)flags);

	// Stop Timer
	LARGE_INTEGER stop;
	QueryPerformanceCounter(&stop);
	double elapsed = (double)(stop.QuadPart - start.QuadPart) / (double)frequency.QuadPart;
	snprintf(s_TimeString,
		sizeof(s_TimeString),
		"Loading took %f seconds (%.0f ms) to load %s file of size %d bytes (%f MB) \r ",
		elapsed,
		elapsed * 1000.0,
		filepath,
		file_size,
		(double)file_size / (1024.0 * 1024.0));


	if (!g_scene) {
		OutputDebugStringA(ofbx::getError());
	}
	else
	{
		auto geometryCount = g_scene->getGeometryCount();
		if (geometryCount != 1) // more than one geo thing in the level? multiple meshes? dunno lol try loading it as one fuck it
		{
			
		}
		if (geometryCount == 0)
		{
			
		}
		// geocount = 1, load normally
		auto object = g_scene->getGeometry(0);
		auto positions = object->getGeometryData().getPositions();
		//returnData.VERTEX_COUNT = 

		// manage your FBX stuff here
		//saveAsOBJ(*g_scene, "out.obj");
	}
	delete[] content;
	fclose(fp);

	return true;
}