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

#include "freeipmi/cmds/ipmi-oem-intel-node-manager-cmds.h"
#include "freeipmi/sdr/oem/ipmi-sdr-oem-intel-node-manager.h"
#include "freeipmi/record-format/ipmi-sdr-record-format.h"
#include "freeipmi/record-format/ipmi-sel-record-format.h"
#include "freeipmi/spec/ipmi-event-reading-type-code-spec.h"
#include "freeipmi/spec/ipmi-iana-enterprise-numbers-spec.h"
#include "freeipmi/spec/ipmi-product-id-spec.h"
#include "freeipmi/spec/oem/ipmi-event-reading-type-code-oem-intel-node-manager-spec.h"
#include "freeipmi/spec/oem/ipmi-sensor-and-event-code-tables-oem-intel-node-manager-spec.h"
#include "freeipmi/spec/oem/ipmi-sensor-numbers-oem-intel-node-manager-spec.h"
#include "freeipmi/spec/oem/ipmi-sensor-types-oem-intel-node-manager-spec.h"

#include "ipmi-sel-common.h"
#include "ipmi-sel-defs.h"
#include "ipmi-sel-string.h"
#include "ipmi-sel-string-intel-node-manager.h"
#include "ipmi-sel-trace.h"
#include "ipmi-sel-util.h"

#include "freeipmi-portability.h"

/*
 * Intel Node Manager
 *
 * For Intel Chips, not just Intel Motherboards.  Confirmed for:
 *
 * Intel S5500WB/Penguin Computing Relion 700
 * Intel S2600JF/Appro 512X
 * Intel S2600WP
 * Intel S2600KP
 * Intel S2600WT2
 * Intel S2600WTT
 * Intel S2600GZ
 * Inventec 5441/Dell Xanadu II
 * Inventec 5442/Dell Xanadu III
 * Quanta S99Q/Dell FS12-TY
 * Quanta QSSC-S4R/Appro GB812X-CN
 *
 * Should be called from ipmi-sel-string-VENDOR.c files, not
 * from ipmi-sel-string.c.
 */

struct intel_node_manager_sdr_callback
{
  ipmi_sel_ctx_t ctx;
  int found;
};

#define INTEL_NODE_MANAGER_EVENT_BUFFER_LENGTH 4096

/* achu:
 *
 * In Intel NM 2.0 specification, sensor numbers are now fixed and you
 * don't have to search the SDR for them.  We could check version of
 * NM on motherboard to determine if we need to search SDR or not, but
 * for time being we'll stick to the search SDR method b/c it will
 * always work.
 */
static int
_intel_node_manager_sdr_callback (ipmi_sdr_ctx_t sdr_ctx,
                                  uint8_t record_type,
                                  const void *sdr_record,
                                  unsigned int sdr_record_len,
                                  void *arg)
{
  struct intel_node_manager_sdr_callback *sdr_callback_arg;
  ipmi_sel_ctx_t ctx;
  int ret;
  int rv = -1;

  assert (sdr_ctx);
  assert (sdr_record);
  assert (sdr_record_len);
  assert (arg);

  sdr_callback_arg = (struct intel_node_manager_sdr_callback *)arg;
  ctx = sdr_callback_arg->ctx;

  if (record_type != IPMI_SDR_FORMAT_OEM_RECORD)
    {
      rv = 0;
      goto cleanup;
    }

  if ((ret = ipmi_sdr_oem_parse_intel_node_manager (sdr_ctx,
                                                    sdr_record,
                                                    sdr_record_len,
                                                    NULL,
                                                    NULL,
                                                    NULL,
                                                    &ctx->intel_node_manager.nm_health_event_sensor_number,
                                                    &ctx->intel_node_manager.nm_exception_event_sensor_number,
                                                    &ctx->intel_node_manager.nm_operational_capabilities_sensor_number,
                                                    &ctx->intel_node_manager.nm_alert_threshold_exceeded_sensor_number)) < 0)
    goto cleanup;

  if (ret)
    {
      ctx->intel_node_manager.node_manager_data_parsed = 1;
      ctx->intel_node_manager.node_manager_data_found = 1;
      sdr_callback_arg->found = 1;
    }
  rv = ret;

 cleanup:
  return (rv);
}

/* return (1) - Node Manager SDR entry found
 * return (0) - No Node Manager SDR entry found
 * return (-1) - error, cleanup and return error
 */
