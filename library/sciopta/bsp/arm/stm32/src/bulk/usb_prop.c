
/* Includes ------------------------------------------------------------------*/
#include <sciopta.h>
#include <stdio.h>
#include "usb_lib.h"
#include "usb_conf.h"
#include "usb_prop.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "usb_hw_setup.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint8_t Request = 0;

/* -------------------------------------------------------------------------- */
/*  Structures initializations */
/* -------------------------------------------------------------------------- */

DEVICE Device_Table =
  {
    EP_NUM,
    1
  };

DEVICE_PROP Device_Property =
  {
    Bulk_init,
    Bulk_Reset,
    Bulk_Status_In,
    Bulk_Status_Out,
    Bulk_Data_Setup,
    Bulk_NoData_Setup,
    Bulk_Get_Interface_Setting,
    Bulk_GetDeviceDescriptor,
    Bulk_GetConfigDescriptor,
    Bulk_GetStringDescriptor,
    0,
    0x40 /*MAX PACKET SIZE*/
  };

USER_STANDARD_REQUESTS User_Standard_Requests =
  {
    Bulk_GetConfiguration,
    Bulk_SetConfiguration,
    Bulk_GetInterface,
    Bulk_SetInterface,
    Bulk_GetStatus,
    Bulk_ClearFeature,
    Bulk_SetEndPointFeature,
    Bulk_SetDeviceFeature,
    Bulk_SetDeviceAddress
  };

static const ONE_DESCRIPTOR Device_Descriptor =
  {
    (uint8_t*)Bulk_DeviceDescriptor,
    BULK_SIZ_DEVICE_DESC
  };

static const ONE_DESCRIPTOR Config_Descriptor =
  {
    (uint8_t*)Bulk_ConfigDescriptor,
    BULK_SIZ_CONFIG_DESC
  };

static const ONE_DESCRIPTOR String_Descriptor[4] =
  {
    {(uint8_t*)Bulk_StringLangID, BULK_SIZ_STRING_LANGID},
    {(uint8_t*)Bulk_StringVendor, BULK_SIZ_STRING_VENDOR},
    {(uint8_t*)Bulk_StringProduct, BULK_SIZ_STRING_PRODUCT},
    {(uint8_t*)Bulk_StringSerial, BULK_SIZ_STRING_SERIAL}
  };

/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
uint8_t *Bulk_ReadData(uint16_t Length);
uint8_t *Bulk_WriteData(uint16_t Length);
/* Extern function prototypes ------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : Bulk_init.
* Description    : Virtual COM Port Mouse init routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Bulk_init(void)
{
  uint32_t serial[3];
  /* Update the serial number string descriptor with the data from the unique
  ID*/
  if ( Get_SerialNum(serial) ){
    int i;
    char help[3*8+1];
    uint16_t *p = (uint16_t *)Bulk_StringSerial;
    sprintf(help,"%08x%08x%08x",serial[0],serial[1],serial[2]);
    kprintf(0,"Serial number:%s\n",help);
    ++p; /* skip count and descriptor-type */
    for(i = 0; i < 24; ++i){
      *p++ = help[i];
    }
  }
    
  pInformation->Current_Configuration = 0;

  /* Connect the device */
  PowerOn();
  /* USB interrupts initialization */
  /* clear pending interrupts */
  _SetISTR(0);
  wInterrupt_Mask = IMR_MSK;
  /* set interrupts mask */
  _SetCNTR(wInterrupt_Mask);

  bDeviceState = UNCONNECTED;
}

/*******************************************************************************
* Function Name  : Bulk_Reset
* Description    : Virtual_Com_Port Mouse reset routine
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Bulk_Reset(void)
{
  /* Set Virtual_Com_Port DEVICE as not configured */
  pInformation->Current_Configuration = 0;

  /* Current Feature initialization */
  pInformation->Current_Feature = Bulk_ConfigDescriptor[7];

  /* Set Virtual_Com_Port DEVICE with the default Interface*/
  pInformation->Current_Interface = 0;
  _SetBTABLE(BTABLE_ADDRESS);

  /* Initialize Endpoint 0 */
  _SetEPType(ENDP0, EP_CONTROL);
  _SetEPTxStatus(ENDP0, EP_TX_STALL);
  _SetEPRxAddr(ENDP0, ENDP0_RXADDR);
  _SetEPTxAddr(ENDP0, ENDP0_TXADDR);
  _ClearEP_KIND(ENDP0);
  _SetEPRxCount(ENDP0, Device_Property.MaxPacketSize);
  _SetEPRxStatus(ENDP0, EP_RX_VALID)
    
  /* Initialize Endpoint 1 */
  _SetEPType(ENDP1, EP_BULK);
  _SetEPTxAddr(ENDP1, ENDP1_TXADDR);
  _SetEPTxStatus(ENDP1, EP_TX_NAK);
//  _SetEPRxStatus(ENDP1, EP_RX_DIS);

  _SetEPRxAddr(ENDP1, ENDP1_RXADDR);
  _SetEPRxCount(ENDP1, BULK_DATA_SIZE);
  _SetEPRxStatus(ENDP1, EP_RX_VALID);
//  _SetEPTxStatus(ENDP1, EP_TX_DIS);

  /* Set this device to response on default address */
  SetDeviceAddress(0);

  bDeviceState = ATTACHED;
}

