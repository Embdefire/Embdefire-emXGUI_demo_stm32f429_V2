#ifndef __GUI_FONT_PORT_H
#define	__GUI_FONT_PORT_H

#ifdef __cplusplus
extern "C" {
#endif 
  
#include	"emXGUI.h"
#include "stm32f4xx.h"
#include	"gui_drv_cfg.h"

/* Ĭ������ */
extern HFONT defaultFont;
/* logo���� */
extern HFONT logoFont;
/* ͼ������ */
extern HFONT iconFont_100;
/* ����ͼ������ */
extern HFONT controlFont_48;
extern HFONT controlFont_64;
extern HFONT controlFont_72;
/* ���� */
extern HFONT GB2312_32_Font;
  

HFONT GUI_Default_FontInit(void);
HFONT GUI_Init_Extern_Font(const char* res_name);

#ifdef	__cplusplus
}
#endif

#endif /* __GUI_FONT_PORT_H */
