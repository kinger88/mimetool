#include <stdio.h>
#include <string.h>
#include <string.h>
#include <stdlib.h>
#include "jpeglib.h"

#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

int width  = 960;
int height = 540;

int gCount = 0;

int get_Y_U_V(unsigned char*rData,unsigned char* in_Y,unsigned char* in_U,unsigned char* in_V,int nStride,int height)
{
	int i = 0;
	int y_n =0;
	int u_n =0;
	int v_n =0;
	int u = 0;
	int v = 2;
	long size = nStride*height*3/2;
	//使用yuv420格式读取yu不连续放置
	#if 1
	while(i < width*height)
	{
		in_Y[y_n] = rData[i];
		i++;
		y_n++;
	}
	while(i < width*height*5/4)
	{
		in_U[u_n] = rData[i];
		i++;
		u_n++;
	}
	while(i < width*height*3/2)
	{
		in_V[v_n] = rData[i];
		i++;
		v_n++;
	
	}
	#endif

	return 0;
}


    int yuv420p_to_yuv420sp(unsigned char * yuv420p,unsigned char* yuv420sp,int width,int height)  
    {  
        if(yuv420p==NULL)  
            return 0;  
        int i=0,j=0;  
        //Y  
        for(i=0;i<width*height;i++)  
        {  
            yuv420sp[i]=yuv420p[i];  
        }  
      
        int m=0,n=0;  
        for(j=0;j<width*height/2;j++)  
        {  
            if(j%2==0)  
	    {
               yuv420sp[j+width*height]=yuv420p[width*height+m];  
		m++;
	    }
            else 
		{
                   	 yuv420sp[j+width*height]=yuv420p[width*height*5/4+n];  
			n++;
		} 
        }  
    }  

    int yuv420p_to_jpeg(char * filename, char* pdata,int image_width,int image_height, int quality)  
    {     
        struct jpeg_compress_struct cinfo;    
        struct jpeg_error_mgr jerr;    
        cinfo.err = jpeg_std_error(&jerr);    
        jpeg_create_compress(&cinfo);    
      
        FILE * outfile;    // target file    
        if ((outfile = fopen(filename, "wb")) == NULL) {    
            fprintf(stderr, "can't open %s\n", filename);    
            return -1; 
        }    
        jpeg_stdio_dest(&cinfo, outfile);    
      
        cinfo.image_width = image_width;  // image width and height, in pixels    
        cinfo.image_height = image_height;    
        cinfo.input_components = 3;    // # of color components per pixel    
        cinfo.in_color_space = JCS_YCbCr;  //colorspace of input image    
        jpeg_set_defaults(&cinfo);    
        jpeg_set_quality(&cinfo, quality, TRUE );    
      
        //////////////////////////////    
        //  cinfo.raw_data_in = TRUE;    
        cinfo.jpeg_color_space = JCS_YCbCr;    
        cinfo.comp_info[0].h_samp_factor = 2;    
        cinfo.comp_info[0].v_samp_factor = 2;    
        /////////////////////////    
      
        jpeg_start_compress(&cinfo, TRUE);    
      
        JSAMPROW row_pointer[1];  
      
        unsigned char *yuvbuf;  
        if((yuvbuf=(unsigned char *)malloc(image_width*3))!=NULL)  
            memset(yuvbuf,0,image_width*3);  
      
        unsigned char *ybase,*ubase;  
        ybase=pdata;  
        ubase=pdata+image_width*image_height;    
        int j=0;  
        while (cinfo.next_scanline < cinfo.image_height)   
        {  
            int idx=0;  
	    int i;
            for(i=0;i<image_width;i++)  
            {   
                yuvbuf[idx++]=ybase[i + j * image_width];  
                yuvbuf[idx++]=ubase[j/2 * image_width+(i/2)*2];  
                yuvbuf[idx++]=ubase[j/2 * image_width+(i/2)*2+1];      
            }  
            row_pointer[0] = yuvbuf;  
            jpeg_write_scanlines(&cinfo, row_pointer, 1);  
            j++;  
        }  
        jpeg_finish_compress(&cinfo);    
        jpeg_destroy_compress(&cinfo);    
        fclose(outfile);    
	free(yuvbuf);
        return 0;    
    }  

