#include "emXGUI.h"
#include "Cdlg_MUSCIPLAYER.h"
#include "emXGUI_JPEG.h"
#include "Widget.h"
Cdlg_Master_Struct sCdlg_Master;//������APP����ṹ��

//ͼ���������
icon_S music_icon[12] = {
   {"yinliang",         {20,400,48,48},       FALSE},//����
   {"yinyueliebiao",    {668,404,72,72},      FALSE},//�����б�
   {"geci",             {728,404,72,72},      FALSE},//�����
   {"NULL",             {0,0,0,0},            FALSE},//��
   {"NULL",             {0,0,0,0},            FALSE},//��
   {"shangyishou",      {294, 404, 72, 72},   FALSE},//��һ��
   {"zanting/bofang",   {364, 406, 72, 72},   FALSE},//����
   {"xiayishou",        {448, 404, 72, 72},   FALSE},//��һ��
  
};


SCROLLINFO g_sif_power;//����������
SCROLLINFO g_sif;//��������
static void MUSICPLAYER_OwnerDraw_BTN(DRAWITEM_HDR *ds)
{
  HDC hdc; //�ؼ�����HDC
  HDC hdc_mem;//�ڴ�HDC����Ϊ������
  HWND hwnd; //�ؼ���� 
  RECT rc_cli;//�ؼ���λ�ô�С����
  WCHAR wbuf[128];
  hwnd = ds->hwnd;
  hdc = ds->hDC; 

  //��ȡ�ؼ���λ�ô�С��Ϣ
  GetClientRect(hwnd, &rc_cli);
  //��������㣬��ʽΪSURF_ARGB4444
  hdc_mem = CreateMemoryDC(SURF_ARGB4444, rc_cli.w, rc_cli.h);

  GetWindowText(ds->hwnd,wbuf,128); //��ð�ť�ؼ�������  


  SetBrushColor(hdc_mem,MapARGB(hdc_mem, 0, 255, 250, 250));
  FillRect(hdc_mem, &rc_cli);
  //���ż�ʹ��100*100������
  if(ds->ID == eID_BUTTON_START)
  SetFont(hdc_mem, controlFont_72);
  else if(ds->ID == eID_BUTTON_NEXT || ds->ID == eID_BUTTON_BACK)
  SetFont(hdc_mem, controlFont_64);
  else
  SetFont(hdc_mem, controlFont_48);
  //���ð�������ɫ
  SetTextColor(hdc_mem, MapARGB(hdc_mem, 250,250,250,250));
  //NEXT����BACK����LIST������ʱ���ı���ɫ
  if((ds->State & BST_PUSHED) )
  { //��ť�ǰ���״̬
    SetTextColor(hdc_mem, MapARGB(hdc_mem, 250,105,105,105));      //��������ɫ     
  }

  DrawText(hdc_mem, wbuf,-1,&rc_cli,DT_VCENTER);//��������(���ж��뷽ʽ)

  BitBlt(hdc, rc_cli.x, rc_cli.y, rc_cli.w, rc_cli.h, hdc_mem, 0, 0, SRCCOPY);

  DeleteDC(hdc_mem);  
}

