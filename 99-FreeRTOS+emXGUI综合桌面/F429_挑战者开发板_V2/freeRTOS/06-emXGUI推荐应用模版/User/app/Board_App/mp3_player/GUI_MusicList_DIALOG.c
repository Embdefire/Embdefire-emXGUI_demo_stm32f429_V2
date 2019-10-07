#include "emXGUI.h"
#include "./mp3_player/Backend_mp3Player.h"
#include "./mp3_player/Backend_musiclist.h"
#include "x_libc.h"
#include "./mp3_player/GUI_MUSICPLAYER_DIALOG.h"
#include	"CListMenu.h"
#include "GUI_AppDef.h"
#include "string.h"

/******************����IDֵ********************/
#define ID_BUTTON_EXIT 0x2000  
/******************�б�IDֵ********************/
#define ID_LISTBOX1    0x2100
#define ID_LISTBOX2    0x2101


#define ID_LIST_1             0x2200
//#define ICON_VIEWER_ID_PREV   0x2201
//#define ICON_VIEWER_ID_NEXT   0x2202

#define ID_EXIT        0x3000

HWND music_list_hwnd;

/**********************����****************************/
char music_playlist[MUSIC_MAX_NUM][FILE_NAME_LEN] __EXRAM;//����List
char music_lcdlist[MUSIC_MAX_NUM][MUSIC_NAME_LEN] __EXRAM;//��ʾlist
uint8_t  music_file_num = 0;//�ļ�����
int play_index = 0;   //���Ÿ����ı��ֵ


//static BITMAP bm;//λͼ�ṹ��

/*******************�ؼ��ػ����************************/
//����
static void button_owner_draw(DRAWITEM_HDR *ds) //����һ����ť���
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	WCHAR wbuf[128];

	hwnd = ds->hwnd; //button�Ĵ��ھ��.
	hdc = ds->hDC;   //button�Ļ�ͼ�����ľ��.
	rc = ds->rc;     //button�Ļ��ƾ�����.

	SetBrushColor(hdc, MapRGB(hdc, 0,0,0));
	FillRect(hdc, &rc); //�þ�����䱳��

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
	SetFont(hdc, controlFont_64);
	//  SetTextColor(hdc,MapRGB(hdc,255,255,255));

	GetWindowText(ds->hwnd, wbuf, 128); //��ð�ť�ؼ�������

	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER | DT_CENTER);//��������(���ж��뷽ʽ)


  /* �ָ�Ĭ������ */
	SetFont(hdc, defaultFont);

}
static void exit_owner_draw(DRAWITEM_HDR *ds) //����һ����ť���
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	WCHAR wbuf[128];

	hwnd = ds->hwnd; //button�Ĵ��ھ��.
	hdc = ds->hDC;   //button�Ļ�ͼ�����ľ��.
	rc = ds->rc;     //button�Ļ��ƾ�����.

   
   
   
	SetBrushColor(hdc, MapRGB(hdc, 0,0,0));
   
   FillCircle(hdc, rc.x, rc.y, rc.w);
   SetBrushColor(hdc, MapRGB(hdc, 0,0,0));
	FillRect(hdc, &rc); //�þ�����䱳��

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
	SetFont(hdc, controlFont_48);
	//  SetTextColor(hdc,MapRGB(hdc,255,255,255));

	GetWindowText(ds->hwnd, wbuf, 128); //��ð�ť�ؼ�������

	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER);//��������(���ж��뷽ʽ)
   rc.x = 35; 
//   rc.y = 20;
  /* �ָ�Ĭ������ */
	SetFont(hdc, defaultFont);
   DrawText(hdc, L"����", -1, &rc, DT_VCENTER);
}
//LIST

/*
 * @brief  �ػ��б�
 * @param  ds:	�Զ�����ƽṹ��
 * @retval NONE
*/
static void listbox_owner_draw(DRAWITEM_HDR *ds)
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	int i,count,cursel;
	WCHAR wbuf[128];
  WCHAR Time[40];
  WCHAR *Temp;
	POINT pt;

	hwnd =ds->hwnd;
	hdc =ds->hDC;

