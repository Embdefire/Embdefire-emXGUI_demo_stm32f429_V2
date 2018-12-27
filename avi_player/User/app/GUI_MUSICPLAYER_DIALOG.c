#include "emXGUI.h"
#include "x_libc.h"
#include <string.h>
#include "GUI_MUSICPLAYER_DIALOG.h"
#include "GUI_MusicList_DIALOG.h"
#include "./Bsp/wm8978/bsp_wm8978.h" 
#include "./mjpegplayer/vidoplayer.h"
#include "emXGUI_JPEG.h"

void	GUI_MusicList_DIALOG(void);
int avi_chl = 0;
COLORREF color_bg;//͸���ؼ��ı�����ɫ
extern int Play_index;
extern uint8_t  file_nums;
extern int sw_flag;//�л���־
extern char playlist[FILE_MAX_NUM][FILE_NAME_LEN];//����List
//ͼ���������
icon_S music_icon[13] = {
   {"yinliang",         {576,398,72,72},      FALSE},
   {"yinyueliebiao",    {20, 400,72,72},      FALSE},
   {"back",             {274,404,72,72},      FALSE},
   {"bofang",           {350,406,72,72},      FALSE},
   {"next",             {438,404,72,72},      FALSE},
   {"shangyishou",      {128, 200, 72, 72},   FALSE},
   {"zanting/bofang",   {300, 140, 200, 200}, FALSE},
   {"xiayishou",        {600, 200, 72, 72},   FALSE},    
   {"mini_next",        {580, 4, 72, 72},     FALSE},
   {"mini_Stop",        {652, 4, 72, 72},     FALSE},
   {"mini_back",        {724, 3, 72, 72},     FALSE},  
   {"�ϱ���",           {0 ,0, 800, 80},     FALSE},
   {"�±���",           {0 ,400, 800, 80},     FALSE},   
};
int power;//����ֵ
int showmenu_flag = 0;//��ʾ�˵���
/****************************�ؼ��ػ溯��***********************/
/**
  * @brief  button_owner_draw ��ť�ؼ����ػ���
  * @param  ds:DRAWITEM_HDR�ṹ��
  * @retval NULL
  */
static void button_owner_draw(DRAWITEM_HDR *ds)
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

   SetBrushColor(hdc, color_bg);
   FillRect(hdc, &rc_cli);
   
   SetBrushColor(hdc_mem,MapARGB(hdc_mem, 0, 255, 250, 250));
   FillRect(hdc_mem, &rc_cli);
   //���ð�������ɫ
   SetTextColor(hdc_mem, MapARGB(hdc_mem, 250,250,250,250));
   if((ds->ID == ID_BUTTON_Back || ds->ID == ID_BUTTON_Next)&& ds->State & BST_PUSHED)
      SetTextColor(hdc_mem, MapARGB(hdc_mem, 250,105,105,105));
   if(ds->ID == ID_BUTTON_Back || ds->ID == ID_BUTTON_Next)
   {
      SetFont(hdc_mem, ICON64_FONT);

   }
   else if(ds->ID == ID_BUTTON_Play || ds->ID == ID_BUTTON_Play)
   {
      SetFont(hdc_mem, ICON72_FONT);
   }
   else
   {
      //���ð�ť����
      SetFont(hdc_mem, hFont_SDCARD);
   }
 
   DrawText(hdc_mem, wbuf,-1,&rc_cli,DT_VCENTER);//��������(���ж��뷽ʽ)
   
   BitBlt(hdc, rc_cli.x, rc_cli.y, rc_cli.w, rc_cli.h, hdc_mem, 0, 0, SRCCOPY);
   
   //StretchBlt(hdc, rc_cli.x, rc_cli.y, rc_cli.w, rc_cli.h, hdc_mem, 0, 0, rc_cli.w, rc_cli.h, SRCCOPY);
   
   DeleteDC(hdc_mem);  
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
	SetBrushColor(hdc, color_bg);
	FillRect(hdc, &rc);

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
	draw_scrollbar(hwnd, hdc_mem1, color_bg, RGB888( 250, 250, 250), RGB888( 255, 255, 255));
	//������ɫ���͵Ĺ�����
	draw_scrollbar(hwnd, hdc_mem, color_bg, RGB888( 250, 0, 0), RGB888( 250, 0, 0));
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

