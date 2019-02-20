#include <emXGUI.h>
#include "pic_view.h"
#include	"CListMenu.h"
#include "GUI_AppDef.h"
#include "Widget.h"
#include "ff.h"
#include "x_libc.h"
#include <string.h>
#include "emXGUI_JPEG.h"
#include "emxgui_png.h"
icon_S GUI_PicViewer_Icon[10] = 
{
  {"Pic_Name",           {100,0,600,35},        FALSE},
  {"Pic_MSGBOX",         {200,240,400,70},      FALSE},
  {"Pic_Res",            {90,35,200,35},       FALSE},
  {"Pic_Res_Value",      {290,35,100,35},       FALSE},
  {"Pic_Scale",          {400,35,120,35},       FALSE}, 
  {"Pic_Scale_Value",    {520,35,150,35},       FALSE},
  {"In_Flash",           {0,420,250,60},       FALSE},
  {"Ex_Flash",           {250,420,250,60},       FALSE}, 
  {"SD_Card",            {500,420,300,60},       FALSE}, 
  
};

static char path[100] = "0:";//�ļ���Ŀ¼

PicViewer_Master_Struct PicViewer = 
{
  .pic_nums = 0,
  .show_index = 0,
  .Menu_State = 0,
  .SecMenu_State = 0,
  .cur_type = 0,
  .cur_path = eID_Pic_SDCARD,
};
static void PicViewer_ExitButton_OwnerDraw(DRAWITEM_HDR *ds) //����һ����ť���
{
	HWND hwnd;
	HDC hdc;
	RECT rc,rc_tmp;
	WCHAR wbuf[128];

	hwnd = ds->hwnd; //button�Ĵ��ھ��.
	hdc = ds->hDC;   //button�Ļ�ͼ�����ľ��.
	rc = ds->rc;     //button�Ļ��ƾ�����.
  GetClientRect(hwnd, &rc_tmp);//�õ��ؼ���λ��
  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//����ת��
  SetBrushColor(hdc, MapRGB(hdc, 0,0,0));
  FillRect(hdc, &rc);
  //BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, PicViewer.mhdc_bk, rc_tmp.x, rc_tmp.y, SRCCOPY);
	SetBrushColor(hdc, MapRGB(hdc, COLOR_DESKTOP_BACK_GROUND));
   
  FillCircle(hdc, rc.x+rc.w, rc.y, rc.w);
	//FillRect(hdc, &rc); //�þ�����䱳��

  if (ds->State & BST_PUSHED)
	{ //��ť�ǰ���״̬
//    GUI_DEBUG("ds->ID=%d,BST_PUSHED",ds->ID);
//		SetBrushColor(hdc,MapRGB(hdc,150,200,250)); //�������ɫ(BrushColor��������Fill���͵Ļ�ͼ����)
//		SetPenColor(hdc,MapRGB(hdc,250,0,0));        //���û���ɫ(PenColor��������Draw���͵Ļ�ͼ����)
		SetTextColor(hdc, MapRGB(hdc, 105, 105, 105));      //��������ɫ
	}
	else
	{ //��ť�ǵ���״̬
//		SetBrushColor(hdc,MapRGB(hdc,255,255,255));
//		SetPenColor(hdc,MapRGB(hdc,0,250,0));
		SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));
	}

	  /* ʹ�ÿ���ͼ������ */
	SetFont(hdc, controlFont_64);
	//  SetTextColor(hdc,MapRGB(hdc,255,255,255));

	GetWindowText(hwnd, wbuf, 128); //��ð�ť�ؼ�������
  rc.y = -10;
  rc.x = 16;
	DrawText(hdc, wbuf, -1, &rc, NULL);//��������(���ж��뷽ʽ)


  /* �ָ�Ĭ������ */
	SetFont(hdc, defaultFont);

}
static void PicViewer_Button_OwnerDraw(DRAWITEM_HDR *ds) //����һ����ť���
{
	HWND hwnd;
	HDC hdc;
	RECT rc, rc_tmp;
	WCHAR wbuf[128];

	hwnd = ds->hwnd; //button�Ĵ��ھ��.
	hdc = ds->hDC;   //button�Ļ�ͼ�����ľ��.
	rc = ds->rc;     //button�Ļ��ƾ�����.

  GetClientRect(hwnd, &rc_tmp);//�õ��ؼ���λ��
  GetClientRect(hwnd, &rc);//�õ��ؼ���λ��
  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//����ת��

  //BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, PicViewer.mhdc_bk, rc_tmp.x, rc_tmp.y, SRCCOPY);
//  SetBrushColor(hdc, MapRGB(hdc, 0,0,0));
//  FillRect(hdc, &rc);
	if (IsWindowEnabled(hwnd) == FALSE)
	{
		SetTextColor(hdc, MapRGB(hdc, COLOR_INVALID));
	}
	else if (ds->State & BST_PUSHED)
	{ //��ť�ǰ���״̬
//    GUI_DEBUG("ds->ID=%d,BST_PUSHED",ds->ID);
//		SetBrushColor(hdc,MapRGB(hdc,150,200,250)); //�������ɫ(BrushColor��������Fill���͵Ļ�ͼ����)
//		SetPenColor(hdc,MapRGB(hdc,250,0,0));        //���û���ɫ(PenColor��������Draw���͵Ļ�ͼ����)
		SetTextColor(hdc, MapRGB(hdc, 105, 105, 105));      //��������ɫ
	}
	else
	{ //��ť�ǵ���״̬
//		SetBrushColor(hdc,MapRGB(hdc,255,255,255));
//		SetPenColor(hdc,MapRGB(hdc,0,250,0));
		SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));
	}


	//	SetBrushColor(hdc,COLOR_BACK_GROUND);

	//	FillRect(hdc,&rc); //�þ�����䱳��
	//	DrawRect(hdc,&rc); //���������
	//  
	//  FillCircle(hdc,rc.x+rc.w/2,rc.x+rc.w/2,rc.w/2); //�þ�����䱳��FillCircle
	//	DrawCircle(hdc,rc.x+rc.w/2,rc.x+rc.w/2,rc.w/2); //���������

	  /* ʹ�ÿ���ͼ������ */
