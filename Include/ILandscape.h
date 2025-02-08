#pragma once

#include "LInterface.h"
#include "LAssert.h"

#include <d3d11.h>
#include <DirectXMath.h>

#include "IMaterial.h"

using namespace DirectX;

struct LANDSCAPE_DESC
{
    const wchar_t* szDir{};
    const wchar_t* szMapName{};
};

struct LANDSCAPE_REGION
{
    int nMaterial{};
    int* pMaterialIDs{};

    int nHeightData{};
    float* pHeightData{};

    ~LANDSCAPE_REGION() {
        delete[] pMaterialIDs;
        delete[] pHeightData;
    }
};

struct LANDSCAPE_MATERIAL
{
    int nLODCount{};
    MATERIAL_SOURCE* pLOD{}; // every LOD has a material
};

struct LANDSCAPE_SOURCE : LUnknown
{
    UINT        RegionSize{};
    UINT        LeafNodeSize{};
    XMFLOAT2    WorldOrigin{};
    XMFLOAT2    UnitScale{};

    XMUINT2     RegionTableSize{};

    int nMaterialCount{};

    LANDSCAPE_MATERIAL* pMaterials{};
    LANDSCAPE_REGION* pRegionTable{};

    virtual ~LANDSCAPE_SOURCE() {
        delete[] pRegionTable;
        delete[] pMaterials;
    }
};

L3DENGINE_API void LoadLandscape(LANDSCAPE_DESC* pDesc, LANDSCAPE_SOURCE*& pSource);