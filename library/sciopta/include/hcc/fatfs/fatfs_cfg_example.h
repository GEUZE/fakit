/*
** 
** Project defines for FATFS
**
*/


/*
** EUSBH
*/
#define SCSI_MAX_DEVICE 4
#define MST_MAX_DEVICE  SCSI_MAX_DEVICE

/*
** CompactFlash/SD-Card
*/
#define CF_MAX_DEVICE   0
#define SD_MAX_DEVICE   1

/*
** Number of LUNs to support
*/

#define ADDITIONAL_LUN 4

/*
** Total number of physical attachable devices (not counting LUNs)
*/

#define FATFS_MAX_PHY_VOLUME (SCSI_MAX_DEVICE+SD_MAX_DEVICE+CF_MAX_DEVICE)

/*
** Normaly udefs_f.h defines the following
*/
#define FN_MAXVOLUME     (FATFS_MAX_PHY_VOLUME+ADDITIONAL_LUN)

