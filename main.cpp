#include <mod/amlmod.h>
#include <mod/logger.h>

#include "timecyc.h"

MYMOD(net.rusjj.timecyc24, GTASA Timecyc24, 1.0, GTAmodding & RusJJ)
BEGIN_DEPLIST()
    ADD_DEPENDENCY_VER(net.rusjj.aml, 1.0.2.1)
END_DEPLIST()

/////////////////////////////////////////////////////////////////////////////
///////////////////////////////     Saves     ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////
uintptr_t pGTASA;
void* hGTASA;

uint8_t aTimecycleHours[NUMHOURS+1] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24 };
uint8_t aTimecycleHorizon[NUMHOURS] = { 30, 30, 30, 30, 30, 30, 30, 50, 52, 54, 56, 58, 60, 60, 60, 60, 60, 60, 60, 60, 50, 42, 35, 32 };

/////////////////////////////////////////////////////////////////////////////
///////////////////////////////     Vars      ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////
float *ms_fTimeStep, *WeatherInterpolationValue, *WeatherUnderWaterness, *WeatherInTunnelness, *CoronasLightsMult,
      *m_BrightnessAddedToAmbientRed, *m_BrightnessAddedToAmbientGreen, *m_BrightnessAddedToAmbientBlue, *m_ExtraColourInter,
      *m_fShadowFrontX, *m_fShadowFrontY, *m_fShadowSideX, *m_fShadowSideY, *m_fShadowDisplacementX, *m_fShadowDisplacementY;
int16_t *OldWeatherType, *NewWeatherType;
uint16_t *ms_nGameClockSeconds;
uint8_t *ms_nGameClockMinutes, *ms_nGameClockHours;
bool *bScriptsForceRain, *ms_runningCutscene, *m_bNightVision, *m_bInfraredVision;
int32_t *m_ExtraColourWeatherType, *m_ExtraColour, *m_bExtraColourOn, *m_FogReduction, *m_CurrentStoredValue;
CVector *m_vecDirnLightToSun, *m_VectorToSun;
RwRGBA *m_BelowHorizonGrey;
CCamera *TheCamera;
int32_t *BrightnessPercent;

/////////////////////////////////////////////////////////////////////////////
///////////////////////////////     Funcs     ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void (*CFileMgr__SetDir)(const char *dir);
int (*CFileMgr__OpenFile)(const char *path, const char *mode);
void (*CFileMgr__CloseFile)(int fd);
char* (*CFileLoader__LoadLine)(int fd);
bool (*CCullZones__PlayerNoRain)();
bool (*CCullZones__CamNoRain)();
bool (*CPostEffects__IsVisionFXActive)();
void (*CShadows__CalcPedShadowValues)(CVector pos, float *sfx, float *sfy, float *ssx, float *ssy, float *sdx, float *sdy);
void (*CTimeCycle__FindTimeCycleBox)(CVector pos, CTimeCycleBox **box, float *interp, bool checkLod, bool checkFar, CTimeCycleBox *exclude);

/////////////////////////////////////////////////////////////////////////////
//////////////////////////////     Patches     //////////////////////////////
/////////////////////////////////////////////////////////////////////////////
uintptr_t ModuloPatch_BackTo;
extern "C" int ModuloPatch_patch(int __fillerVar, int unmoduledVal)
{
    return (unmoduledVal % NUMHOURS);
}
__attribute__((optnone)) __attribute__((naked)) void ModuloPatch_inject(void)
{
    /*asm volatile(
        "SUBS            R0, R1, R0\n"
        "MOV.W           R1, R10,ASR#31\n"
        "VCVT.F32.U32    S2, S2\n"
        "ADD.W           R1, R10, R1,LSR#29\n"
    );
    asm volatile(
        "PUSH {R0,R2-R11}\n"
        "MOV R0, R1\n"
        "BL ModuloPatch_patch\n"
        "MOV R1, R0\n");*/
    asm volatile(
        "SUBS            R0, R1, R0\n"
        "MOV             R1, R10\n"
        "VCVT.F32.U32    S2, S2\n"
    );
    asm volatile(
        "PUSH            {R0,R2-R11}\n"
        "BL              ModuloPatch_patch\n"
        "MOV             R1, R0\n");
    asm volatile(
        "MOV R12, %0\n"
        "POP {R0,R2-R11}\n"
        "BX R12\n"
    :: "r" (ModuloPatch_BackTo));
}

uintptr_t HorizontAngles_BackTo;
__attribute__((optnone)) __attribute__((naked)) void HorizontAngles_inject(void)
{
    asm volatile(
        "ADD.W R12, R11, #0x18\n");
    asm volatile(
        "MOV R0, %0\n"
    :: "r" (&aTimecycleHorizon));
    asm volatile(
        "LDRB R1, [R0,R1]\n"
        "ADD R0, R10\n"
        "LDRB R0, [R0,#-1]\n"
        "VMOV S0, R1\n"
        "PUSH {R0}\n");
    asm volatile(
        "MOV R1, %0\n"
        "POP {R0}\n"
        "BX R1\n"
    :: "r" (HorizontAngles_BackTo));
}

uintptr_t Hours_BackTo;
__attribute__((optnone)) __attribute__((naked)) void Hours_inject(void)
{
    asm volatile(
        "VCVT.F32.U32 S4, S4\n"
        "VADD.F32 S0, S0, S4\n");
    asm volatile(
        "MOV R0, %0\n"
    :: "r" (&aTimecycleHours));
    asm volatile(
        "PUSH {R0}\n");
    asm volatile(
        "MOV R1, %0\n"
        "POP {R0}\n"
        "BX R1\n"
    :: "r" (Hours_BackTo));
}

