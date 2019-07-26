#include <emXGUI.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "ff.h"
#include "Widget.h"
#include "x_libc.h"
#include "GUI_AppDef.h"
#include "emXGUI_JPEG.h"
#include "emxgui_png.h"
#include  "GUI_ADC_CollectVoltage_Dialog.h"


icon_S GUI_ADCViewer_Icon[12] = 
{
  {"ADC_Name",           {100,0,600,35},        FALSE},
  {"ADC_MSGBOX",         {200,240,400,70},      FALSE},
  {"ADC_Res",            {130,35,100,35},       FALSE},
  {"ADC_Res_Value",      {230,35,100,35},       FALSE},
  {"ADC_Time",           {330,35,70,35},       FALSE}, 
  {"ADC_Time_Value",     {400,35,90,35},       FALSE},
  {"ADC_FPS",            {490,35,70,35},       FALSE}, 
  {"ADC_FPS_Value",      {560,35,90,35},       FALSE},  
  {"In_Flash",           {0,420,250,60},       FALSE},
  {"Ex_Flash",           {250,420,250,60},       FALSE}, 
  {"SD_Card",            {500,420,300,60},       FALSE}, 
  
};

/* ���� ID */
#define ID_ADV_WIN    0x1
#define ID_BKL_WIN    0x1

#define CircleCenter_1    (60)
#define CircleCenter_2    (80)
#define CircleCenter_3    (CircleCenter_2 + 10)
#define LeftToRight    0
#define RightToLeft    1
#define MOVE_WIN       1

#define CircleSize    240
#define Circle_X      500
#define Circle_Y      (50)

#define GUI_ADC_BACKGROUNG_PIC      "musicdesktop.jpg"

#define TitleHeight    70    // �������ĸ߶�

#define TriangleLen    20    // �����εı߳�

uint8_t AovingDirection = 0;
double count = 0.0;
HWND MAIN_Handle;
HWND BKL_Handle;
HWND ADC_Handle;

HDC bk_hdc;

// �ֲ����������ڱ���ת�������ĵ�ѹֵ 	 
double ADC_Vol; 

static void	X_MeterPointer(HDC hdc, int cx, int cy, int r, u32 color, double dat_val)
{
  double angle;
  double angle_buff;
  double angle_1;
  int   min_r;
  POINT pt[8];

  angle = (dat_val *3.14 /2.2 + 3.14 / 4);    // ����Ƕ�

  /* ��Զ��һ���� */
  pt[0].x = cx - r * sin(angle);
  pt[0].y = cy + r * cos(angle);

  /*  */
  angle_buff = atan((TriangleLen / 2) / (r - TriangleLen * sin(3.14 / 6)));
  min_r = sqrt(((r - TriangleLen * sin(3.14 / 6)) * (r - TriangleLen * sin(3.14 / 6))) + (TriangleLen / 2) * (TriangleLen / 2));

  angle_1 = angle - angle_buff;    // ����Ƕ�

  pt[1].x = cx - min_r * sin(angle_1);
  pt[1].y = cy + min_r * cos(angle_1);

  angle_1 = angle + angle_buff;    // ����Ƕ�

  pt[2].x = cx - min_r * sin(angle_1);
  pt[2].y = cy + min_r * cos(angle_1);

  pt[3].x = pt[0].x;
  pt[3].y = pt[0].y;


  /* �������� */
  SetBrushColor(hdc,color);
  EnableAntiAlias(hdc, TRUE);
  FillPolygon(hdc,0,0,pt,4);
  EnableAntiAlias(hdc, FALSE);
}

//�˳���ť�ػ���
static void CollectVoltage_ExitButton_OwnerDraw(DRAWITEM_HDR *ds)
{
	HWND hwnd;
  HDC hdc;
	RECT rc;
 // RECT rc_top={0,0,800,70};
	WCHAR wbuf[128];

	hwnd = ds->hwnd; 
	hdc = ds->hDC;   
	rc = ds->rc; 

	SetBrushColor(hdc, MapRGB(hdc, COLOR_DESKTOP_BACK_GROUND));
   
  FillCircle(hdc, rc.x+rc.w, rc.y, rc.w);
	//FillRect(hdc, &rc); //�þ�����䱳��

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

	GetWindowText(hwnd, wbuf, 128); //��ð�ť�ؼ�������
  rc.y = -10;
  rc.x = 16;
	DrawText(hdc, wbuf, -1, &rc, NULL);//��������(���ж��뷽ʽ)

  /* �ָ�Ĭ������ */
	SetFont(hdc, defaultFont);

}

