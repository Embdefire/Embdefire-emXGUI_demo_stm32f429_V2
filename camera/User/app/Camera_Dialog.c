#include <emXGUI.h>
#include "./camera/bsp_ov2640.h"
#include "x_libc.h"
uint8_t fps=0;//֡��
OV2640_IDTypeDef OV2640_Camera_ID;
RECT rc_fps = {17,17,80,80};
HWND hwnd;//�����ھ��
static int state = 0;
U16 *bits;

static LRESULT WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   switch(msg)
   {
      case  WM_CREATE:
      {
        /* ��ȡ����ͷоƬID��ȷ������ͷ�������� */
        OV2640_ReadID(&OV2640_Camera_ID);

        if(OV2640_Camera_ID.PIDH  == 0x26)
        {
          GUI_DEBUG(" OV2640 ID:%x %x",OV2640_Camera_ID.Manufacturer_ID1 ,OV2640_Camera_ID.Manufacturer_ID2);
        }
        else
        {
          //GUI_DEBUG("û�м�⵽OV2640����ͷ�������¼�����ӡ�");
          MSGBOX_OPTIONS ops;
          const WCHAR *btn[]={L"ȷ��"};
          int x,y,w,h;

          ops.Flag =MB_BTN_WIDTH(60)|MB_ICONERROR;
          ops.pButtonText =btn;
          ops.ButtonCount =1;
          w =400;
          h =200;
          x =(GUI_XSIZE-w)>>1;
          y =(GUI_YSIZE-h)>>1;
          MessageBox(hwnd,x,y,w,h,L"û�м�⵽OV2640����ͷ��\n�����¼�����ӡ�",L"��Ϣ",&ops);           
          break;  
        }     
        OV2640_Init();
        OV2640_UXGAConfig(); 
         //ʹ��DCMI�ɼ�����
        DCMI_Cmd(ENABLE); 
        DCMI_CaptureCmd(ENABLE);       
        bits = GUI_VMEM_Alloc(800*480); 
		  SetTimer(hwnd,1,10,TMR_START,NULL);
 
        break;
      }
// 		case WM_TIMER:
//      {
//         state = 2;
//         InvalidateRect(hwnd,NULL,TRUE);
//         break;
//      }     
      case WM_PAINT:
      {
         PAINTSTRUCT ps;
         SURFACE *pSurf;
         HDC hdc_mem;
         HDC hdc;
         WCHAR wbuf[128];
         hdc = BeginPaint(hwnd,&ps);
         GUI_DEBUG("1\n");
         //if(state == 2)
         {
            pSurf =CreateSurface(SURF_RGB565,GUI_XSIZE, GUI_YSIZE, 0, bits);
            hdc_mem =CreateDC(pSurf,NULL);
            x_wsprintf(wbuf,L"֡��:%.1f/s",fps);
            DrawText(hdc_mem, wbuf, -1, &rc_fps, DT_SINGLELINE| DT_VCENTER);
            BitBlt(hdc, 0, 0, 800, 480, 
                   hdc_mem, 0, 0, SRCCOPY); 
         }
         DeleteSurface(pSurf);
         DeleteDC(hdc_mem);
         EndPaint(hwnd,&ps);
         break;
      }
      default:
         return DefWindowProc(hwnd, msg, wParam, lParam);
   }
   return WM_NULL;
}





void	GUI_VideoPlayer_DIALOG(void)
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
	hwnd = CreateWindowEx(WS_EX_NOFOCUS,
                                    &wcex,
                                    L"GUI_Camera_Dialog",
                                    WS_VISIBLE,
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
