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

#ifndef IPMI_NETWORK_H
#define IPMI_NETWORK_H

#include <sys/types.h>
#include <sys/socket.h>

ssize_t ipmi_network_sendto (int s,
                             const void *buf,
                             size_t len,
                             int flags,
                             const struct sockaddr *to,
                             socklen_t tolen);

ssize_t ipmi_network_recvfrom (int s,
                               void *buf,
                               size_t len,
                               int flags,
                               struct sockaddr *from,
                               socklen_t *fromlen);

#endif /* IPMI_NETWORK_H */
