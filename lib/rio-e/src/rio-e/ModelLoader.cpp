#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE
#include "tiny_gltf.h"

#include "rio-e/ModelLoader.h"
#include "rio-e/Types/Skeleton.h"
#include "gfx/mdl/res/rio_MeshData.h"

#include "filedevice/rio_FileDeviceMgr.h"
#include "gpu/rio_Drawer.h"

#include "rio-e/SceneMgr.h"
#include "rio-e/Endian.h"

#include <functional>

namespace rioe
{
	std::unordered_map<std::string, Model*> ModelLoader::mModelCache;
    std::unordered_map<std::string, Skeleton*> ModelLoader::mSkeletonCache;

	bool ModelLoader::CustomLoadImageData(tinygltf::Image* image, const int image_idx, std::string* err, std::string* warn, int req_width, int req_height, const unsigned char* bytes, int size, void* user_data)
	{
		return true;
	}

	void ModelLoader::Cleanup()
	{
		size_t modelsSize = mModelCache.size();
        size_t skeletonsSize = mSkeletonCache.size();

		for (auto& model : mModelCache)
		{
			for (Mesh* mesh : model.second->mMeshes)
				delete mesh;

			size_t meshesSize = model.second->mMeshes.size();

			delete model.second;

			RIO_LOG("[ModelLoader] Cleaned %d meshes from %s.\n", meshesSize, model.first.c_str());
		}

        for (auto& skeleton : mSkeletonCache)
        {
            for (SkeletalMesh* mesh : skeleton.second->mMeshes)
                delete mesh;

            size_t meshesSize = skeleton.second->mMeshes.size();

            delete skeleton.second;

            RIO_LOG("[ModelLoader] Cleaned %d skeletal meshes from %s.\n", meshesSize, skeleton.first.c_str());
        }

        mSkeletonCache.clear();
		mModelCache.clear();
		
		RIO_LOG("[ModelLoader] %d Model(s) cleaned.\n", modelsSize);
        RIO_LOG("[ModelLoader] %d Skeleton(s) cleaned.\n", skeletonsSize);
	}

    template <typename T>
    const T* GetAccessor(tinygltf::Model& model, const tinygltf::Primitive& primitive, int index)
    {
        const tinygltf::Accessor& accessor = model.accessors[index];
        const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
        const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

        const unsigned char* bufferData = &buffer.data[bufferView.byteOffset + accessor.byteOffset];

        return reinterpret_cast<const T*>(bufferData);
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

        bool bigEndian = rioe::Endian::BigEndian();

        // Iterate through meshes in the model
        for (const auto& tinyMesh : model.meshes)
        {
            for (const auto& primitive : tinyMesh.primitives)
            {
                std::vector<rio::mdl::res::Vertex> vertices;
                std::vector<unsigned int> indicesData;

                const tinygltf::Accessor& positionAccessor = model.accessors[primitive.attributes.find("POSITION")->second];
                const tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];

                const float* positions = GetAccessor<float>(model, primitive, primitive.attributes.find("POSITION")->second);
                const float* normals = GetAccessor<float>(model, primitive, primitive.attributes.find("NORMAL")->second);
                const float* texCoords = GetAccessor<float>(model, primitive, primitive.attributes.find("TEXCOORD_0")->second);
                const unsigned short* indices = GetAccessor<unsigned short>(model, primitive, primitive.indices);

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
                        x = rioe::Endian::SwapEndianness(x);
                        y = rioe::Endian::SwapEndianness(y);
                        z = rioe::Endian::SwapEndianness(z);

                        nX = rioe::Endian::SwapEndianness(nX);
                        nY = rioe::Endian::SwapEndianness(nY);
                        nZ = rioe::Endian::SwapEndianness(nZ);

                        uvX = rioe::Endian::SwapEndianness(uvX);
                        uvY = rioe::Endian::SwapEndianness(uvY);
                    }

                    vertex.normal = { nX, nY, nZ };
                    vertex.pos = { x, y, z };
                    vertex.tex_coord = { uvX, uvY };

                    vertices.emplace_back(vertex);
                }

                for (size_t i = 0; i < indexAccessor.count; i++)
                {
                    unsigned short index = indices[i];

                    if (bigEndian)
                        index = rioe::Endian::SwapEndianness(index);

                    indicesData.push_back(index);
                }