/* �ػ�Բ����ʾ���� */
void Circle_Paint(HWND hwnd, HDC hdc)
{
  char  cbuf[128];
  WCHAR wbuf[128];
  RECT rc = {0, 0, CircleSize, CircleSize};

  // EnableAntiAlias(hdc, TRUE);

  SetBrushColor(hdc, MapRGB(hdc, 50, 50, 50));
  FillArc(hdc, CircleSize/2, CircleSize/2, CircleCenter_2, CircleCenter_3, -45, 225);

  SetBrushColor(hdc, MapRGB(hdc, 200, 200, 200));
  FillArc(hdc, CircleSize/2, CircleSize/2, CircleCenter_2+1, CircleCenter_3-1, -45, ((225 - (-45))) * ADC_Vol / 3.3  - 45);

  // EnableAntiAlias(hdc, FALSE);
  /* �������� */
  X_MeterPointer(hdc, CircleSize/2, CircleSize/2, CircleCenter_1, MapRGB(hdc,250,20,20), ADC_Vol);

  /* ʹ��Ĭ������ */
	SetFont(hdc, defaultFont);

  rc.w = 24*4;
  rc.h = 30;
  rc.x = CircleSize/2 - rc.w/2;
  rc.y = CircleSize/2 - rc.h/2;

  x_sprintf(cbuf, "%d%%", (int)(ADC_Vol/3.3*100));
  x_mbstowcs_cp936(wbuf, cbuf, 128);
  DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);    // ��������(���ж��뷽ʽ)
}

