#ifndef _GUI_MUSICPLAYER_DIALOG_H
#define _GUI_MUSICPLAYER_DIALOG_H
/*****************�������ؼ�IDֵ*********************/
#define ID_SCROLLBAR_POWER   0x1104   //������
#define ID_SCROLLBAR_TIMER   0x1105   //������

/******************��ť�ؼ�IDֵ***********************/
#define ID_BUTTON_Power      0x1000   //���� 
#define ID_BUTTON_List       0x1001   //����List
#define ID_BUTTON_Back       0x1002   //��һ��
#define ID_BUTTON_Play       0x1003   //����/��ͣ
#define ID_BUTTON_Next       0x1004   //����/��ͣ
typedef struct{
   char *icon_name;//ͼ����
   RECT rc;        //λ����Ϣ
   BOOL state;     //״̬  
}icon_S;

extern int avi_chl;
extern volatile int win_fps;
#endif

