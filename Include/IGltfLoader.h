#pragma once

#include "LInterface.h"
#include "LAssert.h"
#include "IMesh.h"

// glTF Loader Interface
struct GLTF_DESC
{
    const char* szFileName{};
    float fScale = 1.0f;
    bool bFlipUV = false;  // Whether to flip V coordinate (glTF uses OpenGL convention)
    bool bFlipYZ = false;  // Whether to swap Y and Z coordinates
    bool bLoadAnimations = true;  // Whether to load animations
    bool bLoadSkins = true;       // Whether to load skin/bone data
    int nSceneIndex = -1;         // Scene index to load (-1 for default scene)
};

// glTF Loader Export Functions
L3DENGINE_API bool LoadMeshFromGLTF(GLTF_DESC* pDesc, MESH_SOURCE* pSource);
L3DENGINE_API bool IsGLTFFile(const char* szFileName);