//	hdc =CreateMemoryDC(SURF_ARGB4444,ds->rc.w,ds->rc.h); //����һ���ڴ�DC����ͼ.

	rc =ds->rc;

	SetBrushColor(hdc,MapRGB(hdc,83,98,181));
	FillRect(hdc,&ds->rc);
  
  if (!SendMessage(hwnd,LB_GETCOUNT,0,0))
  {
    /* �б�Ϊ�գ���ʾ��ʾ��ϢȻ��ֱ�ӷ��� */
    DrawText(hdc, L"û���ҵ������ļ�\r\n����SD����", -1, &rc, DT_CENTER|DT_VCENTER);
    return;
  }

	i=SendMessage(hwnd,LB_GETTOPINDEX,0,0);
	count=SendMessage(hwnd,LB_GETCOUNT,0,0);
	cursel=SendMessage(hwnd,LB_GETCURSEL,0,0);

	while(i<count)
	{
		SendMessage(hwnd,LB_GETITEMRECT,i,(LPARAM)&rc);
		if(rc.y > ds->rc.h)
		{
			break;
		}

		SetTextColor(hdc,MapRGB(hdc,50,10,10));

		if(i==cursel)
		{
			SetTextColor(hdc,MapRGB(hdc,2,167,240));
		}
		else
		{
			SetTextColor(hdc,MapRGB(hdc,255,255,255));
		}

		SendMessage(hwnd,LB_GETTEXT,i,(LPARAM)wbuf);
    
		DrawText(hdc,wbuf,-1,&rc,DT_SINGLELINE|DT_LEFT|DT_VCENTER);      // ��ʾ�绰����
    
    SetPenColor(hdc,MapRGB(hdc,10,10,10));
    HLine(hdc, rc.x, rc.y + rc.h - 1, rc.x + rc.w);                  // ��һ����

		i++;
	}
//	BitBlt(ds->hDC,0,0,ds->rc.w,ds->rc.h,hdc,0,0,SRCCOPY); //���ڴ�DC���������������(DC)��.
//	DeleteDC(hdc);
}


