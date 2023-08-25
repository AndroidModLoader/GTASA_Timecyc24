#ifdef AML32
    #include "GTASA_STRUCTS.h"
    #define BYVER(__for32, __for64) (__for32)
#else
    #include "GTASA_STRUCTS_210.h"
    #define BYVER(__for32, __for64) (__for64)
#endif
#define sizeofA(__aVar)  ((int)(sizeof(__aVar)/sizeof(__aVar[0])))

#define NUMWEATHERS 23
#define NUMHOURS 24

extern uint8_t  CTimeCycle__m_nDirectionalMult[NUMHOURS][NUMWEATHERS];
extern uint8_t  CTimeCycle__m_nWaterFogAlpha[NUMHOURS][NUMWEATHERS];
extern uint8_t  CTimeCycle__m_nHighLightMinIntensity[NUMHOURS][NUMWEATHERS];
extern uint8_t  CTimeCycle__m_fCloudAlpha[NUMHOURS][NUMWEATHERS];
extern uint8_t  CTimeCycle__m_fPostFx2Alpha[NUMHOURS][NUMWEATHERS];
extern uint8_t  CTimeCycle__m_fPostFx2Blue[NUMHOURS][NUMWEATHERS];
extern uint8_t  CTimeCycle__m_fPostFx2Green[NUMHOURS][NUMWEATHERS];
extern uint8_t  CTimeCycle__m_fPostFx2Red[NUMHOURS][NUMWEATHERS];
extern uint8_t  CTimeCycle__m_fPostFx1Alpha[NUMHOURS][NUMWEATHERS];
extern uint8_t  CTimeCycle__m_fPostFx1Blue[NUMHOURS][NUMWEATHERS];
extern uint8_t  CTimeCycle__m_fPostFx1Green[NUMHOURS][NUMWEATHERS];
extern uint8_t  CTimeCycle__m_fPostFx1Red[NUMHOURS][NUMWEATHERS];
extern uint8_t  CTimeCycle__m_fWaterAlpha[NUMHOURS][NUMWEATHERS];
extern uint8_t  CTimeCycle__m_fWaterBlue[NUMHOURS][NUMWEATHERS];
extern uint8_t  CTimeCycle__m_fWaterGreen[NUMHOURS][NUMWEATHERS];
extern uint8_t  CTimeCycle__m_fWaterRed[NUMHOURS][NUMWEATHERS];
extern uint8_t  CTimeCycle__m_nFluffyCloudsBottomRed[NUMHOURS][NUMWEATHERS];
extern uint8_t  CTimeCycle__m_nFluffyCloudsBottomGreen[NUMHOURS][NUMWEATHERS];
extern uint8_t  CTimeCycle__m_nFluffyCloudsBottomBlue[NUMHOURS][NUMWEATHERS];
extern uint8_t  CTimeCycle__m_nLowCloudsBlue[NUMHOURS][NUMWEATHERS];
extern uint8_t  CTimeCycle__m_nLowCloudsGreen[NUMHOURS][NUMWEATHERS];
extern uint8_t  CTimeCycle__m_nLowCloudsRed[NUMHOURS][NUMWEATHERS];
extern uint8_t  CTimeCycle__m_fLightsOnGroundBrightness[NUMHOURS][NUMWEATHERS];
extern uint16_t CTimeCycle__m_fFogStart[NUMHOURS][NUMWEATHERS];
extern uint16_t CTimeCycle__m_fFarClip[NUMHOURS][NUMWEATHERS];
extern uint8_t  CTimeCycle__m_nPoleShadowStrength[NUMHOURS][NUMWEATHERS];
extern uint8_t  CTimeCycle__m_nLightShadowStrength[NUMHOURS][NUMWEATHERS];
extern uint8_t  CTimeCycle__m_nShadowStrength[NUMHOURS][NUMWEATHERS];
extern uint8_t  CTimeCycle__m_fSpriteBrightness[NUMHOURS][NUMWEATHERS];
extern uint8_t  CTimeCycle__m_fSpriteSize[NUMHOURS][NUMWEATHERS];
extern uint8_t  CTimeCycle__m_fSunSize[NUMHOURS][NUMWEATHERS];
extern uint8_t  CTimeCycle__m_nSunCoronaBlue[NUMHOURS][NUMWEATHERS];
extern uint8_t  CTimeCycle__m_nSunCoronaGreen[NUMHOURS][NUMWEATHERS];
extern uint8_t  CTimeCycle__m_nSunCoronaRed[NUMHOURS][NUMWEATHERS];
extern uint8_t  CTimeCycle__m_nSunCoreBlue[NUMHOURS][NUMWEATHERS];
extern uint8_t  CTimeCycle__m_nSunCoreGreen[NUMHOURS][NUMWEATHERS];
extern uint8_t  CTimeCycle__m_nSunCoreRed[NUMHOURS][NUMWEATHERS];
extern uint8_t  CTimeCycle__m_nSkyBottomBlue[NUMHOURS][NUMWEATHERS];
extern uint8_t  CTimeCycle__m_nSkyBottomGreen[NUMHOURS][NUMWEATHERS];
extern uint8_t  CTimeCycle__m_nSkyBottomRed[NUMHOURS][NUMWEATHERS];
extern uint8_t  CTimeCycle__m_nSkyTopBlue[NUMHOURS][NUMWEATHERS];
extern uint8_t  CTimeCycle__m_nSkyTopGreen[NUMHOURS][NUMWEATHERS];
extern uint8_t  CTimeCycle__m_nSkyTopRed[NUMHOURS][NUMWEATHERS];
extern uint8_t  CTimeCycle__m_nAmbientBlue_Obj[NUMHOURS][NUMWEATHERS];
extern uint8_t  CTimeCycle__m_nAmbientGreen_Obj[NUMHOURS][NUMWEATHERS];
extern uint8_t  CTimeCycle__m_nAmbientRed_Obj[NUMHOURS][NUMWEATHERS];
extern uint8_t  CTimeCycle__m_nAmbientBlue[NUMHOURS][NUMWEATHERS];
extern uint8_t  CTimeCycle__m_nAmbientGreen[NUMHOURS][NUMWEATHERS];
extern uint8_t  CTimeCycle__m_nAmbientRed[NUMHOURS][NUMWEATHERS];
// Mobile thingies
extern RQVector CTimeCycle__m_vRedGrade[NUMHOURS][NUMWEATHERS];
extern RQVector CTimeCycle__m_vGreenGrade[NUMHOURS][NUMWEATHERS];
extern RQVector CTimeCycle__m_vBlueGrade[NUMHOURS][NUMWEATHERS];

extern CColourSet* m_CurrentColours;

void PatchTheValues();