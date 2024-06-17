#include <mod/amlmod.h>
#include <mod/logger.h>

#include "timecyc.h"

MYMOD(net.rusjj.timecyc24, GTASA Timecyc24, 1.2, GTAmodding & RusJJ)
NEEDGAME(com.rockstargames.gtasa)
BEGIN_DEPLIST()
    ADD_DEPENDENCY_VER(net.rusjj.aml, 1.2.2)
END_DEPLIST()

/////////////////////////////////////////////////////////////////////////////
///////////////////////////////     Saves     ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////
uintptr_t pGTASA;
void* hGTASA;

uint8_t aTimecycleHours[NUMHOURS+1] = {  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24 };
uint8_t aTimecycleHorizon[NUMHOURS] = { 30, 30, 30, 30, 30, 30, 30, 50, 52, 54, 56, 58, 60, 60, 60, 60, 60, 60, 60, 60, 50, 42, 35, 32 };

/////////////////////////////////////////////////////////////////////////////
///////////////////////////////     Vars      ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////
float *m_ExtraColourInter;
int32_t *m_ExtraColourWeatherType, *m_ExtraColour, *m_bExtraColourOn;

/////////////////////////////////////////////////////////////////////////////
///////////////////////////////     Funcs     ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////
uintptr_t (*CFileMgr__OpenFile)(const char *path, const char *mode);
char* (*CFileLoader__LoadLine)(uintptr_t fd);

/////////////////////////////////////////////////////////////////////////////
//////////////////////////////     Patches     //////////////////////////////
/////////////////////////////////////////////////////////////////////////////
uintptr_t TimecycInit1_BackTo, TimecycInit2_BackTo, TimecycInit3_BackTo;
extern "C" uintptr_t TimecycInit1_Patch(void) { return CFileMgr__OpenFile("TIMECYC_24H.DAT", "rb"); }
extern "C" uintptr_t TimecycInit2_Patch(void) { return CFileMgr__OpenFile("DATA/COLORCYCLE_24H.DAT", "rb"); }
extern "C" char* TimecycInit3_Patch(uintptr_t fd)
{
    char* line = NULL;
    while(line = CFileLoader__LoadLine(fd), line) if(line[0] != '/' && line[0] != '\0') break;
    return line;
}
#ifdef AML32
__attribute__((optnone)) __attribute__((naked)) void TimecycInit1_Inject(void)
{
    asm volatile(
        "BL              TimecycInit1_Patch\n"
        "PUSH            {R0}\n");
    asm volatile(
        "MOV             R12, %0\n"
        "POP             {R0}\n"
        "BX              R12\n"
    :: "r" (TimecycInit1_BackTo));
}
__attribute__((optnone)) __attribute__((naked)) void TimecycInit2_Inject(void)
{
    asm volatile(
        "BL              TimecycInit2_Patch\n"
        "PUSH            {R0}\n");
    asm volatile(
        "MOV             R12, %0\n"
        "POP             {R0}\n"
        "BX              R12\n"
    :: "r" (TimecycInit2_BackTo));
}
__attribute__((optnone)) __attribute__((naked)) void TimecycInit3_Inject(void)
{
    asm volatile(
        "MOV             R0, R11\n"
        "ADD.W           R5, R1, R8\n"
        "ADD.W           R4, R5, #0xC\n"
        "PUSH            {R1-R11}\n"
        "BL              TimecycInit3_Patch\n"
        "POP             {R1-R11}\n"
        "PUSH            {R0}\n");
    asm volatile(
        "MOV             R12, %0\n"
        "POP             {R0}\n"
        "BX              R12\n"
    :: "r" (TimecycInit3_BackTo));
}
#else
__attribute__((optnone)) __attribute__((naked)) void TimecycInit1_Inject(void)
{
    asm("BL TimecycInit1_Patch\n");
    asm volatile("MOV X16, %0" :: "r"(TimecycInit1_BackTo));
    asm("BR X16");
}
__attribute__((optnone)) __attribute__((naked)) void TimecycInit2_Inject(void)
{
    asm("BL TimecycInit2_Patch\nMOV X18, #0x2280");
    asm volatile("MOV X16, %0" :: "r"(TimecycInit2_BackTo));
    asm("BR X16");
}
__attribute__((optnone)) __attribute__((naked)) void TimecycInit3_Inject(void)
{
    asm("BL TimecycInit3_Patch\n");
    asm("ADD X21, X25, X28\nADD X22, X27, X28\nADD X24, X26, X28");
    asm volatile("MOV X16, %0" :: "r"(TimecycInit3_BackTo));
    asm("BR X16");
}
#endif


