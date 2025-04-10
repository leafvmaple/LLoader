#include "Flex.h"

#include "LFileReader.h"

void LoadFlex(FLEX_DESC* pDesc, FLEX_SOURCE* pSource)
{
    LFileReader reader;
    CSimpleIniA Ini;

    LFileReader::ReadIni(pDesc->szFileName, Ini);

    uint32_t nVersion = Ini.GetLongValue("HEAD", "dwVersion", 0);
    assert(nVersion >= 241231);

    pSource->nType = Ini.GetLongValue("TYPE", "eFlexObjectType", 0);

    pSource->FlexCloth.fStretchStiffness = Ini.GetDoubleValue("CLOTH_PARAMS", "fStretchStiffness", 0.f);
    pSource->FlexCloth.fBendStiffness = Ini.GetDoubleValue("CLOTH_PARAMS", "fBendStiffness", 0.f);
    pSource->FlexCloth.fTetherStifness = Ini.GetDoubleValue("CLOTH_PARAMS", "fTetherStifness", 0.f);
    pSource->FlexCloth.fTetherGive = Ini.GetDoubleValue("CLOTH_PARAMS", "fTetherGive", 0.f);
    pSource->FlexCloth.fPressure = Ini.GetDoubleValue("CLOTH_PARAMS", "fPressure", 0.f);
    pSource->FlexCloth.fTessellationPrecision = Ini.GetDoubleValue("CLOTH_PARAMS", "fTessellationPrecision", 0.f);
    pSource->FlexCloth.nSmoothLevel = Ini.GetLongValue("CLOTH_PARAMS", "nSmoothLevel", 0);
    pSource->FlexCloth.bAutoLink = Ini.GetBoolValue("CLOTH_PARAMS", "bAutoLink", false);
    pSource->FlexCloth.fSmoothScaleFactor = Ini.GetDoubleValue("CLOTH_PARAMS", "fSmoothScaleFactor", 0.f);
    pSource->FlexCloth.fRScaleFactor = Ini.GetDoubleValue("CLOTH_PARAMS", "fRScaleFactor", 0.f);
    pSource->FlexCloth.fRMaxValue = Ini.GetDoubleValue("CLOTH_PARAMS", "fRMaxValue", 0.f);
}