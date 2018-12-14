#include "emXGUI.h"
#include  "gui_picture_port.h"
struct leddlg
{
	int col_R;  //R����ֵ
	int col_G;  //G����ֵ
	int col_B;  //B����ֵ
}leddlg_S;


/*������ID�궨��*/
#define ID_SCROLLBAR_R  0x1100
#define ID_SCROLLBAR_G  0x1101
#define ID_SCROLLBAR_B  0x1102


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
	GetClientRect(hwnd, &rc);
	/* ���� */
	SetBrushColor(hdc, MapRGB888(hdc, back_c));
	FillRect(hdc, &rc);

	/* ������ */
	/* �߿� */
	InflateRect(&rc, 0, -rc.h >> 2);
	SetBrushColor(hdc, MapRGB(hdc, 169, 169, 169));
	FillRoundRect(hdc, &rc, MIN(rc.w, rc.h) >> 1);

	InflateRect(&rc, -2, -2);
	SetBrushColor(hdc, MapRGB888(hdc, Page_c));
	FillRoundRect(hdc, &rc, MIN(rc.w, rc.h) >> 1);

	/* ���� */
	SendMessage(hwnd, SBM_GETTRACKRECT, 0, (LPARAM)&rc);

	SetBrushColor(hdc, MapRGB(hdc, 169, 169, 169));
	rc.y += (rc.h >> 2) >> 1;
	rc.h -= rc.h >> 2;
	/* �߿� */
	FillRoundRect(hdc, &rc, MIN(rc.w, rc.h) >> 2);
	InflateRect(&rc, -2, -2);

	SetBrushColor(hdc, MapRGB888(hdc, fore_c));
	FillRoundRect(hdc, &rc, MIN(rc.w, rc.h) >> 2);
}
/*
 * @brief  �Զ���ص�����
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
	draw_scrollbar(hwnd, hdc_mem1, RGB888(0, 0, 0), RGB888(250, 250, 250), RGB888(255, 255, 255));
	//������ɫ���͵Ĺ�����
	switch (ds->ID)
	{
		case ID_SCROLLBAR_R:
		{
			draw_scrollbar(hwnd, hdc_mem, RGB888(0, 0, 0), RGB888(leddlg_S.col_R, 0, 0), RGB888(leddlg_S.col_R, 0, 0));
			break;
		}
		case ID_SCROLLBAR_G:
		{
			draw_scrollbar(hwnd, hdc_mem, RGB888(0, 0, 0), RGB888(0, leddlg_S.col_G, 0), RGB888(0, leddlg_S.col_G, 0));
			break;
		}
		case ID_SCROLLBAR_B:
		{
			draw_scrollbar(hwnd, hdc_mem, RGB888(0, 0, 0), RGB888(0, 0, leddlg_S.col_B), RGB888(0, 0, leddlg_S.col_B));
			break;
		}
	}
	SendMessage(hwnd, SBM_GETTRACKRECT, 0, (LPARAM)&rc);
	//��
	BitBlt(hdc, rc_cli.x, rc_cli.y, rc.x, rc_cli.h, hdc_mem, 0, 0, SRCCOPY);
	//��
	BitBlt(hdc, rc.x + rc.w, 0, rc_cli.w - (rc.x + rc.w), rc_cli.h, hdc_mem1, rc.x + rc.w, 0, SRCCOPY);

	//���ƻ���
	if (ds->State & SST_THUMBTRACK)//����
	{
		BitBlt(hdc, rc.x, 0, rc.w, rc_cli.h, hdc_mem, rc.x, 0, SRCCOPY);
	}
	else//δѡ��
	{
		BitBlt(hdc, rc.x, 0, rc.w, rc_cli.h, hdc_mem1, rc.x, 0, SRCCOPY);
	}
	//�ͷ��ڴ�MemoryDC
	DeleteDC(hdc_mem1);
	DeleteDC(hdc_mem);
}

static	LRESULT	win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	RECT rc;
	HWND wnd;
   BOOL res;
	static RECT rc_R, rc_G, rc_B;//RGB����ָʾ��
   static HDC hdc_mem2pic;
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

		/*����������--R*/
		wnd = CreateWindow(SCROLLBAR, L"SCROLLBAR_R", WS_OWNERDRAW | WS_VISIBLE, 160, 120, 255, 40, hwnd, ID_SCROLLBAR_R, NULL, NULL);
		SendMessage(wnd, SBM_SETSCROLLINFO, TRUE, (LPARAM)&sif);
		/*����R����ָʾ���λ�ô�С*/
		rc_R.x = 70;
		rc_R.y = 120;
		rc_R.w = 40;
		rc_R.h = 40;
		/*����������--G*/
		wnd = CreateWindow(SCROLLBAR, L"SCROLLBAR_G", WS_OWNERDRAW | WS_VISIBLE, 160, 240, 255, 40, hwnd, ID_SCROLLBAR_G, NULL, NULL);
		SendMessage(wnd, SBM_SETSCROLLINFO, TRUE, (LPARAM)&sif);
		/*����G����ָʾ���λ�ô�С*/
		rc_G.x = 70;
		rc_G.y = 240;
		rc_G.w = 40;
		rc_G.h = 40;
		/*����������--B*/
		wnd = CreateWindow(SCROLLBAR, L"SCROLLBAR_B", WS_OWNERDRAW | WS_VISIBLE, 160, 360, 255, 40, hwnd, ID_SCROLLBAR_B, NULL, NULL);
		SendMessage(wnd, SBM_SETSCROLLINFO, TRUE, (LPARAM)&sif);
		/*����B����ָʾ���λ�ô�С*/
		rc_B.x = 70;
		rc_B.y = 360;
		rc_B.w = 40;
		rc_B.h = 40;
      
      hdc_mem2pic = CreateMemoryDC(SURF_SCREEN, 800, 480);
       