uintptr_t HorizontAngles_BackTo, Hours_BackTo;
#ifdef AML32
__attribute__((optnone)) __attribute__((naked)) void HorizontAngles_Inject(void)
{
    asm volatile(
        "ADD.W R12, R11, #0x18\n");
    asm volatile(
        "MOV R0, %0\n"
    :: "r" (&aTimecycleHorizon));
    asm volatile(
        "LDRB R1, [R0, R1]\n"
        "ADD R0, R10\n"
        "LDRB R0, [R0, #-1]\n"
        "VMOV S0, R1\n"
        "PUSH {R0}\n");
    asm volatile(
        "MOV R1, %0\n"
        "POP {R0}\n"
        "BX R1\n"
    :: "r" (HorizontAngles_BackTo));
}
__attribute__((optnone)) __attribute__((naked)) void Hours_Inject(void)
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
#else
__attribute__((optnone)) __attribute__((naked)) void HorizontAngles_Inject(void)
{
    asm volatile("MOV X16, %0" :: "r"(HorizontAngles_BackTo));
    asm volatile("MOV X9, %0" :: "r"(&aTimecycleHorizon));
    asm("LDR B2, [X8, W20, SXTW]");
    asm volatile("MOV X20, %0" :: "r"(m_CurrentColours));
    asm("LDR S1, [X22]\nBR X16");
}
__attribute__((optnone)) __attribute__((naked)) void Hours_Inject(void)
{
    asm volatile("MOV X16, %0" :: "r"(Hours_BackTo));
    asm volatile("MOV X8, %0" :: "r"(&aTimecycleHours));
    asm("FADD S0, S1, S0\nMOV X9, #0xFFFFFFFF00000000\nBR X16");
}
#endif