//	SetFont(hdc, controlFont_64);
	//  SetTextColor(hdc,MapRGB(hdc,255,255,255));

	GetWindowText(ds->hwnd, wbuf, 128); //��ð�ť�ؼ�������

	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER | DT_CENTER);//��������(���ж��뷽ʽ)


//  /* �ָ�Ĭ������ */
//	SetFont(hdc, defaultFont);

}
//͸���ı�
static void PicViewer_TBOX_OwnerDraw(DRAWITEM_HDR *ds) //����һ����ť���
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
  
//  SetBrushColor(hdc, MapRGB(hdc, 0,0,0));
//  FillRect(hdc, &rc);
  //BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, PicViewer.mhdc_bk, rc_tmp.x, rc_tmp.y, SRCCOPY);
  SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));


  GetWindowText(hwnd, wbuf, 128); //��ð�ť�ؼ�������

  DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);//��������(���ж��뷽ʽ)

}
static void Pic_ReturnBTN_Ownerdraw(DRAWITEM_HDR *ds) //����һ����ť���
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	WCHAR wbuf[128];

	hwnd = ds->hwnd; //button�Ĵ��ھ��.
	hdc = ds->hDC;   //button�Ļ�ͼ�����ľ��.
	rc = ds->rc;     //button�Ļ��ƾ�����.
  if(ds->State & BST_PUSHED) //��ť�ǰ���״̬
	{

		SetTextColor(hdc, MapRGB(hdc, 105, 105, 105));      //��������ɫ
	}
	else//��ť�ǵ���״̬
	{ 

		SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));
	}

	/* ʹ�ÿ���ͼ������ */
	SetFont(hdc, controlFont_32);

	GetWindowText(hwnd, wbuf, 128); //��ð�ť�ؼ�������

	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER);//��������(���ж��뷽ʽ)
   rc.x = 25; 
  /* �ָ�Ĭ������ */
	SetFont(hdc, defaultFont);
  DrawText(hdc, L"����", -1, &rc, DT_VCENTER);
}

void Draw_Pic_JPG(char *file_name)
{
  BOOL res;
  RECT rc = {0,0,800,480};
  u8 *jpeg_buf;
  u32 jpeg_size;
  JPG_DEC *dec;
  res= FS_Load_Content(file_name, (char**)&jpeg_buf, &jpeg_size);
  if(res)
  {
    /* ����ͼƬ���ݴ���JPG_DEC��� */
    dec = JPG_Open(jpeg_buf, jpeg_size);
    /* ��ȡͼƬ�ļ���Ϣ */
    JPG_GetImageSize(&PicViewer.pic_width, &PicViewer.pic_height,dec);
    
    PicViewer.mhdc_pic = CreateMemoryDC(SURF_SCREEN, 800, 480);
    SetBrushColor(PicViewer.mhdc_pic, MapRGB(PicViewer.mhdc_pic, 0, 0, 0));
    FillRect(PicViewer.mhdc_pic, &rc);     
    
    HDC hdc_tmp;
//    int scale_w = (PicViewer.pic_width>660)?660:PicViewer.pic_width;
//    int scale_h = (PicViewer.pic_height>410)?410:PicViewer.pic_height;
//    
//    PicViewer.scale_x = (scale_w == PicViewer.pic_width)?1:(float)660/PicViewer.pic_width;
//    PicViewer.scale_y = (scale_h == PicViewer.pic_height)?1:(float)410/PicViewer.pic_height;
    
    hdc_tmp = CreateMemoryDC(SURF_SCREEN, 800, 480);
    JPG_Draw(hdc_tmp, 0, 0, dec);
    StretchBlt(PicViewer.mhdc_pic,400-PicViewer.pic_width/2,240 - PicViewer.pic_height/2,
    PicViewer.pic_width,PicViewer.pic_height,
               hdc_tmp,0,0,PicViewer.pic_width,PicViewer.pic_height,SRCCOPY);
    DeleteDC(hdc_tmp);
    /* �ر�JPG_DEC��� */
    JPG_Close(dec);
  }
  /* �ͷ�ͼƬ���ݿռ� */
  RES_Release_Content((char **)&jpeg_buf);  
}


