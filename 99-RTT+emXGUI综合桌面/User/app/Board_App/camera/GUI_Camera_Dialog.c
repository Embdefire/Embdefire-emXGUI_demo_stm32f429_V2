#include <emXGUI.h>
#include "./camera/bsp_ov5640.h"
#include "x_libc.h"
#include "./camera/ov5640_AF.h"

#define ID_BUTTON_Exit  0x1000
#define FONT_H          72
#define FONT_W          72
extern BOOL g_dma2d_en;
rt_thread_t h1;
rt_thread_t h2;
BOOL update_flag = 0;//֡�ʸ��±�־
static RECT win_rc;
static int b_close=FALSE;
uint8_t fps=0;//֡��
OV5640_IDTypeDef OV5640_Camera_ID;
RECT rc_fps = {0,400,800,72};//֡����ʾ�Ӵ���
HWND Cam_hwnd;//�����ھ��
HWND SetWIN;//�������ô���
int state = 0;
U16 *bits;
GUI_SEM *cam_sem = NULL;//ͬ���ź�������ֵ�ͣ�
GUI_SEM *set_sem = NULL;//ͬ���ź�������ֵ�ͣ�
int focus_status = 1;//Ĭ��û�б�����
//����ؼ�ID
enum eID
{
	eID_OK =0x1000,
	eID_SET,             //���ü�������ͷ���ڣ�
	eID_SET1,            //�Զ��Խ�
	eID_SET2,            //����
	eID_SET3,            //���Ͷ�
	eID_SET4,            //�Աȶ�
   eID_SET5,            //�ֱ���
   eID_SET6,            //����ģʽ
   eID_SET7,            //����Ч��
   eID_SCROLLBAR,       //���Ȼ�����
   eID_SCROLLBAR1,      //���ͶȻ�����
   eID_SCROLLBAR2,      //�ԱȶȻ�����
   //��ѡ��---�ֱ���
   eID_RB1,             //320*240
   eID_RB2,             //480*272
   eID_RB3,             //800*480��Ĭ�ϣ�
   //��ѡ��---����ģʽ
   eID_RB4,             //�Զ�
   eID_RB5,             //����
   eID_RB6,             //����
   eID_RB7,             //�칫��
   eID_RB8,             //����
   //��ѡ��---����Ч��
   eID_RB9,              //��ɫ
   eID_RB10,             //ůɫ
   eID_RB11,             //�ڰ�
   eID_RB12,             //����
   eID_RB13,             //��ɫ   
   eID_RB14,             //ƫ��
   eID_RB15,             //����
   eID_RB16,             //����  

   eID_TB1,             //��ǰ�ֱ�����ʾ
   eID_TB2,             //��ǰ����ģʽ��ʾ   
   eID_TB3,             //��ǰ����Ч����ʾ
   
   eID_switch,          //�Զ��Խ�����
   eID_Setting1,        //���÷ֱ��ʰ�ť
   eID_Setting2,        //���ù���ģʽ��ť
   eID_Setting3,        //��������Ч����ť
   
   eID_BT1,             //�ֱ��ʽ��淵�ذ���
   eID_BT2,             //����ģʽ���淵�ذ���
   eID_BT3,             //����Ч�����淵�ذ���
};
static int flag = 0;

static BOOL cbErase(HDC hdc, const RECT* lprc,HWND hwnd)
{
   SetBrushColor(hdc, MapRGB(hdc,0,0,0));
   FillRect(hdc, lprc);
   return TRUE;
}

/*
 * @brief  ���ƹ�����
 * @param  hwnd:   �������ľ��ֵ
 * @param  hdc:    ��ͼ������
 * @param  back_c��������ɫ
 * @param  Page_c: ������Page������ɫ
 * @param  fore_c���������������ɫ
 * @retval NONE
*/
static void draw_scrollbar(HWND hwnd, HDC hdc, COLOR_RGB32 back_c, COLOR_RGB32 Page_c, COLOR_RGB32 fore_c)
{
	RECT rc;
   RECT rc_scrollbar;
	GetClientRect(hwnd, &rc);
	/* ���� */
	SetBrushColor(hdc, back_c);
	FillRect(hdc, &rc);

   rc_scrollbar.x = rc.x;
   rc_scrollbar.y = rc.h/2;
   rc_scrollbar.w = rc.w;
   rc_scrollbar.h = 2;
   
	SetBrushColor(hdc, MapRGB888(hdc, Page_c));
	FillRect(hdc, &rc_scrollbar);

	/* ���� */
	SendMessage(hwnd, SBM_GETTRACKRECT, 0, (LPARAM)&rc);

	SetBrushColor(hdc, MapRGB(hdc, 169, 169, 169));
	//rc.y += (rc.h >> 2) >> 1;
	//rc.h -= (rc.h >> 2);
	/* �߿� */
	//FillRoundRect(hdc, &rc, MIN(rc.w, rc.h) >> 2);
	FillCircle(hdc, rc.x + rc.w / 2, rc.y + rc.h / 2, rc.h / 2);
   InflateRect(&rc, -2, -2);

	SetBrushColor(hdc, MapRGB888(hdc, fore_c));
	FillCircle(hdc, rc.x + rc.w / 2, rc.y + rc.h / 2, rc.h / 2);
   //FillRoundRect(hdc, &rc, MIN(rc.w, rc.h) >> 2);
}
/*
 * @brief  �Զ��廬�������ƺ���
 * @param  ds:	�Զ�����ƽṹ��
 * @retval NONE
*/
static void scrollbar_owner_draw(DRAWITEM_HDR *ds)
{
	HWND hwnd;
	HDC hdc;
	HDC hdc_mem;
	HDC hdc_mem1;
	RECT rc;
	RECT rc_cli;
	//	int i;

	hwnd = ds->hwnd;
	hdc = ds->hDC;
	GetClientRect(hwnd, &rc_cli);

	hdc_mem = CreateMemoryDC(SURF_SCREEN, rc_cli.w, rc_cli.h);
	hdc_mem1 = CreateMemoryDC(SURF_SCREEN, rc_cli.w, rc_cli.h);   	
	//���ư�ɫ���͵Ĺ�����
	draw_scrollbar(hwnd, hdc_mem1, RGB888( 0, 0, 0), RGB888( 250, 250, 250), RGB888( 255, 255, 255));
	//������ɫ���͵Ĺ�����
	draw_scrollbar(hwnd, hdc_mem, RGB888( 0, 0, 0), RGB888( 250, 0, 0), RGB888( 250, 0, 0));
   SendMessage(hwnd, SBM_GETTRACKRECT, 0, (LPARAM)&rc);   

	//��
	BitBlt(hdc, rc_cli.x, rc_cli.y, rc.x, rc_cli.h, hdc_mem, 0, 0, SRCCOPY);
	//��
	BitBlt(hdc, rc.x + rc.w, 0, rc_cli.w - (rc.x + rc.w) , rc_cli.h, hdc_mem1, rc.x + rc.w, 0, SRCCOPY);

	//���ƻ���
	if (ds->State & SST_THUMBTRACK)//����
	{
      BitBlt(hdc, rc.x, 0, rc.w, rc_cli.h, hdc_mem1, rc.x, 0, SRCCOPY);
		
	}
	else//δѡ��
	{
		BitBlt(hdc, rc.x, 0, rc.w+1, rc_cli.h, hdc_mem, rc.x, 0, SRCCOPY);
	}
	//�ͷ��ڴ�MemoryDC
	DeleteDC(hdc_mem1);
	DeleteDC(hdc_mem);
}

