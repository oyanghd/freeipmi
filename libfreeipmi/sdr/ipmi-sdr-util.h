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

#ifndef IPMI_SDR_UTIL_H
#define IPMI_SDR_UTIL_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>
#ifdef STDC_HEADERS
#include <string.h>
#endif /* STDC_HEADERS */
#include <errno.h>

#include "freeipmi/api/ipmi-api.h"
#include "freeipmi/sdr/ipmi-sdr.h"

#include "ipmi-sdr-defs.h"

void sdr_set_sdr_errnum_by_errno (ipmi_sdr_ctx_t ctx, int __errno);

void sdr_set_sdr_errnum_by_fiid_object (ipmi_sdr_ctx_t ctx, fiid_obj_t obj);

void sdr_set_internal_errnum (ipmi_sdr_ctx_t ctx);

#endif /* IPMI_SDR_UTIL_H */