static void MUSICPLAYER_DrawScrollBar(HWND hwnd, HDC hdc, COLOR_RGB32 back_c, COLOR_RGB32 Page_c, COLOR_RGB32 fore_c)
{
  RECT rc,rc_tmp;
  RECT rc_scrollbar;
  GetClientRect(hwnd, &rc);
  /* ���� */
  GetClientRect(hwnd, &rc_tmp);//�õ��ؼ���λ��
  GetClientRect(hwnd, &rc);//�õ��ؼ���λ��
  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//����ת��

  BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, sCdlg_Master.mhdc_bk, rc_tmp.x, rc_tmp.y, SRCCOPY);

  rc_scrollbar.x = rc.x;
  rc_scrollbar.y = rc.h/2-1;
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
  FillCircle(hdc, rc.x + rc.w / 2, rc.y + rc.h / 2, rc.h / 2 - 1);
  InflateRect(&rc, -2, -2);

  SetBrushColor(hdc, MapRGB888(hdc, fore_c));
  FillCircle(hdc, rc.x + rc.w / 2, rc.y + rc.h / 2, rc.h / 2 - 1);

}
/*
 * @brief  �Զ��廬�������ƺ���
 * @param  ds:	�Զ�����ƽṹ��
 * @retval NONE
*/
static void MUSICPLAYER_OwnerDraw_ScrollBar(DRAWITEM_HDR *ds)
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
	MUSICPLAYER_DrawScrollBar(hwnd, hdc_mem1, RGB888( 250, 250, 250), RGB888( 250, 250, 250), RGB888( 255, 255, 255));
	//������ɫ���͵Ĺ�����
	MUSICPLAYER_DrawScrollBar(hwnd, hdc_mem, RGB888( 250, 250, 250), RGB888(	50, 205, 50), RGB888(50, 205, 50));
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
		BitBlt(hdc, rc.x, 0, rc.w, rc_cli.h, hdc_mem, rc.x, 0, SRCCOPY);
	}
	//�ͷ��ڴ�MemoryDC
	DeleteDC(hdc_mem1);
	DeleteDC(hdc_mem);
}
//͸���ı�
static void Music_Button_OwnerDraw(DRAWITEM_HDR *ds) //����һ����ť���
{
	HWND hwnd;
	HDC hdc;
	RECT rc, rc_tmp;
	WCHAR wbuf[128];

	hwnd = ds->hwnd; //button�Ĵ��ھ��.
	hdc = ds->hDC;   //button�Ļ�ͼ�����ľ��.
  GetClientRect(hwnd, &rc_tmp);//�õ��ؼ���λ��
  GetClientRect(hwnd, &rc);//�õ��ؼ���λ��
  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//����ת��

  BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, sCdlg_Master.mhdc_bk, rc_tmp.x, rc_tmp.y, SRCCOPY);
  SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));


  GetWindowText(hwnd, wbuf, 128); //��ð�ť�ؼ�������

  DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);//��������(���ж��뷽ʽ)

}


