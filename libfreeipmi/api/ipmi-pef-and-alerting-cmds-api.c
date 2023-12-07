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
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>
#ifdef STDC_HEADERS
#include <string.h>
#endif /* STDC_HEADERS */
#include <errno.h>
#include <assert.h>

#include "freeipmi/api/ipmi-pef-and-alerting-cmds-api.h"
#include "freeipmi/cmds/ipmi-pef-and-alerting-cmds.h"
#include "freeipmi/fiid/fiid.h"
#include "freeipmi/spec/ipmi-ipmb-lun-spec.h"
#include "freeipmi/spec/ipmi-netfn-spec.h"
#include "freeipmi/spec/ipmi-pef-configuration-parameters-spec.h"

#include "ipmi-api-defs.h"
#include "ipmi-api-trace.h"
#include "ipmi-api-util.h"

#include "libcommon/ipmi-fiid-util.h"

#include "freeipmi-portability.h"

int
ipmi_cmd_get_pef_capabilities (ipmi_ctx_t ctx, fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int rv = -1;

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      return (-1);
    }

  if (!fiid_obj_valid (obj_cmd_rs))
    {
      API_SET_ERRNUM (ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }

  if (FIID_OBJ_TEMPLATE_COMPARE (obj_cmd_rs,
                                 tmpl_cmd_get_pef_capabilities_rs) < 0)
    {
      API_FIID_OBJECT_ERROR_TO_API_ERRNUM (ctx, obj_cmd_rs);
      return (-1);
    }

  if (!(obj_cmd_rq = fiid_obj_create (tmpl_cmd_get_pef_capabilities_rq)))
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_get_pef_capabilities (obj_cmd_rq) < 0)
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (api_ipmi_cmd (ctx,
                    IPMI_BMC_IPMB_LUN_BMC,
                    IPMI_NET_FN_SENSOR_EVENT_RQ,
                    obj_cmd_rq,
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      goto cleanup;
    }

  rv = 0;
 cleanup:
  fiid_obj_destroy (obj_cmd_rq);
  return (rv);
}

int
ipmi_cmd_arm_pef_postpone_timer (ipmi_ctx_t ctx,
                                 uint8_t pef_postpone_timeout,
                                 fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int rv = -1;

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      return (-1);
    }

  if (!fiid_obj_valid (obj_cmd_rs))
    {
      API_SET_ERRNUM (ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }

  if (FIID_OBJ_TEMPLATE_COMPARE (obj_cmd_rs,
                                 tmpl_cmd_arm_pef_postpone_timer_rs) < 0)
    {
      API_FIID_OBJECT_ERROR_TO_API_ERRNUM (ctx, obj_cmd_rs);
      return (-1);
    }

  if (!(obj_cmd_rq = fiid_obj_create (tmpl_cmd_arm_pef_postpone_timer_rq)))
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_arm_pef_postpone_timer (pef_postpone_timeout,
                                       obj_cmd_rq) < 0)
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (api_ipmi_cmd (ctx,
                    IPMI_BMC_IPMB_LUN_BMC,
                    IPMI_NET_FN_SENSOR_EVENT_RQ,
                    obj_cmd_rq,
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      goto cleanup;
    }

  rv = 0;
 cleanup:
  fiid_obj_destroy (obj_cmd_rq);
  return (rv);
}

int
ipmi_cmd_set_pef_configuration_parameters (ipmi_ctx_t ctx,
                                           uint8_t parameter_selector,
                                           const void *configuration_parameter_data,
                                           unsigned int configuration_parameter_data_len,
                                           fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int rv = -1;

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      return (-1);
    }

  /* remaining parameter checks in fill function */
  if (!fiid_obj_valid (obj_cmd_rs))
    {
      API_SET_ERRNUM (ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }

  if (FIID_OBJ_TEMPLATE_COMPARE (obj_cmd_rs,
                                 tmpl_cmd_set_pef_configuration_parameters_rs) < 0)
    {
      API_FIID_OBJECT_ERROR_TO_API_ERRNUM (ctx, obj_cmd_rs);
      return (-1);
    }

  if (!(obj_cmd_rq = fiid_obj_create (tmpl_cmd_set_pef_configuration_parameters_rq)))
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_set_pef_configuration_parameters (parameter_selector,
                                                 configuration_parameter_data,
                                                 configuration_parameter_data_len,
                                                 obj_cmd_rq) < 0)
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (api_ipmi_cmd (ctx,
                    IPMI_BMC_IPMB_LUN_BMC,
                    IPMI_NET_FN_SENSOR_EVENT_RQ,
                    obj_cmd_rq,
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      goto cleanup;
    }

  rv = 0;
 cleanup:
  fiid_obj_destroy (obj_cmd_rq);
  return (rv);
}

