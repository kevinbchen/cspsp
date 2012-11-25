/*
 * PSP Software Development Kit - http://www.pspdev.org
 * -----------------------------------------------------------------------
 * Licensed under the BSD license, see LICENSE in PSPSDK root for details.
 *
 * main.c - Simple PRX example.
 *
 * Copyright (c) 2005 James Forshaw <tyranid@gmail.com>
 *
 * $Id: main.c 1531 2005-12-07 18:27:12Z tyranid $
 */
#include <pspkernel.h>
#include <stdio.h>
#include <pspsdk.h>

PSP_MODULE_INFO("rapmet", 0x1000, 1, 1);

int CheckTempAR() {
	//FILE *file = fopen("modlist.txt","w");

	SceModule* mod = sceKernelFindModuleByName("TempAR");
	if (mod) {
		return 1;
		/*SceUID modid = mod->modid;
		SceKernelModuleInfo info;
		memset(&info, 0, sizeof(info));
		info.size = sizeof(info);
		int ret = sceKernelQueryModuleInfo(modid, &info);
		if(ret >= 0) {
			printf("found tempar, killing\n");
			printf("UID: 0x%08X Attr: %04X - Name: %s\n", modid, info.attribute, info.name);
		}*/
	}
	else {
		return -1;
	}

	/*SceUID ids[100];
	int ret;
	int count;
	int i;

	memset(ids, 0, 100 * sizeof(SceUID));
	ret = sceKernelGetModuleIdList(ids, 100 * sizeof(SceUID), &count);
	if(ret >= 0) {
		printf("<Module List (%d modules)>\n", count);
		for(i = 0; i < count; i++) {
			//print_modinfo(ids[i]);

			SceKernelModuleInfo info;
			char szString2[256] = "";
			memset(&info, 0, sizeof(info));
			info.size = sizeof(info);
			ret = sceKernelQueryModuleInfo(ids[i], &info);
			if(ret >= 0) {
				if (stricmp(info.name,"tempar") == 0) {
					printf("found tempar, killing\n");
					printf("UID: 0x%08X Attr: %04X - Name: %s\n", ids[i], info.attribute, info.name);

				}

				//sprintf(szString2, &info.name);
				//fputs(info.name, file);
			}
		}
	}*/
	//fclose(file);
}

int module_start(SceSize args, void *argp)
{
        return 0;
}

int module_stop()
{
        return 0;
}