int yuv420sp_saveto_jpeg(char*inFilePath, char*outFilePath) {
	unsigned char* rData = (unsigned char*)malloc(width*height*3/2);//存放yuv420p数据
	//unsigned char* pData = (unsigned char*)malloc(width*height*3/2);//存放yuv420sp数据
	
	unsigned long dwSize = 0;
	FILE *rfp = fopen(inFilePath, "rb");
	if(NULL == rfp) {
		printf("fopen file error!\n");
		return -1;
	}

	fread(rData, width*height*3/2, 1, rfp);	//读取yuv文件数据到rData里面
	//yuv420p_to_yuv420sp(rData,pData,width,height);

	int mUvlen	= width*height/2;
	int mYlen	= width*height;
	unsigned char* yuv = rData;
	int i = 0, uv = 0;
	while(i<mUvlen) {
			uv = mYlen+i;
			unsigned char tmp = yuv[uv];
			yuv[uv] = yuv[uv+1];
			yuv[uv+1] = tmp;
			i+=2;
	}


	yuv420p_to_jpeg(outFilePath, rData, width, height, 100);

	//free(pData);
	free(rData);
	if(rfp)
	      fclose(rfp);

	printf("count:%d\n", ++gCount);

	return 0;
}

void processClassDir(char *curDir, char*genDir)  
{  
        DIR              *pDir ;  
        struct dirent    *ent  ;  
        int               i=0  ;  
        char              childpath[512];
	char              newedpath[512];  
  
        pDir=opendir(curDir);  
        memset(childpath,0,sizeof(childpath));  
  
        while((ent=readdir(pDir))!=NULL)  
        {  
                if(ent->d_type & DT_DIR)  
                {  
                        if(strcmp(ent->d_name,".")==0 || strcmp(ent->d_name, "..")==0)  
                                continue;  
  
                        sprintf(childpath, "%s/%s", curDir, ent->d_name);  
                        printf("path:%s\n", childpath);  
  
			memset(newedpath, 0, 512);
                        sprintf(newedpath, "%s/%s", genDir, ent->d_name);  
                        if(access(newedpath,R_OK) !=0)
				if(mkdir(newedpath , 0766)==0) 
					printf("make dir:%s success\n", newedpath); 

                        processClassDir(childpath, newedpath);  
                }  
		else
		{
			char              oriFilename[512]={0}, genFilename[512]={0};
			sprintf(oriFilename, "%s/%s", curDir, ent->d_name); 
			//printf("oriFilename:%s\n", oriFilename);

			sprintf(genFilename, "%s/%s", genDir, ent->d_name); 
			int len = strlen(genFilename);
			memcpy(genFilename, genFilename, len-3);
			memcpy(genFilename+len-3, "jpg", 3);
			//printf("genFilename:%s len:%d\n", genFilename, len);

			yuv420sp_saveto_jpeg(oriFilename, genFilename);
		}
        }
}

void processNoClassDir(char *curDir, char*genDir)  
{  
        DIR              *pDir ;  
        struct dirent    *ent  ;  
        int               i=0  ;  
        char              childpath[512];
	char              newedpath[512];  
  
        pDir=opendir(curDir);  
        memset(childpath,0,sizeof(childpath));  
  
        while((ent=readdir(pDir))!=NULL)  
        {  
                if(ent->d_type & DT_DIR)  
                {  
                        if(strcmp(ent->d_name,".")==0 || strcmp(ent->d_name, "..")==0)  
                                continue;  
  
                        sprintf(childpath, "%s/%s", curDir, ent->d_name);  
                        printf("path:%s\n", childpath);  
  
                        processNoClassDir(childpath, genDir);  
                }  
		else
		{
			char              oriFilename[512]={0}, genFilename[512]={0};
			sprintf(oriFilename, "%s/%s", curDir, ent->d_name); 
			//printf("oriFilename:%s\n", oriFilename);

			sprintf(genFilename, "%s/%s", genDir, ent->d_name); 
			int len = strlen(genFilename);
			memcpy(genFilename, genFilename, len-3);
			memcpy(genFilename+len-3, "jpg", 3);
			//printf("genFilename:%s len:%d\n", genFilename, len);

			yuv420sp_saveto_jpeg(oriFilename, genFilename);
		}
        }
}

int main(int argc, char**argv)
{
/*
	char filename[100]={0};
	memcpy(filename, argv[1], 22);
	memcpy(filename+22, "jpg", 3);

	yuv420sp_saveto_jpeg(argv[1], filename);
*/
	if(argc<3) {
		printf("use exe srcdir destdir\n");
		return 1;
	}

	if(access(argv[2],R_OK) !=0)
		if(mkdir(argv[2] , 0766)==0) 
			printf("make dir:%s success\n", argv[2]);

	processClassDir(argv[1], argv[2]);
	return 0;
}























