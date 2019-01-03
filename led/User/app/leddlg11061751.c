#include "emXGUI.h"
#include "x_libc.h"
#include "./led/bsp_led.h"  


#define PWM_LEDR_GPIO_PORT         GPIOH
#define PWM_LEDR_GPIO_PIN          GPIO_Pin_10
#define PWM_LEDR_GPIO_CLK          RCC_AHB1Periph_GPIOH
#define PWM_LEDR_PINSOURCE         GPIO_PinSource10
#define PWM_LEDR_AF                GPIO_AF_TIM5

#define PWM_LEDG_GPIO_PORT         GPIOH
#define PWM_LEDG_GPIO_PIN          GPIO_Pin_11
#define PWM_LEDG_GPIO_CLK          RCC_AHB1Periph_GPIOH
#define PWM_LEDG_PINSOURCE         GPIO_PinSource11
#define PWM_LEDG_AF                GPIO_AF_TIM5

#define PWM_LEDB_GPIO_PORT         GPIOH
#define PWM_LEDB_GPIO_PIN          GPIO_Pin_12
#define PWM_LEDB_GPIO_CLK          RCC_AHB1Periph_GPIOH
#define PWM_LEDB_PINSOURCE         GPIO_PinSource12
#define PWM_LEDB_AF                GPIO_AF_TIM5

#define PWM_LEDRGB_TIM             TIM5
#define PWM_LEDRGB_TIM_CLK         RCC_APB1Periph_TIM5

 /**
  * @brief  ����TIM3�������PWMʱ�õ���I/O
  * @param  ��
  * @retval ��
  */
static void TIM_GPIO_Config(void) 
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* GPIOB clock enable */
  RCC_AHB1PeriphClockCmd(PWM_LEDR_GPIO_CLK|PWM_LEDG_GPIO_CLK|PWM_LEDB_GPIO_CLK, ENABLE); 
  
  GPIO_PinAFConfig(PWM_LEDR_GPIO_PORT,PWM_LEDR_PINSOURCE,PWM_LEDR_AF); 
  GPIO_PinAFConfig(PWM_LEDG_GPIO_PORT,PWM_LEDG_PINSOURCE,PWM_LEDG_AF); 
  GPIO_PinAFConfig(PWM_LEDB_GPIO_PORT,PWM_LEDB_PINSOURCE,PWM_LEDB_AF); 
  
  /* ���ú������õ���PB0���� */
  GPIO_InitStructure.GPIO_Pin =  PWM_LEDR_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		    // �����������
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;   //100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 
  GPIO_Init(PWM_LEDR_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin =  PWM_LEDG_GPIO_PIN;
  GPIO_Init(PWM_LEDG_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin =  PWM_LEDB_GPIO_PIN;
  GPIO_Init(PWM_LEDB_GPIO_PORT, &GPIO_InitStructure);
}

/**
  * @brief  ����TIM3�����PWM�źŵ�ģʽ�������ڡ�����
  * @param  ��
  * @retval ��
  */
/*
 * TIMxCLK/CK_PSC --> TIMxCNT --> TIMx_ARR --> �ж� & TIMxCNT ���¼���
 *                    TIMx_CCR(��ƽ�����仯)
 *
 * �ź�����=(TIMx_ARR +1 ) * ʱ������
 * 
 */
/*    _______    ______     _____      ____       ___        __         _
 * |_|       |__|      |___|     |____|    |_____|   |______|  |_______| |________|
 */
static void TIM_Mode_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;																				
	
	/* ����TIM3CLK ʱ��Ϊ72MHZ */
   RCC_APB1PeriphClockCmd(PWM_LEDRGB_TIM_CLK, ENABLE); 					//ʹ��TIM5ʱ��

   /* ������ʱ������ */		 
   TIM_TimeBaseStructure.TIM_Period = 255;       							  //����ʱ����0������255����Ϊ266�Σ�Ϊһ����ʱ����
   TIM_TimeBaseStructure.TIM_Prescaler = 2499;	    							//����Ԥ��Ƶ��
   TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;			//����ʱ�ӷ�Ƶϵ��������Ƶ(�����ò���)
   TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  	//���ϼ���ģʽ
   TIM_TimeBaseInit(PWM_LEDRGB_TIM, &TIM_TimeBaseStructure);

   /* PWMģʽ���� */
   TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    				//����ΪPWMģʽ1
   TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//ʹ�����
   TIM_OCInitStructure.TIM_Pulse = 0;										  			//���ó�ʼPWM������Ϊ0	
   TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;  	  //����ʱ������ֵС��CCR1_ValʱΪ�͵�ƽ
 
   TIM_OC1Init(PWM_LEDRGB_TIM, &TIM_OCInitStructure);	 									//ʹ��ͨ��3
   TIM_OC1PreloadConfig(PWM_LEDRGB_TIM, TIM_OCPreload_Enable);						//ʹ��Ԥװ��	

   TIM_OC2Init(PWM_LEDRGB_TIM, &TIM_OCInitStructure);	 									//ʹ��ͨ��3
   TIM_OC2PreloadConfig(PWM_LEDRGB_TIM, TIM_OCPreload_Enable);						//ʹ��Ԥװ��	
  
   TIM_OC3Init(PWM_LEDRGB_TIM, &TIM_OCInitStructure);	 									//ʹ��ͨ��3
   TIM_OC3PreloadConfig(PWM_LEDRGB_TIM, TIM_OCPreload_Enable);						//ʹ��Ԥװ��	
  
   TIM_ARRPreloadConfig(PWM_LEDRGB_TIM, ENABLE);			 										//ʹ��TIM5���ؼĴ���ARR
 
   /* TIM5 enable counter */
   TIM_Cmd(PWM_LEDRGB_TIM, ENABLE);                   										//ʹ�ܶ�ʱ��5
	
}

