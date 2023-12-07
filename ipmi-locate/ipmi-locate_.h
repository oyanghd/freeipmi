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

/* file is named ipmi-locate_.h to differentiate itself from the
 * library ipmi-locate.h.
 *
 * I am scared of the portability of the #include_next directive, so
 * that's why I'm doing it this way.
 */

/* file is "ipmi-locate_.h", so double underscore */

#ifndef IPMI_LOCATE__H
#define IPMI_LOCATE__H

#include <freeipmi/freeipmi.h>

enum ipmi_locate_argp_option_keys
  {
    DEFAULTS_KEY = 160,
  };

struct ipmi_locate_arguments
{
  int defaults;
};

#endif /* IPMI_LOCATE__H */
