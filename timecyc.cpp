#include <mod/amlmod.h>
#include <mod/logger.h>
#include <timecyc.h>

uint8_t  CTimeCycle__m_nDirectionalMult[NUMHOURS][NUMWEATHERS];
uint8_t  CTimeCycle__m_nWaterFogAlpha[NUMHOURS][NUMWEATHERS];
uint8_t  CTimeCycle__m_nHighLightMinIntensity[NUMHOURS][NUMWEATHERS];
uint8_t  CTimeCycle__m_fCloudAlpha[NUMHOURS][NUMWEATHERS];
uint8_t  CTimeCycle__m_fPostFx2Alpha[NUMHOURS][NUMWEATHERS];
uint8_t  CTimeCycle__m_fPostFx2Blue[NUMHOURS][NUMWEATHERS];
uint8_t  CTimeCycle__m_fPostFx2Green[NUMHOURS][NUMWEATHERS];
uint8_t  CTimeCycle__m_fPostFx2Red[NUMHOURS][NUMWEATHERS];
uint8_t  CTimeCycle__m_fPostFx1Alpha[NUMHOURS][NUMWEATHERS];
uint8_t  CTimeCycle__m_fPostFx1Blue[NUMHOURS][NUMWEATHERS];
uint8_t  CTimeCycle__m_fPostFx1Green[NUMHOURS][NUMWEATHERS];
uint8_t  CTimeCycle__m_fPostFx1Red[NUMHOURS][NUMWEATHERS];
uint8_t  CTimeCycle__m_fWaterAlpha[NUMHOURS][NUMWEATHERS];
uint8_t  CTimeCycle__m_fWaterBlue[NUMHOURS][NUMWEATHERS];
uint8_t  CTimeCycle__m_fWaterGreen[NUMHOURS][NUMWEATHERS];
uint8_t  CTimeCycle__m_fWaterRed[NUMHOURS][NUMWEATHERS];
uint8_t  CTimeCycle__m_nFluffyCloudsBottomRed[NUMHOURS][NUMWEATHERS];
uint8_t  CTimeCycle__m_nFluffyCloudsBottomGreen[NUMHOURS][NUMWEATHERS];
uint8_t  CTimeCycle__m_nFluffyCloudsBottomBlue[NUMHOURS][NUMWEATHERS];
uint8_t  CTimeCycle__m_nLowCloudsBlue[NUMHOURS][NUMWEATHERS];
uint8_t  CTimeCycle__m_nLowCloudsGreen[NUMHOURS][NUMWEATHERS];
uint8_t  CTimeCycle__m_nLowCloudsRed[NUMHOURS][NUMWEATHERS];
uint8_t  CTimeCycle__m_fLightsOnGroundBrightness[NUMHOURS][NUMWEATHERS];
uint16_t CTimeCycle__m_fFogStart[NUMHOURS][NUMWEATHERS];
uint16_t CTimeCycle__m_fFarClip[NUMHOURS][NUMWEATHERS];
uint8_t  CTimeCycle__m_nPoleShadowStrength[NUMHOURS][NUMWEATHERS];
uint8_t  CTimeCycle__m_nLightShadowStrength[NUMHOURS][NUMWEATHERS];
uint8_t  CTimeCycle__m_nShadowStrength[NUMHOURS][NUMWEATHERS];
uint8_t  CTimeCycle__m_fSpriteBrightness[NUMHOURS][NUMWEATHERS];
uint8_t  CTimeCycle__m_fSpriteSize[NUMHOURS][NUMWEATHERS];
uint8_t  CTimeCycle__m_fSunSize[NUMHOURS][NUMWEATHERS];
uint8_t  CTimeCycle__m_nSunCoronaBlue[NUMHOURS][NUMWEATHERS];
uint8_t  CTimeCycle__m_nSunCoronaGreen[NUMHOURS][NUMWEATHERS];
uint8_t  CTimeCycle__m_nSunCoronaRed[NUMHOURS][NUMWEATHERS];
uint8_t  CTimeCycle__m_nSunCoreBlue[NUMHOURS][NUMWEATHERS];
uint8_t  CTimeCycle__m_nSunCoreGreen[NUMHOURS][NUMWEATHERS];
uint8_t  CTimeCycle__m_nSunCoreRed[NUMHOURS][NUMWEATHERS];
uint8_t  CTimeCycle__m_nSkyBottomBlue[NUMHOURS][NUMWEATHERS];
uint8_t  CTimeCycle__m_nSkyBottomGreen[NUMHOURS][NUMWEATHERS];
uint8_t  CTimeCycle__m_nSkyBottomRed[NUMHOURS][NUMWEATHERS];
uint8_t  CTimeCycle__m_nSkyTopBlue[NUMHOURS][NUMWEATHERS];
uint8_t  CTimeCycle__m_nSkyTopGreen[NUMHOURS][NUMWEATHERS];
uint8_t  CTimeCycle__m_nSkyTopRed[NUMHOURS][NUMWEATHERS];
uint8_t  CTimeCycle__m_nAmbientBlue_Obj[NUMHOURS][NUMWEATHERS];
uint8_t  CTimeCycle__m_nAmbientGreen_Obj[NUMHOURS][NUMWEATHERS];
uint8_t  CTimeCycle__m_nAmbientRed_Obj[NUMHOURS][NUMWEATHERS];
uint8_t  CTimeCycle__m_nAmbientBlue[NUMHOURS][NUMWEATHERS];
uint8_t  CTimeCycle__m_nAmbientGreen[NUMHOURS][NUMWEATHERS];
uint8_t  CTimeCycle__m_nAmbientRed[NUMHOURS][NUMWEATHERS];
// Mobile thingies
RQVector CTimeCycle__m_vRedGrade[NUMHOURS][NUMWEATHERS];
RQVector CTimeCycle__m_vGreenGrade[NUMHOURS][NUMWEATHERS];
RQVector CTimeCycle__m_vBlueGrade[NUMHOURS][NUMWEATHERS];

