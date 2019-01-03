#include "emXGUI.h"
#include "GUI_MUSICPLAYER_DIALOG.h"
#include "x_libc.h"
#include "string.h"
#include "ff.h"
#include "./wm8978/mp3Player.h"
/******************��ť�ؼ�IDֵ***********************/
#define ID_BUTTON_Power      0x1000   //���� 
#define ID_BUTTON_List       0x1001   //����List
#define ID_BUTTON_Equa       0x1002   //������
#define ID_BUTTON_Folder     0x1003   //�ļ���
#define ID_BUTTON_BACK       0x1004   //��һ��
#define ID_BUTTON_START      0x1005   //��ͣ��
#define ID_BUTTON_NEXT       0x1006   //��һ��
#define ID_BUTTON_MINISTOP   0x1007   //�������ͣ��
/*****************�������ؼ�IDֵ*********************/
#define ID_SCROLLBAR_POWER   0x1104   //������
#define ID_SCROLLBAR_TIMER   0x1105   //������
/*****************�ı���ؼ�IDֵ*********************/
//��������ʾ���и��
#define ID_TEXTBOX_LRC1      0x1201   //��ʵ�һ��
#define ID_TEXTBOX_LRC2      0x1202   //��ʵڶ���
#define ID_TEXTBOX_LRC3      0x1203   //��ʵ����У���ǰ�У�
#define ID_TEXTBOX_LRC4      0x1204   //��ʵ�����
#define ID_TEXTBOX_LRC5      0x1205   //��ʵ�����




//ͼ���������
icon_S music_icon[12] = {
   {"yinliang",         {10,402,72,72},       FALSE},//����
   {"yinyueliebiao",    {668,404,72,72},      FALSE},//�����б�
   {"geci",             {728,404,72,72},      FALSE},//�����
   {"NULL",             {0,0,0,0},            FALSE},//��
   {"NULL",             {0,0,0,0},            FALSE},//��
   {"shangyishou",      {294, 404, 72, 72},   FALSE},//��һ��
   {"zanting/bofang",   {364, 406, 72, 72},   FALSE},//����
   {"xiayishou",        {448, 404, 72, 72},   FALSE},//��һ��
  
};
char path[100]="0:";//�ļ���Ŀ¼
int power;//����ֵ
s32 old_scrollbar_value;//��һ������ֵ
rt_thread_t h_music;//���ֲ��Ž���
int enter_flag = 0;//�л���־λ
COLORREF color_bg;//͸���ؼ��ı�����ɫ
uint8_t chgsch=0; //������������־λ
char music_name[FILE_NAME_LEN]={0};//����������
//�ļ�ϵͳ��ر���
FRESULT f_result; 
FIL     f_file;
UINT    f_num;
//�������--��Ÿ������
uint8_t ReadBuffer1[1024*5]={0};
//MINI���ż�����һ�ס���һ�׿ؼ�������
static HWND mini_next,mini_start,mini_back;
//�����ʾ��־λ
static int show_lrc = 0;
//��ʽṹ��
LYRIC lrc;

extern const unsigned char gImage_0[];

#define ROTATE_DISK_NAME "rotate_disk_ARGB8888.bmp"
/*============================================================================*/
static BITMAP bm_0;
static HDC rotate_disk_hdc;

static SURFACE *pSurf;
static HDC hdc_mem11=NULL;


/***********************�ⲿ����*************************/
extern void	GUI_MusicList_DIALOG(void);

/******************��ȡ����ļ�*************************/

static uint16_t getonelinelrc(uint8_t *buff,uint8_t *str,int16_t len)
{
	uint16_t i;
	for(i=0;i<len;i++)
	{
		*(str+i)=*(buff+i);
		if((*(buff+i)==0x0A)||(*(buff+i)==0x00))
		{
			*(buff+i)='\0';
			*(str+i)='\0';
			break;
		}
	}
	return (i+1);
}
/**
  * @brief  �����ַ���
  * @param  name��  ��������
  * @param  sfx��   ������������ַ���
  * @retval ��
  * @notes  ��������øú���Ϊ����ļ�����.lrc��׺
  */
static void lrc_chg_suffix(uint8_t *name,const char *sfx)
{		    	     
	while(*name!='\0')name++;
	while(*name!='.')name--;
	*(++name)=sfx[0];
	*(++name)=sfx[1];
	*(++name)=sfx[2];
	*(++name)='\0';
}
/**
  * @brief  ����ļ�����
  * @param  lyric��  ��ʽṹ��
  * @retval ��
  * @notes  ��
  */
