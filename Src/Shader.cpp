#include "Shader.h"

#include "LFileReader.h"

#define FXO_PATH "data/material/Shader/Base/FXO/g_%s_%s_ASSEMBLY.fxo"

static bool LoadShader(const char* szFileName, const char* szType, BYTE** ppByteCode, size_t* puSize)
{
    char szFilePath[MAX_PATH]{};

    if (sprintf_s(szFilePath, FXO_PATH, szType, szFileName) < 0)
        return false;

    if (FAILED(LFileReader::Read(szFilePath, ppByteCode, puSize)))
        return false;

    *puSize -= sizeof(GUID);

    return true;
}

bool LoadVSShader(const SHADER_DESC* pDesc, SHADER_SOURCE* pSource)
{
    return LoadShader(pDesc->szName, "vs", &pSource->pByte, &pSource->nSize);
}

bool LoadPSShader(const SHADER_DESC* pDesc, SHADER_SOURCE* pSource)
{
    return LoadShader(pDesc->szName, "ps", &pSource->pByte, &pSource->nSize);
}