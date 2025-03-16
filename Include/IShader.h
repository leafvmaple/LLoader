#pragma once

#include "LInterface.h"
#include "LAssert.h"
#include "LFormat.h"

struct SHADER_DESC {
    const char* szName{};
};

struct SHADER_SOURCE {
    BYTE* pByte{};
    size_t nSize{};
};

L3DENGINE_API bool LoadVSShader(const SHADER_DESC* pDesc, SHADER_SOURCE* pSource);
L3DENGINE_API bool LoadPSShader(const SHADER_DESC* pDesc, SHADER_SOURCE* pSource);