static void lrc_sequence(LYRIC	*lyric)
{
	uint16_t i=0,j=0;
	uint16_t temp=0;
	if (lyric->indexsize == 0)return;
	
	for(i = 0; i < lyric->indexsize - 1; i++)
	{
		for(j = i+1; j < lyric->indexsize; j++)
		{
			if(lyric->time_tbl[i] > lyric->time_tbl[j])
			{
				temp = lyric->time_tbl[i];
				lyric->time_tbl[i] = lyric->time_tbl[j];
				lyric->time_tbl[j] = temp;

				temp = lyric->addr_tbl[i];
				lyric->addr_tbl[i] = lyric->addr_tbl[j];
				lyric->addr_tbl[j] = temp;
			}
		}
	}	
}
/**
  * @brief  ����ļ�����
  * @param  lyric��  ��ʽṹ��
  * @param  strbuf�� ��Ÿ�ʵ�����
  * @retval ��
  * @notes  
  */
static void lyric_analyze(LYRIC	*lyric,uint8_t *strbuf)
{
	uint8_t strtemp[MAX_LINE_LEN]={0};
	uint8_t *pos=NULL;
	uint8_t sta=0,strtemplen=0;
	uint16_t lrcoffset=0;
	uint16_t str_len=0,i=0;
	
	pos=strbuf;
	str_len=strlen((const char *)strbuf);
	if(str_len==0)return;
	i=str_len;
   //�˴���whileѭ�������жϸ���ļ��ı�׼
	while(--i)
	{
		if(*pos=='[')
			sta=1;
		else if((*pos==']')&&(sta==1))
			sta=2;
	  else if((sta==2)&&(*pos!=' '))
		{
			sta=3;
			break;
		}
		pos++; 
	}
	if(sta!=3)return;	
	lrcoffset=0;
	lyric->indexsize=0;
	while(lrcoffset<=str_len)
	{
		i=getonelinelrc(strbuf+lrcoffset,strtemp,MAX_LINE_LEN);
		lrcoffset+=i;
//		printf("lrcoffset:%d,i:%d\n",lrcoffset,i);
		strtemplen=strlen((const char *)strtemp);
		pos=strtemp;
		while(*pos!='[')
			pos++;
		pos++;
      
		if((*pos<='9')&&(*pos>='0'))
		{
         //��¼ʱ���ǩ
			lyric->time_tbl[lyric->indexsize]=(((*pos-'0')*10+(*(pos + 1)-'0'))*60+((*(pos+3)-'0')*10+(*(pos+4)-'0')))*100+((*(pos+6)-'0')*10+(*(pos+7)-'0'));
			//��¼�������
         lyric->addr_tbl[lyric->indexsize]=(uint16_t)(lrcoffset-strtemplen+10); 
         //��¼��ʳ���
			lyric->length_tbl[lyric->indexsize]=strtemplen-10;
			lyric->indexsize++;
		}		
//		else
//				continue;		
	}
}

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
      h1=rt_thread_create("App_MusicList",(void(*)(void*))App_MusicList,NULL,4*1024,5,1);
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
/**
  * @brief  ���������б����
  * @param  hwnd����Ļ���ڵľ��
  * @retval ��
  * @notes  
  */
static void App_PlayMusic(HWND hwnd)
{
	static int thread=0;
	static int app=0;
   HDC hdc;
   
	if(thread==0)
	{  
      h_music=rt_thread_create("App_PlayMusic",(void(*)(void*))App_PlayMusic,NULL,5*1024,5,1);
      thread =1;
      rt_thread_startup(h_music);//�����߳�				
//      rt_thread_suspend(h_music);//��ʱ����
//      rt_schedule();//�����������
      return;
	}
	while(thread) //�߳��Ѵ�����
	{     
		if(app==0)
		{
			app=1;
         hdc = GetDC(hwnd);   
         int i = 0;      
         //��ȡ����ļ�
         while(playlist[play_index][i]!='\0')
         {
           music_name[i]=playlist[play_index][i];
           i++;
         }			         
         music_name[i]='\0';
         //Ϊ����ļ����.lrc��׺
         lrc_chg_suffix((uint8_t *)music_name,"lrc");
         i=0;
         //��ʼ����������
         while(i<LYRIC_MAX_SIZE)
         {
           lrc.addr_tbl[i]=0;
           lrc.length_tbl[i]=0;
           lrc.time_tbl[i]=0;
           i++;
         }
         lrc.indexsize=0;
         lrc.oldtime=0;
         lrc.curtime=0;
         //�򿪸���ļ�
         f_result=f_open(&f_file, music_name,FA_OPEN_EXISTING | FA_READ);
         //�򿪳ɹ�����ȡ����ļ�����������ļ���ͬʱ��flag��1����ʾ�ļ���ȡ�ɹ�
         if((f_result==FR_OK)&&(f_file.fsize<COMDATA_SIZE))
         {					
           f_result=f_read(&f_file,ReadBuffer1, sizeof(ReadBuffer1),&f_num);		
           if(f_result==FR_OK) 
           {  
              lyric_analyze(&lrc,ReadBuffer1);
              lrc_sequence(&lrc);
              lrc.flag = 1;      
           }
         }
         //��ʧ�ܣ�δ�ҵ��ø���ļ�������flag���㣬��ʾû�ж�ȡ���ø���ļ�
         else
         {
            lrc.flag = 0;
            printf("��ȡʧ��\n");
         }
         //�ر��ļ�
			f_close(&f_file);	 
         
         i = 0;
         //�õ�������Ŀ���ļ���
         while(playlist[play_index][i]!='\0')
			{
				music_name[i]=playlist[play_index][i];
				i++;
			}
			music_name[i]='\0';
         

         
         if(strstr(music_name,".wav")||strstr(music_name,".WAV"))
         {
            printf("wav\r");
           wavplayer(music_name, power,hdc);
         }
         else
         {
           mp3PlayerDemo(music_name, power, hdc);  
         }
			 
         printf("���Ž���\n");
         
			app=0;
         //ʹ�� GETDC֮����Ҫ�ͷŵ�HDC
         ReleaseDC(hwnd, hdc);
         //�����������
         GUI_msleep(20);
		}
	}
}
/**
  * @brief  scan_files �ݹ�ɨ��sd���ڵĸ����ļ�
  * @param  path:��ʼɨ��·��
  * @retval result:�ļ�ϵͳ�ķ���ֵ
  */
