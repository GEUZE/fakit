/****************************************************************************
 *
 *            Copyright (c) 2003-2008 by HCC Embedded
 *
 * This software is copyrighted by and is the sole property of
 * HCC.  All rights, title, ownership, or other interests
 * in the software remain the property of HCC.  This
 * software may only be used in accordance with the corresponding
 * license agreement.  Any unauthorized use, duplication, transmission,
 * distribution, or disclosure of this software is expressly forbidden.
 *
 * This Copyright notice may not be removed or modified without prior
 * written consent of HCC.
 *
 * HCC reserves the right to modify this software without notice.
 *
 * HCC Embedded
 * Budapest 1133
 * Vaci Ut 110
 * Hungary
 *
 * Tel:  +36 (1) 450 1302
 * Fax:  +36 (1) 450 1303
 * http: www.hcc-embedded.com
 * email: info@hcc-embedded.com
 *
 ***************************************************************************/
#include <hcc/eusbh/usb_driver/usb_host.h>
#include <hcc/eusbh/usb_driver/usb_utils.h>
#include <hcc/eusbh/hid/hid_usage.h>
#include <hcc/eusbh/hid/host_hid.h>
#include <hcc/eusbh/hid/host_hid_explore.h>
#include <hcc/eusbh/hid/hid_parser.h>

/*****************************************************************************
 * Module variables.
 *****************************************************************************/
typedef struct {
  hid_info_t hid_info;

  hcc_u8 hid_device;

  hcc_u16 report_size;
  hcc_u8 report_descriptor[HID_REPORT_DESCRIPTOR_MAX_SIZE];
  report_t reports[MAX_NO_OF_REPORTS];

  t_usbh_transfer uht;
  hcc_u8 report_data_uht[64];

  OS_MUTEX_TYPE *mutex;
  hcc_u8 change[MAX_NO_OF_REPORTS][MAX_REPORT_ITEMS];
  hcc_u8 report_data[64];

} t_hid_explore_info;
static t_hid_explore_info hid_explore_info;

/*****************************************************************************
 * Function predefinitions.
 *****************************************************************************/
static int hid_explore_receive_init (void);

/*****************************************************************************
 * Function definitions.
 *****************************************************************************/
/*****************************************************************************
 * Name:
 *    hid_explore_receive_cb
 * In:
 *    param - parameter
 *    rc - return code
 *
 * Description:
 *    Callback function for the receive transfer.
 *
 * Assumptions:
 *    -
 *****************************************************************************/
static void hid_explore_receive_cb (void *param, int rc)
{
  if (rc==USBH_SUCCESS || rc==USBH_SHORT_PACKET)
  {
    t_hid_explore_info *hei=(t_hid_explore_info *)param;
    if (os_mutex_get(hei->mutex)==OS_SUCCESS)
    {
      hcc_uint i;
      for (i=0;i<hei->uht.csize && hei->report_data_uht[i]==0;i++)
        ;
      if (i<hei->uht.csize)
      {
        _memcpy(hei->report_data,hei->report_data_uht,hei->uht.csize);
        _memset(hei->change,1,sizeof(hei->change));
      }
      (void)os_mutex_put(hei->mutex);
    }
  }
  hid_explore_receive_init();
}


/*****************************************************************************
 * Name:
 *    hid_explore_receive_init
 * In:
 *    -
 * Out:
 *    transfer start return code
 *
 * Description:
 *    Initialize receive.
 *
 * Assumptions:
 *    -
 *****************************************************************************/
static int hid_explore_receive_init (void)
{
  hid_explore_info.uht.eph=hid_explore_info.hid_info.ep_hdl[IN_IT];
  hid_explore_info.uht.buf=hid_explore_info.report_data_uht;
  hid_explore_info.uht.size=USBH_SIZE_DEFAULT;
  hid_explore_info.uht.toggle=USBH_TOGGLE_PRESERVE;
  hid_explore_info.uht.cb_fn=hid_explore_receive_cb;
  hid_explore_info.uht.cb_param=&hid_explore_info;
  return usbh_transfer(&hid_explore_info.uht,USBH_MODE_NON_BLOCK);
}