int
ipmi_cmd_set_pef_configuration_parameters_set_in_progress (ipmi_ctx_t ctx,
                                                           uint8_t state,
                                                           fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int rv = -1;

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      return (-1);
    }

  /* remaining parameter checks in fill function */
  if  (!fiid_obj_valid (obj_cmd_rs))
    {
      API_SET_ERRNUM (ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }

  if (FIID_OBJ_TEMPLATE_COMPARE (obj_cmd_rs,
                                 tmpl_cmd_set_pef_configuration_parameters_rs) < 0)
    {
      API_FIID_OBJECT_ERROR_TO_API_ERRNUM (ctx, obj_cmd_rs);
      return (-1);
    }

  if (!(obj_cmd_rq = fiid_obj_create (tmpl_cmd_set_pef_configuration_parameters_set_in_progress_rq)))
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_set_pef_configuration_parameters_set_in_progress (state,
                                                                 obj_cmd_rq) < 0)
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (api_ipmi_cmd (ctx,
                    IPMI_BMC_IPMB_LUN_BMC,
                    IPMI_NET_FN_SENSOR_EVENT_RQ,
                    obj_cmd_rq,
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      goto cleanup;
    }

  rv = 0;
 cleanup:
  fiid_obj_destroy (obj_cmd_rq);
  return (rv);
}

int
ipmi_cmd_set_pef_configuration_parameters_pef_control (ipmi_ctx_t ctx,
                                                       uint8_t pef,
                                                       uint8_t pef_event_messages,
                                                       uint8_t pef_startup_delay,
                                                       uint8_t pef_alert_startup_delay,
                                                       fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int rv = -1;

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      return (-1);
    }

  /* remaining parameter checks in fill function */
  if  (!fiid_obj_valid (obj_cmd_rs))
    {
      API_SET_ERRNUM (ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }

  if (FIID_OBJ_TEMPLATE_COMPARE (obj_cmd_rs,
                                 tmpl_cmd_set_pef_configuration_parameters_rs) < 0)
    {
      API_FIID_OBJECT_ERROR_TO_API_ERRNUM (ctx, obj_cmd_rs);
      return (-1);
    }

  if (!(obj_cmd_rq = fiid_obj_create (tmpl_cmd_set_pef_configuration_parameters_pef_control_rq)))
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_set_pef_configuration_parameters_pef_control (pef,
                                                             pef_event_messages,
                                                             pef_startup_delay,
                                                             pef_alert_startup_delay,
                                                             obj_cmd_rq) < 0)
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (api_ipmi_cmd (ctx,
                    IPMI_BMC_IPMB_LUN_BMC,
                    IPMI_NET_FN_SENSOR_EVENT_RQ,
                    obj_cmd_rq,
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      goto cleanup;
    }

  rv = 0;
 cleanup:
  fiid_obj_destroy (obj_cmd_rq);
  return (rv);
}

int
ipmi_cmd_set_pef_configuration_parameters_pef_action_global_control (ipmi_ctx_t ctx,
                                                                     uint8_t alert_action,
                                                                     uint8_t power_down_action,
                                                                     uint8_t reset_action,
                                                                     uint8_t power_cycle_action,
                                                                     uint8_t oem_action,
                                                                     uint8_t diagnostic_interrupt,
                                                                     fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int rv = -1;

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      return (-1);
    }

  /* remaining parameter checks in fill function */
  if (!fiid_obj_valid (obj_cmd_rs))
    {
      API_SET_ERRNUM (ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }

  if (FIID_OBJ_TEMPLATE_COMPARE (obj_cmd_rs,
                                 tmpl_cmd_set_pef_configuration_parameters_rs) < 0)
    {
      API_FIID_OBJECT_ERROR_TO_API_ERRNUM (ctx, obj_cmd_rs);
      return (-1);
    }

  if (!(obj_cmd_rq = fiid_obj_create (tmpl_cmd_set_pef_configuration_parameters_pef_action_global_control_rq)))
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_set_pef_configuration_parameters_pef_action_global_control (alert_action,
                                                                           power_down_action,
                                                                           reset_action,
                                                                           power_cycle_action,
                                                                           oem_action,
                                                                           diagnostic_interrupt,
                                                                           obj_cmd_rq) < 0)
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (api_ipmi_cmd (ctx,
                    IPMI_BMC_IPMB_LUN_BMC,
                    IPMI_NET_FN_SENSOR_EVENT_RQ,
                    obj_cmd_rq,
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      goto cleanup;
    }

  rv = 0;
 cleanup:
  fiid_obj_destroy (obj_cmd_rq);
  return (rv);
}