static FRESULT scan_files (char* path) 
{ 
  FRESULT res; 		//�����ڵݹ���̱��޸ĵı���������ȫ�ֱ���	
  FILINFO fno; 
  DIR dir; 
  int i; 
  char *fn; 
  char file_name[FILE_NAME_LEN];	
	
#if _USE_LFN 
  static char lfn[_MAX_LFN * (_DF1S ? 2 : 1) + 1]; 	//���ļ���֧��
  fno.lfname = lfn; 
  fno.lfsize = sizeof(lfn); 
#endif  
  res = f_opendir(&dir, path); //��Ŀ¼
  if (res == FR_OK) 
  { 
    i = strlen(path); 
    for (;;) 
    { 
      res = f_readdir(&dir, &fno); 										//��ȡĿ¼�µ�����
     if (res != FR_OK || fno.fname[0] == 0) break; 	//Ϊ��ʱ��ʾ������Ŀ��ȡ��ϣ�����
#if _USE_LFN 
      fn = *fno.lfname ? fno.lfname : fno.fname; 
#else 
      fn = fno.fname; 
#endif 
      if(strstr(path,"recorder")!=NULL)continue;       //�ӹ�¼���ļ�
      if (*fn == '.') continue; 											//���ʾ��ǰĿ¼������			
      if (fno.fattrib & AM_DIR) 
			{ 																							//Ŀ¼���ݹ��ȡ
        sprintf(&path[i], "/%s", fn); 							//�ϳ�����Ŀ¼��
        res = scan_files(path);											//�ݹ���� 
        if (res != FR_OK) 
					break; 																		//��ʧ�ܣ�����ѭ��
        path[i] = 0; 
      } 
      else 
		{ 
				//printf("%s%s\r\n", path, fn);								//����ļ���
				if(strstr(fn,".wav")||strstr(fn,".WAV")||strstr(fn,".mp3")||strstr(fn,".MP3"))//�ж��Ƿ�mp3��wav�ļ�
				{
					if ((strlen(path)+strlen(fn)<FILE_NAME_LEN)&&(file_num<MUSIC_MAX_NUM))
					{
						sprintf(file_name, "%s/%s", path, fn);						
						memcpy(playlist[file_num],file_name,strlen(file_name));
                  //printf("%s\r\n", playlist[file_num]);
						memcpy(lcdlist[file_num],fn,strlen(fn));						
						file_num++;//��¼�ļ�����
					}
				}//if mp3||wav
      }//else
     } //for
  } 
  return res; 
}
/***********************�ؼ��ػ溯��********************************/
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
//   if(ds->ID ==  ID_BUTTON_START && show_lrc == 1)
//      return;
   //��ȡ�ؼ���λ�ô�С��Ϣ
   GetClientRect(hwnd, &rc_cli);
   //��������㣬��ʽΪSURF_ARGB4444
   hdc_mem = CreateMemoryDC(SURF_ARGB4444, rc_cli.w, rc_cli.h);
   
	GetWindowText(ds->hwnd,wbuf,128); //��ð�ť�ؼ�������  
   if(ds->ID == ID_BUTTON_Power || ds->ID == ID_BUTTON_MINISTOP){
      SetBrushColor(hdc, color_bg);
      FillRect(hdc, &rc_cli);
   }
   
   SetBrushColor(hdc_mem,MapARGB(hdc_mem, 0, 255, 250, 250));
   FillRect(hdc_mem, &rc_cli);
   //���ż�ʹ��100*100������
   if(ds->ID == ID_BUTTON_START)
      SetFont(hdc_mem, ICON72_FONT);
   else if(ds->ID == ID_BUTTON_NEXT || ds->ID == ID_BUTTON_BACK)
      SetFont(hdc_mem, ICON64_FONT);
   else
      SetFont(hdc_mem, hFont_SDCARD);
   //���ð�������ɫ
   SetTextColor(hdc_mem, MapARGB(hdc_mem, 250,250,250,250));
   //NEXT����BACK����LIST������ʱ���ı���ɫ
	if((ds->State & BST_PUSHED) && (ds->ID == ID_BUTTON_NEXT || ds->ID == ID_BUTTON_BACK || ds->ID == ID_BUTTON_List))
	{ //��ť�ǰ���״̬
		SetTextColor(hdc_mem, MapARGB(hdc_mem, 250,70,130,180));      //��������ɫ     
	}
 
   DrawText(hdc_mem, wbuf,-1,&rc_cli,DT_VCENTER);//��������(���ж��뷽ʽ)
   
   BitBlt(hdc, rc_cli.x, rc_cli.y, rc_cli.w, rc_cli.h, hdc_mem, 0, 0, SRCCOPY);
   
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
	draw_scrollbar(hwnd, hdc_mem, color_bg, RGB888(	50, 205, 50), RGB888(50, 205, 50));
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


