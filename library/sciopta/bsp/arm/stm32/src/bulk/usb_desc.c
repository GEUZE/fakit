
/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
#include "usb_desc.h"
/* USB Standard Device Descriptor */
const uint8_t Bulk_DeviceDescriptor[] =
  {
    0x12,   /* bLength */
    USB_DEVICE_DESCRIPTOR_TYPE,     /* bDescriptorType */
    0x00,
    0x02,   /* bcdUSB = 2.00 */
    0x02,   /* bDeviceClass: CDC */
    0x00,   /* bDeviceSubClass */
    0x00,   /* bDeviceProtocol */
    0x40,   /* bMaxPacketSize0 */
    0x34,0x12,   /* idVendor = 0x1234 */
    0x78,0x56,   /* idProduct = 0x5678 */
    0x00,
    0x02,   /* bcdDevice = 2.00 */
    1,    /* Index of string descriptor describing manufacturer */
    2,    /* Index of string descriptor describing product */
    3,    /* Index of string descriptor describing the device's serial number */
    0x01    /* bNumConfigurations */
  };

const uint8_t Bulk_ConfigDescriptor[] =
  {
    /*Configuation Descriptor*/
    0x09,   /* bLength: Configuation Descriptor size */
    USB_CONFIGURATION_DESCRIPTOR_TYPE,      /* bDescriptorType: Configuration */
    9+9+2*7,0,
    0x01,   /* bNumInterfaces: 1 interface */
    0x01,   /* bConfigurationValue: Configuration value */
    0x00,   /* iConfiguration: Index of string descriptor describing the configuration */
    0xC0,   /* bmAttributes: self powered */
    0x32,   /* MaxPower 0 mA */
    
    /*Interface Descriptor*/
    0x09,   /* bLength: Interface Descriptor size */
    USB_INTERFACE_DESCRIPTOR_TYPE,  /* bDescriptorType: Interface */
    /* Interface descriptor type */
    0x00,   /* bInterfaceNumber: Number of Interface */
    0x00,   /* bAlternateSetting: Alternate setting */
    0x02,   /* bNumEndpoints: One endpoints used */
    0xfe,   /* bInterfaceClass: BULK */
    0x01,   /* bInterfaceSubClass: Abstract Control Model */
    0x00,   /* bInterfaceProtocol */
    0x00,   /* iInterface: */

    /*Endpoint 1 Descriptor*/
    0x07,   /* bLength: Endpoint Descriptor size */
    USB_ENDPOINT_DESCRIPTOR_TYPE,   /* bDescriptorType: Endpoint */
    0x01,   /* bEndpointAddress: (OUT1) */
    0x02,   /* bmAttributes: Bulk */
    BULK_DATA_SIZE, 0,            /* wMaxPacketSize: */
    0x00,   /* bInterval: ignore for Bulk transfer */

    /*Endpoint 1 Descriptor*/
    0x07,   /* bLength: Endpoint Descriptor size */
    USB_ENDPOINT_DESCRIPTOR_TYPE,   /* bDescriptorType: Endpoint */
    0x81,   /* bEndpointAddress: (IN1) */
    0x02,   /* bmAttributes: Bulk */
    BULK_DATA_SIZE,0,             /* wMaxPacketSize: */
    0x00,    /* bInterval */
  };

/* USB String Descriptors */
const uint8_t Bulk_StringLangID[BULK_SIZ_STRING_LANGID] =
  {
    BULK_SIZ_STRING_LANGID,
    USB_STRING_DESCRIPTOR_TYPE,
    0x09,
    0x04 /* LangID = 0x0409: U.S. English */
  };

#define _(a) a,0
#define _8(a,b,c,d,e,f,g,h) a,0,b,0,c,0,d,0,e,0,f,0,g,0,h,0

const uint8_t Bulk_StringVendor[BULK_SIZ_STRING_VENDOR] =
  {
//    BULK_SIZ_STRING_VENDOR,     /* Size of Vendor string */
    sizeof("SCIOPTA Systems ")*2-2+2,
    USB_STRING_DESCRIPTOR_TYPE,             /* bDescriptorType*/
    /* Manufacturer: "STMicroelectronics" */
    _8('S','C','I','O','P','T','A',' '),
    _8('S','y','s','t','e','m','s',0)
  };

const uint8_t Bulk_StringProduct[BULK_SIZ_STRING_PRODUCT] =
  {
    sizeof("STM32 Bulk")*2-2+2,          /* bLength */
    USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
    /* Product name: "STM32 Virtual COM Port" */
    _8('S','T','M','3','2',' ','B','u'),
    _('l'),_('k')
  };

uint8_t Bulk_StringSerial[BULK_SIZ_STRING_SERIAL] =
  {
    BULK_SIZ_STRING_SERIAL,           /* bLength */
    USB_STRING_DESCRIPTOR_TYPE,                   /* bDescriptorType */
    _8('4','.','5','.','2','0','0','3')
  }
  ;/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