int
ipmi_cmd_set_pef_configuration_parameters_pef_startup_delay (ipmi_ctx_t ctx,
                                                             uint8_t pef_startup_delay,
                                                             fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int rv = -1;

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      return (-1);
    }

  if (!fiid_obj_valid (obj_cmd_rs))
    {
      API_SET_ERRNUM (ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }

  if (FIID_OBJ_TEMPLATE_COMPARE (obj_cmd_rs,
                                 tmpl_cmd_set_pef_configuration_parameters_rs) < 0)
    {
      API_FIID_OBJECT_ERROR_TO_API_ERRNUM (ctx, obj_cmd_rs);
      return (-1);
    }

  if (!(obj_cmd_rq = fiid_obj_create (tmpl_cmd_set_pef_configuration_parameters_pef_startup_delay_rq)))
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_set_pef_configuration_parameters_pef_startup_delay (pef_startup_delay,
                                                                   obj_cmd_rq) < 0)
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (api_ipmi_cmd (ctx,
                    IPMI_BMC_IPMB_LUN_BMC,
                    IPMI_NET_FN_SENSOR_EVENT_RQ,
                    obj_cmd_rq,
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      goto cleanup;
    }

  rv = 0;
 cleanup:
  fiid_obj_destroy (obj_cmd_rq);
  return (rv);
}

int
ipmi_cmd_set_pef_configuration_parameters_pef_alert_startup_delay (ipmi_ctx_t ctx,
                                                                   uint8_t pef_alert_startup_delay,
                                                                   fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int rv = -1;

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      return (-1);
    }

  if (!fiid_obj_valid (obj_cmd_rs))
    {
      API_SET_ERRNUM (ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }

  if (FIID_OBJ_TEMPLATE_COMPARE (obj_cmd_rs,
                                 tmpl_cmd_set_pef_configuration_parameters_rs) < 0)
    {
      API_FIID_OBJECT_ERROR_TO_API_ERRNUM (ctx, obj_cmd_rs);
      return (-1);
    }

  if (!(obj_cmd_rq = fiid_obj_create (tmpl_cmd_set_pef_configuration_parameters_pef_alert_startup_delay_rq)))
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_set_pef_configuration_parameters_pef_alert_startup_delay (pef_alert_startup_delay,
                                                                         obj_cmd_rq) < 0)
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (api_ipmi_cmd (ctx,
                    IPMI_BMC_IPMB_LUN_BMC,
                    IPMI_NET_FN_SENSOR_EVENT_RQ,
                    obj_cmd_rq,
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      goto cleanup;
    }

  rv = 0;
 cleanup:
  fiid_obj_destroy (obj_cmd_rq);
  return (rv);
}

int
ipmi_cmd_set_pef_configuration_parameters_event_filter_table (ipmi_ctx_t ctx,
                                                              uint8_t filter_number,
                                                              uint8_t filter_configuration_type,
                                                              uint8_t filter_configuration_filter,
                                                              uint8_t event_filter_action_alert,
                                                              uint8_t event_filter_action_power_off,
                                                              uint8_t event_filter_action_reset,
                                                              uint8_t event_filter_action_power_cycle,
                                                              uint8_t event_filter_action_oem,
                                                              uint8_t event_filter_action_diagnostic_interrupt,
                                                              uint8_t event_filter_action_group_control_operation,
                                                              uint8_t alert_policy_number_policy_number,
                                                              uint8_t alert_policy_number_group_control_selector,
                                                              uint8_t event_severity,
                                                              uint8_t generator_id_byte1,
                                                              uint8_t generator_id_byte2,
                                                              uint8_t sensor_type,
                                                              uint8_t sensor_number,
                                                              uint8_t event_trigger,
                                                              uint16_t event_data1_offset_mask,
                                                              uint8_t event_data1_AND_mask,
                                                              uint8_t event_data1_compare1,
                                                              uint8_t event_data1_compare2,
                                                              uint8_t event_data2_AND_mask,
                                                              uint8_t event_data2_compare1,
                                                              uint8_t event_data2_compare2,
                                                              uint8_t event_data3_AND_mask,
                                                              uint8_t event_data3_compare1,
                                                              uint8_t event_data3_compare2,
                                                              fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int rv = -1;

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      return (-1);
    }

  /* remaining parameter checks in fill function */
  if (!fiid_obj_valid (obj_cmd_rs))
    {
      API_SET_ERRNUM (ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }

  if (FIID_OBJ_TEMPLATE_COMPARE (obj_cmd_rs,
                                 tmpl_cmd_set_pef_configuration_parameters_rs) < 0)
    {
      API_FIID_OBJECT_ERROR_TO_API_ERRNUM (ctx, obj_cmd_rs);
      return (-1);
    }

  if (!(obj_cmd_rq = fiid_obj_create (tmpl_cmd_set_pef_configuration_parameters_event_filter_table_rq)))
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_set_pef_configuration_parameters_event_filter_table (filter_number,
                                                                    filter_configuration_type,
                                                                    filter_configuration_filter,
                                                                    event_filter_action_alert,
                                                                    event_filter_action_power_off,
                                                                    event_filter_action_reset,
                                                                    event_filter_action_power_cycle,
                                                                    event_filter_action_oem,
                                                                    event_filter_action_diagnostic_interrupt,
                                                                    event_filter_action_group_control_operation,
                                                                    alert_policy_number_policy_number,
                                                                    alert_policy_number_group_control_selector,
                                                                    event_severity,
                                                                    generator_id_byte1,
                                                                    generator_id_byte2,
                                                                    sensor_type,
                                                                    sensor_number,
                                                                    event_trigger,
                                                                    event_data1_offset_mask,
                                                                    event_data1_AND_mask,
                                                                    event_data1_compare1,
                                                                    event_data1_compare2,
                                                                    event_data2_AND_mask,
                                                                    event_data2_compare1,
                                                                    event_data2_compare2,
                                                                    event_data3_AND_mask,
                                                                    event_data3_compare1,
                                                                    event_data3_compare2,
                                                                    obj_cmd_rq) < 0)
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (api_ipmi_cmd (ctx,
                    IPMI_BMC_IPMB_LUN_BMC,
                    IPMI_NET_FN_SENSOR_EVENT_RQ,
                    obj_cmd_rq,
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      goto cleanup;
    }

  rv = 0;
 cleanup:
  fiid_obj_destroy (obj_cmd_rq);
  return (rv);
}

