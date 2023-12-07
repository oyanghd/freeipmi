/*
 * Copyright (C) 2003-2015 FreeIPMI Core Team
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>
#if STDC_HEADERS
#include <string.h>
#include <stdarg.h>
#endif /* STDC_HEADERS */
#include <assert.h>
#include <errno.h>

#include "freeipmi/sel/ipmi-sel.h"

#include "freeipmi/record-format/ipmi-sel-record-format.h"
#include "freeipmi/spec/ipmi-event-reading-type-code-spec.h"
#include "freeipmi/spec/ipmi-iana-enterprise-numbers-spec.h"
#include "freeipmi/spec/ipmi-product-id-spec.h"
#include "freeipmi/spec/ipmi-sensor-and-event-code-tables-spec.h"
#include "freeipmi/spec/ipmi-sensor-types-spec.h"
#include "freeipmi/spec/oem/ipmi-sensor-and-event-code-tables-oem-inventec-spec.h"
#include "freeipmi/spec/oem/ipmi-sensor-numbers-oem-inventec-spec.h"
#include "freeipmi/spec/oem/ipmi-slave-address-oem-inventec-spec.h"

#include "ipmi-sel-common.h"
#include "ipmi-sel-defs.h"
#include "ipmi-sel-string.h"
#include "ipmi-sel-string-intel-node-manager.h"
#include "ipmi-sel-string-inventec-common.h"
#include "ipmi-sel-trace.h"
#include "ipmi-sel-util.h"

#include "freeipmi-portability.h"

/* return (0) - no OEM match
 * return (1) - OEM match
 * return (-1) - error, cleanup and return error
 *
 * in oem_rv, return
 * 0 - continue on
 * 1 - buffer full, return full buffer to user
 */
int
sel_string_output_inventec_5442_sensor_name (ipmi_sel_ctx_t ctx,
                                             struct ipmi_sel_entry *sel_entry,
                                             uint8_t sel_record_type,
                                             char *buf,
                                             unsigned int buflen,
                                             unsigned int flags,
                                             unsigned int *wlen,
                                             struct ipmi_sel_system_event_record_data *system_event_record_data,
                                             int *oem_rv)
{
  int ret;

  assert (ctx);
  assert (ctx->magic == IPMI_SEL_CTX_MAGIC);
  assert (ctx->manufacturer_id == IPMI_IANA_ENTERPRISE_ID_INVENTEC);
  assert (sel_entry);
  assert (buf);
  assert (buflen);
  assert (!(flags & ~IPMI_SEL_STRING_FLAGS_MASK));
  assert (flags & IPMI_SEL_STRING_FLAGS_INTERPRET_OEM_DATA);
  assert (wlen);
  assert (system_event_record_data);
  assert (oem_rv);
  assert (ctx->product_id == IPMI_INVENTEC_PRODUCT_ID_5442);

  /*
   * Inventec 5442/Dell Xanadu III
   */
  if ((ret = sel_string_output_inventec_5441_5442_common_sensor_name (ctx,
                                                                      sel_entry,
                                                                      sel_record_type,
                                                                      buf,
                                                                      buflen,
                                                                      flags,
                                                                      wlen,
                                                                      system_event_record_data,
                                                                      oem_rv)) < 0)
    return (-1);

  if (ret)
    return (1);

  return (0);
}

/* return (0) - no OEM match
 * return (1) - OEM match
 * return (-1) - error, cleanup and return error
 */
int
sel_string_output_inventec_5442_event_data1_class_oem (ipmi_sel_ctx_t ctx,
                                                       struct ipmi_sel_entry *sel_entry,
                                                       uint8_t sel_record_type,
                                                       char *tmpbuf,
                                                       unsigned int tmpbuflen,
                                                       unsigned int flags,
                                                       unsigned int *wlen,
                                                       struct ipmi_sel_system_event_record_data *system_event_record_data)
{
  int ret;

