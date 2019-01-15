
#include <stdio.h>
#include "emXGUI.h"
#include "x_libc.h"
#include "./led/bsp_led.h"  
#include "GUI_AppDef.h"



/**********************�ֽ���*********************/

/*������ID�궨��*/
#define ID_SCROLLBAR_R  0x1100
#define ID_SCROLLBAR_G  0x1101
#define ID_SCROLLBAR_B  0x1102

/*��ѡ��ID�궨��*/
#define ID_TEXTBOX_R	      0x1010
#define ID_TEXTBOX_R_NUM	0x1013
#define ID_TEXTBOX_G	      0x1011
#define ID_TEXTBOX_G_NUM   0x1014
#define ID_TEXTBOX_B	      0x1012
#define ID_TEXTBOX_B_NUM   0x1015

#define ID_EXIT            0x3000
#define ID_BURN           0x3001
#define ID_RESET           0x3002
#define ID_INFO           0x3003



/**
  * @brief  ������Ƶ����
  * @param  hwnd����Ļ���ڵľ��
  * @retval ��
  * @notes  
  */
static int thread=0;

/**
  * @brief  ��¼Ӧ���߳�
  */
static rt_thread_t h_flash;

HWND info_textbox ;

static void App_FLASH_Writer(HWND hwnd)
{
	
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
    BurnFile();
    thread = 0;
    
	}
  return;
}

/**
  * @brief  ��¼Ӧ�ûص�����
  */
static	LRESULT	win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	RECT rc,rc0;
	HWND wnd;

	//static RECT rc_R, rc_G, rc_B;//RGB����ָʾ��

   //HDC hdc_mem2pic;
	switch (msg)
	{
    case WM_CREATE: {

          GetClientRect(hwnd,&rc); //��ô��ڵĿͻ�������.
      
          rc0.x = 5;
          rc0.y = 10;
          rc0.w = rc.w-10;
          rc0.h = 200;
          CreateWindow(TEXTBOX, L"It's seem that the FLASH is missing some resources\r\n\
      ,Please insert a SD card with 'srcdata',reboot the board\r\n \
      and than Click 'Burn File to FLASH' Button to load the resources!",WS_VISIBLE,
                        rc0.x, rc0.y, rc0.w, rc0.h, hwnd, ID_INFO, NULL, NULL); 

          OffsetRect(&rc0,0,rc0.h+10);  
          rc0.w = 250;
          rc0.h = 70;

          CreateWindow(BUTTON, L"Burn File to FLASH",WS_VISIBLE,
                        rc0.x, rc0.y, rc0.w, rc0.h, hwnd, ID_BURN, NULL, NULL); 

          OffsetRect(&rc0,rc0.w+50,0);  
          CreateWindow(BUTTON, L"Reset System",WS_VISIBLE,
                        rc0.x, rc0.y, rc0.w, rc0.h, hwnd, ID_RESET, NULL, NULL); 



      break;
	}
 
	case WM_NOTIFY: {
      NMHDR *nr;
      WCHAR wbuf[128];
      u16 ctr_id; 
      u16 code,  id;
      id  =LOWORD(wParam);//��ȡ��Ϣ��ID��
      code=HIWORD(wParam);//��ȡ��Ϣ������
      ctr_id = LOWORD(wParam); //wParam��16λ�Ƿ��͸���Ϣ�Ŀؼ�ID.
      nr = (NMHDR*)lParam; //lParam����������NMHDR�ṹ�忪ͷ.
//		if(id == ID_EXIT && code == BN_CLICKED)
//      {
//         PostCloseMessage(hwnd);
//      }
      
      if(id == ID_BURN && code == BN_CLICKED)
      {
//         rt_enter_critical();
         
         info_textbox = GetDlgItem(hwnd,ID_INFO);
         App_FLASH_Writer(hwnd);
        
//         rt_exit_critical();

      }
      
      if(id == ID_RESET && code == BN_CLICKED)
      {
         NVIC_SystemReset();
      }      
      
      
		break;
	}
	case WM_DRAWITEM:
	{
		DRAWITEM_HDR *ds;
		ds = (DRAWITEM_HDR*)lParam;

//      if(ds->ID == ID_EXIT)
//      {
//			exit_owner_draw(ds);
//			return TRUE;      
//      }

	}
   case	WM_CTLCOLOR:
   {
      /* �ؼ��ڻ���ǰ���ᷢ�� WM_CTLCOLOR��������.
       * wParam����ָ���˷��͸���Ϣ�Ŀؼ�ID;lParam����ָ��һ��CTLCOLOR�Ľṹ��ָ��.
       * �û�����ͨ������ṹ��ı�ؼ�����ɫֵ.�û��޸���ɫ�������践��TRUE������ϵͳ
       * �����Ա��β���������ʹ��Ĭ�ϵ���ɫ���л���.
       *
       */
			u16 id;
			id =LOWORD(wParam);
			if(id== ID_INFO )
			{
				CTLCOLOR *cr;
				cr =(CTLCOLOR*)lParam;
				cr->TextColor =RGB888(255,255,255);//������ɫ��RGB888��ɫ��ʽ)
				cr->BackColor =RGB888(0,0,0);//������ɫ��RGB888��ɫ��ʽ)
				cr->BorderColor =RGB888(255,10,10);//�߿���ɫ��RGB888��ɫ��ʽ)
				return TRUE;
			}
			else
			{
				return FALSE;
			}
      
   }   
   case WM_ERASEBKGND:
   {
      HDC hdc =(HDC)wParam;
      RECT rc;
      RECT rc_text = {0, 0, 100, 40};
      GetClientRect(hwnd, &rc);
      
      SetBrushColor(hdc, MapRGB(hdc, 0, 0, 0));
      FillRect(hdc, &rc);
      
      
      return TRUE;
      
   }

	case	WM_PAINT: //������Ҫ�ػ���ʱ�����Զ��յ�����Ϣ.
	{	
      PAINTSTRUCT ps;
      HDC hdc, hdc_mem, hdc_mem1;//��Ļhdc
      RECT rc = {0,0,72,72};
      RECT rc_cli = {0,0,72,72};
      GetClientRect(hwnd, &rc_cli);
      hdc = BeginPaint(hwnd, &ps); 

		EndPaint(hwnd, &ps);
		return	TRUE;
	}
	default:
		return	DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return	WM_NULL;
}


void	GUI_RES_WRITER_DIALOG(void)
{
	HWND	hwnd;
	WNDCLASS	wcex;
	MSG msg;


	wcex.Tag = WNDCLASS_TAG;

	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = win_proc; //������������Ϣ����Ļص�����.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);
	//����������
	hwnd = CreateWindowEx(WS_EX_LOCKPOS,
                        &wcex,
                        L"GUI FLASH Writer",
                        WS_OVERLAPPEDWINDOW,
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
