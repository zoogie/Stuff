#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <3ds.h>
#include "checksum.h"
void *buffer;
int size=1024*128;

int dumpNVRAMbin();
int loadNVRAMbin();
u32 checkCRC16s(u8 *buf, bool fix);

int main(int argc, char **argv)
{
	u32 res=0;
	gfxInitDefault();
	consoleInit(GFX_TOP, NULL);
	res=cfgnorInit(1);
	if(res) printf("cfgnor failed!\n");
	res = CFGNOR_Initialize(1);
	if(res) printf("cfgnor initialize failed!\n");
    buffer=malloc(size); 
	if(!buffer) printf("Error allocating buffer\n");
	
    printf("3DS NVRAM tool - zoogie\n");
	printf("Press A to dump nvram.bin\n");
	printf("Press B to write nvram.bin to 3DS\n");
	printf("Press X to fix nvram.bin crc16s\n");
	
	printf("\n");
		
	// Main loop
	while (aptMainLoop())
	{
		hidScanInput();
		u32 kDown = hidKeysDown();

		if (kDown & KEY_START) break; // break in order to return to hbmenu
		
		if (kDown & KEY_A){
			res=cfgnorDumpFlash(buffer, size);
			dumpNVRAMbin();
			printf("dump %08lX\n", res);
		}
		if (kDown & KEY_B){
			loadNVRAMbin();
			res=cfgnorWriteFlash(buffer, size);
			printf("write %08lX\n", res);
		}
		if (kDown & KEY_X){
			res=loadNVRAMbin();
			printf("load %08lX\n", res);
			res=checkCRC16s(buffer, false);
			if(res){
				printf("nvram.bin has bad crc16s %08lX ", res);
				res=checkCRC16s(buffer, true);
				if(res){ 
					printf("- fix failed!\n");
				}
				else{
					printf("- fix success!\n");
					dumpNVRAMbin();
				}
			}
			else printf("nvram.bin is fine! %08lX\n", res);
		}

		// Flush and swap framebuffers
		gfxFlushBuffers();
		gfxSwapBuffers();

		//Wait for VBlank
		gspWaitForVBlank();
	}
	CFGNOR_Shutdown();
	cfgnorExit();

	gfxExit();
	return 0;
}


int dumpNVRAMbin(){
	Result res=0;
	
	FILE *out = fopen("nvram.bin","wb");
	if (out == NULL) {
		printf("Error opening nvram.bin\n"); 
		return 1;
	}
		
	res=fwrite(buffer,1,size,out);
	fclose(out);
	if(res != size){
		printf("Something went wrong writing data, aborting...\n");
		return 3;
	}
	
	return 0;
}
int loadNVRAMbin(){
	Result res=0;
	
	FILE *in = fopen("nvram.bin","rb");
	if (in == NULL){
		printf("Error opening nvram.bin\n"); 
		return 1;
	}
	
	res=fread(buffer,1,size,in);
	fclose(in);
	if(res != size){
		printf("Something went wrong loading data, aborting...\n");
		return 3;
	}

	return 0;
}

u32 checkCRC16s(u8 *buff, bool fix){
	u32 result=0;
	u32 offset=0x1f400;
	u16 messageCRC=0;
	u16 existingCRC=0;
	int i=0;
	
	for(i=0; i<9; i++){                         //wifi settings (slot 4,5,6  - 2 crc16s each  // slot 1,2,3 - 1 crc16 each)
		messageCRC=crc_16(buff+offset, 0xFE);   //every crc16 is spaced 0x100 apart
		existingCRC=*(u16*)(buff+offset+0xFE);
		if(fix){
			if(messageCRC != existingCRC) *(u16*)(buff+offset+0xFE)=messageCRC;
			existingCRC=*(u16*)(buff+offset+0xFE);
		}
		if(messageCRC != existingCRC) result |= 1<<i;
		//printf("%04X %04X\n",messageCRC, existingCRC);
		offset+=0x100;
	}
	
	offset=0x1fe00; //DS profile 1
	
	for(int j=0; j<2; j++){
		messageCRC=crc_modbus(buff+offset, 0x70);
		existingCRC=*(u16*)(buff+offset+0x72);
		if(fix){
			if(messageCRC != existingCRC) *(u16*)(buff+offset+0x72)=messageCRC;
			existingCRC=*(u16*)(buff+offset+0x72);
		}
		if(messageCRC != existingCRC) result |= 1<<i;
		//printf("%04X %04X\n",messageCRC, existingCRC);
		
		i++;
		messageCRC=crc_modbus(buff+offset+0x74, 0x8a);
		existingCRC=*(u16*)(buff+offset+0xfe);
		if(fix){
			if(messageCRC != existingCRC) *(u16*)(buff+offset+0xfe)=messageCRC;
			existingCRC=*(u16*)(buff+offset+0xfe);
		}
		if(messageCRC != existingCRC) result |= 1<<i;
		//printf("%04X %04X\n",messageCRC, existingCRC);
		i++;
		offset+=0x100; //DS profile 2
	}
	
	return result;
}