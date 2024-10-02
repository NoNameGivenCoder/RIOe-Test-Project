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
        uint32_t intRep;
        // Use memcpy to avoid strict aliasing issues
        rio::MemUtil::copy(&intRep, &value, sizeof(float));
        intRep = swapEndianness32(intRep);
        float swapped;
        rio::MemUtil::copy(&swapped, &intRep, sizeof(float));
        return swapped;
    }

    unsigned short swapShort(unsigned short value) {
        return (value >> 8) | (value << 8);
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
			//for (Mesh* mesh : model.second->mMeshes)
			//{
			//	delete mesh;
			//}

			size_t meshesSize = model.second->mMeshes.size();

			//delete model.second;

			RIO_LOG("[ModelLoader] Cleaned %d meshes from %s.\n", meshesSize, model.first.c_str());
		}

		mModelCache.clear();
		
		RIO_LOG("[ModelLoader] %d Model(s) cleaned.\n", modelsSize);
	}

    Model* ModelLoader::LoadModel(std::string filename)
    {
        auto newModel = new Model();

        // Loading GLTF file
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
            return nullptr;  // Return early on failure
        }

        bool bigEndian = isBigEndian();

        RIO_LOG("Meshes: %d\n", model.meshes.size());

        // Iterate through meshes in the model
        for (const auto& tinyMesh : model.meshes)
        {
            RIO_LOG("Primitives: %d\n", tinyMesh.primitives.size());

            for (const auto& primitive : tinyMesh.primitives)
            {
                RIO_LOG("Does this model have indices? %d\n", primitive.indices);

                std::vector<rio::mdl::res::Vertex> vertices;
                std::vector<unsigned int> indicesData;

                const tinygltf::Accessor& positionAccessor = model.accessors[primitive.attributes.find("POSITION")->second];
                const tinygltf::BufferView& positionBufferView = model.bufferViews[positionAccessor.bufferView];
                const tinygltf::Buffer& positionBuffer = model.buffers[positionBufferView.buffer];

                const tinygltf::Accessor& normalAccessor = model.accessors[primitive.attributes.find("NORMAL")->second];
                const tinygltf::BufferView& normalBufferView = model.bufferViews[normalAccessor.bufferView];
                const tinygltf::Buffer& normalBuffer = model.buffers[normalBufferView.buffer];

                const tinygltf::Accessor& texCoordAccessor = model.accessors[primitive.attributes.find("TEXCOORD_0")->second];
                const tinygltf::BufferView& texCoordBufferView = model.bufferViews[texCoordAccessor.bufferView];
                const tinygltf::Buffer& texCoordBuffer = model.buffers[texCoordBufferView.buffer];

                const tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];
                const tinygltf::BufferView& indexBufferView = model.bufferViews[indexAccessor.bufferView];
                const tinygltf::Buffer& indexBuffer = model.buffers[indexBufferView.buffer];

                const float* positions = reinterpret_cast<const float*>(&positionBuffer.data[positionBufferView.byteOffset + positionAccessor.byteOffset]);
                const float* normals = reinterpret_cast<const float*>(&normalBuffer.data[normalBufferView.byteOffset + normalAccessor.byteOffset]);
                const float* texCoords = reinterpret_cast<const float*>(&texCoordBuffer.data[texCoordBufferView.byteOffset + texCoordAccessor.byteOffset]);
                const unsigned short* indices = reinterpret_cast<const unsigned short*>(&indexBuffer.data[indexBufferView.byteOffset + indexAccessor.byteOffset]);

                for (size_t i = 0; i < positionAccessor.count; i++)
                {
                    rio::mdl::res::Vertex vertex;

                    float x = positions[i * 3 + 0];
                    float y = positions[i * 3 + 1];
                    float z = positions[i * 3 + 2];

                    float nX = normals[i * 3 + 0];
                    float nY = normals[i * 3 + 1];
                    float nZ = normals[i * 3 + 2];

                    float uvX = texCoords[i * 2 + 0];
                    float uvY = texCoords[i * 2 + 1];

                    if (bigEndian)
                    {
                        x = swapFloat(x);
                        y = swapFloat(y);
                        z = swapFloat(z);

                        nX = swapFloat(nX);
                        nY = swapFloat(nY);
                        nZ = swapFloat(nZ);

                        uvX = swapFloat(uvX);
                        uvY = swapFloat(uvY);
                    }

                    vertex.normal = { nX, nY, nZ };
                    vertex.pos = { x, y, z };
                    vertex.tex_coord = { uvX, uvY };

                    vertices.emplace_back(vertex);
                }

                for (size_t i = 0; i < indexAccessor.count; i++)
                {
                    unsigned int index = indices[i];

                    if (bigEndian)
                        index = swapShort(index);

                    indicesData.emplace_back(index);
                }

                auto mesh = new Mesh(vertices, indicesData);

                if (primitive.material >= 0 && primitive.material < model.materials.size())
                {
                    auto& material = model.materials[primitive.material];
                    auto& texture = model.textures[material.pbrMetallicRoughness.baseColorTexture.index];
                    auto& image = model.images[texture.source];

                    mesh->mMaterial = new Material();
                    mesh->mMaterial->mTexture = std::make_shared<rio::Texture2D>(image.name.c_str());
                    mesh->mMaterial->mTextureSampler = std::make_shared<rio::TextureSampler2D>();
                    mesh->mMaterial->mShader = std::make_shared<rio::Shader>();

                    mesh->mMaterial->mTextureSampler->linkTexture2D(mesh->mMaterial->mTexture.get());

                    if (material.doubleSided)
                    {
                        mesh->mMaterial->mRenderState.setCullingMode(rio::Graphics::CULLING_MODE_NONE);
                    }
                }

                newModel->mMeshes.emplace_back(mesh);
            }
        }

        //mModelCache.emplace(filename, std::move(newModel));  // Cache the model
        return newModel;
    }
}