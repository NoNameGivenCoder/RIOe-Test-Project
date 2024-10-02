#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE
#include "tiny_gltf.h"

#include "rio-e/ModelLoader.h"
#include "gfx/mdl/res/rio_MeshData.h"

#include "filedevice/rio_FileDeviceMgr.h"

namespace rioe
{
	std::unordered_map<std::string, Model*> ModelLoader::mModelCache;

	bool isBigEndian() {
		int num = 1;
		return *(char*)&num == 0;
	}

	uint32_t swapEndianness32(uint32_t val) {
		return ((val >> 24) & 0xff) |
			((val << 8) & 0xff0000) |
			((val >> 8) & 0xff00) |
			((val << 24) & 0xff000000);
	}

	float swapFloat(float value) {
		uint32_t intRep = *reinterpret_cast<uint32_t*>(&value);
		intRep = swapEndianness32(intRep);
		return *reinterpret_cast<float*>(&intRep);
	}

	unsigned short swapShort(unsigned short value) {
		uint8_t* ptr = reinterpret_cast<uint8_t*>(&value);
		std::reverse(ptr, ptr + sizeof(unsigned short));
		return value;
	}

	bool ModelLoader::CustomLoadImageData(tinygltf::Image* image, const int image_idx, std::string* err, std::string* warn, int req_width, int req_height, const unsigned char* bytes, int size, void* user_data)
	{
		return true;
	}

	void ModelLoader::Cleanup()
	{
		size_t modelsSize = mModelCache.size();
		for (auto& model : mModelCache)
		{
			for (Mesh* mesh : model.second->mMeshes)
			{
				delete mesh;
			}

			size_t meshesSize = model.second->mMeshes.size();

			delete model.second;

			RIO_LOG("[ModelLoader] Cleaned %d meshes from %s.\n", meshesSize, model.first.c_str());
		}

		mModelCache.clear();
		
		RIO_LOG("[ModelLoader] %d Model(s) cleaned.\n", modelsSize);
	}

	Model* ModelLoader::LoadModel(std::string filename)
	{
		auto newModel = new Model();

		tinygltf::TinyGLTF loader;
		tinygltf::Model model;
		std::string warn, err;

		rio::FileDevice::LoadArg load;
		load.path = "models/" + filename;

		u8* fileBuffer = rio::FileDeviceMgr::instance()->tryLoad(load);

		loader.SetImageLoader(CustomLoadImageData, nullptr);
		bool ret = loader.LoadBinaryFromMemory(&model, &err, &warn, fileBuffer, load.read_size);

		rio::MemUtil::free(fileBuffer);

		if (!ret)
		{
			RIO_LOG("Error! %s %s\n", warn.c_str(), err.c_str());
		}

		bool bigEndian = isBigEndian();

		for (const auto& tinyMesh : model.meshes)
		{
			for (const auto& primitive : tinyMesh.primitives)
			{
				std::vector<unsigned int> indexData;
				std::vector<rio::mdl::res::Vertex> vertices;
				// Vertex and all
				{
					// Access vertex positions (required)
					const auto& pos_accessor = model.accessors[primitive.attributes.find("POSITION")->second];
					const auto& pos_bufferView = model.bufferViews[pos_accessor.bufferView];
					const auto& pos_buffer = model.buffers[pos_bufferView.buffer];
					const float* positions = reinterpret_cast<const float*>(&pos_buffer.data[pos_bufferView.byteOffset + pos_accessor.byteOffset]);

					// Access normals (optional)
					const auto& normal_accessor = model.accessors[primitive.attributes.find("NORMAL")->second];
					const auto& normal_bufferView = model.bufferViews[normal_accessor.bufferView];
					const auto& normal_buffer = model.buffers[normal_bufferView.buffer];
					const float* normals = normal_accessor.count > 0
						? reinterpret_cast<const float*>(&normal_buffer.data[normal_bufferView.byteOffset + normal_accessor.byteOffset])
						: nullptr;

					// Access texture coordinates (optional)
					const auto& texcoord_accessor = model.accessors[primitive.attributes.find("TEXCOORD_0")->second];
					const auto& texcoord_bufferView = model.bufferViews[texcoord_accessor.bufferView];
					const auto& texcoord_buffer = model.buffers[texcoord_bufferView.buffer];
					const float* tex_coords = texcoord_accessor.count > 0
						? reinterpret_cast<const float*>(&texcoord_buffer.data[texcoord_bufferView.byteOffset + texcoord_accessor.byteOffset])
						: nullptr;

					const auto& index_accessor = model.accessors[primitive.indices];
					const auto& index_bufferView = model.bufferViews[index_accessor.bufferView];
					const auto& index_buffer = model.buffers[index_bufferView.buffer];
					const unsigned short* indices = reinterpret_cast<const unsigned short*>(&index_buffer.data[index_bufferView.byteOffset + index_accessor.byteOffset]);

					size_t baseVertexIndex = vertices.size();
					for (size_t i = 0; i < pos_accessor.count; ++i)
					{
						rio::mdl::res::Vertex vertex;

						float x = positions[i * 3];
						float y = positions[i * 3 + 1];
						float z = positions[i * 3 + 2];

						if (bigEndian)
						{
							x = swapFloat(x);
							y = swapFloat(y);
							z = swapFloat(z);
						}
						vertex.pos = { x, y, z };

						if (normals)
						{
							float nx = normals[i * 3];
							float ny = normals[i * 3 + 1];
							float nz = normals[i * 3 + 2];
							if (bigEndian) {
								nx = swapFloat(nx);
								ny = swapFloat(ny);
								nz = swapFloat(nz);
							}
							vertex.normal = { nx, ny, nz };
						}

						if (tex_coords)
						{
							float u = tex_coords[i * 2];
							float v = tex_coords[i * 2 + 1];
							if (bigEndian) {
								u = swapFloat(u);
								v = swapFloat(v);
							}
							vertex.tex_coord = { u, v };
						}

						RIO_LOG("Vertex Position: (%f, %f, %f)\n", vertex.pos.x, vertex.pos.y, vertex.pos.z);

						vertices.push_back(vertex);
					}
					for (size_t i = 0; i < index_accessor.count; ++i)
					{
						unsigned short index = indices[i];
						if (bigEndian) {
							index = swapShort(index);
						}

						RIO_LOG("Index: %d\n", index);
						indexData.push_back(index + baseVertexIndex);
					}
				}

				auto mesh = new Mesh(vertices, indexData);

				if (primitive.material >= 0 && primitive.material < model.materials.size())
				{
					auto& material = model.materials[primitive.material];
					auto& texture = model.textures[material.pbrMetallicRoughness.baseColorTexture.index];
					auto& image = model.images[texture.source];

					mesh->mMaterial = std::make_unique<Material>();
					mesh->mMaterial->mTexture = std::make_shared<rio::Texture2D>(image.name.c_str());
					mesh->mMaterial->mTextureSampler = std::make_shared<rio::TextureSampler2D>();
					mesh->mMaterial->mShader = std::make_shared<rio::Shader>();

					mesh->mMaterial->mTextureSampler->linkTexture2D(mesh->mMaterial->mTexture.get());

					if (material.doubleSided)
						mesh->mMaterial->mRenderState.setCullingMode(rio::Graphics::CULLING_MODE_NONE);
				}

				//tinyMesh.

				newModel->mMeshes.emplace_back(mesh);
			}
		}

		mModelCache.emplace(filename, newModel);
		return newModel;
	}
}