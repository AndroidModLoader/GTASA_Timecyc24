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

CColourSet* m_CurrentColours;
extern uintptr_t pGTASA;

inline void WritePtr(uintptr_t addr, uintptr_t* ptrAddr, size_t len)
{
    uintptr_t* ptr = ptrAddr;
    aml->Write(addr, (uintptr_t)&ptr, len);
}
void PatchTheValues()
{
    WritePtr(pGTASA + BYBIT(0x678CE4, 0x84F9F0), (uintptr_t*)&CTimeCycle__m_nDirectionalMult, sizeof(void*));
    WritePtr(pGTASA + BYBIT(0x678F74, 0x84FF10), (uintptr_t*)&CTimeCycle__m_nWaterFogAlpha, sizeof(void*));
    WritePtr(pGTASA + BYBIT(0x67619C, 0x84A398), (uintptr_t*)&CTimeCycle__m_nHighLightMinIntensity, sizeof(void*));
    WritePtr(pGTASA + BYBIT(0x676CF8, 0x84BA38), (uintptr_t*)&CTimeCycle__m_fCloudAlpha, sizeof(void*));
    WritePtr(pGTASA + BYBIT(0x679BE8, 0x8517E8), (uintptr_t*)&CTimeCycle__m_fPostFx2Alpha, sizeof(void*));
    WritePtr(pGTASA + BYBIT(0x67988C, 0x851130), (uintptr_t*)&CTimeCycle__m_fPostFx2Blue, sizeof(void*));
    WritePtr(pGTASA + BYBIT(0x6769D4, 0x84B3F0), (uintptr_t*)&CTimeCycle__m_fPostFx2Green, sizeof(void*));
    WritePtr(pGTASA + BYBIT(0x678D80, 0x84FB28), (uintptr_t*)&CTimeCycle__m_fPostFx2Red, sizeof(void*));
    WritePtr(pGTASA + BYBIT(0x6763E4, 0x84A820), (uintptr_t*)&CTimeCycle__m_fPostFx1Alpha, sizeof(void*));
    WritePtr(pGTASA + BYBIT(0x679420, 0x850858), (uintptr_t*)&CTimeCycle__m_fPostFx1Blue, sizeof(void*));
    WritePtr(pGTASA + BYBIT(0x676744, 0x84AEE0), (uintptr_t*)&CTimeCycle__m_fPostFx1Green, sizeof(void*));
    WritePtr(pGTASA + BYBIT(0x676574, 0x84AB40), (uintptr_t*)&CTimeCycle__m_fPostFx1Red, sizeof(void*));
    WritePtr(pGTASA + BYBIT(0x6766C0, 0x84ADD8), (uintptr_t*)&CTimeCycle__m_fWaterAlpha, sizeof(void*));
    WritePtr(pGTASA + BYBIT(0x676A98, 0x84B578), (uintptr_t*)&CTimeCycle__m_fWaterBlue, sizeof(void*));
    WritePtr(pGTASA + BYBIT(0x6788FC, 0x84F218), (uintptr_t*)&CTimeCycle__m_fWaterGreen, sizeof(void*));
    WritePtr(pGTASA + BYBIT(0x679314, 0x850640), (uintptr_t*)&CTimeCycle__m_fWaterRed, sizeof(void*));
    WritePtr(pGTASA + BYBIT(0x678294, 0x84E540), (uintptr_t*)&CTimeCycle__m_nFluffyCloudsBottomBlue, sizeof(void*));
    WritePtr(pGTASA + BYBIT(0x678BD4, 0x84F7D0), (uintptr_t*)&CTimeCycle__m_nFluffyCloudsBottomGreen, sizeof(void*));
    WritePtr(pGTASA + BYBIT(0x679E50, 0x851CB0), (uintptr_t*)&CTimeCycle__m_nFluffyCloudsBottomRed, sizeof(void*));
    WritePtr(pGTASA + BYBIT(0x6784B8, 0x84E988), (uintptr_t*)&CTimeCycle__m_nLowCloudsBlue, sizeof(void*));
    WritePtr(pGTASA + BYBIT(0x678A7C, 0x84F520), (uintptr_t*)&CTimeCycle__m_nLowCloudsGreen, sizeof(void*));
    WritePtr(pGTASA + BYBIT(0x6796B8, 0x850D80), (uintptr_t*)&CTimeCycle__m_nLowCloudsRed, sizeof(void*));
    WritePtr(pGTASA + BYBIT(0x67979C, 0x850F40), (uintptr_t*)&CTimeCycle__m_fLightsOnGroundBrightness, sizeof(void*));
    WritePtr(pGTASA + BYBIT(0x679AD8, 0x8515D0), (uintptr_t*)&CTimeCycle__m_fFogStart, sizeof(void*));
    WritePtr(pGTASA + BYBIT(0x6761A4, 0x84A3A8), (uintptr_t*)&CTimeCycle__m_fFarClip, sizeof(void*));
    WritePtr(pGTASA + BYBIT(0x6760FC, 0x84A258), (uintptr_t*)&CTimeCycle__m_nPoleShadowStrength, sizeof(void*));
    WritePtr(pGTASA + BYBIT(0x678298, 0x84E548), (uintptr_t*)&CTimeCycle__m_nLightShadowStrength, sizeof(void*));
    WritePtr(pGTASA + BYBIT(0x676434, 0x84A8C0), (uintptr_t*)&CTimeCycle__m_nShadowStrength, sizeof(void*));
    WritePtr(pGTASA + BYBIT(0x679280, 0x850518), (uintptr_t*)&CTimeCycle__m_fSpriteBrightness, sizeof(void*));
    WritePtr(pGTASA + BYBIT(0x676F50, 0x84BEE0), (uintptr_t*)&CTimeCycle__m_fSpriteSize, sizeof(void*));
    WritePtr(pGTASA + BYBIT(0x679744, 0x850E90), (uintptr_t*)&CTimeCycle__m_fSunSize, sizeof(void*));
    WritePtr(pGTASA + BYBIT(0x678518, 0x84EA48), (uintptr_t*)&CTimeCycle__m_nSunCoronaBlue, sizeof(void*));
    WritePtr(pGTASA + BYBIT(0x6785F4, 0x84EC00), (uintptr_t*)&CTimeCycle__m_nSunCoronaGreen, sizeof(void*));
    WritePtr(pGTASA + BYBIT(0x677734, 0x84CE90), (uintptr_t*)&CTimeCycle__m_nSunCoronaRed, sizeof(void*));
    WritePtr(pGTASA + BYBIT(0x67621C, 0x84A498), (uintptr_t*)&CTimeCycle__m_nSunCoreBlue, sizeof(void*));
    WritePtr(pGTASA + BYBIT(0x679014, 0x850050), (uintptr_t*)&CTimeCycle__m_nSunCoreGreen, sizeof(void*));
    WritePtr(pGTASA + BYBIT(0x67858C, 0x84EB30), (uintptr_t*)&CTimeCycle__m_nSunCoreRed, sizeof(void*));
    WritePtr(pGTASA + BYBIT(0x6799A8, 0x851368), (uintptr_t*)&CTimeCycle__m_nSkyBottomBlue, sizeof(void*));
    WritePtr(pGTASA + BYBIT(0x678F70, 0x84FF08), (uintptr_t*)&CTimeCycle__m_nSkyBottomGreen, sizeof(void*));
    WritePtr(pGTASA + BYBIT(0x677D28, 0x84DA70), (uintptr_t*)&CTimeCycle__m_nSkyBottomRed, sizeof(void*));
    WritePtr(pGTASA + BYBIT(0x677D48, 0x84DAB0), (uintptr_t*)&CTimeCycle__m_nSkyTopBlue, sizeof(void*));
    WritePtr(pGTASA + BYBIT(0x679E38, 0x851C80), (uintptr_t*)&CTimeCycle__m_nSkyTopGreen, sizeof(void*));
    WritePtr(pGTASA + BYBIT(0x679E90, 0x851D30), (uintptr_t*)&CTimeCycle__m_nSkyTopRed, sizeof(void*));
    WritePtr(pGTASA + BYBIT(0x67696C, 0x84B328), (uintptr_t*)&CTimeCycle__m_nAmbientBlue_Obj, sizeof(void*));
    WritePtr(pGTASA + BYBIT(0x675FF4, 0x84A058), (uintptr_t*)&CTimeCycle__m_nAmbientGreen_Obj, sizeof(void*));
    WritePtr(pGTASA + BYBIT(0x6791AC, 0x850378), (uintptr_t*)&CTimeCycle__m_nAmbientRed_Obj, sizeof(void*));
    WritePtr(pGTASA + BYBIT(0x676AFC, 0x84B640), (uintptr_t*)&CTimeCycle__m_nAmbientBlue, sizeof(void*));
    WritePtr(pGTASA + BYBIT(0x678E80, 0x84FD28), (uintptr_t*)&CTimeCycle__m_nAmbientGreen, sizeof(void*));
    WritePtr(pGTASA + BYBIT(0x677EDC, 0x84DDD8), (uintptr_t*)&CTimeCycle__m_nAmbientRed, sizeof(void*));
// Mobile thingies
    WritePtr(pGTASA + BYBIT(0x678630, 0x84EC88), (uintptr_t*)&CTimeCycle__m_vRedGrade, sizeof(void*));
    WritePtr(pGTASA + BYBIT(0x679B58, 0x8516D0), (uintptr_t*)&CTimeCycle__m_vGreenGrade, sizeof(void*));
    WritePtr(pGTASA + BYBIT(0x677E70, 0x84DD00), (uintptr_t*)&CTimeCycle__m_vBlueGrade, sizeof(void*));
}