static void Checkbox_owner_draw(DRAWITEM_HDR *ds) //����һ����ť���
{
	//	HWND hwnd;
	HDC hdc;
	RECT rc;
	WCHAR wbuf[128];

	//	hwnd =ds->hwnd; //button�Ĵ��ھ��.
	hdc = ds->hDC;   //button�Ļ�ͼ�����ľ��.
	rc = ds->rc;     //button�Ļ��ƾ�����.
	SetBrushColor(hdc, MapRGB(hdc, 0, 0, 0)); //�������ɫ(BrushColor��������Fill���͵Ļ�ͼ����)
	FillRect(hdc, &rc);

	if (focus_status==1)
	{ //��ť�ǰ���״̬
		SetBrushColor(hdc, MapRGB(hdc, 119, 136, 153)); //�������ɫ(BrushColor��������Fill���͵Ļ�ͼ����)
		FillRoundRect(hdc, &rc, rc.h / 2); //�þ�����䱳��
		InflateRect(&rc, -3, -3);

		SetBrushColor(hdc, MapRGB(hdc, 60, 179, 113)); //�������ɫ(BrushColor��������Fill���͵Ļ�ͼ����)
		FillRoundRect(hdc, &rc, rc.h / 2); //�þ�����䱳��

		GetClientRect(ds->hwnd, &rc);
		SetBrushColor(hdc, MapRGB(hdc, 119, 136, 153)); //�������ɫ(BrushColor��������Fill���͵Ļ�ͼ����)
		FillCircle(hdc, rc.w - 15, 15, 15);


		SetBrushColor(hdc, MapRGB(hdc, 250, 250, 250)); //�������ɫ(BrushColor��������Fill���͵Ļ�ͼ����)
		FillCircle(hdc, rc.w - 15, 15, 12);
	}
	else
	{ //��ť�ǵ���״̬
		SetBrushColor(hdc, MapRGB(hdc, 119, 136, 153)); //�������ɫ(BrushColor��������Fill���͵Ļ�ͼ����)
		FillRoundRect(hdc, &rc, rc.h/2); //�þ�����䱳��
		InflateRect(&rc, -3,  -3);

		SetBrushColor(hdc, MapRGB(hdc, 250, 250, 250)); //�������ɫ(BrushColor��������Fill���͵Ļ�ͼ����)
		FillRoundRect(hdc, &rc, rc.h / 2); //�þ�����䱳��

		GetClientRect(ds->hwnd, &rc);
		SetBrushColor(hdc, MapRGB(hdc, 119, 136, 153)); //�������ɫ(BrushColor��������Fill���͵Ļ�ͼ����)
		FillCircle(hdc, rc.x + 15, 15, 15);

		
		SetBrushColor(hdc, MapRGB(hdc, 250, 250, 250)); //�������ɫ(BrushColor��������Fill���͵Ļ�ͼ����)
		FillCircle(hdc, rc.x + 15, 15, 12);

	}


}
static void Button_owner_draw(DRAWITEM_HDR *ds) //����һ����ť���
{
	//	HWND hwnd;
	HDC hdc;
	RECT rc;
	WCHAR wbuf[128];

	//	hwnd =ds->hwnd; //button�Ĵ��ھ��.
	hdc = ds->hDC;   //button�Ļ�ͼ�����ľ��.
	rc = ds->rc;     //button�Ļ��ƾ�����.
	SetBrushColor(hdc, MapRGB(hdc, 0, 0, 0)); //�������ɫ(BrushColor��������Fill���͵Ļ�ͼ����)
	FillRect(hdc, &rc);
   
	if(ds->State & BST_PUSHED)
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
		SetTextColor(hdc, MapRGB(hdc, 250, 250, 250));
	}
   GetWindowText(ds->hwnd, wbuf, 128); //��ð�ť�ؼ�������
   DrawText(hdc, wbuf, -1, &rc, DT_VCENTER | DT_LEFT);//��������(���ж��뷽ʽ)

}

static void button_owner_draw(DRAWITEM_HDR *ds) //����һ����ť���
{
	//	HWND hwnd;
	HDC hdc;
	RECT rc;
	WCHAR wbuf[128];

	//	hwnd =ds->hwnd; //button�Ĵ��ھ��.
	hdc = ds->hDC;   //button�Ļ�ͼ�����ľ��.
	rc = ds->rc;     //button�Ļ��ƾ�����.
	SetBrushColor(hdc, MapRGB(hdc, 0, 0, 0)); //�������ɫ(BrushColor��������Fill���͵Ļ�ͼ����)
	FillRect(hdc, &rc);
   
	if(ds->State & BST_PUSHED)
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
		SetTextColor(hdc, MapRGB(hdc, 250, 250, 250));
	}
   GetWindowText(ds->hwnd, wbuf, 128); //��ð�ť�ؼ�������
   DrawText(hdc, wbuf, -1, &rc, DT_VCENTER | DT_LEFT);//��������(���ж��뷽ʽ)

}


static void BtCam_owner_draw(DRAWITEM_HDR *ds) //����һ����ť���
{
	//	HWND hwnd;
	HDC hdc;
	RECT rc;
	WCHAR wbuf[128];
   HFONT hfont_old;
	//	hwnd =ds->hwnd; //button�Ĵ��ھ��.
	hdc = ds->hDC;   //button�Ļ�ͼ�����ľ��.
	rc = ds->rc;     //button�Ļ��ƾ�����.
	SetBrushColor(hdc, MapRGB(hdc, 0, 0, 0)); //�������ɫ(BrushColor��������Fill���͵Ļ�ͼ����)
	FillRect(hdc, &rc);
   hfont_old = SetFont(hdc, controlFont_48);
	if(ds->State & BST_PUSHED)
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
		SetTextColor(hdc, MapRGB(hdc, 250, 250, 250));
	}
   GetWindowText(ds->hwnd, wbuf, 128); //��ð�ť�ؼ�������
   DrawText(hdc, wbuf, -1, &rc, DT_VCENTER | DT_LEFT);//��������(���ж��뷽ʽ)
   rc.x = 35;
   rc.y = 5;
   SetFont(hdc, hfont_old);
   DrawText(hdc, L"����", -1, &rc, DT_VCENTER | DT_LEFT);//��������(���ж��뷽ʽ)
   

}
static void Update_Dialog()
{
	int app=0;

	while(1) //�߳��Ѵ�����
	{
		if(app==0)
		{
         app=1;
			GUI_SemWait(cam_sem, 0xFFFFFFFF);
         InvalidateRect(Cam_hwnd,NULL,FALSE);
			app=0;
		}
	}
}
/**
  * @brief  ���������б����
  * @param  ��
  * @retval ��
  * @notes  
  */