HWND wnd_time;//�������������ھ��
SCROLLINFO sif;/*���û������Ĳ���*/
HWND wnd_lrc1;//��ʴ��ھ��
HWND wnd_lrc2;//��ʴ��ھ��
HWND wnd_lrc3;//��ʴ��ھ��
HWND wnd_lrc4;//��ʴ��ھ��
HWND wnd_lrc5;//��ʴ��ھ��
HWND sub11_wnd; //���ż����
static LRESULT win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){

   static HWND wnd;//�������������ھ�� 
   static HWND wnd_power;//����icon���
   RECT rc;

   switch(msg){
      case WM_CREATE:
      {
         
         //����icon���л�����ģʽ�������ؿؼ����ֵ
         wnd_power = CreateWindow(BUTTON,L"A",WS_OWNERDRAW |WS_VISIBLE,//��ť�ؼ�������Ϊ�Ի��ƺͿ���
                                  music_icon[0].rc.x,music_icon[0].rc.y,//λ������Ϳؼ���С
                                  music_icon[0].rc.w,music_icon[0].rc.h,//��music_icon[0]����
                                  hwnd,ID_BUTTON_Power,NULL,NULL);//������hwnd,IDΪID_BUTTON_Power�����Ӳ���Ϊ�� NULL
         //�����б�icon
         CreateWindow(BUTTON,L"G",WS_OWNERDRAW |WS_VISIBLE, //��ť�ؼ�������Ϊ�Ի��ƺͿ���
                      music_icon[1].rc.x,music_icon[1].rc.y,//λ������
                      music_icon[1].rc.w,music_icon[1].rc.h,//�ؼ���С
                      hwnd,ID_BUTTON_List,NULL,NULL);//������hwnd,IDΪID_BUTTON_List�����Ӳ���Ϊ�� NULL
         //���icon
         CreateWindow(BUTTON,L"W",WS_OWNERDRAW |WS_VISIBLE,
                      music_icon[2].rc.x,music_icon[2].rc.y,
                      music_icon[2].rc.w,music_icon[2].rc.h,
                      hwnd,ID_BUTTON_Equa,NULL,NULL);

         //��һ��
         CreateWindow(BUTTON,L"S",WS_OWNERDRAW |WS_VISIBLE,
                      music_icon[5].rc.x,music_icon[5].rc.y,
                      music_icon[5].rc.w,music_icon[5].rc.h,
                      hwnd,ID_BUTTON_BACK,NULL,NULL);
         //��һ��
         CreateWindow(BUTTON,L"V",WS_OWNERDRAW |WS_VISIBLE,
                      music_icon[7].rc.x,music_icon[7].rc.y,
                      music_icon[7].rc.w,music_icon[7].rc.h,
                      hwnd,ID_BUTTON_NEXT,NULL,NULL);
         //���ż�
         sub11_wnd = CreateWindow(BUTTON,L"U",WS_OWNERDRAW |WS_VISIBLE,
                      music_icon[6].rc.x,music_icon[6].rc.y,
                      music_icon[6].rc.w,music_icon[6].rc.h,
                      hwnd,ID_BUTTON_START,NULL,NULL);                      
         /*********************����������******************/
         sif.cbSize = sizeof(sif);
         sif.fMask = SIF_ALL;
         sif.nMin = 0;
         sif.nMax = 255;
         sif.nValue = 0;//��ʼֵ
         sif.TrackSize = 30;//����ֵ
         sif.ArrowSize = 0;//���˿��Ϊ0��ˮƽ��������          
         wnd_time = CreateWindow(SCROLLBAR, L"SCROLLBAR_Time",  WS_OWNERDRAW| WS_VISIBLE, 
                         80, 370, 640, 35, hwnd, ID_SCROLLBAR_TIMER, NULL, NULL);
         SendMessage(wnd_time, SBM_SETSCROLLINFO, TRUE, (LPARAM)&sif);         

         /*********************����ֵ������******************/
         sif.cbSize = sizeof(sif);
         sif.fMask = SIF_ALL;
         sif.nMin = 0;
         sif.nMax = 63;//�������ֵΪ63
         sif.nValue = 20;//��ʼ����ֵ
         sif.TrackSize = 30;//����ֵ
         sif.ArrowSize = 0;//���˿��Ϊ0��ˮƽ��������
         wnd = CreateWindow(SCROLLBAR, L"SCROLLBAR_R", WS_OWNERDRAW|WS_TRANSPARENT | WS_VISIBLE, 
                            60, 425, 150, 31, hwnd, ID_SCROLLBAR_POWER, NULL, NULL);
         SendMessage(wnd, SBM_SETSCROLLINFO, TRUE, (LPARAM)&sif);
         
         //���¿ؼ�ΪTEXTBOX�Ĵ���
         wnd_lrc1 = CreateWindow(TEXTBOX, L" ", NULL, 
                                0, 80, 800, 60, hwnd, ID_TEXTBOX_LRC1, NULL, NULL);  
         SendMessage(wnd_lrc1,TBM_SET_TEXTFLAG,0,DT_VCENTER|DT_CENTER|DT_BKGND);                                
         wnd_lrc2 = CreateWindow(TEXTBOX, L" ", NULL, 
                                0, 140, 800, 60, hwnd, ID_TEXTBOX_LRC2, NULL, NULL); 
         SendMessage(wnd_lrc2,TBM_SET_TEXTFLAG,0,DT_VCENTER|DT_CENTER|DT_BKGND);
         wnd_lrc3 = CreateWindow(TEXTBOX, L" ", NULL, 
                                0, 200, 800, 60, hwnd, ID_TEXTBOX_LRC3, NULL, NULL);  
         SendMessage(wnd_lrc3,TBM_SET_TEXTFLAG,0,DT_VCENTER|DT_CENTER|DT_BKGND);     
         wnd_lrc4 = CreateWindow(TEXTBOX, L" ", NULL, 
                                0, 260, 800, 60, hwnd, ID_TEXTBOX_LRC4, NULL, NULL);  
         SendMessage(wnd_lrc4,TBM_SET_TEXTFLAG,0,DT_VCENTER|DT_CENTER|DT_BKGND); 
         wnd_lrc5 = CreateWindow(TEXTBOX, L" ", NULL, 
                                0, 320, 800, 50, hwnd, ID_TEXTBOX_LRC5, NULL, NULL);  
         SendMessage(wnd_lrc5,TBM_SET_TEXTFLAG,0,DT_VCENTER|DT_CENTER|DT_BKGND);         
         //��ȡ�����б�
         scan_files(path);
         //�������ֲ����߳�
         App_PlayMusic(hwnd);
         
			CreateWindow(TEXTBOX,L"�����ļ���",WS_VISIBLE,
                      0,0,800,80,hwnd,ID_TB5,NULL,NULL);
         SendMessage(GetDlgItem(hwnd, ID_TB5),TBM_SET_TEXTFLAG,0,DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_BKGND);         

         CreateWindow(TEXTBOX,L"00:00",WS_VISIBLE,
                      720,370,80,30,hwnd,ID_TB1,NULL,NULL);
         SendMessage(GetDlgItem(hwnd, ID_TB1),TBM_SET_TEXTFLAG,0,DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_BKGND);

         CreateWindow(TEXTBOX,L"00:00",WS_VISIBLE,
                      0,370,80,30,hwnd,ID_TB2,NULL,NULL);
         SendMessage(GetDlgItem(hwnd, ID_TB2),TBM_SET_TEXTFLAG,0,DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_BKGND);

         GetClientRect(hwnd,&rc); //��ô��ڵĿͻ�������

			//����λͼ�ṹ����
//			bm_0.Format	= BM_ARGB8888;     //λͼ��ʽ
//			bm_0.Width  = 240;              //���
//			bm_0.Height = 240;             //�߶�
//			bm_0.WidthBytes =bm_0.Width*4; //ÿ���ֽ���
//			bm_0.LUT =NULL;                //���ұ�(RGB/ARGB��ʽ��ʹ�øò���)
//			bm_0.Bits =(void*)gImage_0;    //λͼ����
      
        /* ���������memdc */
      rotate_disk_hdc = CreateMemoryDC(COLOR_FORMAT_ARGB8888,240,240); 
      /* ��ձ���Ϊ͸�� */
      ClrDisplay(rotate_disk_hdc,NULL,0);
      /* ����bmp��hdc */
      PIC_BMP_Draw_Res(rotate_disk_hdc,0,0,ROTATE_DISK_NAME, NULL);
      /* ת����bitmap */
      DCtoBitmap(rotate_disk_hdc,&bm_0);

			pSurf =CreateSurface(SURF_RGB565,240,240,-1,NULL);
         
         
         SetTimer(hwnd, 1, 200, TMR_START,NULL);

			rc.x =0;
			rc.y =0;
			rc.w =240;
			rc.h =240;
			hdc_mem11 =CreateDC(pSurf,&rc);

         break;
      }
		case WM_TIMER:
      {
			if(1)
			{
				static int a=0;
				a+=5;
				a%=360;
				ClrDisplay(hdc_mem11,NULL,MapRGB(hdc_mem11,0,0,0));
				RotateBitmap(hdc_mem11,120,120,&bm_0,a);
			}
			rc.x=280;
			rc.y=120;
			rc.w=240;
			rc.h=240;

			InvalidateRect(hwnd,&rc,FALSE);
			break;
      }         
      
      
      case WM_NOTIFY:
      {
         u16 code,  id, ctr_id;;
         id  =LOWORD(wParam);//��ȡ��Ϣ��ID��
         code=HIWORD(wParam);//��ȡ��Ϣ������
         ctr_id = LOWORD(wParam); //wParam��16λ�Ƿ��͸���Ϣ�Ŀؼ�ID. 
         
         NMHDR *nr;        
         HDC hdc;
         //���͵���
         if(code == BN_CLICKED)
         { 
            switch(id)
            {
               //����ICON����case
               case ID_BUTTON_List:
               {
                  enter_flag = 1;
                  App_MusicList();
                  break;
               }
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
               
               //���icon����case
               case ID_BUTTON_Equa:
               {
                  music_icon[2].state = ~music_icon[2].state;
                  if(music_icon[2].state == FALSE)
                  {
                     show_lrc = 0; //δ������ʴ���
                     //���ظ����������MINI����
//                     ShowWindow(mini_next, SW_HIDE);                     
//                     ShowWindow(mini_start, SW_HIDE); 
//                     ShowWindow(mini_back, SW_HIDE);
                     ShowWindow(wnd_lrc1, SW_HIDE);
                     ShowWindow(wnd_lrc2, SW_HIDE);
                     ShowWindow(wnd_lrc3, SW_HIDE);
                     ShowWindow(wnd_lrc4, SW_HIDE);
                     ShowWindow(wnd_lrc5, SW_HIDE);
                     RedrawWindow(hwnd, NULL, RDW_ALLCHILDREN|RDW_INVALIDATE);
                     ResetTimer(hwnd, 1, 200, TMR_START,NULL);
                  }
                  else
                  {
                     show_lrc = 1;//��ʴ����ѵ���
                     //��ʾ�����Ŀ�Լ����е�MINI����
//                     ShowWindow(mini_next, SW_SHOW);                     
//                     ShowWindow(mini_start, SW_SHOW); 
//                     ShowWindow(mini_back, SW_SHOW);
                     ShowWindow(wnd_lrc1, SW_SHOW);  
                     ShowWindow(wnd_lrc2, SW_SHOW);
                     ShowWindow(wnd_lrc3, SW_SHOW);
                     ShowWindow(wnd_lrc4, SW_SHOW);
                     ShowWindow(wnd_lrc5, SW_SHOW);

                     ResetTimer(hwnd, 1, 200, NULL,NULL);
                  }                                              
                  break;
               }
               //����icon����case
               case ID_BUTTON_START:
               {
//                     WCHAR wbuf[128];
                     music_icon[6].state = ~music_icon[6].state;
                     //����icon�ı���
                     //

                     if(music_icon[6].state == FALSE)
                     {

                        rt_thread_resume(h_music);
                        I2S_Play_Start();
                        SetWindowText(sub11_wnd, L"U");
                        ResetTimer(hwnd, 1, 200, TMR_START,NULL);
                        
                     }
                     else if(music_icon[6].state != FALSE)
                     {
                        rt_thread_suspend(h_music);
                        I2S_Play_Stop();                    
                        SetWindowText(sub11_wnd, L"T");
                        ResetTimer(hwnd, 1, 200, NULL,NULL);                       

                        
                     }  
                     InvalidateRect(hwnd, &music_icon[6].rc, TRUE);                     
                  break;                  
               }
               //��һ��icon����case
               case ID_BUTTON_NEXT:
               {     
                  WCHAR wbuf[128];
                  COLORREF color;
                  play_index++;
                  if(play_index >= file_num) play_index = 0;
                  if(play_index < 0) play_index = file_num - 1;
                  mp3player.ucStatus = STA_SWITCH;
                  hdc = GetDC(hwnd);
                                
                  color = GetPixel(hdc, 385, 404);  
                  x_mbstowcs_cp936(wbuf, lcdlist[play_index], FILE_NAME_LEN);
                  SetWindowText(GetDlgItem(hwnd, ID_TB5), wbuf);
                                 
                  SendMessage(wnd_time, SBM_SETVALUE, TRUE, 0); //���ý���ֵ
                  SetWindowText(GetDlgItem(MusicPlayer_hwnd, ID_TB1), L"00:00"); 
                  SetWindowText(GetDlgItem(MusicPlayer_hwnd, ID_TB2), L"00:00"); 
//                  DrawText(hdc, wbuf, -1, &rc_musicname, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
//                  ClrDisplay(hdc, &rc_MusicTimes, color);
//                  DrawText(hdc, L"00:00", -1, &rc_MusicTimes, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
                  ReleaseDC(hwnd, hdc);
                  
                  break;
               }
               //��һ��icon����case
               case ID_BUTTON_BACK:
               {
                 
                  COLORREF color;
                  play_index--;
                  if(play_index > file_num) play_index = 0;
                  if(play_index < 0) play_index = file_num - 1;
                  mp3player.ucStatus = STA_SWITCH;   
                  hdc = GetDC(hwnd);
                  color = GetPixel(hdc, 385, 404);
//                  x_mbstowcs_cp936(wbuf, lcdlist[play_index], FILE_NAME_LEN);
//                  SetWindowText(GetDlgItem(hwnd, ID_TB5), wbuf);
//                  DrawText(hdc, wbuf, -1, &rc_musicname, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
                  ReleaseDC(hwnd, hdc);            
                  break;
               }            
            
                       
            
               //MINI����/��ͣ����case
               case ID_BUTTON_MINISTOP:
               {
                     music_icon[9].state = ~music_icon[9].state;
                     //InvalidateRect(hwnd, &music_icon[9].rc, TRUE);
                     //ShowWindow(sub11_wnd, SW_HIDE);
                     //RedrawWindow(hwnd, NULL, RDW_ALLCHILDREN|RDW_INVALIDATE);
                     if(music_icon[9].state == FALSE)
                     {
                        
                        rt_thread_suspend(h_music);
                        I2S_Play_Stop();                    
                        SetWindowText(mini_start, L"I");
                        
                        SetWindowText(sub11_wnd, L"I");
                        
                     }
                     else if(music_icon[9].state != FALSE)
                     {
                        
                        rt_thread_resume(h_music);
                        I2S_Play_Start();
                        SetWindowText(mini_start, L"H");
                        SetWindowText(sub11_wnd, L"H");
                        
                     }

                     
                     //ShowWindow(sub11_wnd, SW_HIDE);                  
                  break;                  
               }
            }
         }//end of if(code == BN_CLICKED) 
         
      	nr = (NMHDR*)lParam; //lParam����������NMHDR�ṹ�忪ͷ.
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
                  chgsch = 1;
               }
               break;
            }
         }         
         
         break;
      } 
      //�ػ��ƺ�����Ϣ
      case WM_DRAWITEM:
      {
         DRAWITEM_HDR *ds;
         ds = (DRAWITEM_HDR*)lParam;
         if (ds->ID == ID_SCROLLBAR_POWER || ds->ID == ID_SCROLLBAR_TIMER)
         {
            scrollbar_owner_draw(ds);
            return TRUE;
         }
         if (ds->ID >= ID_BUTTON_Power && ds->ID<= ID_BUTTON_MINISTOP)
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
         HDC hdc_mem;//������
         RECT rc_top = {0 ,0, 800, 80};//�ϱ���
         RECT rc_bot = {0 ,400, 800, 80};//�±���
         //RECT test={0,90,100,100};
         
         
         //��ʼ����
         hdc = BeginPaint(hwnd, &ps);   
         //��ʾ����ʱ��
         //DrawText(hdc, L"00:00", -1, &rc_MusicTimes, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
         /*�ϱ���Ŀ*/
//         SetBrushColor(hdc_mem, MapARGB(hdc_mem, 50, 0, 0, 0));
//         FillRect(hdc_mem, &rc_top);
//         BitBlt(hdc, rc_top.x, rc_top.y, rc_top.w, rc_top.h, 
//                hdc_mem, rc_top.x, rc_top.y, SRCCOPY);         
//         /*�±���Ŀ*/ 
//         SetBrushColor(hdc_mem, MapARGB(hdc_mem, 50, 0, 0, 0));
//         FillRect(hdc_mem, &rc_bot);                                                                                                                                                                                                                                                                                                                                                                                                                                                                        
//         BitBlt(hdc, rc_bot.x, rc_bot.y, rc_bot.w, rc_bot.h, 
//                hdc_mem, rc_bot.x, rc_bot.y, SRCCOPY);
         
         rc.x=280;
			rc.y=120;
			rc.w=240;
			rc.h=240;
			BitBlt(hdc,rc.x,rc.y,rc.w,rc.h,hdc_mem11,0,0,SRCCOPY);
         
         
         
         //��ȡ��Ļ�㣨385��404������ɫ����Ϊ͸���ؼ��ı�����ɫ
         color_bg = GetPixel(hdc, 385, 404);
         EndPaint(hwnd, &ps);
         break;
      }
      case WM_ERASEBKGND:
      {
         HDC hdc =(HDC)wParam;
         RECT rc_cli;//�ͻ�������
         GetClientRect(hwnd, &rc_cli);//��ȡ�ͻ���λ����Ϣ
         
         SetBrushColor(hdc, MapRGB(hdc, 0,0,0));
         FillRect(hdc, &rc_cli);  
      }
      //����TEXTBOX�ı�����ɫ�Լ�������ɫ
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
         //������TEXTBOXΪ��ǰ�ĸ����
			if(id== ID_TEXTBOX_LRC3)
			{
				CTLCOLOR *cr;
				cr =(CTLCOLOR*)lParam;
				cr->TextColor =RGB888(255,255,255);//������ɫ��RGB888��ɫ��ʽ)
				cr->BackColor =RGB888(0,0,0);//������ɫ��RGB888��ɫ��ʽ)
				//cr->BorderColor =RGB888(255,10,10);//�߿���ɫ��RGB888��ɫ��ʽ)
				return TRUE;
			}
			else if(id == ID_TEXTBOX_LRC1||id == ID_TEXTBOX_LRC2||id == ID_TEXTBOX_LRC5||id == ID_TEXTBOX_LRC4)
			{
				CTLCOLOR *cr;
				cr =(CTLCOLOR*)lParam;
				cr->TextColor =RGB888(250,0,0);//������ɫ��RGB888��ɫ��ʽ)
				cr->BackColor =RGB888(0,0,0);//������ɫ��RGB888��ɫ��ʽ)
				//cr->BorderColor =RGB888(255,10,10);//�߿���ɫ��RGB888��ɫ��ʽ)
				return TRUE;				
			}
         if(id== ID_TB1 || id== ID_TB2 || id== ID_TB5 )
			{
				CTLCOLOR *cr;
				cr =(CTLCOLOR*)lParam;
				cr->TextColor =RGB888(255,255,255);//������ɫ��RGB888��ɫ��ʽ)
				cr->BackColor =RGB888(0,0,0);//������ɫ��RGB888��ɫ��ʽ)
				cr->BorderColor =RGB888(255,0,0);//�߿���ɫ��RGB888��ɫ��ʽ)
				return TRUE;
			}
         return FALSE;
		}     
      //�رմ�����Ϣ����case
      case WM_DESTROY:
      {
         DeleteSurface(pSurf);
			DeleteDC(hdc_mem11);
      DeleteDC(rotate_disk_hdc);
        
         return PostQuitMessage(hwnd);	
      }      
      
      default:
         return DefWindowProc(hwnd, msg, wParam, lParam);
   }
     
   return WM_NULL;
}


//���ֲ��������
HWND	MusicPlayer_hwnd;
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
	MusicPlayer_hwnd = CreateWindowEx(WS_EX_NOFOCUS,
		&wcex,
		L"GUI_MUSICPLAYER_DIALOG",
		WS_VISIBLE,
		0, 0, GUI_XSIZE, GUI_YSIZE,
		NULL, NULL, NULL, NULL);

	//��ʾ������
	ShowWindow(MusicPlayer_hwnd, SW_SHOW);

	//��ʼ������Ϣѭ��(���ڹرղ�����ʱ,GetMessage������FALSE,�˳�����Ϣѭ��)��
	while (GetMessage(&msg, MusicPlayer_hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}


