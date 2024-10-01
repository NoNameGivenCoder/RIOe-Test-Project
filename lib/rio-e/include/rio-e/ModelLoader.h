#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "rio.h"
#include "math/rio_Vector.h"
#include "rio-e/Types/Mesh.h"
#include "rio-e/Types/Model.h"

namespace tinygltf { class Image; }

namespace rioe 
{
	class ModelLoader
	{
	public:
		static Model* LoadModel(std::string base_filename);
		static Model* GetModel(std::string filename) { return mModelCache[filename]; };
		static void Cleanup();
		static bool CustomLoadImageData(tinygltf::Image* image, const int image_idx, std::string* err, std::string* warn, int req_width, int req_height, const unsigned char* bytes, int size, void* user_data);

	private:
		static std::unordered_map<std::string, Model*> mModelCache;
		static std::vector<std::string> mTextureNameCache;
	};
	Mesh* GLTFRead(std::string path);
}