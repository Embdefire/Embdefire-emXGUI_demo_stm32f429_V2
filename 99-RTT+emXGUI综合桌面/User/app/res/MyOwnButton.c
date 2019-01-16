#include <emXGUI.h>

//����ؼ���֪ͨ��
#define MYBTN_UP   0
#define MYBTN_DOWN 1

//����ؼ���˽����Ϣ(���пؼ���˽����ϢҪ�� WM_WIDGET ��ʼ����)
#define MSG_MYBTN_SET_STATE WM_WIDGET+0 //���ð�Ť״̬
#define MSG_MYBTN_GET_STATE WM_WIDGET+1 //��ð�Ť״̬
//����ؼ���״̬
#define BTN_STATE_PRESSED (1<<0)
//����ؼ�����չ���ݽṹ.
typedef struct
{
	u32 state; //���ڼ�¼��ť״̬.
}my_btn_ext;
//�ؼ��Ĵ��ڹ��̺���
static LRESULT	my_btn_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	my_btn_ext *btn_ex;
	btn_ex = (my_btn_ext*)GetWindowLong(hwnd, GWL_EXDATA_ADDR); //��ô�����չ��������
	switch (msg)
	{
	case WM_CREATE: //���ڴ���ʱ,���Զ���������Ϣ,��������һЩ��ʼ���Ĳ����򴴽��Ӵ���.
	{
		btn_ex->state = 0;
	}
	return TRUE;
	case WM_LBUTTONDOWN: //������/�����ڴ����ڰ����ˡ�
	{
		btn_ex->state = ~btn_ex->state; //���� �����¡�״̬.
		NotifyParent(hwnd, MYBTN_DOWN); //��֪ͨ��Ϣ��������
		//ʹ�ô����ػ�.
		InvalidateRect(hwnd, NULL, FALSE);
	}
	break;
	//		case WM_LBUTTONUP:  //������/�����ڴ����ڡ�̧���ˡ�
	//		{
	//			btn_ex->state  &= ~BTN_STATE_PRESSED; //��������¡�״̬.
	//			NotifyParent(hwnd,MYBTN_UP); //��֪ͨ��Ϣ��������
	//			//ʹ�ô����ػ�.
	//			InvalidateRect(hwnd,NULL,FALSE);
	//		}
	case MSG_MYBTN_SET_STATE: //���ð�Ť״̬
	{
		btn_ex->state = wParam;

		InvalidateRect(hwnd, NULL, FALSE); //�ı�״̬��Ӧ��Ҫ�ػ���һ��...
	}
	break;
	case MSG_MYBTN_GET_STATE: //��ð�Ť״̬
	{
		return btn_ex->state;
	}

	case WM_ERASEBKGND:
	{
		RECT rc;
		HDC hdc = (HDC)wParam;
      HDC hdc_mem;
      
		GetClientRect(hwnd, &rc); //��ô��ڵĿͻ�������
      hdc_mem = CreateMemoryDC(SURF_ARGB4444, rc.w, rc.h);
		SetBrushColor(hdc_mem, MapARGB(hdc_mem, 0, 255, 255, 255));
		FillRect(hdc_mem, &rc);
      BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_mem, 0, 0, SRCCOPY);
      DeleteDC(hdc_mem);
	}
	break;

	case WM_PAINT: //������Ҫ����ʱ�����Զ���������Ϣ.
	{
		PAINTSTRUCT ps;
		HDC hdc;
		RECT rc;
		WCHAR wbuf[64];
		hdc = BeginPaint(hwnd, &ps); //��ʼ��ͼ

		GetClientRect(hwnd, &rc); //��ô��ڵĿͻ�������
//		SetBrushColor(hdc, MapRGB(hdc, 255, 255, 255));
//		FillRect(hdc, &rc);


		GetWindowText(hwnd, wbuf, 64); //��ô�������.
		GetClientRect(hwnd, &rc);
		if (btn_ex->state)
		{ // �����¡�״̬
			SetTextColor(hdc, MapRGB(hdc, 250, 10, 10));
			SetPenColor(hdc, MapRGB(hdc, 0, 255, 0));
			SetBrushColor(hdc, MapRGB(hdc, 0, 255, 0));
		}
		else
		{	// �ǡ����¡�״̬
			SetTextColor(hdc, MapRGB(hdc, 0, 30, 30));
			SetPenColor(hdc, MapRGB(hdc, 255, 0, 0));
			SetBrushColor(hdc, MapRGB(hdc, 255, 0, 0));
		}
		//DrawCircle(hdc, rc.x + rc.w / 4, rc.y + rc.w / 4, rc.w / 4 - 2);
		FillCircle(hdc, rc.x + rc.w / 4, rc.x + rc.w / 4, rc.w / 4-2);

		EndPaint(hwnd, &ps); //������ͼ
	}
	break;
	//		case WM_CLOSE: //���ڹر�ʱ�����Զ���������Ϣ.
	//		{
	//			return DestroyWindow(hwnd); //����DestroyWindow�������ٴ���.
	//		}
	default: //�û������ĵ���Ϣ,��ϵͳ����.
	{
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	}
	return WM_NULL;
}




//����һ���ؼ���ṹʵ��.
const WNDCLASS my_roundbutton =
{
	.Tag = WNDCLASS_TAG,
	.Style = 0,
	.lpfnWndProc = my_btn_proc,
	.cbClsExtra = 0,
	.cbWndExtra = sizeof(my_btn_ext),
	.hInstance = NULL,
	.hIcon = NULL,
	.hCursor = NULL,
};