  assert (ctx);
  assert (ctx->magic == IPMI_SEL_CTX_MAGIC);
  assert (ctx->manufacturer_id == IPMI_IANA_ENTERPRISE_ID_INVENTEC);
  assert (sel_entry);
  assert (tmpbuf);
  assert (tmpbuflen);
  assert (!(flags & ~IPMI_SEL_STRING_FLAGS_MASK));
  assert (flags & IPMI_SEL_STRING_FLAGS_INTERPRET_OEM_DATA);
  assert (wlen);
  assert (system_event_record_data);
  assert (ctx->product_id == IPMI_INVENTEC_PRODUCT_ID_5442);

  /*
   * Inventec 5442/Dell Xanadu III
   */
  if ((ret = sel_string_output_inventec_5441_5442_common_event_data1_class_oem (ctx,
                                                                                sel_entry,
                                                                                sel_record_type,
                                                                                tmpbuf,
                                                                                tmpbuflen,
                                                                                flags,
                                                                                wlen,
                                                                                system_event_record_data)) < 0)
    return (-1);

  if (ret)
    return (1);

  return (0);
}

/* return (0) - no OEM match
 * return (1) - OEM match
 * return (-1) - error, cleanup and return error
 */
int
sel_string_output_inventec_5442_event_data2_discrete_oem (ipmi_sel_ctx_t ctx,
                                                          struct ipmi_sel_entry *sel_entry,
                                                          uint8_t sel_record_type,
                                                          char *tmpbuf,
                                                          unsigned int tmpbuflen,
                                                          unsigned int flags,
                                                          unsigned int *wlen,
                                                          struct ipmi_sel_system_event_record_data *system_event_record_data)
{
  int ret;

  assert (ctx);
  assert (ctx->magic == IPMI_SEL_CTX_MAGIC);
  assert (ctx->manufacturer_id == IPMI_IANA_ENTERPRISE_ID_INVENTEC);
  assert (sel_entry);
  assert (tmpbuf);
  assert (tmpbuflen);
  assert (!(flags & ~IPMI_SEL_STRING_FLAGS_MASK));
  assert (flags & IPMI_SEL_STRING_FLAGS_INTERPRET_OEM_DATA);
  assert (wlen);
  assert (system_event_record_data);
  assert (system_event_record_data->event_data2_flag == IPMI_SEL_EVENT_DATA_OEM_CODE);
  assert (ctx->product_id == IPMI_INVENTEC_PRODUCT_ID_5442);

  /*
   * Inventec 5442/Dell Xanadu III
   */
  if ((ret = sel_string_output_inventec_5441_5442_common_event_data2_discrete_oem (ctx,
                                                                                   sel_entry,
                                                                                   sel_record_type,
                                                                                   tmpbuf,
                                                                                   tmpbuflen,
                                                                                   flags,
                                                                                   wlen,
                                                                                   system_event_record_data)) < 0)
    return (-1);

  if (ret)
    return (ret);

  return (0);
}

/* return (0) - no OEM match
 * return (1) - OEM match
 * return (-1) - error, cleanup and return error
 */
int
sel_string_output_inventec_5442_event_data2_class_oem (ipmi_sel_ctx_t ctx,
                                                       struct ipmi_sel_entry *sel_entry,
                                                       uint8_t sel_record_type,
                                                       char *tmpbuf,
                                                       unsigned int tmpbuflen,
                                                       unsigned int flags,
                                                       unsigned int *wlen,
                                                       struct ipmi_sel_system_event_record_data *system_event_record_data)
{
  int ret;

  assert (ctx);
  assert (ctx->magic == IPMI_SEL_CTX_MAGIC);
  assert (ctx->manufacturer_id == IPMI_IANA_ENTERPRISE_ID_INVENTEC);
  assert (sel_entry);
  assert (tmpbuf);
  assert (tmpbuflen);
  assert (!(flags & ~IPMI_SEL_STRING_FLAGS_MASK));
  assert (flags & IPMI_SEL_STRING_FLAGS_INTERPRET_OEM_DATA);
  assert (wlen);
  assert (system_event_record_data);
  assert (ctx->product_id == IPMI_INVENTEC_PRODUCT_ID_5442);

