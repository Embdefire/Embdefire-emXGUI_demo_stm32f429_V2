
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

static void exit_owner_draw(DRAWITEM_HDR *ds) //����һ����ť���
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	WCHAR wbuf[128];

	hwnd = ds->hwnd; //button�Ĵ��ھ��.
	hdc = ds->hDC;   //button�Ļ�ͼ�����ľ��.
	rc = ds->rc;     //button�Ļ��ƾ�����.
   
	SetBrushColor(hdc, MapRGB(hdc, 0,0,0));   
   FillRect(hdc, &rc); //�þ�����䱳��
	SetBrushColor(hdc, MapRGB(hdc, COLOR_DESKTOP_BACK_GROUND));
   
   FillCircle(hdc, rc.x+rc.w, rc.y, rc.w);
	//

   if (ds->State & BST_PUSHED)
	{ //��ť�ǰ���״̬
//    GUI_DEBUG("ds->ID=%d,BST_PUSHED",ds->ID);
//		SetBrushColor(hdc,MapRGB(hdc,150,200,250)); //�������ɫ(BrushColor��������Fill���͵Ļ�ͼ����)
//		SetPenColor(hdc,MapRGB(hdc,250,0,0));        //���û���ɫ(PenColor��������Draw���͵Ļ�ͼ����)
		SetTextColor(hdc, MapRGB(hdc, 105, 105, 105));      //��������ɫ
	}
	else
	{ //��ť�ǵ���״̬
//		SetBrushColor(hdc,MapRGB(hdc,255,255,255));
//		SetPenColor(hdc,MapRGB(hdc,0,250,0));
		SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));
	}

	  /* ʹ�ÿ���ͼ������ */
	SetFont(hdc, controlFont_64);
	//  SetTextColor(hdc,MapRGB(hdc,255,255,255));

	GetWindowText(ds->hwnd, wbuf, 128); //��ð�ť�ؼ�������
   rc.y = -10;
   rc.x = 16;
	DrawText(hdc, wbuf, -1, &rc, NULL);//��������(���ж��뷽ʽ)


  /* �ָ�Ĭ������ */
	SetFont(hdc, defaultFont);

}

static	LRESULT	win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
//	RECT rc;
	HWND wnd;

	//static RECT rc_R, rc_G, rc_B;//RGB����ָʾ��

   //HDC hdc_mem2pic;
	switch (msg)
	{
	case WM_CREATE: {
		/*���û������Ĳ���*/
		SCROLLINFO sif;
      
		sif.cbSize = sizeof(sif);
		sif.fMask = SIF_ALL;
		sif.nMin = 0;
		sif.nMax = 255;
		sif.nValue = 0;
		sif.TrackSize = 30;
		sif.ArrowSize = 0;//20;


      CreateWindow(BUTTON, L"O",WS_OWNERDRAW|WS_VISIBLE,
                        730, 0, 70, 70, hwnd, ID_EXIT, NULL, NULL); 
    
      CreateWindow(BUTTON, L"Burn File to FLASH",WS_VISIBLE,
                    40, 40, 250, 70, hwnd, ID_BURN, NULL, NULL); 


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
		if(id == ID_EXIT && code == BN_CLICKED)
      {
         PostCloseMessage(hwnd);
      }
      
    if(id == ID_BURN )
      {
         BurnFile();
      }
      
      
		break;
	}
	case WM_DRAWITEM:
	{
		DRAWITEM_HDR *ds;
		ds = (DRAWITEM_HDR*)lParam;

      if(ds->ID == ID_EXIT)
      {
			exit_owner_draw(ds);
			return TRUE;      
      }

	}
//   case	WM_CTLCOLOR:
//   {
//      /* �ؼ��ڻ���ǰ���ᷢ�� WM_CTLCOLOR��������.
//       * wParam����ָ���˷��͸���Ϣ�Ŀؼ�ID;lParam����ָ��һ��CTLCOLOR�Ľṹ��ָ��.
//       * �û�����ͨ������ṹ��ı�ؼ�����ɫֵ.�û��޸���ɫ�������践��TRUE������ϵͳ
//       * �����Ա��β���������ʹ��Ĭ�ϵ���ɫ���л���.
//       *
//       */
//      u16 id;
//      id =LOWORD(wParam);         
//      CTLCOLOR *cr;
//      cr =(CTLCOLOR*)lParam;

//      
//      return FALSE;
//      
//   }   
   case WM_ERASEBKGND:
   {
      HDC hdc =(HDC)wParam;
      RECT rc;
      RECT rc_text = {0, 0, 100, 40};
      GetClientRect(hwnd, &rc);
      
      SetBrushColor(hdc, MapRGB(hdc, 0, 0, 0));
      FillRect(hdc, &rc);
      
      SetTextColor(hdc, MapRGB(hdc, 250, 250, 250));
      DrawText(hdc, L"FLASH Writer", -1, &rc_text, DT_VCENTER);
      
      return TRUE;
      
   }
/*�����ֿ���ɫָʾ�򣬸�ѡ�򣨿��أ�����ɫ����ֵ���������ؼ���ɣ�
 *λ�ù�ϵ��
 *	  ���ֿ�     ��ɫ      ��ɫ			��
 *										      ��
 *	  ��ѡ��    ָʾ��	    ����ֵ	   ��
*/ 
	case	WM_PAINT: //������Ҫ�ػ���ʱ�����Զ��յ�����Ϣ.
	{	
      PAINTSTRUCT ps;
      HDC hdc, hdc_mem, hdc_mem1;//��Ļhdc
      RECT rc = {0,0,72,72};
      RECT rc_cli = {0,0,72,72};
      GetClientRect(hwnd, &rc_cli);
      hdc = BeginPaint(hwnd, &ps); 
//      hdc_mem = CreateMemoryDC(SURF_SCREEN, 72, 72);
//      hdc_mem1 = CreateMemoryDC(SURF_SCREEN, 72, 72);
//      
//      
//      /****************���������水ť******************/
//      SetBrushColor(hdc, MapRGB(hdc, 0,0,0));
//      FillCircle(hdc, rc_cli.w, 0, 80);  
//      
//      SetBrushColor(hdc, MapRGB(hdc, 250,0,0));
//      FillCircle(hdc, rc_cli.w, 0, 76); 
//      //�����
//      SetBrushColor(hdc_mem1, MapRGB(hdc, 250,0,0));
//      FillRect(hdc_mem1, &rc);        
//      
//      SetFont(hdc, controlFont_72);
//      SetTextColor(hdc, MapRGB(hdc, 250, 250,250));
//      TextOut(hdc, 742, -10, L"O", -1);

////      StretchBlt(hdc, 755, 12, 40, 40, 
////                 hdc_mem1, 0, 0, 72, 72, SRCCOPY);

//      DeleteDC(hdc_mem);
//      DeleteDC(hdc_mem1);
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
	hwnd = CreateWindowEx(NULL,
                        &wcex,
                        L"GUI_FLASH_WRITER_DIALOG",
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
