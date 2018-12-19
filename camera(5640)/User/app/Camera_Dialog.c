#include <emXGUI.h>
#include "./camera/bsp_ov5640.h"
#include "x_libc.h"
#include "./camera/ov5640_AF.h"

#define ID_BUTTON_Exit  0x1000
#define FONT_H          72
#define FONT_W          72


uint8_t fps=0;//֡��
OV5640_IDTypeDef OV5640_Camera_ID;
RECT rc_fps = {17,17,80,80};//֡����ʾ�Ӵ���
HWND Cam_hwnd;//�����ھ��
int state = 0;
U16 *bits;
GUI_SEM *cam_sem = NULL;//ͬ���ź�������ֵ�ͣ�

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

uint8_t focus_status = 0;
rt_thread_t h1;
BOOL update_flag = 0;//֡�ʸ��±�־
BOOL show_menu = 0;//֡�ʸ��±�־
static LRESULT WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   switch(msg)
   {
      case  WM_CREATE:
      {
         RECT rc;
         GetClientRect(hwnd, &rc);
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


        
        break;
      }
      case WM_LBUTTONDOWN:
      {
         POINT pt;
         pt.x =GET_LPARAM_X(lParam); //���X����
         pt.y =GET_LPARAM_Y(lParam); //���Y����
         RECT rc = {718, 0, 72, 72};
         if(PtInRect(&rc, &pt))
         {
            PostCloseMessage(hwnd); //����WM_CLOSE��Ϣ�ر�������
         }
         else
         {
            show_menu = ~show_menu;
         }
         
         break;
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
            x_wsprintf(wbuf,L"֡��:%d/s",old_fps);
            DrawText(hdc_mem, wbuf, -1, &rc_fps, DT_SINGLELINE| DT_VCENTER);
            
            /****************�����˳���ť******************/
            SetBrushColor(hdc_mem, MapRGB(hdc_mem, 255,0,0));
            FillCircle(hdc_mem, rc.w, 0, 82);
            
            
            SetFont(hdc_mem, hFont_SDCARD);
            TextOut(hdc_mem, 743, 0, L"O", -1);
            
            if(show_menu)
            {
               RECT rc = {0, 0, 72, 480};
               
               SetBrushColor(hdc_mem, MapARGB(hdc_mem,50, 0,0,0));
               FillRect(hdc_mem, &rc);
            }            
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
         DMA_ITConfig(DMA2_Stream1,DMA_IT_TC,DISABLE); 
         DCMI_Cmd(DISABLE); 
         DCMI_CaptureCmd(DISABLE); 
         rt_thread_delete(h1);
         GUI_VMEM_Free(bits);
         return PostQuitMessage(hwnd);	
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
                                    WS_VISIBLE,
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
