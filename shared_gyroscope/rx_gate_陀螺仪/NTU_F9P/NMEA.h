
#ifndef __NMEA_H__
#define __NMEA_H__

//#include "Std_Types.h"
#include "../Std_Types.h"

#define BUFLEN 512       /*可随意取，但要大于GPGGA_MAX*/
#define GPGGA_MAX 100       /*要大于$GPGGA……"的最大值*/ 
#define DBG_GPS

#define NMEA_SIG_BAD        (0)
#define NMEA_SIG_LOW        (1)
#define NMEA_SIG_MID        (2)
#define NMEA_SIG_HIGH       (3)

#define NMEA_FIX_BAD        (1)
#define NMEA_FIX_2D         (2)
#define NMEA_FIX_3D         (3)

#define NMEA_MAXSAT         (12)
#define NMEA_SATINPACK      (4)
#define NMEA_NSATPACKS      (NMEA_MAXSAT / NMEA_SATINPACK)

#define NMEA_DEF_LAT        (5001.2621)
#define NMEA_DEF_LON        (3613.0595)


typedef struct{  
    uint16 year;    
    uint8 month;   
    uint8 day;  
    uint8 hour;  
    uint8 minute;  
    uint16 second;  
}date_time;  
  
typedef struct
{  
     date_time D;       /*时间*/  
     uint8 status;       /*接收状态 */
     uint8 calu_status;  /*计算求解状态 */
     float64 latitude;   /*纬度*/
     float64 longitude;  /*经度 */
     uint8 NS;           /*南北极*/  
     uint8 EW;           /*东西 */ 
     uint8 num;           /*卫星数*/ 
     float32 speed;      /*速度 */ 
     float32 high;       /*高度*/ 
}GPS_INFO;

void AddStringToBuf(sint8 * buf, sint32 count);
uint32 Get_GPSData(sint8 * buf);
uint8 nmea_calc_crc(const sint8 *buff, uint32 buff_sz);
sint8 gps_parse(sint8 *line, GPS_INFO *GPS);
void show_gps(GPS_INFO *GPS);
sint32 nmea_frame_check(sint8 *line, uint32 len);

void gps_bl_xy(double b, double l,double *x, double *y);    //b,纬度， l,经度

#endif /* __NMEA_H__ */
