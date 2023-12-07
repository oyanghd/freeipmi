/*
 * Copyright (C) 2008-2015 FreeIPMI Core Team
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

#if HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>
#if STDC_HEADERS
#include <string.h>
#endif /* STDC_HEADERS */
#include <errno.h>
#include <assert.h>

#include "ipmi-config.h"
#include "ipmi-config-category-chassis-sections.h"
#include "ipmi-config-section.h"

#include "ipmi-config-category-chassis-boot-flags.h"
#include "ipmi-config-category-chassis-front-panel-buttons.h"
#include "ipmi-config-category-chassis-power-conf.h"

#include "freeipmi-portability.h"
#include "pstdout.h"

struct ipmi_config_section *
ipmi_config_chassis_sections_create (ipmi_config_state_data_t *state_data)
{
  struct ipmi_config_section *sections = NULL;
  struct ipmi_config_section *section = NULL;

  assert (state_data);

  if (!(section = ipmi_config_chassis_front_panel_buttons_get (state_data)))
    goto cleanup;
  if (ipmi_config_section_append (&sections, section) < 0)
    goto cleanup;

  if (!(section = ipmi_config_chassis_power_conf_get (state_data)))
    goto cleanup;
  if (ipmi_config_section_append (&sections, section) < 0)
    goto cleanup;

  if (!(section = ipmi_config_chassis_boot_flags_get (state_data)))
    goto cleanup;
  if (ipmi_config_section_append (&sections, section) < 0)
    goto cleanup;

  return (sections);

 cleanup:
  ipmi_config_sections_destroy (sections);
  return (NULL);
}