/**
  * @brief  ������Ƶ����
  * @param  hwnd����Ļ���ڵľ��
  * @retval ��
  * @notes  
  */
rt_thread_t h_music;//���ֲ��Ž���
static void App_PlayMusic(HWND hwnd)
{
	static int thread=0;
	static int app=0;
   //HDC hdc;
   
	if(thread==0)
	{  
      h_music=rt_thread_create("App_PlayMusic",(void(*)(void*))App_PlayMusic,NULL,15*1024,1,5);
      thread =1;
      rt_thread_startup(h_music);//�����߳�				
      return;
	}
	while(thread) //�߳��Ѵ�����
	{     
		if(app==0)
		{
         //hdc = GetDC(hwnd);
			app=1;
         AVI_play(playlist[Play_index], hwnd);         
			app=0;
        // ReleaseDC(hwnd, hdc);
         GUI_msleep(20);
		}
	}
}
#if 0
rt_thread_t h_decode;//���ֽ������
void App_DecodeMusic(HWND hwnd, const void *dat, int cbSize, JPG_DEC *dec)
{
	static int thread=0;
	static int app=0;
   HDC hdc_mem;
   static U16 pic_width,pic_height;
	if(thread==0)
	{  
      h_decode=rt_thread_create("App_PlayMusic",(void(*)(void*))App_PlayMusic,NULL,5*1024,5,5);
      thread =1;
      rt_thread_startup(h_music);//�����߳�				
      return;
	}
   hdc_mem = GetDC(hwnd);
	while(thread) //�߳��Ѵ�����
	{     
		if(app==0)
		{
         
			app=1;
         dec = JPG_Open(dat, cbSize);
         JPG_GetImageSize(&pic_width, &pic_height,dec);
         //hdc_mem = CreateMemoryDC(SURF_SCREEN,pic_width,pic_height); 
         if(cbSize>10)
           //JPG_Draw(hdc_mem, 400, 0, dec);    
			app=0;
         ReleaseDC(hwnd, hdc_mem);
         GUI_msleep(20);
		}
	}
}
#endif

/**
  * @brief  ���������б����
  * @param  ��
  * @retval ��
  * @notes  
  */
static void App_MusicList()
{
	static int thread=0;
	static int app=0;
   rt_thread_t h1;
	if(thread==0)
	{  
      h1=rt_thread_create("App_MusicList",(void(*)(void*))App_MusicList,NULL,4096,5,5);
      rt_thread_startup(h1);				
      thread =1;
      return;
	}
	if(thread==1) //�߳��Ѵ�����
	{
		if(app==0)
		{
			app=1;
			GUI_MusicList_DIALOG();
			app=0;
			thread=0;
		}
	}
}


static SCROLLINFO sif_time;/*���ý������Ĳ���*/
HWND wnd_time;

static SCROLLINFO sif;/*�����������Ĳ���*/
static HWND wnd;

static HWND wnd_power;//����icon���
static HWND wnd_list;//����icon���

HDC hdc_AVI=NULL;
HWND hwnd_AVI=NULL;

static int t0=0;
static int frame=0;
volatile int win_fps=0;
extern volatile int avi_fps;
extern UINT      BytesRD;
extern uint8_t   Frame_buf[];