//RGBLED��ʾ��ɫ
void SetRGBColor(uint32_t rgb)
{
	uint8_t r=0,g=0,b=0;
	r=(uint8_t)(rgb>>16);
	g=(uint8_t)(rgb>>8);
	b=(uint8_t)rgb;
	PWM_LEDRGB_TIM->CCR1 = r;	//����PWM���޸Ķ�ʱ���ıȽϼĴ���ֵ
	PWM_LEDRGB_TIM->CCR2 = g;	//����PWM���޸Ķ�ʱ���ıȽϼĴ���ֵ        
	PWM_LEDRGB_TIM->CCR3 = b;	//����PWM���޸Ķ�ʱ���ıȽϼĴ���ֵ
}

//RGBLED��ʾ��ɫ
void SetColorValue(uint8_t r,uint8_t g,uint8_t b)
{
	PWM_LEDRGB_TIM->CCR1 = r;	//����PWM���޸Ķ�ʱ���ıȽϼĴ���ֵ
	PWM_LEDRGB_TIM->CCR2 = g;	//����PWM���޸Ķ�ʱ���ıȽϼĴ���ֵ        
	PWM_LEDRGB_TIM->CCR3 = b;	//����PWM���޸Ķ�ʱ���ıȽϼĴ���ֵ
}

//ֹͣpwm���
void TIM_RGBLED_Close(void)
{
	SetColorValue(0,0,0);
	TIM_ForcedOC1Config(PWM_LEDRGB_TIM,TIM_ForcedAction_InActive);
	TIM_ForcedOC2Config(PWM_LEDRGB_TIM,TIM_ForcedAction_InActive);
	TIM_ForcedOC3Config(PWM_LEDRGB_TIM,TIM_ForcedAction_InActive);
	TIM_ARRPreloadConfig(PWM_LEDRGB_TIM, DISABLE);
	TIM_Cmd(PWM_LEDRGB_TIM, DISABLE);                   							//ʧ�ܶ�ʱ��3						
	RCC_APB1PeriphClockCmd(PWM_LEDRGB_TIM_CLK, DISABLE); 	//ʧ�ܶ�ʱ��3ʱ��
	LED_GPIO_Config();
}