static LRESULT Win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   static struct __obj_list *menu_list = NULL;
   static WCHAR (*wbuf)[128];
   switch(msg)
   {
      case WM_CREATE:
      {
         
         HWND wnd;
         int i = 0;
         list_menu_cfg_t cfg;
	   	   RECT rc;
         GetClientRect(hwnd, &rc);
         /* ��Ҫ����N+1����һ��Ϊ�� */
//         menu_list = (struct __obj_list *)GUI_VMEM_Alloc(sizeof(struct __obj_list)*(music_file_num+1));
//         wbuf = (WCHAR (*)[128])GUI_VMEM_Alloc(sizeof(WCHAR *)*music_file_num);
//         if(menu_list == NULL) 
//            return 0;
//         for(;i < music_file_num; i++){
//            //printf("%s\n", lcdlist[i]);
//            
//            
//         char p[128] ;
//         strcpy(p, music_lcdlist[i]);
//         //printf("%s\n",p);
//         int t, L;
//         L = (int)strlen(p);
//         if (L > 13)
//         {
//            for (t = L; t > 13; t --)
//            {
//               p[t] = p[t - 1];
//            }
//            p[13] = '\0';
//            p[L + 1] = '\0';
//         }            
//            
//            
//            x_mbstowcs_cp936(wbuf[i], p, FILE_NAME_LEN);
//            menu_list[i].pName = wbuf[i];
//            menu_list[i].cbStartup = NULL;
//            menu_list[i].icon = L"a";
//            menu_list[i].bmp = NULL;
//            menu_list[i].color = RGB_WHITE;
//         } 
//          /* ���һ��Ϊ�� */
//          menu_list[i].pName = NULL;
//          menu_list[i].cbStartup = NULL;
//          menu_list[i].icon = NULL;
//          menu_list[i].bmp = NULL;
//          menu_list[i].color = NULL;         
//         
//         cfg.list_objs = menu_list; 
//         cfg.x_num = 3;
//         cfg.y_num = 2; 
//         cfg.bg_color = 0;
         wnd = CreateWindow(LISTBOX,
                      L"ListMenu1",
                      WS_VISIBLE | WS_OWNERDRAW | LBS_NOTIFY,
                      10, 36, 359, 294,
                      hwnd,
                      ID_LIST_1,
                      NULL,
                      NULL);         
         SendMessage(wnd, MSG_SET_SEL, play_index, 0);
         WCHAR wbuf1[128];
         for (uint16_t xC=0; xC<music_file_num; xC++)
         {
           x_mbstowcs_cp936(wbuf1, music_lcdlist[xC], FILE_NAME_LEN);
           SendMessage(wnd, LB_ADDSTRING, -1, (LPARAM)wbuf1);
         }
      //    wnd= CreateWindow(BUTTON, L"L", BS_FLAT | BS_NOTIFY | WS_OWNERDRAW |WS_VISIBLE,
      //                   0, rc.h * 1 / 2, 70, 70, hwnd, ICON_VIEWER_ID_PREV, NULL, NULL);
      //    SetWindowFont(wnd, controlFont_48); 
	   //    wnd = CreateWindow(BUTTON, L"K", BS_FLAT | BS_NOTIFY | WS_OWNERDRAW | WS_VISIBLE,
      //   rc.w - 65, rc.h * 1 / 2, 70, 70, hwnd, ICON_VIEWER_ID_NEXT, NULL, NULL);
      //    SetWindowFont(wnd, controlFont_48);    
         
      //    CreateWindow(BUTTON, L"F", BS_FLAT | BS_NOTIFY|WS_OWNERDRAW |WS_VISIBLE,
      //                   0, 0, 240, 80, hwnd, ID_EXIT, NULL, NULL);         
         
         
         break;
      } 
      case WM_DRAWITEM:
      {

         DRAWITEM_HDR *ds;

         ds = (DRAWITEM_HDR*)lParam;
  
         if (ds->ID == ID_LIST_1)
         {
           listbox_owner_draw(ds);
           return TRUE;
         }
         
         if(ds->ID == ID_EXIT)
            exit_owner_draw(ds);
         else
            button_owner_draw(ds); //ִ���Ի��ư�ť
         return TRUE;

      }    
      case WM_PAINT:
      {
         PAINTSTRUCT ps;
         HDC hdc;
         RECT rc;//���ڴ�С
         GetClientRect(hwnd, &rc); //��ÿͻ�������.
         hdc = BeginPaint(hwnd, &ps);
         //����
         SetBrushColor(hdc, MapRGB(hdc, 83,98,181));
         FillRect(hdc, &rc);  
         //DrawBitmap(hdc,0,0,&bm_0,NULL);   
         rc.x = 0;
         rc.y = 0;
         rc.w = 379;
         rc.h = 35;
         SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));
         DrawText(hdc, L"�����б�", -1, &rc, DT_VCENTER|DT_CENTER);

         EndPaint(hwnd, &ps);
         break;
      }  
      case WM_NOTIFY:
      {
         u16 code, id;	
         LM_NMHDR *nm;
         code = HIWORD(wParam);
         id = LOWORD(wParam); 

         nm = (LM_NMHDR*)lParam;

         if (code == LBN_SELCHANGE)
         {
            switch (id)
            {
               case ID_LIST_1:
                 {
                     play_index = SendMessage(GetDlgItem(hwnd, ID_LIST_1), LB_GETCURSEL,0,0);               // ��õ�ǰѡ����;//�л�����һ��
                     mp3player.ucStatus = STA_SWITCH;	                  
//                  Play_index = ;
//                  sw_flag = 1;
                  //PostCloseMessage(hwnd); //����WM_CLOSE��Ϣ�ر�������
                  //menu_list_1[nm->idx].cbStartup(hwnd);
               }

               break;
            }

         }

      
//         if (code == BN_CLICKED && id == ICON_VIEWER_ID_PREV)
//         {
//            SendMessage(GetDlgItem(hwnd, ID_LIST_1), MSG_MOVE_PREV, TRUE, 0);
//         }
//         ////
//         if (code == BN_CLICKED && id == ICON_VIEWER_ID_NEXT)
//         {
//            SendMessage(GetDlgItem(hwnd, ID_LIST_1), MSG_MOVE_NEXT, TRUE, 0);
//         }         
//         if (code == BN_CLICKED && id == ID_EXIT)
//         {
//           
//            PostCloseMessage(hwnd);
//         }   
         break;
      }      
      case WM_CLOSE: //���ڹر�ʱ�����Զ���������Ϣ.
		{         
//         GUI_VMEM_Free(menu_list);
//         GUI_VMEM_Free(wbuf);
         enter_flag = 0;
         SetForegroundWindow(MusicPlayer_hwnd);
			return DestroyWindow(hwnd); //����DestroyWindow�������ٴ��ڣ��ú�����ʹ�����ڽ������˳���Ϣѭ��;���򴰿ڽ���������.
		} 
    //�رմ�����Ϣ����case
      case WM_DESTROY:
      {               
        return PostQuitMessage(hwnd);	    // �˳���Ϣѭ��
      }
    
      default:
         return DefWindowProc(hwnd, msg, wParam, lParam); 
   }
   return WM_NULL;
} 


void GUI_MusicList_DIALOG(HWND hwnd)
{
	WNDCLASS	wcex;
	MSG msg;

	wcex.Tag = WNDCLASS_TAG;
	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = Win_proc; //������������Ϣ����Ļص�����.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);

	//����������
	music_list_hwnd = CreateWindow(//WS_EX_NOFOCUS|WS_EX_FRAMEBUFFER,//|WS_EX_LOCKZORDER
                         &wcex,
                         L"GUI_MusicList_DIALOG",
                         WS_CLIPSIBLINGS,
                         421, 56, 379, 335,
                         hwnd, NULL, NULL, NULL);
	//��ʾ������
//	ShowWindow(music_list_hwnd, SW_SHOW);
//  SetForegroundWindow(music_list_hwnd);
	//��ʼ������Ϣѭ��(���ڹرղ�����ʱ,GetMessage������FALSE,�˳�����Ϣѭ��)��
//	while (GetMessage(&msg, music_list_hwnd))
//	{
//		TranslateMessage(&msg);
//		DispatchMessage(&msg);
//	}
  //UpdateWindow(MusicPlayer_hwnd);
}