                auto mesh = new Mesh(vertices, indicesData);

                if (primitive.material >= 0 && primitive.material < model.materials.size())
                {
                    auto& material = model.materials[primitive.material];
                    auto& texture = model.textures[material.pbrMetallicRoughness.baseColorTexture.index];
                    auto& image = model.images[texture.source];

                    mesh->mMaterial = new Material();
                    mesh->mMaterial->mTexture = new rio::Texture2D(image.name.c_str());
                    mesh->mMaterial->mTextureSampler.linkTexture2D(mesh->mMaterial->mTexture);

                    if (material.doubleSided)
                    {
                        mesh->mMaterial->mRenderState.setCullingMode(rio::Graphics::CULLING_MODE_NONE);
                    }
                }

                newModel->mMeshes.emplace_back(mesh);
            }
        }

        RIO_LOG("[ModelLoader] Loaded %s.\n", filename.c_str());

        mModelCache.emplace(filename, newModel);  // Cache the model
        return newModel;
    }

    Skeleton* ModelLoader::LoadSkeleton(std::string base_filename)
    {
        auto newSkeleton = new Skeleton();

        // Loading GLTF file
        tinygltf::TinyGLTF loader;
        tinygltf::Model model;
        std::string warn, err;

        rio::FileDevice::LoadArg load;
        load.path = "models/" + base_filename;

        u8* fileBuffer = rio::FileDeviceMgr::instance()->tryLoad(load);

        loader.SetImageLoader(CustomLoadImageData, nullptr);
        bool ret = loader.LoadBinaryFromMemory(&model, &err, &warn, fileBuffer, load.read_size);

        rio::MemUtil::free(fileBuffer);

        if (!ret)
        {
            RIO_LOG("Error! %s %s\n", warn.c_str(), err.c_str());
            return nullptr;  // Return early on failure
        }

        bool bigEndian = rioe::Endian::BigEndian();

        for (const auto& mesh : model.meshes)
        {
            for (const auto& primitive : mesh.primitives)
            {
                std::vector<rioe::SkeletalMesh::Vertex> vertices;
                std::vector<unsigned int> indicesData;

                const tinygltf::Accessor& positionAccessor = model.accessors[primitive.attributes.find("POSITION")->second];
                const tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];

                const float* positions = GetAccessor<float>(model, primitive, primitive.attributes.find("POSITION")->second);
                const float* normals = GetAccessor<float>(model, primitive, primitive.attributes.find("NORMAL")->second);
                const float* texCoords = GetAccessor<float>(model, primitive, primitive.attributes.find("TEXCOORD_0")->second);
                const unsigned short* indices = GetAccessor<unsigned short>(model, primitive, primitive.indices);

                const float* weights = GetAccessor<float>(model, primitive, primitive.attributes.find("WEIGHTS_0")->second);
                const u8* joints = GetAccessor<u8>(model, primitive, primitive.attributes.find("JOINTS_0")->second);

                for (size_t i = 0; i < positionAccessor.count; ++i)
                {
                    rioe::SkeletalMesh::Vertex vertex;

                    float x = positions[i * 3 + 0];
                    float y = positions[i * 3 + 1];
                    float z = positions[i * 3 + 2];

                    float nX = normals[i * 3 + 0];
                    float nY = normals[i * 3 + 1];
                    float nZ = normals[i * 3 + 2];

                    float uvX = texCoords[i * 2 + 0];
                    float uvY = texCoords[i * 2 + 1];
                    
                    float w1 = weights[i * 4 + 0];
                    float w2 = weights[i * 4 + 1];
                    float w3 = weights[i * 4 + 2];
                    float w4 = weights[i * 4 + 3];

                    u8 j1 = joints[i * 4 + 0];
                    u8 j2 = joints[i * 4 + 1];
                    u8 j3 = joints[i * 4 + 2];
                    u8 j4 = joints[i * 4 + 3];

                    if (bigEndian)
                    {
                        x = rioe::Endian::SwapEndianness(x);
                        y = rioe::Endian::SwapEndianness(y);
                        z = rioe::Endian::SwapEndianness(z);

                        nX = rioe::Endian::SwapEndianness(nX);
                        nY = rioe::Endian::SwapEndianness(nY);
                        nZ = rioe::Endian::SwapEndianness(nZ);

                        uvX = rioe::Endian::SwapEndianness(uvX);
                        uvY = rioe::Endian::SwapEndianness(uvY);

                        w1 = rioe::Endian::SwapEndianness(w1);
                        w2 = rioe::Endian::SwapEndianness(w2);
                        w3 = rioe::Endian::SwapEndianness(w3);
                        w4 = rioe::Endian::SwapEndianness(w4);
                    }

                    vertex.normal = { nX, nY, nZ };
                    vertex.pos = { x, y, z };
                    vertex.tex_coord = { uvX, uvY };
                    vertex.weights = { w1, w2, w3, w4 };
                    vertex.joints = { j1, j2, j3, j4 };

                    vertices.emplace_back(vertex);
                }

                for (size_t i = 0; i < indexAccessor.count; i++)
                {
                    unsigned short index = indices[i];

                    if (bigEndian)
                        index = rioe::Endian::SwapEndianness(index);

                    indicesData.push_back(index);
                }

                auto newMesh = new SkeletalMesh(vertices, indicesData);

                if (primitive.material >= 0 && primitive.material < model.materials.size())
                {
                    auto& material = model.materials[primitive.material];
                    auto& texture = model.textures[material.pbrMetallicRoughness.baseColorTexture.index];
                    auto& image = model.images[texture.source];

                    newMesh->mMaterial = new Material();
                    newMesh->mMaterial->mTexture = new rio::Texture2D(image.name.c_str());
                    newMesh->mMaterial->mTextureSampler.linkTexture2D(newMesh->mMaterial->mTexture);

                    if (material.doubleSided)
                    {
                        newMesh->mMaterial->mRenderState.setCullingMode(rio::Graphics::CULLING_MODE_NONE);
                    }
                }

                newSkeleton->mMeshes.emplace_back(newMesh);
            }
        }

        for (const auto& skin : model.skins)
        {
            newSkeleton->mBones.resize(skin.joints.size());

            // Creating node data
            for (size_t i = 0; i < skin.joints.size(); ++i)
            {
                const auto& gltfNode = model.nodes[skin.joints[i]];
                auto newNode = rioe::SceneMgr::instance()->GetCurrentScene()->CreateNode();

                newNode->name = gltfNode.name;
                newNode->AddProperty(rioe::PropertyCreatorMgr::instance()->CreateProperty("DebugProperty"));

                //RIO_LOG("%f %f %f\n", static_cast<f32>(gltfNode.translation[0]), static_cast<f32>(gltfNode.translation[1]), static_cast<f32>(gltfNode.translation[2]));

                if (!gltfNode.translation.empty())
                    newNode->SetPosition({ static_cast<f32>(gltfNode.translation[0]), static_cast<f32>(gltfNode.translation[1]), static_cast<f32>(gltfNode.translation[2]) });

                //TODO: rotation

                if (!gltfNode.scale.empty())
                    newNode->SetScale({ static_cast<f32>(gltfNode.scale[0]) * 3, static_cast<f32>(gltfNode.scale[1]) * 3, static_cast<f32>(gltfNode.scale[2]) * 3 });

                if (gltfNode.scale.empty())
                    newNode->SetScale({ 1, 1, 1 });

                newSkeleton->mBones[i] = newNode;
            }

            // Parenting nodes together
            for (size_t i = 0; i < skin.joints.size(); ++i)
            {
                int jointNodeIndex = skin.joints[i];
                const auto& gltfNode = model.nodes[jointNodeIndex];

                for (const int childIndex : gltfNode.children)
                {
                    auto it = std::find(skin.joints.begin(), skin.joints.end(), childIndex);
                    if (it != skin.joints.end())
                    {
                        int childJointIndex = std::distance(skin.joints.begin(), it);
                        newSkeleton->mBones[i]->AddChild(newSkeleton->mBones[childJointIndex]);
                    }
                }
            }
        }

        mSkeletonCache.emplace(base_filename, newSkeleton);

        RIO_LOG("[ModelLoader] Loaded %s.\n", base_filename.c_str());

        for (const auto& bone : newSkeleton->mBones)
        {
            RIO_LOG("Bone: %s\n", bone->name.c_str());
        }

        return newSkeleton;
    }
}