/**********************�ֽ���*********************/
struct leddlg
{
	int col_R;  //R����ֵ
	int col_G;  //G����ֵ
	int col_B;  //B����ֵ
   int led_R;
   int led_G;
   int led_B;
   int colR_ctr;//���Ʋ���
   int colG_ctr;//���Ʋ���
   int colB_ctr;//���Ʋ���
}leddlg_S;
/*������ID�궨��*/
#define ID_SCROLLBAR_R  0x1100
#define ID_SCROLLBAR_G  0x1101
#define ID_SCROLLBAR_B  0x1102

/*��ѡ��ID�궨��*/
#define ID_TEXTBOX_R	      0x1010
#define ID_TEXTBOX_R_NUM	0x1013
#define ID_TEXTBOX_G	      0x1011
#define ID_TEXTBOX_G_NUM   0x1014
#define ID_TEXTBOX_B	      0x1012
#define ID_TEXTBOX_B_NUM   0x1015


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
	SetBrushColor(hdc, MapRGB888(hdc, back_c));
	FillRect(hdc, &rc);

//	/* ������ */
//	/* �߿� */
//	InflateRect(&rc, -rc.w >> 2, 0);
//	SetBrushColor(hdc, MapRGB(hdc, 169, 169, 169));
//	FillRoundRect(hdc, &rc, MIN(rc.w, rc.h) >> 1);

