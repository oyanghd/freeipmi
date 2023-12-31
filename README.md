# freeipmi
mirror version to https://git.savannah.gnu.org/git/freeipmi.git

FreeIPMI
--------

FreeIPMI provides in-band and out-of-band IPMI software based on the IPMI v1.5/2.0 specification.

What is IPMI?
-------------

The IPMI specification defines a set of interfaces for platform management.  It is utilized by a wide variety of vendors for system management on motherboards.  The features of IPMI that most users will be interested in are sensor monitoring, remote power control, serial-over-LAN (SOL), and system debugging.  The FreeIPMI tools and libraries listed below should provide users with the ability to access and utilize these features.

Getting Started with IPMI
-------------------------

IPMI can be used in-band (i.e. running on a machine locally) or out-of-band (i.e. connecting remotely).

Most FreeIPMI tools can operate in-band by using one of the in-band drivers included.  These in-band drivers include a userspace KCS interface driver, a SSIF driver through the Linux SSIF device (i.e. /dev/i2c-0), the OpenIPMI Linux kernel driver (i.e. /dev/ipmi0), the Sun/Solaris BMC driver (i.e. /dev/bmc), and the Intel DCMI/MEI driver (i.e. /dev/dcmi).  If your system requires the use of installed drivers, those appropriate modules must be installed ahead of time. However, most systems should automatically load these drivers when appropriate.

Under most scenarios, the FreeIPMI tools should automatically discover which in-band interface to use and the proper settings to use.  Users may execute the tools on the command line to begin using them.  Some
motherboards may require you to determine driver type, addresses, paths, etc. on your own and pass them as command line options to the tools.  You may use ipmi-locate(8) to help determine this information. Other tools such as dmidecode(8) may also provide this information.

To use IPMI out-of-band with tools such as ipmipower(8) or ipmi-sensors(8), the remote machine's BMC must first be configured for out of band communication.  Typically, this involves setting a username, password, IP address, MAC address, and a few other parameters.  This can be done using the tool ipmi-config(8). Additional information on how to configure with ipmi-config(8) can be found in the ipmi-config.conf(5) manpage.  Some vendors may pre-configure their motherboards with default values so that ipmi-config(8) can be used remotely to configure the machine.  However, most of the time, the BMC must be configured in-band before out-of-band access can be allowed (for example, the correct IP address and MAC address must be configured).

In order to remotely connect to a machine, you typically must specify the host, username, and password for the tool in order to connect. Depending on configuration settings, a K_g key, privilege level, authentication type, cipher suite id, or protocol version may need to be specified.

Some vendors may have not implemented IPMI properly and a workaround must be specified into FreeIPMI to ensure the tool can execute properly.  For example, a fair number of vendors have populated their FRU records with invalid checksums.  To properly ignore these set of checksums a 'skipchecks' workaround has been added to ipmi-fru(8). Please see each of the tool manpages to see a list of available workarounds.

Additional information, examples, and general trouble-shooting can be found in each of the tool manpages.

General Use
-----------

The primary tools that most users of FreeIPMI will be interested in for system management are the following:

```
ipmi-sensors
```

A tool to read IPMI sensor readings to aid in system monitoring.

```
ipmi-sel
```

A tool to read and manage IPMI System Event Log (SEL) records to aid
in system debugging.

```
ipmipower
```

A tool for remote power control.

```
ipmiconsole
```

A tool for Serial-over-Lan (SOL) console access.

Many other tools and libraries are listed below that cover additional features and areas of IPMI.

Additional information, examples, and general trouble-shooting can be found in each of the tool manpages.

Configuration
-------------

In order to avoid typing in a long list of command line options to specify IPMI communication requirements every time a command is executed (e.g. driver paths, usernames, passwords, etc.), an alternate set of default values can be set for most FreeIPMI tools in the FreeIPMI configuration file.  See freeipmi.conf(5) for more information.

HPC Support
-----------

Much of FreeIPMI was written with HPC support in mind.  Ipmi-config(8) comes with file input/output support so that configuration can be copied and verified across nodes in a cluster.  Most tools (like ipmipower(8) and ipmi-sensors(8)) come with hostrange support so multiple hosts can be specified on the command line at the same time and IPMI can be executed against the hosts in parallel.  See tool manpages for more information.  Also see the document freeipmi-hostrange.txt for detailed usage and explanation. Ipmi-sensors(8) and the libipmimonitoring(3) library support the ability to interpret sensor readings as well as just reporting them. By mapping sensor readings into NOMINAL, WARNING, or CRITICAL states, it makes monitoring sensors easier across large numbers of nodes.