static int
_intel_node_manager_init (ipmi_sel_ctx_t ctx)
{
  struct intel_node_manager_sdr_callback sdr_callback_arg;
  int rv = -1;

  assert (ctx);
  assert (ctx->magic == IPMI_SEL_CTX_MAGIC);

  if (!ctx->sdr_ctx)
    return (0);

  if (ctx->intel_node_manager.node_manager_data_parsed)
    return (ctx->intel_node_manager.node_manager_data_found);

  sdr_callback_arg.ctx = ctx;
  sdr_callback_arg.found = 0;

  if (ipmi_sdr_cache_iterate (ctx->sdr_ctx,
                              _intel_node_manager_sdr_callback,
                              &sdr_callback_arg) < 0)
    goto cleanup;

  if (!sdr_callback_arg.found)
    {
      ctx->intel_node_manager.node_manager_data_parsed = 1;
      ctx->intel_node_manager.node_manager_data_found = 0;
    }

  rv = sdr_callback_arg.found;
 cleanup:
  return (rv);
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
sel_string_output_intel_node_manager_sensor_name (ipmi_sel_ctx_t ctx,
                                                  struct ipmi_sel_entry *sel_entry,
                                                  uint8_t sel_record_type,
                                                  char *buf,
                                                  unsigned int buflen,
                                                  unsigned int flags,
                                                  unsigned int *wlen,
                                                  struct ipmi_sel_system_event_record_data *system_event_record_data,
                                                  int *oem_rv)
{
  assert (ctx);
  assert (ctx->magic == IPMI_SEL_CTX_MAGIC);
  assert (sel_entry);
  assert (buf);
  assert (buflen);
  assert (!(flags & ~IPMI_SEL_STRING_FLAGS_MASK));
  assert (flags & IPMI_SEL_STRING_FLAGS_INTERPRET_OEM_DATA);
  assert (wlen);
  assert (system_event_record_data);
  assert (oem_rv);

  /* achu: As far as I can tell, SDR is required to have Node Manager
   * OEM SDR, but is not required to have "Event Only" records for the
   * individual event types.  It appears most motherboards do have SDR
   * entries for these events, but it's best to cover all cases if
   * necessary.  In upper level calls, if an SDR entry is available,
   * it will be found before reaching this point.
   */

  if (system_event_record_data->sensor_type == IPMI_SENSOR_TYPE_OEM_INTEL_NODE_MANAGER)
    {
      int nm_found;

      if ((nm_found = _intel_node_manager_init (ctx)) < 0)
        return (-1);

      if (!nm_found)
        goto out;

      /* achu: On the first motherboards I found w/ NM support, these are the sensor names I've seen.
       *
       * NM Capabilities
       * NM Exception
       * NM Health
       * NM Threshold
       *
       * So we'll copy them.  I've found no common name for the
       * IPMI_EVENT_READING_TYPE_CODE_OEM_INTEL_ME_FIRMWARE_HEALTH_EVENT
       * event, so we'll call it "NM Firmware".
       */

      if (system_event_record_data->event_type_code == IPMI_EVENT_READING_TYPE_CODE_OEM_INTEL_NODE_MANAGER_HEALTH_EVENT
          && system_event_record_data->sensor_number == ctx->intel_node_manager.nm_health_event_sensor_number)
        {
          if (sel_string_snprintf (buf,
                                   buflen,
                                   wlen,
                                   "NM Health"))
            (*oem_rv) = 1;
          else
            (*oem_rv) = 0;

          return (1);
        }
      else if (system_event_record_data->event_type_code == IPMI_EVENT_READING_TYPE_CODE_OEM_INTEL_NODE_MANAGER_EXCEPTION_EVENT
               && system_event_record_data->sensor_number == ctx->intel_node_manager.nm_exception_event_sensor_number)
        {
          if (sel_string_snprintf (buf,
                                   buflen,
                                   wlen,
                                   "NM Exception"))
            (*oem_rv) = 1;
          else
            (*oem_rv) = 0;

          return (1);
        }
      else if (system_event_record_data->event_type_code == IPMI_EVENT_READING_TYPE_CODE_OEM_INTEL_NODE_MANAGER_OPERATIONAL_CAPABILITIES_CHANGE_EVENT
               && system_event_record_data->sensor_number == ctx->intel_node_manager.nm_operational_capabilities_sensor_number)
        {
          if (sel_string_snprintf (buf,
                                   buflen,
                                   wlen,
                                   "NM Capabilities"))
            (*oem_rv) = 1;
          else
            (*oem_rv) = 0;

          return (1);
        }
      else if (system_event_record_data->event_type_code == IPMI_EVENT_READING_TYPE_CODE_OEM_INTEL_NODE_MANAGER_ALERT_THRESHOLD_EXCEEDED
               && system_event_record_data->sensor_number == ctx->intel_node_manager.nm_alert_threshold_exceeded_sensor_number)
        {
          if (sel_string_snprintf (buf,
                                   buflen,
                                   wlen,
                                   "NM Threshold"))
            (*oem_rv) = 1;
          else
            (*oem_rv) = 0;

          return (1);
        }
      else if (system_event_record_data->event_type_code == IPMI_EVENT_READING_TYPE_CODE_OEM_INTEL_ME_FIRMWARE_HEALTH_EVENT
               && system_event_record_data->sensor_number == IPMI_SENSOR_NUMBER_OEM_INTEL_ME_FIRMWARE_HEALTH_EVENT)
        {
          if (sel_string_snprintf (buf,
                                   buflen,
                                   wlen,
                                   "NM Firmware"))
            (*oem_rv) = 1;
          else
            (*oem_rv) = 0;

          return (1);
        }
    }

 out:
  return (0);
}

/* return (0) - no OEM match
 * return (1) - OEM match
 * return (-1) - error, cleanup and return error
 */
int
sel_string_output_intel_node_manager_event_data1_class_oem (ipmi_sel_ctx_t ctx,
                                                            struct ipmi_sel_entry *sel_entry,
                                                            uint8_t sel_record_type,
                                                            char *tmpbuf,
                                                            unsigned int tmpbuflen,
                                                            unsigned int flags,
                                                            unsigned int *wlen,
                                                            struct ipmi_sel_system_event_record_data *system_event_record_data)
{
  assert (ctx);
  assert (ctx->magic == IPMI_SEL_CTX_MAGIC);
  assert (sel_entry);
  assert (tmpbuf);
  assert (tmpbuflen);
  assert (!(flags & ~IPMI_SEL_STRING_FLAGS_MASK));
  assert (flags & IPMI_SEL_STRING_FLAGS_INTERPRET_OEM_DATA);
  assert (wlen);
  assert (system_event_record_data);

  if (system_event_record_data->sensor_type == IPMI_SENSOR_TYPE_OEM_INTEL_NODE_MANAGER_THERMAL_SENSOR_ON_DIMM
      && system_event_record_data->sensor_number == IPMI_SENSOR_NUMBER_OEM_INTEL_NODE_MANAGER_THERMAL_SENSOR_ON_DIMM_SMBUS_STATUS
      && system_event_record_data->event_type_code == IPMI_EVENT_READING_TYPE_CODE_OEM_INTEL_THERMAL_SENSOR_ON_DIMM)
    {
      uint8_t cpu0_mc0;
      int cpu0_mc0_available;
      uint8_t cpu0_mc1;
      int cpu0_mc1_available;
      uint8_t cpu1_mc0;
      int cpu1_mc0_available;
      uint8_t cpu1_mc1;
      int cpu1_mc1_available;
      uint8_t cpu2_mc0;
      int cpu2_mc0_available;
      uint8_t cpu2_mc1;
      int cpu2_mc1_available;
      uint8_t cpu3_mc0;
      int cpu3_mc0_available;
      uint8_t cpu3_mc1;
      int cpu3_mc1_available;

      cpu0_mc0 = system_event_record_data->event_data1 & IPMI_OEM_INTEL_NODE_MANAGER_THERMAL_SENSOR_ON_DIMM_CPU_0_MEMORY_CONTROLLER_0_BITMASK;
      cpu0_mc0 >>= IPMI_OEM_INTEL_NODE_MANAGER_THERMAL_SENSOR_ON_DIMM_CPU_0_MEMORY_CONTROLLER_0_SHIFT;

      cpu0_mc0_available = (cpu0_mc0 == IPMI_OEM_INTEL_NODE_MANAGER_THERMAL_SENSOR_ON_DIMM_AVAILABLE) ? 1 : 0;

      cpu0_mc1 = system_event_record_data->event_data1 & IPMI_OEM_INTEL_NODE_MANAGER_THERMAL_SENSOR_ON_DIMM_CPU_0_MEMORY_CONTROLLER_1_BITMASK;
      cpu0_mc1 >>= IPMI_OEM_INTEL_NODE_MANAGER_THERMAL_SENSOR_ON_DIMM_CPU_0_MEMORY_CONTROLLER_1_SHIFT;

      cpu0_mc1_available = (cpu0_mc0 == IPMI_OEM_INTEL_NODE_MANAGER_THERMAL_SENSOR_ON_DIMM_AVAILABLE) ? 1 : 0;

      cpu1_mc0 = system_event_record_data->event_data1 & IPMI_OEM_INTEL_NODE_MANAGER_THERMAL_SENSOR_ON_DIMM_CPU_1_MEMORY_CONTROLLER_0_BITMASK;
      cpu1_mc0 >>= IPMI_OEM_INTEL_NODE_MANAGER_THERMAL_SENSOR_ON_DIMM_CPU_1_MEMORY_CONTROLLER_0_SHIFT;

      cpu1_mc0_available = (cpu0_mc0 == IPMI_OEM_INTEL_NODE_MANAGER_THERMAL_SENSOR_ON_DIMM_AVAILABLE) ? 1 : 0;

      cpu1_mc1 = system_event_record_data->event_data1 & IPMI_OEM_INTEL_NODE_MANAGER_THERMAL_SENSOR_ON_DIMM_CPU_1_MEMORY_CONTROLLER_1_BITMASK;
      cpu1_mc1 >>= IPMI_OEM_INTEL_NODE_MANAGER_THERMAL_SENSOR_ON_DIMM_CPU_1_MEMORY_CONTROLLER_1_SHIFT;

      cpu1_mc1_available = (cpu0_mc0 == IPMI_OEM_INTEL_NODE_MANAGER_THERMAL_SENSOR_ON_DIMM_AVAILABLE) ? 1 : 0;

      cpu2_mc0 = system_event_record_data->event_data1 & IPMI_OEM_INTEL_NODE_MANAGER_THERMAL_SENSOR_ON_DIMM_CPU_2_MEMORY_CONTROLLER_0_BITMASK;
      cpu2_mc0 >>= IPMI_OEM_INTEL_NODE_MANAGER_THERMAL_SENSOR_ON_DIMM_CPU_2_MEMORY_CONTROLLER_0_SHIFT;

      cpu2_mc0_available = (cpu0_mc0 == IPMI_OEM_INTEL_NODE_MANAGER_THERMAL_SENSOR_ON_DIMM_AVAILABLE) ? 1 : 0;

      cpu2_mc1 = system_event_record_data->event_data1 & IPMI_OEM_INTEL_NODE_MANAGER_THERMAL_SENSOR_ON_DIMM_CPU_2_MEMORY_CONTROLLER_1_BITMASK;
      cpu2_mc1 >>= IPMI_OEM_INTEL_NODE_MANAGER_THERMAL_SENSOR_ON_DIMM_CPU_2_MEMORY_CONTROLLER_1_SHIFT;

      cpu2_mc1_available = (cpu0_mc0 == IPMI_OEM_INTEL_NODE_MANAGER_THERMAL_SENSOR_ON_DIMM_AVAILABLE) ? 1 : 0;

      cpu3_mc0 = system_event_record_data->event_data1 & IPMI_OEM_INTEL_NODE_MANAGER_THERMAL_SENSOR_ON_DIMM_CPU_3_MEMORY_CONTROLLER_0_BITMASK;
      cpu3_mc0 >>= IPMI_OEM_INTEL_NODE_MANAGER_THERMAL_SENSOR_ON_DIMM_CPU_3_MEMORY_CONTROLLER_0_SHIFT;

      cpu3_mc0_available = (cpu0_mc0 == IPMI_OEM_INTEL_NODE_MANAGER_THERMAL_SENSOR_ON_DIMM_AVAILABLE) ? 1 : 0;

      cpu3_mc1 = system_event_record_data->event_data1 & IPMI_OEM_INTEL_NODE_MANAGER_THERMAL_SENSOR_ON_DIMM_CPU_3_MEMORY_CONTROLLER_1_BITMASK;
      cpu3_mc1 >>= IPMI_OEM_INTEL_NODE_MANAGER_THERMAL_SENSOR_ON_DIMM_CPU_3_MEMORY_CONTROLLER_1_SHIFT;

      cpu3_mc1_available = (cpu0_mc0 == IPMI_OEM_INTEL_NODE_MANAGER_THERMAL_SENSOR_ON_DIMM_AVAILABLE) ? 1 : 0;

      snprintf (tmpbuf,
                tmpbuflen,
                "CPU0/MC0 = %s, CPU0/MC1 = %s, CPU1/MC0 = %s, CPU1/MC1 = %s, CPU2/MC0 = %s, CPU2/MC1 = %s, CPU3/MC0 = %s, CPU3/MC1 = %s",
                cpu0_mc0_available ? "Available" : "Not Available",
                cpu0_mc1_available ? "Available" : "Not Available",
                cpu1_mc0_available ? "Available" : "Not Available",
                cpu1_mc1_available ? "Available" : "Not Available",
                cpu2_mc0_available ? "Available" : "Not Available",
                cpu2_mc1_available ? "Available" : "Not Available",
                cpu3_mc0_available ? "Available" : "Not Available",
                cpu3_mc1_available ? "Available" : "Not Available");

      return (1);
    }

  if (system_event_record_data->sensor_type == IPMI_SENSOR_TYPE_OEM_INTEL_NODE_MANAGER)
    {
      int nm_found;
      uint8_t node_manager_policy_event;

      if ((nm_found = _intel_node_manager_init (ctx)) < 0)
        return (-1);

      if (!nm_found)
        goto out;

      node_manager_policy_event = system_event_record_data->offset_from_event_reading_type_code & IPMI_OEM_INTEL_NODE_MANAGER_EXCEPTION_EVENT_EVENT_DATA1_NODE_MANAGER_POLICY_EVENT_BITMASK;
      node_manager_policy_event >>= IPMI_OEM_INTEL_NODE_MANAGER_EXCEPTION_EVENT_EVENT_DATA1_NODE_MANAGER_POLICY_EVENT_SHIFT;

      if (system_event_record_data->event_type_code == IPMI_EVENT_READING_TYPE_CODE_OEM_INTEL_NODE_MANAGER_EXCEPTION_EVENT
          && system_event_record_data->sensor_number == ctx->intel_node_manager.nm_exception_event_sensor_number
          && node_manager_policy_event == IPMI_OEM_INTEL_NODE_MANAGER_EXCEPTION_EVENT_EVENT_DATA1_NODE_MANAGER_POLICY_EVENT_POLICY_CORRECTION_TIME_EXCEEDED)
        {
          snprintf (tmpbuf,
                    tmpbuflen,
                    "Policy Correction Time Exceeded");

          return (1);
        }

      if (system_event_record_data->event_type_code == IPMI_EVENT_READING_TYPE_CODE_OEM_INTEL_NODE_MANAGER_HEALTH_EVENT
          && system_event_record_data->sensor_number == ctx->intel_node_manager.nm_health_event_sensor_number
          && system_event_record_data->offset_from_event_reading_type_code == IPMI_OEM_INTEL_NODE_MANAGER_HEALTH_EVENT_SENSOR_NODE_MANAGER)
        {
          snprintf (tmpbuf,
                    tmpbuflen,
                    "Sensor Node Manager");

          return (1);
        }

      if (system_event_record_data->event_type_code == IPMI_EVENT_READING_TYPE_CODE_OEM_INTEL_NODE_MANAGER_OPERATIONAL_CAPABILITIES_CHANGE_EVENT
          && system_event_record_data->sensor_number == ctx->intel_node_manager.nm_operational_capabilities_sensor_number)
        {
          uint8_t policy_interface_capability;
          uint8_t monitoring_capability;
          uint8_t power_limiting_capability;

          policy_interface_capability = system_event_record_data->offset_from_event_reading_type_code & IPMI_OEM_INTEL_NODE_MANAGER_OPERATIONAL_CAPABILITIES_CHANGE_EVENT_EVENT_DATA1_POLICY_INTERFACE_CAPABILITY_BITMASK;
          policy_interface_capability >>= IPMI_OEM_INTEL_NODE_MANAGER_OPERATIONAL_CAPABILITIES_CHANGE_EVENT_EVENT_DATA1_POLICY_INTERFACE_CAPABILITY_SHIFT;

          monitoring_capability = system_event_record_data->offset_from_event_reading_type_code & IPMI_OEM_INTEL_NODE_MANAGER_OPERATIONAL_CAPABILITIES_CHANGE_EVENT_EVENT_DATA1_MONITORING_CAPABILITY_BITMASK;
          monitoring_capability >>= IPMI_OEM_INTEL_NODE_MANAGER_OPERATIONAL_CAPABILITIES_CHANGE_EVENT_EVENT_DATA1_MONITORING_CAPABILITY_SHIFT;

          power_limiting_capability = system_event_record_data->offset_from_event_reading_type_code & IPMI_OEM_INTEL_NODE_MANAGER_OPERATIONAL_CAPABILITIES_CHANGE_EVENT_EVENT_DATA1_POWER_LIMITING_CAPABILITY_BITMASK;
          power_limiting_capability >>= IPMI_OEM_INTEL_NODE_MANAGER_OPERATIONAL_CAPABILITIES_CHANGE_EVENT_EVENT_DATA1_POWER_LIMITING_CAPABILITY_SHIFT;

          snprintf (tmpbuf,
                    tmpbuflen,
                    "Policy Interface = %s, Monitoring = %s, Power Limiting = %s",
                    policy_interface_capability ? "Available" : "Not Available",
                    monitoring_capability ? "Available" : "Not Available",
                    power_limiting_capability ? "Available" : "Not Available");

          return (1);
        }

      if (system_event_record_data->event_type_code == IPMI_EVENT_READING_TYPE_CODE_OEM_INTEL_NODE_MANAGER_ALERT_THRESHOLD_EXCEEDED
          && system_event_record_data->sensor_number == ctx->intel_node_manager.nm_alert_threshold_exceeded_sensor_number)
        {
          node_manager_policy_event = system_event_record_data->offset_from_event_reading_type_code & IPMI_OEM_INTEL_NODE_MANAGER_ALERT_THRESHOLD_EXCEEDED_EVENT_DATA1_NODE_MANAGER_POLICY_EVENT_BITMASK;
          node_manager_policy_event >>= IPMI_OEM_INTEL_NODE_MANAGER_ALERT_THRESHOLD_EXCEEDED_EVENT_DATA1_NODE_MANAGER_POLICY_EVENT_SHIFT;

          if (node_manager_policy_event == IPMI_OEM_INTEL_NODE_MANAGER_ALERT_THRESHOLD_EXCEEDED_EVENT_DATA1_NODE_MANAGER_POLICY_EVENT_THRESHOLD_EXCEEDED)
            {
              uint8_t threshold_number;

              threshold_number = system_event_record_data->offset_from_event_reading_type_code & IPMI_OEM_INTEL_NODE_MANAGER_ALERT_THRESHOLD_EXCEEDED_EVENT_DATA1_THRESHOLD_NUMBER_BITMASK;
              threshold_number >>= IPMI_OEM_INTEL_NODE_MANAGER_ALERT_THRESHOLD_EXCEEDED_EVENT_DATA1_THRESHOLD_NUMBER_SHIFT;

              snprintf (tmpbuf,
                        tmpbuflen,
                        "Threshold Exceeded, Threshold Number = %u",
                        threshold_number);

              return (1);

            }
          else /* node_manager_policy_event == IPMI_OEM_INTEL_NODE_MANAGER_ALERT_THRESHOLD_EXCEEDED_EVENT_DATA1_NODE_MANAGER_POLICY_EVENT_POLICY_CORRECTION_TIME_EXCEEDED */
            {
              snprintf (tmpbuf,
                        tmpbuflen,
                        "Policy Correction Time Exceeded");

              return (1);
            }
        }

      if (system_event_record_data->event_type_code == IPMI_EVENT_READING_TYPE_CODE_OEM_INTEL_ME_FIRMWARE_HEALTH_EVENT
          && system_event_record_data->sensor_number == IPMI_SENSOR_NUMBER_OEM_INTEL_ME_FIRMWARE_HEALTH_EVENT
          && system_event_record_data->offset_from_event_reading_type_code == IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_FIRMWARE_STATUS)
        {
          snprintf (tmpbuf,
                    tmpbuflen,
                    "Firmware Status");

          return (1);
        }

      if (system_event_record_data->event_type_code == IPMI_EVENT_READING_TYPE_CODE_OEM_INTEL_NODE_MANAGER_CUPS_EVENT
          && system_event_record_data->sensor_number == IPMI_SENSOR_NUMBER_OEM_INTEL_NODE_MANAGER_CUPS_EVENT_SENSOR)
        {
          uint8_t cups_event;

          cups_event = system_event_record_data->offset_from_event_reading_type_code & IPMI_OEM_INTEL_NODE_MANAGER_CUPS_EVENT_EVENT_DATA1_CUPS_POLICY_EVENT_BITMASK;
          cups_event >>= IPMI_OEM_INTEL_NODE_MANAGER_CUPS_EVENT_EVENT_DATA1_CUPS_POLICY_EVENT_SHIFT;

          if (cups_event == IPMI_OEM_INTEL_NODE_MANAGER_CUPS_EVENT_EVENT_DATA1_CUPS_EVENT_EVENT_THRESHOLD_EXCEEDED)
            {
              uint8_t threshold_number;

              threshold_number = system_event_record_data->offset_from_event_reading_type_code & IPMI_OEM_INTEL_NODE_MANAGER_CUPS_EVENT_EVENT_DATA1_THRESHOLD_NUMBER_BITMASK;
              threshold_number >>= IPMI_OEM_INTEL_NODE_MANAGER_CUPS_EVENT_EVENT_DATA1_THRESHOLD_NUMBER_SHIFT;

              snprintf (tmpbuf,
                        tmpbuflen,
                        "Threshold Exceeded, Threshold Number = %u",
                        threshold_number);

              return (1);

            }
        }
    }

 out:
  return (0);
}

static void
_sel_string_output_intel_node_manager_domain_id (ipmi_sel_ctx_t ctx,
                                                 char *domain_id_str,
                                                 unsigned int domain_id_str_len,
                                                 uint8_t domain_id)
{
  assert (ctx);
  assert (ctx->magic == IPMI_SEL_CTX_MAGIC);
  assert (domain_id_str);
  assert (domain_id_str_len);

  if (domain_id == IPMI_OEM_INTEL_NODE_MANAGER_DOMAIN_ID_ENTIRE_PLATFORM)
    snprintf (domain_id_str,
              domain_id_str_len,
              "Entire platform (%u)",
              domain_id);
  else if (domain_id == IPMI_OEM_INTEL_NODE_MANAGER_DOMAIN_ID_CPU_SUBSYSTEM)
    snprintf (domain_id_str,
              domain_id_str_len,
              "CPU subsystem (%u)",
              domain_id);
  else if (domain_id == IPMI_OEM_INTEL_NODE_MANAGER_DOMAIN_ID_MEMORY_SUBSYSTEM)
    snprintf (domain_id_str,
              domain_id_str_len,
              "Memory subsystem (%u)",
              domain_id);
  else if (domain_id == IPMI_OEM_INTEL_NODE_MANAGER_DOMAIN_ID_HIGH_POWER_IO_SUBSYSTEM)
    snprintf (domain_id_str,
              domain_id_str_len,
              "High Power I/O subsystem (%u)",
              domain_id);
  else
    snprintf (domain_id_str,
              domain_id_str_len,
              "%u",
              domain_id);

  return;
}

/* return (0) - no OEM match
 * return (1) - OEM match
 * return (-1) - error, cleanup and return error
 */
int
sel_string_output_intel_node_manager_event_data2_class_oem (ipmi_sel_ctx_t ctx,
                                                            struct ipmi_sel_entry *sel_entry,
                                                            uint8_t sel_record_type,
                                                            char *tmpbuf,
                                                            unsigned int tmpbuflen,
                                                            unsigned int flags,
                                                            unsigned int *wlen,
                                                            struct ipmi_sel_system_event_record_data *system_event_record_data)
{
  assert (ctx);
  assert (ctx->magic == IPMI_SEL_CTX_MAGIC);
  assert (sel_entry);
  assert (tmpbuf);
  assert (tmpbuflen);
  assert (!(flags & ~IPMI_SEL_STRING_FLAGS_MASK));
  assert (flags & IPMI_SEL_STRING_FLAGS_INTERPRET_OEM_DATA);
  assert (wlen);
  assert (system_event_record_data);

  if (system_event_record_data->sensor_type == IPMI_SENSOR_TYPE_OEM_INTEL_NODE_MANAGER)
    {
      int nm_found;
      uint8_t node_manager_policy_event;

      if ((nm_found = _intel_node_manager_init (ctx)) < 0)
        return (-1);

      if (!nm_found)
        goto out;

      node_manager_policy_event = system_event_record_data->offset_from_event_reading_type_code & IPMI_OEM_INTEL_NODE_MANAGER_EXCEPTION_EVENT_EVENT_DATA1_NODE_MANAGER_POLICY_EVENT_BITMASK;
      node_manager_policy_event >>= IPMI_OEM_INTEL_NODE_MANAGER_EXCEPTION_EVENT_EVENT_DATA1_NODE_MANAGER_POLICY_EVENT_SHIFT;

      if (system_event_record_data->event_type_code == IPMI_EVENT_READING_TYPE_CODE_OEM_INTEL_NODE_MANAGER_EXCEPTION_EVENT
          && system_event_record_data->sensor_number == ctx->intel_node_manager.nm_exception_event_sensor_number
          && node_manager_policy_event == IPMI_OEM_INTEL_NODE_MANAGER_EXCEPTION_EVENT_EVENT_DATA1_NODE_MANAGER_POLICY_EVENT_POLICY_CORRECTION_TIME_EXCEEDED)
        {
          uint8_t domain_id;
          char domain_id_str[INTEL_NODE_MANAGER_EVENT_BUFFER_LENGTH + 1];

          domain_id = (system_event_record_data->event_data2 & IPMI_OEM_INTEL_NODE_MANAGER_EXCEPTION_EVENT_EVENT_DATA2_DOMAIN_ID_BITMASK);
          domain_id >>= IPMI_OEM_INTEL_NODE_MANAGER_EXCEPTION_EVENT_EVENT_DATA2_DOMAIN_ID_SHIFT;

          memset (domain_id_str, '\0', INTEL_NODE_MANAGER_EVENT_BUFFER_LENGTH + 1);

          _sel_string_output_intel_node_manager_domain_id (ctx,
                                                           domain_id_str,
                                                           INTEL_NODE_MANAGER_EVENT_BUFFER_LENGTH,
                                                           domain_id);

          snprintf (tmpbuf,
                    tmpbuflen,
                    "Domain ID = %s",
                    domain_id_str);

          return (1);
        }

      if (system_event_record_data->event_type_code == IPMI_EVENT_READING_TYPE_CODE_OEM_INTEL_NODE_MANAGER_HEALTH_EVENT
          && system_event_record_data->sensor_number == ctx->intel_node_manager.nm_health_event_sensor_number
          && system_event_record_data->offset_from_event_reading_type_code == IPMI_OEM_INTEL_NODE_MANAGER_HEALTH_EVENT_SENSOR_NODE_MANAGER)
        {
          uint8_t domain_id;
          uint8_t error_type;
          char domain_id_str[INTEL_NODE_MANAGER_EVENT_BUFFER_LENGTH + 1];
          char *error_type_str;

          domain_id = (system_event_record_data->event_data2 & IPMI_OEM_INTEL_NODE_MANAGER_HEALTH_EVENT_EVENT_DATA2_DOMAIN_ID_BITMASK);
          domain_id >>= IPMI_OEM_INTEL_NODE_MANAGER_HEALTH_EVENT_EVENT_DATA2_DOMAIN_ID_SHIFT;

          error_type = (system_event_record_data->event_data2 & IPMI_OEM_INTEL_NODE_MANAGER_HEALTH_EVENT_EVENT_DATA2_ERROR_TYPE_BITMASK);
          error_type >>= IPMI_OEM_INTEL_NODE_MANAGER_HEALTH_EVENT_EVENT_DATA2_ERROR_TYPE_SHIFT;

          switch (error_type)
            {
            case IPMI_OEM_INTEL_NODE_MANAGER_HEALTH_EVENT_EVENT_DATA2_ERROR_TYPE_OUTLET_TEMPERATURE_READING_FAILURE:
              error_type_str = "Outlet Temperature Reading Failure";
              break;
            case IPMI_OEM_INTEL_NODE_MANAGER_HEALTH_EVENT_EVENT_DATA2_ERROR_TYPE_VOLUMETRIC_AIRFLOW_READING_FAILURE:
              error_type_str = "Volumetric Airflow Reading Failure";
              break;
            case IPMI_OEM_INTEL_NODE_MANAGER_HEALTH_EVENT_EVENT_DATA2_ERROR_TYPE_POLICY_MISCONFIGURATION:
              error_type_str = "Policy Misconfiguration";
              break;
            case IPMI_OEM_INTEL_NODE_MANAGER_HEALTH_EVENT_EVENT_DATA2_ERROR_TYPE_POWER_SENSOR_READING_FAILURE:
              error_type_str = "Power Sensor Reading Failure";
              break;
            case IPMI_OEM_INTEL_NODE_MANAGER_HEALTH_EVENT_EVENT_DATA2_ERROR_TYPE_INLET_TEMPERATURE_READING_FAILURE:
              error_type_str = "Inlet Temperature Reading Failure";
              break;
            case IPMI_OEM_INTEL_NODE_MANAGER_HEALTH_EVENT_EVENT_DATA2_ERROR_TYPE_HOST_COMMUNICATION_ERROR:
              error_type_str = "Host Communication error";
              break;
            case IPMI_OEM_INTEL_NODE_MANAGER_HEALTH_EVENT_EVENT_DATA2_ERROR_TYPE_REAL_TIME_CLOCK_SYNCHRONIZATION_FAILURE:
              error_type_str = "Real-time clock synchronization failure";
              break;
            case IPMI_OEM_INTEL_NODE_MANAGER_HEALTH_EVENT_EVENT_DATA2_ERROR_TYPE_PLATFORM_SHUTDOWN_INITIATED_BY_INTEL_NM_POLICY:
              error_type_str = "Platform shutdown initiated by Intel NM policy";
              break;
            default:
              error_type_str = "Unknown";
            }

          memset (domain_id_str, '\0', INTEL_NODE_MANAGER_EVENT_BUFFER_LENGTH + 1);

          _sel_string_output_intel_node_manager_domain_id (ctx,
                                                           domain_id_str,
                                                           INTEL_NODE_MANAGER_EVENT_BUFFER_LENGTH,
                                                           domain_id);

          snprintf (tmpbuf,
                    tmpbuflen,
                    "Domain ID = %s, Error Type = %s",
                    domain_id_str,
                    error_type_str);

          return (1);
        }

      if (system_event_record_data->event_type_code == IPMI_EVENT_READING_TYPE_CODE_OEM_INTEL_NODE_MANAGER_ALERT_THRESHOLD_EXCEEDED
          && system_event_record_data->sensor_number == ctx->intel_node_manager.nm_alert_threshold_exceeded_sensor_number)
        {
          uint8_t domain_id;
          char domain_id_str[INTEL_NODE_MANAGER_EVENT_BUFFER_LENGTH + 1];

          domain_id = (system_event_record_data->event_data2 & IPMI_OEM_INTEL_NODE_MANAGER_ALERT_THRESHOLD_EXCEEDED_EVENT_DATA2_DOMAIN_ID_BITMASK);
          domain_id >>= IPMI_OEM_INTEL_NODE_MANAGER_ALERT_THRESHOLD_EXCEEDED_EVENT_DATA2_DOMAIN_ID_SHIFT;

          memset (domain_id_str, '\0', INTEL_NODE_MANAGER_EVENT_BUFFER_LENGTH + 1);

          _sel_string_output_intel_node_manager_domain_id (ctx,
                                                           domain_id_str,
                                                           INTEL_NODE_MANAGER_EVENT_BUFFER_LENGTH,
                                                           domain_id);

          snprintf (tmpbuf,
                    tmpbuflen,
                    "Domain ID = %s",
                    domain_id_str);

          return (1);
        }

      if (system_event_record_data->event_type_code == IPMI_EVENT_READING_TYPE_CODE_OEM_INTEL_ME_FIRMWARE_HEALTH_EVENT
          && system_event_record_data->sensor_number == IPMI_SENSOR_NUMBER_OEM_INTEL_ME_FIRMWARE_HEALTH_EVENT
          && system_event_record_data->offset_from_event_reading_type_code == IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_FIRMWARE_STATUS)
        {
          uint8_t health_event;
          char *health_event_str;

          health_event = system_event_record_data->event_data2;

          switch (health_event)
            {
            case IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA2_RECOVERY_GPIO_FORCED:
              health_event_str = "Recovery GPIO forced";
              break;
            case IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA2_IMAGE_EXECUTION_FAILED:
              health_event_str = "Image execution failed";
              break;
            case IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA2_FLASH_ERASE_ERROR:
              health_event_str = "Flash erase error";
              break;
            case IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA2_FLASH_STATE_INFORMATION:
              health_event_str = "Flash state information";
              break;
            case IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA2_INTERNAL_ERROR:
              health_event_str = "Internal error";
              break;
            case IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA2_BMC_COLD_RESET_ERROR:
              health_event_str = "BMC did not respond to cold reset";
              break;
            case IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA2_DIRECT_FLASH_UPDATE:
              health_event_str = "Direct flash update requested by the BIOS";
              break;
            case IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA2_MANUFACTURING_ERROR:
              health_event_str = "Manufacturing error";
              break;
            case IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA2_PERSISTENT_STORAGE_INTEGRITY_ERROR:
              health_event_str = "Persistent storage integrity error";
              break;
            case IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA2_FIRMWARE_EXCEPTION:
              health_event_str = "Firmware Exception";
              break;
            case IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA2_FLASH_WEAR_OUT_PROTECTION_WARNING:
              health_event_str = "Flash Wear-Out Protection Warning";
              break;
            case IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA2_PECI_OVER_DMI_INTERFACE_ERROR:
              health_event_str = "PECI over DMI interface error";
              break;
            case IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA2_MCTP_INTERFACE_ERROR:
              health_event_str = "MCTP interface error";
              break;
            case IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA2_AUTO_CONFIGURATION_FINISHED:
              health_event_str = "Auto-configuration finished";
              break;
            default:
              health_event_str = "Unknown";
            }

          snprintf (tmpbuf,
                    tmpbuflen,
                    "Health Event = %s",
                    health_event_str);

          return (1);
        }

      if (system_event_record_data->event_type_code == IPMI_EVENT_READING_TYPE_CODE_OEM_INTEL_NODE_MANAGER_CUPS_EVENT
          && system_event_record_data->sensor_number == IPMI_SENSOR_NUMBER_OEM_INTEL_NODE_MANAGER_CUPS_EVENT_SENSOR)
        {
          uint8_t domain_id;
          char domain_id_str[INTEL_NODE_MANAGER_EVENT_BUFFER_LENGTH + 1];

          domain_id = (system_event_record_data->event_data2 & IPMI_OEM_INTEL_NODE_MANAGER_CUPS_EVENT_EVENT_DATA2_DOMAIN_ID_BITMASK);
          domain_id >>= IPMI_OEM_INTEL_NODE_MANAGER_CUPS_EVENT_EVENT_DATA2_DOMAIN_ID_SHIFT;

          memset (domain_id_str, '\0', INTEL_NODE_MANAGER_EVENT_BUFFER_LENGTH + 1);

          _sel_string_output_intel_node_manager_domain_id (ctx,
                                                           domain_id_str,
                                                           INTEL_NODE_MANAGER_EVENT_BUFFER_LENGTH,
                                                           domain_id);

          snprintf (tmpbuf,
                    tmpbuflen,
                    "Domain ID = %s",
                    domain_id_str);

          return (1);
        }
    }

 out:
  return (0);
}

/* return (0) - no OEM match
 * return (1) - OEM match
 * return (-1) - error, cleanup and return error
 */
int
sel_string_output_intel_node_manager_event_data3_discrete_oem (ipmi_sel_ctx_t ctx,
                                                               struct ipmi_sel_entry *sel_entry,
                                                               uint8_t sel_record_type,
                                                               char *tmpbuf,
                                                               unsigned int tmpbuflen,
                                                               unsigned int flags,
                                                               unsigned int *wlen,
                                                               struct ipmi_sel_system_event_record_data *system_event_record_data)
{
  assert (ctx);
  assert (ctx->magic == IPMI_SEL_CTX_MAGIC);
  assert (sel_entry);
  assert (tmpbuf);
  assert (tmpbuflen);
  assert (!(flags & ~IPMI_SEL_STRING_FLAGS_MASK));
  assert (flags & IPMI_SEL_STRING_FLAGS_INTERPRET_OEM_DATA);
  assert (wlen);
  assert (system_event_record_data);

  if (system_event_record_data->sensor_type == IPMI_SENSOR_TYPE_OEM_INTEL_NODE_MANAGER
      && system_event_record_data->event_type_code == IPMI_EVENT_READING_TYPE_CODE_STATE
      && system_event_record_data->sensor_number == IPMI_SENSOR_NUMBER_OEM_INTEL_NODE_MANAGER_NM_SMART_CLST_SENSOR
      && system_event_record_data->event_data3_flag == IPMI_SEL_EVENT_DATA_OEM_CODE)
    {
      if (system_event_record_data->event_data3 == IPMI_OEM_INTEL_NODE_MANAGER_NM_SMART_CLST_SENSOR_EVENT_DATA3_EXTERNAL)
        snprintf (tmpbuf,
                  tmpbuflen,
                  "Source of assertion is external");
      else
        snprintf (tmpbuf,
                  tmpbuflen,
                  "Power Supply Status Number = %u",
                  system_event_record_data->event_data3);

      return (1);
    }

  return (0);
}

/* return (0) - no OEM match
 * return (1) - OEM match
 * return (-1) - error, cleanup and return error
 */
int
sel_string_output_intel_node_manager_event_data3_class_oem (ipmi_sel_ctx_t ctx,
                                                            struct ipmi_sel_entry *sel_entry,
                                                            uint8_t sel_record_type,
                                                            char *tmpbuf,
                                                            unsigned int tmpbuflen,
                                                            unsigned int flags,
                                                            unsigned int *wlen,
                                                            struct ipmi_sel_system_event_record_data *system_event_record_data)
{
  assert (ctx);
  assert (ctx->magic == IPMI_SEL_CTX_MAGIC);
  assert (sel_entry);
  assert (tmpbuf);
  assert (tmpbuflen);
  assert (!(flags & ~IPMI_SEL_STRING_FLAGS_MASK));
  assert (flags & IPMI_SEL_STRING_FLAGS_INTERPRET_OEM_DATA);
  assert (wlen);
  assert (system_event_record_data);

  if (system_event_record_data->sensor_type == IPMI_SENSOR_TYPE_OEM_INTEL_NODE_MANAGER)
    {
      int nm_found;
      uint8_t node_manager_policy_event;

      if ((nm_found = _intel_node_manager_init (ctx)) < 0)
        return (-1);

      if (!nm_found)
        goto out;

      node_manager_policy_event = system_event_record_data->offset_from_event_reading_type_code & IPMI_OEM_INTEL_NODE_MANAGER_EXCEPTION_EVENT_EVENT_DATA1_NODE_MANAGER_POLICY_EVENT_BITMASK;
      node_manager_policy_event >>= IPMI_OEM_INTEL_NODE_MANAGER_EXCEPTION_EVENT_EVENT_DATA1_NODE_MANAGER_POLICY_EVENT_SHIFT;

      if (system_event_record_data->event_type_code == IPMI_EVENT_READING_TYPE_CODE_OEM_INTEL_NODE_MANAGER_EXCEPTION_EVENT
          && system_event_record_data->sensor_number == ctx->intel_node_manager.nm_exception_event_sensor_number
          && node_manager_policy_event == IPMI_OEM_INTEL_NODE_MANAGER_EXCEPTION_EVENT_EVENT_DATA1_NODE_MANAGER_POLICY_EVENT_POLICY_CORRECTION_TIME_EXCEEDED)
        {
          snprintf (tmpbuf,
                    tmpbuflen,
                    "Policy ID = %u",
                    system_event_record_data->event_data3);

          return (1);
        }

      if (system_event_record_data->event_type_code == IPMI_EVENT_READING_TYPE_CODE_OEM_INTEL_NODE_MANAGER_HEALTH_EVENT
          && system_event_record_data->sensor_number == ctx->intel_node_manager.nm_health_event_sensor_number
          && system_event_record_data->offset_from_event_reading_type_code == IPMI_OEM_INTEL_NODE_MANAGER_HEALTH_EVENT_SENSOR_NODE_MANAGER
          && system_event_record_data->event_data2_flag == IPMI_SEL_EVENT_DATA_OEM_CODE)
        {
          uint8_t error_type;

          error_type = (system_event_record_data->event_data2 & IPMI_OEM_INTEL_NODE_MANAGER_HEALTH_EVENT_EVENT_DATA2_ERROR_TYPE_BITMASK);
          error_type >>= IPMI_OEM_INTEL_NODE_MANAGER_HEALTH_EVENT_EVENT_DATA2_ERROR_TYPE_SHIFT;

          if (error_type == IPMI_OEM_INTEL_NODE_MANAGER_HEALTH_EVENT_EVENT_DATA2_ERROR_TYPE_POLICY_MISCONFIGURATION
              || error_type == IPMI_OEM_INTEL_NODE_MANAGER_HEALTH_EVENT_EVENT_DATA2_ERROR_TYPE_PLATFORM_SHUTDOWN_INITIATED_BY_INTEL_NM_POLICY)
            {
              snprintf (tmpbuf,
                        tmpbuflen,
                        "Policy ID = %u",
                        system_event_record_data->event_data3);

              return (1);
            }
          else if (error_type == IPMI_OEM_INTEL_NODE_MANAGER_HEALTH_EVENT_EVENT_DATA2_ERROR_TYPE_POWER_SENSOR_READING_FAILURE)
            {
              snprintf (tmpbuf,
                        tmpbuflen,
                        "Power Sensor Address = %02Xh",
                        system_event_record_data->event_data3);

              return (1);
            }
          else if (error_type == IPMI_OEM_INTEL_NODE_MANAGER_HEALTH_EVENT_EVENT_DATA2_ERROR_TYPE_INLET_TEMPERATURE_READING_FAILURE)
            {
              snprintf (tmpbuf,
                        tmpbuflen,
                        "Inlet Sensor Address = %02Xh",
                        system_event_record_data->event_data3);

              return (1);
            }
        }

      if (system_event_record_data->event_type_code == IPMI_EVENT_READING_TYPE_CODE_OEM_INTEL_NODE_MANAGER_ALERT_THRESHOLD_EXCEEDED
          && system_event_record_data->sensor_number == ctx->intel_node_manager.nm_alert_threshold_exceeded_sensor_number)
        {
          snprintf (tmpbuf,
                    tmpbuflen,
                    "Policy ID = %u",
                    system_event_record_data->event_data3);

          return (1);
        }

      if (system_event_record_data->event_type_code == IPMI_EVENT_READING_TYPE_CODE_OEM_INTEL_ME_FIRMWARE_HEALTH_EVENT
          && system_event_record_data->sensor_number == IPMI_SENSOR_NUMBER_OEM_INTEL_ME_FIRMWARE_HEALTH_EVENT
          && system_event_record_data->offset_from_event_reading_type_code == IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_FIRMWARE_STATUS)
        {
          if (system_event_record_data->event_data2 == IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA2_FLASH_STATE_INFORMATION)
            {
              if (system_event_record_data->event_data3 == IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA3_FLASH_STATE_INFORMATION_IMAGE_CORRUPTED)
                {
                  snprintf (tmpbuf,
                            tmpbuflen,
                            "flash partition table, recovery image or factory presets image corrupted");

                  return (1);
                }
              else if (system_event_record_data->event_data3 == IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA3_FLASH_STATE_INFORMATION_FLASH_ERASE_LIMIT_REACHED)
                {
                  snprintf (tmpbuf,
                            tmpbuflen,
                            "flash erase limit has been reached");

                  return (1);
                }
              else if (system_event_record_data->event_data3 == IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA3_FLASH_STATE_INFORMATION_FLASH_WRITE_LIMIT_REACHED)
                {
                  snprintf (tmpbuf,
                            tmpbuflen,
                            "flash write limit has been reached ; writing to flash has been disabled");

                  return (1);
                }
              else if (system_event_record_data->event_data3 == IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA3_FLASH_STATE_INFORMATION_WRITING_TO_FLASH_ENABLED)
                {
                  snprintf (tmpbuf,
                            tmpbuflen,
                            "writing to the flash has been enabled");

                  return (1);
                }
            }
          else if (system_event_record_data->event_data2 == IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA2_INTERNAL_ERROR)
            {
              if (system_event_record_data->event_data3 == IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA3_INTERNAL_ERROR_FW_WATCHDOG_TIMEOUT)
                {
                  snprintf (tmpbuf,
                            tmpbuflen,
                            "FW Watchdog Timeout");

                  return (1);
                }
              else if (system_event_record_data->event_data3 == IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA3_INTERNAL_ERROR_LOADER_MANIFEST_VALIDATION_FAILURE)
                {
                  snprintf (tmpbuf,
                            tmpbuflen,
                            "Loader manifest validation failure");

                  return (1);
                }
              else if (system_event_record_data->event_data3 == IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA3_INTERNAL_ERROR_UNKNOWN_POWER_MANAGEMENT_EVENT)
                {
                  snprintf (tmpbuf,
                            tmpbuflen,
                            "Unknown power management event");

                  return (1);
                }
              else if (system_event_record_data->event_data3 == IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA3_INTERNAL_ERROR_NON_GRACEFUL_PMC_RESET_EVENT)
                {
                  snprintf (tmpbuf,
                            tmpbuflen,
                            "Non graceful PMC reset event detected i.e. after Dynamic Fusing");

                  return (1);
                }
              else if (system_event_record_data->event_data3 == IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA3_INTERNAL_ERROR_FLASH_WEAROUT_PROTECTION)
                {
                  snprintf (tmpbuf,
                            tmpbuflen,
                            "Flash wearout protection (EFFS wearout violation)");

                  return (1);
                }
            }
          else if (system_event_record_data->event_data2 == IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA2_MANUFACTURING_ERROR)
            {
              if (system_event_record_data->event_data3 == IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA3_MANUFACTURING_ERROR_GENERIC_ERROR)
                {
                  snprintf (tmpbuf,
                            tmpbuflen,
                            "Generic error");

                  return (1);
                }
              else if (system_event_record_data->event_data3 == IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA3_MANUFACTURING_ERROR_WRONG_OR_MISSING_VSCC_TABLE)
                {
                  snprintf (tmpbuf,
                            tmpbuflen,
                            "Wrong or missing VSCC table");

                  return (1);
                }
              else if (system_event_record_data->event_data3 == IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA3_MANUFACTURING_ERROR_WRONG_SENSOR_SCANNING_PERIOD_IN_PIA)
                {
                  snprintf (tmpbuf,
                            tmpbuflen,
                            "Wrong sensor scanning period in PIA");

                  return (1);
                }
              else if (system_event_record_data->event_data3 == IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA3_MANUFACTURING_ERROR_WRONG_DEVICE_DEFINITION_IN_PIA)
                {
                  snprintf (tmpbuf,
                            tmpbuflen,
                            "Wrong device definition in PIA");

                  return (1);
                }
              else if (system_event_record_data->event_data3 == IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA3_MANUFACTURING_ERROR_WRONG_SMART_CLST_CONFIGURATION)
                {
                  snprintf (tmpbuf,
                            tmpbuflen,
                            "Wrong SMART/CLST configuration");

                  return (1);
                }
              else if (system_event_record_data->event_data3 == IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA3_MANUFACTURING_ERROR_INTEL_ME_FW_CONFIGURATION_IS_INCONSISTENT_OR_OUT_OF_RANGE)
                {
                  snprintf (tmpbuf,
                            tmpbuflen,
                            "Intel ME FW configuration is inconsistent or out of range");

                  return (1);
                }
              else if (system_event_record_data->event_data3 == IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA3_MANUFACTURING_ERROR_PERCENTAGE_OF_FLASH_WRITE_OPERATIONS_WHICH_HAVE_BEEN_CONDUCTED)
                {
                  snprintf (tmpbuf,
                            tmpbuflen,
                            "percentage of flash write operations which have been conducted");

                  return (1);
                }
            }
          else if (system_event_record_data->event_data2 == IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA2_PECI_OVER_DMI_INTERFACE_ERROR)
            {
              if (system_event_record_data->event_data3 == IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA3_DRAM_INIT_DONE_EVENT_NOT_RECEIVED)
                {
                  snprintf (tmpbuf,
                            tmpbuflen,
                            "DRAM Init Done event not received");

                  return (1);
                }
              else if (system_event_record_data->event_data3 == IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA3_MCTP_SAD_REGISTER_NOT_CORRECTLY_CONFIGURED_BY_BIOS)
                {
                  snprintf (tmpbuf,
                            tmpbuflen,
                            "MCTP SAD Register not correctly configured by BIOS");

                  return (1);
                }
              else if (system_event_record_data->event_data3 == IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA3_DMI_TIMEOUT_OF_PECI_REQUEST)
                {
                  snprintf (tmpbuf,
                            tmpbuflen,
                            "DMI timeout of PECI request");

                  return (1);
                }
            }
          else if (system_event_record_data->event_data2 == IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA2_AUTO_CONFIGURATION_FINISHED)
            {
              uint8_t auto_configuration_finished_result;

              auto_configuration_finished_result = (system_event_record_data->event_data3 & IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA3_AUTO_CONFIGURATION_FINISHED_RESULT_BITMASK);
              auto_configuration_finished_result >>= IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA3_AUTO_CONFIGURATION_FINISHED_RESULT_SHIFT;

              if (auto_configuration_finished_result == IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA3_AUTO_CONFIGURATION_FINISHED_RESULT_SUCCESS)
                {
                  uint8_t dc_power_source, chassis_power_input_source, psu_efficiency_source, unmanaged_power_source;
                  char *dc_power_source_str, *chassis_power_input_source_str, *psu_efficiency_source_str, *unmanaged_power_source_str;

                  dc_power_source = (system_event_record_data->event_data3 & IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA3_AUTO_CONFIGURATION_FINISHED_RESULT_SUCCESS_DC_POWER_SOURCE_BITMASK);
                  dc_power_source >>= IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA3_AUTO_CONFIGURATION_FINISHED_RESULT_SUCCESS_DC_POWER_SOURCE_SHIFT;

                  switch (dc_power_source)
                    {
                    case IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA3_AUTO_CONFIGURATION_FINISHED_RESULT_SUCCESS_DC_POWER_SOURCE_BMC:
                      dc_power_source_str = "BMC";
                      break;
                    case IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA3_AUTO_CONFIGURATION_FINISHED_RESULT_SUCCESS_DC_POWER_SOURCE_PSU:
                      dc_power_source_str = "PSU";
                      break;
                    case IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA3_AUTO_CONFIGURATION_FINISHED_RESULT_SUCCESS_DC_POWER_SOURCE_ON_BOARD_POWER_SENSOR:
                      dc_power_source_str = "On-board power sensor";
                      break;
                    default:
                      dc_power_source_str = "Unknown";
                    }

                  chassis_power_input_source = (system_event_record_data->event_data3 & IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA3_AUTO_CONFIGURATION_FINISHED_RESULT_SUCCESS_CHASSIS_POWER_INPUT_SOURCE_BITMASK);
                  chassis_power_input_source >>= IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA3_AUTO_CONFIGURATION_FINISHED_RESULT_SUCCESS_CHASSIS_POWER_INPUT_SOURCE_SHIFT;

                  switch (chassis_power_input_source)
                    {
                    case IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA3_AUTO_CONFIGURATION_FINISHED_RESULT_SUCCESS_CHASSIS_POWER_INPUT_SOURCE_BMC:
                      chassis_power_input_source_str = "BMC";
                      break;
                    case IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA3_AUTO_CONFIGURATION_FINISHED_RESULT_SUCCESS_CHASSIS_POWER_INPUT_SOURCE_PSU:
                      chassis_power_input_source_str = "PSU";
                      break;
                    case IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA3_AUTO_CONFIGURATION_FINISHED_RESULT_SUCCESS_CHASSIS_POWER_INPUT_SOURCE_ON_BOARD_POWER_SENSOR_PSU_EFFICIENCY:
                      chassis_power_input_source_str = "On-board power sensor / PSU efficiency";
                      break;
                    case IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA3_AUTO_CONFIGURATION_FINISHED_RESULT_SUCCESS_CHASSIS_POWER_INPUT_SOURCE_NOT_SUPPORTED:
                      chassis_power_input_source_str = "not supported";
                      break;
                    default:
                      chassis_power_input_source_str = "Unknown";
                    }

                  psu_efficiency_source = (system_event_record_data->event_data3 & IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA3_AUTO_CONFIGURATION_FINISHED_RESULT_SUCCESS_PSU_EFFICIENCY_SOURCE_BITMASK);
                  psu_efficiency_source >>= IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA3_AUTO_CONFIGURATION_FINISHED_RESULT_SUCCESS_PSU_EFFICIENCY_SOURCE_SHIFT;

                  switch (psu_efficiency_source)
                    {
                    case IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA3_AUTO_CONFIGURATION_FINISHED_RESULT_SUCCESS_PSU_EFFICIENCY_SOURCE_BMC:
                      psu_efficiency_source_str = "BMC";
                      break;
                    case IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA3_AUTO_CONFIGURATION_FINISHED_RESULT_SUCCESS_PSU_EFFICIENCY_SOURCE_PSU:
                      psu_efficiency_source_str = "PSU";
                      break;
                    case IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA3_AUTO_CONFIGURATION_FINISHED_RESULT_SUCCESS_PSU_EFFICIENCY_SOURCE_NOT_SUPPORTED:
                      psu_efficiency_source_str = "not supported";
                      break;
                    default:
                      psu_efficiency_source_str = "Unknown";
                    }

                  unmanaged_power_source = (system_event_record_data->event_data3 & IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA3_AUTO_CONFIGURATION_FINISHED_RESULT_SUCCESS_UNMANAGED_POWER_SOURCE_BITMASK);
                  unmanaged_power_source >>= IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA3_AUTO_CONFIGURATION_FINISHED_RESULT_SUCCESS_UNMANAGED_POWER_SOURCE_SHIFT;

                  switch (unmanaged_power_source)
                    {
                    case IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA3_AUTO_CONFIGURATION_FINISHED_RESULT_SUCCESS_UNMANAGED_POWER_SOURCE_BMC:
                      unmanaged_power_source_str = "BMC";
                      break;
                    case IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA3_AUTO_CONFIGURATION_FINISHED_RESULT_SUCCESS_UNMANAGED_POWER_SOURCE_ESTIMATED:
                      unmanaged_power_source_str = "estimated";
                      break;
                    default:
                      unmanaged_power_source_str = "Unknown";
                    }

                  snprintf (tmpbuf,
                            tmpbuflen,
                            "Auto Configuration Success - DC Power Source = %s, Chassis Power input source = %s, PSU efficiency source = %s, Unmanaged power source = %s",
                            dc_power_source_str,
                            chassis_power_input_source_str,
                            psu_efficiency_source_str,
                            unmanaged_power_source_str);

                  return (1);
                }
              else /* auto_configuration_finished_result == IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA3_AUTO_CONFIGURATION_FINISHED_RESULT_FAILURE */
                {
                  uint8_t failure;
                  char *failure_str;

                  failure = (system_event_record_data->event_data3 & IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA3_AUTO_CONFIGURATION_FINISHED_RESULT_FAILURE_FAILURE_BITMASK);
                  failure >>= IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA3_AUTO_CONFIGURATION_FINISHED_RESULT_FAILURE_FAILURE_SHIFT;

                  switch (failure)
                    {
                    case IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA3_AUTO_CONFIGURATION_FINISHED_RESULT_FAILURE_FAILURE_BMC_DISCOVERY_FAILURE:
                      failure_str = "BMC discovery failure";
                      break;
                    case IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA3_AUTO_CONFIGURATION_FINISHED_RESULT_FAILURE_FAILURE_INSUFFICIENT_FACTORY_CONFIGURATION:
                      failure_str = "Insufficient factory configuration";
                      break;
                    case IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA3_AUTO_CONFIGURATION_FINISHED_RESULT_FAILURE_FAILURE_UNKNOWN_SENSOR_TYPE:
                      failure_str = "Unknown sensor type";
                      break;
                    case IPMI_OEM_INTEL_NODE_MANAGER_INTEL_ME_FIRMWARE_HEALTH_EVENT_EVENT_DATA3_AUTO_CONFIGURATION_FINISHED_RESULT_FAILURE_FAILURE_OTHER_ERROR_ENCOUNTERED:
                      failure_str = "Other error encountered";
                      break;
                    default:
                      failure_str = "Unknown";
                    }

                  snprintf (tmpbuf,
                            tmpbuflen,
                            "Auto Configuration Failure - %s",
                            failure_str);

                  return (1);
                }
            }

          snprintf (tmpbuf,
                    tmpbuflen,
                    "Extended error info = %02Xh",
                    system_event_record_data->event_data3);

          return (1);
        }

      if (system_event_record_data->event_type_code == IPMI_EVENT_READING_TYPE_CODE_OEM_INTEL_NODE_MANAGER_CUPS_EVENT
          && system_event_record_data->sensor_number == IPMI_SENSOR_NUMBER_OEM_INTEL_NODE_MANAGER_CUPS_EVENT_SENSOR)
        {
          snprintf (tmpbuf,
                    tmpbuflen,
                    "Policy ID = %u",
                    system_event_record_data->event_data3);

          return (1);
        }
    }

 out:
  return (0);
}