//	InflateRect(&rc, -2, -2);
//	SetBrushColor(hdc, MapRGB888(hdc, Page_c));
//	FillRoundRect(hdc, &rc, MIN(rc.w, rc.h) >> 1);
   rc_scrollbar.x = rc.w/2;
   rc_scrollbar.y = rc.y;
   rc_scrollbar.w = 2;
   rc_scrollbar.h = rc.h;
   
	SetBrushColor(hdc, MapRGB888(hdc, Page_c));
	FillRect(hdc, &rc_scrollbar);

	/* ���� */
	SendMessage(hwnd, SBM_GETTRACKRECT, 0, (LPARAM)&rc);

	SetBrushColor(hdc, MapRGB(hdc, 169, 169, 169));
	rc.x += (rc.w >> 2) >> 1;
	rc.w -= rc.w >> 2;
	/* �߿� */
	FillRoundRect(hdc, &rc, MIN(rc.w, rc.h) >> 2);
	InflateRect(&rc, -2, -2);

	SetBrushColor(hdc, MapRGB888(hdc, fore_c));
	FillRoundRect(hdc, &rc, MIN(rc.w, rc.h) >> 2);
}
/*
 * @brief  �Զ���ص�����
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
	draw_scrollbar(hwnd, hdc_mem1, RGB888(0, 0, 0), RGB888(250, 250, 250), RGB888(255, 255, 255));
	//������ɫ���͵Ĺ�����
	switch (ds->ID)
	{
		case ID_SCROLLBAR_R:
		{
			draw_scrollbar(hwnd, hdc_mem, RGB888(0, 0, 0), RGB888(leddlg_S.col_R, 0, 0), RGB888(leddlg_S.col_R, 0, 0));
			break;
		}
		case ID_SCROLLBAR_G:
		{
			draw_scrollbar(hwnd, hdc_mem, RGB888(0, 0, 0), RGB888(0, leddlg_S.col_G, 0), RGB888(0, leddlg_S.col_G, 0));
			break;
		}
		case ID_SCROLLBAR_B:
		{
			draw_scrollbar(hwnd, hdc_mem, RGB888(0, 0, 0), RGB888(0, 0, leddlg_S.col_B), RGB888(0, 0, leddlg_S.col_B));
			break;
		}
	}
   
   
   SendMessage(hwnd, SBM_GETTRACKRECT, 0, (LPARAM)&rc);
	//���ƻ���
	if (ds->State & SST_THUMBTRACK)//����
	{
		BitBlt(hdc, rc.x, 0, rc.w, rc_cli.h, hdc_mem, rc.x, 0, SRCCOPY);
	}
	else//δѡ��
	{
		BitBlt(hdc, rc.x, 0, rc.w, rc_cli.h, hdc_mem1, rc.x, 0, SRCCOPY);
	}	
	//��
	BitBlt(hdc, rc_cli.x, rc_cli.y, rc_cli.w, rc.y, hdc_mem, 0, 0, SRCCOPY);
	//��
	BitBlt(hdc, rc_cli.x, rc.y+rc.h, rc_cli.w , rc_cli.h-(rc.y+rc.h), hdc_mem1, 0, rc.y + rc.h, SRCCOPY);



	//�ͷ��ڴ�MemoryDC
	DeleteDC(hdc_mem1);
	DeleteDC(hdc_mem);
}

static	LRESULT	win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
//	RECT rc;
	HWND wnd;

	//static RECT rc_R, rc_G, rc_B;//RGB����ָʾ��

   //HDC hdc_mem2pic;
	switch (msg)
	{
	case WM_CREATE: {
		/*���û������Ĳ���*/
		SCROLLINFO sif;
      
		sif.cbSize = sizeof(sif);
		sif.fMask = SIF_ALL;
		sif.nMin = 0;
		sif.nMax = 255;
		sif.nValue = 0;
		sif.TrackSize = 40;
		sif.ArrowSize = 0;//20;

		/*����������--R*/
		wnd = CreateWindow(SCROLLBAR, L"SCROLLBAR_R", SBS_VERT|WS_OWNERDRAW | WS_VISIBLE, 215, 120, 50, 255, hwnd, ID_SCROLLBAR_R, NULL, NULL);
		SendMessage(wnd, SBM_SETSCROLLINFO, TRUE, (LPARAM)&sif);

		/*����������--G*/
		wnd = CreateWindow(SCROLLBAR, L"SCROLLBAR_G", SBS_VERT|WS_OWNERDRAW | WS_VISIBLE, 375, 120, 50, 255, hwnd, ID_SCROLLBAR_G, NULL, NULL);
		SendMessage(wnd, SBM_SETSCROLLINFO, TRUE, (LPARAM)&sif);

		/*����������--B*/
		wnd = CreateWindow(SCROLLBAR, L"SCROLLBAR_B", SBS_VERT|WS_OWNERDRAW | WS_VISIBLE, 535, 120, 50, 255, hwnd, ID_SCROLLBAR_B, NULL, NULL);
		SendMessage(wnd, SBM_SETSCROLLINFO, TRUE, (LPARAM)&sif);

 
