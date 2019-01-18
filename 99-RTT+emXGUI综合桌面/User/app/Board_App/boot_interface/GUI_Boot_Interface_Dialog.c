
#include <stdio.h>
#include "emXGUI.h"
#include "x_libc.h"
#include "GUI_AppDef.h"




/**********************�ֽ���*********************/

/* ����ؼ�ID */
#define ID_LOGO            0x3000
#define ID_TEXT            0x3001

static COLORREF logo_col[4]={RGB888(169,169,169), RGB888(96,0,7), RGB888(220,20,60), RGB888(255,0,0)};

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
   static int col = 0;//ѡ����ɫ
	switch (msg)
	{  
      
      case WM_CREATE:
      {
         CreateWindow(TEXTBOX, L"B", WS_VISIBLE, 0,100,800,280,        
                      hwnd, ID_LOGO, NULL, NULL);
         SendMessage(GetDlgItem(hwnd, ID_LOGO),TBM_SET_TEXTFLAG,0,
                        DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_BKGND);   
         SetWindowFont(GetDlgItem(hwnd, ID_LOGO), logoFont_200);
         
         CreateWindow(TEXTBOX, L"tuvwxyz", WS_VISIBLE, 0,380,800,100,        
                      hwnd, ID_TEXT, NULL, NULL);
         SendMessage(GetDlgItem(hwnd, ID_TEXT),TBM_SET_TEXTFLAG,0,
                        DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_BKGND);   
         SetWindowFont(GetDlgItem(hwnd, ID_TEXT), controlFont_32);   

         SetTimer(hwnd,1, 500, TMR_START,NULL);   
         break;
      }
      case WM_TIMER:
      {
         static int state = -1;
         state++;
         switch(state)
         {
            case 0:
            {
               col = 1;
               InvalidateRect(GetDlgItem(hwnd, ID_LOGO), NULL, FALSE);
               SetWindowText(GetDlgItem(hwnd, ID_TEXT), L"tuvwxyz.");
               break;
            }
            case 1:
            {
               col = 2;
               InvalidateRect(GetDlgItem(hwnd, ID_LOGO), NULL, FALSE);
               SetWindowText(GetDlgItem(hwnd, ID_TEXT), L"tuvwxyz..");
               break;
            }
            case 2:
            { 
               col = 3;
               state = -1;
               InvalidateRect(GetDlgItem(hwnd, ID_LOGO), NULL, FALSE);
               SetWindowText(GetDlgItem(hwnd, ID_TEXT), L"tuvwxyz...");
               break;
            }          
         }
         
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
      case WM_CTLCOLOR:
      {
         u16 id;
         id =LOWORD(wParam);         
         CTLCOLOR *cr;
         cr =(CTLCOLOR*)lParam;
         
         switch(id)
         {
            case ID_LOGO:
            {
               cr->TextColor = logo_col[col];
               cr->BackColor = RGB888(0,0,0);            
               return TRUE;               
            }
            break;
            case ID_TEXT:
            {
               cr->TextColor = RGB888(169,169,169);
               cr->BackColor = RGB888(0,0,0);            
               return TRUE;               
            }            
         }
         break;
      }
      case	WM_PAINT: //������Ҫ�ػ���ʱ�����Զ��յ�����Ϣ.
      {	
         PAINTSTRUCT ps;
   //      HDC hdc;//��Ļhdc
   //      hdc = BeginPaint(hwnd, &ps); 
       BeginPaint(hwnd, &ps); 

         EndPaint(hwnd, &ps);
         return	TRUE;
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

  /* �ָ�����Ĭ������ */
  GUI_SetDefFont(defaultFont);  

}