int
ipmi_cmd_set_pef_configuration_parameters_event_filter_table_data1_ (ipmi_ctx_t ctx,
                                                                     uint8_t filter_number,
                                                                     uint8_t filter_configuration_type,
                                                                     uint8_t filter_configuration_filter,
                                                                     fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int rv = -1;

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      return (-1);
    }

  /* remaining parameter checks in fill function */
  if (!fiid_obj_valid (obj_cmd_rs))
    {
      API_SET_ERRNUM (ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }

  if (FIID_OBJ_TEMPLATE_COMPARE (obj_cmd_rs,
                                 tmpl_cmd_set_pef_configuration_parameters_rs) < 0)
    {
      API_FIID_OBJECT_ERROR_TO_API_ERRNUM (ctx, obj_cmd_rs);
      return (-1);
    }

  if (!(obj_cmd_rq = fiid_obj_create (tmpl_cmd_set_pef_configuration_parameters_event_filter_table_data1_rq)))
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_set_pef_configuration_parameters_event_filter_table_data1 (filter_number,
                                                                          filter_configuration_type,
                                                                          filter_configuration_filter,
                                                                          obj_cmd_rq) < 0)
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (api_ipmi_cmd (ctx,
                    IPMI_BMC_IPMB_LUN_BMC,
                    IPMI_NET_FN_SENSOR_EVENT_RQ,
                    obj_cmd_rq,
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      goto cleanup;
    }

  rv = 0;
 cleanup:
  fiid_obj_destroy (obj_cmd_rq);
  return (rv);
}

int
ipmi_cmd_set_pef_configuration_parameters_alert_policy_table (ipmi_ctx_t ctx,
                                                              uint8_t alert_policy_entry_number,
                                                              uint8_t policy_type,
                                                              uint8_t policy_enabled,
                                                              uint8_t policy_number,
                                                              uint8_t destination_selector,
                                                              uint8_t channel_number,
                                                              uint8_t alert_string_set_selector,
                                                              uint8_t event_specific_alert_string,
                                                              fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int rv = -1;

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      return (-1);
    }

  /* remaining parameter checks in fill function */
  if (!fiid_obj_valid (obj_cmd_rs))
    {
      API_SET_ERRNUM (ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }

  if (FIID_OBJ_TEMPLATE_COMPARE (obj_cmd_rs,
                                 tmpl_cmd_set_pef_configuration_parameters_rs) < 0)
    {
      API_FIID_OBJECT_ERROR_TO_API_ERRNUM (ctx, obj_cmd_rs);
      return (-1);
    }

  if (!(obj_cmd_rq = fiid_obj_create (tmpl_cmd_set_pef_configuration_parameters_alert_policy_table_rq)))
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_set_pef_configuration_parameters_alert_policy_table (alert_policy_entry_number,
                                                                    policy_type,
                                                                    policy_enabled,
                                                                    policy_number,
                                                                    destination_selector,
                                                                    channel_number,
                                                                    alert_string_set_selector,
                                                                    event_specific_alert_string,
                                                                    obj_cmd_rq) < 0)
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (api_ipmi_cmd (ctx,
                    IPMI_BMC_IPMB_LUN_BMC,
                    IPMI_NET_FN_SENSOR_EVENT_RQ,
                    obj_cmd_rq,
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      goto cleanup;
    }

  rv = 0;
 cleanup:
  fiid_obj_destroy (obj_cmd_rq);
  return (rv);
}