void Draw_Pic_PNG(char *file_name)
{
  BOOL res;
  BITMAP png_bm;
  u8 *png_buf;
  u32 png_size;
  PNG_DEC *png_dec;
  HDC hdc_tmp;
  RECT rc = {0,0,800,480};
  res= FS_Load_Content(file_name, (char**)&png_buf, &png_size);
  if(res)
  {
    GUI_DEBUG("�򿪳ɹ�");
    png_dec = PNG_Open(png_buf, png_size);
    PNG_GetBitmap(png_dec, &png_bm);
    
    PicViewer.mhdc_pic = CreateMemoryDC(SURF_SCREEN, 800, 480);
    SetBrushColor(PicViewer.mhdc_pic, MapRGB(PicViewer.mhdc_pic, 0, 0, 0));
    FillRect(PicViewer.mhdc_pic, &rc);  
    hdc_tmp = CreateMemoryDC(SURF_SCREEN, 800, 480); 
    SetBrushColor(hdc_tmp, MapRGB(hdc_tmp, 0, 0, 0));
    FillRect(hdc_tmp, &rc);       
    PicViewer.pic_width = png_bm.Width;
    PicViewer.pic_height = png_bm.Height;
    
    DrawBitmap(hdc_tmp, 0,0, &png_bm, NULL);
    
    StretchBlt(PicViewer.mhdc_pic,400-PicViewer.pic_width/2,240 - PicViewer.pic_height/2,
               PicViewer.pic_width,PicViewer.pic_height,
               hdc_tmp,0,0,PicViewer.pic_width,PicViewer.pic_height,SRCCOPY);    
    DeleteDC(hdc_tmp);
    PNG_Close(png_dec);
  }
  RES_Release_Content((char **)&png_buf);
}

BITMAPINFO bm_info;
void Draw_Pic_BMP(HDC hdc, char *file_name)
{
  RECT rc = {0,0,800,480};
  PIC_BMP_GetInfo_FS(&bm_info,file_name);

  PicViewer.pic_width = bm_info.Width;
  PicViewer.pic_height = bm_info.Height;

  PicViewer.mhdc_pic = CreateMemoryDC(SURF_SCREEN, 800, 480);
  SetBrushColor(PicViewer.mhdc_pic, MapRGB(PicViewer.mhdc_pic, 0, 0, 0));
  FillRect(PicViewer.mhdc_pic, &rc);     
    
  HDC hdc_tmp;
    
  hdc_tmp = CreateMemoryDC(SURF_SCREEN, 800, 480);
  SetBrushColor(hdc_tmp, MapRGB(hdc_tmp, 0, 0, 0));
  FillRect(hdc_tmp, &rc);   
  
   /* ����bmp��hdc */
  PIC_BMP_Draw_FS(hdc_tmp, 0,0, file_name, NULL); 
  
  StretchBlt(PicViewer.mhdc_pic,400-PicViewer.pic_width/2,240 - PicViewer.pic_height/2,
  PicViewer.pic_width,PicViewer.pic_height,
             hdc_tmp,0,0,PicViewer.pic_width,PicViewer.pic_height,SRCCOPY);
  DeleteDC(hdc_tmp);  
}

