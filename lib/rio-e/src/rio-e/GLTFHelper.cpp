#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE

#include "tiny_gltf.h"
#include "rio-e/GLTFHelper.h"
#include "gfx/mdl/res/rio_MeshData.h"

namespace rioe
{
	Mesh* GLTFRead(std::string path)
	{
		tinygltf::TinyGLTF loader;
		tinygltf::Model model;
		std::string warn, err;

		bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, path);

		if (!ret)
		{
			RIO_LOG("Error! %s %s\n", warn.c_str(), err.c_str());
		}

		for (const auto& mesh : model.meshes)
		{
			std::vector<unsigned int> indexData;
			std::vector<rio::mdl::res::Vertex> vertices;

			for (const auto& primitive : mesh.primitives)
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

					vertex.pos = { positions[i * 3], positions[i * 3 + 1], positions[i * 3 + 2] };

					if (normals)
						vertex.normal = { normals[i * 3], normals[i * 3 + 1], normals[i * 3 + 2] };

					if (tex_coords)
						vertex.tex_coord = { tex_coords[i * 2], tex_coords[i * 2 + 1] };

					vertices.push_back(vertex);
				}

				for (size_t i = 0; i < index_accessor.count; ++i)
				{
					indexData.push_back(indices[i] + baseVertexIndex);
				}
			}

			RIO_LOG("Total: %d, %d\n", vertices.size(), indexData.size());

			return new Mesh(vertices, indexData);
		}
	}
}