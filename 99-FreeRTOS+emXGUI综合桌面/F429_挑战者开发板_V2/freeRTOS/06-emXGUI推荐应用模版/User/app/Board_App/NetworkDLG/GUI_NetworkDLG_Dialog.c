#include <emXGUI.h>
#include <string.h>
#include <stdio.h>
#include "x_libc.h"
#include "GUI_AppDef.h"
#include "netconf.h"
#include "tcp_echoclient.h"
#include "tcp_echoserver.h"
#include "udp_echoclient.h"

/* ��ѡ�� ID */
#define ID_RB1    (0x1100 | (1<<16))
#define ID_RB2    (0x1101 | (1<<16))
#define ID_RB3    (0x1102 | (1<<16))


/* ���� ID */
#define ID_TEXTBOX_Title       0x00     // ������
#define ID_TEXTBOX_Send        0x01     // ������ʾ
#define ID_TEXTBOX_Receive     0x02     // ������ʾ
#define ID_TEXTBOX_RemoteIP1   0x07     // Զ��IP
#define ID_TEXTBOX_RemoteIP2   0x08     // Զ��IP
#define ID_TEXTBOX_RemoteIP3   0x09     // Զ��IP
#define ID_TEXTBOX_RemoteIP4   0x0A     // Զ��IP
#define ID_TEXTBOX_RemotePort  0x0B     // Զ�˶˿�

/* ��ť ID */
#define eID_Network_EXIT    0x03
#define eID_LINK_STATE      0x04
#define eID_Network_Send    0x05
#define eID_Receive_Clear   0x06

#define TitleHeight     70

int8_t NetworkTypeSelection = 0;

HWND Send_Handle;
HWND Receive_Handle;
static HWND Network_Main_Handle;

extern struct netif gnetif;
extern __IO uint8_t EthLinkStatus;
__IO uint32_t LocalTime = 0; /* this variable is used to create a time reference incremented by 10ms */
DRV_NETWORK drv_network;
uint16_t bsp_result=0;

/**
  * @brief  ͨ�ö�ʱ��3�жϳ�ʼ��
  * @param  period : �Զ���װֵ��
  * @param  prescaler : ʱ��Ԥ��Ƶ��
  * @retval ��
  * @note   ��ʱ�����ʱ����㷽��:Tout=((period+1)*(prescaler+1))/Ft us.
  *          Ft=��ʱ������Ƶ��,ΪSystemCoreClock/2=90,��λ:Mhz
  */
static void TIM3_Config(uint16_t period,uint16_t prescaler)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  ///ʹ��TIM3ʱ��
	
	TIM_TimeBaseInitStructure.TIM_Prescaler=prescaler;  //��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_Period=period;   //�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //����ʱ��3�����ж�
	TIM_Cmd(TIM3,ENABLE); //ʹ�ܶ�ʱ��3
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //��ʱ��3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

