#include "emXGUI.h"
#include "./mp3_player/Backend_mp3Player.h"
#include "./mp3_player/Backend_musiclist.h"
#include "x_libc.h"
#include "./mp3_player/GUI_MUSICPLAYER_DIALOG.h"
#include	"CListMenu.h"
#include "GUI_AppDef.h"
/******************����IDֵ********************/
#define ID_BUTTON_EXIT 0x2000  
/******************�б�IDֵ********************/
#define ID_LISTBOX1    0x2100
#define ID_LISTBOX2    0x2101

#define ID_LIST_1             0x2200
#define ICON_VIEWER_ID_PREV   0x2201
#define ICON_VIEWER_ID_NEXT   0x2202

/**********************����****************************/
char music_playlist[MUSIC_MAX_NUM][FILE_NAME_LEN];//����List
char music_lcdlist[MUSIC_MAX_NUM][MUSIC_NAME_LEN];//��ʾlist
uint8_t  music_file_num = 0;//�ļ�����
int play_index = 0;   //���Ÿ����ı��ֵ


static BITMAP bm;//λͼ�ṹ��

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
//LIST
static HDC hdc_mem; //�ڴ�DC�����ڻ�������
static HDC hdc_pic; //�ڴ�DC����������ͼƬ����Ϣ
static void _listbox_owner_draw_x(DRAWITEM_HDR *ds)
{
	HWND hwnd;
	HDC hdc;
	RECT rc;//List����Ŀλ����Ϣ
   RECT rc_cli;//List�ؼ���λ����Ϣ
	int i, count;//
   int item = 0;//��Ŀ���
	hwnd = ds->hwnd;
	hdc = ds->hDC;	
   RECT rc1;//��Ŀ���λ�ü���С
	WCHAR wbuf[128];
   //��ȡ�ؼ���λ����Ϣ
	GetClientRect(hwnd, &rc_cli);
   HFONT font_old;
   //���ñ�����ɫ����ɫ��
	SetBrushColor(hdc_mem, MapRGB(hdc_mem, 0, 0, 0));
   //��䱳��
	FillRect(hdc_mem, &rc_cli);
				
	//��ȡ��ǰ�ĵ�һ��
	i = SendMessage(hwnd, LB_GETTOPINDEX, 0, 0);
  
	//��ȡ�б���Ŀ����
	count = SendMessage(hwnd, LB_GETCOUNT, 0, 0);
   
	while (i < count)
	{
      //����������ɫ����ɫ��
     //SetTextColor(hdc_mem, MapRGB(hdc_mem, 250, 250, 250));
      
      switch(ds->ID)
      {
         case ID_LISTBOX1:
         {   
            item = 2*i;
            if(item == play_index)
            {
               SetTextColor(hdc_mem, MapRGB(hdc_mem, 250, 0, 0));
            }
            else
            {
               SetTextColor(hdc_mem, MapRGB(hdc_mem, 255, 255, 255));
            }
            
            break;
         }
         case ID_LISTBOX2:
         {
            item = 2*i+1;        
            if(item == play_index)
            {
               SetTextColor(hdc_mem, MapRGB(hdc_mem, 250, 0, 0));
            }
            else
            {
               SetTextColor(hdc_mem, MapRGB(hdc_mem, 255, 255, 255));
            }   
                                       
            break;
         }
      
      }      
      
      //��ȡ��Ŀ��λ����Ϣ
		SendMessage(hwnd, LB_GETITEMRECT, i, (LPARAM)&rc);
      font_old = SetFont(hdc_mem, controlFont_48);
      
		//BitBlt(hdc_mem, 50, (rc.y + 2), 72, 58, hdc_pic, 0, 0, SRCCOPY);//����ͼ��
      rc1.x = 50;
      rc1.y = (rc.y + 2);
      rc1.w = 72;
      rc1.h = 58;

      DrawText(hdc_mem, L"X", -1, &rc1, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
      SetFont(hdc_mem, font_old);
      switch(ds->ID)
      {
         case ID_LISTBOX1:
         {   
            item = 2*i;
            if(item == play_index)
            {
               SetTextColor(hdc_mem, MapRGB(hdc_mem, 250, 0, 0));
            }
            else
            {
               SetTextColor(hdc_mem, MapRGB(hdc_mem, 255, 255, 255));
            }
            
            break;
         }
         case ID_LISTBOX2:
         {
            item = 2*i+1;        
            if(item == play_index)
            {
               SetTextColor(hdc_mem, MapRGB(hdc_mem, 250, 0, 0));
            }
            else
            {
               SetTextColor(hdc_mem, MapRGB(hdc_mem, 255, 255, 255));
            }   
                                       
            break;
         }
      
      }
      //������ŵ�λ��
		rc1.x = rc.x + 5;
		rc1.y = rc.y + 2;
		rc1.w = 40;
		rc1.h = 58;
		if (rc.y > rc_cli.h)//����list�ĸߣ����˳��������л���
		{
			break;
		}
      //��ȡ��Ŀi���ı�����
		SendMessage(hwnd, LB_GETTEXT, i, (LPARAM)wbuf);
      //�����ı�����
		DrawText(hdc_mem, wbuf, -1, &rc1, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
      //���ø������ֵ�λ����Ϣ
  		rc1.x = 50+72+5;
		rc1.y = rc.y;          
		rc1.w = 200;  
      SetTextColor(hdc_mem, MapRGB(hdc_mem, 255, 255, 255));
      //����List��IDֵ������������Ŀ�����������music_lcdlist��������ݾ����ģ�
      //�����ĸ���������LIST1
      //˫���ĸ���������LIST2
      switch(ds->ID)
      {
         case ID_LISTBOX1:
         {   
            item = 2*i;
            if(item == play_index)
            {
               SetTextColor(hdc_mem, MapRGB(hdc_mem, 250, 0, 0));
            }
            else
            {
               SetTextColor(hdc_mem, MapRGB(hdc_mem, 255, 255, 255));
            }
            
                        
            x_mbstowcs_cp936(wbuf, music_lcdlist[item], FILE_NAME_LEN);            
                       
            DrawText(hdc_mem, wbuf, 7, &rc1, DT_VCENTER);
            
            break;
         }
         case ID_LISTBOX2:
         {
            item = 2*i+1;        
            if(item == play_index)
            {
               SetTextColor(hdc_mem, MapRGB(hdc_mem, 250, 0, 0));
            }
            else
            {
               SetTextColor(hdc_mem, MapRGB(hdc_mem, 255, 255, 255));
            }   
                    
            x_mbstowcs_cp936(wbuf, music_lcdlist[item], FILE_NAME_LEN);
            DrawText(hdc_mem, wbuf, -1, &rc1, DT_SINGLELINE| DT_VCENTER);                       
            break;
         }
      
      }
      i++;
	}
	
	BitBlt(hdc, 0, 0, rc_cli.w, rc_cli.h, hdc_mem, 0, 0, SRCCOPY);
}



static LRESULT win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   static HWND wnd_list1;//��߸���list�ľ��
   static HWND wnd_list2;//�ұ߸���list�ľ��
   switch(msg)
   {
      case WM_CREATE:
      { 
//         int i;
//         int item1=0, item2=0;//��¼�б���ID���
//         int item_tmp=0;
//         HDC hdc_tmp;
//         HWND wnd_tmp;
//         WCHAR wbuff[128];
//         RECT rc_tmp ={ 0,0,72,58 };//
//         RECT rc_button_exit = {5, 5, 100, 72};//���ؼ�λ��
//         CreateWindow(BUTTON,L"FYZ",WS_OWNERDRAW |WS_VISIBLE,
//                      rc_button_exit.x, rc_button_exit.y, 
//                      rc_button_exit.w, rc_button_exit.h,
//                      hwnd,ID_BUTTON_EXIT,NULL,NULL);
//  
//         wnd_list1 = CreateWindow(LISTBOX, _T("Listbox"), WS_OWNERDRAW |  LBS_LINE | LBS_NOTIFY | WS_VISIBLE,
//            0, 80, 400, 400, hwnd, ID_LISTBOX1, NULL, NULL);
//         wnd_list2 = CreateWindow(LISTBOX, _T("Listbox"), WS_OWNERDRAW | LBS_LINE | LBS_NOTIFY | WS_VISIBLE ,
//            400, 80, 400, 400, hwnd, ID_LISTBOX2, NULL, NULL);         
//         //�����ļ�������������һ����˫���ŵڶ���
//         if(music_file_num)
//         {
//            for(i = 0; i < music_file_num; i++)
//            {
//               wnd_tmp = (i%2==0)? wnd_list1:wnd_list2;
//               item_tmp = (i%2==0)? item1:item2;
//               x_wsprintf(wbuff, L"%d",i+1);
//               SendMessage(wnd_tmp, LB_ADDSTRING, item_tmp, (LPARAM)wbuff);
//               if(wnd_tmp == wnd_list1) item1++;
//               if(wnd_tmp == wnd_list2) item2++;
//            }
//            
//         }
//         for(i = 0; i < item1; i++)
//            SendMessage(wnd_list1, LB_SETITEMHEIGHT, i, 58);
//         for (i = 0; i < item2; i++)
//            SendMessage(wnd_list2, LB_SETITEMHEIGHT, i, 58);   
//         //����λͼ�ṹ����
////         bm.Format = BM_RGB888;     //λͼ��ʽ
////         bm.Width = 72;              //���
////         bm.Height = 58;             //�߶�
////         bm.WidthBytes = bm.Width * 3; //ÿ���ֽ���
////         bm.LUT = NULL;                //���ұ�(RGB/ARGB��ʽ��ʹ�øò���)
////         bm.Bits = (void*)timg;    //λͼ����

//         hdc_mem = CreateMemoryDC(SURF_SCREEN, 800, 480);
//         //hdc_pic = CreateMemoryDC(SURF_SCREEN, bm.Width, bm.Height);
////         DrawBitmap(hdc_pic, 0, 0, &bm, NULL);
////         //���´���Ϊ��Բ�ε�pic       
////         hdc_tmp = CreateMemoryDC(SURF_ARGB4444, bm.Width, bm.Height);
////         
////         SetBrushColor(hdc_tmp, MapARGB(hdc_tmp, 255, 0, 0, 0));
////         FillRect(hdc_tmp, &rc_tmp);

////         SetBrushColor(hdc_tmp, MapARGB(hdc_tmp, 0, 0, 0, 0));
////         FillCircle(hdc_tmp, 36, 29, 25);

////         BitBlt(hdc_pic, 0, 0, 72, 58, hdc_tmp, 0, 0, SRCCOPY);
//         //�ͷŻ���DC,������ͼ
////         DeleteDC(hdc_tmp);            
//         
         break;
      }
      case WM_PAINT:
      {
         PAINTSTRUCT ps;
         HDC hdc;
         RECT rc;//���ڴ�С
         GetClientRect(hwnd, &rc); //��ÿͻ�������.
         hdc = BeginPaint(hwnd, &ps);
         //����
         SetBrushColor(hdc, MapRGB(hdc, 0,0,0));
         FillRect(hdc, &rc);  
         //DrawBitmap(hdc,0,0,&bm_0,NULL);   
         rc.x = 0;
         rc.y = 0;
         rc.w = 800;
         rc.h = 80;
         SetTextColor(hdc, MapRGB(hdc, 250, 250, 250));
         DrawText(hdc, L"�����б�", -1, &rc, DT_VCENTER|DT_CENTER);
         EndPaint(hwnd, &ps);
         break;
      }
		case WM_NOTIFY: //WM_NOTIFY��Ϣ:wParam��16λΪ���͸���Ϣ�Ŀؼ�ID,��16λΪ֪ͨ��;lParamָ����һ��NMHDR�ṹ��.
		{
			u16 code,id;

			code =HIWORD(wParam); //���֪ͨ������.
			id   =LOWORD(wParam); //��ò�������Ϣ�Ŀؼ�ID.

			if(id==ID_BUTTON_EXIT && code==BN_CLICKED) // ��ť����������.
			{
				PostCloseMessage(hwnd); //ʹ����WM_CLOSE��Ϣ�رմ���.
			}
         //��ȡ�б�������Ϣ
         NMHDR *nr;        
         nr = (NMHDR*)lParam;
         switch(nr->code)
         {
            case LBN_CLICKED:
            {
               switch(nr->idFrom)
               {
                  case ID_LISTBOX1:
                  {
                     int i = 0;
                     i = SendMessage(nr->hwndFrom, LB_GETCURSEL, 0, 0);
                     play_index = 2*i;//�л�����һ��
                     mp3player.ucStatus = STA_SWITCH;	
                     InvalidateRect(wnd_list1, NULL, TRUE);            
                     InvalidateRect(wnd_list2, NULL, TRUE);
                     break;
                  }//end of case ID_LISTBOX1
                  case ID_LISTBOX2:
                  {
                     int i = 0;
                     i = SendMessage(nr->hwndFrom, LB_GETCURSEL, 0, 0);
                     play_index = 2*i+1;//�л�����һ��
                     mp3player.ucStatus = STA_SWITCH;		
                     InvalidateRect(wnd_list1, NULL, TRUE);
                     InvalidateRect(wnd_list2, NULL, TRUE);                     
                        
                  }//end of case ID_LISTBOX2
               }//end of switch (nr->idFrom)
               break;
            }//end of case LBN_CLICKED
            case LBN_POSCHANGE:
            {
               switch (nr->idFrom)
               {
                  static int flag_A = 0;
                  static int flag_B = 0;
                  
                  case ID_LISTBOX1:
                  {
                     int i;
                     flag_A = 1;
                     if (flag_B == 0 && flag_A == 1 )
                     {
                        i = SendMessage(nr->hwndFrom, LB_GETPOS, 0, 0);
                        SendMessage(wnd_list2, LB_SETPOS, 0, i);
                     }// end of if (flag_B == 0 && flag_A == 1 )
                     else if(flag_B == 1)
                     {
                        i = SendMessage(wnd_list1, LB_GETPOS, 0, 0);
                       
                        flag_B = 0;		
                        flag_A = 0;
                        InvalidateRect(wnd_list1, NULL, TRUE);
                     }//end of else if(flag_B == 1)
                     break;
                  }//end of case ID_LISTBOX1
                  case ID_LISTBOX2:
                  {
                     int t;
                     flag_B = 1;
                     if (flag_A == 0 && flag_B == 1) {
                        t = SendMessage(nr->hwndFrom, LB_GETPOS, 0, 0);
                        SendMessage(wnd_list1, LB_SETPOS, 0, t);
                     }//end of if (flag_A == 0 && flag_B == 1)
                     else if(flag_A == 1)
                     {
                        t = SendMessage(nr->hwndFrom, LB_GETPOS, 0, 0);
                        InvalidateRect(wnd_list2, NULL, TRUE);
                        flag_A = 0;	
                        flag_B = 0;
                     }//end of else if(flag_A == 1)
                     break;
                  }//end of case ID_LISTBOX2
               default:
                  break;
               }               
               
               
               break;
            }
         
         }
         break;
      }         
      //�Զ�����ƿؼ�
      case WM_DRAWITEM:
      {
         DRAWITEM_HDR *ds;
         ds = (DRAWITEM_HDR*)lParam;
         if (ds->ID == ID_BUTTON_EXIT)
         {
            button_owner_draw(ds);
            return TRUE;
         }
         if(ds->ID == ID_LISTBOX1 || wParam == ID_LISTBOX2)
         {
            _listbox_owner_draw_x(ds);
            return TRUE;
         }
      }      
      case WM_CLOSE: //���ڹر�ʱ�����Զ���������Ϣ.
		{
         DeleteDC(hdc_mem);//�ͷ�
         //DeleteDC(hdc_pic);
         enter_flag = 0;
         SetForegroundWindow(MusicPlayer_hwnd);//����ǰ̨����ΪMusicPlayer_hwnd������Ļ��ᴥ���ػ�
			return DestroyWindow(hwnd); //����DestroyWindow�������ٴ��ڣ��ú�����ʹ�����ڽ������˳���Ϣѭ��;���򴰿ڽ���������.
		}
      default:
         return DefWindowProc(hwnd, msg, wParam, lParam);
      
   }
   return WM_NULL;
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
         menu_list = (struct __obj_list *)GUI_VMEM_Alloc(sizeof(struct __obj_list)*(music_file_num+1));
         wbuf = (WCHAR (*)[128])GUI_VMEM_Alloc(sizeof(WCHAR *)*music_file_num);
         if(menu_list == NULL) 
            return 0;
         for(;i < music_file_num; i++){
            //printf("%s\n", lcdlist[i]);
            x_mbstowcs_cp936(wbuf[i], music_lcdlist[i], FILE_NAME_LEN);
            menu_list[i].pName = wbuf[i];
            menu_list[i].cbStartup = NULL;
            menu_list[i].icon = L"a";
            menu_list[i].bmp = NULL;
            menu_list[i].color = RGB_WHITE;
         } 
          /* ���һ��Ϊ�� */
          menu_list[i].pName = NULL;
          menu_list[i].cbStartup = NULL;
          menu_list[i].icon = NULL;
          menu_list[i].bmp = NULL;
          menu_list[i].color = NULL;         
         
         cfg.list_objs = menu_list; 
         cfg.x_num = 3;
         cfg.y_num = 2; 
         cfg.bg_color = 0x363636;
         wnd = CreateWindow(&wcex_ListMenu,
                      L"ListMenu1",
                      WS_VISIBLE | LMS_ICONFRAME|LMS_PAGEMOVE,
                      rc.x + 100, rc.y + 80, rc.w - 200, rc.h-80,
                      hwnd,
                      ID_LIST_1,
                      NULL,
                      &cfg);         
         
         wnd= CreateWindow(BUTTON, L"L", BS_FLAT | BS_NOTIFY | WS_OWNERDRAW |WS_VISIBLE,
                        0, rc.h * 1 / 2, 70, 70, hwnd, ICON_VIEWER_ID_PREV, NULL, NULL);
         SetWindowFont(wnd, controlFont_48); 
	      wnd = CreateWindow(BUTTON, L"K", BS_FLAT | BS_NOTIFY | WS_OWNERDRAW | WS_VISIBLE,
			rc.w - 65, rc.h * 1 / 2, 70, 70, hwnd, ICON_VIEWER_ID_NEXT, NULL, NULL);
         SetWindowFont(wnd, controlFont_48);    
         
         break;
      } 
      case WM_DRAWITEM:
      {

         DRAWITEM_HDR *ds;

         ds = (DRAWITEM_HDR*)lParam;

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
         SetBrushColor(hdc, MapRGB(hdc, 0,0,0));
         FillRect(hdc, &rc);  
         //DrawBitmap(hdc,0,0,&bm_0,NULL);   
         rc.x = 0;
         rc.y = 0;
         rc.w = 800;
         rc.h = 80;
         SetTextColor(hdc, MapRGB(hdc, 250, 250, 250));
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

         if (code == LMN_CLICKED)
         {
            switch (id)
            {
               case ID_LIST_1:{
                  
//                  Play_index = nm->idx;
//                  sw_flag = 1;
                  PostCloseMessage(hwnd); //����WM_CLOSE��Ϣ�ر�������
                  //menu_list_1[nm->idx].cbStartup(hwnd);
               }

               break;
            }

         }

         
         if (code == BN_CLICKED && id == ICON_VIEWER_ID_PREV)
         {
            SendMessage(GetDlgItem(hwnd, ID_LIST_1), MSG_MOVE_PREV, TRUE, 0);
         }
         ////
         if (code == BN_CLICKED && id == ICON_VIEWER_ID_NEXT)
         {
            SendMessage(GetDlgItem(hwnd, ID_LIST_1), MSG_MOVE_NEXT, TRUE, 0);
         }
         break;
      }      
      case WM_CLOSE: //���ڹر�ʱ�����Զ���������Ϣ.
		{         
         GUI_VMEM_Free(menu_list);
         GUI_VMEM_Free(wbuf);
			return DestroyWindow(hwnd); //����DestroyWindow�������ٴ��ڣ��ú�����ʹ�����ڽ������˳���Ϣѭ��;���򴰿ڽ���������.
		}      
      default:
         return DefWindowProc(hwnd, msg, wParam, lParam); 
   }

} 


void GUI_MusicList_DIALOG(void)
{
	HWND	hwnd;
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
	hwnd = CreateWindowEx(WS_EX_NOFOCUS,
                         &wcex,
                         L"GUI_MusicList_DIALOG",
                         WS_CLIPSIBLINGS,
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
