

#include <emXGUI.h>
/*============================================================================*/
//����ؼ�ID
#define	ID_OK		         0x1000
//����ͼ����ɫ
#define  DrawRect_col       RGB888(250,250,250)  //����
#define  DrawCir_col        RGB888(79,129,189)   //Բ
#define  DrawEllipse_col    RGB888(247,186,0)    //��Բ
#define  DrawLine_col       RGB888(146,208,80)   //��
//���ͼ����ɫ
#define  FillRTri_col       RGB888(0,176,240)    //ֱ��������
#define  FillEqTri_col      RGB888(146,208,80)   //�ȱ�������
#define  FillCir_col        RGB888(255,0,0)      //Բ
#define  FillRECT_col       RGB888(255,192,0)    //����
#define  FillRoundRECT_col  RGB888(79,129,189)   //Բ�Ǿ���
#define  FillEllipse_col    RGB888(112,48,160)   //��Բ
//����ɫ����
#define  Fillsquare_col0    RGB888(50,100,250)   
#define  Fillsquare_col1    RGB888(234,236,198)
//������
#define  FillCube_col       RGB888(50,150,250)   //����
#define  FillCubeside_col   RGB888(40,121,201)   //����
#define  FillCubetop_col    RGB888(90,171,251)   //����
//������
#define  Fillcuboid_col     RGB888(255,0,0)      //����
#define  Fillcuboidside_col RGB888(205,0,0)      //����
#define  Fillcuboidtop_col  RGB888(255,50,50)    //����


#define  FillRect_borcol    RGB888(220,220,220)   //��߿�
#define  FillborRect_col    RGB888(79,99,40)   //������ɫ
/*============================================================================*/
void Draw_Func(HWND hwnd, HDC hdc)
{
   RECT rc; 
   //���þ��β���
   rc.x =50;
   rc.y =80;
   rc.w =140;
   rc.h =75;
   SetPenColor(hdc,MapRGB888(hdc, DrawRect_col)); //���û�����ɫ(���ڻ��ߣ���).
   DrawRect(hdc,&rc); //����һ�����ľ���
   //����һ������Բ.

   SetPenColor(hdc,MapRGB888(hdc,DrawCir_col));
   DrawCircle(hdc,80,100,50); 
   //��б��
   SetPenColor(hdc,MapRGB888(hdc,DrawLine_col));
   Line(hdc,30,160,200,230); 
   //����Բ  
   SetPenColor(hdc,MapRGB888(hdc,DrawEllipse_col));
   DrawEllipse(hdc, 150, 155, 65, 40);
}

void Fill_Func(HWND hwnd, HDC hdc)
{
   POINT pt_RTri[3]={{35, 200},
                     {35, 300},
                     {235, 300}
                    };
   //����ֱ��������
   SetBrushColor(hdc, MapRGB888(hdc, FillRTri_col)); //������ɫ�� ʹ��BrushColor��
   FillPolygon(hdc, 0, 0, &pt_RTri[0],3); //������Ρ�  
   //���Ƶȱ�������
   POINT pt_EqTri[3]={{220, 120},{320, 120},{270, 13}};   
   SetBrushColor(hdc, MapRGB888(hdc, FillEqTri_col)); //������ɫ�� ʹ��BrushColor��
   FillPolygon(hdc, 0, 0, &pt_EqTri[0],3); //������Ρ�
   //����Բ
   SetBrushColor(hdc, MapRGB888(hdc, FillCir_col));
   FillCircle(hdc, 320, 250, 60);
   //���ƾ���
   RECT rc_rect = {280, 320, 165, 55};
   SetBrushColor(hdc, MapRGB888(hdc, FillRECT_col));
   FillRect(hdc, &rc_rect);  
   //����Բ�Ǿ���
   RECT rc_Roundrect = {390, 160, 55, 150};
   SetBrushColor(hdc, MapRGB888(hdc, FillRoundRECT_col));
   FillRoundRect(hdc, &rc_Roundrect,14);
   //������Բ
   SetBrushColor(hdc, MapRGB888(hdc, FillEllipse_col));
   FillEllipse(hdc, 500, 80, 90, 55);   
   
   //���ƽ���ɫ������
   RECT rc_square = {470, 235, 140, 140};  
   GradientFillRect(hdc, &rc_square, MapRGB888(hdc, Fillsquare_col0), 
                    MapRGB888(hdc, Fillsquare_col1), FALSE);
   //���������嶥��
   RECT Cube = {630, 120, 100, 100};
   SetBrushColor(hdc, MapRGB888(hdc, FillCube_col));
   FillRect(hdc, &Cube);
   //�������������
   POINT pt_SidePolygon[4]={{730, 120}, {730, 220},
                            {765, 185}, {765, 85}};
   SetBrushColor(hdc, MapRGB888(hdc, FillCubeside_col));
   FillPolygon(hdc, 0, 0, &pt_SidePolygon[0],4); //������Ρ�
   //��������������                     
   POINT pt_TopPolygon[4]={{665, 85},  {630, 120},
                           {730, 120}, {765, 85}};
   SetBrushColor(hdc, MapRGB888(hdc, FillCubetop_col));
   FillPolygon(hdc, 0, 0, &pt_TopPolygon[0],4); //������Ρ�
                           
   //���Ƴ�����
   RECT cuboid = {650, 280, 60, 165};
   SetBrushColor(hdc, MapRGB888(hdc, Fillcuboid_col));
   FillRect(hdc, &cuboid);
   //���Ƴ��������
   POINT pt_cuboidSide[4]={{710, 280}, {710, 445},
                           {731, 424}, {731, 259}};
   SetBrushColor(hdc, MapRGB888(hdc, Fillcuboidside_col));
   FillPolygon(hdc, 0, 0, &pt_cuboidSide[0],4); //������Ρ�
   //���Ƴ���������                  
   POINT pt_cuboidTop[4]={{671, 259},  {650, 280},
                           {710, 280}, {731, 259}};
   SetBrushColor(hdc, MapRGB888(hdc, Fillcuboidtop_col));
   FillPolygon(hdc, 0, 0, &pt_cuboidTop[0],4); //������Ρ�  


   RECT rc_borrect = {25, 330, 220, 100};
   SetBrushColor(hdc, MapRGB888(hdc, FillRect_borcol));//���þ��ε���߿���ɫ
   FillRect(hdc, &rc_borrect);    
   
   InflateRect(&rc_borrect, -5, -5);//�����������ߵ�λ�ü���5������ֵ
   SetBrushColor(hdc, MapRGB888(hdc, FillborRect_col));//���þ��ε���ɫ
   FillRect(hdc, &rc_borrect);    
}