int
ipmi_cmd_set_pef_configuration_parameters_alert_string_keys (ipmi_ctx_t ctx,
                                                             uint8_t string_selector,
                                                             uint8_t filter_number,
                                                             uint8_t set_number_for_string,
                                                             fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int rv = -1;

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      return (-1);
    }

  /* remaining parameter checks in fill function */
  if (!fiid_obj_valid (obj_cmd_rs))
    {
      API_SET_ERRNUM (ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }

  if (FIID_OBJ_TEMPLATE_COMPARE (obj_cmd_rs,
                                 tmpl_cmd_set_pef_configuration_parameters_rs) < 0)
    {
      API_FIID_OBJECT_ERROR_TO_API_ERRNUM (ctx, obj_cmd_rs);
      return (-1);
    }

  if (!(obj_cmd_rq = fiid_obj_create (tmpl_cmd_set_pef_configuration_parameters_alert_string_keys_rq)))
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_set_pef_configuration_parameters_alert_string_keys (string_selector,
                                                                   filter_number,
                                                                   set_number_for_string,
                                                                   obj_cmd_rq) < 0)
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (api_ipmi_cmd (ctx,
                    IPMI_BMC_IPMB_LUN_BMC,
                    IPMI_NET_FN_SENSOR_EVENT_RQ,
                    obj_cmd_rq,
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      goto cleanup;
    }

  rv = 0;
 cleanup:
  fiid_obj_destroy (obj_cmd_rq);
  return (rv);
}

int
ipmi_cmd_set_pef_configuration_parameters_alert_strings (ipmi_ctx_t ctx,
                                                         uint8_t string_selector,
                                                         uint8_t block_selector,
                                                         const uint8_t *string_data,
                                                         unsigned int string_data_len,
                                                         fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int rv = -1;

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      return (-1);
    }

  /* remaining parameter checks in fill function */
  if (!fiid_obj_valid (obj_cmd_rs))
    {
      API_SET_ERRNUM (ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }

  if (FIID_OBJ_TEMPLATE_COMPARE (obj_cmd_rs,
                                 tmpl_cmd_set_pef_configuration_parameters_rs) < 0)
    {
      API_FIID_OBJECT_ERROR_TO_API_ERRNUM (ctx, obj_cmd_rs);
      return (-1);
    }

  if (!(obj_cmd_rq = fiid_obj_create (tmpl_cmd_set_pef_configuration_parameters_alert_strings_rq)))
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_set_pef_configuration_parameters_alert_strings (string_selector,
                                                               block_selector,
                                                               string_data,
                                                               string_data_len,
                                                               obj_cmd_rq) < 0)
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (api_ipmi_cmd (ctx,
                    IPMI_BMC_IPMB_LUN_BMC,
                    IPMI_NET_FN_SENSOR_EVENT_RQ,
                    obj_cmd_rq,
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      goto cleanup;
    }

  rv = 0;
 cleanup:
  fiid_obj_destroy (obj_cmd_rq);
  return (rv);
}

static int
_ipmi_cmd_get_pef_configuration_parameters_common (ipmi_ctx_t ctx,
                                                   uint8_t get_parameter,
                                                   uint8_t set_selector,
                                                   uint8_t block_selector,
                                                   fiid_obj_t obj_cmd_rs,
                                                   fiid_field_t *tmpl_cmd_rs_expected,
                                                   uint8_t parameter_selector)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int rv = -1;

  assert (tmpl_cmd_rs_expected);

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      return (-1);
    }

  /* remaining parameter checks in fill function */
  if (!fiid_obj_valid (obj_cmd_rs))
    {
      API_SET_ERRNUM (ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }

  if (FIID_OBJ_TEMPLATE_COMPARE (obj_cmd_rs, tmpl_cmd_rs_expected) < 0)
    {
      API_FIID_OBJECT_ERROR_TO_API_ERRNUM (ctx, obj_cmd_rs);
      return (-1);
    }

  if (!(obj_cmd_rq = fiid_obj_create (tmpl_cmd_get_pef_configuration_parameters_rq)))
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_get_pef_configuration_parameters (parameter_selector,
                                                 get_parameter,
                                                 set_selector,
                                                 block_selector,
                                                 obj_cmd_rq) < 0)
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (api_ipmi_cmd (ctx,
                    IPMI_BMC_IPMB_LUN_BMC,
                    IPMI_NET_FN_SENSOR_EVENT_RQ,
                    obj_cmd_rq,
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      goto cleanup;
    }

  rv = 0;
 cleanup:
  fiid_obj_destroy (obj_cmd_rq);
  return (rv);
}

int
ipmi_cmd_get_pef_configuration_parameters (ipmi_ctx_t ctx,
                                           uint8_t parameter_selector,
                                           uint8_t get_parameter,
                                           uint8_t set_selector,
                                           uint8_t block_selector,
                                           fiid_obj_t obj_cmd_rs)
{
  if (_ipmi_cmd_get_pef_configuration_parameters_common (ctx,
                                                         get_parameter,
                                                         set_selector,
                                                         block_selector,
                                                         obj_cmd_rs,
                                                         tmpl_cmd_get_pef_configuration_parameters_rs,
                                                         parameter_selector) < 0)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      return (-1);
    }

  return (0);
}

int
ipmi_cmd_get_pef_configuration_parameters_set_in_progress (ipmi_ctx_t ctx,
                                                           uint8_t get_parameter,
                                                           uint8_t set_selector,
                                                           uint8_t block_selector,
                                                           fiid_obj_t obj_cmd_rs)
{
  if (_ipmi_cmd_get_pef_configuration_parameters_common (ctx,
                                                         get_parameter,
                                                         set_selector,
                                                         block_selector,
                                                         obj_cmd_rs,
                                                         tmpl_cmd_get_pef_configuration_parameters_set_in_progress_rs,
                                                         IPMI_PEF_CONFIGURATION_PARAMETER_SET_IN_PROGRESS) < 0)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      return (-1);
    }

  return (0);
}

