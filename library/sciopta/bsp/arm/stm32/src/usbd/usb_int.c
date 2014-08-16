/*SOC
**********************************************************************
**  _______  _______ _________ _______  _______ _________ _______   **
** (  ____ \(  ____ \\__   __/(  ___  )(  ____ )\__   __/(  ___  )  **
** | (    \/| (    \/   ) (   | (   ) || (    )|   ) (   | (   ) |  **
** | (_____ | |         | |   | |   | || (____)|   | |   | (___) |  **
** (_____  )| |         | |   | |   | ||  _____)   | |   |  ___  |  **
**       ) || |         | |   | |   | || (         | |   | (   ) |  **
** /\____) || (____/\___) (___| (___) || )         | |   | )   ( |  **
** \_______)(_______/\_______/(_______)|/          )_(   |/     \|  **
**                                                                  **
**        (c) 2009 Sciopta Systems GmbH, Sciopta AG/ Schweiz        **
**                                                                  **
**********************************************************************
** ID: S09155BS1                                                    **
** +Revision: 1.1.2.1 +                                                 **
** +Date: 2009/08/20 09:01:22 +                                     **
** USBD EP interrupt handling                                       **
**********************************************************************
EOC*/

/******************** (C) COPYRIGHT 2009 STMicroelectronics ********************
* File Name          : usb_int.c
* Author             : MCD Application Team
* Version            : V3.0.1
* Date               : 04/27/2009
* Description        : Endpoint CTR (Low and High) interrupt's service routines
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/
#include <sciopta.h>
#include <string.h>
#include "usbd.msg"
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "misc.h"

#include "usb_lib.h"
#include "usb_istr.h"

/* Private typedef -----------------------------------------------------------*/
union sc_msg {
  sc_msgid_t id;
  usb_setup_req_t  setup_req;
  usb_ctrlPacket_t ctrl;
  usb_setDesc_t    setDesc;
  usb_epConfig_t   epConfig;
  usb_epComplete_t epComplete;
  usb_epReset_t    epReset;
  usb_sendData_t   sendData;
  usb_recvData_t   recvData;
  usb_suspend_t    suspend;
};

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

static sc_msg_t txMsg[8];
static sc_msg_t rxMsg[8];
/* Globale variables ---------------------------------------------------------*/

uint16_t SaveRState;
uint16_t SaveTState;