IMAGE_INFO img_info; 
HANDLE hgif;
void Draw_Pic_GIF(char *file_name, HDC hdc)
{
  static int i = 0;
  RECT rc = {0,0,800,480};
  //GUI_DEBUG("%s", file_name);
//  if(res)
  {
    switch(PicViewer.gif_state)
    {
      case 0:
      {
        GUI_DEBUG("%s", file_name);
        GUI_DEBUG("0");
        u8 *gif_buf;
        u32 gif_size;
        i = 0;//�������
        PicViewer.gif_state = 1;
        FS_Load_Content(file_name, (char**)&gif_buf, &gif_size);
        hgif = GIF_Open(gif_buf);
        GIF_GetInfo(hgif,&img_info);
        PicViewer.frame_num = GIF_GetFrameCount(hgif); 

        PicViewer.pic_width = img_info.Width;
        PicViewer.pic_height = img_info.Height;
           
        RES_Release_Content((char **)&gif_buf);
      }
      case 1:
      {
        if(i>=PicViewer.frame_num)
        {
           i=0;
        } 
        PicViewer.mhdc_pic = CreateMemoryDC(SURF_SCREEN, 800, 480);
        SetBrushColor(PicViewer.mhdc_pic, MapRGB(PicViewer.mhdc_pic, 0, 0, 0));
        FillRect(PicViewer.mhdc_pic, &rc);     
          
        HDC hdc_tmp;
          
        hdc_tmp = CreateMemoryDC(SURF_SCREEN, 800, 480);
        SetBrushColor(hdc_tmp, MapRGB(hdc_tmp, 0, 0, 0));
        FillRect(hdc_tmp, &rc);        
        PicViewer.delay = GIF_DrawFrame(hdc_tmp,0,0,MapRGB(hdc,0,0,0),hgif,i); 
        StretchBlt(PicViewer.mhdc_pic,400-PicViewer.pic_width/2,240 - PicViewer.pic_height/2,
        PicViewer.pic_width,PicViewer.pic_height,
                   hdc_tmp,0,0,PicViewer.pic_width,PicViewer.pic_height,SRCCOPY);     
        DeleteDC(hdc_tmp);         
        i++;
        break;
      }
    } 
  }
}

static FRESULT scan_Picfiles(char* path, char* type) 
{ 
  FRESULT res; 		//�����ڵݹ���̱��޸ĵı���������ȫ�ֱ���	
  FILINFO fno; 
  DIR dir; 
  int i; 
  char *fn; 
  char file_name[100];	
	
#if _USE_LFN 
  static char lfn[_MAX_LFN * (0x81 ? 2 : 1) + 1]; 	//���ļ���֧��
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
      if (*fn == '.') continue; 											//���ʾ��ǰĿ¼������			
      if (fno.fattrib & AM_DIR) 
			{ 																							//Ŀ¼���ݹ��ȡ
        sprintf(&path[i], "/%s", fn); 							//�ϳ�����Ŀ¼��
        res = scan_Picfiles(path,type);											//�ݹ���� 
        if (res != FR_OK) 
					break; 																		//��ʧ�ܣ�����ѭ��
        path[i] = 0; 
      } 
      else 
		{ 
				//printf("%s%s\r\n", path, fn);								//����ļ���
				if(strstr(fn, type))
				{
					if ((strlen(path)+strlen(fn)<100)&&(PicViewer.pic_nums<100))
					{
						sprintf(file_name, "%s/%s", path, fn);						
						memcpy(PicViewer.pic_list[PicViewer.pic_nums],file_name,strlen(file_name));//��·��
            printf("%s\r\n", PicViewer.pic_list[PicViewer.pic_nums]);
						memcpy(PicViewer.pic_lcdlist[PicViewer.pic_nums],fn,strlen(fn));						//�ļ���
						PicViewer.pic_nums++;//��¼�ļ�����
					}
				}//if
      }//else
     } //for
  } 
  return res; 
}

void PicViewer_Init(void)
{
  int i = 0, j = 0;
  
  //Step1:�����ڴ�ռ�  
  PicViewer.pic_list = (char **)GUI_VMEM_Alloc(sizeof(char*) * PICFILE_NUM_MAX);//�����пռ�
  for(i = 0; i < PICFILE_NUM_MAX; i++)
  {
    PicViewer.pic_list[i] = (char *)GUI_VMEM_Alloc(sizeof(char) * PICFILE_NAME_MAXLEN);//�����пռ�
  }
  
  PicViewer.pic_lcdlist = (char **)GUI_VMEM_Alloc(sizeof(char*) * PICFILE_NUM_MAX);//�����пռ�
  for(i = 0; i < PICFILE_NUM_MAX; i++)
  {
    PicViewer.pic_lcdlist[i] = (char *)GUI_VMEM_Alloc(sizeof(char) * PICFILE_NAME_MAXLEN);//�����пռ�
  }  
  for(i = 0; i < PICFILE_NUM_MAX; i++)
  {
    for(j = 0; j < PICFILE_NAME_MAXLEN; j++)
    {
      PicViewer.pic_lcdlist[i][j] = '\0';
      PicViewer.pic_list[i][j] = '\0';
    }
  }
  //Step2��ɨ��ͼƬ�ļ�
  scan_Picfiles(path,".jpg");
}
void PicViewer_Quit(void)
{
  int i = 0;
  for(;i < PICFILE_NUM_MAX; i++)
  {
    GUI_VMEM_Free(PicViewer.pic_list[i]);
    GUI_VMEM_Free(PicViewer.pic_lcdlist[i]);
  }
  GUI_VMEM_Free(PicViewer.pic_list);
  GUI_VMEM_Free(PicViewer.pic_lcdlist);
  
  PicViewer.pic_nums = 0;
  PicViewer.show_index = 0;
  PicViewer.cur_type = eID_Pic_JPG;
  PicViewer.cur_path = eID_Pic_SDCARD;
  //DeleteDC(PicViewer.mhdc_bk);
  
}