static void Set_AutoFocus()
{
	static int thread=0;
	static int app=0;
   rt_thread_t h1;
	if(thread==0)
	{  
      h1=rt_thread_create("Set_AutoFocus",(void(*)(void*))Set_AutoFocus,NULL,1024*2,5,5);
      rt_thread_startup(h1);				
      thread =1;
      return;
	}
	while(thread==1) //�߳��Ѵ�����
	{
         GUI_SemWait(set_sem, 0xFFFFFFFF);
         if(focus_status != 1)
         {
            //��ͣ�Խ�
            OV5640_FOCUS_AD5820_Pause_Focus();
            
         }
         else
         {
            //�Զ��Խ�
            OV5640_FOCUS_AD5820_Constant_Focus();
           
         } 
	
			GUI_Yield();
		
	}
}
/*============================================================================*/
//���÷ֱ���
int cur_Resolution = eID_RB3;
static LRESULT	dlg_set_Resolution_WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
		case WM_CREATE: //���ڴ���ʱ,���Զ���������Ϣ,��������һЩ��ʼ���Ĳ����򴴽��Ӵ���.
		{
         RECT rc;
         GetClientRect(hwnd, &rc);
			rc.x =35;
			rc.y =55;
			rc.w =200;
			rc.h =24;
         CreateWindow(BUTTON,L"320*240",BS_RADIOBOX|WS_VISIBLE,
                      rc.x,rc.y,rc.w,rc.h,hwnd,(1<<16)|eID_RB1,NULL,NULL);
         OffsetRect(&rc,0,rc.h+10);
         CreateWindow(BUTTON,L"480*272",BS_RADIOBOX|WS_VISIBLE,
                      rc.x,rc.y,rc.w,rc.h,hwnd,(1<<16)|eID_RB2,NULL,NULL);         
         OffsetRect(&rc,0,rc.h+10);
         CreateWindow(BUTTON,L"800*480(Ĭ��)",BS_RADIOBOX|WS_VISIBLE,
                      rc.x,rc.y,rc.w,rc.h,hwnd,(1<<16)|eID_RB3,NULL,NULL);   
         
         switch(cur_Resolution)
         {
            case eID_RB1:
            {
               SendMessage(GetDlgItem(hwnd, eID_RB1),BM_SETSTATE,BST_CHECKED,0);
               break;
            }
            case eID_RB2:
            {
               SendMessage(GetDlgItem(hwnd, eID_RB2),BM_SETSTATE,BST_CHECKED,0);
               break;
            }    
            case eID_RB3:
            {
               SendMessage(GetDlgItem(hwnd, eID_RB3),BM_SETSTATE,BST_CHECKED,0);
               break;
            }              
         }   
         CreateWindow(BUTTON,L"FYZ",WS_OWNERDRAW|WS_VISIBLE,
                      5,0,100,40,hwnd,eID_BT1,NULL,NULL); 
         SetWindowFont(GetDlgItem(hwnd, eID_BT1), controlFont_48);         
		
         SetWindowEraseEx(hwnd, cbErase, TRUE);
      
      }
		return TRUE;
		////
		case WM_PAINT: //������Ҫ����ʱ�����Զ���������Ϣ.
		{
			PAINTSTRUCT ps;
			HDC hdc;
			RECT rc;
			hdc =BeginPaint(hwnd,&ps); //��ʼ��ͼ

			////�û��Ļ�������...
         GetClientRect(hwnd, &rc);
			SetTextColor(hdc,MapRGB(hdc,250,250,250));
			SetBrushColor(hdc,MapRGB(hdc,0,0,0));
         FillRect(hdc, &rc);


			//TextOut(hdc,10,10,L"Hello",-1);

			EndPaint(hwnd,&ps); //������ͼ
		}
		break;
      case WM_DRAWITEM:
      {
         
         
         DRAWITEM_HDR *ds;
         ds = (DRAWITEM_HDR*)lParam;
         if (ds->ID == eID_SCROLLBAR)
         {
            scrollbar_owner_draw(ds);
            return TRUE;
         }
         if (ds->ID == eID_BT1)
         {
            button_owner_draw(ds);
            return TRUE;
         }
      } 
      case WM_NOTIFY:
      {
			u16 code,id;
			code =HIWORD(wParam); //���֪ͨ������.
			id   =LOWORD(wParam); //��ò�������Ϣ�Ŀؼ�ID.
         if(id==eID_BT1 && code==BN_CLICKED)
         {
            PostCloseMessage(hwnd);
         }
         if(id >= eID_RB1 && id<= eID_RB3)
			{
				if(code == BN_CLICKED)
				{ 
					cur_Resolution = id;
               switch(cur_Resolution)
               {
                  case eID_RB1:
                       
                       OV5640_Capture_Control(DISABLE);
                  	  //�������
                       cam_mode.scaling = 1;      //ʹ���Զ�����
                       cam_mode.cam_out_sx = 16;	//ʹ���Զ����ź�һ�����ó�16����
                       cam_mode.cam_out_sy = 4;	  //ʹ���Զ����ź�һ�����ó�4����
                       cam_mode.cam_out_width = 320;
                       cam_mode.cam_out_height = 240;
                     
                       //LCDλ��
                       cam_mode.lcd_sx = 270;
                       cam_mode.lcd_sy = 120;
                       OV5640_OutSize_Set(cam_mode.scaling,
                                                         cam_mode.cam_out_sx,
                                                         cam_mode.cam_out_sy,
                                                         cam_mode.cam_out_width,
                                                         cam_mode.cam_out_height);
                       state = 3;
                       OV5640_Capture_Control(ENABLE);
                       break;                 
                  case eID_RB2:
                       OV5640_Capture_Control(DISABLE);
                  	  //�������
                       cam_mode.scaling = 1;      //ʹ���Զ�����
                       cam_mode.cam_out_sx = 16;	//ʹ���Զ����ź�һ�����ó�16����
                       cam_mode.cam_out_sy = 4;	  //ʹ���Զ����ź�һ�����ó�4����
                       cam_mode.cam_out_width = 480;
                       cam_mode.cam_out_height = 272;
                     
                       //LCDλ��
                       cam_mode.lcd_sx = 160;
                       cam_mode.lcd_sy = 104;
                       OV5640_OutSize_Set(cam_mode.scaling,
                                                         cam_mode.cam_out_sx,
                                                         cam_mode.cam_out_sy,
                                                         cam_mode.cam_out_width,
                                                         cam_mode.cam_out_height);
                       state = 3;
                       OV5640_Capture_Control(ENABLE);
                     break;
                  case eID_RB3:
                       OV5640_Capture_Control(DISABLE);
                  	  //�������
                       cam_mode.scaling = 1;      //ʹ���Զ�����
                       cam_mode.cam_out_sx = 16;	//ʹ���Զ����ź�һ�����ó�16����
                       cam_mode.cam_out_sy = 4;	  //ʹ���Զ����ź�һ�����ó�4����
                       cam_mode.cam_out_width = 800;
                       cam_mode.cam_out_height = 480;
                     
                       //LCDλ��
                       cam_mode.lcd_sx = 0;
                       cam_mode.lcd_sy = 0;
                       OV5640_OutSize_Set(cam_mode.scaling,
                                                         cam_mode.cam_out_sx,
                                                         cam_mode.cam_out_sy,
                                                         cam_mode.cam_out_width,
                                                         cam_mode.cam_out_height);
                       state = 3;
                       
                       OV5640_Capture_Control(ENABLE);
                     break;
               }                 
				}
			}
         break;
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
         CTLCOLOR *cr;
         cr =(CTLCOLOR*)lParam;
         if(id >=eID_RB1 && id <= eID_RB3)
         {
				cr->TextColor =RGB888(250,250,250);
				cr->BackColor =RGB888(200,220,200);
				cr->BorderColor =RGB888(50,50,50);
				cr->ForeColor =RGB888(105,105,105);
				return TRUE;            
         }
         
			return FALSE;
			
		}     
      case WM_CLOSE:
      {
//         WCHAR wbuf[128];
//         GetWindowText(GetDlgItem(hwnd, cur_Resolution), wbuf, 128); //��ð�ť�ؼ�������
         switch(cur_Resolution)
         {
            case eID_RB1:
               SetWindowText(GetDlgItem(SetWIN, eID_TB1), L"320*240");break;                 
            case eID_RB2:
               SetWindowText(GetDlgItem(SetWIN, eID_TB1), L"480*272");break;
            case eID_RB3:
               SetWindowText(GetDlgItem(SetWIN, eID_TB1), L"800*480(Ĭ��)");break;
         }         
         
         DestroyWindow(hwnd);
         return TRUE; 
      }
      
      default: //�û������ĵ���Ϣ,��ϵͳ����.
		{
			return DefWindowProc(hwnd,msg,wParam,lParam);
		}

	}
	return WM_NULL;

}
int cur_LightMode = eID_RB4;
//���ù���ģʽ
static LRESULT	dlg_set_LightMode_WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
		case WM_CREATE: //���ڴ���ʱ,���Զ���������Ϣ,��������һЩ��ʼ���Ĳ����򴴽��Ӵ���.
		{
         RECT rc;
         GetClientRect(hwnd, &rc);
			rc.x =35;
			rc.y =55;
			rc.w =200;
			rc.h =24;
         CreateWindow(BUTTON,L"�Զ�(Ĭ��)",BS_RADIOBOX|WS_VISIBLE,
                      rc.x,rc.y,rc.w,rc.h,hwnd,(1<<16)|eID_RB4,NULL,NULL);
         OffsetRect(&rc,0,rc.h+10);
         CreateWindow(BUTTON,L"����",BS_RADIOBOX|WS_VISIBLE,
                      rc.x,rc.y,rc.w,rc.h,hwnd,(1<<16)|eID_RB5,NULL,NULL);         
         OffsetRect(&rc,0,rc.h+10);
         CreateWindow(BUTTON,L"����",BS_RADIOBOX|WS_VISIBLE,
                      rc.x,rc.y,rc.w,rc.h,hwnd,(1<<16)|eID_RB6,NULL,NULL);          
          OffsetRect(&rc,0,rc.h+10);
         CreateWindow(BUTTON,L"�칫��",BS_RADIOBOX|WS_VISIBLE,
                      rc.x,rc.y,rc.w,rc.h,hwnd,(1<<16)|eID_RB7,NULL,NULL);         
         OffsetRect(&rc,0,rc.h+10);
         CreateWindow(BUTTON,L"����",BS_RADIOBOX|WS_VISIBLE,
                      rc.x,rc.y,rc.w,rc.h,hwnd,(1<<16)|eID_RB8,NULL,NULL);     

         CreateWindow(BUTTON,L"FYZ",WS_OWNERDRAW|WS_VISIBLE,
                      5,0,100,40,hwnd,eID_BT2,NULL,NULL); 
         SetWindowFont(GetDlgItem(hwnd, eID_BT2), controlFont_48);         
		
         SetWindowEraseEx(hwnd, cbErase, TRUE);
         
         switch(cur_LightMode)
         {
            case eID_RB4:
            {
               SendMessage(GetDlgItem(hwnd, eID_RB4),BM_SETSTATE,BST_CHECKED,0);
               break;
            }
            case eID_RB5:
            {
               SendMessage(GetDlgItem(hwnd, eID_RB5),BM_SETSTATE,BST_CHECKED,0);
               break;
            }    
            case eID_RB6:
            {
               SendMessage(GetDlgItem(hwnd, eID_RB6),BM_SETSTATE,BST_CHECKED,0);
               break;
            }    
            case eID_RB7:
            {
               SendMessage(GetDlgItem(hwnd, eID_RB7),BM_SETSTATE,BST_CHECKED,0);
               break;
            }    
            case eID_RB8:
            {
               SendMessage(GetDlgItem(hwnd, eID_RB8),BM_SETSTATE,BST_CHECKED,0);
               break;
            }             
         }          
         
		}
		return TRUE;
		////
		case WM_PAINT: //������Ҫ����ʱ�����Զ���������Ϣ.
		{
			PAINTSTRUCT ps;
			HDC hdc;
			RECT rc;
			hdc =BeginPaint(hwnd,&ps); //��ʼ��ͼ

			////�û��Ļ�������...

			SetTextColor(hdc,MapRGB(hdc,250,250,250));
			SetBrushColor(hdc,MapRGB(hdc,240,10,10));



			//TextOut(hdc,10,10,L"Hello",-1);

			EndPaint(hwnd,&ps); //������ͼ
		}
		break;
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
         CTLCOLOR *cr;
         cr =(CTLCOLOR*)lParam;
         if(id >=eID_RB4 && id <= eID_RB8)
         {
				cr->TextColor =RGB888(250,250,250);
				cr->BackColor =RGB888(200,220,200);
				cr->BorderColor =RGB888(50,50,50);
				cr->ForeColor =RGB888(105,105,105);
				return TRUE;            
         }
         
			return FALSE;
			
		}  
      case WM_NOTIFY:
      {
			u16 code,id;
			code =HIWORD(wParam); //���֪ͨ������.
			id   =LOWORD(wParam); //��ò�������Ϣ�Ŀؼ�ID.
         if(id==eID_BT2 && code==BN_CLICKED)
         {
            PostCloseMessage(hwnd);
         }
         if(id >= eID_RB4 && id<= eID_RB8)
			{
				if(code == BN_CLICKED)
				{ 
					cur_LightMode = id;
               switch(cur_LightMode)
               {
                  case eID_RB4:
                     cam_mode.light_mode = 0;
                     OV5640_LightMode(cam_mode.light_mode);
                     
                     break;                 
                  case eID_RB5:
                     cam_mode.light_mode = 1;
                     OV5640_LightMode(cam_mode.light_mode);
                     break;
                  case eID_RB6:
                     cam_mode.light_mode = 2;
                     OV5640_LightMode(cam_mode.light_mode);
                     break;
                  case eID_RB7:
                     cam_mode.light_mode = 3;
                     OV5640_LightMode(cam_mode.light_mode);
                     break;
                  case eID_RB8:
                     cam_mode.light_mode = 4;
                     OV5640_LightMode(cam_mode.light_mode);
                     break;
               }   
				}
			}
         break;
      }     
      case WM_DRAWITEM:
      { 
         DRAWITEM_HDR *ds;
         ds = (DRAWITEM_HDR*)lParam;
         if (ds->ID == eID_BT2)
         {
            button_owner_draw(ds);
            return TRUE;
         }
      }  
      case WM_CLOSE:
      {
//         WCHAR wbuf[128];
//         GetWindowText(GetDlgItem(hwnd, cur_Resolution), wbuf, 128); //��ð�ť�ؼ�������
         switch(cur_LightMode)
         {
            case eID_RB4:
     
               SetWindowText(GetDlgItem(SetWIN, eID_TB2), L"�Զ�(Ĭ��)");
               break;                 
            case eID_RB5:
    
               SetWindowText(GetDlgItem(SetWIN, eID_TB2), L"����");break;
            case eID_RB6:

               SetWindowText(GetDlgItem(SetWIN, eID_TB2), L"����");break;
            case eID_RB7:

               SetWindowText(GetDlgItem(SetWIN, eID_TB2), L"�칫��");break;
            case eID_RB8:

               SetWindowText(GetDlgItem(SetWIN, eID_TB2), L"����");break;
         }         
         
         DestroyWindow(hwnd);
         return TRUE; 
      }      
		default: //�û������ĵ���Ϣ,��ϵͳ����.
		{
			return DefWindowProc(hwnd,msg,wParam,lParam);
		}

	}
	return WM_NULL;

}


