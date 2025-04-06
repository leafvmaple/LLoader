#pragma once

#include "LInterface.h"

#pragma pack(push, 8)

struct SCENE_DESC
{
    const char* szFileName{};
};

struct SCENE_SOURCE
{
    char szDir[MAX_PATH]{};
    char szMapName[MAX_PATH]{};
    char szLandscape[MAX_PATH]{};
};

#pragma pack(pop)

L3DENGINE_API void LoadScene(SCENE_DESC* pDesc, SCENE_SOURCE* pSource);