static	LRESULT DlgSDCARD_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)
  {
    case WM_CREATE:
    {
      RECT rc;
      GetClientRect(hwnd, &rc); 
      CreateWindow(BUTTON, L"ͼƬ����", WS_TRANSPARENT|WS_OWNERDRAW|WS_VISIBLE, 
                   GUI_PicViewer_Icon[0].rc.x, GUI_PicViewer_Icon[0].rc.y, 
                   GUI_PicViewer_Icon[0].rc.w, GUI_PicViewer_Icon[0].rc.h,          
                   hwnd, eID_Pic_Name, NULL, NULL);  
      CreateWindow(BUTTON, L"L", BS_FLAT | BS_NOTIFY |WS_TRANSPARENT| WS_OWNERDRAW |WS_VISIBLE,
                   0, rc.h * 1 / 2, 70, 70, hwnd, eID_Pic_PREV, NULL, NULL);
      SetWindowFont(GetDlgItem(hwnd,eID_Pic_PREV), controlFont_64); 
      CreateWindow(BUTTON, L"K", BS_FLAT | BS_NOTIFY |WS_TRANSPARENT| WS_OWNERDRAW | WS_VISIBLE,
                   rc.w - 65, rc.h * 1 / 2, 70, 70, hwnd, eID_Pic_NEXT, NULL, NULL);
      SetWindowFont(GetDlgItem(hwnd,eID_Pic_NEXT), controlFont_64);   
      
      CreateWindow(BUTTON, L"F", BS_FLAT | BS_NOTIFY|WS_TRANSPARENT|WS_OWNERDRAW |WS_VISIBLE,
                    0, 0, 75, 50, hwnd, eID_Pic_Return, NULL, NULL);  
      
      CreateWindow(BUTTON, L"JPG", WS_TRANSPARENT| WS_OWNERDRAW |WS_VISIBLE,
                   0, 420, 60, 60, hwnd, eID_Pic_JPG, NULL, NULL);
 
      CreateWindow(BUTTON, L"+", BS_FLAT | BS_NOTIFY | WS_TRANSPARENT|WS_OWNERDRAW | WS_VISIBLE,
                   675, 420, 60, 60, hwnd, eID_ZOOMIN, NULL, NULL);
      
      CreateWindow(BUTTON, L"-", BS_FLAT | BS_NOTIFY | WS_TRANSPARENT|WS_OWNERDRAW| WS_VISIBLE,
                   740, 420, 60, 60, hwnd, eID_ZOOMOUT, NULL, NULL);
                   
      CreateWindow(TEXTBOX, L" ", NULL, 
                   GUI_PicViewer_Icon[1].rc.x, GUI_PicViewer_Icon[1].rc.y, 
                   GUI_PicViewer_Icon[1].rc.w, GUI_PicViewer_Icon[1].rc.h,          
                   hwnd, eID_Pic_MsgBOX, NULL, NULL);
      SendMessage(GetDlgItem(hwnd,eID_Pic_MsgBOX),TBM_SET_TEXTFLAG,0,DT_VCENTER|DT_CENTER|DT_BORDER|DT_BKGND); 
      SetTimer(hwnd,1,0,TMR_SINGLE,NULL);//��������
      SetTimer(hwnd,2,0,TMR_SINGLE,NULL);//��������
      PicViewer_Init();
      break;
    }
    case WM_TIMER:
    {
      
      switch((UINT)wParam)
      {
        case 1:
        {
          ShowWindow(GetDlgItem(hwnd, eID_Pic_MsgBOX), SW_HIDE);
          break;
        }
        case 2:
        {
          InvalidateRect(hwnd,NULL,TRUE);
          break;
        }
      }
      break;
    }    
    case WM_NOTIFY:
    {
      u16 code, id;
      id  =LOWORD(wParam);//��ȡ��Ϣ��ID��
      code=HIWORD(wParam);//��ȡ��Ϣ������
      if(code == BN_CLICKED)
      { 
        switch(id)
        {
          case eID_Pic_NEXT:
          {
            PicViewer.show_index++;
            
            if(PicViewer.show_index < PicViewer.pic_nums)
            {
              if(PicViewer.cur_type == 3)
                PicViewer.gif_state = 0;
              RedrawWindow(hwnd, NULL, RDW_ALLCHILDREN|RDW_ERASE|RDW_INVALIDATE);
            }
            else
            {
              PicViewer.show_index--;
              SetWindowText(GetDlgItem(hwnd, eID_Pic_MsgBOX), L"����Ƭ�Ѿ������һ����");
              ResetTimer(hwnd,1,1000,TMR_START|TMR_SINGLE,NULL);
              ShowWindow(GetDlgItem(hwnd, eID_Pic_MsgBOX), SW_SHOW);    
            }

            
            break;
          }
          case eID_Pic_PREV:
          {
            PicViewer.show_index--;
            if(PicViewer.show_index >= 0)
            {
              if(PicViewer.cur_type == 3)
                PicViewer.gif_state = 0;              
              RedrawWindow(hwnd, NULL, RDW_ALLCHILDREN|RDW_ERASE|RDW_INVALIDATE);

            }
            else
            {
              PicViewer.show_index++;
              SetWindowText(GetDlgItem(hwnd, eID_Pic_MsgBOX), L"����Ƭ�Ѿ��ǵ�һ����");
              ResetTimer(hwnd,1,1000,TMR_START|TMR_SINGLE,NULL);
              ShowWindow(GetDlgItem(hwnd, eID_Pic_MsgBOX), SW_SHOW);    
            }

            //InvalidateRect(hwnd, NULL, TRUE);
            break;
          }
          case eID_Pic_JPG:
          {
            int i, j;
            PicViewer.cur_type++;
            PicViewer.show_index = 0;
            PicViewer.pic_nums= 0;
            for(i = 0; i < PICFILE_NUM_MAX; i++)
            {
              for(j = 0; j < PICFILE_NAME_MAXLEN; j++)
              {
                PicViewer.pic_lcdlist[i][j] = '\0';
                PicViewer.pic_list[i][j] = '\0';
              }
            }  
            if(PicViewer.cur_type > 3)
              PicViewer.cur_type = 0; 
            if(PicViewer.gif_state != 0 && PicViewer.cur_type != 3)
            {
              PicViewer.gif_state = 0;
              GIF_Close(hgif);
              
            }            
            switch(PicViewer.cur_type)
            {
              case 0:
              {
                scan_Picfiles(path,".jpg");
                SetWindowText(GetDlgItem(hwnd, eID_Pic_JPG), L"JPG");
                InvalidateRect(PicViewer.mPicViewer, NULL, TRUE);
                break;
              }//JPG
              case 1:
              {
                scan_Picfiles(path,".png");
                InvalidateRect(PicViewer.mPicViewer, NULL, TRUE);
                SetWindowText(GetDlgItem(hwnd, eID_Pic_JPG), L"PNG");
                
                break;                
              }
              case 2:
              {
                scan_Picfiles(path,".bmp");                
                SetWindowText(GetDlgItem(hwnd, eID_Pic_JPG), L"BMP");
                InvalidateRect(PicViewer.mPicViewer, NULL, TRUE);
                break;
              }
              case 3:
              {
                scan_Picfiles(path,".gif");
                
                SetWindowText(GetDlgItem(hwnd, eID_Pic_JPG), L"GIF");
                InvalidateRect(PicViewer.mPicViewer, NULL, TRUE);
                break;                
              }              
            
            }
            break;
          }
//          case eID_Pic_EXIT:
//          {
//            PostCloseMessage(hwnd);
//            break;
//          }           
        }
        
      }
      break;
    }      
    case WM_DRAWITEM:
    {
       DRAWITEM_HDR *ds;
       ds = (DRAWITEM_HDR*)lParam;
       switch(ds->ID)
       {
          case eID_Pic_Name:
          {              
             PicViewer_TBOX_OwnerDraw(ds);
             return TRUE;              
          } 
          case eID_Pic_PREV:
          {
            PicViewer_Button_OwnerDraw(ds); //ִ���Ի��ư�ť
            return TRUE; 
          }            
          case eID_Pic_NEXT:
          {
            PicViewer_Button_OwnerDraw(ds); //ִ���Ի��ư�ť
            return TRUE; 
          } 
          case eID_Pic_JPG:
          {
            PicViewer_Button_OwnerDraw(ds); //ִ���Ի��ư�ť
            return TRUE; 
          }            
          case eID_ZOOMIN:
          {
            PicViewer_Button_OwnerDraw(ds); //ִ���Ի��ư�ť
            return TRUE; 
          } 
          case eID_ZOOMOUT:
          {
            PicViewer_Button_OwnerDraw(ds); //ִ���Ի��ư�ť
            return TRUE; 
          }
          case eID_Pic_Return:
          {
            Pic_ReturnBTN_Ownerdraw(ds); //ִ���Ի��ư�ť
            return TRUE; 
          }          
                     
       }
       break;
    }
    case WM_ERASEBKGND:
    {
      HDC hdc =(HDC)wParam;
      RECT rc =*(RECT*)lParam;
      WCHAR wbuf[128];
      switch(PicViewer.cur_type)
      {
        case 0:
        {
          //GUI_DEBUG("JPG");
          Draw_Pic_JPG(PicViewer.pic_list[PicViewer.show_index]);
          BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, PicViewer.mhdc_pic, rc.x, rc.y, SRCCOPY);
          DeleteDC(PicViewer.mhdc_pic);
          break;
        }
//        case eID_Pic_BMP:
//        {
////          SetBrushColor(hdc, MapRGB(hdc, 0, 0, 0));
////          FillRect(hdc, &rc); 
//          Draw_Pic_BMP(PicViewer.mhdc_pic, PicViewer.pic_list[PicViewer.show_index]);
//          BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, PicViewer.mhdc_pic, rc.x, rc.y, SRCCOPY);
//          DeleteDC(PicViewer.mhdc_pic);          
//          GUI_DEBUG("BMP");

//          break;
//        }
        case 1:
        {          
          GUI_DEBUG("PNG");
          Draw_Pic_PNG(PicViewer.pic_list[PicViewer.show_index]);
          BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, PicViewer.mhdc_pic, rc.x, rc.y, SRCCOPY);
          DeleteDC(PicViewer.mhdc_pic);         

          break;
        }
        case 2:
        {

          Draw_Pic_BMP(hdc, PicViewer.pic_list[PicViewer.show_index]);
          GUI_DEBUG("BMP");
          BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, PicViewer.mhdc_pic, rc.x, rc.y, SRCCOPY);
          DeleteDC(PicViewer.mhdc_pic);
          break;
        }        
        case 3:
        {
          //GUI_DEBUG("GIF");
     
          Draw_Pic_GIF(PicViewer.pic_list[PicViewer.show_index], hdc);
          BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, PicViewer.mhdc_pic, rc.x, rc.y, SRCCOPY);
          DeleteDC(PicViewer.mhdc_pic);          
          ResetTimer(hwnd,2,PicViewer.delay,TMR_SINGLE|TMR_START,NULL);          
        }          
      }
      //
      
      x_mbstowcs_cp936(wbuf, PicViewer.pic_lcdlist[PicViewer.show_index], PICFILE_NAME_MAXLEN);
      SetWindowText(GetDlgItem(hwnd, eID_Pic_Name), wbuf); 
