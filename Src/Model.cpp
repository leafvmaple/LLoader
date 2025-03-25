#include "Model.h"

#include <cstdio>

bool LoadModel(const MODEL_DESC* pDesc, MODEL_SOURCE* pSource) {
    char szPath[MAX_PATH]{};
    FILE* f = fopen(pDesc->szName, "r");

    assert(fscanf(f, "%s", szPath));

    strcpy(pSource->szSkeletonPath, szPath[0] == '\\' ? szPath + 1 : szPath);

    return true;
}