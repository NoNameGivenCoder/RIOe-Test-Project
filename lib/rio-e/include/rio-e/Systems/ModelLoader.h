#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "rio.h"
#include "math/rio_Vector.h"
#include "rio-e/Types/Mesh.h"
#include "rio-e/Types/Model.h"
#include "rio-e/Types/Skeleton.h"

namespace tinygltf { class Image; class Model; class Primitive; }

namespace rioe 
{
	class Engine;

	class ModelLoader
	{
	public:
		static Model* LoadModel(const std::string& filename, const char* rootNodeName);
		static Model* GetModel(std::string filename) { return mModelCache[filename]; };

		static Skeleton* LoadSkeleton(const std::string& filename, const char* rootNodeName);
		static void Cleanup();
		static bool CustomLoadImageData(tinygltf::Image* image, const int image_idx, std::string* err, std::string* warn, int req_width, int req_height, const unsigned char* bytes, int size, void* user_data);

	private:
		static bool LoadGLB(std::string filename, tinygltf::Model* model, std::string* warn, std::string* err);
		static Material* CreateMaterial(const tinygltf::Model& model, const tinygltf::Primitive& primitive);

		static std::unordered_map<std::string, Model*> mModelCache;
		static std::unordered_map<std::string, Skeleton*> mSkeletonCache;
	};
}