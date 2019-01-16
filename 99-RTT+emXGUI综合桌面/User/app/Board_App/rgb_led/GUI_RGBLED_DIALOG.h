#ifndef _GUI_RGBLED_DIALOG_H_
#define _GUI_RGBLED_DIALOG_H_

#include <emXGUI.h>
//��������ɫ����
#define ID_TEXTBOX_Title_BackColor   RGB888(0,0,0)          //������ɫ
#define ID_TEXTBOX_Title_TextColor   RGB888(255,255,255)    //������ɫ


enum GUI_RGBLEDID
{
   /*������ID�궨��*/
   ID_SCROLLBAR_R = 0x1100,
   ID_SCROLLBAR_G  ,
   ID_SCROLLBAR_B ,

   /*�ı���ID�궨��*/
   ID_TEXTBOX_Title,
   ID_TEXTBOX_APP,
   ID_TEXTBOX_R,
   ID_TEXTBOX_R_LED,   
   ID_TEXTBOX_R_NUM,
   ID_TEXTBOX_G,	 
   ID_TEXTBOX_G_LED,   
   ID_TEXTBOX_G_NUM,  
   ID_TEXTBOX_B,
   ID_TEXTBOX_B_LED,   
   ID_TEXTBOX_B_NUM,   
   /*��ťID�궨��*/
   ID_EXIT,  
   ID_CHECKBOX_SW, 
   
};
typedef struct
{
   HWND RGBLED_Hwnd;//���ھ��
   BOOL State;      //Ӳ��RGB�ƵĿ���
   
	SCROLLINFO sif_R;//��ɫ����ֵ
   SCROLLINFO sif_G;//��ɫ����ֵ
   SCROLLINFO sif_B;//��ɫ����ֵ
   
   int col_R;       //��ɫ������ʼֵ
   int col_G;       //��ɫ������ʼֵ
   int col_B;       //��ɫ������ʼֵ
   void (* exit_bt_draw)(DRAWITEM_HDR*);//�����˳���ť�ص�����
}RGBLED_DIALOG_s;
#endif /* _GUI_RGBLED_DIALOG_H_ */