static LRESULT win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   
   switch(msg)
   {
      case WM_CREATE:
      {
					t0 =GUI_GetTickCount();
				  frame =0;
					win_fps =0;
				
			    hwnd_AVI =hwnd;
					hdc_AVI =CreateMemoryDC(SURF_SCREEN,480,272);
#if 1 
         //����icon���л�����ģʽ�������ؿؼ����ֵ
         wnd_power = CreateWindow(BUTTON,L"A",WS_OWNERDRAW|WS_VISIBLE,//��ť�ؼ�������Ϊ�Ի��ƺͿ���
                                  music_icon[0].rc.x,music_icon[0].rc.y,//λ������Ϳؼ���С
                                  music_icon[0].rc.w,music_icon[0].rc.h,//��music_icon[0]����
                                  hwnd,ID_BUTTON_Power,NULL,NULL);//������hwnd,IDΪID_BUTTON_Power�����Ӳ���Ϊ�� NULL
         //�����б�icon
         wnd_list = CreateWindow(BUTTON,L"D",WS_OWNERDRAW|WS_VISIBLE, //��ť�ؼ�������Ϊ�Ի��ƺͿ���
                      music_icon[1].rc.x,music_icon[1].rc.y,//λ������
                      music_icon[1].rc.w,music_icon[1].rc.h,//�ؼ���С
                      hwnd,ID_BUTTON_List,NULL,NULL);//������hwnd,IDΪID_BUTTON_List�����Ӳ���Ϊ�� NULL

         //��һ��icon
         CreateWindow(BUTTON,L"S",WS_OWNERDRAW|WS_VISIBLE, //��ť�ؼ�������Ϊ�Ի��ƺͿ���
                      music_icon[2].rc.x,music_icon[2].rc.y,//λ������
                      music_icon[2].rc.w,music_icon[2].rc.h,//�ؼ���С
                      hwnd,ID_BUTTON_Back,NULL,NULL);//������hwnd,IDΪID_BUTTON_List�����Ӳ���Ϊ�� NULL
         //����icon
         CreateWindow(BUTTON,L"U",WS_OWNERDRAW|WS_VISIBLE, //��ť�ؼ�������Ϊ�Ի��ƺͿ���
                      music_icon[3].rc.x,music_icon[3].rc.y,//λ������
                      music_icon[3].rc.w,music_icon[3].rc.h,//�ؼ���С
                      hwnd,ID_BUTTON_Play,NULL,NULL);//������hwnd,IDΪID_BUTTON_List�����Ӳ���Ϊ�� NULL

         //����icon
         CreateWindow(BUTTON,L"V",WS_OWNERDRAW|WS_VISIBLE, //��ť�ؼ�������Ϊ�Ի��ƺͿ���
                      music_icon[4].rc.x,music_icon[4].rc.y,//λ������
                      music_icon[4].rc.w,music_icon[4].rc.h,//�ؼ���С
                      hwnd,ID_BUTTON_Next,NULL,NULL);//������hwnd,IDΪID_BUTTON_List�����Ӳ���Ϊ�� NULL
                                           
         /*********************����������******************/
         sif_time.cbSize = sizeof(sif_time);
         sif_time.fMask = SIF_ALL;
         sif_time.nMin = 0;
         sif_time.nMax = 255;
         sif_time.nValue = 0;//��ʼֵ
         sif_time.TrackSize = 30;//����ֵ
         sif_time.ArrowSize = 0;//���˿��Ϊ0��ˮƽ��������          
         wnd_time = CreateWindow(SCROLLBAR, L"SCROLLBAR_Time",  WS_OWNERDRAW|WS_VISIBLE, 
                         120, 370, 560, 30, hwnd, ID_SCROLLBAR_TIMER, NULL, NULL);
         SendMessage(wnd_time, SBM_SETSCROLLINFO, TRUE, (LPARAM)&sif_time);
         /*********************����ֵ������******************/
         sif.cbSize = sizeof(sif);
         sif.fMask = SIF_ALL;
         sif.nMin = 0;
         sif.nMax = 63;//�������ֵΪ63
         sif.nValue = 20;//��ʼ����ֵ
         sif.TrackSize = 31;//����ֵ
         sif.ArrowSize = 0;//���˿��Ϊ0��ˮƽ��������
         wnd = CreateWindow(SCROLLBAR, L"SCROLLBAR_R", WS_OWNERDRAW|WS_TRANSPARENT|WS_VISIBLE, 
                            635, 422, 150, 30, hwnd, ID_SCROLLBAR_POWER, NULL, NULL);
         SendMessage(wnd, SBM_SETSCROLLINFO, TRUE, (LPARAM)&sif);         
 #endif   
			 App_PlayMusic(hwnd);
         break;
      }

      case WM_DRAWITEM:
      {
         
         
         DRAWITEM_HDR *ds;
         ds = (DRAWITEM_HDR*)lParam;
         if (ds->ID == ID_SCROLLBAR_POWER || ds->ID == ID_SCROLLBAR_TIMER)
         {
            scrollbar_owner_draw(ds);
            return TRUE;
         }
         if (ds->ID >= 0x1000 && ds->ID<= 0x1099)
         {
            button_owner_draw(ds);
            return TRUE;
         }
      }  
			
      //���ƴ��ڽ�����Ϣ
      case WM_PAINT:
      {
         PAINTSTRUCT ps;
         HDC hdc;//��Ļhdc
         WCHAR buff[128];

//				WCHAR wbuf[40];
         RECT rc;
         int t1;
         GetClientRect(hwnd, &rc);
         hdc = BeginPaint(hwnd, &ps);   
         SetBrushColor(hdc, MapRGB(hdc, 0,0,0));
         FillRect(hdc, &rc);
         frame++;
         t1 =GUI_GetTickCount();
         if((t1-t0)>=1000)
         {
            win_fps =frame;
            t0 =t1;
            frame =0;
         }
                
         
         
         color_bg = GetPixel(hdc, 385, 404);
         EndPaint(hwnd, &ps);
         break;
      }
      case WM_NOTIFY:
      {
         u16 code, id, ctr_id;
         id  =LOWORD(wParam);//��ȡ��Ϣ��ID��
         code=HIWORD(wParam);//��ȡ��Ϣ������
         //���͵���
         if(code == BN_CLICKED)
         { 
            switch(id)
            {
               //����icon����case
               case ID_BUTTON_Power:
               {
                  RECT rc_cli = {80, 431, 150, 30};
                  music_icon[0].state = ~music_icon[0].state;
                  //InvalidateRect(hwnd, &music_icon[0].rc, TRUE);
                  //������iconδ������ʱ
                  if(music_icon[0].state == FALSE)
                  {
                     wm8978_OutMute(0);
                     //���½�������ֵ
                     sif.nValue = power;
                     SendMessage(wnd, SBM_SETSCROLLINFO, TRUE, (LPARAM)&sif);     
                     EnableWindow(wnd, ENABLE);//��������������
                     SetWindowText(wnd_power, L"A");
                  }
                  //������icon������ʱ������Ϊ����ģʽ
                  else
                  {                
                     wm8978_OutMute(1);//����
                     power = SendMessage(wnd, SBM_GETVALUE, TRUE, TRUE);//��ȡ��ǰ����ֵ
                     sif.nValue = 0;//��������Ϊ0
                     SendMessage(wnd, SBM_SETSCROLLINFO, TRUE, (LPARAM)&sif);
                     EnableWindow(wnd, DISABLE); //��������������               
                     SetWindowText(wnd_power, L"J");
                  }
                  InvalidateRect(hwnd, &rc_cli, TRUE);
                  break;
               }              
            
               case ID_BUTTON_List:
               {
                  App_MusicList();
                  break;
               }
               case ID_BUTTON_Play:
               {

                  music_icon[3].state = ~music_icon[3].state;
                  //InvalidateRect(hwnd, &music_icon[0].rc, TRUE);
                  //������iconδ������ʱ
                  if(music_icon[3].state == FALSE)
                  {
                     I2S_Play_Start();
                     TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //����ʱ��3�����ж�
                     TIM_Cmd(TIM3,ENABLE); //ʹ�ܶ�ʱ��3                        
                     
                     SetWindowText(GetDlgItem(hwnd, ID_BUTTON_Play), L"U");
                              
                  }
                  //������icon������ʱ����ͣ
                  else
                  {          
                     I2S_Play_Stop();
                     TIM_ITConfig(TIM3,TIM_IT_Update,DISABLE); //����ʱ��3�����ж�
                     TIM_Cmd(TIM3,DISABLE); //ʹ�ܶ�ʱ��3                     
                     SetWindowText(GetDlgItem(hwnd, ID_BUTTON_Play), L"T");
                  }
                  //
                  break;
               }
               case ID_BUTTON_Back:
               {
                  RECT rc0 = {0, 370,120,30};//��ǰʱ��
                  RECT rc1 = {680,370,120,30};//��ʱ��
                  RECT rc2 = {0,0,800,40};//��������
                  RECT rc3 = {0,40,380,40};//�ֱ���
                  RECT rc4 = {440,40,360,40};//��������                  
                  WCHAR wbuf[128];
                  HDC hdc;
                  Play_index--;
                  if(Play_index < 0)
                     Play_index = file_nums - 1;  
                  sw_flag = 1;   
                  hdc = GetDC(hwnd);     
                  char *ss;
                  int length1=strlen(playlist[Play_index]);
                  int length2=strlen("0:/srcdata/");
                  if(strncpy(playlist[Play_index],"0:/srcdata/",length2))//�Ƚ�ǰn���ַ���������strcpy
                  {
                    ss = playlist[Play_index] + length2;
                  }
                  SetTextColor(hdc, MapRGB(hdc,255,255,255));
                  ClrDisplay(hdc, &rc2, MapRGB(hdc, 0,0,0));
                  x_mbstowcs_cp936(wbuf, ss, 200);
                  DrawText(hdc, wbuf,-1,&rc2,DT_VCENTER|DT_CENTER); 
                  
                  
                  x_wsprintf(wbuf, L"֡�ʣ�0FPS/s");
                  ClrDisplay(hdc, &rc4, MapRGB(hdc, 0,0,0));
                  DrawText(hdc, wbuf,-1,&rc4,DT_VCENTER|DT_LEFT);            
                  ClrDisplay(hdc, &rc3, MapRGB(hdc, 0,0,0));
                  x_wsprintf(wbuf, L"�ֱ��ʣ� 0*0");
                  DrawText(hdc, wbuf,-1,&rc3,DT_VCENTER|DT_RIGHT); 
                  ClrDisplay(hdc, &rc1, MapRGB(hdc, 0,0,0)); 
                  ClrDisplay(hdc, &rc0, MapRGB(hdc, 0,0,0));                  
                  DrawText(hdc, L"00:00:00",-1,&rc0,DT_VCENTER|DT_CENTER);
                  DrawText(hdc, L"00:00:00",-1,&rc1,DT_VCENTER|DT_CENTER);
                  
                  sif_time.nValue = 0;//����Ϊ0
                  SendMessage(wnd_time, SBM_SETSCROLLINFO, TRUE, (LPARAM)&sif_time);                  
                  

                  break;
               }
               case ID_BUTTON_Next:
               {
                  RECT rc0 = {0, 370,120,30};//��ǰʱ��
                  RECT rc1 = {680,370,120,30};//��ʱ��
                  RECT rc2 = {0,0,800,40};//��������
                  RECT rc3 = {0,40,380,40};//�ֱ���
                  RECT rc4 = {440,40,360,40};//��������                     
                  Play_index++;
                  HDC hdc;
                  WCHAR wbuf[128];
                  
                  if(Play_index > file_nums -1 )
                     Play_index = 0;
                  sw_flag = 1;
                  hdc = GetDC(hwnd);     
                  char *ss;
                  int length1=strlen(playlist[Play_index]);
                  int length2=strlen("0:/srcdata/");
                  if(strncpy(playlist[Play_index],"0:/srcdata/",length2))//�Ƚ�ǰn���ַ���������strcpy
                  {
                    ss = playlist[Play_index] + length2;
                  }
                  SetTextColor(hdc, MapRGB(hdc,255,255,255));
                  ClrDisplay(hdc, &rc2, MapRGB(hdc, 0,0,0));
                  x_mbstowcs_cp936(wbuf, ss, 200);
                  DrawText(hdc, wbuf,-1,&rc2,DT_VCENTER|DT_CENTER);                   
                  x_wsprintf(wbuf, L"֡�ʣ�0FPS/s");
                  ClrDisplay(hdc, &rc4, MapRGB(hdc, 0,0,0));
                  DrawText(hdc, wbuf,-1,&rc4,DT_VCENTER|DT_LEFT);            
                  ClrDisplay(hdc, &rc3, MapRGB(hdc, 0,0,0));
                  x_wsprintf(wbuf, L"�ֱ��ʣ� 0*0");
                  DrawText(hdc, wbuf,-1,&rc3,DT_VCENTER|DT_RIGHT); 
                  ClrDisplay(hdc, &rc1, MapRGB(hdc, 0,0,0)); 
                  ClrDisplay(hdc, &rc0, MapRGB(hdc, 0,0,0));                  
                  DrawText(hdc, L"00:00:00",-1,&rc0,DT_VCENTER|DT_CENTER);
                  DrawText(hdc, L"00:00:00",-1,&rc1,DT_VCENTER|DT_CENTER);


                  sif_time.nValue = 0;//����Ϊ0
                  SendMessage(wnd_time, SBM_SETSCROLLINFO, TRUE, (LPARAM)&sif_time);                    
                  break;
               }
            }
         }
         NMHDR *nr;  
         ctr_id = LOWORD(wParam); //wParam��16λ�Ƿ��͸���Ϣ�Ŀؼ�ID. 
         nr = (NMHDR*)lParam; //lParam����������NMHDR�ṹ�忪ͷ.
         //����������case
         if (ctr_id == ID_SCROLLBAR_TIMER)
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
                  //��λ���������λ��
                  avi_chl = 1;//������־
               }
               break;
            }
         }
         //����������case
         if (ctr_id == ID_SCROLLBAR_POWER)
         {
            NM_SCROLLBAR *sb_nr;
            sb_nr = (NM_SCROLLBAR*)nr; //Scrollbar��֪ͨ��Ϣʵ��Ϊ NM_SCROLLBAR��չ�ṹ,���渽���˸������Ϣ.
            switch (nr->code)
            {
               case SBN_THUMBTRACK: //R�����ƶ�
               {
                  power= sb_nr->nTrackValue; //�õ���ǰ������ֵ
                  //����WM8978������ֵ
                  wm8978_SetOUT1Volume(power); 
                  SendMessage(nr->hwndFrom, SBM_SETVALUE, TRUE, power); //����SBM_SETVALUE����������ֵ
               }
               break;
            }
         }   
         break;
      }       
      case WM_CLOSE:
      {
         showmenu_flag = 0;
				DeleteDC(hdc_AVI);
         DestroyWindow(hwnd); //����DestroyWindow���������ٴ��ڣ��ú��������WM_DESTROY��Ϣ����
         return TRUE; //�رմ��ڷ���TRUE��
      }
      default :
         return DefWindowProc(hwnd, msg, wParam, lParam);
   }  
   return WM_NULL;
}



//���ֲ��������
HWND	VideoPlayer_hwnd;
void	GUI_VideoPlayer_DIALOG(void)
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
	VideoPlayer_hwnd = CreateWindowEx(WS_EX_NOFOCUS,
                                    &wcex,
                                    L"GUI_MUSICPLAYER_DIALOG",
                                    WS_VISIBLE,
                                    0, 0, GUI_XSIZE, GUI_YSIZE,
                                    NULL, NULL, NULL, NULL);

	//��ʾ������
	ShowWindow(VideoPlayer_hwnd, SW_SHOW);

	//��ʼ������Ϣѭ��(���ڹرղ�����ʱ,GetMessage������FALSE,�˳�����Ϣѭ��)��
	while (GetMessage(&msg, VideoPlayer_hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}


