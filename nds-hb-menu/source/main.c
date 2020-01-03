#include <3ds.h>
#include <stdlib.h>
#include <stdio.h>
//#include "libsvchax.h"

Result app_install(const char* path); //from FBI by Steveice10
Result file_install(const char *path, const char *destination);
Result app_delete(u64 titleId); 
void waitSec(int sec);
Result fileExist(char *file);

int main (void) {
	gfxInitDefault(); // Initialize services
	gfxSwapBuffers(); 
	
	Result res;
	const char *path="romfs:/bootstrap.cia";
	char productCode[64]={0};
	
	consoleInit(GFX_TOP,NULL);
	printf("nds-hb-menu installer\n\n");
	
	res=amInit();
	printf("aminit     %08X\n",(int)res);
	res = romfsInit();
	printf("romfsinit  %08X\n",(int)res);
	
	res = AM_GetTitleProductCode(MEDIATYPE_NAND, 0x0004800448424C41LL, productCode); //existence check
	if(res){
		printf("installing...\n");
		res=app_install(path);
		printf("result     %08X ",(int)res);
		if(res) printf("failure\n");
		else    printf("success!\n");
	}
	else printf("bootstrap present, skip install\n");
	
	if(!fileExist("sdmc:/BOOT.NDS")){
		printf("writing BOOT.NDS to sdmc:/ ...\n");
		res = file_install("romfs:/BOOT.NDS", "sdmc:/BOOT.NDS");
		if(res) printf("success %X bytes written\n", (int)res);
		else    printf("fail!!\n");
	}
	else printf("BOOT.NDS present, skip install\n");
	
	printf("cleanup... ");
	res=app_delete(0x0004000000715300LL);
	if(res) printf("failure\n");
	else    printf("success!\n");
	
	printf("\nwait for to reset\n");
	
	waitSec(3);
	
	APT_HardwareResetAsync();

	while(aptMainLoop()) {
        gspWaitForVBlank();
        hidScanInput();
        
        if(hidKeysDown() & KEY_START)
            break;
		if( (hidKeysDown() &  KEY_UP) && (hidKeysDown() &  KEY_Y) ) {
			//res=app_delete(0x000400000F800100LL); debugging stuff
			//printf("FBI deleted.\n%08X\n",(int)res);
			waitSec(1);
		}
            
        gfxFlushBuffers();
        gfxSwapBuffers();
    }
	
	gfxExit();
	return 0; // Return to hbmenu
}

Result app_install(const char* path) {
	
	u8 media=MEDIATYPE_NAND;  //sd
	u32 bufSize = 1024 * 1024; // 1MB
	void* buf = malloc(bufSize);
	u64 pos = 0;
	u32 bytesRead;
	Result res;
	
	AM_InitializeExternalTitleDatabase(false); //dont overwrite if db already exists
	
	FILE *f=fopen(path,"rb");
	if(!f) {
		res = 1;
		goto exit;
	}
	u64 size;

	fseek(f, 0, SEEK_END);
    size = ftell(f);
	fseek(f, 0, SEEK_SET);

	Handle ciaHandle;
	res=AM_StartCiaInstall(media, &ciaHandle);
	if(res) goto exit;
	
	FSFILE_SetSize(ciaHandle, size);

	for(pos=0; pos<size; pos+=bufSize){
		//FSFILE_Read(fileHandle, &bytesRead, pos, buf, bufSize);
		bytesRead = fread(buf, 1, bufSize, f);
		FSFILE_Write(ciaHandle, NULL, pos, buf, bytesRead, FS_WRITE_FLUSH);
		printf("\rprogress %d/%d MB",(int)(pos/bufSize+1),(int)(size/bufSize+1));
	}
	printf("\n");
	
	gfxFlushBuffers();
    gfxSwapBuffers();
    gspWaitForVBlank();

	res=AM_FinishCiaInstall(ciaHandle);

	exit:
	free(buf);
	fclose(f);
	return res;
}

Result file_install(const char *path, const char *destination){
	Result res;
	FILE *f=fopen(path,"rb");
	u8 *buf=(u8*)malloc(0x80000); //.5 MB
	fseek(f, 0, SEEK_END);
    int size = ftell(f);
	fseek(f, 0, SEEK_SET);
	fread(buf,1,size,f);
	fclose(f);
	
	f=fopen(destination,"wb");
	res = fwrite(buf,1,size,f);
	fclose(f);
	free(buf);
	return res;
}

Result app_delete(u64 titleId) {
	return AM_DeleteAppTitle(MEDIATYPE_SD, titleId);
}

void waitSec(int sec){
	for(int i=0;i<sec*60;i++){
		gfxFlushBuffers();
		gfxSwapBuffers();
		gspWaitForVBlank();
	}
}

Result fileExist(char *file){
	Result res=0;
	FILE *f=fopen(file,"r");
	if(f){
		res=1;
	    fclose(f);
	}
	return res;
}