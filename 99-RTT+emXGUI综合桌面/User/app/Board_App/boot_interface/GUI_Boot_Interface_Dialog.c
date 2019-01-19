
#include <stdio.h>
#include "emXGUI.h"
#include "x_libc.h"
#include "GUI_AppDef.h"
#include "emxgui_png.h"



/**********************�ֽ���*********************/

/* ����ؼ�ID */
#define ID_LOGO            0x3000
#define ID_TEXT            0x3001
/* �ⲿͼƬ���� */
extern char bootlogo[];
/* �ⲿͼƬ���ݴ�С */
extern unsigned int bootlogo_size(void);

/**
  * @brief  ��¼Ӧ���߳�
  */
static void App_FLASH_Writer(void )
{
  static int thread=0;
	static rt_thread_t h_flash;
  
   //HDC hdc;
   
	if(thread==0)
	{  
      h_flash=rt_thread_create("Flash writer",(void(*)(void*))App_FLASH_Writer,NULL,5*1024,1,5);
      thread =1;
      rt_thread_startup(h_flash);//�����߳�
      return;
	}
	while(thread) //�߳��Ѵ�����
	{     
    

    thread = 0;       

    rt_thread_delete(h_flash);

	}
  return;
}

/**
  * @brief  ��¼Ӧ�ûص�����
  */
static	LRESULT	win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   static  BITMAP png_bm;
   static  PNG_DEC *png_dec; 
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
         //SendMessage(GetDlgItem(hwnd, ID_LOGO),TBM_SET_TEXTFLAG,0,DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_BKGND);           
         break;
      }
      case WM_ERASEBKGND:
      {
         HDC hdc =(HDC)wParam;
         RECT rc =*(RECT*)lParam;
         
         SetBrushColor(hdc, MapRGB(hdc, 0, 0, 0));
         FillRect(hdc, &rc);      
         return TRUE;
         
      }
		case WM_PAINT: //������Ҫ����ʱ�����Զ���������Ϣ.
		{
			PAINTSTRUCT ps;
			HDC hdc;
//			WCHAR wbuf[128];
      
			hdc =BeginPaint(hwnd,&ps);


         /* ��ʾͼƬ */
         DrawBitmap(hdc, 0, 0, &png_bm, NULL);          


			EndPaint(hwnd,&ps);
         break;
		}
		      
//		case	WM_CTLCOLOR:
//		{
//			u16 id;
//         CTLCOLOR *cr;
//			id =LOWORD(wParam);				
//         cr =(CTLCOLOR*)lParam;
//			if(id == ID_LOGO)
//			{

//				cr->TextColor =RGB888(255,255,255);//������ɫ��RGB888��ɫ��ʽ)
//				cr->BackColor =RGB888(0,0,0);//������ɫ��RGB888��ɫ��ʽ)
//				//cr->BorderColor =RGB888(255,10,10);//�߿���ɫ��RGB888��ɫ��ʽ)
//				return TRUE;
//			}
//         
//         break;
//		}  
		case WM_DESTROY: //��������ʱ�����Զ���������Ϣ����������һЩ��Դ�ͷŵĲ���.
		{
         /* �ر�PNG_DEC��� */
         PNG_Close(png_dec);
      
			return PostQuitMessage(hwnd); //����PostQuitMessage��ʹ�������ڽ������˳���Ϣѭ��.
		}      
      default:
         return	DefWindowProc(hwnd, msg, wParam, lParam);
      }
      return	WM_NULL;                                     
}


void	GUI_Boot_Interface_DIALOG(void)
{
	HWND	hwnd;
	WNDCLASS	wcex;
	MSG msg;

  /* ����Ĭ������ΪASCII �ڲ�flash�ֿ⣬��ֹ����ʱ���� */
  GUI_SetDefFont(defaultFontEn);  //����Ĭ�ϵ�����

	wcex.Tag = WNDCLASS_TAG;

	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = win_proc; //������������Ϣ����Ļص�����.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);
	//����������
	hwnd = CreateWindowEx(WS_EX_FRAMEBUFFER,
                        &wcex,
                        L"GUI Boot Interface",
                        WS_CLIPCHILDREN,
                        0, 0, GUI_XSIZE, GUI_YSIZE,
                        NULL, NULL, NULL, NULL);
   //��ʾ������
	ShowWindow(hwnd, SW_SHOW);
	//��ʼ������Ϣѭ��(���ڹرղ�����ʱ,GetMessage������FALSE,�˳�����Ϣѭ��)��
	while (GetMessage(&msg, hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}


}
