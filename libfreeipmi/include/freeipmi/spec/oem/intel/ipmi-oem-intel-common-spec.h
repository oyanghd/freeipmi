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

#ifndef IPMI_OEM_INTEL_COMMON_SPEC_H
#define IPMI_OEM_INTEL_COMMON_SPEC_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Intel S5500WB/Penguin Computing Relion 700
 * Intel S2600JF/Appro 512X
 */

/* w/ IPMI_CMD_OEM_INTEL_RESTORE_CONFIGURATION */
#define IPMI_OEM_INTEL_RESTORE_CONFIGURATION_OPERATION_INITIATE_RESTORE   0xAA
#define IPMI_OEM_INTEL_RESTORE_CONFIGURATION_OPERATION_GET_RESTORE_STATUS 0x00

#define IPMI_OEM_INTEL_RESTORE_CONFIGURATION_RESTORE_PROGRESS_RESTORE_IN_PROGRESS 0x00
#define IPMI_OEM_INTEL_RESTORE_CONFIGURATION_RESTORE_PROGRESS_RESTORE_COMPLETED   0x01

/* w/ IPMI_CMD_OEM_INTEL_SET_SMTP_CONFIGURATION / IPMI_CMD_OEM_INTEL_GET_SMTP_CONFIGURATION */

#define IPMI_OEM_INTEL_SMTP_CONFIGURATION_PARAMETER_ENABLE_SMTP            0x00
/* MS byte first */
#define IPMI_OEM_INTEL_SMTP_CONFIGURATION_PARAMETER_SMTP_SERVER_ADDRESS    0x01
#define IPMI_OEM_INTEL_SMTP_CONFIGURATION_PARAMETER_SMTP_USER_NAME         0x02
/* write only */
#define IPMI_OEM_INTEL_SMTP_CONFIGURATION_PARAMETER_USER_PASSWORD          0x03
/* read only */
#define IPMI_OEM_INTEL_SMTP_CONFIGURATION_PARAMETER_NUMBER_OF_DESTINATIONS 0x04
/* max 2 blocks - doc says 64 bytes, but 2 blocks appears to be correct */
#define IPMI_OEM_INTEL_SMTP_CONFIGURATION_PARAMETER_EMAIL_ADDRESS          0x05
/* max 2 blocks */
#define IPMI_OEM_INTEL_SMTP_CONFIGURATION_PARAMETER_SUBJECT                0x06
/* max 4 blocks */
#define IPMI_OEM_INTEL_SMTP_CONFIGURATION_PARAMETER_MESSAGE_CONTENT        0x07
/* max 4 blocks according to doc, but according to firmware max 1 block when you write */
#define IPMI_OEM_INTEL_SMTP_CONFIGURATION_PARAMETER_SENDER_EMAIL_ADDRESS   0x08
/* max 2 blocks */
#define IPMI_OEM_INTEL_SMTP_CONFIGURATION_PARAMETER_SMTP_HOST_NAME         0x09

#define IPMI_OEM_INTEL_SMTP_STRING_BLOCK_LENGTH 16

#define IPMI_OEM_INTEL_SMTP_CONFIGURATION_PARAMETER_SMTP_USER_NAME_MAX_BLOCKS       1
/* XXX: document says max 20 bytes, but fails on second block, so we use 1 block */
#define IPMI_OEM_INTEL_SMTP_CONFIGURATION_PARAMETER_USER_PASSWORD_MAX_BLOCKS        1
#define IPMI_OEM_INTEL_SMTP_CONFIGURATION_PARAMETER_EMAIL_ADDRESS_MAX_BLOCKS        2
#define IPMI_OEM_INTEL_SMTP_CONFIGURATION_PARAMETER_SUBJECT_MAX_BLOCKS              2
#define IPMI_OEM_INTEL_SMTP_CONFIGURATION_PARAMETER_MESSAGE_CONTENT_MAX_BLOCKS      4
/* XXX: document indicates 2 blocks, but firmware has errors on > 1 block, so we use 1 block */
#define IPMI_OEM_INTEL_SMTP_CONFIGURATION_PARAMETER_SENDER_EMAIL_ADDRESS_MAX_BLOCKS 1
/* XXX: document indicates 2 blocks, but firmware has errors on > 1 block, so we use 1 block */
#define IPMI_OEM_INTEL_SMTP_CONFIGURATION_PARAMETER_SMTP_HOST_NAME_MAX_BLOCKS       1

#define IPMI_OEM_INTEL_SMTP_CONFIGURATION_PARAMETER_STRING_LENGTH_MAX               (4*IPMI_OEM_INTEL_SMTP_STRING_BLOCK_LENGTH)

#define IPMI_OEM_INTEL_SMTP_CONFIGURATION_PARAMETER_USER_PASSWORD_LENGTH_MAX        20

/* To avoid gcc warnings, add +1 in comparison */
#define IPMI_OEM_INTEL_SMTP_CONFIGURATION_PARAMETER_VALID(__value)      \
  (((__value + 1) >= (IPMI_OEM_INTEL_SMTP_CONFIGURATION_PARAMETER_ENABLE_SMTP + 1) \
    && (__value) <= IPMI_OEM_INTEL_SMTP_CONFIGURATION_PARAMETER_SMTP_HOST_NAME) ? 1 : 0)

#define IPMI_OEM_INTEL_SMTP_CONFIGURATION_NO_SET_SELECTOR   0x0
#define IPMI_OEM_INTEL_SMTP_CONFIGURATION_NO_BLOCK_SELECTOR 0x0

#define IPMI_OEM_INTEL_SMTP_ENABLE_BITMASK 0x01
#define IPMI_OEM_INTEL_SMTP_ENABLE_SHIFT   0

#define IPMI_OEM_INTEL_SMTP_ENABLE  0x1
#define IPMI_OEM_INTEL_SMTP_DISABLE 0x0

#ifdef __cplusplus
}
#endif

#endif /* IPMI_OEM_INTEL_COMMON_SPEC_H */
