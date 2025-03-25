#include "Skeleton.h"

#include <cstdio>
#include <unordered_map>

bool LoadSkeleton(const SKELETON_DESC* pDesc, SKELETON_SOURCE* pSource) {
    char szLine[MAX_PATH];
    std::vector<std::vector<std::string>> childBoneNames;
    std::unordered_map<std::string, int> boneMap;
    FILE* f = fopen(pDesc->szName, "r");

    assert(fscanf(f, "%d\n", &pSource->nBoneCount));

    pSource->pBones = new SKELETON_BONE_SOURCE[pSource->nBoneCount];
    childBoneNames.resize(pSource->nBoneCount);

    for (int i = 0; i < pSource->nBoneCount; i++) {
        fgets(szLine, MAX_PATH, f);
        szLine[strlen(szLine) - 1] = '\0';
        for (int j = 0; szLine[j]; j++)
            szLine[j] = tolower(szLine[j]);

        _LineToBoneInfo(szLine, childBoneNames[i]);

        boneMap[childBoneNames[i][0]] = i;
    }

    for (int i = 0; i < pSource->nBoneCount; i++) {
        auto& bone = pSource->pBones[i];

        strcpy(bone.szBoneName, childBoneNames[i][0].c_str());

        bone.nChildCount = childBoneNames[i].size() - 1;
        bone.pChildren = new int[bone.nChildCount];

        for (int j = 0; j < bone.nChildCount; j++) {
            int childBoneIndex = boneMap[childBoneNames[i][j + 1]];
            bone.pChildren[j] = childBoneIndex;
            pSource->pBones[childBoneIndex].nParentIndex = i;
        }
    }

    assert(fscanf(f, "%d\n", &pSource->nRootBoneCount));
    pSource->pRootBoneIndies = new int[pSource->nRootBoneCount];
    for (int i = 0; i < pSource->nRootBoneCount; i++) {
        assert(fscanf(f, "%d\n", &pSource->pRootBoneIndies[i]));
    }

    return true;
}

void _LineToBoneInfo(const char szLineBuffer[], std::vector<std::string>& childBoneNames) {
    char* szFind = nullptr;
    char* szLine = nullptr;
    bool bFirst = false;

    szLine = (char*)szLineBuffer;
    while (szFind = strstr(szLine, "  "))
    {
        *szFind = '\0';
        childBoneNames.push_back(szLine);
        szLine = szFind + 2;
    }
    childBoneNames.push_back(szLine);
}