#ifdef   AAAA// ��Backend_vidoplayer.c��ʵ��
/**
  * @brief  ��ʱ��3�жϷ�����
  * @param  ��
  * @retval ��
  */
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //����ж�
	{
		LocalTime+=10;//10ms����
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //����жϱ�־λ
}
#endif
void Network_Dispose_Task(void *p) 
{
  static uint8_t startflag=0;

  if(startflag==0)
  {
    /* Configure ethernet (GPIOs, clocks, MAC, DMA) */
    if(ETH_BSP_Config()==1)
    {
      bsp_result |=1;
    }
    else
    {
      bsp_result &=~ 1;        
    }
    startflag=1;
  }
  if((drv_network.net_init==0)&&((bsp_result&1)==0))
  {     
    /* Initilaize the LwIP stack */
    LwIP_Init(); 

    drv_network.net_local_ip1=(uint8_t)(gnetif.ip_addr.addr&0xFF);
    drv_network.net_local_ip2=(uint8_t)((gnetif.ip_addr.addr>>8)&0xFF);
    drv_network.net_local_ip3=(uint8_t)((gnetif.ip_addr.addr>>16)&0xFF);
    drv_network.net_local_ip4=(uint8_t)((gnetif.ip_addr.addr>>24)&0xFF);
    drv_network.net_local_port=LOCAL_PORT;
    
    drv_network.net_remote_ip1=DEST_IP_ADDR0;
    drv_network.net_remote_ip2=DEST_IP_ADDR1;
    drv_network.net_remote_ip3=DEST_IP_ADDR2;
    drv_network.net_remote_ip4=DEST_IP_ADDR3;
    drv_network.net_remote_port=DEST_PORT;     
    
    drv_network.net_init=1;
  }

  //// WM_DeleteWindow(WM_GetClientWindow(htext));
  if(bsp_result&1)
  {		
    char str[30];
    if(startflag==2)
    {
      /* Configure ethernet (GPIOs, clocks, MAC, DMA) */
      if(ETH_BSP_Config()==1)
      {
        bsp_result |= 1;
        sprintf(str," ");  
      }
      else
      {
        bsp_result &=~ 1;    
        sprintf(str,"< must be restart Safari >"); 
      }
    }
    else
    {
      sprintf(str," ");  
    }
    startflag=2;
    // ErrorDialog(hWin,UTF8_NETWORKERROR,str);

  }
  // DROPDOWN_SetSel(WM_GetDialogItem(hWin, GUI_ID_DROPDOWN0),drv_network.net_type);
  // EDIT_SetValue(WM_GetDialogItem(hWin, GUI_ID_EDIT0),drv_network.net_remote_ip1);
  // EDIT_SetValue(WM_GetDialogItem(hWin, GUI_ID_EDIT1),drv_network.net_remote_ip2);
  // EDIT_SetValue(WM_GetDialogItem(hWin, GUI_ID_EDIT2),drv_network.net_remote_ip3);
  // EDIT_SetValue(WM_GetDialogItem(hWin, GUI_ID_EDIT3),drv_network.net_remote_ip4);
  // EDIT_SetValue(WM_GetDialogItem(hWin, GUI_ID_EDIT4),drv_network.net_remote_port);
  // WM_InvalidateWindow(hWin);
  InvalidateRect(Network_Main_Handle, NULL, TRUE);
  drv_network.net_connect=0;
  drv_network.net_type=0; 
  TIM3_Config(999,899);//10ms��ʱ�� 
  LocalTime=0;
  TIM_SetCounter(TIM3,0);
  EthLinkStatus=0;
  while(1)
  {
    // if(EthLinkStatus)
    // {
    //   ErrorDialog(hWin,UTF8_NETWORKERROR,UTF8_NETWORKERROR1);
    //   while(1)
    //   {
    //     if(tpad_flag)WM_DeleteWindow(hWin);
    //      if(!Flag_ICON6)return;
    //      GUI_Delay(10);
    //   }
    // }
    /* check if any packet received */
    if (ETH_CheckFrameReceived())
    { 
      /* process received ethernet packet */
      LwIP_Pkt_Handle();
    }
    /* handle periodic timers for LwIP */
    LwIP_Periodic_Handle(LocalTime);

    GUI_msleep(3);//WM_Exec();//
  }
}

//�˳���ť�ػ���
static void CollectVoltage_ExitButton_OwnerDraw(DRAWITEM_HDR *ds)
{
	HWND hwnd;
  HDC hdc;
	RECT rc;
 // RECT rc_top={0,0,800,70};
	WCHAR wbuf[128];

	hwnd = ds->hwnd; 
	hdc = ds->hDC;   
	rc = ds->rc; 

	SetBrushColor(hdc, MapRGB(hdc, COLOR_DESKTOP_BACK_GROUND));
   
  FillCircle(hdc, rc.x+rc.w, rc.y, rc.w);
	// //FillRect(hdc, &rc); //�þ�����䱳��

  if (ds->State & BST_PUSHED)
	{ //��ť�ǰ���״̬
		SetTextColor(hdc, MapRGB(hdc, 105, 105, 105));      //��������ɫ
	}
	else
	{ //��ť�ǵ���״̬

		SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));
	}

	  /* ʹ�ÿ���ͼ������ */
	SetFont(hdc, controlFont_64);

	GetWindowText(hwnd, wbuf, 128); //��ð�ť�ؼ�������
   rc.y = -10;
   rc.x = 16;
	DrawText(hdc, wbuf, -1, &rc, NULL);//��������(���ж��뷽ʽ)

  /* �ָ�Ĭ������ */
	SetFont(hdc, defaultFont);

}