//    /*�����ı���--R*/
		CreateWindow(TEXTBOX, L"R", WS_VISIBLE, 215, 80, 50, 40, hwnd, ID_TEXTBOX_R, NULL, NULL);
      SendMessage(GetDlgItem(hwnd, ID_TEXTBOX_R),TBM_SET_TEXTFLAG,0,
                     DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_BKGND); 
      //R�ķ���ֵ               
		CreateWindow(TEXTBOX, L"0", WS_VISIBLE, 215, 375, 60, 40, hwnd, ID_TEXTBOX_R_NUM, NULL, NULL);
      SendMessage(GetDlgItem(hwnd, ID_TEXTBOX_R_NUM),TBM_SET_TEXTFLAG,0,
                     DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_BKGND);                      
		/*������ѡ��--G(on/off)*/
		CreateWindow(TEXTBOX, L"G", WS_VISIBLE, 375, 80, 50, 40, hwnd, ID_TEXTBOX_G, NULL, NULL);
      SendMessage(GetDlgItem(hwnd, ID_TEXTBOX_G),TBM_SET_TEXTFLAG,0,
                     DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_BKGND);   
      //G�ķ���ֵ               
		CreateWindow(TEXTBOX, L"0", WS_VISIBLE, 375, 375, 60, 40, hwnd, ID_TEXTBOX_G_NUM, NULL, NULL);
      SendMessage(GetDlgItem(hwnd, ID_TEXTBOX_G_NUM),TBM_SET_TEXTFLAG,0,
                     DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_BKGND);                       
		/*������ѡ��--B(on/off)*/
		CreateWindow(TEXTBOX, L"B", WS_VISIBLE, 535, 80, 50, 40, hwnd, ID_TEXTBOX_B, NULL, NULL);     
      SendMessage(GetDlgItem(hwnd, ID_TEXTBOX_B),TBM_SET_TEXTFLAG,0,
                     DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_BKGND);
      //BG�ķ���ֵ               
		CreateWindow(TEXTBOX, L"0", WS_VISIBLE, 535, 375, 60, 40, hwnd, ID_TEXTBOX_B_NUM, NULL, NULL);
      SendMessage(GetDlgItem(hwnd, ID_TEXTBOX_B_NUM),TBM_SET_TEXTFLAG,0,
                     DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_BKGND);      

      break;
	}
   case WM_LBUTTONDOWN:
   {
      POINT pt;
      pt.x =GET_LPARAM_X(lParam); //���X����
      pt.y =GET_LPARAM_Y(lParam); //���Y����
      RECT rc = {720, 0, 80, 80};
      if(PtInRect(&rc, &pt))
      {
         PostCloseMessage(hwnd);
         //����WM_CLOSE��Ϣ�ر�������
      }
      break;         
   }   
	case WM_NOTIFY: {
		NMHDR *nr;
      WCHAR wbuf[128];
		u16 ctr_id; 
		ctr_id = LOWORD(wParam); //wParam��16λ�Ƿ��͸���Ϣ�Ŀؼ�ID.
		nr = (NMHDR*)lParam; //lParam����������NMHDR�ṹ�忪ͷ.
		if (ctr_id == ID_SCROLLBAR_R)
		{
			NM_SCROLLBAR *sb_nr;		
			sb_nr = (NM_SCROLLBAR*)nr; //Scrollbar��֪ͨ��Ϣʵ��Ϊ NM_SCROLLBAR��չ�ṹ,���渽���˸������Ϣ.
			switch (nr->code)
			{
				case SBN_THUMBTRACK: //R�����ƶ�
				{
					leddlg_S.col_R = sb_nr->nTrackValue; //��û��鵱ǰλ��ֵ
					SendMessage(nr->hwndFrom, SBM_SETVALUE, TRUE, leddlg_S.col_R); //����λ��ֵ
               x_wsprintf(wbuf, L"%d", leddlg_S.col_R);
               SetWindowText(GetDlgItem(hwnd, ID_TEXTBOX_R_NUM), wbuf);
				}
				break;
			}
		}
		if (ctr_id == ID_SCROLLBAR_G)
		{
			NM_SCROLLBAR *sb_nr;
			sb_nr = (NM_SCROLLBAR*)nr; //Scrollbar��֪ͨ��Ϣʵ��Ϊ NM_SCROLLBAR��չ�ṹ,���渽���˸������Ϣ.
			switch (nr->code)
			{
				case SBN_THUMBTRACK: //G�����ƶ�
				{
					leddlg_S.col_G = sb_nr->nTrackValue; //��û��鵱ǰλ��ֵ
					SendMessage(nr->hwndFrom, SBM_SETVALUE, TRUE, leddlg_S.col_G); //����λ��ֵ
               x_wsprintf(wbuf, L"%d", leddlg_S.col_G);
               SetWindowText(GetDlgItem(hwnd, ID_TEXTBOX_G_NUM), wbuf);
				}
				break;
			}
		}

		if (ctr_id == ID_SCROLLBAR_B)
		{
			NM_SCROLLBAR *sb_nr;
			sb_nr = (NM_SCROLLBAR*)nr; //Scrollbar��֪ͨ��Ϣʵ��Ϊ NM_SCROLLBAR��չ�ṹ,���渽���˸������Ϣ.
			switch (nr->code)
			{
            case SBN_THUMBTRACK: //B�����ƶ�
            {
               leddlg_S.col_B = sb_nr->nTrackValue; //���B���鵱ǰλ��ֵ
               SendMessage(nr->hwndFrom, SBM_SETVALUE, TRUE, leddlg_S.col_B); //����B�����λ��
               x_wsprintf(wbuf, L"%d", leddlg_S.col_B);
               SetWindowText(GetDlgItem(hwnd, ID_TEXTBOX_B_NUM), wbuf);
            }
            break;
			}
		}
  
      leddlg_S.led_R=(leddlg_S.colR_ctr != 0)? 0:leddlg_S.col_R;
      leddlg_S.led_G=(leddlg_S.colG_ctr != 0)? 0:leddlg_S.col_G;
      leddlg_S.led_B=(leddlg_S.colB_ctr != 0)? 0:leddlg_S.col_B;
      SetColorValue(leddlg_S.led_R, leddlg_S.led_G, leddlg_S.led_B);
		break;
	}
	case WM_DRAWITEM:
	{
		DRAWITEM_HDR *ds;
		ds = (DRAWITEM_HDR*)lParam;
		if(ds->ID == ID_SCROLLBAR_R || ds->ID == ID_SCROLLBAR_G || ds->ID == ID_SCROLLBAR_B)
		{
			scrollbar_owner_draw(ds);
			return TRUE;
		}

	}
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
      CTLCOLOR *cr;
      cr =(CTLCOLOR*)lParam;
      if(id== ID_TEXTBOX_R_NUM || id== ID_TEXTBOX_G_NUM || id== ID_TEXTBOX_B_NUM)
      {

         cr->TextColor =RGB888(255,255,255);//������ɫ��RGB888��ɫ��ʽ)
         cr->BackColor =RGB888(0,0,0);//������ɫ��RGB888��ɫ��ʽ)
         cr->BorderColor =RGB888(255,0,0);//�߿���ɫ��RGB888��ɫ��ʽ)
         return TRUE;
      }
      switch(id)
      {
         case ID_TEXTBOX_R:
         {
            cr->TextColor =RGB888(255,0,0);//������ɫ��RGB888��ɫ��ʽ)
            cr->BackColor =RGB888(0,0,0);//������ɫ��RGB888��ɫ��ʽ)
            cr->BorderColor =RGB888(255,0,0);//�߿���ɫ��RGB888��ɫ��ʽ)            
            break;
         }
         case ID_TEXTBOX_G:
         {
            cr->TextColor =RGB888(0,255,0);//������ɫ��RGB888��ɫ��ʽ)
            cr->BackColor =RGB888(0,0,0);//������ɫ��RGB888��ɫ��ʽ)
            cr->BorderColor =RGB888(255,0,0);//�߿���ɫ��RGB888��ɫ��ʽ)              
            break;
         }
         case ID_TEXTBOX_B:
         {
            cr->TextColor =RGB888(0,0,255);//������ɫ��RGB888��ɫ��ʽ)
            cr->BackColor =RGB888(0,0,0);//������ɫ��RGB888��ɫ��ʽ)
            cr->BorderColor =RGB888(255,0,0);//�߿���ɫ��RGB888��ɫ��ʽ)                       
            break;
         }
         case ID_TEXTBOX_R_NUM:
         {
            cr->TextColor =RGB888(255,255,255);//������ɫ��RGB888��ɫ��ʽ)
            cr->BackColor =RGB888(0,0,0);//������ɫ��RGB888��ɫ��ʽ)
            cr->BorderColor =RGB888(255,0,0);//�߿���ɫ��RGB888��ɫ��ʽ)            
         }
         case ID_TEXTBOX_G_NUM:
         {
            cr->TextColor =RGB888(255,255,255);//������ɫ��RGB888��ɫ��ʽ)
            cr->BackColor =RGB888(0,0,0);//������ɫ��RGB888��ɫ��ʽ)
            cr->BorderColor =RGB888(255,0,0);//�߿���ɫ��RGB888��ɫ��ʽ)          
         }
         case ID_TEXTBOX_B_NUM:
         {
            cr->TextColor =RGB888(255,255,255);//������ɫ��RGB888��ɫ��ʽ)
            cr->BackColor =RGB888(0,0,0);//������ɫ��RGB888��ɫ��ʽ)
            cr->BorderColor =RGB888(255,0,0);//�߿���ɫ��RGB888��ɫ��ʽ)     
            break;
         }
         default:
            return FALSE;
         
      }
      return TRUE;
      
   }   
   case WM_ERASEBKGND:
   {
      HDC hdc =(HDC)wParam;
      RECT rc;
      GetClientRect(hwnd, &rc);
      
      SetBrushColor(hdc, MapRGB(hdc, 0, 0, 0));
      FillRect(hdc, &rc);
      
      
      return TRUE;
      
   }
