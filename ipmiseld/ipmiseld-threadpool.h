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

#ifndef IPMISELD_THREADPOOL_H
#define IPMISELD_THREADPOOL_H

#include "ipmiseld.h"

typedef int (*IpmiSeldThreadPoolCallback)(void *arg);

typedef int (*IpmiSeldThreadPoolPostProcess)(void *arg);

int ipmiseld_threadpool_init (struct ipmiseld_prog_data *prog_data,
                              IpmiSeldThreadPoolCallback callback,
                              IpmiSeldThreadPoolPostProcess postprocess);

void ipmiseld_threadpool_destroy (void);

int ipmiseld_threadpool_queue (void *arg);

#endif /* IPMISELD_THREADPOOL_H */