/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : CTR_LP.
* Description    : Low priority Endpoint Correct Transfer interrupt's service
*                  routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void CTR_LP(void)
{
  uint16_t wIstr;
  uint8_t EPindex;
  uint32_t wEPVal = 0;
  /* stay in loop while pending ints */
  while (((wIstr = _GetISTR()) & ISTR_CTR) != 0)
  {
    _SetISTR((uint16_t)CLR_CTR); /* clear CTR flag */
    /* extract highest priority endpoint number */
    EPindex = (uint8_t)(wIstr & ISTR_EP_ID);
    if (EPindex == 0)
    {
      /* Decode and service control endpoint interrupt */
      /* calling related service routine */
      /* (Setup0_Process, In0_Process, Out0_Process) */

      /* save RX & TX status */
      /* and set both to NAK */
      SaveRState = _GetEPRxStatus(ENDP0);
      SaveTState = _GetEPTxStatus(ENDP0);
      _SetEPRxStatus(ENDP0, EP_RX_NAK);
      _SetEPTxStatus(ENDP0, EP_TX_NAK);


      /* DIR bit = origin of the interrupt */

      if ((wIstr & ISTR_DIR) == 0)
      {
        /* DIR = 0 */

        /* DIR = 0      => IN  int */
        /* DIR = 0 implies that (EP_CTR_TX = 1) always  */


        _ClearEP_CTR_TX(ENDP0);
        In0_Process();

           /* before terminate set Tx & Rx status */
          _SetEPRxStatus(ENDP0, SaveRState);
          _SetEPTxStatus(ENDP0, SaveTState);
          return;
      }
      else
      {
        /* DIR = 1 */

        /* DIR = 1 & CTR_RX       => SETUP or OUT int */
        /* DIR = 1 & (CTR_TX | CTR_RX) => 2 int pending */

        wEPVal = _GetENDPOINT(ENDP0);
        if ((wEPVal & EP_CTR_TX) != 0)
        {
          _ClearEP_CTR_TX(ENDP0);
          In0_Process();
          /* before terminate set Tx & Rx status */
          _SetEPRxStatus(ENDP0, SaveRState);
          _SetEPTxStatus(ENDP0, SaveTState);
          return;
        }
        else if ((wEPVal &EP_SETUP) != 0)
        {
          _ClearEP_CTR_RX(ENDP0); /* SETUP bit kept frozen while CTR_RX = 1 */
          Setup0_Process();
          /* before terminate set Tx & Rx status */
          _SetEPRxStatus(ENDP0, SaveRState);
          _SetEPTxStatus(ENDP0, SaveTState);
          return;
        }

        else if ((wEPVal & EP_CTR_RX) != 0)
        {
          _ClearEP_CTR_RX(ENDP0);
          Out0_Process();
          /* before terminate set Tx & Rx status */
          _SetEPRxStatus(ENDP0, SaveRState);
          _SetEPTxStatus(ENDP0, SaveTState);
          return;
        }
      }
    }/* if(EPindex == 0) */
    else
    {
      /* Decode and service non control endpoints interrupt  */

      /* process related endpoint register */
      wEPVal = _GetENDPOINT(EPindex);
      if ((wEPVal & EP_CTR_RX) != 0)
      {
	extern sc_msg_t rxMsg[8];
	sc_msg_t msg;
	int size;
	int msize;
	
        /* clear int flag */
        _ClearEP_CTR_RX(EPindex);
	
	if ( (msg = rxMsg[EPindex]) ){	    
	  rxMsg[EPindex] = NULL;
	  size = _GetEPRxCount(EPindex);
	  msg->id = USB_EP_COMPLETE_MSG(EPindex);
	  msize = sc_msgSizeGet(&msg)-sizeof(usb_epComplete_t);
	  size = msize > size ? size : msize;
	  msg->epComplete.size = size;
	  PMAToUserBufferCopy(msg->recvData.data, msg->recvData.rxAddr, size);
	  
	  msg->epComplete.rcvSnd = 0;
	  sc_msgTx(&msg, sc_msgSndGet(&msg), 0);
	}
      } /* if((wEPVal & EP_CTR_RX) */

      if ((wEPVal & EP_CTR_TX) != 0)
      {
	extern sc_msg_t txMsg[8];
        /* clear int flag */
        _ClearEP_CTR_TX(EPindex);

	if ( txMsg[EPindex] ){
	  sc_msgid_t sel[2];
	  sel[0] = USB_SEND_DATA_MSG(EPindex);
	  sel[1] = 0;
	  txMsg[EPindex]->id = USB_EP_COMPLETE_MSG(EPindex);
	  txMsg[EPindex]->epComplete.rcvSnd = 1;
          txMsg[EPindex]->epComplete.size = _GetEPTxCount(EPindex);
	  sc_msgTx(&txMsg[EPindex],sc_msgSndGet(&txMsg[EPindex]),0);

	  txMsg[EPindex] = sc_msgRx(SC_NO_TMO, sel, SC_MSGRX_MSGID);
	  if ( txMsg[EPindex] ){
	    UserToPMABufferCopy(txMsg[EPindex]->sendData.data,
				txMsg[EPindex]->sendData.txAddr,
				txMsg[EPindex]->sendData.size);
	    _SetEPTxCount(EPindex, txMsg[EPindex]->sendData.size);
	    _SetEPTxStatus(EPindex, EP_TX_VALID);
	  }
	}
      } /* if((wEPVal & EP_CTR_TX) != 0) */

    }/* if(EPindex == 0) else */

  }/* while(...) */
}

