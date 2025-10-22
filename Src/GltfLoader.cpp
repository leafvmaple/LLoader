#include "GltfLoader.h"
#include <iostream>
#include <algorithm>
#include <cstring>
#include <fstream>
#include <sstream>
#include <vector>
#include <windows.h>

GltfLoader::GltfLoader()
{
}

GltfLoader::~GltfLoader()
{
}

bool GltfLoader::LoadMeshFromFile(const char* szFileName, MESH_SOURCE* pSource, const GLTF_DESC* pDesc)
{
    std::cout << "Loading glTF file: " << szFileName << std::endl;
    
    // Clear previous data
    m_Vertices.clear();
    m_Indices.clear();
    m_Bones.clear();
    m_Sockets.clear();
    
    // Load glTF model using tinygltf
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;
    
    bool success = false;
    std::string filename(szFileName);
    
    if (filename.find(".glb") != std::string::npos)
    {
        success = loader.LoadBinaryFromFile(&model, &err, &warn, szFileName);
    }
    else
    {
        success = loader.LoadASCIIFromFile(&model, &err, &warn, szFileName);
    }
    
    // Extract mesh data
    if (!ExtractMeshData(model, pSource, pDesc))
    {
        return false;
    }
    
    return true;
}

bool GltfLoader::ExtractMeshData(const tinygltf::Model& model, MESH_SOURCE* pSource, const GLTF_DESC* pDesc)
{
    if (model.meshes.empty())
    {
        std::cout << "No meshes found in glTF file" << std::endl;
        return false;
    }
    
    // Use the first mesh for now
    // TODO: Support scene-based mesh selection
    const tinygltf::Mesh& mesh = model.meshes[0];
    
    if (mesh.primitives.empty())
    {
        std::cout << "No primitives found in mesh" << std::endl;
        return false;
    }
    
    // Extract vertex data from first primitive (assuming all primitives share same vertices)
    const tinygltf::Primitive& firstPrimitive = mesh.primitives[0];
    if (!ExtractVertexData(model, firstPrimitive, m_Vertices))
    {
        std::cout << "Failed to extract vertex data" << std::endl;
        return false;
    }
    
    // Collect all indices from all primitives in order
    m_Indices.clear();
    std::vector<size_t> primitiveIndexCounts;
    
    for (size_t primIdx = 0; primIdx < mesh.primitives.size(); ++primIdx)
    {
        const tinygltf::Primitive& primitive = mesh.primitives[primIdx];
        
        std::vector<DWORD> primitiveIndices;
        if (!ExtractIndexData(model, primitive, primitiveIndices))
        {
            std::cout << "Failed to extract index data for primitive " << primIdx << std::endl;
            primitiveIndexCounts.push_back(0);
            continue;
        }
        
        // Append indices directly (they should reference the same vertex buffer)
        m_Indices.insert(m_Indices.end(), primitiveIndices.begin(), primitiveIndices.end());
        primitiveIndexCounts.push_back(primitiveIndices.size());
    }
    
    // Extract skin data if requested
    if (pDesc->bLoadSkins)
    {
        ExtractSkinData(model, pSource);
    }
    
    // Set proper subset count based on primitives
    pSource->nSubsetCount = static_cast<int>(primitiveIndexCounts.size());
    if (pSource->pSubsetVertexCount)
        delete[] pSource->pSubsetVertexCount;
    pSource->pSubsetVertexCount = new WORD[pSource->nSubsetCount];
    
    // Set index count for each subset/primitive
    for (size_t i = 0; i < primitiveIndexCounts.size(); ++i)
    {
        pSource->pSubsetVertexCount[i] = static_cast<WORD>(primitiveIndexCounts[i]);
    }
    
    // Convert to mesh source
    ConvertToMeshSource(pSource, pDesc);
    
    return true;
}