/*****************************************************************************
 * Name:
 *    hid_explore_init
 * In:
 *    -
 * Out:
 *    USBH_SUCCESS
 *
 * Description:
 *    Initialize mouse driver.
 *
 * Assumptions:
 *    -
 *****************************************************************************/
int hid_explore_init (void)
{
  _memset(&hid_explore_info,0,sizeof(hid_explore_info));
  if (os_mutex_create(&hid_explore_info.mutex)) return USBH_ERR_RESOURCE;
  return USBH_SUCCESS;
}


/*****************************************************************************
 * Name:
 *    hid_explore_get_hdl
 * In:
 *    -
 * Out:
 *    NULL / pointer to info structure
 *
 * Description:
 *    Get mouse information handler if available.
 *
 * Assumptions:
 *    -
 *****************************************************************************/
void *hid_explore_get_hdl (void)
{
  if (hid_explore_info.hid_device==0)
  {
    hid_explore_info.report_size=0;
    return &hid_explore_info;
  }
  return NULL;
}


/*****************************************************************************
 * Name:
 *    hid_explore_connect
 * In:
 *    device handler
 * Out:
 *    USBH_*
 *
 * Description:
 *    Connect mouse driver.
 *
 * Assumptions:
 *    -
 *****************************************************************************/
int hid_explore_connect (void *dev_hdl)
{
  t_hid_explore_info *hei=(t_hid_explore_info *)dev_hdl;

  if (hid_get_report_descriptor(&hei->hid_info,hei->report_descriptor,hei->report_size)==USBH_SUCCESS)
  {
    if (get_reports(hei->reports, MAX_NO_OF_REPORTS, hei->report_descriptor, hei->report_size)==0)
    {
      hcc_uint i,j,v=1;
      for (i=0;i<MAX_NO_OF_REPORTS;i++)
      {
        for (j=0;j<MAX_REPORT_ITEMS;j++)
        {
          rp_item_t *rpi=&hei->reports[i].items[j];
          if (rpi->usage_page==0) break;
          if (rpi->usage_page<HIDUPG_VENDOR) v=0;
        }
      }
      if (v==0)
      {
        int rc=hid_explore_receive_init();
        if (rc==USBH_SUCCESS) hei->hid_device=1;
        return rc;
      }
    }
  }
  return USBH_ERR_NOT_AVAILABLE;
}


/*****************************************************************************
 * Name:
 *    hid_explore_disconnect
 * In:
 *    Device handler
 * Out:
 *    error code
 *
 * Description:
 *    Disconnect mouse driver.
 *
 * Assumptions:
 *    -
 *****************************************************************************/
int hid_explore_disconnect (void *dev_hdl)
{
  t_hid_explore_info *hei=(t_hid_explore_info *)dev_hdl;
  hei->hid_device=0;
  return 0;
}


/*****************************************************************************
 * Name:
 *    hid_explore_report_size
 * In:
 *    Device handler
 * Out:
 *    error code
 *
 * Description:
 *    Set report size
 *
 * Assumptions:
 *    -
 *****************************************************************************/
int hid_explore_report_size (void *dev_hdl, hcc_u16 report_size)
{
  t_hid_explore_info *hei=(t_hid_explore_info *)dev_hdl;
  hei->report_size=report_size;
  return USBH_SUCCESS;
}



int hid_explore_get_value (hcc_u8 usage_page, hcc_u8 usage, hcc_s32 *value)
{
  hcc_uint i,j;
  t_hid_explore_info *hei=&hid_explore_info;
  if (os_mutex_get(hei->mutex)==OS_SUCCESS)
  {
    for (i=0;i<MAX_NO_OF_REPORTS;i++)
    {
      for (j=0;j<MAX_REPORT_ITEMS;j++)
      {
        rp_item_t *rpi=&hei->reports[i].items[j];
        if (rpi->usage_page==0) break;
        if (rpi->usage_page==usage_page && rpi->usage_min==usage)
        {
          if (hei->change[i][j])
          {
            hei->change[i][j]=0;
            if (read_item(rpi,hid_explore_info.report_data,0,value)==0)
            {
              if (*value)
              {
                (void)os_mutex_put(hei->mutex);
                return USBH_SUCCESS;
              }
            }
          }
        }
      }
    }
    (void)os_mutex_put(hei->mutex);
  }
  return USBH_ERR_NOT_AVAILABLE;
}


