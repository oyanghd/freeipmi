/*****************************************************************************\
 *  $Id: ipmiseld.h,v 1.11 2010-02-08 22:02:30 chu11 Exp $
 *****************************************************************************
 *  Copyright (C) 2012-2015 Lawrence Livermore National Security, LLC.
 *  Produced at Lawrence Livermore National Laboratory (cf, DISCLAIMER).
 *  Written by Albert Chu <chu11@llnl.gov>
 *  LLNL-CODE-559172
 *
 *  This file is part of Ipmiseld, an IPMI SEL syslog logging daemon.
 *  For details, see https://savannah.gnu.org/projects/freeipmi/.
 *
 *  Ipmiseld is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by the
 *  Free Software Foundation; either version 3 of the License, or (at your
 *  option) any later version.
 *
 *  Ipmiseld is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 *  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with Ipmiseld.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

#ifndef IPMISELD_CACHE_H
#define IPMISELD_CACHE_H

#include "ipmiseld.h"

int ipmiseld_sdr_cache_create_and_load (ipmiseld_host_data_t *host_data);

/* returns 1 on data found/loaded, 0 if not found, -1 on error loading
 *  (permission, corrupted, etc.)
 */
int ipmiseld_data_cache_load (ipmiseld_host_data_t *host_data);

int ipmiseld_data_cache_store (ipmiseld_host_data_t *host_data);

#endif /* IPMISELD_CACHE_H */