int cur_SpecialEffects = eID_RB16;
//��������Ч��
static LRESULT	dlg_set_SpecialEffects_WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
		case WM_CREATE: //���ڴ���ʱ,���Զ���������Ϣ,��������һЩ��ʼ���Ĳ����򴴽��Ӵ���.
		{
         RECT rc;
         GetClientRect(hwnd, &rc);
			rc.x =35;
			rc.y =55;
			rc.w =200;
			rc.h =24;
         CreateWindow(BUTTON,L"��ɫ",BS_RADIOBOX|WS_VISIBLE,
                      rc.x,rc.y,rc.w,rc.h,hwnd,(1<<16)|eID_RB9,NULL,NULL);
         OffsetRect(&rc,0,rc.h+10);
         CreateWindow(BUTTON,L"ůɫ",BS_RADIOBOX|WS_VISIBLE,
                      rc.x,rc.y,rc.w,rc.h,hwnd,(1<<16)|eID_RB10,NULL,NULL);         
         OffsetRect(&rc,0,rc.h+10);
         CreateWindow(BUTTON,L"�ڰ�",BS_RADIOBOX|WS_VISIBLE,
                      rc.x,rc.y,rc.w,rc.h,hwnd,(1<<16)|eID_RB11,NULL,NULL);          
          OffsetRect(&rc,0,rc.h+10);
         CreateWindow(BUTTON,L"����",BS_RADIOBOX|WS_VISIBLE,
                      rc.x,rc.y,rc.w,rc.h,hwnd,(1<<16)|eID_RB12,NULL,NULL);         
         OffsetRect(&rc,0,rc.h+10);
         CreateWindow(BUTTON,L"��ɫ",BS_RADIOBOX|WS_VISIBLE,
                      rc.x,rc.y,rc.w,rc.h,hwnd,(1<<16)|eID_RB13,NULL,NULL); 
         OffsetRect(&rc,0,rc.h+10);         
         CreateWindow(BUTTON,L"ƫ��",BS_RADIOBOX|WS_VISIBLE,
                      rc.x,rc.y,rc.w,rc.h,hwnd,(1<<16)|eID_RB14,NULL,NULL);         
         OffsetRect(&rc,0,rc.h+10);
         CreateWindow(BUTTON,L"����",BS_RADIOBOX|WS_VISIBLE,
                      rc.x,rc.y,rc.w,rc.h,hwnd,(1<<16)|eID_RB15,NULL,NULL);   
          OffsetRect(&rc,0,rc.h+10);
          CreateWindow(BUTTON,L"����(Ĭ��)",BS_RADIOBOX|WS_VISIBLE,
                      rc.x,rc.y,rc.w,rc.h,hwnd,(1<<16)|eID_RB16,NULL,NULL); 
         CreateWindow(BUTTON,L"FYZ",WS_OWNERDRAW|WS_VISIBLE,
                      5,0,100,40,hwnd,eID_BT3,NULL,NULL); 
         SetWindowFont(GetDlgItem(hwnd, eID_BT3), controlFont_48);         
		
         SetWindowEraseEx(hwnd, cbErase, TRUE);
         
         switch(cur_SpecialEffects)
         {
            case eID_RB9:
            {
               SendMessage(GetDlgItem(hwnd, eID_RB9),BM_SETSTATE,BST_CHECKED,0);
               break;
            }
            case eID_RB10:
            {
               SendMessage(GetDlgItem(hwnd, eID_RB10),BM_SETSTATE,BST_CHECKED,0);
               break;
            }    
            case eID_RB11:
            {
               SendMessage(GetDlgItem(hwnd, eID_RB11),BM_SETSTATE,BST_CHECKED,0);
               break;
            }    
            case eID_RB12:
            {
               SendMessage(GetDlgItem(hwnd, eID_RB12),BM_SETSTATE,BST_CHECKED,0);
               break;
            }    
            case eID_RB13:
            {
               SendMessage(GetDlgItem(hwnd, eID_RB13),BM_SETSTATE,BST_CHECKED,0);
               break;
            }     
            case eID_RB14:
            {
               SendMessage(GetDlgItem(hwnd, eID_RB14),BM_SETSTATE,BST_CHECKED,0);
               break;
            }    
            case eID_RB15:
            {
               SendMessage(GetDlgItem(hwnd, eID_RB15),BM_SETSTATE,BST_CHECKED,0);
               break;
            }    
            case eID_RB16:
            {
               SendMessage(GetDlgItem(hwnd, eID_RB16),BM_SETSTATE,BST_CHECKED,0);
               break;
            }              
         }          
         
		}
		return TRUE;
		////
		case WM_PAINT: //������Ҫ����ʱ�����Զ���������Ϣ.
		{
			PAINTSTRUCT ps;
			HDC hdc;
			RECT rc;
			hdc =BeginPaint(hwnd,&ps); //��ʼ��ͼ

			////�û��Ļ�������...

			SetTextColor(hdc,MapRGB(hdc,250,250,250));
			SetBrushColor(hdc,MapRGB(hdc,240,10,10));



			//TextOut(hdc,10,10,L"Hello",-1);

			EndPaint(hwnd,&ps); //������ͼ
		}
		break;
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
         CTLCOLOR *cr;
         cr =(CTLCOLOR*)lParam;
         if(id >=eID_RB9 && id <= eID_RB16)
         {
				cr->TextColor =RGB888(250,250,250);
				cr->BackColor =RGB888(200,220,200);
				cr->BorderColor =RGB888(50,50,50);
				cr->ForeColor =RGB888(105,105,105);
				return TRUE;            
         }
         
			return FALSE;
			
		}  
      case WM_NOTIFY:
      {
			u16 code,id;
			code =HIWORD(wParam); //���֪ͨ������.
			id   =LOWORD(wParam); //��ò�������Ϣ�Ŀؼ�ID.
         if(id==eID_BT3 && code==BN_CLICKED)
         {
            PostCloseMessage(hwnd);
         }
         if(id >= eID_RB9 && id<= eID_RB16)
			{
				if(code == BN_CLICKED)
				{ 
					cur_SpecialEffects = id;
               switch(cur_SpecialEffects)
               {
                  case eID_RB9:
                     cam_mode.effect = 1;
                     OV5640_SpecialEffects(cam_mode.effect);
                     break;                 
                  case eID_RB10:
                     cam_mode.effect = 2;
                     OV5640_SpecialEffects(cam_mode.effect);
                     break;
                  case eID_RB11:
                     cam_mode.effect = 3;
                     OV5640_SpecialEffects(cam_mode.effect);               
                     break;
                  case eID_RB12:
                     cam_mode.effect = 4;
                     OV5640_SpecialEffects(cam_mode.effect);                
                     break;
                  case eID_RB13:
                     cam_mode.effect = 5;
                     OV5640_SpecialEffects(cam_mode.effect);                
                     break;
                  case eID_RB14:
                     cam_mode.effect = 6;
                     OV5640_SpecialEffects(cam_mode.effect);                
                     break;
                  case eID_RB15:
                     cam_mode.effect = 7;
                     OV5640_SpecialEffects(cam_mode.effect);                
                     break;
                  case eID_RB16:
                     cam_mode.effect = 0;
                     OV5640_SpecialEffects(cam_mode.effect);
                     break;            
               } 
				}
			}
         break;
      }     
      case WM_DRAWITEM:
      { 
         DRAWITEM_HDR *ds;
         ds = (DRAWITEM_HDR*)lParam;
         if (ds->ID == eID_BT3)
         {
            button_owner_draw(ds);
            return TRUE;
         }
      }  
      case WM_CLOSE:
      {
//         WCHAR wbuf[128];
//         GetWindowText(GetDlgItem(hwnd, cur_Resolution), wbuf, 128); //��ð�ť�ؼ�������
         switch(cur_SpecialEffects)
         {
            case eID_RB9:

               SetWindowText(GetDlgItem(SetWIN, eID_TB3), L"��ɫ");break;                 
            case eID_RB10:
       
               SetWindowText(GetDlgItem(SetWIN, eID_TB3), L"ůɫ");break;
            case eID_RB11:
                      
               SetWindowText(GetDlgItem(SetWIN, eID_TB3), L"�ڰ�");break;
            case eID_RB12:
                   
               SetWindowText(GetDlgItem(SetWIN, eID_TB3), L"����");break;
            case eID_RB13:
                  
               SetWindowText(GetDlgItem(SetWIN, eID_TB3), L"��ɫ");break;
            case eID_RB14:
                       
               SetWindowText(GetDlgItem(SetWIN, eID_TB3), L"ƫ��");break;
            case eID_RB15:
              
               SetWindowText(GetDlgItem(SetWIN, eID_TB3), L"����");break;
            case eID_RB16:
               
               SetWindowText(GetDlgItem(SetWIN, eID_TB3), L"����(Ĭ��)");break;            
         }         
         
         DestroyWindow(hwnd);
         return TRUE; 
      }      
		default: //�û������ĵ���Ϣ,��ϵͳ����.
		{
			return DefWindowProc(hwnd,msg,wParam,lParam);
		}

	}
	return WM_NULL;

}
//�������ô��ڻص�����
static LRESULT	dlg_set_WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc,rc0;
   static SCROLLINFO sif, sif1, sif2;
   static HWND wnd;
	switch(msg)
	{
		case WM_CREATE: //���ڴ���ʱ,���Զ���������Ϣ,��������һЩ��ʼ���Ĳ����򴴽��Ӵ���.
		{
			b_close =FALSE;

//			GetClientRect(hwnd,&rc0); //��ô��ڵĿͻ�������.
//			CreateWindow(BUTTON,L"OK",WS_VISIBLE,rc0.w-80,8,68,32,hwnd,eID_OK,NULL,NULL);
			rc.x =40;
			rc.y =50;
			rc.w =100;
			rc.h =40;
         CreateWindow(TEXTBOX,L"�Զ��Խ�",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_SET1,NULL,NULL);
         SendMessage(GetDlgItem(hwnd, eID_SET1),TBM_SET_TEXTFLAG,0,
                     DT_SINGLELINE|DT_LEFT|DT_VCENTER|DT_BKGND|DT_BORDER); 
         
			OffsetRect(&rc,0,rc.h+10);
         CreateWindow(TEXTBOX,L"����",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_SET2,NULL,NULL);
         SendMessage(GetDlgItem(hwnd, eID_SET2),TBM_SET_TEXTFLAG,0,
                     DT_SINGLELINE|DT_LEFT|DT_VCENTER|DT_BKGND|DT_BORDER);         
         
			OffsetRect(&rc,0,rc.h+10);
         CreateWindow(TEXTBOX,L"���Ͷ�",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_SET3,NULL,NULL);
         SendMessage(GetDlgItem(hwnd, eID_SET3),TBM_SET_TEXTFLAG,0,
                     DT_SINGLELINE|DT_LEFT|DT_VCENTER|DT_BKGND|DT_BORDER); 

			OffsetRect(&rc,0,rc.h+10);
         CreateWindow(TEXTBOX,L"�Աȶ�",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_SET4,NULL,NULL);
         SendMessage(GetDlgItem(hwnd, eID_SET4),TBM_SET_TEXTFLAG,0,
                     DT_SINGLELINE|DT_LEFT|DT_VCENTER|DT_BKGND|DT_BORDER); 
			OffsetRect(&rc,0,rc.h+10);
         CreateWindow(TEXTBOX,L"�ֱ���",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_SET5,NULL,NULL);
         SendMessage(GetDlgItem(hwnd, eID_SET5),TBM_SET_TEXTFLAG,0,
                     DT_SINGLELINE|DT_LEFT|DT_VCENTER|DT_BKGND|DT_BORDER);         
         
			OffsetRect(&rc,0,rc.h+10);
         CreateWindow(TEXTBOX,L"����ģʽ",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_SET6,NULL,NULL);
         SendMessage(GetDlgItem(hwnd, eID_SET6),TBM_SET_TEXTFLAG,0,
                     DT_SINGLELINE|DT_LEFT|DT_VCENTER|DT_BKGND|DT_BORDER); 

			OffsetRect(&rc,0,rc.h+10);
         CreateWindow(TEXTBOX,L"����Ч��",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_SET7,NULL,NULL);
         SendMessage(GetDlgItem(hwnd, eID_SET7),TBM_SET_TEXTFLAG,0,
                     DT_SINGLELINE|DT_LEFT|DT_VCENTER|DT_BKGND|DT_BORDER); 

			SetTimer(hwnd,2,20,TMR_START,NULL);
         
         sif.cbSize = sizeof(sif);
         sif.fMask = SIF_ALL;
         sif.nMin = -2;
         sif.nMax = 2;
         sif.nValue = cam_mode.brightness;
         sif.TrackSize = 31;//����ֵ
         sif.ArrowSize = 0;//���˿��Ϊ0��ˮƽ��������
         CreateWindow(SCROLLBAR, L"SCROLLBAR_R", WS_OWNERDRAW|WS_VISIBLE, 
                       180, 105, 180, 31, hwnd, eID_SCROLLBAR, NULL, NULL);
         SendMessage(GetDlgItem(hwnd, eID_SCROLLBAR), SBM_SETSCROLLINFO, TRUE, (LPARAM)&sif);


         sif1.cbSize = sizeof(sif1);
         sif1.fMask = SIF_ALL;
         sif1.nMin = -3;
         sif1.nMax = 3;
         sif1.nValue = cam_mode.saturation;
         sif1.TrackSize = 31;//����ֵ
         sif1.ArrowSize = 0;//���˿��Ϊ0��ˮƽ��������
         CreateWindow(SCROLLBAR, L"SCROLLBAR_R", WS_OWNERDRAW|WS_VISIBLE, 
                       180, 155, 180, 31, hwnd, eID_SCROLLBAR1, NULL, NULL);
         SendMessage(GetDlgItem(hwnd, eID_SCROLLBAR1), SBM_SETSCROLLINFO, TRUE, (LPARAM)&sif1);
         
         
         sif2.cbSize = sizeof(sif2);
         sif2.fMask = SIF_ALL;
         sif2.nMin = -3;
         sif2.nMax = 3;
         sif2.nValue = cam_mode.contrast;
         sif2.TrackSize = 31;//����ֵ
         sif2.ArrowSize = 0;//���˿��Ϊ0��ˮƽ��������
         CreateWindow(SCROLLBAR, L"SCROLLBAR_R", WS_OWNERDRAW|WS_VISIBLE, 
                       180, 205, 180, 31, hwnd, eID_SCROLLBAR2, NULL, NULL);
         SendMessage(GetDlgItem(hwnd, eID_SCROLLBAR2), SBM_SETSCROLLINFO, TRUE, (LPARAM)&sif2);
         
         CreateWindow(BUTTON,L" ",WS_OWNERDRAW|WS_VISIBLE,
                      280,55,60,30,hwnd,eID_switch,NULL,NULL);
                      
         CreateWindow(TEXTBOX,L"800*480(Ĭ��)",WS_VISIBLE,
                      200,255,160,30,hwnd,eID_TB1,NULL,NULL);                      
         SendMessage(GetDlgItem(hwnd, eID_TB1),TBM_SET_TEXTFLAG,0,
                     DT_SINGLELINE|DT_RIGHT|DT_VCENTER|DT_BKGND|DT_BORDER);
         switch(cur_Resolution)
         {
            case eID_RB1:
               SetWindowText(GetDlgItem(hwnd, eID_TB1), L"320*240");break;                 
            case eID_RB2:
               SetWindowText(GetDlgItem(hwnd, eID_TB1), L"480*272");break;
            case eID_RB3:
               SetWindowText(GetDlgItem(hwnd, eID_TB1), L"800*480(Ĭ��)");break;
         }                       
         CreateWindow(BUTTON,L"C",WS_OWNERDRAW|WS_VISIBLE,
                      358,253,40,30,hwnd,eID_Setting1,NULL,NULL); 
         SetWindowFont(GetDlgItem(hwnd, eID_Setting1), controlFont_48);
         
         CreateWindow(TEXTBOX,L"�Զ�(Ĭ��)",WS_VISIBLE,
                      200,305,160,30,hwnd,eID_TB2,NULL,NULL);                      
         SendMessage(GetDlgItem(hwnd, eID_TB2),TBM_SET_TEXTFLAG,0,
                     DT_SINGLELINE|DT_RIGHT|DT_VCENTER|DT_BKGND|DT_BORDER);        
         switch(cur_LightMode)
         {
            case eID_RB4:
               SetWindowText(GetDlgItem(hwnd, eID_TB2), L"�Զ�(Ĭ��)");break;                 
            case eID_RB5:
               SetWindowText(GetDlgItem(hwnd, eID_TB2), L"����");break;
            case eID_RB6:
               SetWindowText(GetDlgItem(hwnd, eID_TB2), L"����");break;
            case eID_RB7:
               SetWindowText(GetDlgItem(hwnd, eID_TB2), L"�칫��");break;
            case eID_RB8:
               SetWindowText(GetDlgItem(hwnd, eID_TB2), L"����");break;
         }  
         CreateWindow(BUTTON,L"C",WS_OWNERDRAW|WS_VISIBLE,
                      358,303,40,30,hwnd,eID_Setting2,NULL,NULL); 
         SetWindowFont(GetDlgItem(hwnd, eID_Setting2), controlFont_48);         
         
         CreateWindow(TEXTBOX,L"����(Ĭ��)",WS_VISIBLE,
                      200,355,160,30,hwnd,eID_TB3,NULL,NULL);                      
         SendMessage(GetDlgItem(hwnd, eID_TB3),TBM_SET_TEXTFLAG,0,
                     DT_SINGLELINE|DT_RIGHT|DT_VCENTER|DT_BKGND|DT_BORDER);
         switch(cur_SpecialEffects)
         {
            case eID_RB9:
               SetWindowText(GetDlgItem(hwnd, eID_TB3), L"��ɫ");break;                 
            case eID_RB10:
               SetWindowText(GetDlgItem(hwnd, eID_TB3), L"ůɫ");break;
            case eID_RB11:
               SetWindowText(GetDlgItem(hwnd, eID_TB3), L"�ڰ�");break;
            case eID_RB12:
               SetWindowText(GetDlgItem(hwnd, eID_TB3), L"����");break;
            case eID_RB13:
               SetWindowText(GetDlgItem(hwnd, eID_TB3), L"��ɫ");break;
            case eID_RB14:
               SetWindowText(GetDlgItem(hwnd, eID_TB3), L"ƫ��");break;
            case eID_RB15:
               SetWindowText(GetDlgItem(hwnd, eID_TB3), L"����");break;
            case eID_RB16:
               SetWindowText(GetDlgItem(hwnd, eID_TB3), L"����(Ĭ��)");break;            
         }           
         CreateWindow(BUTTON,L"C",WS_OWNERDRAW|WS_VISIBLE,
                      358,353,40,30,hwnd,eID_Setting3,NULL,NULL); 
         SetWindowFont(GetDlgItem(hwnd, eID_Setting3), controlFont_48); 
 

      }
		return TRUE;

		case WM_TIMER:
		{
			switch(wParam)
			{

				case 1:
				{
					//tmr_500ms++;
					//InvalidateRect(hwnd,NULL,FALSE);

				}
				break;
				////

				case 2:
				{
					if(GetKeyState(VK_LBUTTON)!=0)
					{
						break;
					}

					////
					GetWindowRect(hwnd,&rc);

					if(b_close==FALSE)
					{
						if(rc.y < win_rc.y )
						{
							if((win_rc.y-rc.y)>50)
							{
								rc.y +=30;
							}
							if((win_rc.y-rc.y)>30)
							{
								rc.y +=20;
							}
							else
							{
								rc.y +=4;
							}
							ScreenToClient(GetParent(hwnd),(POINT*)&rc.x,1);
                     
							MoveWindow(hwnd,rc.x,rc.y,rc.w,rc.h,TRUE);
						}
					}
					else
					{
						if(rc.y > -(rc.h))
						{
							rc.y -= 40;

							ScreenToClient(GetParent(hwnd),(POINT*)&rc.x,1);
                     
							MoveWindow(hwnd,rc.x,rc.y,rc.w,rc.h,TRUE);
						}
						else
						{
							PostCloseMessage(hwnd);
						}
					}


				}
				break;
			}
		}
		break;
		////

		case WM_NOTIFY: //WM_NOTIFY��Ϣ:wParam��16λΪ���͸���Ϣ�Ŀؼ�ID,��16λΪ֪ͨ��;lParamָ����һ��NMHDR�ṹ��.
		{
			u16 code,id;
         int i;
         NMHDR *nr;  
         u16 ctr_id = LOWORD(wParam); //wParam��16λ�Ƿ��͸���Ϣ�Ŀؼ�ID. 
         nr = (NMHDR*)lParam; //lParam����������NMHDR�ṹ�忪ͷ.
			code =HIWORD(wParam); //���֪ͨ������.
			id   =LOWORD(wParam); //��ò�������Ϣ�Ŀؼ�ID.
//         if (ctr_id == eID_SCROLLBAR || ctr_id == eID_SCROLLBAR1 || ctr_id == eID_SCROLLBAR2)
//         {
         NM_SCROLLBAR *sb_nr;
         sb_nr = (NM_SCROLLBAR*)nr; //Scrollbar��֪ͨ��Ϣʵ��Ϊ NM_SCROLLBAR��չ�ṹ,���渽���˸������Ϣ.
         switch (nr->code)
         {
            case SBN_THUMBTRACK: //R�����ƶ�
            {
               switch(ctr_id)
               {
//                  i= sb_nr->nTrackValue; //�õ���ǰ������ֵ
//                  SendMessage(nr->hwndFrom, SBM_SETVALUE, TRUE, i); //����SBM_SETVALUE����������ֵ
                  case eID_SCROLLBAR://����
                  {
                     cam_mode.brightness = sb_nr->nTrackValue; //�õ���ǰ������ֵ
                     OV5640_BrightnessConfig(cam_mode.brightness);
                     SendMessage(nr->hwndFrom, SBM_SETVALUE, TRUE, cam_mode.brightness); 
                     break;
                  }
                  case eID_SCROLLBAR1://���Ͷ�
                  {
                     cam_mode.saturation = sb_nr->nTrackValue; //�õ���ǰ������ֵ
                     OV5640_Color_Saturation(cam_mode.saturation);
                     SendMessage(nr->hwndFrom, SBM_SETVALUE, TRUE, cam_mode.saturation); 
                     
                     
                     
                     break;
                  }
                  case eID_SCROLLBAR2://�Աȶ�
                  {
                     cam_mode.contrast = sb_nr->nTrackValue; //�õ���ǰ������ֵ
                     OV5640_ContrastConfig(cam_mode.contrast);
                     SendMessage(nr->hwndFrom, SBM_SETVALUE, TRUE, cam_mode.contrast);                      
                     break;
                  }
                  
               }
            }
            break;
         }
			if((id==eID_Setting1|| id == eID_TB1)&& code==BN_CLICKED)
			{
 
               WNDCLASS wcex;

//               GetWindowRect(GetDlgItem(hwnd,eID_SET1),&rc);
//               ScreenToClient(hwnd,(POINT*)&rc.x,1);
               GetClientRect(hwnd,&rc);
               rc.x = rc.x+(rc.w-win_rc.w)/2;
               //rc.y = 50;
               rc.w =400;
               rc.h =400;
            
               wcex.Tag	 		= WNDCLASS_TAG;
               wcex.Style			= CS_HREDRAW | CS_VREDRAW;
               wcex.lpfnWndProc	= (WNDPROC)dlg_set_Resolution_WinProc;
               wcex.cbClsExtra		= 0;
               wcex.cbWndExtra		= 0;
               wcex.hInstance		= NULL;
               wcex.hIcon			= NULL;
               wcex.hCursor		= NULL;


               wnd = CreateWindowEx(
                           WS_EX_FRAMEBUFFER,
                           &wcex,L"Set_1_xxx",
                           WS_OVERLAPPED|WS_CLIPCHILDREN|WS_VISIBLE,
                           rc.x,rc.y,rc.w,rc.h,
                           hwnd,0,NULL,NULL);

			}

			if((id==eID_Setting2|| id == eID_TB2) && code==BN_CLICKED)
			{
				WNDCLASS wcex;
            GetClientRect(hwnd,&rc);
//				GetWindowRect(GetDlgItem(hwnd,eID_SET2),&rc);
//				ScreenToClient(hwnd,(POINT*)&rc.x,1);
	         rc.x = rc.x+(rc.w-win_rc.w)/2;
               //rc.y = 50;
            rc.w =400;
            rc.h =400;
				wcex.Tag	 		= WNDCLASS_TAG;
				wcex.Style			= CS_HREDRAW | CS_VREDRAW;
				wcex.lpfnWndProc	= (WNDPROC)dlg_set_LightMode_WinProc;
				wcex.cbClsExtra		= 0;
				wcex.cbWndExtra		= 0;
				wcex.hInstance		= NULL;
				wcex.hIcon			= NULL;
				wcex.hCursor		= NULL;


				CreateWindowEx(
								WS_EX_FRAMEBUFFER,
								&wcex,L"Set_2_xxx",
								WS_OVERLAPPED|WS_CLIPCHILDREN|WS_VISIBLE,
								rc.x,rc.y,rc.w,rc.h,
								hwnd,0,NULL,NULL);

			}
			if((id==eID_Setting3|| id == eID_TB3) && code==BN_CLICKED)
			{
				WNDCLASS wcex;
            GetClientRect(hwnd,&rc);
//				GetWindowRect(GetDlgItem(hwnd,eID_SET2),&rc);
//				ScreenToClient(hwnd,(POINT*)&rc.x,1);
	         rc.x = rc.x+(rc.w-win_rc.w)/2;
               //rc.y = 50;
            rc.w =400;
            rc.h =400;
				wcex.Tag	 		= WNDCLASS_TAG;
				wcex.Style			= CS_HREDRAW | CS_VREDRAW;
				wcex.lpfnWndProc	= (WNDPROC)dlg_set_SpecialEffects_WinProc;
				wcex.cbClsExtra		= 0;
				wcex.cbWndExtra		= 0;
				wcex.hInstance		= NULL;
				wcex.hIcon			= NULL;
				wcex.hCursor		= NULL;


				CreateWindowEx(
								WS_EX_FRAMEBUFFER,
								&wcex,L"Set_3_xxx",
								WS_OVERLAPPED|WS_CLIPCHILDREN|WS_VISIBLE,
								rc.x,rc.y,rc.w,rc.h,
								hwnd,0,NULL,NULL);

			}
			if(id==eID_OK && code==BN_CLICKED) // ��ť����������.
			{
				PostCloseMessage(hwnd); //ʹ����WM_CLOSE��Ϣ�رմ���.
			}
         
			if (id == eID_switch && code == BN_CLICKED)
			{
				focus_status = ~focus_status;
//            EnableWindow(GetDlgItem(hwnd, eID_switch), FALSE);
            GUI_SemPost(set_sem);
//            EnableWindow(GetDlgItem(hwnd, eID_switch), TRUE);
			}               
		}
		break;
		////
      
      case WM_ERASEBKGND:
      {
         HDC hdc =(HDC)wParam;

         return TRUE;
      }

		case WM_PAINT: //������Ҫ����ʱ�����Զ���������Ϣ.
		{
			PAINTSTRUCT ps;
			HDC hdc;
         HDC hdc_mem;
			RECT rc;
			hdc =BeginPaint(hwnd,&ps); //��ʼ��ͼ
         
			////�û��Ļ�������...
         GetClientRect(hwnd, &rc);
			hdc_mem = CreateMemoryDC(SURF_ARGB4444, rc.w, rc.h);
	
         SetBrushColor(hdc,MapRGB(hdc,0,0,0));
         FillRect(hdc, &rc);
//			//TextOut(hdc,10,10,L"Hello",-1);
//         SetBrushColor(hdc_mem,MapARGB(hdc_mem,0,0,0,0));
//         FillRect(hdc_mem, &rc);
         
//         
//         BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_mem, 0, 0, SRCCOPY);
//         InvalidateRect(hwnd,&rc,TRUE); 
         DeleteDC(hdc_mem);
         HFONT hfont_old = NULL;
         rc.x =5;
         rc.y =5;
         rc.w =40;
         rc.h =40;
         SetTextColor(hdc,MapRGB(hdc,240,250,250));
         hfont_old = SetFont(hdc, controlFont_48);
         DrawText(hdc,L"a",-1,&rc,DT_CENTER|DT_VCENTER|DT_BKGND);
         SetFont(hdc, hfont_old);
         rc.x = 45;
         rc.y = 10;
         DrawText(hdc,L"����",-1,&rc,DT_CENTER|DT_VCENTER|DT_BKGND);         
         
         EndPaint(hwnd,&ps); //������ͼ
         break;
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
         CTLCOLOR *cr;
         cr =(CTLCOLOR*)lParam;
//			if(id== eID_SET1 || id == eID_SET2)
//			{		
//				cr->TextColor =RGB888(255,255,255);//������ɫ��RGB888��ɫ��ʽ)
//				cr->BackColor =RGB888(0,0,0);//������ɫ��RGB888��ɫ��ʽ)
//				cr->BorderColor =RGB888(255,0,0);//�߿���ɫ��RGB888��ɫ��ʽ)
//				return TRUE;
//			}
         if((id >=eID_SET1 && id <= eID_SET7 )|| (id >= eID_TB1 && id <= eID_TB3) )
         {
				cr->TextColor =RGB888(255,255,255);//������ɫ��RGB888��ɫ��ʽ)
				cr->BackColor =RGB888(0,0,0);//������ɫ��RGB888��ɫ��ʽ)
				cr->BorderColor =RGB888(0,0,0);//�߿���ɫ��RGB888��ɫ��ʽ)
				return TRUE;            
         }
         
			return FALSE;
			
		}	

		case	WM_DRAWITEM:
		{
			/*�����ؼ�ָ����WS_OWNERDRAW�����ÿ���ڻ���ǰ����������ڷ���WM_DRAWITEM��Ϣ��
			 *  wParam����ָ���˷��͸���Ϣ�Ŀؼ�ID;lParam����ָ��һ��DRAWITEM_HDR�Ľṹ��ָ�룬
			 *  ��ָ���Ա������һЩ�ؼ�������صĲ���.
			 */
			DRAWITEM_HDR *ds;
			ds = (DRAWITEM_HDR*)lParam;
			if (ds->ID == eID_switch)
			{
				Checkbox_owner_draw(ds); //ִ���Ի��ư�ť
			}
         if (ds->ID == eID_SCROLLBAR || ds->ID == eID_SCROLLBAR1 || ds->ID == eID_SCROLLBAR2)
         {
            scrollbar_owner_draw(ds);
            return TRUE;
         }         
			if (ds->ID >= eID_Setting1&& ds->ID <= eID_Setting3)
			{
				Button_owner_draw(ds); //ִ���Ի��ư�ť
			}         
         
			return TRUE;
		}
		////
		case WM_CLOSE:
		{
			b_close=TRUE;

			GetWindowRect(hwnd,&rc);

			if(rc.y <= -(rc.h))
			{
				DestroyWindow(hwnd);
				return TRUE;
			}
			return FALSE;
		}
		break;
		////
 
		default: //�û������ĵ���Ϣ,��ϵͳ����.
		{
			return DefWindowProc(hwnd,msg,wParam,lParam);
		}

	}

	return WM_NULL;

}
//����ͷ���ڻص�����
static LRESULT WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   switch(msg)
   {
      case WM_CREATE:
      {

        /* ��ʼ������ͷGPIO��IIC */
        OV5640_HW_Init();  
        /* ��ȡ����ͷоƬID��ȷ������ͷ�������� */
        OV5640_ReadID(&OV5640_Camera_ID);

        if(OV5640_Camera_ID.PIDH  == 0x56)
        {
          GUI_DEBUG("OV5640 ID:%x %x",OV5640_Camera_ID.PIDH ,OV5640_Camera_ID.PIDL);
        }
        else
        {
          MSGBOX_OPTIONS ops;
          //const WCHAR *btn[]={L"ȷ��"};
          int x,y,w,h;

          ops.Flag =MB_ICONERROR;
          //ops.pButtonText =btn;
          ops.ButtonCount =0;
          w =500;
          h =200;
          x =(GUI_XSIZE-w)>>1;
          y =(GUI_YSIZE-h)>>1;
          MessageBox(hwnd,x,y,w,h,L"û�м�⵽OV5640����ͷ��\n�����¼�����ӡ�",L"��Ϣ",&ops); 
          break;  
        }     
        cam_sem = GUI_SemCreate(1,1);
        set_sem = GUI_SemCreate(1,1);
        h1=rt_thread_create("Update_Dialog",(void(*)(void*))Update_Dialog,NULL,4096,5,5);
        //h2=rt_thread_create("SetPara",(void(*)(void*))Set_Para,NULL,1024,5,5);
        Set_AutoFocus();
        rt_thread_startup(h1);	
        bits = (U16 *)GUI_VMEM_Alloc(2*800*480); 
        
		  SetTimer(hwnd,1,1000,TMR_START,NULL);  
        RECT rc;
        GetClientRect(hwnd, &rc);
        CreateWindow(BUTTON,L"a",WS_OWNERDRAW,rc.w-90,rc.h-40,90,40,hwnd,eID_SET,NULL,NULL);        
        
        
        break;
      }
		case	WM_DRAWITEM:
		{
			/*�����ؼ�ָ����WS_OWNERDRAW�����ÿ���ڻ���ǰ����������ڷ���WM_DRAWITEM��Ϣ��
			 *  wParam����ָ���˷��͸���Ϣ�Ŀؼ�ID;lParam����ָ��һ��DRAWITEM_HDR�Ľṹ��ָ�룬
			 *  ��ָ���Ա������һЩ�ؼ�������صĲ���.
			 */
			DRAWITEM_HDR *ds;
			ds = (DRAWITEM_HDR*)lParam;
			if (ds->ID == eID_SET)
			{
				BtCam_owner_draw(ds); //ִ���Ի��ư�ť
			}

			return TRUE;
		}
 		case WM_TIMER:
      {
         switch(state)
         {
            case 0:
            {
              OV5640_Init();  
              OV5640_RGB565Config();
              OV5640_USER_Config();
              OV5640_FOCUS_AD5820_Init();
               
               if(cam_mode.auto_focus ==1)
               {
                  OV5640_FOCUS_AD5820_Constant_Focus();
                  focus_status = 1;
               }
               //ʹ��DCMI�ɼ�����
              DCMI_Cmd(ENABLE); 
              DCMI_CaptureCmd(ENABLE); 
                             
               state = 1;
               break;
            }
            case 1:
            {
               ShowWindow(GetDlgItem(hwnd, eID_SET), SW_SHOW);
               state=2;
               break;
            }
            case 2:
            {
               update_flag = 1;
               break;
            }
         }
         break;
      }
//      case WM_ERASEBKGND:
//      {
//         HDC hdc =(HDC)wParam;
//         ClrDisplay(hdc, NULL, MapRGB(hdc, 0, 0, 0));
//      }
      case WM_PAINT:
      {
         PAINTSTRUCT ps;
         SURFACE *pSurf;
         HDC hdc_mem;
         HDC hdc;
         RECT rc;
         static int switch_res = 0;
         static int old_fps = 0;
         WCHAR wbuf[128];
         hdc = BeginPaint(hwnd,&ps);
         GetClientRect(hwnd,&rc);
			if(state==0)
			{
				SetTextColor(hdc,MapRGB(hdc,250,0,0));
				SetBrushColor(hdc,MapRGB(hdc,50,0,0));
				SetPenColor(hdc,MapRGB(hdc,250,0,0));

				DrawText(hdc,L"���ڳ�ʼ������ͷ\r\n��ȴ�...",-1,&rc,DT_VCENTER|DT_CENTER|DT_BKGND);

			}              
         if(state == 2)
         {     
            
            pSurf =CreateSurface(SURF_RGB565,GUI_XSIZE, GUI_YSIZE, 0, bits);
            if(switch_res == 1)
            {
               switch_res = 0;
               memset(bits,0,GUI_XSIZE*GUI_YSIZE*2);
            }
            hdc_mem =CreateDC(pSurf,NULL);
//            ClrDisplay(hdc_mem, NULL, MapRGB(hdc_mem,0,0,0));
            if(update_flag)
            {
               update_flag = 0;
               old_fps = fps;
               fps = 0;
            } 
            SetTextColor(hdc_mem, MapRGB(hdc_mem, 255,255,255));                 
            x_wsprintf(wbuf,L"֡��FPS:%d/s",old_fps);
 
            switch(cur_Resolution)
            {
               case eID_RB1:
                  rc_fps.x = 270;rc_fps.y = 200;rc_fps.w = 320;rc_fps.h = 240;
               break;                 
               case eID_RB2:
                  
                  rc_fps.x = 160;rc_fps.y = 200;rc_fps.w = 480;rc_fps.h = 272;
                  break;
               case eID_RB3:
                  rc_fps.x = 0;rc_fps.y = 400;rc_fps.w = 800;rc_fps.h = 72;
               break;
               
            }               
            
            DrawText(hdc_mem, wbuf, -1, &rc_fps, DT_SINGLELINE| DT_VCENTER|DT_CENTER);
                   
            BitBlt(hdc, 0, 0, 800, 480, 
                   hdc_mem, 0, 0, SRCCOPY);          
            DeleteSurface(pSurf);
            DeleteDC(hdc_mem);
         }
         if(state == 3)
         {
            switch_res = 1;
            state = 2;
         }
           
         EndPaint(hwnd,&ps);
         break;
      }
      case WM_DESTROY:
      {
         state = 0;
         OV5640_Capture_Control(DISABLE);
         DMA_ITConfig(DMA2_Stream1,DMA_IT_TC,DISABLE); 
         DCMI_Cmd(DISABLE); 
         DCMI_CaptureCmd(DISABLE); 
         rt_thread_delete(h1);
         GUI_VMEM_Free(bits);
      
         return PostQuitMessage(hwnd);	
      }    
 		case WM_NOTIFY: //WM_NOTIFY��Ϣ:wParam��16λΪ���͸���Ϣ�Ŀؼ�ID,��16λΪ֪ͨ��;lParamָ����һ��NMHDR�ṹ��.
		{
			u16 code,id;
         static int flag = 0;
			code =HIWORD(wParam); //���֪ͨ������.
			id   =LOWORD(wParam); //��ò�������Ϣ�Ŀؼ�ID.
         if(flag == 0)
         {
            if(id==eID_SET && code==BN_CLICKED)
            {
               flag = 1;
               WNDCLASS wcex;


               wcex.Tag	 		= WNDCLASS_TAG;
               wcex.Style			= CS_HREDRAW | CS_VREDRAW;
               wcex.lpfnWndProc	= (WNDPROC)dlg_set_WinProc;
               wcex.cbClsExtra		= 0;
               wcex.cbWndExtra		= 0;
               wcex.hInstance		= NULL;
               wcex.hIcon			= NULL;
               wcex.hCursor		= NULL;

               if(1)
               {
                  RECT rc;

                  GetClientRect(hwnd,&rc);
                  //ClientToScreen(hwnd,(POINT*)&rc.x,1);

                  win_rc.w =400;
                  win_rc.h =400;

                  win_rc.x = rc.x+(rc.w-win_rc.w)/2;
                  win_rc.y = rc.y;//rc.y+(rc.h>>2);

                  SetWIN = CreateWindowEx(
                              NULL,
                              &wcex,L"Set",
                              WS_OVERLAPPED|WS_CLIPCHILDREN|WS_VISIBLE,

                              win_rc.x,-win_rc.h-4,win_rc.w,win_rc.h,
                              hwnd,0,NULL,NULL);
               }

            }
         }
         else
         {
            flag = 0;
            PostCloseMessage(SetWIN);
         
         }
         break;  
		}
		   
      default:
         return DefWindowProc(hwnd, msg, wParam, lParam);
   }
   return WM_NULL;
}


//����ͷ����
void	GUI_Camera_DIALOG(void)
{	
	WNDCLASS	wcex;
	MSG msg;

   g_dma2d_en = FALSE;
	wcex.Tag = WNDCLASS_TAG;

	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WinProc; //������������Ϣ����Ļص�����.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);

	//����������
	Cam_hwnd = CreateWindowEx(WS_EX_NOFOCUS,
                                    &wcex,
                                    L"GUI_Camera_Dialog",
                                    WS_VISIBLE|WS_CLIPCHILDREN,
                                    0, 0, GUI_XSIZE, GUI_YSIZE,
                                    NULL, NULL, NULL, NULL);

	//��ʾ������
	ShowWindow(Cam_hwnd, SW_SHOW);

	//��ʼ������Ϣѭ��(���ڹرղ�����ʱ,GetMessage������FALSE,�˳�����Ϣѭ��)��
	while (GetMessage(&msg, Cam_hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}