static LRESULT	ADCWinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)
  {
    case WM_CREATE:
    {
      RECT rc;
      GetClientRect(hwnd, &rc);
      Rheostat_Init();    // ��ʼ�� ADC

    CreateWindow(BUTTON,L"HHHHHH",WS_VISIBLE,
                      800+100, 200, 120, 50,
                      hwnd,1,NULL,NULL);

    SetTimer(hwnd, 2, 10, TMR_START, NULL);

      break;
    } 

    case WM_TIMER:
    {
      RECT rc;
      int tmr_id;

      tmr_id = wParam;
      GetWindowRect(hwnd, &rc);
      SetForegroundWindow(hwnd);

      if (tmr_id == MOVE_WIN)
      {
        if (AovingDirection == LeftToRight)
        {
          if (rc.x < 0)
          {
            OffsetRect(&rc, (rc.w >> 4), 0);
            rc.x = MIN(rc.x, 0);
            MoveWindow(hwnd, rc.x, rc.y, rc.w, rc.h, TRUE);
          }
          else
          {
            KillTimer(hwnd, 1);
          }
        }
        else if (AovingDirection == RightToLeft)
        {
          if (rc.x > -800)
          {
            OffsetRect(&rc, -(rc.w >> 4), 0);
            rc.x = MAX(rc.x, -800);
            MoveWindow(hwnd, rc.x, rc.y, rc.w, rc.h, TRUE);
          }
          else
          {
            KillTimer(hwnd, 1);
          }
        }
      }
      else if (tmr_id == 2)
      {
        RECT rc;
        static double ADC_buff = 0.0;
        double vol_buff = 0.0;
        static uint8_t xC = 0;

        vol_buff =(double) ADC_ConvertedValue/4096*(double)3.3; // ��ȡת����ADֵ
//        GUI_DEBUG("��ѹֵǰΪ��%f", ADC_Vol);
        #if 1

          if (xC++ < 10)
          {
            ADC_buff += vol_buff;
            break;
          }
          else
          {
            ADC_Vol = ADC_buff / ((double)(xC-1));
            ADC_buff = 0;
            xC = 0;
          }

        #else
          
        ADC_Vol = (double)(((int)(vol_buff * 10)) / 10.0);

        #endif
//        GUI_DEBUG("��ѹֵ��Ϊ��%f", ADC_Vol);
        
        rc.x = Circle_X;
        rc.y = Circle_Y;
        rc.w = CircleSize * 2;
        rc.h = CircleSize * 2;

        InvalidateRect(hwnd, &rc, FALSE);
      }
      
      
      break;
    }

    case WM_ERASEBKGND:
    {
      
      HDC hdc =(HDC)wParam;
      RECT rc = {0, TitleHeight, GUI_XSIZE, GUI_YSIZE - TitleHeight};

      ScreenToClient(hwnd, (POINT *)&rc, 1);
      BitBlt(hdc, rc.x, rc.y, GUI_XSIZE, rc.h, bk_hdc, 0, TitleHeight, SRCCOPY);

      return TRUE;
    }

    case WM_PAINT:
    {
      HDC hdc, hdc_mem;
      PAINTSTRUCT ps;
      RECT rc = {Circle_X, Circle_Y, 2, 2};


     ClientToScreen(hwnd, (POINT *)&rc, 1);

      hdc_mem = CreateMemoryDC(SURF_SCREEN, CircleSize, CircleSize);

      hdc = BeginPaint(hwnd, &ps);

      BitBlt(hdc_mem, 0, 0, CircleSize, CircleSize, bk_hdc, rc.x, rc.y, SRCCOPY);

      Circle_Paint(hwnd, hdc_mem);
      
      BitBlt(hdc, Circle_X, Circle_Y, CircleSize, CircleSize, hdc_mem, 0, 0, SRCCOPY);
      DeleteDC(hdc_mem);
      EndPaint(hwnd, &ps);

        break;
    } 

    case WM_NOTIFY:
    {
      // u16 code, id;
      // id  =LOWORD(wParam);//��ȡ��Ϣ��ID��
      // code=HIWORD(wParam);//��ȡ��Ϣ������    

      break;
    } 
 
    
    static int x_move;

    case WM_LBUTTONDOWN:
    {
      int x;
      x = LOWORD(lParam);
      RECT rc;
	  	GetWindowRect(hwnd, &rc);
      rc.x = x;
      ClientToScreen(hwnd, (POINT *)&rc, 1);
      x_move = rc.x;
//      GUI_DEBUG("���£�x = %d",x);
      break;
    }

    static int x_old;
    case WM_MOUSEMOVE:
    {
      int x;
      RECT rc;
      RECT Client_rc;
	  	GetWindowRect(hwnd, &rc);
      GetWindowRect(hwnd, &Client_rc);
      x = LOWORD(lParam);
      
      rc.x = x;
      ClientToScreen(hwnd, (POINT *)&rc, 1);
      OffsetRect(&Client_rc, rc.x - x_move, 0);

      Client_rc.x = MIN(Client_rc.x, 30);
      Client_rc.x = MAX(Client_rc.x, -800 - 30);
      
      MoveWindow(hwnd, Client_rc.x, Client_rc.y, Client_rc.w, Client_rc.h, TRUE);

      x_old = x_move;
      x_move = rc.x;

//      GUI_DEBUG("�ƶ���x = %d", x_old);
      break;
    }

    case WM_LBUTTONUP:
    { 
      int x;
      RECT Client_rc;
      RECT rc;
      
      x = LOWORD(lParam);
      rc.x = x;
      GetWindowRect(hwnd, &Client_rc);
      ClientToScreen(hwnd, (POINT *)&rc, 1);
//      GUI_DEBUG("̧��x = %d",rc.x);

      if (rc.x - x_old > 0)    // �������һ�
      {
        if (Client_rc.x > 0)
        {
          MoveWindow(hwnd, 0, Client_rc.y, Client_rc.w, Client_rc.h, TRUE);
        }
        else 
        {
          SetTimer(hwnd, 1, 5, TMR_START, NULL);
          AovingDirection = LeftToRight;
        }
      }
      else    // ��������
      {
        if (Client_rc.x < -800)
        {
          MoveWindow(hwnd, -800, Client_rc.y, Client_rc.w, Client_rc.h, TRUE);
        }
        else 
        {
          SetTimer(hwnd, 1, 5, TMR_START, NULL);
          AovingDirection = RightToLeft;
        }
      }
      
      break;
    } 

    case WM_DESTROY:
    {
      

      return PostQuitMessage(hwnd);	
    } 

    default:
      return	DefWindowProc(hwnd, msg, wParam, lParam);   
  }
}
 