/*******************************************************************************
* Function Name  : Bulk_SetConfiguration.
* Description    : Udpade the device state to configured.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Bulk_SetConfiguration(void)
{
  DEVICE_INFO *pInfo = &Device_Info;

  if (pInfo->Current_Configuration != 0)
  {
    /* Device configured */
    bDeviceState = CONFIGURED;
  }
}

/*******************************************************************************
* Function Name  : Bulk_SetConfiguration.
* Description    : Udpade the device state to addressed.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Bulk_SetDeviceAddress (void)
{
  bDeviceState = ADDRESSED;
}

/*******************************************************************************
* Function Name  : Bulk_Status_In.
* Description    : Virtual COM Port Status In Routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Bulk_Status_In(void)
{
}

/*******************************************************************************
* Function Name  : Bulk_Status_Out
* Description    : Virtual COM Port Status OUT Routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Bulk_Status_Out(void)
{
}

/*******************************************************************************
* Function Name  : Bulk_Data_Setup
* Description    : handle the data class specific requests
* Input          : Request Nb.
* Output         : None.
* Return         : USB_UNSUPPORT or USB_SUCCESS.
*******************************************************************************/
RESULT Bulk_Data_Setup(uint8_t RequestNo)
{
  uint8_t    *(*CopyRoutine)(uint16_t);

  if (RequestNo == GET_LINE_CODING)
  {
    if (Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
    {
      CopyRoutine = Bulk_ReadData;
    }
  }
  else if (RequestNo == SET_LINE_CODING)
  {
    if (Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
    {
      CopyRoutine = Bulk_WriteData;
    }
    Request = SET_LINE_CODING;
  } else {
    return USB_UNSUPPORT;
  }

  pInformation->Ctrl_Info.CopyData = CopyRoutine;
  pInformation->Ctrl_Info.Usb_wOffset = 0;
  (*CopyRoutine)(0);
  return USB_SUCCESS;
}

/*******************************************************************************
* Function Name  : Bulk_NoData_Setup.
* Description    : handle the no data class specific requests.
* Input          : Request Nb.
* Output         : None.
* Return         : USB_UNSUPPORT or USB_SUCCESS.
*******************************************************************************/
RESULT Bulk_NoData_Setup(uint8_t RequestNo)
{

  if (Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
  {
  }

  return USB_UNSUPPORT;
}

/*******************************************************************************
* Function Name  : Bulk_GetDeviceDescriptor.
* Description    : Gets the device descriptor.
* Input          : Length.
* Output         : None.
* Return         : The address of the device descriptor.
*******************************************************************************/
uint8_t *Bulk_GetDeviceDescriptor(uint16_t Length)
{
  return Standard_GetDescriptorData(Length, &Device_Descriptor);
}

/*******************************************************************************
* Function Name  : Bulk_GetConfigDescriptor.
* Description    : get the configuration descriptor.
* Input          : Length.
* Output         : None.
* Return         : The address of the configuration descriptor.
*******************************************************************************/
uint8_t *Bulk_GetConfigDescriptor(uint16_t Length)
{
  return Standard_GetDescriptorData(Length, &Config_Descriptor);
}

/*******************************************************************************
* Function Name  : Bulk_GetStringDescriptor
* Description    : Gets the string descriptors according to the needed index
* Input          : Length.
* Output         : None.
* Return         : The address of the string descriptors.
*******************************************************************************/
uint8_t *Bulk_GetStringDescriptor(uint16_t Length)
{
  uint8_t wValue0 = pInformation->USBwValue0;
  if (wValue0 > 4)
  {
    return NULL;
  }
  else
  {
    return Standard_GetDescriptorData(Length, &String_Descriptor[wValue0]);
  }
}

/*******************************************************************************
* Function Name  : Bulk_Get_Interface_Setting.
* Description    : test the interface and the alternate setting according to the
*                  supported one.
* Input1         : uint8_t: Interface : interface number.
* Input2         : uint8_t: AlternateSetting : Alternate Setting number.
* Output         : None.
* Return         : The address of the string descriptors.
*******************************************************************************/
RESULT Bulk_Get_Interface_Setting(uint8_t Interface, uint8_t AlternateSetting)
{
  if (AlternateSetting > 0)
  {
    return USB_UNSUPPORT;
  }
  else if (Interface > 1)
  {
    return USB_UNSUPPORT;
  }
  return USB_SUCCESS;
}

uint8_t buffer[64];
/*******************************************************************************
* Function Name  : Bulk_GetLineCoding.
* Description    : send the linecoding structure to the PC host.
* Input          : Length.
* Output         : None.
* Return         : Inecoding structure base address.
*******************************************************************************/
uint8_t *Bulk_ReadData(uint16_t Length)
{
  if (Length == 0)
  {
    pInformation->Ctrl_Info.Usb_wLength = sizeof(buffer);
    return NULL;
  }
  return buffer;
}

/*******************************************************************************
* Function Name  : Bulk_SetLineCoding.
* Description    : Set the linecoding structure fields.
* Input          : Length.
* Output         : None.
* Return         : Linecoding structure base address.
*******************************************************************************/
uint8_t *Bulk_WriteData(uint16_t Length)
{
  if (Length == 0)
  {
    pInformation->Ctrl_Info.Usb_wLength = sizeof(buffer);
    return NULL;
  }
  return buffer;
}

