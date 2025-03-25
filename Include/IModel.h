#pragma once

#include "LInterface.h"

struct MODEL_DESC {
    const char* szName{};
};

struct MODEL_SOURCE {
    char szSkeletonPath[MAX_PATH]{};
};

L3DENGINE_API bool LoadModel(const MODEL_DESC* pDesc, MODEL_SOURCE* pSource);