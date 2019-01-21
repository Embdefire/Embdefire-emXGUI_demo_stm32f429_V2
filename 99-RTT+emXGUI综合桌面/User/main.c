/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   RT-Thread 3.0 + STM32F429 + emxgui demo
  *********************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F429-��ս�� STM32 ������
  * ����    :www.embedfire.com
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  **********************************************************************
  */ 
 
/*
*************************************************************************
*                             ������ͷ�ļ�
*************************************************************************
*/ 
#include "board.h"
#include "rtthread.h"


/*
*************************************************************************
*                               ����
*************************************************************************
*/
/* �����߳̿��ƿ� */
static rt_thread_t gui_thread = RT_NULL;
static rt_thread_t gui_boot_thread = RT_NULL;
//static rt_thread_t get_cpu_use_thread = RT_NULL;

/*
*************************************************************************
*                             ��������
*************************************************************************
*/
static void gui_thread_entry(void* parameter);
static void gui_boot_entry(void* parameter);
rt_sem_t GUI_BOOT_SEM;
extern void	GUI_Boot_Interface_DIALOG(void);

/*
*************************************************************************
*                             main ����
*************************************************************************
*/
/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{	
    /* 
	 * ������Ӳ����ʼ����RTTϵͳ��ʼ���Ѿ���main����֮ǰ��ɣ�
	 * ����component.c�ļ��е�rtthread_startup()����������ˡ�
	 * ������main�����У�ֻ��Ҫ�����̺߳������̼߳��ɡ�
	 */
	
	gui_thread =                          /* �߳̿��ƿ�ָ�� */
    rt_thread_create( "gui",              /* �߳����� */
                      gui_thread_entry,   /* �߳���ں��� */
                      RT_NULL,             /* �߳���ں������� */
                      2048,                 /* �߳�ջ��С */
                      3,                   /* �̵߳����ȼ����������ȼ�Խ���߼����ȼ�ԽС */
                      1);                 /* �߳�ʱ��Ƭ */
 
	gui_boot_thread =                          /* �߳̿��ƿ�ָ�� */
    rt_thread_create( "gui_boot",              /* �߳����� */
                      gui_boot_entry,   /* �߳���ں��� */
                      RT_NULL,             /* �߳���ں������� */
                      2048,                 /* �߳�ջ��С */
                      3,                   /* �̵߳����ȼ����������ȼ�Խ���߼����ȼ�ԽС */
                      1);                 /* �߳�ʱ��Ƭ */  
   GUI_BOOT_SEM = rt_sem_create(NULL,1,RT_IPC_FLAG_FIFO);                      
    /* �����̣߳��������� */
   if (gui_thread != RT_NULL)
        rt_thread_startup(gui_thread);
    else
        return -1;

   if (gui_boot_thread != RT_NULL)
        rt_thread_startup(gui_boot_thread);
    else
        return -1;
    
//   get_cpu_use_thread =                          /* �߳̿��ƿ�ָ�� */
//    rt_thread_create( "get_cpu_use",              /* �߳����� */
//                      get_cpu_use_thread_entry,   /* �߳���ں��� */
//                      RT_NULL,             /* �߳���ں������� */
//                      512,                 /* �߳�ջ��С */
//                      5,                   /* �̵߳����ȼ� */
//                      20);                 /* �߳�ʱ��Ƭ */
//                   
//    /* �����̣߳��������� */
//   if (get_cpu_use_thread != RT_NULL)
//        rt_thread_startup(get_cpu_use_thread);
//    else
//        return -1; 
}

/*
*************************************************************************
*                             �̶߳���
*************************************************************************
*/
extern void GUI_Startup(void);

static void gui_thread_entry(void* parameter)
{	 
   rt_sem_take(GUI_BOOT_SEM, 0xFFFFFFFF);
  /* ִ�б��������᷵�� */
	GUI_Startup();
	
  while (1)
  {
    LED1_ON;
    rt_thread_delay(500);   /* ��ʱ500��tick */
    rt_kprintf("gui_thread running,LED1_ON\r\n");
    
    LED1_OFF;     
    rt_thread_delay(500);   /* ��ʱ500��tick */		 		
    rt_kprintf("gui_thread running,LED1_OFF\r\n");
  }
}

//  rt_uint8_t major,minor;

static void gui_boot_entry(void* parameter)
{	

  while (1)
  {
      rt_sem_take(GUI_BOOT_SEM, 0xFFFFFFFF);
      rt_kprintf("�����ɹ�\r\n");
      GUI_Boot_Interface_DIALOG();
  }
}

/********************************END OF FILE****************************/
