#include "emXGUI.h"
/*�ؼ�����*/
//�˳������Ŀ��
#define EXIT_H		 (80)
#define EXIT_W		 (80)
#define EXIT_POS_X	 (GUI_XSIZE - EXIT_W)	
#define ID_EXIT		 0x1000

static void button_owner_draw(DRAWITEM_HDR *ds)
{
	HDC hdc;
	RECT rc;
	WCHAR wbuf[128];
	hdc = ds->hDC;   //button�Ļ�ͼ�����ľ��.
	rc = ds->rc;     //button�Ļ��ƾ�����.

	if (ds->State & BST_PUSHED)
	{ //��ť�ǰ���״̬
		SetBrushColor(hdc, MapRGB(hdc, 150, 200, 250)); //�������ɫ(BrushColor��������Fill���͵Ļ�ͼ����)
		SetPenColor(hdc, MapRGB(hdc, 250, 0, 0));        //���û���ɫ(PenColor��������Draw���͵Ļ�ͼ����)
		SetTextColor(hdc, MapRGB(hdc, 250, 0, 0));      //��������ɫ
	}
	else
	{ //��ť�ǵ���״̬
		SetBrushColor(hdc, MapRGB(hdc, 30, 150, 30));
		SetPenColor(hdc, MapRGB(hdc, 0, 250, 0));
		SetTextColor(hdc, MapRGB(hdc, 0, 50, 100));
	}
	FillRect(hdc, &rc); //�þ�����䱳��
	DrawRect(hdc, &rc); //���������
	GetWindowText(ds->hwnd, wbuf, 128); //��ð�ť�ؼ�������
	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER | DT_CENTER);//��������(���ж��뷽ʽ)

}


static LRESULT win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	RECT rc_cli;
	switch (msg)
	{
	case WM_CREATE:
	{
		GetClientRect(hwnd, &rc_cli);
		CreateWindow(BUTTON, L"��", WS_VISIBLE|WS_OWNERDRAW, EXIT_POS_X, 0, 80, 80, hwnd, ID_EXIT, NULL, NULL);
	}
	case WM_DRAWITEM:
	{
		DRAWITEM_HDR *ds;
		ds = (DRAWITEM_HDR*)lParam;
		if (ds->ID == ID_EXIT)
		{
			button_owner_draw(ds); //ִ���Ի��ư�ť
		}
		return TRUE;

	}
	default: 
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

}


void	GUI_LED_DIALOG(void)
{
	HWND	hwnd;
	WNDCLASS	wcex;
	MSG msg;

	/////
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