bool GltfLoader::ExtractVertexData(const tinygltf::Model& model, const tinygltf::Primitive& primitive, 
                                  std::vector<VERTEX_SOURCE>& vertices)
{
    // Get vertex count from POSITION attribute
    auto positionIt = primitive.attributes.find("POSITION");
    if (positionIt == primitive.attributes.end())
    {
        std::cout << "No POSITION attribute found" << std::endl;
        return false;
    }
    
    int positionAccessor = positionIt->second;
    if (positionAccessor >= static_cast<int>(model.accessors.size()))
    {
        std::cout << "Invalid POSITION accessor index" << std::endl;
        return false;
    }
    
    size_t vertexCount = model.accessors[positionAccessor].count;
    vertices.resize(vertexCount);
    
    // Initialize vertices with default values
    for (auto& vertex : vertices)
    {
        vertex.Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
        vertex.Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
        vertex.Tangent = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
        vertex.Color = 0xFFFFFFFF;
        vertex.TexCoords = XMFLOAT2(0.0f, 0.0f);
        for (int i = 0; i < VERTEX_MAX_BONE; ++i)
        {
            vertex.BoneWeights[i] = 0.0f;
            vertex.BoneIndices[i] = 0;
        }
        vertex.Emissive = 0xFFFFFFFF;
    }
    
    // Extract POSITION
    std::vector<XMFLOAT3> positions;
    if (ReadAccessor(model, positionAccessor, positions))
    {
        for (size_t i = 0; i < vertices.size() && i < positions.size(); ++i)
        {
            vertices[i].Position = positions[i];
        }
    }
    
    // Extract NORMAL
    auto normalIt = primitive.attributes.find("NORMAL");
    if (normalIt != primitive.attributes.end())
    {
        std::vector<XMFLOAT3> normals;
        if (ReadAccessor(model, normalIt->second, normals))
        {
            for (size_t i = 0; i < vertices.size() && i < normals.size(); ++i)
            {
                vertices[i].Normal = normals[i];
            }
        }
    }
    
    // Extract TANGENT
    auto tangentIt = primitive.attributes.find("TANGENT");
    if (tangentIt != primitive.attributes.end())
    {
        std::vector<XMFLOAT4> tangents;
        if (ReadAccessor(model, tangentIt->second, tangents))
        {
            for (size_t i = 0; i < vertices.size() && i < tangents.size(); ++i)
            {
                vertices[i].Tangent = tangents[i];
            }
        }
    }
    
    // Extract TEXCOORD_0
    auto texcoordIt = primitive.attributes.find("TEXCOORD_0");
    if (texcoordIt != primitive.attributes.end())
    {
        std::vector<XMFLOAT2> texCoords;
        if (ReadAccessor(model, texcoordIt->second, texCoords))
        {
            for (size_t i = 0; i < vertices.size() && i < texCoords.size(); ++i)
            {
                vertices[i].TexCoords = texCoords[i];
            }
        }
    }
    
    // Extract COLOR_0
    auto colorIt = primitive.attributes.find("COLOR_0");
    if (colorIt != primitive.attributes.end())
    {
        // Handle both VEC3 and VEC4 color formats
        const tinygltf::Accessor& accessor = model.accessors[colorIt->second];
        if (accessor.type == TINYGLTF_TYPE_VEC3)
        {
            std::vector<XMFLOAT3> colors;
            if (ReadAccessor(model, colorIt->second, colors))
            {
                for (size_t i = 0; i < vertices.size() && i < colors.size(); ++i)
                {
                    const XMFLOAT3& color = colors[i];
                    vertices[i].Color = ((DWORD)(255) << 24) | 
                                       ((DWORD)(color.x * 255) << 16) | 
                                       ((DWORD)(color.y * 255) << 8) | 
                                       ((DWORD)(color.z * 255));
                }
            }
        }
        else if (accessor.type == TINYGLTF_TYPE_VEC4)
        {
            std::vector<XMFLOAT4> colors;
            if (ReadAccessor(model, colorIt->second, colors))
            {
                for (size_t i = 0; i < vertices.size() && i < colors.size(); ++i)
                {
                    const XMFLOAT4& color = colors[i];
                    vertices[i].Color = ((DWORD)(color.w * 255) << 24) | 
                                       ((DWORD)(color.x * 255) << 16) | 
                                       ((DWORD)(color.y * 255) << 8) | 
                                       ((DWORD)(color.z * 255));
                }
            }
        }
    }
    
    // Extract JOINTS_0 and WEIGHTS_0 for skinning
    auto jointsIt = primitive.attributes.find("JOINTS_0");
    auto weightsIt = primitive.attributes.find("WEIGHTS_0");
    
    if (jointsIt != primitive.attributes.end() && weightsIt != primitive.attributes.end())
    {
        std::vector<XMFLOAT4> weights;
        if (ReadAccessor(model, weightsIt->second, weights))
        {
            const tinygltf::Accessor& jointsAccessor = model.accessors[jointsIt->second];
            
            if (jointsAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
            {
                // Read as uint8_t VEC4
                const tinygltf::BufferView& bufferView = model.bufferViews[jointsAccessor.bufferView];
                const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];
                const uint8_t* jointData = buffer.data.data() + bufferView.byteOffset + jointsAccessor.byteOffset;
                
                for (size_t i = 0; i < vertices.size() && i < weights.size(); ++i)
                {
                    vertices[i].BoneWeights[0] = weights[i].x;
                    vertices[i].BoneWeights[1] = weights[i].y;
                    vertices[i].BoneWeights[2] = weights[i].z;
                    vertices[i].BoneWeights[3] = weights[i].w;
                    
                    vertices[i].BoneIndices[0] = jointData[i * 4 + 0];
                    vertices[i].BoneIndices[1] = jointData[i * 4 + 1];
                    vertices[i].BoneIndices[2] = jointData[i * 4 + 2];
                    vertices[i].BoneIndices[3] = jointData[i * 4 + 3];
                }
            }
            else if (jointsAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
            {
                // Read as uint16_t VEC4
                const tinygltf::BufferView& bufferView = model.bufferViews[jointsAccessor.bufferView];
                const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];
                const uint16_t* jointData = reinterpret_cast<const uint16_t*>(
                    buffer.data.data() + bufferView.byteOffset + jointsAccessor.byteOffset);
                
                for (size_t i = 0; i < vertices.size() && i < weights.size(); ++i)
                {
                    vertices[i].BoneWeights[0] = weights[i].x;
                    vertices[i].BoneWeights[1] = weights[i].y;
                    vertices[i].BoneWeights[2] = weights[i].z;
                    vertices[i].BoneWeights[3] = weights[i].w;
                    
                    vertices[i].BoneIndices[0] = static_cast<uint8_t>(std::min(jointData[i * 4 + 0], (uint16_t)255));
                    vertices[i].BoneIndices[1] = static_cast<uint8_t>(std::min(jointData[i * 4 + 1], (uint16_t)255));
                    vertices[i].BoneIndices[2] = static_cast<uint8_t>(std::min(jointData[i * 4 + 2], (uint16_t)255));
                    vertices[i].BoneIndices[3] = static_cast<uint8_t>(std::min(jointData[i * 4 + 3], (uint16_t)255));
                }
            }
        }
    }
    
    return true;
}