int
ipmi_cmd_get_pef_configuration_parameters_pef_control (ipmi_ctx_t ctx,
                                                       uint8_t get_parameter,
                                                       uint8_t set_selector,
                                                       uint8_t block_selector,
                                                       fiid_obj_t obj_cmd_rs)
{
  if (_ipmi_cmd_get_pef_configuration_parameters_common (ctx,
                                                         get_parameter,
                                                         set_selector,
                                                         block_selector,
                                                         obj_cmd_rs,
                                                         tmpl_cmd_get_pef_configuration_parameters_pef_control_rs,
                                                         IPMI_PEF_CONFIGURATION_PARAMETER_PEF_CONTROL) < 0)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      return (-1);
    }

  return (0);
}

int
ipmi_cmd_get_pef_configuration_parameters_pef_action_global_control (ipmi_ctx_t ctx,
                                                                     uint8_t get_parameter,
                                                                     uint8_t set_selector,
                                                                     uint8_t block_selector,
                                                                     fiid_obj_t obj_cmd_rs)
{
  if (_ipmi_cmd_get_pef_configuration_parameters_common (ctx,
                                                         get_parameter,
                                                         set_selector,
                                                         block_selector,
                                                         obj_cmd_rs,
                                                         tmpl_cmd_get_pef_configuration_parameters_pef_action_global_control_rs,
                                                         IPMI_PEF_CONFIGURATION_PARAMETER_PEF_ACTION_GLOBAL_CONTROL) < 0)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      return (-1);
    }

  return (0);
}

int
ipmi_cmd_get_pef_configuration_parameters_pef_startup_delay (ipmi_ctx_t ctx,
                                                             uint8_t get_parameter,
                                                             uint8_t set_selector,
                                                             uint8_t block_selector,
                                                             fiid_obj_t obj_cmd_rs)
{
  if (_ipmi_cmd_get_pef_configuration_parameters_common (ctx,
                                                         get_parameter,
                                                         set_selector,
                                                         block_selector,
                                                         obj_cmd_rs,
                                                         tmpl_cmd_get_pef_configuration_parameters_pef_startup_delay_rs,
                                                         IPMI_PEF_CONFIGURATION_PARAMETER_PEF_STARTUP_DELAY) < 0)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      return (-1);
    }

  return (0);
}

int
ipmi_cmd_get_pef_configuration_parameters_pef_alert_startup_delay (ipmi_ctx_t ctx,
                                                                   uint8_t get_parameter,
                                                                   uint8_t set_selector,
                                                                   uint8_t block_selector,
                                                                   fiid_obj_t obj_cmd_rs)
{
  if (_ipmi_cmd_get_pef_configuration_parameters_common (ctx,
                                                         get_parameter,
                                                         set_selector,
                                                         block_selector,
                                                         obj_cmd_rs,
                                                         tmpl_cmd_get_pef_configuration_parameters_pef_alert_startup_delay_rs,
                                                         IPMI_PEF_CONFIGURATION_PARAMETER_PEF_ALERT_STARTUP_DELAY) < 0)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      return (-1);
    }

  return (0);
}

int
ipmi_cmd_get_pef_configuration_parameters_number_of_event_filters (ipmi_ctx_t ctx,
                                                                   uint8_t get_parameter,
                                                                   uint8_t set_selector,
                                                                   uint8_t block_selector,
                                                                   fiid_obj_t obj_cmd_rs)
{
  if (_ipmi_cmd_get_pef_configuration_parameters_common (ctx,
                                                         get_parameter,
                                                         set_selector,
                                                         block_selector,
                                                         obj_cmd_rs,
                                                         tmpl_cmd_get_pef_configuration_parameters_number_of_event_filters_rs,
                                                         IPMI_PEF_CONFIGURATION_PARAMETER_NUMBER_OF_EVENT_FILTERS) < 0)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      return (-1);
    }

  return (0);
}

int
ipmi_cmd_get_pef_configuration_parameters_event_filter_table (ipmi_ctx_t ctx,
                                                              uint8_t get_parameter,
                                                              uint8_t set_selector,
                                                              uint8_t block_selector,
                                                              fiid_obj_t obj_cmd_rs)
{
  if (_ipmi_cmd_get_pef_configuration_parameters_common (ctx,
                                                         get_parameter,
                                                         set_selector,
                                                         block_selector,
                                                         obj_cmd_rs,
                                                         tmpl_cmd_get_pef_configuration_parameters_event_filter_table_rs,
                                                         IPMI_PEF_CONFIGURATION_PARAMETER_EVENT_FILTER_TABLE) < 0)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      return (-1);
    }

  return (0);
}

