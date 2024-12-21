#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE
#include "tiny_gltf.h"

#include "rio-e/Systems/ModelLoader.h"
#include "rio-e/Types/Skeleton.h"
#include "gfx/mdl/res/rio_MeshData.h"
#include "gpu/rio_RenderState.h"

#include "filedevice/rio_FileDeviceMgr.h"

#include "math/rio_Quat.h"

#include "rio-e/Common/Endian.h"

#include "rio-e/Properties/DebugDisplayProperty.hpp"
#include "rio-e/Properties/MeshDisplayProperty.hpp"

#include "rio-e/Engine.hpp"

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
    const T* GetAccessor(tinygltf::Model& model, int index)
    {
        const tinygltf::Accessor& accessor = model.accessors[index];
        const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
        const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

        const unsigned char* bufferData = &buffer.data[bufferView.byteOffset + accessor.byteOffset];

        return reinterpret_cast<const T*>(bufferData);
    }

    bool ModelLoader::LoadGLB(std::string filename, tinygltf::Model* model, std::string* warn, std::string* err)
    {
        tinygltf::TinyGLTF loader;
        rio::FileDevice::LoadArg load;
        load.path = "models/" + filename;

        u8* fileBuffer = rio::FileDeviceMgr::instance()->tryLoad(load);

        loader.SetImageLoader(CustomLoadImageData, nullptr);
        bool ret = loader.LoadBinaryFromMemory(model, err, warn, fileBuffer, load.read_size);

        rio::MemUtil::free(fileBuffer);

        if (!ret)
        {
            RIO_LOG("Error! %s %s\n", warn->c_str(), err->c_str());
            return false;
        }
            
        return true;
    }

    std::pair<rio::TexWrapMode, rio::TexWrapMode> GetWrapMode(const tinygltf::Sampler& sampler)
    {
        rio::TexWrapMode wrapModeS = rio::TexWrapMode::TEX_WRAP_MODE_REPEAT;
        rio::TexWrapMode wrapModeT = rio::TexWrapMode::TEX_WRAP_MODE_REPEAT;

        if (sampler.wrapS == TINYGLTF_TEXTURE_WRAP_REPEAT)
            wrapModeS = rio::TEX_WRAP_MODE_REPEAT;
        else if (sampler.wrapS == TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE)
            wrapModeS = rio::TEX_WRAP_MODE_CLAMP;
        else if (sampler.wrapS == TINYGLTF_TEXTURE_WRAP_MIRRORED_REPEAT)
            wrapModeS = rio::TEX_WRAP_MODE_REPEAT;
        
        if (sampler.wrapT == TINYGLTF_TEXTURE_WRAP_REPEAT)
            wrapModeT = rio::TEX_WRAP_MODE_REPEAT;
        else if (sampler.wrapT == TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE)
            wrapModeT = rio::TEX_WRAP_MODE_CLAMP;
        else if (sampler.wrapT == TINYGLTF_TEXTURE_WRAP_MIRRORED_REPEAT)
            wrapModeT = rio::TEX_WRAP_MODE_REPEAT;

		return { wrapModeS, wrapModeT };
    }

    Material* ModelLoader::CreateMaterial(const tinygltf::Model& model, const tinygltf::Primitive& primitive)
    {
        auto& tinyMaterial = model.materials[primitive.material];
        auto& texture = model.textures[tinyMaterial.pbrMetallicRoughness.baseColorTexture.index];
        auto& image = model.images[texture.source];

        auto material = new Material();

        material->mName = tinyMaterial.name;
        material->SetTexture(new rio::Texture2D(image.name.c_str()), rioe::Material::TEXTURE_TYPE_ALBEDO);
        material->GetTextureSampler(rioe::Material::TEXTURE_TYPE_ALBEDO)->linkTexture2D(material->GetTexture(rioe::Material::TEXTURE_TYPE_ALBEDO));
        
		auto wrapMode = GetWrapMode(model.samplers[texture.sampler]);

        material->GetTextureSampler(rioe::Material::TEXTURE_TYPE_ALBEDO)->setWrap(wrapMode.first, wrapMode.second, rio::TEX_WRAP_MODE_REPEAT);

        if (tinyMaterial.doubleSided)
            material->mRenderState.setCullingMode(rio::Graphics::CULLING_MODE_NONE);

        if (tinyMaterial.alphaMode == "OPAQUE")
            material->mRenderState.setDepthEnable(true, true);
        else if (tinyMaterial.alphaMode == "MASK")
            material->mRenderState.setDepthEnable(true, false);
        else if (tinyMaterial.alphaMode == "BLEND")
        {
            material->mRenderState.setBlendEnable(true);
            material->mRenderState.setBlendConstantColor({ 1.0f, 1.0f, 1.0f, (f32)tinyMaterial.alphaCutoff });
        }

        if (tinyMaterial.doubleSided)
			material->mRenderState.setCullingMode(rio::Graphics::CULLING_MODE_NONE);
        else
			material->mRenderState.setCullingMode(rio::Graphics::CULLING_MODE_BACK);

        return material;
    }

    Model* ModelLoader::LoadModel(const std::string& filename, const char* rootNodeName)
    {
        auto it = mModelCache.find(filename);
        if (it != mModelCache.end() && it->second)
            return mModelCache[filename];

        tinygltf::Model model;
        std::string warn, err;

        bool result = LoadGLB(filename, &model, &warn, &err);

        if (!result)
            return nullptr;

        auto newModel = new Model();
        bool bigEndian = rioe::Endian::BigEndian();
        auto rootNode = rioe::GetEngine()->GetActiveScene()->CreateNode(rootNodeName);

        newModel->mRootNode = rootNode;
        
        //// Creating node data
        std::unordered_map<int, std::shared_ptr<Node>> nodeMap;
        for (size_t i = 0; i < model.nodes.size(); ++i)
        {
            const auto& tinyNode = model.nodes[i];
            auto newNode = rioe::GetEngine()->GetActiveScene()->CreateNode();
            newNode->name = tinyNode.name;
            nodeMap[i] = newNode;
        
            rootNode->AddChild(newNode);
            
            if (tinyNode.translation.size() == 3)
                newNode->SetPosition({ static_cast<f32>(tinyNode.translation[0]), static_cast<f32>(tinyNode.translation[1]), static_cast<f32>(tinyNode.translation[2]) });
        
            if (tinyNode.scale.size() == 3)
                newNode->SetScale({ static_cast<f32>(tinyNode.scale[0]), static_cast<f32>(tinyNode.scale[1]), static_cast<f32>(tinyNode.scale[2]) });
        
            if (tinyNode.rotation.size() == 4)
                newNode->SetRotation({ static_cast<f32>(tinyNode.rotation[0]), static_cast<f32>(tinyNode.rotation[1]), static_cast<f32>(tinyNode.rotation[2]), static_cast<f32>(tinyNode.rotation[3]) });
        
            if (tinyNode.mesh == -1)
                continue;
        
            auto& tinyMesh = model.meshes[tinyNode.mesh];
        
            newNode->AddProperty(std::make_shared<rioe::properties::MeshDisplayProperty>());
            auto property = newNode->GetProperty<rioe::properties::MeshDisplayProperty>();
        
            for (const auto& primitive : tinyMesh.primitives)
            {
                std::vector<rio::mdl::res::Vertex> vertices;
                std::vector<unsigned int> indicesData;
        
                const tinygltf::Accessor& positionAccessor = model.accessors[primitive.attributes.find("POSITION")->second];
                const tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];
        
                const float* positions = GetAccessor<float>(model, primitive.attributes.find("POSITION")->second);
                const float* normals = GetAccessor<float>(model, primitive.attributes.find("NORMAL")->second);
                const float* texCoords = GetAccessor<float>(model, primitive.attributes.find("TEXCOORD_0")->second);
                const unsigned short* indices = GetAccessor<unsigned short>(model, primitive.indices);
        
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
        
                    vertex.normal = { nX, nY, nZ };
                    vertex.pos = { x, y, z };
                    vertex.tex_coord = { uvX, uvY };
        
                    if (bigEndian)
                    {
                        vertex.normal = rioe::Endian::SwapEndianness(vertex.normal);
                        vertex.pos = rioe::Endian::SwapEndianness(vertex.pos);
                        vertex.tex_coord = { rioe::Endian::SwapEndianness(vertex.tex_coord.x), rioe::Endian::SwapEndianness(vertex.tex_coord.y) };
                    }
                    
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
        
                // Material Data
                if (primitive.material >= 0 && primitive.material < model.materials.size())
                    mesh->mMaterial = CreateMaterial(model, primitive);
        
                property->mMeshes.emplace_back(mesh);
                newModel->mMeshes.emplace_back(mesh);
            }
        
            property->Initialize();
        }
        
        // Parenting nodes together
        for (size_t i = 0; i < model.nodes.size(); ++i)
        {
            const auto& tinyNode = model.nodes[i];
            auto newNode = nodeMap[i];
        
            // Attach children nodes
            for (int childIndex : tinyNode.children)
            {
                auto childNode = nodeMap[childIndex];
                newNode->AddChild(childNode);
            }
        }
        
        RIO_LOG("[ModelLoader] Loaded %s.\n", filename.c_str());
        
        mModelCache.emplace(filename, newModel);  // Cache the model
        return newModel;
    }

    Skeleton* ModelLoader::LoadSkeleton(const std::string& filename, const char* rootNodeName)
    {  
       // Loading GLTF file
       tinygltf::TinyGLTF loader;
       tinygltf::Model model;
       std::string warn, err;
       
       bool result = LoadGLB(filename, &model, &warn, &err);

       if (!result)
           return nullptr;

       auto newSkeleton = new Skeleton();
       bool bigEndian = rioe::Endian::BigEndian();
	   auto rootNode = rioe::GetEngine()->GetActiveScene()->CreateNode(rootNodeName);
       
       for (const auto& mesh : model.meshes)
       {
           for (const auto& primitive : mesh.primitives)
           {
               std::vector<rioe::SkeletalMesh::Vertex> vertices;
               std::vector<unsigned int> indicesData;
       
               const tinygltf::Accessor& positionAccessor = model.accessors[primitive.attributes.find("POSITION")->second];
               const tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];
       
               const float* positions = GetAccessor<float>(model, primitive.attributes.find("POSITION")->second);
               const float* normals = GetAccessor<float>(model, primitive.attributes.find("NORMAL")->second);
               const float* texCoords = GetAccessor<float>(model, primitive.attributes.find("TEXCOORD_0")->second);
               const unsigned short* indices = GetAccessor<unsigned short>(model, primitive.indices);
       
               const float* weights = GetAccessor<float>(model, primitive.attributes.find("WEIGHTS_0")->second);
               const u8* joints = GetAccessor<u8>(model, primitive.attributes.find("JOINTS_0")->second);
       
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
                   newMesh->mMaterial = CreateMaterial(model, primitive);
               }
       
               newSkeleton->mMeshes.emplace_back(newMesh);
           }
       }
       
       for (const auto& skin : model.skins)
       {
           newSkeleton->mBones.resize(skin.joints.size());
           newSkeleton->mBoneAnimationMap.reserve(skin.joints.size());
       
           // Getting inverse bind matrices
           if (skin.inverseBindMatrices >= 0)
           {
               const auto& accessor = model.accessors[skin.inverseBindMatrices];
       
               const float* matrixData = GetAccessor<float>(model, skin.inverseBindMatrices);
               newSkeleton->mInverseBindMatrices.resize(accessor.count);
       
               for (size_t i = 0; i < accessor.count; ++i) {
                   rio::Matrix44f inverseBindMatrix;
       
                   // Read 16 floats from matrixData for the current matrix
                   for (int j = 0; j < 16; ++j) {
                       float inv = matrixData[i * 16 + j];
       
                       if (bigEndian)
                           inv = rioe::Endian::SwapEndianness(inv);
       
                       inverseBindMatrix.a[j] = inv;
                   }
       
                   inverseBindMatrix.transpose();
       
                   newSkeleton->mInverseBindMatrices[i] = inverseBindMatrix;  // Store the matrix
               }
           }
       
           // Creating node data
           for (size_t i = 0; i < skin.joints.size(); i++)
           {
               const auto& gltfNode = model.nodes[skin.joints[i]];
               auto newNode = rioe::GetEngine()->GetActiveScene()->CreateNode();
       
               newNode->name = gltfNode.name;
       
               if (!gltfNode.translation.empty())
                   newNode->SetPosition({ static_cast<f32>(gltfNode.translation[0]), static_cast<f32>(gltfNode.translation[1]), static_cast<f32>(gltfNode.translation[2]) });
                   
               if (!gltfNode.scale.empty())
                   newNode->SetScale({ static_cast<f32>(gltfNode.scale[0]), static_cast<f32>(gltfNode.scale[1]), static_cast<f32>(gltfNode.scale[2]) });
       
               if (!gltfNode.rotation.empty())
                   newNode->SetRotation({ static_cast<f32>(gltfNode.rotation[0]), static_cast<f32>(gltfNode.rotation[1]), static_cast<f32>(gltfNode.rotation[2]), static_cast<f32>(gltfNode.rotation[3]) });
       
               newSkeleton->mBones[i] = newNode;
               newSkeleton->mBoneAnimationMap[skin.joints[i]] = newNode;
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
       
       for (const auto& animation : model.animations)
       {
           RIO_LOG("Animation Name: %s\n", animation.name.c_str());
           Animation anim;
           anim.name = animation.name;
       
           for (const auto& channel : animation.channels)
           {
               AnimationChannel animChannel;
               animChannel.nodeIndex = channel.target_node;
       
               const tinygltf::AnimationSampler& sampler = animation.samplers[channel.sampler];
       
               if (sampler.interpolation == "STEP")
                   animChannel.interpolation = Step;
               else if (sampler.interpolation == "CUBICSPLINE")
                   animChannel.interpolation = Cubic;
               else if (sampler.interpolation == "LINEAR")
                   animChannel.interpolation = Linear;
       
               if (channel.target_path == "translation")
                   animChannel.target = Translation;
               else if (channel.target_path == "rotation")
                   animChannel.target = Rotation;
               else if (channel.target_path == "scale")
                   animChannel.target = Scale;
       
               const tinygltf::Accessor& inputAccessor = model.accessors[sampler.input];
               const tinygltf::Accessor& outputAccessor = model.accessors[sampler.output];
       
               const tinygltf::BufferView& inputBufferView = model.bufferViews[inputAccessor.bufferView];
               const tinygltf::Buffer& inputBuffer = model.buffers[inputBufferView.buffer];
               const float* inputData = reinterpret_cast<const float*>(&inputBuffer.data[inputBufferView.byteOffset]);
       
               const tinygltf::BufferView& outputBufferView = model.bufferViews[outputAccessor.bufferView];
               const tinygltf::Buffer& outputBuffer = model.buffers[outputBufferView.buffer];
               const float* outputData = reinterpret_cast<const float*>(&outputBuffer.data[outputBufferView.byteOffset]);
       
               size_t inputStride = inputAccessor.ByteStride(model.bufferViews[inputAccessor.bufferView]);
               size_t outputStride = outputAccessor.ByteStride(model.bufferViews[outputAccessor.bufferView]);
       
               for (size_t i = 0; i < inputAccessor.count; ++i)
               {
                   Keyframe keyframe;
                   keyframe.time = inputData[i * inputStride / sizeof(float)];
       
                   if (channel.target_path == "translation")
                       keyframe.translation = { outputData[i * outputStride / sizeof(float) + 0], outputData[i * outputStride / sizeof(float) + 1], outputData[i * outputStride / sizeof(float) + 2] };
                   else if (channel.target_path == "rotation")
                   {
                       keyframe.rotation.x = outputData[i * outputStride / sizeof(float) + 0];
                       keyframe.rotation.y = outputData[i * outputStride / sizeof(float) + 1];
                       keyframe.rotation.z = outputData[i * outputStride / sizeof(float) + 2];
                       keyframe.rotation.w = outputData[i * outputStride / sizeof(float) + 3];
                   }
                   else if (channel.target_path == "scale")
                       keyframe.scale = { outputData[i * outputStride / sizeof(float) + 0], outputData[i * outputStride / sizeof(float) + 1], outputData[i * outputStride / sizeof(float) + 2] };
       
                   if (bigEndian)
                   {
                       keyframe.translation = rioe::Endian::SwapEndianness(keyframe.translation);
                       keyframe.rotation = rioe::Endian::SwapEndianness(keyframe.rotation);
                       keyframe.scale = rioe::Endian::SwapEndianness(keyframe.scale);
                       keyframe.time = rioe::Endian::SwapEndianness(keyframe.time);
                   }
       
                   animChannel.keyframes.push_back(keyframe);
               }
       
               anim.channels.push_back(animChannel);
           }
       
           newSkeleton->mAnimations.emplace(animation.name, anim);
       
           RIO_LOG("[ModelLoader] Loaded Animation: %s\n", animation.name.c_str());
       }
       
       mSkeletonCache.emplace(filename, newSkeleton);
       
       RIO_LOG("[ModelLoader] Loaded %s.\n", filename.c_str());
       
       return newSkeleton;
    }
}