bool GltfLoader::ExtractIndexData(const tinygltf::Model& model, const tinygltf::Primitive& primitive, 
                                 std::vector<DWORD>& indices)
{
    if (primitive.indices < 0)
    {
        // Non-indexed mesh, generate sequential indices
        size_t vertexCount = m_Vertices.size();
        indices.resize(vertexCount);
        for (size_t i = 0; i < vertexCount; ++i)
        {
            indices[i] = static_cast<DWORD>(i);
        }
        return true;
    }
    
    const tinygltf::Accessor& accessor = model.accessors[primitive.indices];
    
    if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
    {
        std::vector<uint16_t> indices16;
        if (ReadAccessor(model, primitive.indices, indices16))
        {
            indices.resize(indices16.size());
            for (size_t i = 0; i < indices16.size(); ++i)
            {
                indices[i] = indices16[i];
            }
            return true;
        }
    }
    else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
    {
        return ReadAccessor(model, primitive.indices, indices);
    }
    else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
    {
        std::vector<uint8_t> indices8;
        if (ReadAccessor(model, primitive.indices, indices8))
        {
            indices.resize(indices8.size());
            for (size_t i = 0; i < indices8.size(); ++i)
            {
                indices[i] = indices8[i];
            }
            return true;
        }
    }
    
    return false;
}