//      x_wsprintf(wbuf, L"%d*%d", PicViewer.pic_width, PicViewer.pic_height);
//      SetWindowText(GetDlgItem(hwnd, eID_Pic_Res_Value), wbuf); 
      //x_mbstowcs_cp936(wbuf, PicViewer.pic_lcdlist[PicViewer.show_index], PICFILE_NAME_MAXLEN);
//      SetBrushColor(PicViewer.mhdc_bk, MapRGB(PicViewer.mhdc_bk, 0, 0, 0));
//      FillRect(PicViewer.mhdc_bk, &rc);
      //PIC_BMP_Draw_FS(hdc, 70, 70, PicViewer.pic_list[PicViewer.show_index], NULL); 
      //BitBlt(PicViewer.mhdc_bk, rc.x, rc.y, rc.w, rc.h, PicViewer.mhdc_pic, 0, 0, SRCCOPY);
      
      
      
      return TRUE;
    }        
    default:
      return	DefWindowProc(hwnd, msg, wParam, lParam);    
  }
  return WM_NULL;
}

static	LRESULT	PicViewer_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)
  {
    case WM_CREATE:
    {
      RECT rc_in = {70,140,660,200};
      RECT rc[3];

      CreateWindow(TEXTBOX, L"ͼƬ�����", WS_VISIBLE, 100,0,600,35,          
                   hwnd, eID_Pic_Title, NULL, NULL);
      SendMessage(GetDlgItem(hwnd,eID_Pic_Title),TBM_SET_TEXTFLAG,0,DT_VCENTER|DT_CENTER|DT_BKGND);      
      CreateWindow(TEXTBOX, L"֧��jpg��bmp��png��gif��ʽ", WS_VISIBLE, 100,35,600,35,          
                   hwnd, eID_Pic_Def, NULL, NULL);
      SendMessage(GetDlgItem(hwnd,eID_Pic_Def),TBM_SET_TEXTFLAG,0,DT_VCENTER|DT_CENTER|DT_BKGND);            
      
     
      MakeMatrixRect(rc, &rc_in, 20, 0, 3, 1);            
      
      CreateWindow(BUTTON, L"�ڲ�FLASH", BS_PUSHLIKE | BS_RADIOBOX |WS_VISIBLE,
                   rc[0].x, rc[0].y, 
                   rc[0].w, rc[0].h,
                   hwnd, (1<<16)|eID_Pic_INTFLASH, NULL, NULL);
      CreateWindow(BUTTON, L"�ⲿFLASH", BS_PUSHLIKE | BS_RADIOBOX|WS_VISIBLE ,
                   rc[1].x, rc[1].y, 
                   rc[1].w, rc[1].h,
                   hwnd, (1<<16)|eID_Pic_EXTFLASH, NULL, NULL);
      CreateWindow(BUTTON, L"SD��", BS_PUSHLIKE | BS_RADIOBOX|WS_VISIBLE   ,
                   rc[2].x, rc[2].y, 
                   rc[2].w, rc[2].h,
                   hwnd, (1<<16)|eID_Pic_SDCARD, NULL, NULL);      
      CreateWindow(BUTTON, L"O", BS_FLAT | BS_NOTIFY |WS_OWNERDRAW|WS_VISIBLE,
                   730, 0, 70, 70, hwnd, eID_Pic_EXIT, NULL, NULL);      
      break;
    }
    case WM_ERASEBKGND:
    {
      HDC hdc =(HDC)wParam;
      RECT rc =*(RECT*)lParam; 
      SetBrushColor(hdc, MapRGB(hdc, 0, 0, 0));
      FillRect(hdc, &rc);       
      return TRUE;
    }
    case WM_DRAWITEM:
    {
       DRAWITEM_HDR *ds;
       ds = (DRAWITEM_HDR*)lParam;
       switch(ds->ID)
       {
          case eID_Pic_EXIT:
          {
             PicViewer_ExitButton_OwnerDraw(ds);
             return TRUE;             
          }
                    
       }
//       if(ds->ID == eID_Pic_INTFLASH || ds->ID == eID_Pic_EXTFLASH || ds->ID == eID_Pic_SDCARD)
//       {
//         
//         PicViewer_AButton_OwnerDraw(ds);//��͸���ȿؼ�
//         return TRUE;
//       }
       break;
    }  
    case WM_NOTIFY:
    {
      u16 code, id;
      id  =LOWORD(wParam);//��ȡ��Ϣ��ID��
      code=HIWORD(wParam);//��ȡ��Ϣ������      
      if(id==eID_Pic_SDCARD&& code==BN_CLICKED)
      {
        NMHDR *nr=(NMHDR*)lParam;
        if(SendMessage(nr->hwndFrom,BM_GETSTATE,0,0)&BST_CHECKED) //��ȡ��ǰ״̬
        { //��ѡ��ѡ��.
          WNDCLASS wcex;

          wcex.Tag	 		= WNDCLASS_TAG;
          wcex.Style			= CS_HREDRAW | CS_VREDRAW;
          wcex.lpfnWndProc	= (WNDPROC)DlgSDCARD_proc;
          wcex.cbClsExtra		= 0;
          wcex.cbWndExtra		= 0;
          wcex.hInstance		= NULL;
          wcex.hIcon			= NULL;
          wcex.hCursor		= NULL;
        
          PicViewer.mPicViewer = CreateWindowEx(WS_EX_FRAMEBUFFER,
                                 &wcex,L"SDCARD",
                                 WS_OVERLAPPED|WS_CLIPCHILDREN|WS_VISIBLE,
                                 0,0,800,480,
                                 hwnd,0,NULL,NULL);

        }                        
      }  
      break;      
    }            
		case	WM_CTLCOLOR:
		{
			u16 id;
			id =LOWORD(wParam);
			if(id== eID_Pic_Title || id == eID_Pic_Def)
			{
				CTLCOLOR *cr;
				cr =(CTLCOLOR*)lParam;
				cr->TextColor =RGB888(255,255,255);//������ɫ��RGB888��ɫ��ʽ)
				cr->BackColor =RGB888(0,0,0);//������ɫ��RGB888��ɫ��ʽ)
//				cr->BorderColor =RGB888(255,0,0);//�߿���ɫ��RGB888��ɫ��ʽ)
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}    
    default:
      return	DefWindowProc(hwnd, msg, wParam, lParam);
  }
	return	WM_NULL;
}


void GUI_PicViewer_DIALOG(void)
{
	HWND	hwnd;
	WNDCLASS	wcex;
	MSG msg;


	wcex.Tag = WNDCLASS_TAG;

	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = PicViewer_proc; //������������Ϣ����Ļص�����.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);
   
	//����������
	hwnd = CreateWindowEx(WS_EX_NOFOCUS|WS_EX_FRAMEBUFFER,
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