//      if(res == FALSE)
//      {
//         GUI_DEBUG("failed\r");
//      }
	}
	case WM_NOTIFY: {
		NMHDR *nr;
		u16 ctr_id;
		ctr_id = LOWORD(wParam); //wParam��16λ�Ƿ��͸���Ϣ�Ŀؼ�ID.
		nr = (NMHDR*)lParam; //lParam����������NMHDR�ṹ�忪ͷ.
		if (ctr_id == ID_SCROLLBAR_R)
		{
			NM_SCROLLBAR *sb_nr;		
			sb_nr = (NM_SCROLLBAR*)nr; //Scrollbar��֪ͨ��Ϣʵ��Ϊ NM_SCROLLBAR��չ�ṹ,���渽���˸������Ϣ.
			switch (nr->code)
			{
				case SBN_THUMBTRACK: //R�����ƶ�
				{
					leddlg_S.col_R = sb_nr->nTrackValue; //��û��鵱ǰλ��ֵ
					SendMessage(nr->hwndFrom, SBM_SETVALUE, TRUE, leddlg_S.col_R); //����λ��ֵ
					InvalidateRect(hwnd, NULL, FALSE );//�����ػ棬����R����ָʾ��ı�
				}
				break;
			}
		}
		if (ctr_id == ID_SCROLLBAR_G)
		{
			NM_SCROLLBAR *sb_nr;
			sb_nr = (NM_SCROLLBAR*)nr; //Scrollbar��֪ͨ��Ϣʵ��Ϊ NM_SCROLLBAR��չ�ṹ,���渽���˸������Ϣ.
			switch (nr->code)
			{
				case SBN_THUMBTRACK: //G�����ƶ�
				{
					leddlg_S.col_G = sb_nr->nTrackValue; //��û��鵱ǰλ��ֵ
					SendMessage(nr->hwndFrom, SBM_SETVALUE, TRUE, leddlg_S.col_G); //����λ��ֵ
					InvalidateRect(hwnd, NULL, FALSE);//�����ػ棬����G����ָʾ��ı�
				}
				break;
			}
		}

		if (ctr_id == ID_SCROLLBAR_B)
		{
			NM_SCROLLBAR *sb_nr;
			sb_nr = (NM_SCROLLBAR*)nr; //Scrollbar��֪ͨ��Ϣʵ��Ϊ NM_SCROLLBAR��չ�ṹ,���渽���˸������Ϣ.
			switch (nr->code)
			{
			case SBN_THUMBTRACK: //B�����ƶ�
			{
				leddlg_S.col_B = sb_nr->nTrackValue; //���B���鵱ǰλ��ֵ
				SendMessage(nr->hwndFrom, SBM_SETVALUE, TRUE, leddlg_S.col_B); //����B�����λ��
				InvalidateRect(hwnd, NULL, FALSE);//�����ػ棬����B����ָʾ��ı�
			}
			break;
			}
		}
		break;
	}
	case WM_DRAWITEM:
	{
		DRAWITEM_HDR *ds;
		ds = (DRAWITEM_HDR*)lParam;
		if(ds->ID == ID_SCROLLBAR_R || ds->ID == ID_SCROLLBAR_G || ds->ID == ID_SCROLLBAR_B)
		{
			scrollbar_owner_draw(ds);
			return TRUE;
		}
	}
   
   case WM_ERASEBKGND:
   {
      
      
      return TRUE;
      
   }
   
	case	WM_PAINT: //������Ҫ�ػ���ʱ�����Զ��յ�����Ϣ.
	{	
		PAINTSTRUCT ps;
      RECT rc_cli;
		WCHAR wbuf[128];		
		RECT rc_text = {115, 127, 40, 25};
      HDC hdc_mem;
		hdc = BeginPaint(hwnd, &ps);
      GetClientRect(hwnd, &rc_cli);//��ȡ�ͻ�������
      
      hdc_mem = CreateMemoryDC(SURF_SCREEN, 800, 480);//����MemoryDC
      
      SetBrushColor(hdc_mem, MapRGB(hdc_mem, 240, 250, 0));
      FillRect(hdc_mem, &rc_cli);
      
		SetPenColor(hdc_mem, MapRGB(hdc_mem, 255, 255, 255));
		/*�޸�Rָʾ�����ɫ*/
		SetBrushColor(hdc_mem, MapRGB(hdc_mem, leddlg_S.col_R, 0, 0));
		FillRect(hdc_mem, &rc_R);
		/*��ʾR����ֵ*/
		x_wsprintf(wbuf, L"%d", leddlg_S.col_R);		
		DrawTextEx(hdc_mem, wbuf, -1, &rc_text,
				     DT_CENTER, NULL);
      
		/*�޸�Gָʾ�����ɫ*/
		SetBrushColor(hdc_mem, MapRGB(hdc_mem, 0, leddlg_S.col_G, 0));
		FillRect(hdc_mem, &rc_G);
		/*��ʾG����ֵ*/
		x_wsprintf(wbuf, L"%d", leddlg_S.col_G);
		rc_text.y = 247;	
		DrawTextEx(hdc_mem, wbuf, -1, &rc_text,
				   DT_CENTER, NULL);
		/*�޸�Bָʾ�����ɫ*/
		SetBrushColor(hdc_mem, MapRGB(hdc_mem, 0, 0,  leddlg_S.col_B));
		FillRect(hdc_mem, &rc_B);
		/*��ʾG����ֵ*/
		x_wsprintf(wbuf, L"%d", leddlg_S.col_B);
		rc_text.y = 367;	
		DrawTextEx(hdc_mem, wbuf, -1, &rc_text,
			       DT_CENTER, NULL);
		/*RGB�Ƶ���ɫ*/
		SetBrushColor(hdc_mem, MapRGB(hdc_mem, leddlg_S.col_R, leddlg_S.col_G, leddlg_S.col_B));
		FillCircle(hdc_mem, 660, 260, 100);
      
      BitBlt(hdc, 0, 0, 800, 480, hdc_mem, 0, 0, SRCCOPY);
      
      DeleteDC(hdc_mem);
		EndPaint(hwnd, &ps);
		return	TRUE;
	}


	default:
		return	DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return	WM_NULL;
}


void	GUI_LED_DIALOG(void)
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
		L"GUI_LED_DIALOG",
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