/*******************************************************************************
* Function Name  : CTR_HP.
* Description    : High Priority Endpoint Correct Transfer interrupt's service 
*                  routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void CTR_HP(void)
{
  uint16_t wIstr;
  uint32_t wEPVal = 0;

  while (((wIstr = _GetISTR()) & ISTR_CTR) != 0)
  {
    _SetISTR((uint16_t)CLR_CTR); /* clear CTR flag */
    /* extract highest priority endpoint number */
    EPindex = (uint8_t)(wIstr & ISTR_EP_ID);
    /* process related endpoint register */
    wEPVal = _GetENDPOINT(EPindex);
    if ((wEPVal & EP_CTR_RX) != 0)
    {
	extern sc_msg_t rxMsg[8];
	sc_msg_t msg;
	int size;
	int msize;
	
        /* clear int flag */
        _ClearEP_CTR_RX(EPindex);
	
	if ( (msg = rxMsg[EPindex]) ){	    
	  rxMsg[EPindex] = NULL;
	  size = GetEPRxCount(EPindex);
	  msg->id = USB_EP_COMPLETE_MSG(EPindex);
	  msize = sc_msgSizeGet(&msg)-sizeof(usb_epComplete_t);
	  size = msize > size ? size : msize;
	  msg->epComplete.size = size;
	  PMAToUserBufferCopy(msg->recvData.data, msg->recvData.rxAddr, size);
	  
	  msg->epComplete.rcvSnd = 0;
	  sc_msgTx(&msg, sc_msgSndGet(&msg), 0);
	}
    } /* if((wEPVal & EP_CTR_RX) */
    else if ((wEPVal & EP_CTR_TX) != 0)
    {
      extern sc_msg_t txMsg[8];
      /* clear int flag */
      _ClearEP_CTR_TX(EPindex);

      if ( txMsg[EPindex] ){	
	sc_msgid_t sel[2];
	sel[0] = USB_SEND_DATA_MSG(EPindex);
	sel[1] = 0;
	txMsg[EPindex]->id = USB_EP_COMPLETE_MSG(EPindex);
	txMsg[EPindex]->epComplete.rcvSnd = 1;
        txMsg[EPindex]->epComplete.size = _GetEPTxCount(EPindex);
	sc_msgTx(&txMsg[EPindex],sc_msgSndGet(&txMsg[EPindex]),0);
	
	txMsg[EPindex] = sc_msgRx(SC_NO_TMO, sel, SC_MSGRX_MSGID);
	if ( txMsg[EPindex] ){
	  UserToPMABufferCopy(txMsg[EPindex]->sendData.data,
			      txMsg[EPindex]->sendData.txAddr,
			      txMsg[EPindex]->sendData.size);
	  _SetEPTxCount(EPindex, txMsg[EPindex]->sendData.size);
	  _SetEPTxStatus(EPindex, EP_TX_VALID);
	}
      }
    } /* if((wEPVal & EP_CTR_TX) != 0) */

  }/* while(...) */
}

SC_INT_PROCESS(SCI_usbd,src)
{
  static sc_msgid_t sel[16];
  int i;
  
  if ( src == -1 ){
    NVIC_InitTypeDef NVIC_InitStructure;

    //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  
    NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    memset(txMsg,0,sizeof(txMsg));
    memset(rxMsg,0,sizeof(rxMsg));
    for(i = 1; i < 8; ++i){
      sel[i-1] = USB_RECV_DATA_MSG(i);
    }
    USB_Init();
  } else if ( src == 0 ){
    USB_Istr();
  } else if ( src == 1 ){
    int o;
    sc_msg_t msg;

    for(o = 7,i = 0; i < 8; ++i){
      if ( txMsg[i] == NULL ){
	sel[o] = USB_SEND_DATA_MSG(i);
        ++o;
      }
    }
    sel[o] = 0;
    msg = sc_msgRx(SC_NO_TMO, sel, SC_MSGRX_MSGID);
    if ( msg ){
      int epnum = msg->id & 0xf;
      switch( msg->id & 0xfffffff0 ){
      case USB_SEND_DATA_MSG(0):
	UserToPMABufferCopy(msg->sendData.data,
			    msg->sendData.txAddr,
			    msg->sendData.size);
	_SetEPTxCount(epnum, msg->sendData.size);
        _SetEPTxStatus(epnum, EP_TX_VALID)
	txMsg[epnum] = msg;
	break;
      case USB_RECV_DATA_MSG(0):
        _SetEPRxStatus(epnum, EP_RX_VALID)
	rxMsg[epnum] = msg;
	break;
      default:
	sc_miscError(SC_ERR_PROCESS_FATAL,msg->id);
      }
    }
  }    
}
