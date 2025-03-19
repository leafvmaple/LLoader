#pragma once

#include "LInterface.h"
#include "LAssert.h"

#pragma pack(push, 8)

struct MODEL_MATERIAL_DESC
{
    const char* szFileName{};
};

struct MATERIAL_DEFINE
{
    char szName[MAX_PATH]{};
    char szShaderName[MAX_PATH]{};

    unsigned int nParam{};
    struct _Param
    {
        char szName[MAX_PATH]{};
        char szRegister[MAX_PATH]{};
        union
        {
            char szValue[MAX_PATH]{};
            float fValue;
        };
    }*pParam{};

    ~MATERIAL_DEFINE()
    {
        delete[] pParam;
    }
};

struct MATERIAL_SOURCE
{
    unsigned int nBlendMode{};
    unsigned int nAlphaRef{};
    unsigned int nAlphaRef2{};
    unsigned int nTwoSideFlag{};
    unsigned int nTexture{};

    struct _Param
    {
        char szName[MAX_PATH]{};
        char szType[MAX_PATH]{};
        char szValue[MAX_PATH]{};
    }* pTexture{};

    MATERIAL_DEFINE Define{};

    ~MATERIAL_SOURCE()
    {
		delete[] pTexture;
    }
};

struct MATERIAL_GROUP_SOURCE
{
    unsigned int nSubset{};
    MATERIAL_SOURCE* pSubset{};

    ~MATERIAL_GROUP_SOURCE()
    {
        delete[] pSubset;
    }
};

struct MATERIAL_LOD_SOURCE
{
    unsigned int nGroup{};
    MATERIAL_GROUP_SOURCE* pGroup{};

    ~MATERIAL_LOD_SOURCE()
    {
        delete[] pGroup;
    }
};

struct MODEL_MATERIAL_SOURCE
{
    unsigned int nLOD{};
    MATERIAL_LOD_SOURCE* pLOD{};

    ~MODEL_MATERIAL_SOURCE()
    {
        delete[] pLOD;
    }
};

struct REFER_MATERIAL_DESC
{
    const char* szFileName{};
};

struct REFER_MATERIAL_SOURCE
{
    char szShaderName[MAX_PATH];

    unsigned int nParam{};

    struct _Param
    {
        char szName[MAX_PATH]{};
        char szRegister[MAX_PATH]{};
        char szValue[MAX_PATH]{};
    }* pParam{};

    ~REFER_MATERIAL_SOURCE()
    {
        delete[] pParam;
    }
};

#pragma pack(pop)

L3DENGINE_API void LoadModelMaterial(MODEL_MATERIAL_DESC* pDesc, MODEL_MATERIAL_SOURCE* pSource);
L3DENGINE_API void LoadReferMaterial(REFER_MATERIAL_DESC* pDesc, REFER_MATERIAL_SOURCE* pSource);