static LRESULT	WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc;
	switch(msg)
	{
		case WM_CREATE: //���ڴ���ʱ,���Զ���������Ϣ,��������һЩ��ʼ���Ĳ����򴴽��Ӵ���.
		{
			GetClientRect(hwnd,&rc); //��ô��ڵĿͻ�������.
			CreateWindow(BUTTON,L"OK",WS_VISIBLE,rc.w-80,8,68,32,hwnd,ID_OK,NULL,NULL); //����һ����ť(ʾ��).
         return TRUE;
		}
		case WM_NOTIFY: //WM_NOTIFY��Ϣ:wParam��16λΪ���͸���Ϣ�Ŀؼ�ID,��16λΪ֪ͨ��;lParamָ����һ��NMHDR�ṹ��.
		{
			u16 code,id;

			code =HIWORD(wParam); //���֪ͨ������.
			id   =LOWORD(wParam); //��ò�������Ϣ�Ŀؼ�ID.

			if(id==ID_OK && code==BN_CLICKED) // ��ť����������.
			{
				PostCloseMessage(hwnd); //ʹ����WM_CLOSE��Ϣ�رմ���.
			}
         break;
		}
		case WM_PAINT: //������Ҫ����ʱ�����Զ���������Ϣ.
		{
			PAINTSTRUCT ps;
			HDC hdc;
         RECT rc;
			hdc =BeginPaint(hwnd,&ps); //��ʼ��ͼ
         GetClientRect(hwnd,&rc);
         SetBrushColor(hdc, MapRGB(hdc, 0, 0, 0));
         FillRect(hdc, &rc);
         Draw_Func(hwnd, hdc);
         Fill_Func(hwnd, hdc);        
			EndPaint(hwnd,&ps); //������ͼ
         break;
		}
		default: //�û������ĵ���Ϣ,��ϵͳ����.
		{
			return DefWindowProc(hwnd,msg,wParam,lParam);
		}

	}
	return WM_NULL;
}

/*============================================================================*/
void	GUI_DEMO_DrawTest(void)
{
	HWND	hwnd;
	WNDCLASS	wcex;
	MSG msg;
	wcex.Tag 		    = WNDCLASS_TAG;

	wcex.Style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WinProc; //������������Ϣ����Ļص�����.
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= NULL;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;

	//����������
	hwnd	=CreateWindowEx(NULL,
								&wcex,
								_T("emXGUI Window"), //��������
								WS_CLIPCHILDREN,
								0,0,GUI_XSIZE,GUI_YSIZE,    //����λ�úʹ�С
								NULL,NULL,NULL,NULL);

	//��ʾ������
	ShowWindow(hwnd,SW_SHOW);

	//��ʼ������Ϣѭ��(���ڹرղ�����ʱ,GetMessage������FALSE,�˳�����Ϣѭ��)��
	while(GetMessage(&msg,hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

/*============================================================================*/