/*
 * @brief  �ػ���ʾ���ȵ�͸���ı�
 * @param  ds:	�Զ�����ƽṹ��
 * @retval NONE
*/
static void Brigh_Textbox_OwnerDraw(DRAWITEM_HDR *ds) //����һ����ť���
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

//  BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, bk_hdc, rc_tmp.x, rc_tmp.y, SRCCOPY);
  SetTextColor(hdc, MapRGB(hdc, 250, 250, 250));
  rc.w -= 45;
  GetWindowText(hwnd, wbuf, 128); //��ð�ť�ؼ�������
  SetFont(hdc, defaultFont);
  DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);//��������(���ж��뷽ʽ)
}
extern void TCP_Echo_Init(void);

static LRESULT	CollectVoltage_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)
  {
    case WM_CREATE:
    {
      RECT rc;
      GetClientRect(hwnd, &rc); 
      HWND Temp_Handle;
      
      GUI_Thread_Create(Network_Dispose_Task,  /* ������ں��� */
                              "Network Dispose Task",/* �������� */
                              3*1024,  /* ����ջ��С */
                              NULL, /* ������ں������� */
                              5,    /* ��������ȼ� */
                              10); /* ����ʱ��Ƭ����������֧�� */

      CreateWindow(BUTTON, L"O", WS_TRANSPARENT|BS_FLAT | BS_NOTIFY |WS_OWNERDRAW|WS_VISIBLE,
                  730, 0, 70, 70, hwnd, eID_Network_EXIT, NULL, NULL); 

      rc.x = 5;
      rc.y = 35;
      rc.w = 160;
      rc.h = 30;
      CreateWindow(BUTTON,L"TCP Server",BS_RADIOBOX|WS_VISIBLE,
      rc.x,rc.y,rc.w,rc.h,hwnd,ID_RB1,NULL,NULL);
      
      OffsetRect(&rc, rc.w + 10, 0);
      CreateWindow(BUTTON,L"TCP Client",BS_RADIOBOX|WS_VISIBLE,
      rc.x,rc.y,rc.w,rc.h,hwnd,ID_RB2,NULL,NULL);

      //������2�鵥ѡ��ť(GroupID��Ϊ2,ʹ�ó��水ť���(BS_PUSHLIKE)).
      OffsetRect(&rc, rc.w + 10, 0);
      rc.w = 82;
      CreateWindow(BUTTON,L"UDP",BS_RADIOBOX|WS_VISIBLE,
      rc.x,rc.y,rc.w,rc.h,hwnd,ID_RB3,NULL,NULL);
      
      
      OffsetRect(&rc, rc.w + 10, 0);
      CreateWindow(BUTTON, L"δ����", WS_TRANSPARENT | BS_NOTIFY|WS_VISIBLE|BS_3D,
                  rc.x,rc.y,rc.w,rc.h, hwnd, eID_LINK_STATE, NULL, NULL); 
                  
      rc.w = GUI_XSIZE / 2;
      rc.h = TitleHeight-2;
      rc.x = GUI_XSIZE / 2 - rc.w / 2;
      rc.y = 0;

      // Title_Handle = CreateWindow(TEXTBOX, L"��ʪ����ʾ", WS_VISIBLE, rc.x, rc.y, rc.w, rc.h, hwnd, ID_TEXTBOX_Title, NULL, NULL);//
      // SendMessage(Title_Handle, TBM_SET_TEXTFLAG, 0, DT_VCENTER | DT_CENTER | DT_BKGND);   
      
      /* ���ݷ����ı����� */
      rc.w = GUI_XSIZE/2-10;
      rc.h = GUI_YSIZE/2-80;
      rc.x = GUI_XSIZE/2;
      rc.y = 70;
      Send_Handle = CreateWindow(TEXTBOX, L"��ã�������Ұ�𿪷��� ^_^", WS_VISIBLE, rc.x, rc.y, rc.w, rc.h, hwnd, ID_TEXTBOX_Send, NULL, NULL);//
      SendMessage(Send_Handle, TBM_SET_TEXTFLAG, 0, DT_VCENTER | DT_CENTER | DT_BKGND); 
      
      rc.x = GUI_XSIZE/2;
      rc.y = 70;
      rc.w = 80;
      rc.h = 30;
      OffsetRect(&rc, -rc.w-5, 0);
      CreateWindow(BUTTON, L"����", WS_TRANSPARENT | BS_NOTIFY|WS_VISIBLE|BS_3D,
                         rc.x,rc.y,rc.w,rc.h, hwnd, eID_Network_Send, NULL, NULL); 
                         
      rc.x = GUI_XSIZE/2;
      rc.h = 30;
      rc.w = 120;
      rc.y = 70+GUI_YSIZE/2-80-rc.h;
      OffsetRect(&rc, -rc.w-5, 0);
      CreateWindow(BUTTON, L"��ս���", WS_TRANSPARENT | BS_NOTIFY|WS_VISIBLE|BS_3D,
                         rc.x,rc.y,rc.w,rc.h, hwnd, eID_Receive_Clear, NULL, NULL); 
      
      rc.w = GUI_XSIZE - 20;
      rc.h = 220;
      rc.x = 10;
      rc.y = 240;
      Receive_Handle = CreateWindow(TEXTBOX, L"", WS_VISIBLE|WS_DISABLED, rc.x, rc.y, rc.w, rc.h, hwnd, ID_TEXTBOX_Receive, NULL, NULL);//
      SendMessage(Receive_Handle, TBM_SET_TEXTFLAG, 0, DT_LEFT | DT_TOP | DT_BKGND);   

      /* ���ݷ����ı����� */
      rc.w = 45;
      rc.h = 30;
      rc.x = 5;
      rc.y = 160;
      Temp_Handle = CreateWindow(TEXTBOX, L"192", WS_VISIBLE, rc.x, rc.y, rc.w, rc.h, hwnd, ID_TEXTBOX_RemoteIP1, NULL, NULL);//
      SendMessage(Temp_Handle, TBM_SET_TEXTFLAG, 0, DT_VCENTER | DT_CENTER | DT_BKGND);

      OffsetRect(&rc, rc.w+15, 0);
      Temp_Handle = CreateWindow(TEXTBOX, L"168", WS_VISIBLE, rc.x, rc.y, rc.w, rc.h, hwnd, ID_TEXTBOX_RemoteIP2, NULL, NULL);//
      SendMessage(Temp_Handle, TBM_SET_TEXTFLAG, 0, DT_VCENTER | DT_CENTER | DT_BKGND);

      OffsetRect(&rc, rc.w+15, 0);
      Temp_Handle = CreateWindow(TEXTBOX, L"000", WS_VISIBLE, rc.x, rc.y, rc.w, rc.h, hwnd, ID_TEXTBOX_RemoteIP3, NULL, NULL);//
      SendMessage(Temp_Handle, TBM_SET_TEXTFLAG, 0, DT_VCENTER | DT_CENTER | DT_BKGND);

      OffsetRect(&rc, rc.w+15, 0);
      Temp_Handle = CreateWindow(TEXTBOX, L"122", WS_VISIBLE, rc.x, rc.y, rc.w, rc.h, hwnd, ID_TEXTBOX_RemoteIP4, NULL, NULL);//
      SendMessage(Temp_Handle, TBM_SET_TEXTFLAG, 0, DT_VCENTER | DT_CENTER | DT_BKGND);

      OffsetRect(&rc, rc.w+15, 0);
      rc.w = 50;
      Temp_Handle = CreateWindow(TEXTBOX, L"5000", WS_VISIBLE, rc.x, rc.y, rc.w, rc.h, hwnd, ID_TEXTBOX_RemotePort, NULL, NULL);//
      SendMessage(Temp_Handle, TBM_SET_TEXTFLAG, 0, DT_VCENTER | DT_CENTER | DT_BKGND);
      
      // /* ����IP&�˿���ʾ�ı��� */
      // rc.x = 441;
      // RH_Handle = CreateWindow(TEXTBOX, L"66", WS_VISIBLE|WS_OWNERDRAW, rc.x, rc.y, rc.w, rc.h, hwnd, ID_TEXTBOX_RH, NULL, NULL);//
      // SendMessage(RH_Handle, TBM_SET_TEXTFLAG, 0, DT_VCENTER | DT_CENTER | DT_BKGND);   

      SetTimer(hwnd, 0, 2000, TMR_START, NULL);

      break;
    } 
    case WM_TIMER:
    {
      
      break;
    }

    case WM_PAINT:
    {
      HDC hdc;
      PAINTSTRUCT ps;
      WCHAR tempstr[30];
      
      RECT rc =  {0, 0, GUI_XSIZE, GUI_YSIZE};
      // hdc_pointer = CreateMemoryDC(SURF_SCREEN, PANEL_W, PANEL_H);
      hdc = BeginPaint(hwnd, &ps);
      
      SetBrushColor(hdc, MapRGB(hdc, 120, 120, 120));
      FillRect(hdc, &rc);
      
      SetFont(hdc, defaultFont);
      SetTextColor(hdc, MapRGB(hdc, 0x80, 0xFF, 0x80));

      rc.x = 5;
      rc.y = 5;
      rc.w = 160;
      rc.h = 30;
      DrawText(hdc, L"ͨѶЭ�飺", -1, &rc, DT_LEFT|DT_TOP);

      rc.x = 550;
      rc.y = 36;
      rc.w = 60;
      rc.h = 30;
      DrawText(hdc, L"���ݷ��ͣ�", -1, &rc, DT_LEFT|DT_TOP);
      
      rc.w = 400;
      rc.h = 30;
      rc.x = 5;
      rc.y = 65;
      DrawText(hdc, L"����IP��ַ&�˿ڣ�", -1, &rc, DT_LEFT|DT_TOP);
      
      SetTextColor(hdc, MapRGB(hdc, 200, 200, 200));
      x_wsprintf(tempstr, L"[%d.%d.%d.%d:%d]",drv_network.net_local_ip1,drv_network.net_local_ip2,\
                                       drv_network.net_local_ip3,drv_network.net_local_ip4,\
                                       drv_network.net_local_port);
      rc.w = 400;
      rc.h = 30;
      rc.x = 5;
      rc.y = 95;
      DrawText(hdc, tempstr, -1, &rc, DT_LEFT|DT_TOP);
      
      SetTextColor(hdc, MapRGB(hdc, 0x80, 0xFF, 0x80));

      rc.w = 120;
      rc.h = 30;
      rc.x = 10;
      rc.y = 210;
      DrawText(hdc, L"���ݽ��գ�", -1, &rc, DT_LEFT|DT_TOP);

      rc.w = 20;
      rc.h = 30;
      rc.x = 49;
      rc.y = 160;
      DrawText(hdc, L".", -1, &rc, DT_LEFT|DT_BOTTOM);
      
      rc.x = 110;
      DrawText(hdc, L".", -1, &rc, DT_LEFT|DT_BOTTOM);
      
      rc.x = 170;
      DrawText(hdc, L".", -1, &rc, DT_LEFT|DT_BOTTOM);
      
      rc.x = 229;
      DrawText(hdc, L":", -1, &rc, DT_LEFT|DT_BOTTOM);
      
      EndPaint(hwnd, &ps);
      break;
    }

    case WM_DRAWITEM:
    {
       DRAWITEM_HDR *ds;
       ds = (DRAWITEM_HDR*)lParam;
       switch(ds->ID)
       {
          case eID_Network_EXIT:
          {
            CollectVoltage_ExitButton_OwnerDraw(ds);
            return TRUE;             
          }  

          case ID_TEXTBOX_Send:
          case ID_TEXTBOX_Receive:
          {
            Brigh_Textbox_OwnerDraw(ds);
            return TRUE;   
          }
       }

       break;
    }
    case WM_NOTIFY:
    {
      u16 code, id;
      id  =LOWORD(wParam);//��ȡ��Ϣ��ID��
      code=HIWORD(wParam);//��ȡ��Ϣ������    
      if(code == BN_CLICKED && id == eID_Network_EXIT)
      {
        PostCloseMessage(hwnd);
        break;
      }
      if(code == BN_CLICKED && id == eID_Receive_Clear)
      {
        SetWindowText(Receive_Handle, L"");
        break;
      }
      
      if(code == BN_CLICKED && id == eID_LINK_STATE)
      {
        if((bsp_result&1)||EthLinkStatus)
          {
            break;
          }
          if(drv_network.net_connect==0)
          {
            uint8_t connectflag;
            
            drv_network.net_remote_ip1=192;//EDIT_GetValue(WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT0));
            drv_network.net_remote_ip2=168;//EDIT_GetValue(WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT1));
            drv_network.net_remote_ip3=0;//EDIT_GetValue(WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT2));
            drv_network.net_remote_ip4=122;//EDIT_GetValue(WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT3));
            drv_network.net_remote_port=8080;//EDIT_GetValue(WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT4));
            drv_network.net_type=NetworkTypeSelection;//DROPDOWN_GetSel(WM_GetDialogItem(pMsg->hWin, GUI_ID_DROPDOWN0));          
            switch(drv_network.net_type)
            {
              case 0:
                /*create tcp server */ 
                connectflag=tcp_echoserver_init(drv_network);
                break;
              case 1:
                /*connect to tcp server */
                connectflag=tcp_echoclient_connect(drv_network);
                break;
              case 2:
                /* Connect to tcp server */ 
                connectflag=udp_echoclient_connect(drv_network);		
                break;            
            }
            if(connectflag==0)
            {
              drv_network.net_connect=1; 
              SetWindowText(GetDlgItem(hwnd, eID_LINK_STATE), L"������");
            }      
          }
          else
          {
            SetWindowText(GetDlgItem(hwnd, eID_LINK_STATE), L"δ����");
            switch(drv_network.net_type)
            {
              case 0:
                tcp_echoserver_close();
                break;
              case 1:
                tcp_echoclient_disconnect();
                break;
              case 2:
                udp_echoclient_disconnect();	
                break;            
            }
            drv_network.net_connect=0;
          }
        }
        if(code == BN_CLICKED && id == eID_Network_Send)
        {
          if(drv_network.net_connect==1)          
          {
            WCHAR wbuf[128];
            char comdata[128];

            GetWindowText(GetDlgItem(hwnd, ID_TEXTBOX_Send), wbuf, 128);
            x_wcstombs_cp936(comdata, wbuf, 128);
            switch(drv_network.net_type)
            {
              case 0:
                network_tcpserver_send((char *)comdata);
                break;
              case 1:
                network_tcpclient_send((char *)comdata);
                break;
              case 2:
                udp_echoclient_send((char *)comdata);
                break;            
            }
          }
        }
      
        if( (id >= (ID_RB1 & ~(1<<16))) && (id <= (ID_RB3 & ~(1<<16))))
        {
          if (code == BN_CLICKED)
          {
            NetworkTypeSelection = id & 3;
            GUI_DEBUG("NetworkTypeSelection = %d", NetworkTypeSelection);
          }
        }

      break;
    } 

    case WM_DESTROY:
    {
      
      // DeleteDC(bk_hdc);
      return PostQuitMessage(hwnd);	
    } 

    default:
      return	DefWindowProc(hwnd, msg, wParam, lParam);   
  }
  
  return WM_NULL;
  
}

void GUI_NetworkDLG_Dialog(void)
{
	
	WNDCLASS	wcex;
	MSG msg;
	wcex.Tag = WNDCLASS_TAG;

	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = CollectVoltage_proc; //������������Ϣ����Ļص�����.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);
   
	//����������
	Network_Main_Handle = CreateWindowEx(WS_EX_NOFOCUS|WS_EX_FRAMEBUFFER,
                              &wcex,
                              L"GUI_ADC_CollectVoltage_Dialog",
                              WS_VISIBLE|WS_CLIPCHILDREN,
                              0, 0, GUI_XSIZE, GUI_YSIZE,
                              NULL, NULL, NULL, NULL);
   //��ʾ������
	ShowWindow(Network_Main_Handle, SW_SHOW);
	//��ʼ������Ϣѭ��(���ڹرղ�����ʱ,GetMessage������FALSE,�˳�����Ϣѭ��)��
	while (GetMessage(&msg, Network_Main_Handle))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}  
}


