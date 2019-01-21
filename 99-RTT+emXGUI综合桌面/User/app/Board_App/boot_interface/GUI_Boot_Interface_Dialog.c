
#include <stdio.h>
#include "emXGUI.h"
#include "x_libc.h"
#include "GUI_AppDef.h"
#include "emxgui_png.h"
#include "gui_font_port.h"


/**********************�ֽ���*********************/

/* ����ؼ�ID */
enum eID
{
   ID_LOGO,
   ID_TEXT1,
   ID_TEXT2,
   ID_PROGBAR,
};
/* �ⲿͼƬ���� */
extern char bootlogo[];
/* �ⲿͼƬ���ݴ�С */
extern unsigned int bootlogo_size(void);
HWND Boot_progbar;
/**
  * @brief  ��¼Ӧ���߳�
  */


/**
  * @brief  ��¼Ӧ�ûص�����
  */
static	LRESULT	win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   static  BITMAP png_bm;
   static  PNG_DEC *png_dec; 
   PROGRESSBAR_CFG cfg;
	switch (msg)
	{  
      case WM_CREATE:
      {
         RECT rc;
         
			GetClientRect(hwnd,&rc); //��ô��ڵĿͻ�������

         /* ����ͼƬ���ݴ���PNG_DEC��� */
         png_dec = PNG_Open((u8 *)bootlogo, bootlogo_size());
         /* ��ͼƬת����bitmap */
         PNG_GetBitmap(png_dec, &png_bm);
         CreateWindow(TEXTBOX, L"emXGUI booting...", WS_VISIBLE, 
                      50,260,800,40,
                      hwnd, ID_TEXT1, NULL, NULL);
         SendMessage(GetDlgItem(hwnd, ID_TEXT1),TBM_SET_TEXTFLAG,0,
                        DT_SINGLELINE|DT_RIGHT|DT_VCENTER|DT_BKGND); 
         
         CreateWindow(TEXTBOX, L"copying FontLIB form SPIFALSH to SDRAM...", WS_VISIBLE, 
                      50,300,800,40,
                      hwnd, ID_TEXT2, NULL, NULL);
         SendMessage(GetDlgItem(hwnd, ID_TEXT2),TBM_SET_TEXTFLAG,0,
                        DT_SINGLELINE|DT_RIGHT|DT_VCENTER|DT_BKGND); 


         //PROGRESSBAR_CFG�ṹ��Ĵ�С
         cfg.cbSize	 = sizeof(PROGRESSBAR_CFG);
         //�������еĹ���
         cfg.fMask    = PB_CFG_ALL;
         //���ָ�ʽˮƽ����ֱ����
         cfg.TextFlag = DT_VCENTER|DT_CENTER;  

         Boot_progbar = CreateWindow(PROGRESSBAR,L"",
                       PBS_TEXT|PBS_ALIGN_LEFT|WS_VISIBLE,
                       50, 380, 700, 40 ,hwnd,ID_PROGBAR,NULL,NULL);

         SendMessage(Boot_progbar,PBM_GET_CFG,TRUE,(LPARAM)&cfg);
			SendMessage(Boot_progbar,PBM_SET_CFG,TRUE,(LPARAM)&cfg);
         SendMessage(Boot_progbar,PBM_SET_RANGLE,TRUE, FONT_NUM);
         SendMessage(Boot_progbar,PBM_SET_VALUE,TRUE,0); 
         //SetTimer(hwnd, 1, 500, TMR_SINGLE|TMR_START, NULL);
           
         break;
      }
      case WM_TIMER:
      {
         //
         
         //GUI_Extern_FontInit();//�������嵽�ⲿ����
         break;
      }

      case WM_ERASEBKGND:
      {
         HDC hdc =(HDC)wParam;
         RECT rc =*(RECT*)lParam;
         
         SetBrushColor(hdc, MapRGB(hdc, 0, 0, 0));
         FillRect(hdc, &rc);           
         /* ��ʾͼƬ */
         DrawBitmap(hdc, 250, 80, &png_bm, NULL);     
         return TRUE;
         
      }

		      
		case	WM_CTLCOLOR:
		{
			u16 id;
         CTLCOLOR *cr;
			id =LOWORD(wParam);				
         cr =(CTLCOLOR*)lParam;
			if(id == ID_TEXT1 || id == ID_TEXT2)
			{

				cr->TextColor =RGB888(255,255,255);//������ɫ��RGB888��ɫ��ʽ)
				cr->BackColor =RGB888(0,0,0);//������ɫ��RGB888��ɫ��ʽ)
				//cr->BorderColor =RGB888(255,10,10);//�߿���ɫ��RGB888��ɫ��ʽ)
				return TRUE;
			}
         
         break;
		}  
		case WM_CLOSE: //��������ʱ�����Զ���������Ϣ����������һЩ��Դ�ͷŵĲ���.
		{
         /* �ر�PNG_DEC��� */
         PNG_Close(png_dec);
         DestroyWindow(hwnd); 
			return TRUE; //����PostQuitMessage��ʹ�������ڽ������˳���Ϣѭ��.
		}      
      default:
         return	DefWindowProc(hwnd, msg, wParam, lParam);
      }
      return	WM_NULL;                                     
}

HWND GUI_Boot_hwnd;
void	GUI_Boot_Interface_DIALOG(void)
{

	WNDCLASS	wcex;
	MSG msg;

	wcex.Tag 		    = WNDCLASS_TAG;

	wcex.Style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= win_proc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= NULL;//hInst;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;//LoadCursor(NULL, IDC_ARROW);

	//�������洰��.
	GUI_Boot_hwnd = CreateWindowEx(	WS_EX_LOCKPOS,
                              &wcex,
                              L"DESKTOP",
                              WS_VISIBLE|WS_CLIPCHILDREN|WS_OVERLAPPED,
                              0,0,GUI_XSIZE,GUI_YSIZE,
                              NULL,0,NULL,NULL);

	GUI_Printf("HWND_Desktop=%08XH\r\n",	GUI_Boot_hwnd);

	//��ʾ���洰��.
	ShowWindow(GUI_Boot_hwnd,SW_SHOW);

	//����ϵͳ�򿪹����ʾ(���԰�ʵ��������Ƿ���Ҫ).
//	ShowCursor(TRUE);

	while(GetMessage(&msg,GUI_Boot_hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}


}
