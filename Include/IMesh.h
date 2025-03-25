#pragma once

#include "LInterface.h"
#include "LAssert.h"
#include "LFormat.h"

#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

using namespace DirectX;
using namespace DirectX::PackedVector;

enum D3DFVF
{
    FVF_XYZW = 0x001,
    FVF_NORMAL = 0x002,
    FVF_COLOR = 0x004,
    FVF_TEX1 = 0x008,
    FVF_SKIN = 0x010,
    FVF_TANGENT = 0x020,
};

#pragma pack(push, 8)

struct MESH_DESC
{
    const char* szName{};
};

struct BONE_SOURCE
{
    NAME_STRING szName{};

    int nChildCount{};
    NAME_STRING* pChildNames{};

    XMFLOAT4X4 mOffset{};
    XMFLOAT4X4 mOffset2Parent{};
    XMFLOAT4X4 mInvPxPose{};
    BOUND_BOX BoundingBox{};

    ~BONE_SOURCE()
    {
        delete[] pChildNames;
    }
};

struct SOCKET_SOURCE
{
    NAME_STRING szName{};
    NAME_STRING szParentName{};
    XMFLOAT4X4 mOffset{};
};

static const unsigned VERTEX_MAX_BONE = 4;
struct VERTEX_SOURCE
{
    XMFLOAT3 Position{ 0.f, 0.f, 0.f };
    XMFLOAT3 Normal{ 0.f, 0.f, 0.f };
    XMCOLOR Color{ 0, 0, 0, 0 };
    XMFLOAT2 TexCoords{ 0.f, 0.f };
    float BoneWeights[VERTEX_MAX_BONE]{ 0.f };
    BYTE BoneIndices[VERTEX_MAX_BONE]{ 0xFF, 0xFF, 0xFF, 0xFF };
    XMFLOAT4 Tangent{ 0.0f, 1.0f, 0.0f, 1.0f };
};

struct MESH_SOURCE
{
    int nVertexFVF{};
    int nVertexSize{};
    int nFacesCount{};

    int nVerticesCount{};
    VERTEX_SOURCE* pVertices{};

    int nIndexCount{};
    DWORD* pIndices{};

    int nBonesCount{};
    BONE_SOURCE* pBones{};

    int nSocketCount{};
    SOCKET_SOURCE* pSockets{};

    int nSubsetCount{};
    WORD* pSubsetVertexCount{};

    ~MESH_SOURCE()
    {
        delete[] pVertices;
        delete[] pIndices;
        delete[] pBones;
        delete[] pSockets;
        delete[] pSubsetVertexCount;
    }
};

#pragma pack(pop)

L3DENGINE_API void LoadMesh(MESH_DESC* pDesc, MESH_SOURCE* pSource);