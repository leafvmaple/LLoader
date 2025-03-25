#pragma once

#include "LInterface.h"

#pragma pack(push, 8)

struct SKELETON_DESC {
    const char* szName{};
};

struct SKELETON_BONE_SOURCE {
    NAME_STRING szBoneName{};
    int nParentIndex{};

    int nChildCount{};
    int* pChildren{};

    ~SKELETON_BONE_SOURCE() {
        delete[] pChildren;
    }
};

struct SKELETON_SOURCE {
    unsigned int nBoneCount{};
    SKELETON_BONE_SOURCE* pBones{};

    int nRootBoneCount{};
    int* pRootBoneIndies{};

    ~SKELETON_SOURCE() {
        delete[] pBones;
        delete[] pRootBoneIndies;
    }
};

L3DENGINE_API bool LoadSkeleton(const SKELETON_DESC* pDesc, SKELETON_SOURCE* pSource);