bool GltfLoader::ExtractSkinData(const tinygltf::Model& model, MESH_SOURCE* pSource)
{
    if (model.skins.empty())
        return true;
    
    // Use first skin for now
    const tinygltf::Skin& skin = model.skins[0];
    
    m_Bones.resize(skin.joints.size());
    
    // Extract bone hierarchy
    for (size_t i = 0; i < skin.joints.size(); ++i)
    {
        int nodeIndex = skin.joints[i];
        if (nodeIndex >= 0 && nodeIndex < static_cast<int>(model.nodes.size()))
        {
            const tinygltf::Node& node = model.nodes[nodeIndex];
            BONE_SOURCE& bone = m_Bones[i];
            
            // Copy bone name
            strncpy_s(bone.szName, node.name.c_str(), sizeof(bone.szName) - 1);
            bone.szName[sizeof(bone.szName) - 1] = '\0';
            
            // Set transformation matrix
            if (node.matrix.size() == 16)
            {
                for (int j = 0; j < 16; ++j)
                {
                    reinterpret_cast<float*>(&bone.mOffset2Parent)[j] = static_cast<float>(node.matrix[j]);
                }
            }
            else
            {
                // Build matrix from TRS
                XMMATRIX T = XMMatrixIdentity();
                XMMATRIX R = XMMatrixIdentity();
                XMMATRIX S = XMMatrixIdentity();
                
                if (node.translation.size() == 3)
                {
                    T = XMMatrixTranslation(static_cast<float>(node.translation[0]),
                                          static_cast<float>(node.translation[1]),
                                          static_cast<float>(node.translation[2]));
                }
                
                if (node.rotation.size() == 4)
                {
                    XMFLOAT4 quat(static_cast<float>(node.rotation[0]),
                                 static_cast<float>(node.rotation[1]),
                                 static_cast<float>(node.rotation[2]),
                                 static_cast<float>(node.rotation[3]));
                    R = XMMatrixRotationQuaternion(XMLoadFloat4(&quat));
                }
                
                if (node.scale.size() == 3)
                {
                    S = XMMatrixScaling(static_cast<float>(node.scale[0]),
                                       static_cast<float>(node.scale[1]),
                                       static_cast<float>(node.scale[2]));
                }
                
                XMStoreFloat4x4(&bone.mOffset2Parent, S * R * T);
            }
            
            // Set up children
            bone.nChildCount = static_cast<int>(node.children.size());
            if (bone.nChildCount > 0)
            {
                bone.pChildNames = new NAME_STRING[bone.nChildCount];
                for (int j = 0; j < bone.nChildCount; ++j)
                {
                    int childIndex = node.children[j];
                    if (childIndex >= 0 && childIndex < static_cast<int>(model.nodes.size()))
                    {
                        strncpy_s(bone.pChildNames[j], model.nodes[childIndex].name.c_str(), 
                                 sizeof(NAME_STRING) - 1);
                        bone.pChildNames[j][sizeof(NAME_STRING) - 1] = '\0';
                    }
                }
            }
            
            // Initialize other fields
            XMStoreFloat4x4(&bone.mOffset, XMMatrixIdentity());
            XMStoreFloat4x4(&bone.mInvPxPose, XMMatrixIdentity());
            bone.BoundingBox = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
        }
    }
    
    // Extract inverse bind matrices
    if (skin.inverseBindMatrices >= 0)
    {
        std::vector<XMFLOAT4X4> invBindMatrices;
        if (ReadAccessor(model, skin.inverseBindMatrices, invBindMatrices))
        {
            for (size_t i = 0; i < m_Bones.size() && i < invBindMatrices.size(); ++i)
            {
                m_Bones[i].mOffset = invBindMatrices[i];
            }
        }
    }
    
    // Extract socket data from nodes with "Socket_" prefix
    for (size_t i = 0; i < model.nodes.size(); ++i)
    {
        const tinygltf::Node& node = model.nodes[i];
        if (node.name.find("Socket_") == 0)
        {
            SOCKET_SOURCE socket;
            
            // Copy socket name (remove "Socket_" prefix)
            std::string socketName = node.name.substr(7); // Remove "Socket_"
            strncpy_s(socket.szName, socketName.c_str(), sizeof(socket.szName) - 1);
            socket.szName[sizeof(socket.szName) - 1] = '\0';
            
            // Find parent bone name by checking which bone has this socket as child
            socket.szParentName[0] = '\0'; // Initialize as empty
            for (size_t j = 0; j < model.nodes.size(); ++j)
            {
                const tinygltf::Node& parentNode = model.nodes[j];
                if (!parentNode.children.empty())
                {
                    for (int childIdx : parentNode.children)
                    {
                        if (childIdx == static_cast<int>(i))
                        {
                            // Found parent, copy its name
                            strncpy_s(socket.szParentName, parentNode.name.c_str(), 
                                     sizeof(socket.szParentName) - 1);
                            socket.szParentName[sizeof(socket.szParentName) - 1] = '\0';
                            break;
                        }
                    }
                }
            }
            
            // Set transformation matrix
            if (node.matrix.size() == 16)
            {
                for (int j = 0; j < 16; ++j)
                {
                    reinterpret_cast<float*>(&socket.mOffset)[j] = static_cast<float>(node.matrix[j]);
                }
            }
            else
            {
                // Build matrix from TRS
                XMMATRIX T = XMMatrixIdentity();
                XMMATRIX R = XMMatrixIdentity();
                XMMATRIX S = XMMatrixIdentity();
                
                if (node.translation.size() == 3)
                {
                    T = XMMatrixTranslation(static_cast<float>(node.translation[0]),
                                          static_cast<float>(node.translation[1]),
                                          static_cast<float>(node.translation[2]));
                }
                
                if (node.rotation.size() == 4)
                {
                    XMFLOAT4 quat(static_cast<float>(node.rotation[0]),
                                 static_cast<float>(node.rotation[1]),
                                 static_cast<float>(node.rotation[2]),
                                 static_cast<float>(node.rotation[3]));
                    R = XMMatrixRotationQuaternion(XMLoadFloat4(&quat));
                }
                
                if (node.scale.size() == 3)
                {
                    S = XMMatrixScaling(static_cast<float>(node.scale[0]),
                                       static_cast<float>(node.scale[1]),
                                       static_cast<float>(node.scale[2]));
                }
                
                XMStoreFloat4x4(&socket.mOffset, S * R * T);
            }
            
            m_Sockets.push_back(socket);
        }
    }
    
    return true;
}

