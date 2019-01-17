/**
  *********************************************************************
  * @file    gui_font_port.c
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   �������ӿ�
  *********************************************************************
  * @attention
  * ����    :www.emXGUI.com
  *
  **********************************************************************
  */ 

#include	"emXGUI.h"
#include  "gui_drv_cfg.h"
#include "board.h" 
#include "gui_mem_port.h"
#include "gui_font_port.h"
#include "gui_resource_port.h"

/*===================================================================================*/
/*===================================================================================*/
#include <GUI_Font_XFT.h>

/**
  * @note  ��Ҫ��������뵽�ڲ�FLASH��
  * ����Ӧ�����������ļ���ӵ����̣�Ȼ���ڴ˴���������
  * ��ʹ��XFT_CreateFont��������
  */
extern const char GB2312_16_1BPP[];
extern const char GB2312_16_2BPP[];
extern const char GB2312_20_2BPP[];

extern const char GB2312_16_4BPP[];
extern const char GB2312_20_4BPP[];

extern const char ASCII_16_4BPP[];
extern const char ASCII_20_4BPP[];
extern const char ASCII_24_4BPP[];
extern const char ASCII_32_4BPP[];

HFONT hFont_SDCARD=NULL;
HFONT hFont_SDCARD_100=NULL;
HFONT DEFAULT_FONT  =NULL;

/*===================================================================================*/
#if (GUI_USE_EXTERN_FONT && (!GUI_FONT_LOAD_TO_RAM))

/**
  * @brief  ����ý��������ݵĻص�����
  * @param  buf[out] �洢��ȡ�������ݻ�����
  * @param  offset Ҫ��ȡ��λ��
  * @param  size Ҫ��ȡ�����ݴ�С
  * @param  lParam ���ú���ʱ���Զ���������û�������
  * @retval ��ȡ�������ݴ�С
  */
static int font_read_data_exFlash(void *buf,int offset,int size,LONG lParam)
{
  /* ��������offset�Ǿ����ַ������������ļ��е�ƫ�� 
   * lParam �������ļ���FLASH�еĻ���ַ
  */
	offset += lParam;
  
  /* ��ȡ�������ģ�������� */
	RES_DevRead(buf,offset,size);
	return size;
}
#endif

#if GUI_USE_SDCARD_FONT
BOOL font_read_data_SDCARD(char *res_name, char** buf, u32 size) 
{	 	
    /* file objects */
    FIL     *file;												
    FRESULT fresult;  
    BOOL result = TRUE;
    UINT br;
    file =(FIL*)GUI_VMEM_Alloc(sizeof(FIL));
     
    fresult = f_open(file, res_name, FA_OPEN_EXISTING | FA_READ );

    size = f_size(file);    
    /* �ļ����ݿռ� */
    *buf = (char *)GUI_VMEM_Alloc(size);
    fresult = f_read(file, *buf, size, &br);
    /* �ر��ļ� */
    f_close(file);

    
    /* �ͷſռ� */
    GUI_VMEM_Free(file);    
    
    return result;
}
#else
FIL *file;												
FRESULT fresult;  
BOOL result = TRUE;
UINT br;
int fsize;
int font_read_data_SDCARD(void *buf, int offset, int size,LONG lParam) 
{	 	     
//    /* �ļ����ݿռ� */
//    buf = (char *)GUI_VMEM_Alloc(size);   
      /* ��������ͼƬ�ļ� */
      fresult = f_read(file, buf, size, &br);
       
    
    /* �ͷſռ� */
    
    return result;
}
#endif
/**
  * @brief  GUIĬ�������ʼ��
  * @param  ��
  * @retval ����Ĭ������ľ��
  */
BOOL res;
HFONT GUI_Default_FontInit(void)
{

	HFONT hFont=NULL;

#if (GUI_FONT_LOAD_TO_RAM  )
  {  
    /* ���������ļ�������RAM */
    
    int font_base;
    
    /* ָ�򻺳�����ָ�� */
    static u8 *pFontData_XFT=NULL;
    CatalogTypeDef dir;
    
    /* RES_GetInfo��ȡ����dir.offset����Դ�ľ��Ե�ַ */
    font_base =RES_GetInfo_AbsAddr(GUI_DEFAULT_EXTERN_FONT, &dir);

    if(font_base > 0)
    {
    	pFontData_XFT =(u8*)GUI_VMEM_Alloc(dir.size);
      if(pFontData_XFT!=NULL)
      {
        RES_DevRead(pFontData_XFT, font_base, dir.size);

        hFont = XFT_CreateFont(pFontData_XFT);
      }
    }
  }
#elif (GUI_USE_EXTERN_FONT)   
  {
    /* ʹ�����豸�������壬����Ҫ��ȡ */
    if(hFont==NULL)
    { 
    	int font_base;
      CatalogTypeDef dir;

    	font_base =RES_GetInfo_AbsAddr(GUI_DEFAULT_EXTERN_FONT, &dir);
    	if(font_base > 0)
    	{
    		hFont =XFT_CreateFontEx(font_read_data_exFlash,font_base);
    	}
    }
  }
#elif (GUI_USE_SDCARD_FONT)  
  {
     /* ָ�򻺳�����ָ�� */
    static u8 *pFontData_XFT=NULL;
    static u8 *pFontData_XFT_100=NULL;
    static u8 *pFontData_XFT_DEFAULT=NULL;
    u32  	fsize;
 
    if(hFont_SDCARD==NULL)
    { 
      res = font_read_data_SDCARD(GUI_DEFAULT_SDCARD_FONT,(char **)&pFontData_XFT, fsize);   
      hFont_SDCARD = XFT_CreateFont(pFontData_XFT);
    }    
//    if(hFont_SDCARD_100==NULL)
//    { 
//      res = font_read_data_SDCARD(GUI_DEFAULT_SDCARD_100FONT,(char **)&pFontData_XFT_100, fsize);   
//      hFont_SDCARD_100 = XFT_CreateFont(pFontData_XFT_100);
//    }     
    if(DEFAULT_FONT==NULL)
    { 
      res = font_read_data_SDCARD(GUI_DEFAULT_EXTERN_FONT,(char **)&pFontData_XFT_DEFAULT, fsize);   
      DEFAULT_FONT = XFT_CreateFont(pFontData_XFT_DEFAULT);
    }
   
  }   
#endif

    /* ��ǰ����������ʧ�ܣ�ʹ���ڲ�FLASH�е����ݣ������е�C�������飩
    *  �����������ʱ���������ļ���ӵ����̣��ڱ��ļ�ͷ�����Ӧ���������������
    *  Ȼ�����XFT_CreateFont�����������弴��
    */
    if(hFont==NULL)
    { 
      /* �ӱ��ؼ���(������������) */    	
      hFont =XFT_CreateFont(GUI_DEFAULT_FONT);  /*ASCii�ֿ�,20x20,4BPP�����*/
      /* �����ֿ�洢ռ�ÿռ�ǳ��󣬲��Ƽ������ڲ�FLASH */
    	//hFont =XFT_CreateFont(GB2312_16_2BPP); /*GB2312�ֿ�,16x16,2BPP�����*/
    	//hFont =XFT_CreateFont(GB2312_20_4BPP); /*GB2312�ֿ�,20x20,4BPP�����*/
    }
    
	return DEFAULT_FONT;
}

/********************************END OF FILE****************************/