static void Cdlg_Init(void)
{
  //Step1:��ʼ������
  u8 *jpeg_buf;
  u32 jpeg_size;
  JPG_DEC *dec;
  sCdlg_Master.LoadPic_State = RES_Load_Content(GUI_RGB_BACKGROUNG_PIC, (char**)&jpeg_buf, &jpeg_size);
  sCdlg_Master.mhdc_bk = CreateMemoryDC(SURF_SCREEN, 800, 480);
  if(sCdlg_Master.LoadPic_State)
  {
    /* ����ͼƬ���ݴ���JPG_DEC��� */
    dec = JPG_Open(jpeg_buf, jpeg_size);

    /* �������ڴ���� */
    JPG_Draw(sCdlg_Master.mhdc_bk, 0, 0, dec);

    /* �ر�JPG_DEC��� */
    JPG_Close(dec);
  }
  /* �ͷ�ͼƬ���ݿռ� */
  RES_Release_Content((char **)&jpeg_buf);   
  //Step2:
}
static LRESULT win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)
  { 
    case WM_CREATE:
    {
      
      Cdlg_Init();
      
      music_icon[0].rc.y = 440-music_icon[0].rc.h/2;//����
      CreateWindow(BUTTON,L"A",WS_OWNERDRAW |WS_VISIBLE,//��ť�ؼ�������Ϊ�Ի��ƺͿ���
                   music_icon[0].rc.x,music_icon[0].rc.y,//λ������Ϳؼ���С
                   music_icon[0].rc.w,music_icon[0].rc.h,//��music_icon[0]����
                   hwnd,eID_BUTTON_Power,NULL,NULL);//������hwnd,IDΪID_BUTTON_Power�����Ӳ���Ϊ�� NULL      
      //�����б�icon
      CreateWindow(BUTTON,L"G",WS_OWNERDRAW |WS_VISIBLE, //��ť�ؼ�������Ϊ�Ի��ƺͿ���
                   music_icon[1].rc.x,music_icon[1].rc.y,//λ������
                   music_icon[1].rc.w,music_icon[1].rc.h,//�ؼ���С
                   hwnd,eID_BUTTON_List,NULL,NULL);//������hwnd,IDΪID_BUTTON_List�����Ӳ���Ϊ�� NULL
      //���icon
      CreateWindow(BUTTON,L"W",WS_OWNERDRAW |WS_VISIBLE,
                   music_icon[2].rc.x,music_icon[2].rc.y,
                   music_icon[2].rc.w,music_icon[2].rc.h,
                   hwnd,eID_BUTTON_Equa,NULL,NULL);

      //��һ��
      CreateWindow(BUTTON,L"S",WS_OWNERDRAW |WS_VISIBLE,
                   music_icon[5].rc.x,music_icon[5].rc.y,
                   music_icon[5].rc.w,music_icon[5].rc.h,
                   hwnd,eID_BUTTON_BACK,NULL,NULL);
      //��һ�� 
      CreateWindow(BUTTON,L"V",WS_OWNERDRAW |WS_VISIBLE,
                   music_icon[7].rc.x,music_icon[7].rc.y,
                   music_icon[7].rc.w,music_icon[7].rc.h,
                   hwnd,eID_BUTTON_NEXT,NULL,NULL);
      //���ż�
      CreateWindow(BUTTON,L"U",WS_OWNERDRAW |WS_VISIBLE,
                   music_icon[6].rc.x,music_icon[6].rc.y,
                   music_icon[6].rc.w,music_icon[6].rc.h,
                   hwnd,eID_BUTTON_START,NULL,NULL);       
      /*********************����������******************/
      g_sif.cbSize = sizeof(g_sif);
      g_sif.fMask = SIF_ALL;
      g_sif.nMin = 0;
      g_sif.nMax = 255;
      g_sif.nValue = 0;//��ʼֵ
      g_sif.TrackSize = 30;//����ֵ
      g_sif.ArrowSize = 0;//���˿��Ϊ0��ˮƽ��������          
      sCdlg_Master.mMusic_wnd_time = CreateWindow(SCROLLBAR, L"SCROLLBAR_Time",  WS_OWNERDRAW| WS_VISIBLE, 
                                    80, 370, 640, 35, 
                                    hwnd, eID_SCROLLBAR_TIMER, NULL, NULL);
      SendMessage(sCdlg_Master.mMusic_wnd_time, SBM_SETSCROLLINFO, TRUE, (LPARAM)&g_sif);         

      /*********************����ֵ������******************/
      g_sif_power.cbSize = sizeof(g_sif_power);
      g_sif_power.fMask = SIF_ALL;
      g_sif_power.nMin = 0;
      g_sif_power.nMax = 63;//�������ֵΪ63
      g_sif_power.nValue = sCdlg_Master.power;//��ʼ����ֵ
      g_sif_power.TrackSize = 30;//����ֵ
      g_sif_power.ArrowSize = 0;//���˿��Ϊ0��ˮƽ��������

      CreateWindow(SCROLLBAR, L"SCROLLBAR_R", WS_OWNERDRAW|WS_TRANSPARENT, 
                         70, 440-31/2, 150, 31, 
                         hwnd, eID_SCROLLBAR_POWER, NULL, NULL);
      SendMessage(GetDlgItem(hwnd, eID_SCROLLBAR_POWER), SBM_SETSCROLLINFO, TRUE, (LPARAM)&g_sif_power);


      CreateWindow(BUTTON,L"�����ļ���",WS_OWNERDRAW|WS_TRANSPARENT|WS_VISIBLE,
                  100,0,600,80,hwnd,eID_TB5,NULL,NULL);


      CreateWindow(BUTTON,L"00:00",WS_TRANSPARENT|WS_OWNERDRAW|WS_VISIBLE,
                  720,387-15,80,30,hwnd,eID_TB1,NULL,NULL);


      CreateWindow(BUTTON,L"00:00",WS_TRANSPARENT|WS_OWNERDRAW|WS_VISIBLE,
                   0,387-15,80,30,hwnd,eID_TB2,NULL,NULL);
      break;
    }
    case WM_NOTIFY:
    {
      u16 code,  id, ctr_id;
      NMHDR *nr;
      id  =LOWORD(wParam);//��ȡ��Ϣ��ID��
      code=HIWORD(wParam);//��ȡ��Ϣ������
      ctr_id = LOWORD(wParam); //wParam��16λ�Ƿ��͸���Ϣ�Ŀؼ�ID.   
      if(code == BN_CLICKED)
      { 
        switch(id)
        {
          case eID_BUTTON_Power:
          {
            music_icon[0].state = ~music_icon[0].state;
            //������iconδ������ʱ
            if(music_icon[0].state == FALSE)
            {
              RedrawWindow(hwnd, NULL, RDW_ALLCHILDREN|RDW_INVALIDATE);
              ShowWindow(GetDlgItem(hwnd, eID_SCROLLBAR_POWER), SW_HIDE); //��������
            }
            //������icon������ʱ������Ϊ����ģʽ
            else
            {                
              ShowWindow(GetDlgItem(hwnd, eID_SCROLLBAR_POWER), SW_SHOW); //������ʾ
            }
            break;
          }           
        }

      }//end of if(code == BN_CLICKED)        
      nr = (NMHDR*)lParam; //lParam����������NMHDR�ṹ�忪ͷ.
      if (ctr_id == eID_SCROLLBAR_TIMER)
      {
        NM_SCROLLBAR *sb_nr;
        int i = 0;
        sb_nr = (NM_SCROLLBAR*)nr; //Scrollbar��֪ͨ��Ϣʵ��Ϊ NM_SCROLLBAR��չ�ṹ,���渽���˸������Ϣ.
        switch (nr->code)
        {
          case SBN_THUMBTRACK: //R�����ƶ�
          {
            i = sb_nr->nTrackValue; //��û��鵱ǰλ��ֵ                
            SendMessage(nr->hwndFrom, SBM_SETVALUE, TRUE, i); //���ý���ֵ
            break;
          }
            
        }
      }
      nr = (NMHDR*)lParam; //lParam����������NMHDR�ṹ�忪ͷ.
      //����������case
      if (ctr_id == eID_SCROLLBAR_POWER)
      {
        NM_SCROLLBAR *sb_nr;
        sb_nr = (NM_SCROLLBAR*)nr; //Scrollbar��֪ͨ��Ϣʵ��Ϊ NM_SCROLLBAR��չ�ṹ,���渽���˸������Ϣ.
        static int NoVol_flag = 0;
        switch (nr->code)
        {
        case SBN_THUMBTRACK: //R�����ƶ�
        {
          sCdlg_Master.power= sb_nr->nTrackValue; //�õ���ǰ������ֵ
          if(sCdlg_Master.power == 0) 
          {
            //wm8978_OutMute(1);//����
            SetWindowText(GetDlgItem(hwnd, eID_BUTTON_Power), L"J");
            NoVol_flag = 1;
          }
          else
          {
            if(NoVol_flag == 1)
            {
              NoVol_flag = 0;
              SetWindowText(GetDlgItem(hwnd, eID_BUTTON_Power), L"A");
            }
//            wm8978_OutMute(0);
//            wm8978_SetOUT1Volume(power);//����WM8978������ֵ
          } 
          SendMessage(nr->hwndFrom, SBM_SETVALUE, TRUE, sCdlg_Master.power); //����SBM_SETVALUE����������ֵ
        }
        break;
        }
      }


      
      break;   
    }

    case WM_ERASEBKGND:
    {
      HDC hdc =(HDC)wParam;
      RECT rc =*(RECT*)lParam;
   
      if(sCdlg_Master.LoadPic_State!=FALSE)
        BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, sCdlg_Master.mhdc_bk, rc.x, rc.y, SRCCOPY);         

      return TRUE;
    }   
    case WM_DRAWITEM:
    {
      DRAWITEM_HDR *ds;
      ds = (DRAWITEM_HDR*)lParam;        
      if (ds->ID >= eID_BUTTON_Power && ds->ID<= eID_BUTTON_START)
      {
        MUSICPLAYER_OwnerDraw_BTN(ds);
        return TRUE;
      }
      if (ds->ID == eID_SCROLLBAR_POWER || ds->ID == eID_SCROLLBAR_TIMER)
      {
        MUSICPLAYER_OwnerDraw_ScrollBar(ds);
        return TRUE;
      }
      if(ds->ID == eID_TB1 || ds->ID == eID_TB2 || ds->ID == eID_TB5)
      {
        Music_Button_OwnerDraw(ds);
        return TRUE;
      }      
    }    
    default:
      return DefWindowProc(hwnd, msg, wParam, lParam);
  }
  return WM_NULL;
}


void	GUI_MUSICPLAYER_DIALOG(void)
{
	
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
	sCdlg_Master.mMusicPlayer_hwnd= CreateWindowEx(WS_EX_NOFOCUS|WS_EX_FRAMEBUFFER,
		&wcex,
		L"GUI_MUSICPLAYER_DIALOG",
		WS_VISIBLE,
		0, 0, GUI_XSIZE, GUI_YSIZE,
		NULL, NULL, NULL, NULL);

	//��ʾ������
	ShowWindow(sCdlg_Master.mMusicPlayer_hwnd, SW_SHOW);

	//��ʼ������Ϣѭ��(���ڹرղ�����ʱ,GetMessage������FALSE,�˳�����Ϣѭ��)��
	while (GetMessage(&msg, sCdlg_Master.mMusicPlayer_hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}