CColourSet m_CurrentColours;
CColourSet* m_ptrCurrentColours = &m_CurrentColours;

extern uintptr_t pGTASA;

inline void WritePtr(uintptr_t addr, uintptr_t* ptrAddr, size_t len)
{
    uintptr_t* ptr = ptrAddr;
    aml->Write(addr, (uintptr_t)&ptr, len);
}
void PatchTheValues()
{
    WritePtr(pGTASA + 0x678CE4, (uintptr_t*)&CTimeCycle__m_nDirectionalMult, sizeof(void*));
    WritePtr(pGTASA + 0x678F74, (uintptr_t*)&CTimeCycle__m_nWaterFogAlpha, sizeof(void*));
    WritePtr(pGTASA + 0x67619C, (uintptr_t*)&CTimeCycle__m_nHighLightMinIntensity, sizeof(void*));
    WritePtr(pGTASA + 0x676CF8, (uintptr_t*)&CTimeCycle__m_fCloudAlpha, sizeof(void*));
    WritePtr(pGTASA + 0x679BE8, (uintptr_t*)&CTimeCycle__m_fPostFx2Alpha, sizeof(void*));
    WritePtr(pGTASA + 0x67988C, (uintptr_t*)&CTimeCycle__m_fPostFx2Blue, sizeof(void*));
    WritePtr(pGTASA + 0x6769D4, (uintptr_t*)&CTimeCycle__m_fPostFx2Green, sizeof(void*));
    WritePtr(pGTASA + 0x678D80, (uintptr_t*)&CTimeCycle__m_fPostFx2Red, sizeof(void*));
    WritePtr(pGTASA + 0x6763E4, (uintptr_t*)&CTimeCycle__m_fPostFx1Alpha, sizeof(void*));
    WritePtr(pGTASA + 0x679420, (uintptr_t*)&CTimeCycle__m_fPostFx1Blue, sizeof(void*));
    WritePtr(pGTASA + 0x676744, (uintptr_t*)&CTimeCycle__m_fPostFx1Green, sizeof(void*));
    WritePtr(pGTASA + 0x676574, (uintptr_t*)&CTimeCycle__m_fPostFx1Red, sizeof(void*));
    WritePtr(pGTASA + 0x6766C0, (uintptr_t*)&CTimeCycle__m_fWaterAlpha, sizeof(void*));
    WritePtr(pGTASA + 0x676A98, (uintptr_t*)&CTimeCycle__m_fWaterBlue, sizeof(void*));
    WritePtr(pGTASA + 0x6788FC, (uintptr_t*)&CTimeCycle__m_fWaterGreen, sizeof(void*));
    WritePtr(pGTASA + 0x679314, (uintptr_t*)&CTimeCycle__m_fWaterRed, sizeof(void*));
    WritePtr(pGTASA + 0x679E50, (uintptr_t*)&CTimeCycle__m_nFluffyCloudsBottomRed, sizeof(void*));
    WritePtr(pGTASA + 0x678BD4, (uintptr_t*)&CTimeCycle__m_nFluffyCloudsBottomGreen, sizeof(void*));
    WritePtr(pGTASA + 0x678294, (uintptr_t*)&CTimeCycle__m_nFluffyCloudsBottomBlue, sizeof(void*));
    WritePtr(pGTASA + 0x6784B8, (uintptr_t*)&CTimeCycle__m_nLowCloudsBlue, sizeof(void*));
    WritePtr(pGTASA + 0x678A7C, (uintptr_t*)&CTimeCycle__m_nLowCloudsGreen, sizeof(void*));
    WritePtr(pGTASA + 0x6796B8, (uintptr_t*)&CTimeCycle__m_nLowCloudsRed, sizeof(void*));
    WritePtr(pGTASA + 0x67979C, (uintptr_t*)&CTimeCycle__m_fLightsOnGroundBrightness, sizeof(void*));
    WritePtr(pGTASA + 0x679AD8, (uintptr_t*)&CTimeCycle__m_fFogStart, sizeof(void*));
    WritePtr(pGTASA + 0x6761A4, (uintptr_t*)&CTimeCycle__m_fFarClip, sizeof(void*));
    WritePtr(pGTASA + 0x6760FC, (uintptr_t*)&CTimeCycle__m_nPoleShadowStrength, sizeof(void*));
    WritePtr(pGTASA + 0x678298, (uintptr_t*)&CTimeCycle__m_nLightShadowStrength, sizeof(void*));
    WritePtr(pGTASA + 0x676434, (uintptr_t*)&CTimeCycle__m_nShadowStrength, sizeof(void*));
    WritePtr(pGTASA + 0x679280, (uintptr_t*)&CTimeCycle__m_fSpriteBrightness, sizeof(void*));
    WritePtr(pGTASA + 0x676F50, (uintptr_t*)&CTimeCycle__m_fSpriteSize, sizeof(void*));
    WritePtr(pGTASA + 0x679744, (uintptr_t*)&CTimeCycle__m_fSunSize, sizeof(void*));
    WritePtr(pGTASA + 0x678518, (uintptr_t*)&CTimeCycle__m_nSunCoronaBlue, sizeof(void*));
    WritePtr(pGTASA + 0x6785F4, (uintptr_t*)&CTimeCycle__m_nSunCoronaGreen, sizeof(void*));
    WritePtr(pGTASA + 0x677734, (uintptr_t*)&CTimeCycle__m_nSunCoronaRed, sizeof(void*));
    WritePtr(pGTASA + 0x67621C, (uintptr_t*)&CTimeCycle__m_nSunCoreBlue, sizeof(void*));
    WritePtr(pGTASA + 0x679014, (uintptr_t*)&CTimeCycle__m_nSunCoreGreen, sizeof(void*));
    WritePtr(pGTASA + 0x67858C, (uintptr_t*)&CTimeCycle__m_nSunCoreRed, sizeof(void*));
    WritePtr(pGTASA + 0x6799A8, (uintptr_t*)&CTimeCycle__m_nSkyBottomBlue, sizeof(void*));
    WritePtr(pGTASA + 0x678F70, (uintptr_t*)&CTimeCycle__m_nSkyBottomGreen, sizeof(void*));
    WritePtr(pGTASA + 0x677D28, (uintptr_t*)&CTimeCycle__m_nSkyBottomRed, sizeof(void*));
    WritePtr(pGTASA + 0x677D48, (uintptr_t*)&CTimeCycle__m_nSkyTopBlue, sizeof(void*));
    WritePtr(pGTASA + 0x679E38, (uintptr_t*)&CTimeCycle__m_nSkyTopGreen, sizeof(void*));
    WritePtr(pGTASA + 0x679E90, (uintptr_t*)&CTimeCycle__m_nSkyTopRed, sizeof(void*));
    WritePtr(pGTASA + 0x67696C, (uintptr_t*)&CTimeCycle__m_nAmbientBlue_Obj, sizeof(void*));
    WritePtr(pGTASA + 0x675FF4, (uintptr_t*)&CTimeCycle__m_nAmbientGreen_Obj, sizeof(void*));
    WritePtr(pGTASA + 0x6791AC, (uintptr_t*)&CTimeCycle__m_nAmbientRed_Obj, sizeof(void*));
    WritePtr(pGTASA + 0x676AFC, (uintptr_t*)&CTimeCycle__m_nAmbientBlue, sizeof(void*));
    WritePtr(pGTASA + 0x678E80, (uintptr_t*)&CTimeCycle__m_nAmbientGreen, sizeof(void*));
    WritePtr(pGTASA + 0x677EDC, (uintptr_t*)&CTimeCycle__m_nAmbientRed, sizeof(void*));
// Mobile thingies
    WritePtr(pGTASA + 0x678630, (uintptr_t*)&CTimeCycle__m_vRedGrade, sizeof(void*));
    WritePtr(pGTASA + 0x679B58, (uintptr_t*)&CTimeCycle__m_vGreenGrade, sizeof(void*));
    WritePtr(pGTASA + 0x677E70, (uintptr_t*)&CTimeCycle__m_vBlueGrade, sizeof(void*));
}