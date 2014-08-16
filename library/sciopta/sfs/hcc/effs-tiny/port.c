/****************************************************************************
 *
 *            Copyright (c) 2005 by HCC Embedded 
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
 * Budapest 1132
 * Victor Hugo Utca 11-15
 * Hungary
 *
 * Tel:  +36 (1) 450 1302
 * Fax:  +36 (1) 450 1303
 * http: www.hcc-embedded.com
 * email: info@hcc-embedded.com
 *
 ***************************************************************************/

/*
** get_timedate
** 
** INPUT: time - previous file time
**        date - previous file date
** OUTPUT:time - new time
**        date - new date
** RETURN:0 - on success, other if error
*/
static unsigned short time_base=0;
unsigned char get_timedate (unsigned short *time, unsigned short *date)
{
  *date=0;
  if (*time) ++(*time);
        else *time=time_base++;
  return 0;
}