/////////////////////////////////////////////////////////////////////////////
///////////////////////////////     Hooks     ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////
DECL_HOOKv(CColourSet_ctor, CColourSet* self, int h, int w)
{
    self->ambr = CTimeCycle__m_nAmbientRed[h][w];
    self->ambg = CTimeCycle__m_nAmbientGreen[h][w];
    self->ambb = CTimeCycle__m_nAmbientBlue[h][w];
    self->ambobjr = CTimeCycle__m_nAmbientRed_Obj[h][w];
    self->ambobjg = CTimeCycle__m_nAmbientGreen_Obj[h][w];
    self->ambobjb = CTimeCycle__m_nAmbientBlue_Obj[h][w];
    self->skytopr = CTimeCycle__m_nSkyTopRed[h][w];
    self->skytopg = CTimeCycle__m_nSkyTopGreen[h][w];
    self->skytopb = CTimeCycle__m_nSkyTopBlue[h][w];
    self->skybotr = CTimeCycle__m_nSkyBottomRed[h][w];
    self->skybotg = CTimeCycle__m_nSkyBottomGreen[h][w];
    self->skybotb = CTimeCycle__m_nSkyBottomBlue[h][w];
    self->suncorer = CTimeCycle__m_nSunCoreRed[h][w];
    self->suncoreg = CTimeCycle__m_nSunCoreGreen[h][w];
    self->suncoreb = CTimeCycle__m_nSunCoreBlue[h][w];
    self->suncoronar = CTimeCycle__m_nSunCoronaRed[h][w];
    self->suncoronag = CTimeCycle__m_nSunCoronaGreen[h][w];
    self->suncoronab = CTimeCycle__m_nSunCoronaBlue[h][w];
    self->sunsz = CTimeCycle__m_fSunSize[h][w];
    self->sprsz = CTimeCycle__m_fSpriteSize[h][w];
    self->sprbght = CTimeCycle__m_fSpriteBrightness[h][w];
    self->shd = CTimeCycle__m_nShadowStrength[h][w];
    self->lightshd = CTimeCycle__m_nLightShadowStrength[h][w];
    self->poleshd = CTimeCycle__m_nPoleShadowStrength[h][w];
    self->farclp = CTimeCycle__m_fFarClip[h][w];
    self->fogst = CTimeCycle__m_fFogStart[h][w];
    self->lightonground = CTimeCycle__m_fLightsOnGroundBrightness[h][w];
    self->lowcloudr = CTimeCycle__m_nLowCloudsRed[h][w];
    self->lowcloudg = CTimeCycle__m_nLowCloudsGreen[h][w];
    self->lowcloudb = CTimeCycle__m_nLowCloudsBlue[h][w];
    self->fluffycloudr = CTimeCycle__m_nFluffyCloudsBottomRed[h][w];
    self->fluffycloudg = CTimeCycle__m_nFluffyCloudsBottomGreen[h][w];
    self->fluffycloudb = CTimeCycle__m_nFluffyCloudsBottomBlue[h][w];
    self->waterr = CTimeCycle__m_fWaterRed[h][w];
    self->waterg = CTimeCycle__m_fWaterGreen[h][w];
    self->waterb = CTimeCycle__m_fWaterBlue[h][w];
    self->watera = CTimeCycle__m_fWaterAlpha[h][w];
    self->postfx1r = CTimeCycle__m_fPostFx1Red[h][w];
    self->postfx1g = CTimeCycle__m_fPostFx1Green[h][w];
    self->postfx1b = CTimeCycle__m_fPostFx1Blue[h][w];
    self->postfx1a = CTimeCycle__m_fPostFx1Alpha[h][w];
    self->postfx2r = CTimeCycle__m_fPostFx2Red[h][w];
    self->postfx2g = CTimeCycle__m_fPostFx2Green[h][w];
    self->postfx2b = CTimeCycle__m_fPostFx2Blue[h][w];
    self->postfx2a = CTimeCycle__m_fPostFx2Alpha[h][w];
    self->cloudalpha = CTimeCycle__m_fCloudAlpha[h][w];
    self->intensityLimit = CTimeCycle__m_nHighLightMinIntensity[h][w];
    self->waterfogalpha = CTimeCycle__m_nWaterFogAlpha[h][w];
    self->lodDistMult = 1.0f;
    self->directionalmult = (float)(CTimeCycle__m_nDirectionalMult[h][w]) / 100.0f;
}
DECL_HOOKv(CColourSet_Interpolate, CColourSet *self, CColourSet *a, CColourSet *b, float fa, float fb, bool ignoreSky)
{
    if(!ignoreSky)
    {
        self->skytopr = a->skytopr * fa + b->skytopr * fb;
        self->skytopg = a->skytopg * fa + b->skytopg * fb;
        self->skytopb = a->skytopb * fa + b->skytopb * fb;
        self->skybotr = a->skybotr * fa + b->skybotr * fb;
        self->skybotg = a->skybotg * fa + b->skybotg * fb;
        self->skybotb = a->skybotb * fa + b->skybotb * fb;
        self->suncorer = a->suncorer * fa + b->suncorer * fb;
        self->suncoreg = a->suncoreg * fa + b->suncoreg * fb;
        self->suncoreb = a->suncoreb * fa + b->suncoreb * fb;
        self->suncoronar = a->suncoronar * fa + b->suncoronar * fb;
        self->suncoronag = a->suncoronag * fa + b->suncoronag * fb;
        self->suncoronab = a->suncoronab * fa + b->suncoronab * fb;
        self->sunsz = fa * a->sunsz + fb * b->sunsz;
        self->lowcloudr = a->lowcloudr * fa + b->lowcloudr * fb;
        self->lowcloudg = a->lowcloudg * fa + b->lowcloudg * fb;
        self->lowcloudb = a->lowcloudb * fa + b->lowcloudb * fb;
        self->fluffycloudr = a->fluffycloudr * fa + b->fluffycloudr * fb;
        self->fluffycloudg = a->fluffycloudg * fa + b->fluffycloudg * fb;
        self->fluffycloudb = a->fluffycloudb * fa + b->fluffycloudb * fb;
    }
    self->ambr = fa * a->ambr + fb * b->ambr;
    self->ambg = fa * a->ambg + fb * b->ambg;
    self->ambb = fa * a->ambb + fb * b->ambb;
    self->ambobjr = fa * a->ambobjr + fb * b->ambobjr;
    self->ambobjg = fa * a->ambobjg + fb * b->ambobjg;
    self->ambobjb = fa * a->ambobjb + fb * b->ambobjb;
    self->sprsz = fa * a->sprsz + fb * b->sprsz;
    self->sprbght = fa * a->sprbght + fb * b->sprbght;
    self->shd = a->shd * fa + b->shd * fb;
    self->lightshd = a->lightshd * fa + b->lightshd * fb;
    self->poleshd = a->poleshd * fa + b->poleshd * fb;
    self->farclp = fa * a->farclp + fb * b->farclp;
    self->fogst = fa * a->fogst + fb * b->fogst;
    self->postfx1r = fa * a->postfx1r + fb * b->postfx1r;
    self->postfx1g = fa * a->postfx1g + fb * b->postfx1g;
    self->postfx1b = fa * a->postfx1b + fb * b->postfx1b;
    self->postfx1a = fa * a->postfx1a + fb * b->postfx1a;
    self->postfx2r = fa * a->postfx2r + fb * b->postfx2r;
    self->postfx2g = fa * a->postfx2g + fb * b->postfx2g;
    self->postfx2b = fa * a->postfx2b + fb * b->postfx2b;
    self->postfx2a = fa * a->postfx2a + fb * b->postfx2a;
    self->lightonground = fa * a->lightonground + fb * b->lightonground;
    self->cloudalpha = fa * a->cloudalpha + fb * b->cloudalpha;
    self->intensityLimit = a->intensityLimit * fa + b->intensityLimit * fb;
    self->waterfogalpha = a->waterfogalpha * fa + b->waterfogalpha * fb;
    self->directionalmult = fa * a->directionalmult + fb * b->directionalmult;
    self->lodDistMult = fa * a->lodDistMult + fb * b->lodDistMult;
    self->waterr = fa * a->waterr + fb * b->waterr;
    self->waterg = fa * a->waterg + fb * b->waterg;
    self->waterb = fa * a->waterb + fb * b->waterb;
    self->watera = fa * a->watera + fb * b->watera;
}
DECL_HOOKv(SetConstantParametersForPostFX)
{
    if(!CPostEffects__IsVisionFXActive()) return;
    if(*m_bNightVision)
    {
        m_CurrentColours.shd = 0;
        m_CurrentColours.lightshd = 0;
        m_CurrentColours.poleshd = 0;
        m_CurrentColours.ambr = 0.0;
        m_CurrentColours.ambg = 0.4;
        m_CurrentColours.ambb = 0.0;
        m_CurrentColours.ambobjr = 0.0;
        m_CurrentColours.ambobjg = 0.4;
        m_CurrentColours.ambobjb = 0.0;
        m_CurrentColours.skytopr = 0;
        m_CurrentColours.skytopg = 128;
        m_CurrentColours.skytopb = 0;
        m_CurrentColours.skybotr = 0;
        m_CurrentColours.skybotg = 128;
        m_CurrentColours.skybotb = 0;
    }
    if(*m_bInfraredVision)
    {
        m_CurrentColours.shd = 0;
        m_CurrentColours.lightshd = 0;
        m_CurrentColours.poleshd = 0;
        m_CurrentColours.lightonground = 0;
        m_CurrentColours.intensityLimit = 0;
        m_CurrentColours.waterfogalpha = 0;
        m_CurrentColours.ambr = 0.0;
        m_CurrentColours.ambg = 0.0;
        m_CurrentColours.ambb = 1.0;
        m_CurrentColours.ambobjr = 0.0;
        m_CurrentColours.ambobjg = 0.0;
        m_CurrentColours.ambobjb = 1.0;
        m_CurrentColours.skytopr = 0;
        m_CurrentColours.skytopg = 0;
        m_CurrentColours.skytopb = 128;
        m_CurrentColours.skybotr = 0;
        m_CurrentColours.skybotg = 0;
        m_CurrentColours.skybotb = 128;
    }
}
DECL_HOOKv(StartExtraColour, int extracolor, bool keepInter)
{
    *m_ExtraColourWeatherType = (float)(extracolor) / NUMHOURS + WEATHER_EXTRA_START;
    *m_ExtraColour = extracolor % NUMHOURS;
    *m_bExtraColourOn = 1;
    if(keepInter) *m_ExtraColourInter = 0.0f;
    else *m_ExtraColourInter = 1.0f;
}
DECL_HOOKv(TimecycInit, bool __unused)
{
    int fd, w, h;
    char *line;

    /* Timecycle Section */
    
    int ambr, ambg, ambb;
    int ambobjr, ambobjg, ambobjb;
    int dirr, dirg, dirb;
    int skytopr, skytopg, skytopb;
    int skybotr, skybotg, skybotb;
    int suncorer, suncoreg, suncoreb;
    int suncoronar, suncoronag, suncoronab;
    float sunsz, sprsz, sprbght;
    int shdw, lightshd, poleshd;
    float farclp, fogst, lightonground;
    int lowcloudr, lowcloudg, lowcloudb;
    int bottomcloudr, bottomcloudg, bottomcloudb;
    float waterr, waterg, waterb, watera;
    float postfx1a, postfx1r, postfx1g, postfx1b;
    float postfx2a, postfx2r, postfx2g, postfx2b;
    float cloudalpha;
    int radlimit;
    int waterfogalpha;
    float dirMult;
    
    CFileMgr__SetDir("DATA");
    fd = CFileMgr__OpenFile("TIMECYC_24H.DAT", "rb");
    CFileMgr__SetDir("");
    
    for(w = 0; w < NUMWEATHERS; ++w)
    {
        for(h = 0; h < NUMHOURS; ++h)
        {
            while(line = CFileLoader__LoadLine(fd), line) if(line[0] != '/' && line[0] != '\0') break;
            sscanf(line, "%d %d %d" " %d %d %d" " %d %d %d" " %d %d %d" " %d %d %d" " %d %d %d" " %d %d %d"
                " %f %f %f"
                " %d %d %d"
                " %f %f %f"
                " %d %d %d" " %d %d %d"
                " %f %f %f %f"
                "  %f %f %f %f"
                "  %f %f %f %f"
                " %f %d %d %f",
                &ambr, &ambg, &ambb,
                &ambobjr, &ambobjg, &ambobjb,
                &dirr, &dirg, &dirb,
                &skytopr, &skytopg, &skytopb,
                &skybotr, &skybotg, &skybotb,
                &suncorer, &suncoreg, &suncoreb,
                &suncoronar, &suncoronag, &suncoronab,
                &sunsz, &sprsz, &sprbght,
                &shdw, &lightshd, &poleshd,
                &farclp, &fogst, &lightonground,
                &lowcloudr, &lowcloudg, &lowcloudb,
                &bottomcloudr, &bottomcloudg, &bottomcloudb,
                &waterr, &waterg, &waterb, &watera,
                &postfx1a, &postfx1r, &postfx1g, &postfx1b,
                &postfx2a, &postfx2r, &postfx2g, &postfx2b,
                &cloudalpha,
                &radlimit,
                &waterfogalpha,
                &dirMult);
            CTimeCycle__m_nAmbientRed[h][w] = ambr;
            CTimeCycle__m_nAmbientGreen[h][w] = ambg;
            CTimeCycle__m_nAmbientBlue[h][w] = ambb;
            CTimeCycle__m_nAmbientRed_Obj[h][w] = ambobjr;
            CTimeCycle__m_nAmbientGreen_Obj[h][w] = ambobjg;
            CTimeCycle__m_nAmbientBlue_Obj[h][w] = ambobjb;
            CTimeCycle__m_nSkyTopRed[h][w] = skytopr;
            CTimeCycle__m_nSkyTopGreen[h][w] = skytopg;
            CTimeCycle__m_nSkyTopBlue[h][w] = skytopb;
            CTimeCycle__m_nSkyBottomRed[h][w] = skybotr;
            CTimeCycle__m_nSkyBottomGreen[h][w] = skybotg;
            CTimeCycle__m_nSkyBottomBlue[h][w] = skybotb;
            CTimeCycle__m_nSunCoreRed[h][w] = suncorer;
            CTimeCycle__m_nSunCoreGreen[h][w] = suncoreg;
            CTimeCycle__m_nSunCoreBlue[h][w] = suncoreb;
            CTimeCycle__m_nSunCoronaRed[h][w] = suncoronar;
            CTimeCycle__m_nSunCoronaGreen[h][w] = suncoronag;
            CTimeCycle__m_nSunCoronaBlue[h][w] = suncoronab;
            CTimeCycle__m_fSunSize[h][w] = sunsz*10.0f + 0.5f;
            CTimeCycle__m_fSpriteSize[h][w] = sprsz*10.0f + 0.5f;
            CTimeCycle__m_fSpriteBrightness[h][w] = sprbght*10.0f + 0.5f;
            CTimeCycle__m_nShadowStrength[h][w] = shdw;
            CTimeCycle__m_nLightShadowStrength[h][w] = lightshd;
            CTimeCycle__m_nPoleShadowStrength[h][w] = poleshd;
            CTimeCycle__m_fFarClip[h][w] = farclp;
            CTimeCycle__m_fFogStart[h][w] = fogst;
            CTimeCycle__m_fLightsOnGroundBrightness[h][w] = lightonground*10.0f + 0.5f;
            CTimeCycle__m_nLowCloudsRed[h][w] = lowcloudr;
            CTimeCycle__m_nLowCloudsGreen[h][w] = lowcloudg;
            CTimeCycle__m_nLowCloudsBlue[h][w] = lowcloudb;
            CTimeCycle__m_nFluffyCloudsBottomRed[h][w] = bottomcloudr;
            CTimeCycle__m_nFluffyCloudsBottomGreen[h][w] = bottomcloudg;
            CTimeCycle__m_nFluffyCloudsBottomBlue[h][w] = bottomcloudb;
            CTimeCycle__m_fWaterRed[h][w] = waterr;
            CTimeCycle__m_fWaterGreen[h][w] = waterg;
            CTimeCycle__m_fWaterBlue[h][w] = waterb;
            CTimeCycle__m_fWaterAlpha[h][w] = watera;
            CTimeCycle__m_fPostFx1Red[h][w] = postfx1r;
            CTimeCycle__m_fPostFx1Green[h][w] = postfx1g;
            CTimeCycle__m_fPostFx1Blue[h][w] = postfx1b;
            CTimeCycle__m_fPostFx2Red[h][w] = postfx2r;
            CTimeCycle__m_fPostFx2Green[h][w] = postfx2g;
            CTimeCycle__m_fPostFx2Blue[h][w] = postfx2b;
            CTimeCycle__m_fPostFx1Alpha[h][w] = postfx1a + postfx1a;
            CTimeCycle__m_fPostFx2Alpha[h][w] = postfx2a + postfx2a;
            CTimeCycle__m_fCloudAlpha[h][w] = cloudalpha;
            CTimeCycle__m_nHighLightMinIntensity[h][w] = radlimit;
            CTimeCycle__m_nWaterFogAlpha[h][w] = waterfogalpha;
            CTimeCycle__m_nDirectionalMult[h][w] = dirMult*100.0f;
        }
    }
    CFileMgr__CloseFile(fd);

    /* Colorcycle Section */

    RQVector redGradeNewVal, greenGradeNewVal, blueGradeNewVal;

    CFileMgr__SetDir("DATA");
    fd = CFileMgr__OpenFile("COLORCYCLE_24H.DAT", "rb");
    CFileMgr__SetDir("");
    
    for(w = 0; w < NUMWEATHERS; ++w)
    {
        for(h = 0; h < NUMHOURS; ++h)
        {
            while(line = CFileLoader__LoadLine(fd), line) if(line[0] != '/' && line[0] != '\0') break;
            sscanf(line, "%f %f %f %f %f %f %f %f %f %f %f %f",
                &redGradeNewVal.r, &redGradeNewVal.g, &redGradeNewVal.b, &redGradeNewVal.a,
                &greenGradeNewVal.r, &greenGradeNewVal.g, &greenGradeNewVal.b, &greenGradeNewVal.a,
                &blueGradeNewVal.r, &blueGradeNewVal.g, &blueGradeNewVal.b, &blueGradeNewVal.a);

            // Some sort of additional logic
            float redSum = redGradeNewVal.r + redGradeNewVal.g + redGradeNewVal.b - 1.7f;
            if(redSum > 0) redGradeNewVal.a -= redSum * 0.13f;

            float greenSum = greenGradeNewVal.r + greenGradeNewVal.g + greenGradeNewVal.b - 1.7f;
            if(greenSum > 0) greenGradeNewVal.a -= greenSum * 0.13f;

            float blueSum = blueGradeNewVal.r + blueGradeNewVal.g + blueGradeNewVal.b - 1.7f;
            if(blueSum > 0) blueGradeNewVal.a -= blueSum * 0.13f;

            CTimeCycle__m_vRedGrade[w][h] = redGradeNewVal * 0.666666666667f;
            CTimeCycle__m_vGreenGrade[w][h] = greenGradeNewVal * 0.666666666667f;
            CTimeCycle__m_vBlueGrade[w][h] = blueGradeNewVal * 0.666666666667f;
        }
    }
    CFileMgr__CloseFile(fd);

    /* Function end Section */

    *m_vecDirnLightToSun = {-0.5f, -0.5f, 0.707107f};
    VectorNormalise(m_vecDirnLightToSun);
    *m_FogReduction = 0;
    *m_bExtraColourOn = 0;
}
DECL_HOOKv(CalcColoursForPoint, CVector pos, CColourSet* colorset)
{
    CalcColoursForPoint(pos, colorset);
    return;

    CTimeCycleBox *lodBox = NULL, *farBox1 = NULL, *farBox2 = NULL, *weatherBox = NULL, *tmpBox = NULL;
    float lodBoxInterp, farBox1Interp, farBox2Interp, weatherBoxInterp, tmpInterp;
    float time;
    int curHourSel, nextHourSel;
    int curHour, nextHour;
    float timeInterp, invTimeInterp, weatherInterp, invWeatherInterp;
    int boxWeather, boxHour;
    float lightMult;
    float sunAngle;
    CVector *vec;
    float inc;
    float camZ;
    float horiz;
    float brightness;
    float max, f;

    CTimeCycle__FindTimeCycleBox(pos, &lodBox, &lodBoxInterp, true, false, NULL);
    CTimeCycle__FindTimeCycleBox(pos, &farBox1, &farBox1Interp, false, true, NULL);

    if(farBox1)
    {
        CTimeCycle__FindTimeCycleBox(pos, &farBox2, &farBox2Interp, false, true, farBox1);
        if(farBox2) if(farBox2->m_bBox.m_vecMax.x - farBox2->m_bBox.m_vecMin.x > farBox1->m_bBox.m_vecMax.x - farBox1->m_bBox.m_vecMin.x)
        {
            tmpBox = farBox1;
            farBox1 = farBox2;
            farBox2 = tmpBox;
            tmpInterp = farBox2Interp;
            farBox2Interp = farBox1Interp;
            farBox1Interp = tmpInterp;
        }
    }
    else
    {
        farBox2 = NULL;
    }
    CTimeCycle__FindTimeCycleBox(pos, &weatherBox, &weatherBoxInterp, false, false, NULL);

    time = (float)(*ms_nGameClockMinutes) / 60.0f + (float)(*ms_nGameClockSeconds) / 3600.0f + (float)(*ms_nGameClockHours);
    if(time >= 23.999f) time = 23.999f;

    for(curHourSel = 0; time >= aTimecycleHours[curHourSel+1]; ++curHourSel);
    nextHourSel = (curHourSel + 1) % NUMHOURS;
    curHour = aTimecycleHours[curHourSel];
    nextHour = aTimecycleHours[curHourSel+1];
    timeInterp = (time - curHour) / (float)(nextHour - curHour);
    invTimeInterp = 1.0f - timeInterp;
    weatherInterp = *WeatherInterpolationValue;
    invWeatherInterp = 1.0f - weatherInterp;
    if(weatherBox)
    {
        boxWeather = ((float)(weatherBox->m_index) / NUMHOURS) + 21;
        boxHour = weatherBox->m_index % 8;
    }
    CColourSet curold; HookOf_CColourSet_ctor(&curold, curHourSel, *OldWeatherType);
    CColourSet nextold; HookOf_CColourSet_ctor(&nextold, nextHourSel, *OldWeatherType);
    CColourSet curnew; HookOf_CColourSet_ctor(&curnew, curHourSel, *NewWeatherType);
    CColourSet nextnew; HookOf_CColourSet_ctor(&nextnew, nextHourSel, *NewWeatherType);

    f = (TheCamera->GetPosition().z - 20.0) / 200.0;

    if(f < 0.0) f = 0.0;
    else if(f > 1.0) f = 1.0;
    
    if(*OldWeatherType == WEATHER_EXTRASUNNY_SMOG_LA)
    {
        CColourSet set1; HookOf_CColourSet_ctor(&set1, curHourSel, WEATHER_EXTRASUNNY_LA);
        HookOf_CColourSet_Interpolate(&curold, &curold, &set1, 1.0-f, f, false);
        CColourSet set2; HookOf_CColourSet_ctor(&set2, nextHourSel, WEATHER_EXTRASUNNY_LA);
        HookOf_CColourSet_Interpolate(&nextold, &nextold, &set2, 1.0-f, f, false);
    }
    else if(*OldWeatherType == WEATHER_SUNNY_SMOG_LA)
    {
        CColourSet set1; HookOf_CColourSet_ctor(&set1, curHourSel, WEATHER_SUNNY_LA);
        HookOf_CColourSet_Interpolate(&curold, &curold, &set1, 1.0-f, f, false);
        CColourSet set2; HookOf_CColourSet_ctor(&set2, nextHourSel, WEATHER_SUNNY_LA);
        HookOf_CColourSet_Interpolate(&nextold, &nextold, &set2, 1.0-f, f, false);
    }

    if(*NewWeatherType == WEATHER_EXTRASUNNY_SMOG_LA)
    {
        CColourSet set1; HookOf_CColourSet_ctor(&set1, curHourSel, WEATHER_EXTRASUNNY_LA);
        HookOf_CColourSet_Interpolate(&curnew, &curnew, &set1, 1.0-f, f, false);
        CColourSet set2; HookOf_CColourSet_ctor(&set2, nextHourSel, WEATHER_EXTRASUNNY_LA);
        HookOf_CColourSet_Interpolate(&nextnew, &nextnew, &set2, 1.0-f, f, false);
    }
    else if(*NewWeatherType == WEATHER_SUNNY_SMOG_LA)
    {
        CColourSet set1; HookOf_CColourSet_ctor(&set1, curHourSel, WEATHER_SUNNY_LA);
        HookOf_CColourSet_Interpolate(&curnew, &curnew, &set1, 1.0-f, f, false);
        CColourSet set2; HookOf_CColourSet_ctor(&set2, nextHourSel, WEATHER_SUNNY_LA);
        HookOf_CColourSet_Interpolate(&nextnew, &nextnew, &set2, 1.0-f, f, false);
    }

    CColourSet oldInterp, newInterp;
    HookOf_CColourSet_Interpolate(&oldInterp, &curold, &nextold, invTimeInterp, timeInterp, false);
    HookOf_CColourSet_Interpolate(&newInterp, &curnew, &nextnew, invTimeInterp, timeInterp, false);
    HookOf_CColourSet_Interpolate(colorset, &oldInterp, &newInterp, invWeatherInterp, weatherInterp, false);

    lightMult = (1.0f / *CoronasLightsMult + 3.0f) * 0.25f;
    colorset->skytopr *= lightMult;
    colorset->skytopg *= lightMult;
    colorset->skytopb *= lightMult;
    colorset->skybotr *= lightMult;
    colorset->skybotg *= lightMult;
    colorset->skybotb *= lightMult;
    if(colorset->skytopr > 0xFF) colorset->skytopr = 0xFF;
    if(colorset->skytopg > 0xFF) colorset->skytopg = 0xFF;
    if(colorset->skytopb > 0xFF) colorset->skytopb = 0xFF;
    if(colorset->skybotr > 0xFF) colorset->skybotr = 0xFF;
    if(colorset->skybotg > 0xFF) colorset->skybotg = 0xFF;
    if(colorset->skybotb > 0xFF) colorset->skybotb = 0xFF;

    if(*m_FogReduction)
        colorset->farclp = colorset->farclp > *m_FogReduction * 10.15625 ? colorset->farclp : *m_FogReduction * 10.15625;

    *m_CurrentStoredValue = (*m_CurrentStoredValue + 1) & 0xF;
    vec = &m_VectorToSun[*m_CurrentStoredValue];
    sunAngle = (*ms_nGameClockMinutes + 60 * *ms_nGameClockHours + (float)(*ms_nGameClockSeconds) / 60.0f) * 0.0043633231;
    vec->x = 0.7 + sin(sunAngle);
    vec->y = -0.7;
    vec->z = 0.2 - cos(sunAngle);
    VectorNormalise(vec);

    if(weatherBox && weatherBox->m_index >= 0)
    {
        float boxf = weatherBoxInterp * weatherBox->m_percentage;
        float invboxf = 1.0 - boxf;

        colorset->skytopr = colorset->skytopr*invboxf + CTimeCycle__m_nSkyTopRed[boxHour][boxWeather]*boxf;
        colorset->skytopg = colorset->skytopg*invboxf + CTimeCycle__m_nSkyTopGreen[boxHour][boxWeather]*boxf;
        colorset->skytopb = colorset->skytopg*invboxf + CTimeCycle__m_nSkyTopBlue[boxHour][boxWeather]*boxf;

        colorset->skybotr = colorset->skybotr*invboxf + CTimeCycle__m_nSkyBottomRed[boxHour][boxWeather]*boxf;
        colorset->skybotg = colorset->skybotg*invboxf + CTimeCycle__m_nSkyBottomGreen[boxHour][boxWeather]*boxf;
        colorset->skybotb = colorset->skybotb*invboxf + CTimeCycle__m_nSkyBottomBlue[boxHour][boxWeather]*boxf;

        colorset->waterr = colorset->waterr*invboxf + CTimeCycle__m_fWaterRed[boxHour][boxWeather]*boxf;
        colorset->waterg = colorset->waterg*invboxf + CTimeCycle__m_fWaterGreen[boxHour][boxWeather]*boxf;
        colorset->waterb = colorset->waterb*invboxf + CTimeCycle__m_fWaterBlue[boxHour][boxWeather]*boxf;
        colorset->watera = colorset->watera*invboxf + CTimeCycle__m_fWaterAlpha[boxHour][boxWeather]*boxf;

        colorset->ambr = colorset->ambr*invboxf + CTimeCycle__m_nAmbientRed[boxHour][boxWeather]*boxf;
        colorset->ambg = colorset->ambg*invboxf + CTimeCycle__m_nAmbientGreen[boxHour][boxWeather]*boxf;
        colorset->ambb = colorset->ambb*invboxf + CTimeCycle__m_nAmbientBlue[boxHour][boxWeather]*boxf;

        colorset->ambobjr = colorset->ambobjr*invboxf + CTimeCycle__m_nAmbientRed_Obj[boxHour][boxWeather]*boxf;
        colorset->ambobjg = colorset->ambobjg*invboxf + CTimeCycle__m_nAmbientGreen_Obj[boxHour][boxWeather]*boxf;
        colorset->ambobjb = colorset->ambobjb*invboxf + CTimeCycle__m_nAmbientBlue_Obj[boxHour][boxWeather]*boxf;

        if(CTimeCycle__m_fFarClip[boxHour][boxWeather] < colorset->farclp)
            colorset->farclp = colorset->farclp*invboxf + CTimeCycle__m_fFarClip[boxHour][boxWeather]*boxf;

        colorset->fogst = colorset->fogst*invboxf + CTimeCycle__m_fFogStart[boxHour][boxWeather]*boxf;

        colorset->postfx1r = colorset->postfx1r*invboxf + CTimeCycle__m_fPostFx1Red[boxHour][boxWeather]*boxf;
        colorset->postfx1g = colorset->postfx1g*invboxf + CTimeCycle__m_fPostFx1Green[boxHour][boxWeather]*boxf;
        colorset->postfx1b = colorset->postfx1b*invboxf + CTimeCycle__m_fPostFx1Blue[boxHour][boxWeather]*boxf;
        colorset->postfx1a = colorset->postfx1a*invboxf + CTimeCycle__m_fPostFx1Alpha[boxHour][boxWeather]*boxf;

        colorset->postfx2r = colorset->postfx2r*invboxf + CTimeCycle__m_fPostFx2Red[boxHour][boxWeather]*boxf;
        colorset->postfx2g = colorset->postfx2g*invboxf + CTimeCycle__m_fPostFx2Green[boxHour][boxWeather]*boxf;
        colorset->postfx2b = colorset->postfx2b*invboxf + CTimeCycle__m_fPostFx2Blue[boxHour][boxWeather]*boxf;
        colorset->postfx2a = colorset->postfx2a*invboxf + CTimeCycle__m_fPostFx2Alpha[boxHour][boxWeather]*boxf;
    }

    if(lodBox)
        colorset->lodDistMult = colorset->lodDistMult * (1.0f - lodBoxInterp) + (float)(lodBox->m_lodMult) / 32.0f * lodBoxInterp;

    if(farBox1 && farBox1->m_farClip < colorset->farclp)
        colorset->farclp = colorset->farclp*(1.0f-farBox1Interp) + farBox1->m_farClip*farBox1Interp;
    if(farBox2 && farBox2->m_farClip < colorset->farclp)
        colorset->farclp = colorset->farclp*(1.0f-farBox2Interp) + farBox2->m_farClip*farBox2Interp;

    inc = *ms_fTimeStep / 120.0f;
    if(*m_bExtraColourOn){
        *m_ExtraColourInter += inc;
        if(*m_ExtraColourInter > 1.0f)
        *m_ExtraColourInter = 1.0f;
    }else{
        *m_ExtraColourInter -= inc;
        if(*m_ExtraColourInter < 0.0f)
            *m_ExtraColourInter = 0.0f;
    }
    if(*m_ExtraColourInter > 0.0f)
    {
        CColourSet extraset; HookOf_CColourSet_ctor(&extraset, *m_ExtraColour, *m_ExtraColourWeatherType);
        HookOf_CColourSet_Interpolate(colorset, colorset, &extraset, 1.0f-*m_ExtraColourInter, *m_ExtraColourInter, 
            CTimeCycle__m_nSkyTopRed[*m_ExtraColour][*m_ExtraColourWeatherType] == 0 && 
            CTimeCycle__m_nSkyTopGreen[*m_ExtraColour][*m_ExtraColourWeatherType] == 0 && 
            CTimeCycle__m_nSkyTopBlue[*m_ExtraColour][*m_ExtraColourWeatherType] == 0);
    }

    if(*WeatherUnderWaterness > 0.0f)
    {
        CColourSet curuwset; HookOf_CColourSet_ctor(&curuwset, curHourSel, 20);
        CColourSet nextuwset; HookOf_CColourSet_ctor(&nextuwset, nextHourSel, 20);
        CColourSet tmpset;
        HookOf_CColourSet_Interpolate(&tmpset, &curuwset, &nextuwset, invTimeInterp, timeInterp, false);
        HookOf_CColourSet_Interpolate(colorset, colorset, &tmpset, 1.0f-*WeatherUnderWaterness, *WeatherUnderWaterness, false);
    }

    if(*WeatherInTunnelness > 0.0f)
    {
        CColourSet tunnelset; HookOf_CColourSet_ctor(&tunnelset, 0, 21); //9 % NUMHOURS, 9 / NUMHOURS + EXTRASTART);
        HookOf_CColourSet_Interpolate(colorset, colorset, &tunnelset, 1.0f-*WeatherInTunnelness, *WeatherInTunnelness, 
            CTimeCycle__m_nSkyTopRed[*m_ExtraColour][*m_ExtraColourWeatherType] == 0 && 
            CTimeCycle__m_nSkyTopGreen[*m_ExtraColour][*m_ExtraColourWeatherType] == 0 && 
            CTimeCycle__m_nSkyTopBlue[*m_ExtraColour][*m_ExtraColourWeatherType] == 0);
    }
    
    colorset->ambr /= 255.0f;
    colorset->ambg /= 255.0f;
    colorset->ambb /= 255.0f;
    colorset->ambobjr /= 255.0f;
    colorset->ambobjg /= 255.0f;
    colorset->ambobjb /= 255.0f;

    CShadows__CalcPedShadowValues(
        m_VectorToSun[*m_CurrentStoredValue],
        &m_fShadowFrontX[*m_CurrentStoredValue],
        &m_fShadowFrontY[*m_CurrentStoredValue],
        &m_fShadowSideX[*m_CurrentStoredValue],
        &m_fShadowSideY[*m_CurrentStoredValue],
        &m_fShadowDisplacementX[*m_CurrentStoredValue],
        &m_fShadowDisplacementY[*m_CurrentStoredValue]);

    if((TheCamera->m_matCameraMatrix.up.z < -0.9 || !*bScriptsForceRain)
        && (CCullZones__PlayerNoRain() || CCullZones__CamNoRain() || *ms_runningCutscene))
    {
        ++(*m_FogReduction);
        if(*m_FogReduction > 64) *m_FogReduction = 64;
    }
    else
    {
        --(*m_FogReduction);
        if(*m_FogReduction < 0) *m_FogReduction = 0;
    }

    if(camZ > 200.0f)
    {
        if(camZ <= 500.0f)
            colorset->farclp = colorset->farclp*(1.0f - (camZ-200.0f)/300.0f) + 1000.0f*(camZ-200.0f)/300.0f;
        else if(colorset->farclp >= 1000.0f)
            colorset->farclp = 1000.0f;
    }

    horiz = aTimecycleHorizon[curHourSel] * invTimeInterp + aTimecycleHorizon[nextHourSel] * timeInterp;
    m_BelowHorizonGrey->red = m_CurrentColours.skybotr * *WeatherUnderWaterness + horiz*(1.0f - *WeatherUnderWaterness);
    m_BelowHorizonGrey->green = m_CurrentColours.skybotg * *WeatherUnderWaterness + horiz*(1.0f - *WeatherUnderWaterness);
    m_BelowHorizonGrey->blue = m_CurrentColours.skybotb * *WeatherUnderWaterness + horiz*(1.0f - *WeatherUnderWaterness);

    colorset->ambBeforeBrightnessr = colorset->ambr;
    colorset->ambBeforeBrightnessg = colorset->ambg;
    colorset->ambBeforeBrightnessb = colorset->ambb;

    if(*BrightnessPercent > 59)
    {
        f = (float)(*BrightnessPercent - 60) / 20.0f + 1.0f;
        max = colorset->ambr;
        if(colorset->ambg > max)
            max = colorset->ambg;
        if(colorset->ambb > max)
            max = colorset->ambb;
        max = max*f - max;
        colorset->ambr += max;
        colorset->ambg += max;
        colorset->ambb += max;
    }
    else
    {
        f = (float)(*BrightnessPercent) / 60.0f * 0.8f + 0.2f;
        colorset->ambr *= f;
        colorset->ambg *= f;
        colorset->ambb *= f;
    }

    if(f > 1.0)
    {
        float r, g, b;
        f = (f - 1.0f) * 0.06f;
        max = colorset->ambr;
        if(colorset->ambg > max)
            max = colorset->ambg;
        if(colorset->ambb > max)
            max = colorset->ambb;
        r = colorset->ambr;
        g = colorset->ambg;
        b = colorset->ambb;
        if(max == 0.0)
        {
            colorset->ambr = 0.001;
            colorset->ambg = 0.001;
            colorset->ambb = 0.001;
        }
        if(f > max)
        {
            f /= max;
            colorset->ambr *= f;
            colorset->ambg *= f;
            colorset->ambb *= f;
        }
        *m_BrightnessAddedToAmbientRed = colorset->ambr - r;
        *m_BrightnessAddedToAmbientGreen = colorset->ambg - g;
        *m_BrightnessAddedToAmbientBlue = colorset->ambb - b;
    }

    f = 0.0;
    if(pos.x < -3000.0)
        f = -(pos.x + 3000.0);
    else if(pos.x > 3000.0)
        f = pos.x - 3000.0;

    if(pos.y < -3000.0)
        f += -(pos.y + 3000.0);
    else if(pos.y > 3000.0)
        f += pos.y - 3000.0;

    if(f >= 1000.0)
        colorset->lodDistMult *= 2.0;
    else if(f > 0.0)
        colorset->lodDistMult *= (f / 1000.0 + 1.0);

    HookOf_SetConstantParametersForPostFX();
}
DECL_HOOK(float, FindFarClipForCoors, CVector pos)
{
    CColourSet s;
    bool extraOn;
    float extraInter;
    extraOn = *m_bExtraColourOn != 0;
    extraInter = *m_ExtraColourInter;
    *m_bExtraColourOn = 0;
    *m_ExtraColourInter = 0.0;
    HookOf_CalcColoursForPoint(pos, &s);
    *m_bExtraColourOn = extraOn;
    *m_ExtraColourInter = extraInter;
    return s.farclp;
}
DECL_HOOKv(TimecycUpdate)
{
    HookOf_CalcColoursForPoint(TheCamera->GetPosition(), &m_CurrentColours);
}

