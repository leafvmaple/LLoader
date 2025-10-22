#pragma once

#include "IGltfLoader.h"
#include <vector>
#include <string>
#include <map>
#include <memory>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tinygltf/tiny_gltf.h"

class GltfLoader
{
public:
    GltfLoader();
    ~GltfLoader();

    bool LoadMeshFromFile(const char* szFileName, MESH_SOURCE* pSource, const GLTF_DESC* pDesc);

private:
    // Data extraction
    bool ExtractMeshData(const tinygltf::Model& model, MESH_SOURCE* pSource, const GLTF_DESC* pDesc);
    bool ExtractVertexData(const tinygltf::Model& model, const tinygltf::Primitive& primitive, 
                          std::vector<VERTEX_SOURCE>& vertices);
    bool ExtractIndexData(const tinygltf::Model& model, const tinygltf::Primitive& primitive, 
                         std::vector<DWORD>& indices);
    bool ExtractSkinData(const tinygltf::Model& model, MESH_SOURCE* pSource);
    
    // Accessor helpers
    template<typename T>
    bool ReadAccessor(const tinygltf::Model& model, int accessorIndex, std::vector<T>& data);
    
    // Data conversion helpers
    void ConvertToMeshSource(MESH_SOURCE* pSource, const GLTF_DESC* pDesc);
    void ApplyTransforms(VERTEX_SOURCE& vertex, const GLTF_DESC* pDesc);

private:
    // Parsed data
    std::vector<VERTEX_SOURCE> m_Vertices;
    std::vector<DWORD> m_Indices;
    std::vector<BONE_SOURCE> m_Bones;
    std::vector<SOCKET_SOURCE> m_Sockets;
};