#include <emXGUI.h>
#include "./camera/bsp_ov5640.h"
#include "x_libc.h"
#include "./camera/ov5640_AF.h"

#define ID_BUTTON_Exit  0x1000
#define FONT_H          72
#define FONT_W          72

uint8_t focus_status = 0;
rt_thread_t h1;
BOOL update_flag = 0;//֡�ʸ��±�־
static RECT win_rc;
static int b_close=FALSE;
uint8_t fps=0;//֡��
OV5640_IDTypeDef OV5640_Camera_ID;
RECT rc_fps = {0,400,800,72};//֡����ʾ�Ӵ���
HWND Cam_hwnd;//�����ھ��
int state = 0;
U16 *bits;
GUI_SEM *cam_sem = NULL;//ͬ���ź�������ֵ�ͣ�
int fgg = 0;//Ĭ��û�б�����
//����ؼ�ID
enum eID
{
	eID_OK =0x1000,
	eID_SET,
	eID_SET1,
	eID_SET2,
	eID_SET3,
	eID_SET4,
   eID_SET5,
   eID_Focus,
   eID_WhiteBalance,
   eID_SCROLLBAR
   
};

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

	if (fgg)
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
/*============================================================================*/
static LRESULT	dlg_set_xx_WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
		case WM_CREATE: //���ڴ���ʱ,���Զ���������Ϣ,��������һЩ��ʼ���Ĳ����򴴽��Ӵ���.
		{
         RECT rc;
         GetClientRect(hwnd, &rc);
         
         
         
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

			rc.x =10;
			rc.y =20;
			rc.w =200;
			rc.h =24;
			DrawText(hdc,L"Set xxx..",-1,&rc,DT_CENTER|DT_VCENTER|DT_BKGND);
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
//         if (ds->ID >= 0x1000 && ds->ID<= 0x1099)
//         {
//            button_owner_draw(ds);
//            return TRUE;
//         }
      } 
		default: //�û������ĵ���Ϣ,��ϵͳ����.
		{
			return DefWindowProc(hwnd,msg,wParam,lParam);
		}

	}
	return WM_NULL;

}