/*�����ֿ���ɫָʾ�򣬸�ѡ�򣨿��أ�����ɫ����ֵ���������ؼ���ɣ�
 *λ�ù�ϵ��
 *	  ���ֿ�     ��ɫ      ��ɫ			��
 *										      ��
 *	  ��ѡ��    ָʾ��	    ����ֵ	   ��
*/ 
	case	WM_PAINT: //������Ҫ�ػ���ʱ�����Զ��յ�����Ϣ.
	{	
      PAINTSTRUCT ps;
      HDC hdc, hdc_mem, hdc_mem1;//��Ļhdc
      RECT rc = {0,0,72,72};
      RECT rc_cli = {0,0,72,72};
      GetClientRect(hwnd, &rc_cli);
      hdc = BeginPaint(hwnd, &ps); 
      hdc_mem = CreateMemoryDC(SURF_SCREEN, 72, 72);
      hdc_mem1 = CreateMemoryDC(SURF_SCREEN, 72, 72);
      
      
      /****************���������水ť******************/
      SetBrushColor(hdc, MapRGB(hdc, 0,0,0));
      FillCircle(hdc, rc_cli.w, 0, 80);  
      
      SetBrushColor(hdc, MapRGB(hdc, 250,0,0));
      FillCircle(hdc, rc_cli.w, 0, 76); 
      //�����
      SetBrushColor(hdc_mem1, MapRGB(hdc, 250,0,0));
      FillRect(hdc_mem1, &rc);        
      
//      SetFont(hdc_mem1, hFont_SDCARD);
      SetTextColor(hdc_mem1, MapRGB(hdc_mem1, 250, 250,250));
      TextOut(hdc_mem1, 0, 0, L"O", -1);

      StretchBlt(hdc, 755, 12, 40, 40, 
                 hdc_mem1, 0, 0, 72, 72, SRCCOPY);

      DeleteDC(hdc_mem);
      DeleteDC(hdc_mem1);
		EndPaint(hwnd, &ps);
		return	TRUE;
	}
	default:
		return	DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return	WM_NULL;
}


void	GUI_LED_DIALOG(void)
{
	HWND	hwnd;
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
   //��ʼ����ʱ��
   //LED_GPIO_Config();
   TIM_GPIO_Config();
   TIM_Mode_Config();
   leddlg_S.colR_ctr = 0;
   leddlg_S.colG_ctr = 0;
   leddlg_S.colB_ctr = 0;
   //�ر�PWM���
// TIM_ForcedOC1Config(PWM_LEDRGB_TIM,TIM_ForcedAction_InActive);
//	TIM_ForcedOC2Config(PWM_LEDRGB_TIM,TIM_ForcedAction_InActive);
//	TIM_ForcedOC3Config(PWM_LEDRGB_TIM,TIM_ForcedAction_InActive);
	//����������
	hwnd = CreateWindowEx(NULL,
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
