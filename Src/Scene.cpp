#include "Scene.h"
#include "IScene.h"

#include "LFileReader.h"

#include <filesystem>

void LoadScene(SCENE_DESC* pDesc, SCENE_SOURCE* pSource) {
    std::filesystem::path filePath = pDesc->szFileName;

    auto dir = filePath.parent_path();
    auto mapName = filePath.stem();

    strcpy(pSource->szDir, dir.string().c_str());
    strcpy(pSource->szMapName, mapName.string().c_str());

    sprintf(pSource->szLandscape, "%s/landscape/%s_landscapeinfo.json", pSource->szDir, pSource->szMapName);
}