/*
 * File:   easyconnect.cpp
 * Author: joris
 *
 * Created on 15 augustus 2009, 19:58
 */

#include "easyconnect.h"

static unsigned int __attribute__((aligned(16))) list[262144];

EasyConnect::EasyConnect() {
    bgcolor=0;

    sceUtilityLoadNetModule(PSP_NET_MODULE_COMMON);
    sceUtilityLoadNetModule(PSP_NET_MODULE_INET);
    sceNetInit(128 * 1024, 42, 4 * 1024, 42, 4 * 1024);
    sceNetInetInit();
    sceNetApctlInit(0x8000, 48);

    memset(&data, 0, sizeof (data));
    data.base.size = sizeof (data);
    sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_LANGUAGE, &data.base.language);
    sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_UNKNOWN, &data.base.buttonSwap);
    data.base.graphicsThread = 17;
    data.base.accessThread = 19;
    data.base.fontThread = 18;
    data.base.soundThread = 16;
    //data.action = PSP_NETCONF_ACTION_CONNECTAP;
    data.action = 3;

    memset(&adhocparam, 0, sizeof (adhocparam));
    data.adhocparam = &adhocparam;
}

EasyConnect::~EasyConnect() {
    sceNetApctlTerm();
    sceNetInetTerm();
    sceNetTerm();

    sceUtilityUnloadNetModule(PSP_NET_MODULE_INET);
    sceUtilityUnloadNetModule(PSP_NET_MODULE_COMMON);
}

int EasyConnect::Connect() {
    sceUtilityNetconfInitStart(&data);
    int done = 0;
    int state;
    sceNetApctlGetState(&state);
    int newstate = 0;
    do {
        sceGuStart(GU_DIRECT, list);
        sceGuClearColor(bgcolor);
        sceGuClearDepth(0);
        sceGuClear(GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT);
        sceGuFinish();
        sceGuSync(0, 0);

        switch (sceUtilityNetconfGetStatus()) {
            case PSP_UTILITY_DIALOG_NONE:
                break;

            case PSP_UTILITY_DIALOG_VISIBLE:
                sceUtilityNetconfUpdate(1);
                break;

            case PSP_UTILITY_DIALOG_QUIT:
                sceUtilityNetconfShutdownStart();
                break;

            case PSP_UTILITY_DIALOG_FINISHED:
                done = 1;
                break;
            default:
                break;
        }
        sceDisplayWaitVblankStart();
        sceGuSwapBuffers();

        sceNetApctlGetState(&newstate);
        if (state != newstate) {
            onStateChange(newstate);
            state = newstate;
        }
    } while (!done);
    if (state == 4) {
        return 1;
    } else {
        return 0;
    }
}

int EasyConnect::IsConnected(){
    int state;
    sceNetApctlGetState(&state);
    if (state == 4) {
        return 1;
    } else {
        return 0;
    }
}

void EasyConnect::Disconnect(){
    sceNetApctlDisconnect();
}

void EasyConnect::SetBGColor(int color){
    bgcolor=color;
}

void EasyConnect::InitGU() {
    sceGuInit();
    sceGuStart(GU_DIRECT, list);
    sceGuDrawBuffer(GU_PSM_8888, (void*) 0, 512);
    sceGuDispBuffer(480, 272, (void*) 0x88000, 512);
    sceGuDepthBuffer((void*) 0x110000, 512);
    sceGuOffset(2048 - (480 / 2), 2048 - (272 / 2));
    sceGuViewport(2048, 2048, 480, 272);
    sceGuDepthRange(0xc350, 0x2710);
    sceGuScissor(0, 0, 480, 272);
    sceGuEnable(GU_SCISSOR_TEST);
    sceGuDepthFunc(GU_GEQUAL);
    sceGuEnable(GU_DEPTH_TEST);
    sceGuFrontFace(GU_CW);
    sceGuShadeModel(GU_SMOOTH);
    sceGuEnable(GU_CULL_FACE);
    sceGuEnable(GU_CLIP_PLANES);
    sceGuEnable(GU_BLEND);
    sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
    sceGuFinish();
    sceGuSync(0, 0);

    sceDisplayWaitVblankStart();
    sceGuDisplay(GU_TRUE);
}

void EasyConnect::TermGU(){
    sceGuTerm();
}

int EasyConnect::onStateChange(int state){
    return 0;
}