Development
-----------

For information on the libraries that can be used to program IPMI applications with, please see, please see libfreeipmi(3), libipmiconsole(3), libipmimonitoring(3), and libipmidetect(3).  Or see the document freeipmi-libraries.txt.

Project Tools
-------------

The following tools are distributed and supported by FreeIPMI.

```
bmc-info
```

A tool to read information about a BMC such as device version numbers, device support, and globally unique IDs (guids).

```
bmc-watchdog
```

A tool/daemon to manage a BMC Watchdog. This tool is typically used for system timeout management and automatic system restarts in the event of a system crash.

```
ipmi-chassis
```

A tool to manage/monitor a chassis, such as chassis power, identification (i.e. LED control), and status.

```
ipmi-fru
```

A tool to read field replaceable unit (FRU) information from a motherboard/machine.

```
ipmi-sel
```

A tool to read and manage IPMI System Event Log (SEL) records.  SEL records store system event information and may be useful for debugging problems.

```
ipmi-sensors
```

A tool to read IPMI sensor readings and sensor data repository (SDR) information.

```
ipmipower
```

A tool for remote power control.

```
ipmiconsole
```

A tool for Serial-over-Lan (SOL) console access.

```
ipmi-config
```

A tool to configure BMC and IPMI information.  In can be used to configured usernames, passwords, networking information, security, Serial-over-LAN (SOL), Platform Event Filtering (PEF), boot devices, power restoration policy, sensor thresholds, sensor events, and many more configuration options.

```
ipmi-raw
```

A tool that provides hex input/output of IPMI commands.

```
ipmi-locate
```

A tool that can probe for information about the location of a BMC device, such as device addresses.

```
ipmi-pet
```

A tool to parse and interpret Platform Event Traps (PET).

```
ipmi-dcmi
```

A tool to perform Data Center Manageability Interface (DCMI) IPMI extension commands.  Supports extensions for asset management and power usage management.

```
bmc-device
```

A tool to perform advanced BMC commands, such as resetting the BMC, configuring ACPI, configuring SDR/SEL time, manually generating events, re-arming sensors, and configuring manufacturer settings.

```
ipmiping
```

An IPMI ping tool for debugging.

```
rmcpping
```

A RMCP ping tool for debugging.

```
ipmi-oem
```

An IPMI tool for OEM specific commands.

```
ipmidetect/ipmidetectd
```

A tool and daemon for IPMI node detection.

```
ipmiseld
```

A daemon that regularly polls the SEL and stores the events to the local syslog.

Additional information, examples, and general trouble-shooting can be found in each of the tool manpages.

Project Libraries
-----------------

The following libraries are distributed and supported by FreeIPMI.

```
libfreeipmi
```

A C library that includes KCS, SSIF, OpenIPMI Linux, and Solaris BMC drivers, IPMI 1.5 and IPMI 2.0 LAN communication interfaces, IPMI packet building utilities, IPMI command utilities, and utilities for reading/interpreting/managing IPMI.  This library is for programmers intimately familiar with the IPMI protocol and IPMI specification. Most users may wish to use the libraries listed below.

```
libipmiconsole
```

A library for Serial-over-Lan (SOL) console access.  SOL console access is abstracted into a file descriptor interface, so users may read and write console data through a file descriptor.

```
libipmimonitoring
```

A library for sensor and system event log (SEL) monitoring that abstracts away most IPMI details.  Interpretation of those sensors and events is abstracted into an API with an iterator interface.

```
libipmidetect
```

A library for IPMI node detection.

Building
--------

For those who have downloaded official releases, please follow the directions in INSTALL for building and installation.

For those trying to build from the repo you typically need to run `autogen.sh` first.  i.e.

> ./autogen.sh; ./configure; make

to build.

Most systems should have all the packages you need to build already installed.  The one semi-untypical package that may not be installed is libgcrypt and its development headers (i.e. libgcrypt-devel or libgcrypt-dev).

To build an rpm, you typically want to make a release distribution and use rpmbuild with it.  In modern systems, typically the following will work:

> make dist
> rpmbuild -ta --with systemd freeipmi-<VERSION>.tar.gz

Help, Questions, etc.
---------------------

Please send comments, help, and questions to the freeipmi mailing lists, freeipmi-users@gnu.org and freeipmi-devel@gnu.org.  Or see our webpage at http://www.gnu.org/software/freeipmi/.
