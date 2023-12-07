
#ifndef IPMI_LOCATE_DEFS_H
#define IPMI_LOCATE_DEFS_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /*
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

#include <stdint.h>

#include "freeipmi/locate/ipmi-locate.h"

#define IPMI_LOCATE_CTX_MAGIC  0xF1CD9376

struct ipmi_locate_ctx {
  uint32_t magic;
  int errnum;
};

#endif /* IPMI_LOCATE_DEFS_H */
