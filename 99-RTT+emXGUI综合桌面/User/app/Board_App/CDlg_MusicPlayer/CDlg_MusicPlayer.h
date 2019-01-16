#ifndef _CDlg_MusicPlayer_H_
#define _CDlg_MusicPlayer_H_

#include <emXGUI.h>
#include "ExitButton.h"
enum CDlg_MusicPlayerID
{
   ID_BUTTON_Power = 0x1000,
   ID_BUTTON_List,
   ID_BUTTON_LRC,
   ID_BUTTON_BACK,
   ID_BUTTON_NEXT,
   ID_BUTTON_START,
   ID_BUTTON_EXIT,
   //������
   ID_SCROLLBAR_TIMER,
   ID_SCROLLBAR_POWER,
   //TEXTBOX
   ID_TEXTBOX_LRC1,
   ID_TEXTBOX_LRC2,
   ID_TEXTBOX_LRC3,
   ID_TEXTBOX_LRC4,
   ID_TEXTBOX_LRC5,
   
   ID_TEXTBOX_MUSICNAME,
   ID_TEXTBOX_CURTIME,
   ID_TEXTBOX_ALLTIME,
};

typedef struct{
   char *icon_name;//ͼ����
   RECT rc;        //λ����Ϣ
   BOOL state;     //״̬  
}icon_S;

typedef struct
{ 
   HWND h_MusicPlayer; //��������� 
   HWND h_MusicLrc;    //��ʾ��
   
   COLORREF back_col;  //������ɫ
   COLORREF bt_default_col;  //�ؼ������µ���ɫ
   COLORREF bt_pushed_col;  //�ؼ������µ���ɫ
   
   SCROLLINFO sif_time;//����������
   SCROLLINFO sif_power;//����������
   RECT rc_cli;        //����������λ�ô�С            
   int power;          //����
   BOOL power_state;   //�����仯��־
   
   ScrollBar_S ScrollBar_parm1;//�����������ṹ��
   ScrollBar_S ScrollBar_parm2;//�����������ṹ��
   icon_S *pCDlg_MusicPlayerICON; //ͼ���λ�ô�С
   void (* exit_bt_draw)(DRAWITEM_HDR*);
}CDlg_MusicPlayer_s;



extern void GUI_CDlg_MusicPlayer(void);

#endif /*  _CDlg_MusicPlayer_H_  */