  /*
   * Inventec 5442/Dell Xanadu III
   */
  if ((ret = sel_string_output_inventec_5441_5442_common_event_data2_class_oem (ctx,
                                                                                sel_entry,
                                                                                sel_record_type,
                                                                                tmpbuf,
                                                                                tmpbuflen,
                                                                                flags,
                                                                                wlen,
                                                                                system_event_record_data)) < 0)
    return (-1);

  if (ret)
    return (1);

  return (0);
}

/* return (0) - no OEM match
 * return (1) - OEM match
 * return (-1) - error, cleanup and return error
 */
int
sel_string_output_inventec_5442_event_data3_discrete_oem (ipmi_sel_ctx_t ctx,
                                                          struct ipmi_sel_entry *sel_entry,
                                                          uint8_t sel_record_type,
                                                          char *tmpbuf,
                                                          unsigned int tmpbuflen,
                                                          unsigned int flags,
                                                          unsigned int *wlen,
                                                          struct ipmi_sel_system_event_record_data *system_event_record_data)
{
  int ret;

  assert (ctx);
  assert (ctx->magic == IPMI_SEL_CTX_MAGIC);
  assert (ctx->manufacturer_id == IPMI_IANA_ENTERPRISE_ID_INVENTEC);
  assert (sel_entry);
  assert (tmpbuf);
  assert (tmpbuflen);
  assert (!(flags & ~IPMI_SEL_STRING_FLAGS_MASK));
  assert (flags & IPMI_SEL_STRING_FLAGS_INTERPRET_OEM_DATA);
  assert (wlen);
  assert (system_event_record_data);
  assert (system_event_record_data->event_data3_flag == IPMI_SEL_EVENT_DATA_OEM_CODE);
  assert (ctx->product_id == IPMI_INVENTEC_PRODUCT_ID_5442);

  /*
   * Inventec 5442/Dell Xanadu III
   */