static LRESULT	CollectVoltage_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)
  {
    case WM_CREATE:
    {
      RECT rc;
      GetClientRect(hwnd, &rc); 
            
      CreateWindow(BUTTON, L"O", WS_TRANSPARENT|BS_FLAT | BS_NOTIFY |WS_OWNERDRAW|WS_VISIBLE,
                  730, 0, 70, 70, hwnd, eID_ADC_EXIT, NULL, NULL); 

      WNDCLASS wcex;

      wcex.Tag	 		    = WNDCLASS_TAG;
      wcex.Style			  = CS_HREDRAW | CS_VREDRAW;
      wcex.lpfnWndProc	= (WNDPROC)ADCWinProc;
      wcex.cbClsExtra		= 0;
      wcex.cbWndExtra		= 0;
      wcex.hInstance		= NULL;
      wcex.hIcon			  = NULL;
      wcex.hCursor		  = NULL;
      
      rc.x = 0;
      rc.y = TitleHeight;
      rc.w = GUI_XSIZE*2;
      rc.h = GUI_YSIZE - TitleHeight * 2;
      ////����"ADC�ɼ�����"�Ŀؼ�.
      ADC_Handle = CreateWindowEx(WS_EX_NOFOCUS|WS_EX_FRAMEBUFFER ,&wcex,L"---",WS_CLIPCHILDREN|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_ADV_WIN,NULL,NULL);

      BOOL res;
      u8 *jpeg_buf;
      u32 jpeg_size;
      JPG_DEC *dec;
      res = RES_Load_Content(GUI_ADC_BACKGROUNG_PIC, (char**)&jpeg_buf, &jpeg_size);
      bk_hdc = CreateMemoryDC(SURF_SCREEN, GUI_XSIZE, GUI_YSIZE);
      if(res)
      {
        /* ����ͼƬ���ݴ���JPG_DEC��� */
        dec = JPG_Open(jpeg_buf, jpeg_size);

        /* �������ڴ���� */
        JPG_Draw(bk_hdc, 0, 0, dec);

        /* �ر�JPG_DEC��� */
        JPG_Close(dec);
      }
      /* �ͷ�ͼƬ���ݿռ� */
      RES_Release_Content((char **)&jpeg_buf);

    //  SetTimer(hwnd,2,0,TMR_SINGLE,NULL);

      break;
    } 
    case WM_TIMER:
    {
      
      
      break;
    }
    // case WM_ERASEBKGND:
    // {
      
    //   HDC hdc =(HDC)wParam;
    //   RECT rc =*(RECT*)lParam;
      
    //   SetBrushColor(hdc, MapRGB(hdc, 255, 0, 0));;
    //   FillRect(hdc, &rc);

    //   return TRUE;
    //   break;
    // }

    case WM_PAINT:
    {
      HDC hdc, hdc_mem;
      PAINTSTRUCT ps;
      //  RECT rc = {0,0,800,70};
      //  hdc_mem = CreateMemoryDC(SURF_ARGB4444, 800,70);
       
      hdc = BeginPaint(hwnd, &ps);

      
       
      //  SetBrushColor(hdc_mem, MapARGB(hdc_mem,100,105, 105, 105));
      //  FillRect(hdc_mem, &rc);

      BitBlt(hdc, 0, 0, GUI_XSIZE, GUI_YSIZE, bk_hdc, 0, 0, SRCCOPY);
      
      //  BitBlt(hdc, 0,0,800,70,hdc_mem,0,0,SRCCOPY);
      //  DeleteDC(hdc_mem);
      EndPaint(hwnd, &ps);

      break;
    } 
    case WM_DRAWITEM:
    {
       DRAWITEM_HDR *ds;
       ds = (DRAWITEM_HDR*)lParam;
       switch(ds->ID)
       {
          case eID_ADC_EXIT:
          {
            CollectVoltage_ExitButton_OwnerDraw(ds);
            return TRUE;             
          }      
       }
       break;
    }
    case WM_NOTIFY:
    {
      u16 code, id;
      id  =LOWORD(wParam);//��ȡ��Ϣ��ID��
      code=HIWORD(wParam);//��ȡ��Ϣ������    
      if(code == BN_CLICKED && id == eID_ADC_EXIT)
      {
        PostCloseMessage(hwnd);
        break;
      }

      break;
    } 

    static int x_move;

    case WM_LBUTTONDOWN:
    {
      int x;
      x = LOWORD(lParam);

      x_move = x;

      break;
    }

    case WM_MOUSEMOVE:
    {

      break;
    }

    case WM_LBUTTONUP:
    { 
      
      break;
    } 

    case WM_DESTROY:
    {
      Rheostat_DISABLE();    // ֹͣADC�Ĳɼ�
      DeleteDC(bk_hdc);
      return PostQuitMessage(hwnd);	
    } 

    default:
      return	DefWindowProc(hwnd, msg, wParam, lParam);   
  }
  
  return WM_NULL;
  
}

void GUI_ADC_CollectVoltage_Dialog(void)
{
	
	WNDCLASS	wcex;
	MSG msg;

	wcex.Tag = WNDCLASS_TAG;

	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = CollectVoltage_proc; //������������Ϣ����Ļص�����.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);
   
	//����������
	MAIN_Handle = CreateWindowEx(WS_EX_NOFOCUS|WS_EX_FRAMEBUFFER,
                              &wcex,
                              L"GUI_ADC_CollectVoltage_Dialog",
                              WS_VISIBLE|WS_CLIPCHILDREN,
                              0, 0, GUI_XSIZE, GUI_YSIZE,
                              NULL, NULL, NULL, NULL);
   //��ʾ������
	ShowWindow(MAIN_Handle, SW_SHOW);
	//��ʼ������Ϣѭ��(���ڹرղ�����ʱ,GetMessage������FALSE,�˳�����Ϣѭ��)��
	while (GetMessage(&msg, MAIN_Handle))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}  
}


