import struct
from pygltflib import GLTF2
from structs import Vertex


def read_gltf(path):
    gltf = GLTF2().load(path)
    buffer = gltf.binary_blob()

    primitive = gltf.meshes[0].primitives[0]
    accessors = gltf.accessors
    bufferViews = gltf.bufferViews

    # Reading indices
    index_buffer_index = primitive.indices
    index_accessor = accessors[index_buffer_index]
    index_count = index_accessor.count
    index_buffer_view = bufferViews[index_accessor.bufferView]
    indices = struct.unpack_from(f"<{index_count}H", buffer, index_buffer_view.byteOffset)

    # Reading positions
    position_buffer_index = primitive.attributes.POSITION
    position_accessor = accessors[position_buffer_index]
    position_count = position_accessor.count
    position_buffer_view = bufferViews[position_accessor.bufferView]
    positions = [struct.unpack_from("<3f", buffer, position_buffer_view.byteOffset + i * 12) for i in range(position_count)]

    # Reading normals
    normal_buffer_index = primitive.attributes.NORMAL
    normal_accessor = accessors[normal_buffer_index]
    normal_count = normal_accessor.count
    normal_buffer_view = bufferViews[normal_accessor.bufferView]
    normals = [struct.unpack_from("<3f", buffer, normal_buffer_view.byteOffset + i * 12) for i in range(normal_count)]

    # Reading texture coordinates
    tex_coord_buffer_index = primitive.attributes.TEXCOORD_0
    tex_coord_accessor = accessors[tex_coord_buffer_index]
    tex_coord_count = tex_coord_accessor.count
    tex_coord_buffer_view = bufferViews[tex_coord_accessor.bufferView]
    tex_coords = [struct.unpack_from("<2f", buffer, tex_coord_buffer_view.byteOffset + i * 8) for i in range(tex_coord_count)]

    vertices = []
    for pos, tex_coord, normal in zip(positions, tex_coords, normals):
        vertices.append(Vertex(pos, tex_coord, normal))

    return vertices, list(indices)

