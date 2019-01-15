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
extern const char LOGO_50_4BPP[];
extern const char ICON_100_4BPP[];
extern const char CONTROL_50_4BPP[];
extern const char CONTROL_60_8BPP[];
extern const char CONTROL_70_8BPP[];
extern const char CONTROL_80_8BPP[];
extern const char app_icon_200_200_4BPP[];


/* Ĭ������ */
HFONT defaultFont =NULL;

#if(GUI_ICON_LOGO_EN)  
/* logo���� */
HFONT logoFont =NULL;
/* ͼ������ */
HFONT iconFont_100 =NULL;
HFONT iconFont_200 =NULL;
/* ����ͼ������ */
HFONT controlFont_48 =NULL;
HFONT controlFont_64 =NULL;
HFONT controlFont_72 =NULL;
#endif

/* ���� */
HFONT GB2312_32_Font =NULL;




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

extern void BurnFile(void);

/**
  * @brief  ��ʼ���ⲿFLASH����
  * @param  res_name ������Դ����
  * @retval ����Ĭ������ľ��
  */
HFONT GUI_Init_Extern_Font(const char* res_name)
{
  /* ʹ�����豸�������壬����Ҫ��ȡ */

  int font_base;
  HFONT hFont;
  CatalogTypeDef dir;

  font_base =RES_GetInfo_AbsAddr(res_name, &dir);
  if(font_base > 0)
  {
    hFont =XFT_CreateFontEx(font_read_data_exFlash,font_base);
  }
  else
  {
    BurnFile();
    while(1);
    GUI_ERROR("Can not find RES:%s",res_name);
  }
  
  if(hFont==NULL)  
     GUI_ERROR("%s font create failed",res_name);

  return hFont;
}


/**
  * @brief  GUIĬ�������ʼ��
  * @param  ��
  * @retval ����Ĭ������ľ��
  */
HFONT GUI_Default_FontInit(void)
{

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

        defaultFont = XFT_CreateFont(pFontData_XFT);
      }
    }
  }
#elif (GUI_USE_EXTERN_FONT)   
  {
    /* ʹ�����豸�������壬����Ҫ��ȡ */
    if(defaultFont==NULL)
    { 
    	defaultFont =GUI_Init_Extern_Font(GUI_DEFAULT_EXTERN_FONT);
    }
  }
#endif

    /* ��ǰ����������ʧ�ܣ�ʹ���ڲ�FLASH�е����ݣ������е�C�������飩
    *  �����������ʱ���������ļ���ӵ����̣��ڱ��ļ�ͷ�����Ӧ���������������
    *  Ȼ�����XFT_CreateFont�����������弴��
    */
    if(defaultFont==NULL)
    { 
      /* �ӱ��ؼ���(������������) */    	
      defaultFont =XFT_CreateFont(GUI_DEFAULT_FONT);  /*ASCii�ֿ�,20x20,4BPP�����*/
      
      /* �����ֿ�洢ռ�ÿռ�ǳ��󣬲��Ƽ������ڲ�FLASH */
    	//defaultFont =XFT_CreateFont(GB2312_16_2BPP); /*GB2312�ֿ�,16x16,2BPP�����*/
    	//defaultFont =XFT_CreateFont(GB2312_20_4BPP); /*GB2312�ֿ�,20x20,4BPP�����*/
    }
#if(GUI_ICON_LOGO_EN)    
  #if 0 
    /* �ڲ����� */
    /* ����logo���� */  
    logoFont =  XFT_CreateFont(GUI_LOGO_FONT);
    /* ����ͼ������ */  
    iconFont_100 =  XFT_CreateFont(GUI_ICON_FONT_100);
    iconFont_300 =  XFT_CreateFont(GUI_ICON_FONT_300);
    /* ��������ͼ������ */  
    controlFont_48 =  XFT_CreateFont(GUI_CONTROL_FONT_48);
    /* ��������ͼ������ */  
    controlFont_64 =  XFT_CreateFont(GUI_CONTROL_FONT_64);
    /* ��������ͼ������ */  
    controlFont_72 =  XFT_CreateFont(GUI_CONTROL_FONT_72);

      
    if(logoFont==NULL)  
      GUI_ERROR("logoFont create failed");
        
    if(iconFont_100 ==NULL) 
      GUI_ERROR("iconFont_100 create failed");
    
    if(controlFont_64 ==NULL) 
      GUI_ERROR("controlFont_64 create failed");
    if(iconFont_300 ==NULL) 
      GUI_ERROR("iconFont_100 create failed");
  #else
    /*�ŵ��ⲿflash*/    
    {      
      /* ����logo���� */  
      logoFont =  GUI_Init_Extern_Font(GUI_LOGO_FONT);
      /* ����ͼ������ */  
      iconFont_100 =  GUI_Init_Extern_Font(GUI_ICON_FONT_100);        
      /* ��������ͼ������ */  
      controlFont_48 =  GUI_Init_Extern_Font(GUI_CONTROL_FONT_48); 
      /* ��������ͼ������ */  
      controlFont_64 =  GUI_Init_Extern_Font(GUI_CONTROL_FONT_64); 
      /* ��������ͼ������ */  
      controlFont_72 =  GUI_Init_Extern_Font(GUI_CONTROL_FONT_72); 
      iconFont_200 =  XFT_CreateFont(app_icon_200_200_4BPP); 
    }     

  #endif
    
#endif
   
#if 0    
  /* ���� */  
  {
    /* ʹ�����豸�������壬����Ҫ��ȡ */
    	int font_base;
      CatalogTypeDef dir;

    	font_base =RES_GetInfo_AbsAddr("GB2312_32_4BPP.xft", &dir);
    	if(font_base > 0)
    	{
    		GB2312_32_Font =XFT_CreateFontEx(font_read_data_exFlash,font_base);
    	}  
      
      if(GB2312_32_Font ==NULL) 
        GUI_ERROR("GB2312_32_4BPP create failed");
  }
#endif  

	return defaultFont;
}

/********************************END OF FILE****************************/