int
ipmi_cmd_get_pef_configuration_parameters_event_filter_table_data1_ (ipmi_ctx_t ctx,
                                                                     uint8_t get_parameter,
                                                                     uint8_t set_selector,
                                                                     uint8_t block_selector,
                                                                     fiid_obj_t obj_cmd_rs)
{
  if (_ipmi_cmd_get_pef_configuration_parameters_common (ctx,
                                                         get_parameter,
                                                         set_selector,
                                                         block_selector,
                                                         obj_cmd_rs,
                                                         tmpl_cmd_get_pef_configuration_parameters_event_filter_table_data1_rs,
                                                         IPMI_PEF_CONFIGURATION_PARAMETER_EVENT_FILTER_TABLE_DATA_1) < 0)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      return (-1);
    }

  return (0);
}

int
ipmi_cmd_get_pef_configuration_parameters_number_of_alert_policy_entries (ipmi_ctx_t ctx,
                                                                          uint8_t get_parameter,
                                                                          uint8_t set_selector,
                                                                          uint8_t block_selector,
                                                                          fiid_obj_t obj_cmd_rs)
{
  if (_ipmi_cmd_get_pef_configuration_parameters_common (ctx,
                                                         get_parameter,
                                                         set_selector,
                                                         block_selector,
                                                         obj_cmd_rs,
                                                         tmpl_cmd_get_pef_configuration_parameters_number_of_alert_policy_entries_rs,
                                                         IPMI_PEF_CONFIGURATION_PARAMETER_NUMBER_OF_ALERT_POLICY_ENTRIES) < 0)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      return (-1);
    }

  return (0);
}

int
ipmi_cmd_get_pef_configuration_parameters_number_of_alert_strings (ipmi_ctx_t ctx,
                                                                   uint8_t get_parameter,
                                                                   uint8_t set_selector,
                                                                   uint8_t block_selector,
                                                                   fiid_obj_t obj_cmd_rs)
{
  if (_ipmi_cmd_get_pef_configuration_parameters_common (ctx,
                                                         get_parameter,
                                                         set_selector,
                                                         block_selector,
                                                         obj_cmd_rs,
                                                         tmpl_cmd_get_pef_configuration_parameters_number_of_alert_strings_rs,
                                                         IPMI_PEF_CONFIGURATION_PARAMETER_NUMBER_OF_ALERT_STRINGS) < 0)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      return (-1);
    }

  return (0);
}

int
ipmi_cmd_get_pef_configuration_parameters_alert_string_keys (ipmi_ctx_t ctx,
                                                             uint8_t get_parameter,
                                                             uint8_t set_selector,
                                                             uint8_t block_selector,
                                                             fiid_obj_t obj_cmd_rs)
{
  if (_ipmi_cmd_get_pef_configuration_parameters_common (ctx,
                                                         get_parameter,
                                                         set_selector,
                                                         block_selector,
                                                         obj_cmd_rs,
                                                         tmpl_cmd_get_pef_configuration_parameters_alert_string_keys_rs,
                                                         IPMI_PEF_CONFIGURATION_PARAMETER_ALERT_STRING_KEYS) < 0)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      return (-1);
    }

  return (0);
}

int
ipmi_cmd_get_pef_configuration_parameters_alert_string (ipmi_ctx_t ctx,
                                                        uint8_t get_parameter,
                                                        uint8_t set_selector,
                                                        uint8_t block_selector,
                                                        fiid_obj_t obj_cmd_rs)
{
  if (_ipmi_cmd_get_pef_configuration_parameters_common (ctx,
                                                         get_parameter,
                                                         set_selector,
                                                         block_selector,
                                                         obj_cmd_rs,
                                                         tmpl_cmd_get_pef_configuration_parameters_alert_strings_rs,
                                                         IPMI_PEF_CONFIGURATION_PARAMETER_ALERT_STRINGS) < 0)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      return (-1);
    }

  return (0);
}

int
ipmi_cmd_get_pef_configuration_parameters_alert_policy_table (ipmi_ctx_t ctx,
                                                              uint8_t get_parameter,
                                                              uint8_t set_selector,
                                                              uint8_t block_selector,
                                                              fiid_obj_t obj_cmd_rs)
{
  if (_ipmi_cmd_get_pef_configuration_parameters_common (ctx,
                                                         get_parameter,
                                                         set_selector,
                                                         block_selector,
                                                         obj_cmd_rs,
                                                         tmpl_cmd_get_pef_configuration_parameters_alert_policy_table_rs,
                                                         IPMI_PEF_CONFIGURATION_PARAMETER_ALERT_POLICY_TABLE) < 0)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      return (-1);
    }

  return (0);
}

