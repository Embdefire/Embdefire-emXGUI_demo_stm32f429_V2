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
extern HFONT iconFont;
/* ����ͼ������ */
extern HFONT controlFont;
/* ���� */
extern HFONT GB2312_32_Font;
  
extern HFONT hFont_SDCARD;
extern HFONT hFont_SDCARD_100;
extern HFONT DEFAULT_FONT;
extern HFONT ICON64_FONT;
extern HFONT ICON72_FONT;

HFONT GUI_Default_FontInit(void);
HFONT GUI_Init_Extern_Font(const char* res_name);

#ifdef	__cplusplus
}
#endif

#endif /* __GUI_FONT_PORT_H */