static LRESULT	dlg_set_WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc,rc0;
   static SCROLLINFO sif;
	switch(msg)
	{
		case WM_CREATE: //���ڴ���ʱ,���Զ���������Ϣ,��������һЩ��ʼ���Ĳ����򴴽��Ӵ���.
		{
			b_close =FALSE;

//			GetClientRect(hwnd,&rc0); //��ô��ڵĿͻ�������.
//			CreateWindow(BUTTON,L"OK",WS_VISIBLE,rc0.w-80,8,68,32,hwnd,eID_OK,NULL,NULL);
			rc.x =10;
			rc.y =20;
			rc.w =200;
			rc.h =40;
         CreateWindow(TEXTBOX,L"�Զ��Խ�",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_Focus,NULL,NULL);
         SendMessage(GetDlgItem(hwnd, eID_Focus),TBM_SET_TEXTFLAG,0,
                     DT_SINGLELINE|DT_LEFT|DT_VCENTER|DT_BKGND); 
         
			rc.y =60;
         CreateWindow(TEXTBOX,L"��ƽ��",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_WhiteBalance,NULL,NULL);
         SendMessage(GetDlgItem(hwnd, eID_WhiteBalance),TBM_SET_TEXTFLAG,0,
                     DT_SINGLELINE|DT_LEFT|DT_VCENTER|DT_BKGND);         
         
         
			rc.x =10;
			rc.y =100;
			rc.w =80;
			rc.h =40;
			CreateWindow(BUTTON,L"���Ͷ�",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_SET4,NULL,NULL);

			OffsetRect(&rc,0,rc.h+4);
			CreateWindow(BUTTON,L"�Աȶ�",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_SET3,NULL,NULL);

			OffsetRect(&rc,0,rc.h+4);
			CreateWindow(BUTTON,L"�ֱ���",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_SET1,NULL,NULL);

			OffsetRect(&rc,0,rc.h+4);
			CreateWindow(BUTTON,L"����ģʽ",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_SET2,NULL,NULL);

			OffsetRect(&rc,0,rc.h+4);
			CreateWindow(BUTTON,L"����Ч��",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_SET5,NULL,NULL);
			//SetTimer(hwnd,1,500,TMR_START,NULL);
			SetTimer(hwnd,2,20,TMR_START,NULL);
         
         sif.cbSize = sizeof(sif);
         sif.fMask = SIF_ALL;
         sif.nMin = 0;
         sif.nMax = 63;//�������ֵΪ63
         sif.nValue = 20;//��ʼ����ֵ
         sif.TrackSize = 31;//����ֵ
         sif.ArrowSize = 0;//���˿��Ϊ0��ˮƽ��������
         CreateWindow(SCROLLBAR, L"SCROLLBAR_R", WS_VISIBLE, 
                       220, 64, 150, 31, hwnd, eID_SCROLLBAR, NULL, NULL);
         SendMessage(GetDlgItem(hwnd, eID_SCROLLBAR), SBM_SETSCROLLINFO, TRUE, (LPARAM)&sif);
         
         
         CreateWindow(BUTTON,L" ",WS_OWNERDRAW|WS_VISIBLE,
                      280,30,60,30,hwnd,0x1234,NULL,NULL);
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
         if (ctr_id == eID_SCROLLBAR)
         {
            NM_SCROLLBAR *sb_nr;
            sb_nr = (NM_SCROLLBAR*)nr; //Scrollbar��֪ͨ��Ϣʵ��Ϊ NM_SCROLLBAR��չ�ṹ,���渽���˸������Ϣ.
            switch (nr->code)
            {
               case SBN_THUMBTRACK: //R�����ƶ�
               {
                  i= sb_nr->nTrackValue; //�õ���ǰ������ֵ
                  SendMessage(nr->hwndFrom, SBM_SETVALUE, TRUE, i); //����SBM_SETVALUE����������ֵ
               }
               break;
            }
         } 
			if(id==eID_SET1 && code==BN_CLICKED)
			{
				WNDCLASS wcex;

				GetWindowRect(GetDlgItem(hwnd,eID_SET1),&rc);
				ScreenToClient(hwnd,(POINT*)&rc.x,1);
				rc.x += rc.w+1;
				rc.w =180;
				rc.h =100;
            //rc.y = 64;
				wcex.Tag	 		= WNDCLASS_TAG;
				wcex.Style			= CS_HREDRAW | CS_VREDRAW;
				wcex.lpfnWndProc	= (WNDPROC)dlg_set_xx_WinProc;
				wcex.cbClsExtra		= 0;
				wcex.cbWndExtra		= 0;
				wcex.hInstance		= NULL;
				wcex.hIcon			= NULL;
				wcex.hCursor		= NULL;


				CreateWindowEx(
								WS_EX_FRAMEBUFFER,
								&wcex,L"Set_1_xxx",
								WS_OVERLAPPED|WS_CLIPCHILDREN|WS_VISIBLE,
								rc.x,rc.y,rc.w,rc.h,
								hwnd,0,NULL,NULL);

			}

			if(id==eID_SET2 && code==BN_CLICKED)
			{
				WNDCLASS wcex;

				GetWindowRect(GetDlgItem(hwnd,eID_SET2),&rc);
				ScreenToClient(hwnd,(POINT*)&rc.x,1);
				rc.x += rc.w+1;
				rc.w =220;
				rc.h =100;

				wcex.Tag	 		= WNDCLASS_TAG;
				wcex.Style			= CS_HREDRAW | CS_VREDRAW;
				wcex.lpfnWndProc	= (WNDPROC)dlg_set_xx_WinProc;
				wcex.cbClsExtra		= 0;
				wcex.cbWndExtra		= 0;
				wcex.hInstance		= NULL;
				wcex.hIcon			= NULL;
				wcex.hCursor		= NULL;


				CreateWindowEx(
								WS_EX_FRAMEBUFFER,
								&wcex,L"Set_2_xxx",
								WS_OVERLAPPED|WS_CLIPCHILDREN|WS_CAPTION|WS_DLGFRAME|WS_BORDER|WS_VISIBLE|WS_CLOSEBOX,
								rc.x,rc.y,rc.w,rc.h,
								hwnd,0,NULL,NULL);

			}

			if(id==eID_OK && code==BN_CLICKED) // ��ť����������.
			{
				PostCloseMessage(hwnd); //ʹ����WM_CLOSE��Ϣ�رմ���.
			}
         
			if (id == 0x1234 && code == BN_CLICKED)
			{
				fgg = ~fgg;

			}         
         
		}
		break;
		////
      
//      case WM_ERASEBKGND:
//      {
//         HDC hdc =(HDC)wParam;
//         HDC hdc_mem;
//         RECT rc;
//         GetClientRect(hwnd, &rc);
//         hdc_mem = CreateMemoryDC(SURF_ARGB4444, rc.w, rc.h);
//         
//         SetPenColor(hdc_mem, MapARGB(hdc, 255, 0, 0, 0));
//         SetBrushColor(hdc_mem, MapARGB(hdc, 125, 112,128,144));
//         
//         DrawRect(hdc_mem, &rc);
//         FillRect(hdc_mem, &rc);
//         
//         
//         BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_mem, rc.x, rc.y, SRCCOPY);
//         DeleteDC(hdc_mem);
//      }

		case WM_PAINT: //������Ҫ����ʱ�����Զ���������Ϣ.
		{
			PAINTSTRUCT ps;
			HDC hdc;
			RECT rc;
			hdc =BeginPaint(hwnd,&ps); //��ʼ��ͼ

			////�û��Ļ�������...
         GetClientRect(hwnd, &rc);
//			SetTextColor(hdc,MapRGB(hdc,240,10,10));
			SetBrushColor(hdc,MapRGB(hdc,0,0,0));
         FillRect(hdc, &rc);
//			rc.x =10;
//			rc.y =20;
//			rc.w =200;
//			rc.h =24;
//			DrawText(hdc,L"Set...",-1,&rc,DT_CENTER|DT_VCENTER|DT_BKGND);
//			//TextOut(hdc,10,10,L"Hello",-1);

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
			if(id== eID_Focus || id == eID_WhiteBalance)
			{
				
				
				cr->TextColor =RGB888(255,255,255);//������ɫ��RGB888��ɫ��ʽ)
				cr->BackColor =RGB888(0,0,0);//������ɫ��RGB888��ɫ��ʽ)
				cr->BorderColor =RGB888(255,0,0);//�߿���ɫ��RGB888��ɫ��ʽ)
				return TRUE;
			}
         if(id >=eID_SET1 && id <= eID_SET5)
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
			if (ds->ID == 0x1234)
			{
				Checkbox_owner_draw(ds); //ִ���Ի��ư�ť
			}
         if (ds->ID == eID_SCROLLBAR)
         {
            scrollbar_owner_draw(ds);
            return TRUE;
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

static LRESULT WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   switch(msg)
   {
      case  WM_CREATE:
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
        h1=rt_thread_create("Update_Dialog",(void(*)(void*))Update_Dialog,NULL,4096,5,5);
        rt_thread_startup(h1);	
        bits = (U16 *)GUI_VMEM_Alloc(800*480); 
		  SetTimer(hwnd,1,1000,TMR_START,NULL);  
        RECT rc;
        GetClientRect(hwnd, &rc);
        CreateWindow(BUTTON,L"Set",NULL,rc.w-80,rc.h-32-2,68,32,hwnd,eID_SET,NULL,NULL);        
        
        
        break;
      }
//      case WM_LBUTTONDOWN:
//      {
//         POINT pt;
//         pt.x =GET_LPARAM_X(lParam); //���X����
//         pt.y =GET_LPARAM_Y(lParam); //���Y����
//         RECT rc = {718, 0, 72, 72};
//         if(PtInRect(&rc, &pt))
//         {
//            
//            //����WM_CLOSE��Ϣ�ر�������
//         }
//         else
//         {
//            PostCloseMessage(hwnd);
//            
//            show_menu = ~show_menu;
//         }
//         
//         break;
//      }
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
      case WM_PAINT:
      {
         PAINTSTRUCT ps;
         SURFACE *pSurf;
         HDC hdc_mem;
         HDC hdc;
         RECT rc;
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
            hdc_mem =CreateDC(pSurf,NULL);
            if(update_flag)
            {
               update_flag = 0;
               old_fps = fps;
               fps = 0;
            } 
            SetTextColor(hdc_mem, MapRGB(hdc_mem, 255,255,255));                 
            x_wsprintf(wbuf,L"֡��FPS:%d/s",old_fps);
            DrawText(hdc_mem, wbuf, -1, &rc_fps, DT_SINGLELINE| DT_VCENTER|DT_CENTER);
            
//            /****************�����˳���ť******************/
//            SetBrushColor(hdc_mem, MapRGB(hdc_mem, 0,0,0));
//            FillCircle(hdc_mem, rc.w, 0, 54);
//            //�߿�
//            SetBrushColor(hdc_mem, MapRGB(hdc_mem, 250,0,0));
//            FillCircle(hdc_mem, rc.w, 0, 50);
//            
//            SetFont(hdc_mem, hFont_SDCARD);
//            TextOut(hdc_mem, rc.w - 20, 0, L"O", -1);
//            
//            if(show_menu)
//            {
//               RECT rc = {0, 0, 72, 480};
//               
//               SetBrushColor(hdc_mem, MapARGB(hdc_mem,50, 0,0,0));
//               FillRect(hdc_mem, &rc);
//            }            
            BitBlt(hdc, 0, 0, 800, 480, 
                   hdc_mem, 0, 0, SRCCOPY);          
            DeleteSurface(pSurf);
            DeleteDC(hdc_mem);
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
         GUI_CameraAvrg_DIALOG();
         return PostQuitMessage(hwnd);	
      }    
 		case WM_NOTIFY: //WM_NOTIFY��Ϣ:wParam��16λΪ���͸���Ϣ�Ŀؼ�ID,��16λΪ֪ͨ��;lParamָ����һ��NMHDR�ṹ��.
		{
			u16 code,id;

			code =HIWORD(wParam); //���֪ͨ������.
			id   =LOWORD(wParam); //��ò�������Ϣ�Ŀؼ�ID.

			if(id==eID_SET && code==BN_CLICKED)
			{
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
					win_rc.h =360;

					win_rc.x = rc.x+(rc.w-win_rc.w)/2;
					win_rc.y = rc.y;//rc.y+(rc.h>>2);

					CreateWindowEx(
									NULL,
									&wcex,L"Set",
									WS_OVERLAPPED|WS_CLIPCHILDREN|WS_VISIBLE,

									win_rc.x,-win_rc.h-2,win_rc.w,win_rc.h,
									hwnd,0,NULL,NULL);
				}

			}
         break;  
		}
		   
      default:
         return DefWindowProc(hwnd, msg, wParam, lParam);
   }
   return WM_NULL;
}
void	GUI_Camera_DIALOG(void)
{	
	WNDCLASS	wcex;
	MSG msg;


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
