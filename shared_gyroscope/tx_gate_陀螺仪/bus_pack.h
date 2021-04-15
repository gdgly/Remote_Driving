#ifndef __BUS_PACK_H__
#define __BUS_PACK_H__

#include "Std_Types.h"

void pack_position_ls(sint8 * buf, sint8 * imei, float64 latitude, float64 longitude);
void load_config_file();

#endif /* __BUS_PACK_H__ */
