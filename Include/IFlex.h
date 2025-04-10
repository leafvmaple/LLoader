#pragma once

#include "LInterface.h"

#pragma pack(push, 8)

struct FLEX_DESC
{
    const char* szFileName{};
};

struct FLEX_CLOTH_SOURCE
{
    float fStretchStiffness;
    float fBendStiffness;
    float fTetherStifness;
    float fTetherGive;
    float fPressure;
    float fTessellationPrecision;
    int   nSmoothLevel;
    bool  bAutoLink;
    float fSmoothScaleFactor;
    float fRScaleFactor;
    float fRMaxValue;
};

struct FLEX_SOFTBODY_SOURCE
{
    float fParticleSpacing;
    float fVolumeSampling;
    float fSurfaceSampling;
    float fClusterSpacing;
    float fClusterRadius;
    float fClusterStiffness;
    float fLinkRadius;
    float fLinkStiffness;
    float fGlobalStiffness;
    float fClusterPlasticThreshold;
    float fClusterPlasticCreep;
    float fSkinningFalloff;
    float fSkinningMaxDistance;
    float fTetherGive;
    int   nSmoothLevel;
    bool  bAutoLink;
};

struct FLEX_SOURCE
{
    int nType{};
    FLEX_CLOTH_SOURCE FlexCloth;
    FLEX_SOFTBODY_SOURCE FlexSoftBody{};
};

#pragma pack(pop)

L3DENGINE_API void LoadFlex(FLEX_DESC* pDesc, FLEX_SOURCE* pSource);