  if (ctx->product_id == IPMI_INVENTEC_PRODUCT_ID_5442)
    {
      if (system_event_record_data->generator_id == IPMI_GENERATOR_ID_OEM_INVENTEC_SMI
          && system_event_record_data->event_type_code == IPMI_EVENT_READING_TYPE_CODE_SENSOR_SPECIFIC
          && system_event_record_data->sensor_type == IPMI_SENSOR_TYPE_MEMORY
          && system_event_record_data->sensor_number == IPMI_SENSOR_NUMBER_OEM_INVENTEC_MEMORY
          && (system_event_record_data->offset_from_event_reading_type_code == IPMI_SENSOR_TYPE_MEMORY_CORRECTABLE_MEMORY_ERROR
              || system_event_record_data->offset_from_event_reading_type_code == IPMI_SENSOR_TYPE_MEMORY_UNCORRECTABLE_MEMORY_ERROR
              || system_event_record_data->offset_from_event_reading_type_code == IPMI_SENSOR_TYPE_MEMORY_PARITY
              || system_event_record_data->offset_from_event_reading_type_code == IPMI_SENSOR_TYPE_MEMORY_CORRECTABLE_MEMORY_ERROR_LOGGING_LIMIT_REACHED)
          && (system_event_record_data->event_data3 == IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA3_OEM_INVENTEC_5442_DIMM_CPU0_CH0_DIM1
              || system_event_record_data->event_data3 == IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA3_OEM_INVENTEC_5442_DIMM_CPU0_CH0_DIM0
              || system_event_record_data->event_data3 == IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA3_OEM_INVENTEC_5442_DIMM_CPU0_CH1_DIM1
              || system_event_record_data->event_data3 == IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA3_OEM_INVENTEC_5442_DIMM_CPU0_CH1_DIM0
              || system_event_record_data->event_data3 == IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA3_OEM_INVENTEC_5442_DIMM_CPU0_CH2_DIM1
              || system_event_record_data->event_data3 == IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA3_OEM_INVENTEC_5442_DIMM_CPU0_CH2_DIM0
              || system_event_record_data->event_data3 == IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA3_OEM_INVENTEC_5442_DIMM_CPU1_CH0_DIM1
              || system_event_record_data->event_data3 == IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA3_OEM_INVENTEC_5442_DIMM_CPU1_CH0_DIM0
              || system_event_record_data->event_data3 == IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA3_OEM_INVENTEC_5442_DIMM_CPU1_CH1_DIM1
              || system_event_record_data->event_data3 == IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA3_OEM_INVENTEC_5442_DIMM_CPU1_CH1_DIM0
              || system_event_record_data->event_data3 == IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA3_OEM_INVENTEC_5442_DIMM_CPU1_CH2_DIM1
              || system_event_record_data->event_data3 == IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA3_OEM_INVENTEC_5442_DIMM_CPU1_CH2_DIM0))
        {
          char *str;

          /* achu: technically there is a pattern here for this
           * motherboard and I could make some of this code shorter
           * w/ some bitmasking.  But I'll just brute force it out for
           * readability.  I'll revisit as more motherboards are added
           * later.
           */

          switch (system_event_record_data->event_data3)
            {
            case IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA3_OEM_INVENTEC_5442_DIMM_CPU0_CH0_DIM1:
              str = "Dimm Number - CPU0/Ch0/DIM1";
              break;
            case IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA3_OEM_INVENTEC_5442_DIMM_CPU0_CH0_DIM0:
              str = "Dimm Number - CPU0/Ch0/DIM0";
              break;
            case IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA3_OEM_INVENTEC_5442_DIMM_CPU0_CH1_DIM1:
              str = "Dimm Number - CPU0/Ch1/DIM1";
              break;
            case IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA3_OEM_INVENTEC_5442_DIMM_CPU0_CH1_DIM0:
              str = "Dimm Number - CPU0/Ch1/DIM0";
              break;
            case IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA3_OEM_INVENTEC_5442_DIMM_CPU0_CH2_DIM1:
              str = "Dimm Number - CPU0/Ch2/DIM1";
              break;
            case IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA3_OEM_INVENTEC_5442_DIMM_CPU0_CH2_DIM0:
              str = "Dimm Number - CPU0/Ch2/DIM0";
              break;
            case IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA3_OEM_INVENTEC_5442_DIMM_CPU1_CH0_DIM1:
              str = "Dimm Number - CPU1/Ch0/DIM1";
              break;
            case IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA3_OEM_INVENTEC_5442_DIMM_CPU1_CH0_DIM0:
              str = "Dimm Number - CPU1/Ch0/DIM0";
              break;
            case IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA3_OEM_INVENTEC_5442_DIMM_CPU1_CH1_DIM1:
              str = "Dimm Number - CPU1/Ch1/DIM1";
              break;
            case IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA3_OEM_INVENTEC_5442_DIMM_CPU1_CH1_DIM0:
              str = "Dimm Number - CPU1/Ch1/DIM0";
              break;
            case IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA3_OEM_INVENTEC_5442_DIMM_CPU1_CH2_DIM1:
              str = "Dimm Number - CPU1/Ch2/DIM1";
              break;
            case IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA3_OEM_INVENTEC_5442_DIMM_CPU1_CH2_DIM0:
              str = "Dimm Number - CPU1/Ch2/DIM0";
              break;
            default:
              str = "Internal Logic Error";
            }

          snprintf (tmpbuf, tmpbuflen, "%s", str);

          return (1);
        }
    }

  /*
   * Inventec 5442/Dell Xanadu III
   */

  if ((ret = sel_string_output_inventec_5441_5442_common_event_data3_discrete_oem (ctx,
                                                                                   sel_entry,
                                                                                   sel_record_type,
                                                                                   tmpbuf,
                                                                                   tmpbuflen,
                                                                                   flags,
                                                                                   wlen,
                                                                                   system_event_record_data)) < 0)
    return (-1);

  if (ret)
    return (1);

  return (0);
}

/* return (0) - no OEM match
 * return (1) - OEM match
 * return (-1) - error, cleanup and return error
 */