uintptr_t ModuloPatch1_BackTo, ModuloPatch2_BackTo;
extern "C" int ModuloPatch_Patch(int unmoduledVal) { return (unmoduledVal % NUMHOURS); }
extern "C" int ModuloPatchPlus1_Patch(int unmoduledVal) { return 1 + (unmoduledVal % NUMHOURS); }
#ifdef AML32
__attribute__((optnone)) __attribute__((naked)) void ModuloPatch1_Inject(void)
{
    asm volatile(
        "SUBS            R0, R1, R0\n"
        "VMOV            S4, R0\n"
        "VCVT.F32.U32    S2, S2\n"
        "MOV             R0, R10\n"
    );
    asm volatile(
        "PUSH            {R2-R11}\n"
        "BL              ModuloPatch_Patch\n"
        "MOV             R1, R0\n"
        "PUSH            {R0}\n");
    asm volatile(
        "MOV R12, %0\n"
        "POP {R0}\n"
        "POP {R2-R11}\n"
        "BX R12\n"
    :: "r" (ModuloPatch1_BackTo));
}
__attribute__((optnone)) __attribute__((naked)) void ModuloPatch2_Inject(void)
{
    asm volatile(
        "PUSH            {R2}\n"
        "BL              ModuloPatch_Patch\n"
        "POP             {R2}\n"
        "PUSH            {R0}\n");
    asm volatile(
        "MOV             R12, %0\n"
        "POP             {R0}\n"
        "BX              R12\n"
    :: "r" (ModuloPatch2_BackTo));
}
#else
__attribute__((optnone)) __attribute__((naked)) void ModuloPatch1_Inject(void)
{
    asm("FSUB S0, S0, S1\nSCVTF S1, W8");
    asm("MOV W0, W21");
    asm("BL ModuloPatchPlus1_Patch");
    asm volatile("MOV X16, %0" :: "r"(ModuloPatch1_BackTo));
    asm("MOV W20, W0\nBR X16");
}
__attribute__((optnone)) __attribute__((naked)) void ModuloPatch2_Inject(void)
{
    asm("MOV W0, W8");
    asm("BL ModuloPatch_Patch");
    asm volatile("MOV X16, %0" :: "r"(ModuloPatch2_BackTo));
    asm("MOV W8, W0\nBR X16");
}
#endif
/////////////////////////////////////////////////////////////////////////////
///////////////////////////////     Hooks     ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////
DECL_HOOKv(StartExtraColour, int extracolor, bool keepInter)
{
    *m_ExtraColourWeatherType = (float)(extracolor) / (float)(NUMHOURS) + WEATHER_EXTRA_START;
    *m_ExtraColour = extracolor % (NUMHOURS);
    *m_bExtraColourOn = 1;
    *m_ExtraColourInter = (keepInter) ? 0.0f : 1.0f;
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
    SET_TO(CFileMgr__OpenFile,              aml->GetSym(hGTASA, "_ZN8CFileMgr8OpenFileEPKcS1_"));
    SET_TO(CFileLoader__LoadLine,           aml->GetSym(hGTASA, BYBIT("_ZN11CFileLoader8LoadLineEj", "_ZN11CFileLoader8LoadLineEy")));

    // GTA Variables
    SET_TO(m_ExtraColourInter,              aml->GetSym(hGTASA, "_ZN10CTimeCycle18m_ExtraColourInterE"));
    SET_TO(m_ExtraColourWeatherType,        aml->GetSym(hGTASA, "_ZN10CTimeCycle24m_ExtraColourWeatherTypeE"));
    SET_TO(m_ExtraColour,                   aml->GetSym(hGTASA, "_ZN10CTimeCycle13m_ExtraColourE"));
    SET_TO(m_bExtraColourOn,                aml->GetSym(hGTASA, "_ZN10CTimeCycle16m_bExtraColourOnE"));
    SET_TO(m_CurrentColours,                aml->GetSym(hGTASA, "_ZN10CTimeCycle16m_CurrentColoursE"));

    // GTA Patches
    // - Redirect all values to our mod
    PatchTheValues();

    // - CTimeCycle::Initialise
    aml->Write(pGTASA + BYBIT(0x471438 + 0x2, 0x55D770 + 0x1), BYBIT("\x18", "\x62"), 1);
    aml->Write(pGTASA + BYBIT(0x47159C, 0x55DCC4), BYBIT("\xB8\xF5\x0A\x5F", "\x9F\x03\x12\xEB"), 4);
    TimecycInit1_BackTo = pGTASA + BYBIT(0x470E62 + 0x1, 0x55D4F8);
    TimecycInit2_BackTo = pGTASA + BYBIT(0x47145C + 0x1, 0x55DB4C);
    TimecycInit3_BackTo = pGTASA + BYBIT(0x4714BE + 0x1, 0x55DBC4);
    aml->Redirect(pGTASA + BYBIT(0x470E56 + 0x1, 0x55D4E4), (uintptr_t)TimecycInit1_Inject);
    aml->Redirect(pGTASA + BYBIT(0x471454 + 0x1, 0x55DB38), (uintptr_t)TimecycInit2_Inject);
    aml->Redirect(pGTASA + BYBIT(0x4714B0 + 0x1, 0x55DBB4), (uintptr_t)TimecycInit3_Inject);

    // - CTimeCycle::CalcColoursForPoint
    HorizontAngles_BackTo = pGTASA + BYBIT(0x41FFCC + 0x1, 0x503AE4);
    Hours_BackTo = pGTASA + BYBIT(0x41F082 + 0x1, 0x502C90);
    aml->Redirect(pGTASA + BYBIT(0x41FFB8 + 0x1, 0x503ACC), (uintptr_t)HorizontAngles_Inject);
    aml->Redirect(pGTASA + BYBIT(0x41F076 + 0x1, 0x502C80), (uintptr_t)Hours_Inject);

    ModuloPatch1_BackTo = pGTASA + BYBIT(0x41F0D2 + 0x1, 0x502CFC);
    ModuloPatch2_BackTo = pGTASA + BYBIT(0x41F122 + 0x1, 0x502D38);
    aml->Redirect(pGTASA + BYBIT(0x41F0C0 + 0x1, 0x502CEC), (uintptr_t)ModuloPatch1_Inject);
    aml->Redirect(pGTASA + BYBIT(0x41F112 + 0x1, 0x502D20), (uintptr_t)ModuloPatch2_Inject);

    aml->Write(pGTASA + BYBIT(0x41F104 + 0x2, 0x502D14 + 0x1), BYBIT("\x18", "\x61"), 1);
    //aml->Write(pGTASA + 0x41FD4A, (uintptr_t)"\x00", sizeof(char));
    //aml->Write(pGTASA + 0x41FD4C, (uintptr_t)"\x15", sizeof(char));

  #ifdef AML32
    aml->PlaceNOP(pGTASA + 0x41F0D6, 2);
    aml->PlaceNOP(pGTASA + 0x41F0E4, 2);
    aml->PlaceNOP(pGTASA + 0x41F108, 2);
  #else
    aml->PlaceNOP(pGTASA + 0x502D18, 1);
  #endif

    // - CTimeCycle::StartExtraColour
    HOOKPLT(StartExtraColour, pGTASA + BYBIT(0x671F24, 0x843670));
}