template<typename T>
bool GltfLoader::ReadAccessor(const tinygltf::Model& model, int accessorIndex, std::vector<T>& data)
{
    if (accessorIndex < 0 || accessorIndex >= static_cast<int>(model.accessors.size()))
        return false;
    
    const tinygltf::Accessor& accessor = model.accessors[accessorIndex];
    const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
    const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];
    
    size_t byteOffset = bufferView.byteOffset + accessor.byteOffset;
    size_t elementSize = sizeof(T);
    size_t totalSize = accessor.count * elementSize;
    
    if (byteOffset + totalSize > buffer.data.size())
        return false;
    
    data.resize(accessor.count);
    memcpy(data.data(), buffer.data.data() + byteOffset, totalSize);
    
    return true;
}

void GltfLoader::ConvertToMeshSource(MESH_SOURCE* pSource, const GLTF_DESC* pDesc)
{
    // Apply transformations
    for (auto& vertex : m_Vertices)
    {
        ApplyTransforms(vertex, pDesc);
    }
    
    // Calculate FVF based on actual vertex attributes present in glTF
    DWORD fvf = 0;
    
    // Check if we have actual vertex data to analyze
    if (!m_Vertices.empty()) {
        // Check a few vertices to determine what attributes are present
        bool hasNormals = false;
        bool hasColors = false;
        bool hasTexCoords = false;
        bool hasSkinning = false;
        bool hasTangents = false;
        
        size_t checkCount = std::min((size_t)10, m_Vertices.size());
        for (size_t i = 0; i < checkCount; ++i) {
            const VERTEX_SOURCE& vertex = m_Vertices[i];
            
            // Check for normal
            if (!hasNormals && (vertex.Normal.x != 0.0f || vertex.Normal.y != 0.0f || vertex.Normal.z != 0.0f)) {
                hasNormals = true;
            }
            
            // Check for color (XMCOLOR is packed RGBA, check if it's not white/default)
            if (!hasColors && vertex.Color.c != 0xFFFFFFFF && vertex.Color.c != 0) {
                hasColors = true;
            }
            
            // Check for texture coordinates
            if (!hasTexCoords && (vertex.TexCoords.x != 0.0f || vertex.TexCoords.y != 0.0f)) {
                hasTexCoords = true;
            }
            
            // Check for skinning data
            if (!hasSkinning && (vertex.BoneWeights[0] > 0.0f || vertex.BoneWeights[1] > 0.0f ||
                                vertex.BoneWeights[2] > 0.0f || vertex.BoneWeights[3] > 0.0f)) {
                hasSkinning = true;
            }
            
            // Check for tangents
            if (!hasTangents && (vertex.Tangent.x != 0.0f || vertex.Tangent.y != 0.0f || 
                                vertex.Tangent.z != 0.0f || vertex.Tangent.w != 0.0f)) {
                hasTangents = true;
            }
        }
        
        // Set FVF flags based on detected attributes
        if (hasNormals) fvf |= FVF_NORMAL;
        if (hasColors) fvf |= FVF_COLOR;
        if (hasTexCoords) fvf |= FVF_TEX1;
        if (hasSkinning) fvf |= FVF_SKIN;
        if (hasTangents) fvf |= FVF_TANGENT;
    }
    
    // Fallback to basic FVF if nothing detected (this shouldn't happen normally)
    if (fvf == 0) {
        fvf = FVF_NORMAL | FVF_TEX1;
    }
    
    pSource->nVertexFVF = fvf;
    pSource->nVertexSize = sizeof(VERTEX_SOURCE);
    
    // Copy vertex data
    pSource->nVerticesCount = static_cast<int>(m_Vertices.size());
    if (pSource->nVerticesCount > 0)
    {
        pSource->pVertices = new VERTEX_SOURCE[pSource->nVerticesCount];
        memcpy(pSource->pVertices, m_Vertices.data(), m_Vertices.size() * sizeof(VERTEX_SOURCE));
    }
    
    // Copy index data
    pSource->nIndexCount = static_cast<int>(m_Indices.size());
    pSource->nFacesCount = pSource->nIndexCount / 3;
    if (pSource->nIndexCount > 0)
    {
        pSource->pIndices = new DWORD[pSource->nIndexCount];
        memcpy(pSource->pIndices, m_Indices.data(), m_Indices.size() * sizeof(DWORD));
    }
    
    // Copy bone data
    pSource->nBonesCount = static_cast<int>(m_Bones.size());
    if (pSource->nBonesCount > 0)
    {
        pSource->pBones = new BONE_SOURCE[pSource->nBonesCount];
        memcpy(pSource->pBones, m_Bones.data(), m_Bones.size() * sizeof(BONE_SOURCE));
        
        // Clear original data to prevent double deletion
        for (auto& bone : m_Bones)
        {
            bone.pChildNames = nullptr;
        }
    }
    
    // Copy socket data
    pSource->nSocketCount = static_cast<int>(m_Sockets.size());
    if (pSource->nSocketCount > 0)
    {
        pSource->pSockets = new SOCKET_SOURCE[pSource->nSocketCount];
        memcpy(pSource->pSockets, m_Sockets.data(), m_Sockets.size() * sizeof(SOCKET_SOURCE));
    }
    
    // Subset data is set in ExtractMeshData, don't override here
}