int
sel_string_output_inventec_5442_event_data3_class_oem (ipmi_sel_ctx_t ctx,
                                                       struct ipmi_sel_entry *sel_entry,
                                                       uint8_t sel_record_type,
                                                       char *tmpbuf,
                                                       unsigned int tmpbuflen,
                                                       unsigned int flags,
                                                       unsigned int *wlen,
                                                       struct ipmi_sel_system_event_record_data *system_event_record_data)
{
  int ret;

  assert (ctx);
  assert (ctx->magic == IPMI_SEL_CTX_MAGIC);
  assert (ctx->manufacturer_id == IPMI_IANA_ENTERPRISE_ID_INVENTEC);
  assert (sel_entry);
  assert (tmpbuf);
  assert (tmpbuflen);
  assert (!(flags & ~IPMI_SEL_STRING_FLAGS_MASK));
  assert (flags & IPMI_SEL_STRING_FLAGS_INTERPRET_OEM_DATA);
  assert (wlen);
  assert (system_event_record_data);
  assert (ctx->product_id == IPMI_INVENTEC_PRODUCT_ID_5442);

  /*
   * Inventec 5442/Dell Xanadu III
   */
  if ((ret = sel_string_output_inventec_5441_5442_common_event_data3_class_oem (ctx,
                                                                                sel_entry,
                                                                                sel_record_type,
                                                                                tmpbuf,
                                                                                tmpbuflen,
                                                                                flags,
                                                                                wlen,
                                                                                system_event_record_data)) < 0)
    return (-1);

  if (ret)
    return (1);

  return (0);
}

/* return (0) - no OEM match
 * return (1) - OEM match
 * return (-1) - error, cleanup and return error
 *
 * in oem_rv, return
 * 0 - continue on
 * 1 - buffer full, return full buffer to user
 */
int
sel_string_output_inventec_5442_event_data2_event_data3 (ipmi_sel_ctx_t ctx,
                                                         struct ipmi_sel_entry *sel_entry,
                                                         uint8_t sel_record_type,
                                                         char *buf,
                                                         unsigned int buflen,
                                                         unsigned int flags,
                                                         unsigned int *wlen,
                                                         struct ipmi_sel_system_event_record_data *system_event_record_data,
                                                         int *oem_rv)
{
  int ret;

  assert (ctx);
  assert (ctx->magic == IPMI_SEL_CTX_MAGIC);
  assert (ctx->manufacturer_id == IPMI_IANA_ENTERPRISE_ID_INVENTEC);
  assert (sel_entry);
  assert (buf);
  assert (buflen);
  assert (!(flags & ~IPMI_SEL_STRING_FLAGS_MASK));
  assert (flags & IPMI_SEL_STRING_FLAGS_INTERPRET_OEM_DATA);
  assert (wlen);
  assert (system_event_record_data);
  assert (oem_rv);
  assert (ctx->product_id == IPMI_INVENTEC_PRODUCT_ID_5442);

  /*
   * Inventec 5442/Dell Xanadu III
   */
  if ((ret = sel_string_output_inventec_5441_5442_common_event_data2_event_data3 (ctx,
                                                                                  sel_entry,
                                                                                  sel_record_type,
                                                                                  buf,
                                                                                  buflen,
                                                                                  flags,
                                                                                  wlen,
                                                                                  system_event_record_data,
                                                                                  oem_rv)) < 0)
    return (-1);

  if (ret)
    return (1);

  return (0);
}

struct sel_string_oem sel_string_oem_inventec_5442 =
  {
    sel_string_output_inventec_5442_sensor_name,
    NULL,
    sel_string_output_inventec_5442_event_data1_class_oem,
    NULL,
    sel_string_output_inventec_5442_event_data2_discrete_oem,
    sel_string_output_inventec_5442_event_data2_class_oem,
    NULL,
    sel_string_output_inventec_5442_event_data3_discrete_oem,
    sel_string_output_inventec_5442_event_data3_class_oem,
    NULL,
    NULL,
    NULL,
  };