int
ipmi_cmd_set_last_processed_event_id (ipmi_ctx_t ctx,
                                      uint8_t set_record_id_for_last_record,
                                      uint16_t record_id,
                                      fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int rv = -1;

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      return (-1);
    }

  /* remaining parameter checks in fill function */
  if (!fiid_obj_valid (obj_cmd_rs))
    {
      API_SET_ERRNUM (ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }

  if (FIID_OBJ_TEMPLATE_COMPARE (obj_cmd_rs,
                                 tmpl_cmd_set_last_processed_event_id_rs) < 0)
    {
      API_FIID_OBJECT_ERROR_TO_API_ERRNUM (ctx, obj_cmd_rs);
      return (-1);
    }

  if (!(obj_cmd_rq = fiid_obj_create (tmpl_cmd_set_last_processed_event_id_rq)))
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_set_last_processed_event_id (set_record_id_for_last_record,
                                            record_id,
                                            obj_cmd_rq) < 0)
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (api_ipmi_cmd (ctx,
                    IPMI_BMC_IPMB_LUN_BMC,
                    IPMI_NET_FN_SENSOR_EVENT_RQ,
                    obj_cmd_rq,
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      goto cleanup;
    }

  rv = 0;
 cleanup:
  fiid_obj_destroy (obj_cmd_rq);
  return (rv);
}

int
ipmi_cmd_get_last_processed_event_id (ipmi_ctx_t ctx, fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int rv = -1;

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      return (-1);
    }

  if (!fiid_obj_valid (obj_cmd_rs))
    {
      API_SET_ERRNUM (ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }

  if (FIID_OBJ_TEMPLATE_COMPARE (obj_cmd_rs,
                                 tmpl_cmd_get_last_processed_event_id_rs) < 0)
    {
      API_FIID_OBJECT_ERROR_TO_API_ERRNUM (ctx, obj_cmd_rs);
      return (-1);
    }

  if (!(obj_cmd_rq = fiid_obj_create (tmpl_cmd_get_last_processed_event_id_rq)))
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_get_last_processed_event_id (obj_cmd_rq) < 0)
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (api_ipmi_cmd (ctx,
                    IPMI_BMC_IPMB_LUN_BMC,
                    IPMI_NET_FN_SENSOR_EVENT_RQ,
                    obj_cmd_rq,
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      goto cleanup;
    }

  rv = 0;
 cleanup:
  fiid_obj_destroy (obj_cmd_rq);
  return (rv);
}

int
ipmi_cmd_alert_immediate (ipmi_ctx_t ctx,
                          uint8_t channel_number,
                          uint8_t destination_selector,
                          uint8_t operation,
                          uint8_t string_selector,
                          uint8_t send_alert_string,
                          fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int rv = -1;

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      return (-1);
    }

  /* remaining parameter checks in fill function */
  if (!fiid_obj_valid (obj_cmd_rs))
    {
      API_SET_ERRNUM (ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }

  if (FIID_OBJ_TEMPLATE_COMPARE (obj_cmd_rs,
                                 tmpl_cmd_alert_immediate_rs) < 0)
    {
      API_FIID_OBJECT_ERROR_TO_API_ERRNUM (ctx, obj_cmd_rs);
      return (-1);
    }

  if (!(obj_cmd_rq = fiid_obj_create (tmpl_cmd_alert_immediate_rq)))
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_alert_immediate (channel_number,
                                destination_selector,
                                operation,
                                string_selector,
                                send_alert_string,
                                obj_cmd_rq) < 0)
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (api_ipmi_cmd (ctx,
                    IPMI_BMC_IPMB_LUN_BMC,
                    IPMI_NET_FN_SENSOR_EVENT_RQ,
                    obj_cmd_rq,
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      goto cleanup;
    }

  rv = 0;
 cleanup:
  fiid_obj_destroy (obj_cmd_rq);
  return (rv);
}

int
ipmi_cmd_pet_acknowledge (ipmi_ctx_t ctx,
                          uint16_t sequence_number,
                          uint32_t local_timestamp,
                          uint8_t event_source_type,
                          uint8_t sensor_device,
                          uint8_t sensor_number,
                          uint32_t event_data,
                          fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int rv = -1;

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      return (-1);
    }

  if (!fiid_obj_valid (obj_cmd_rs))
    {
      API_SET_ERRNUM (ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }

  if (FIID_OBJ_TEMPLATE_COMPARE (obj_cmd_rs,
                                 tmpl_cmd_pet_acknowledge_rs) < 0)
    {
      API_FIID_OBJECT_ERROR_TO_API_ERRNUM (ctx, obj_cmd_rs);
      return (-1);
    }

  if (!(obj_cmd_rq = fiid_obj_create (tmpl_cmd_pet_acknowledge_rq)))
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_pet_acknowledge (sequence_number,
                                local_timestamp,
                                event_source_type,
                                sensor_device,
                                sensor_number,
                                event_data,
                                obj_cmd_rq) < 0)
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (api_ipmi_cmd (ctx,
                    IPMI_BMC_IPMB_LUN_BMC,
                    IPMI_NET_FN_SENSOR_EVENT_RQ,
                    obj_cmd_rq,
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      goto cleanup;
    }

  rv = 0;
 cleanup:
  fiid_obj_destroy (obj_cmd_rq);
  return (rv);
}

