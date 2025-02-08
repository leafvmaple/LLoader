#pragma once

#include "IAnimation.h"

#include "LInterface.h"
#include "LAssert.h"

#include <d3d11.h>
#include <vector>
#include <string>
#include <DirectXMath.h>

using namespace DirectX;

constexpr int ANI_STRING_SIZE = 30;
const unsigned ANI_FILE_MASK = 0x414E494D;
const unsigned ANI_FILE_MASK_VERVION2 = 0x324E494D;

enum ANIMATION_TYPE
{
    ANIMATION_NONE = 0,
    ANIMATION_VERTICES = 1,
    ANIMATION_VERTICES_RELATIVE = 2,
    ANIMATION_BONE = 10,
    ANIMATION_BONE_RELATIVE = 11,
    ANIMATION_BONE_16 = 12,
    ANIMATION_BONE_RTS = 13,
    ANIMATION_BLENDSHAPE = 14,
    ANIMATION_VCIK = 15,    //视频捕捉动画关键点转FullBodyIK
    ANIMATION_COUNT,
    ANIMATION_FORCE_DWORD = 0xffffffff,
};

enum BONE_FLAG
{
    BONE_FLAG_NONE = 0,
    BONE_FLAG_AFFLINE = 1 << 0,
    BONE_FLAG_NO_UPDATE = 1 << 1,
};

struct ANIMATION_DESC
{
    const char* szFileName{};
};

struct RTS
{
    XMFLOAT3 Translation{ 0.f, 0.f, 0.f };
    XMFLOAT3 Scale{ 1.f, 1.f, 1.f };
    XMFLOAT4 Rotation{ 0.f, 0.f, 0.f, 1.f };
    float Sign{ 1.f };
    XMFLOAT4 SRotation{ 0.f, 0.f, 0.f, 1.f };
};

struct ANIMATION_SOURCE : LUnknown
{
    int nAnimationType{};

    int nBonesCount{};
    int nFrameCount{};
    float fFrameLength{};
    int nAnimationLength{};

    std::vector<std::string> pBoneNames;
    std::vector<std::vector<RTS>> pBoneRTS;
    std::vector<int> pFlag;

    virtual ~ANIMATION_SOURCE() = default;
};

L3DENGINE_API void LoadAnimation(ANIMATION_DESC* pDesc, ANIMATION_SOURCE*& pSource);