/////////////////////////////////////////////////////////////////////////////
///////////////////////////////     Funcs     ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////
extern "C" void OnModPreLoad()
{
    logger->SetTag("Timecyc24");
    
    pGTASA = aml->GetLib("libGTASA.so");
    hGTASA = aml->GetLibHandle("libGTASA.so");
    
    // GTA Funcs
    SET_TO(CFileMgr__SetDir,                aml->GetSym(hGTASA, "_ZN8CFileMgr6SetDirEPKc"));
    SET_TO(CFileMgr__OpenFile,              aml->GetSym(hGTASA, "_ZN8CFileMgr8OpenFileEPKcS1_"));
    SET_TO(CFileMgr__CloseFile,             aml->GetSym(hGTASA, "_ZN8CFileMgr9CloseFileEj"));
    SET_TO(CFileLoader__LoadLine,           aml->GetSym(hGTASA, "_ZN11CFileLoader8LoadLineEj"));
    SET_TO(CCullZones__PlayerNoRain,        aml->GetSym(hGTASA, "_ZN10CCullZones12PlayerNoRainEv"));
    SET_TO(CCullZones__CamNoRain,           aml->GetSym(hGTASA, "_ZN10CCullZones9CamNoRainEv"));
    SET_TO(CPostEffects__IsVisionFXActive,  aml->GetSym(hGTASA, "_ZN12CPostEffects16IsVisionFXActiveEv"));
    SET_TO(CShadows__CalcPedShadowValues,   aml->GetSym(hGTASA, "_ZN8CShadows19CalcPedShadowValuesE7CVectorPfS1_S1_S1_S1_S1_"));
    SET_TO(CTimeCycle__FindTimeCycleBox,    aml->GetSym(hGTASA, "_ZN10CTimeCycle16FindTimeCycleBoxE7CVectorPP13CTimeCycleBoxPfbbS2_"));

    // GTA Variables
    SET_TO(ms_fTimeStep,                    aml->GetSym(hGTASA, "_ZN6CTimer12ms_fTimeStepE"));
    SET_TO(WeatherInterpolationValue,       aml->GetSym(hGTASA, "_ZN8CWeather18InterpolationValueE"));
    SET_TO(WeatherUnderWaterness,           aml->GetSym(hGTASA, "_ZN8CWeather14UnderWaternessE"));
    SET_TO(WeatherInTunnelness,             aml->GetSym(hGTASA, "_ZN8CWeather12InTunnelnessE"));
    SET_TO(CoronasLightsMult,               aml->GetSym(hGTASA, "_ZN8CCoronas10LightsMultE"));
    SET_TO(m_BrightnessAddedToAmbientRed,   aml->GetSym(hGTASA, "_ZN10CTimeCycle29m_BrightnessAddedToAmbientRedE"));
    SET_TO(m_BrightnessAddedToAmbientGreen, aml->GetSym(hGTASA, "_ZN10CTimeCycle31m_BrightnessAddedToAmbientGreenE"));
    SET_TO(m_BrightnessAddedToAmbientBlue,  aml->GetSym(hGTASA, "_ZN10CTimeCycle30m_BrightnessAddedToAmbientBlueE"));
    SET_TO(m_ExtraColourInter,              aml->GetSym(hGTASA, "_ZN10CTimeCycle18m_ExtraColourInterE"));
    SET_TO(m_fShadowFrontX,                 aml->GetSym(hGTASA, "_ZN10CTimeCycle15m_fShadowFrontXE"));
    SET_TO(m_fShadowFrontY,                 aml->GetSym(hGTASA, "_ZN10CTimeCycle15m_fShadowFrontYE"));
    SET_TO(m_fShadowSideX,                  aml->GetSym(hGTASA, "_ZN10CTimeCycle14m_fShadowSideXE"));
    SET_TO(m_fShadowSideY,                  aml->GetSym(hGTASA, "_ZN10CTimeCycle14m_fShadowSideYE"));
    SET_TO(m_fShadowDisplacementX,          aml->GetSym(hGTASA, "_ZN10CTimeCycle22m_fShadowDisplacementXE"));
    SET_TO(m_fShadowDisplacementY,          aml->GetSym(hGTASA, "_ZN10CTimeCycle22m_fShadowDisplacementYE"));
    SET_TO(OldWeatherType,                  aml->GetSym(hGTASA, "_ZN8CWeather14OldWeatherTypeE"));
    SET_TO(NewWeatherType,                  aml->GetSym(hGTASA, "_ZN8CWeather14NewWeatherTypeE"));
    SET_TO(ms_nGameClockSeconds,            aml->GetSym(hGTASA, "_ZN6CClock20ms_nGameClockSecondsE"));
    SET_TO(ms_nGameClockMinutes,            aml->GetSym(hGTASA, "_ZN6CClock20ms_nGameClockMinutesE"));
    SET_TO(ms_nGameClockHours,              aml->GetSym(hGTASA, "_ZN6CClock18ms_nGameClockHoursE"));
    SET_TO(bScriptsForceRain,               aml->GetSym(hGTASA, "_ZN8CWeather17bScriptsForceRainE"));
    SET_TO(ms_runningCutscene,              aml->GetSym(hGTASA, "_ZN12CCutsceneMgr10ms_runningE"));
    SET_TO(m_bNightVision,                  aml->GetSym(hGTASA, "_ZN12CPostEffects14m_bNightVisionE"));
    SET_TO(m_bInfraredVision,               aml->GetSym(hGTASA, "_ZN12CPostEffects17m_bInfraredVisionE"));
    SET_TO(m_ExtraColourWeatherType,        aml->GetSym(hGTASA, "_ZN10CTimeCycle24m_ExtraColourWeatherTypeE"));
    SET_TO(m_ExtraColour,                   aml->GetSym(hGTASA, "_ZN10CTimeCycle13m_ExtraColourE"));
    SET_TO(m_bExtraColourOn,                aml->GetSym(hGTASA, "_ZN10CTimeCycle16m_bExtraColourOnE"));
    SET_TO(m_FogReduction,                  aml->GetSym(hGTASA, "_ZN10CTimeCycle14m_FogReductionE"));
    SET_TO(m_CurrentStoredValue,            aml->GetSym(hGTASA, "_ZN10CTimeCycle20m_CurrentStoredValueE"));
    SET_TO(m_vecDirnLightToSun,             aml->GetSym(hGTASA, "_ZN10CTimeCycle19m_vecDirnLightToSunE"));
    SET_TO(m_VectorToSun,                   aml->GetSym(hGTASA, "_ZN10CTimeCycle13m_VectorToSunE"));
    SET_TO(m_BelowHorizonGrey,              aml->GetSym(hGTASA, "_ZN10CTimeCycle18m_BelowHorizonGreyE"));
    SET_TO(TheCamera,                       aml->GetSym(hGTASA, "TheCamera"));
    SET_TO(BrightnessPercent,               pGTASA + 0x6E05FC);

    // GTA Patches
    PatchTheValues();
    aml->Write(pGTASA + 0x41F106, (uintptr_t)"\x18", sizeof(char));
    aml->Write(pGTASA + 0x41FD4A, (uintptr_t)"\x00", sizeof(char));
    aml->Write(pGTASA + 0x41FD4C, (uintptr_t)"\x15", sizeof(char));
    //aml->Write(pGTASA + 0x471438 + 0x2, (uintptr_t)"\x18", sizeof(char)); // Timecyc::Init

    ModuloPatch_BackTo = pGTASA + 0x41F0CE + 0x1;
    aml->Redirect(pGTASA + 0x41F0C0 + 0x1, (uintptr_t)ModuloPatch_inject);
    aml->PlaceNOP(pGTASA + 0x41F0D6 + 0x1, 2);
    aml->PlaceNOP(pGTASA + 0x41F0E4 + 0x1, 2);
    aml->Write(pGTASA + 0x41F116, (uintptr_t)"\x01\x46\x00\xBF\x00\xBF", 6);

    HorizontAngles_BackTo = pGTASA + 0x41FFCC + 0x1;
    aml->Redirect(pGTASA + 0x41FFB8 + 0x1, (uintptr_t)HorizontAngles_inject);

    Hours_BackTo = pGTASA + 0x41F082 + 0x1;
    aml->Redirect(pGTASA + 0x41F076 + 0x1, (uintptr_t)Hours_inject);

    // GTA Hooks
    //HOOK(CColourSet_ctor,                   aml->GetSym(hGTASA, "_ZN10CColourSetC2Eii"));
    //HOOK(CColourSet_Interpolate,            aml->GetSym(hGTASA, "_ZN10CColourSet11InterpolateEPS_S0_ffb"));
    //HOOK(SetConstantParametersForPostFX,    aml->GetSym(hGTASA, "_ZN10CTimeCycle30SetConstantParametersForPostFXEv"));
    HOOK(StartExtraColour,                  aml->GetSym(hGTASA, "_ZN10CTimeCycle16StartExtraColourEib"));
    HOOK(TimecycInit,                       aml->GetSym(hGTASA, "_ZN10CTimeCycle10InitialiseEb"));
    //HOOK(CalcColoursForPoint,               aml->GetSym(hGTASA, "_ZN10CTimeCycle19CalcColoursForPointE7CVectorP10CColourSet"));
    //HOOK(FindFarClipForCoors,               aml->GetSym(hGTASA, "_ZN10CTimeCycle19FindFarClipForCoorsE7CVector"));
    //HOOK(TimecycUpdate,                     aml->GetSym(hGTASA, "_ZN10CTimeCycle6UpdateEv"));
}