void GltfLoader::ApplyTransforms(VERTEX_SOURCE& vertex, const GLTF_DESC* pDesc)
{
    // Apply scale
    vertex.Position.x *= pDesc->fScale;
    vertex.Position.y *= pDesc->fScale;
    vertex.Position.z *= pDesc->fScale;
    
    // Apply coordinate system transformations
    if (pDesc->bFlipYZ)
    {
        std::swap(vertex.Position.y, vertex.Position.z);
        std::swap(vertex.Normal.y, vertex.Normal.z);
        std::swap(vertex.Tangent.y, vertex.Tangent.z);
    }
    
    // Apply UV transforms
    if (pDesc->bFlipUV)
    {
        vertex.TexCoords.y = 1.0f - vertex.TexCoords.y;
    }
}

// Export functions
extern "C" {

bool LoadMeshFromGLTF(GLTF_DESC* pDesc, MESH_SOURCE* pSource)
{
    if (!pDesc || !pSource)
        return false;
    
    GltfLoader loader;
    return loader.LoadMeshFromFile(pDesc->szFileName, pSource, pDesc);
}

bool IsGLTFFile(const char* szFileName)
{
    if (!szFileName)
        return false;
    
    std::string filename(szFileName);
    std::transform(filename.begin(), filename.end(), filename.begin(), ::tolower);
    
    return (filename.size() >= 5 && filename.substr(filename.size() - 5) == ".gltf") ||
           (filename.size() >= 4 && filename.substr(filename.size() - 4) == ".glb");
}

}