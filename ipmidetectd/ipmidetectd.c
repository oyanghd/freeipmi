/*****************************************************************************\
 *  $Id: ipmidetectd.c,v 1.17 2010-02-08 22:02:30 chu11 Exp $
 *****************************************************************************
 *  Copyright (C) 2007-2015 Lawrence Livermore National Security, LLC.
 *  Copyright (C) 2007 The Regents of the University of California.
 *  Produced at Lawrence Livermore National Laboratory (cf, DISCLAIMER).
 *  Written by Albert Chu <chu11@llnl.gov>
 *  UCRL-CODE-228523
 *
 *  This file is part of Ipmidetect, tools and libraries for detecting
 *  IPMI nodes in a cluster. For details, see https://savannah.gnu.org/projects/freeipmi/.
 *
 *  Ipmidetect is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by the
 *  Free Software Foundation; either version 3 of the License, or (at your
 *  option) any later version.
 *
 *  Ipmidetect is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 *  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with Ipmidetect.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#if STDC_HEADERS
#include <string.h>
#endif /* STDC_HEADERS */
#if TIME_WITH_SYS_TIME
#include <sys/time.h>
#include <time.h>
#else  /* !TIME_WITH_SYS_TIME */
#if HAVE_SYS_TIME_H
#include <sys/time.h>
#else /* !HAVE_SYS_TIME_H */
#include <time.h>
#endif  /* !HAVE_SYS_TIME_H */
#endif /* !TIME_WITH_SYS_TIME */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/poll.h>
#if HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */
#if HAVE_FCNTL_H
#include <fcntl.h>
#endif /* HAVE_FCNTL_H */
#include <sys/stat.h>
#include <syslog.h>
#include <limits.h>
#include <signal.h>
#include <assert.h>
#include <errno.h>

#include <freeipmi/freeipmi.h>

#include "ipmidetectd.h"
#include "ipmidetectd-argp.h"
#include "ipmidetectd-config.h"

#include "freeipmi-portability.h"
#include "error.h"
#include "fd.h"
#include "fi_hostlist.h"
#include "hash.h"
#include "list.h"
#include "network.h"
#include "timeval.h"

#include "tool-daemon-common.h"

#define IPMIDETECTD_PIDFILE IPMIDETECTD_LOCALSTATEDIR "/run/ipmidetectd.pid"

#define IPMIDETECTD_BUFLEN           1024
#define IPMIDETECTD_NODES_PER_SOCKET 8
#define IPMIDETECTD_SERVER_BACKLOG   5

/* IPMI has a 6 bit sequence number */
#define IPMI_RQ_SEQ_MAX  0x3F

struct ipmidetectd_arguments cmd_args;

struct ipmidetectd_config conf;

static struct timeval ipmidetectd_next_send;

struct ipmidetectd_info
{
  char *hostname;
  int fd;
  struct sockaddr *destaddr;
  socklen_t destaddr_len;
  struct sockaddr_in destaddr4;
  struct sockaddr_in6 destaddr6;
  char ipstr[IPMIDETECTD_BUFLEN + 1];
  unsigned int sequence_number;
  struct timeval last_received;
};

int *fds = NULL;
unsigned int fds_count = 0;
List nodes = NULL;
unsigned int nodes_count = 0;
hash_t nodes_index = NULL;
int server_fd = 0;

extern int h_errno;

static int exit_flag = 1;

static void
_fds_setup (void)
{
  struct sockaddr_in6 addr6;
  struct sockaddr_in6 servaddr;
  int option_value;
  socklen_t option_value_len;
  unsigned int i;

  assert (!fds);
  assert (!fds_count);
  assert (!nodes_count);
  assert (!server_fd);

  /* IPv4 and IPv6 fds are not needed in the general sense, however b/c
   * we're doing up/down based on IP/string matching, we need binding so
   * that what is received back on recvfrom() calls matches what we sent
   * via IPv4 or IPv6.
   */
  nodes_count = fi_hostlist_count (conf.hosts);
  fds_count = nodes_count/IPMIDETECTD_NODES_PER_SOCKET;
  if (nodes_count % IPMIDETECTD_NODES_PER_SOCKET)
    fds_count++;

  if (!(fds = (int *)malloc (fds_count * sizeof (int))))
    err_exit ("malloc: %s", strerror (errno));

  for (i = 0; i < fds_count; i++)
    {
      if ((fds[i] = socket (AF_INET6, SOCK_DGRAM, 0)) < 0)
        err_exit ("socket: %s", strerror (errno));

      memset (&addr6, '\0', sizeof (struct sockaddr_in6));
      addr6.sin6_family = AF_INET6;
      addr6.sin6_port = htons (0);

      if (bind (fds[i], (struct sockaddr *)&addr6, sizeof (struct sockaddr_in6)) < 0)
        err_exit ("bind: %s", strerror (errno));
    }

  if ((server_fd = socket (AF_INET6, SOCK_STREAM, 0)) < 0)
    err_exit ("socket: %s", strerror (errno));

  memset (&servaddr, '\0', sizeof (struct sockaddr_in6));
  servaddr.sin6_family = AF_INET6;
  servaddr.sin6_port = htons (conf.ipmidetectd_server_port);

  if (bind (server_fd, (struct sockaddr *)&servaddr, sizeof (struct sockaddr_in6)) < 0)
    err_exit ("bind: %s", strerror (errno));

  /* For quick start/restart */
  option_value = 1;
  option_value_len = sizeof(option_value);

  if (setsockopt (server_fd,
                  SOL_SOCKET,
                  SO_REUSEADDR,
                  &option_value,
                  option_value_len) < 0)
    err_exit ("setsockopt: %s", strerror (errno));

  if (listen (server_fd, IPMIDETECTD_SERVER_BACKLOG) < 0)
    err_exit ("listen: %s", strerror (errno));
}

static void
_nodes_setup (void)
{
  fi_hostlist_iterator_t itr = NULL;
  char *host = NULL;
  int count = 0;

  assert (fds);
  assert (fds_count);
  assert (!nodes);
  assert (nodes_count);
  assert (!nodes_index);

  if (!(nodes = list_create ((ListDelF)free)))
    err_exit ("list_create: %s", strerror (errno));

  if (!(nodes_index = hash_create (nodes_count,
                                   (hash_key_f)hash_key_string,
                                   (hash_cmp_f)strcmp,
                                   NULL)))
    err_exit ("hash_create: %s", strerror (errno));

  if (!(itr = fi_hostlist_iterator_create (conf.hosts)))
    err_exit ("fi_hostlist_iterator_create: %s", strerror (errno));

  while ((host = fi_hostlist_next (itr)))
    {
      struct ipmidetectd_info *info = NULL;
      int len;
      char *host_copy = NULL;
      char *host_ptr = NULL;
      char *port_copy = NULL;
      char *port_ptr = NULL;
      uint16_t port = RMCP_PRIMARY_RMCP_PORT;
      char port_str[IPMIDETECTD_BUFLEN + 1];
      struct addrinfo ai_hints, *ai_res = NULL, *ai = NULL;
      int ret;

      if (!(info = (struct ipmidetectd_info *)malloc (sizeof (struct ipmidetectd_info))))
        err_exit ("malloc: %s", strerror (errno));
      memset (info, '\0', sizeof (struct ipmidetectd_info));

      if ((ret = host_is_host_with_port (host, &host_copy, &port_copy)) < 0)
        err_exit ("host_is_host_with_port: %s", strerror (errno));

      if (ret)
        {
          host_ptr = host_copy;
          port_ptr = port_copy;
        }
      else
        host_ptr = host;

      if ((ret = host_is_valid (host_ptr, port_ptr, &port)) < 0)
        err_exit ("host_is_valid: %s", strerror (errno));

      if (!ret)
        err_exit ("Invalid host specified: %s", host);

      if (!(info->hostname = strdup (host_ptr)))
        err_exit ("strdup: %s", strerror (errno));

      /* Use random number for starting sequence number to avoid probability of
       * duplicates and "hanging" BMC issue.
       */
      if ((len = ipmi_get_random (&(info->sequence_number),
                                  sizeof (info->sequence_number))) < 0)
        err_exit ("ipmi_get_random: %s", strerror (errno));
      if (len != sizeof (info->sequence_number))
        err_exit ("ipmi_get_random: invalid len returned");

      memset (port_str, '\0', IPMIDETECTD_BUFLEN + 1);
      snprintf (port_str, IPMIDETECTD_BUFLEN, "%d", port);
      memset (&ai_hints, 0, sizeof (struct addrinfo));
      ai_hints.ai_family = AF_UNSPEC;
      ai_hints.ai_socktype = SOCK_DGRAM;
      ai_hints.ai_flags = (AI_V4MAPPED | AI_ADDRCONFIG);

      if ((ret = getaddrinfo (host_ptr, port_str, &ai_hints, &ai_res)))
        {
          if (ret == EAI_NODATA)
            err_exit ("Invalid hostname: %s", host_ptr);
          else
            err_exit ("getaddrinfo %s: %s", host_ptr, gai_strerror (ret));
        }

      /* Try all of the different answers we got, until we succeed. */
      for (ai = ai_res; ai != NULL; ai = ai->ai_next)
        {
          if (ai->ai_family == AF_INET)
            {
              char tmpipv4buf[IPMIDETECTD_BUFLEN + 1];
              char tmpipv6buf[IPMIDETECTD_BUFLEN + 1];
              struct in6_addr in6;

              memcpy (&(info->destaddr4), ai->ai_addr, ai->ai_addrlen);
              info->destaddr = (struct sockaddr *)&(info->destaddr4);
              info->destaddr_len = sizeof (struct sockaddr_in);

              /* We're using a IPv6 socket, so get IPv4 mapped IPv6 string  */

              memset (tmpipv4buf, '\0', IPMIDETECTD_BUFLEN + 1);
              if (!inet_ntop (AF_INET,
                              &(info->destaddr4.sin_addr),
                              tmpipv4buf,
                              IPMIDETECTD_BUFLEN))
                err_exit ("inet_ntop: %s", strerror (errno));

              memset (tmpipv6buf, '\0', IPMIDETECTD_BUFLEN + 1);
              snprintf (tmpipv6buf, IPMIDETECTD_BUFLEN,
                        "::ffff:%s", tmpipv4buf);

              if (inet_pton (AF_INET6,
                             tmpipv6buf,
                             &in6) != 1)
                err_exit ("inet_pton: %s", strerror (errno));

              /* We inet_ntop() one more time to get the consistent
               * string result we can always expect from inet_ntop(),
               * b/c inet_ntop() may not use the style format we use
               * above
               */
              if (!inet_ntop (AF_INET6,
                              &in6,
                              info->ipstr,
                              IPMIDETECTD_BUFLEN))
                err_exit ("inet_ntop: %s", strerror (errno));

	      info->fd = fds[count/IPMIDETECTD_NODES_PER_SOCKET];
	      count++;
            }
          else if (ai->ai_family == AF_INET6)
            {
              memcpy (&(info->destaddr6), ai->ai_addr, ai->ai_addrlen);
              info->destaddr = (struct sockaddr *)&(info->destaddr6);
              info->destaddr_len = sizeof (struct sockaddr_in6);

              if (!inet_ntop (AF_INET6,
                              &(info->destaddr6.sin6_addr),
                              info->ipstr,
                              IPMIDETECTD_BUFLEN))
                err_exit ("inet_ntop: %s", strerror (errno));

	      info->fd = fds[count/IPMIDETECTD_NODES_PER_SOCKET];
	      count++;
            }
          else
            continue;

          break;
        }

      if (!ai)
        err_exit ("Invalid hostname: %s", host_ptr);

      freeaddrinfo (ai_res);
      free (host_copy);
      free (port_copy);
      free (host);

      if (!list_append (nodes, info))
        err_exit ("list_append: %s", strerror (errno));

      if (hash_find (nodes_index, info->ipstr))
        err_exit ("Duplicate host ip: %s", info->ipstr);

      if (!hash_insert (nodes_index, info->ipstr, info))
        err_exit ("hash_insert: %s", strerror (errno));
    }

  fi_hostlist_iterator_destroy (itr);
}

static void
_ipmidetectd_setup (void)
{
  /* Initialize ipmidetectd_next_send to 0 so there is a sweep of pings in the beginning */
  memset (&ipmidetectd_next_send, '\0', sizeof (struct timeval));

  _fds_setup ();
  _nodes_setup ();

  /* Avoid sigpipe exiting during server writes */
  if (signal (SIGPIPE, SIG_IGN) == SIG_ERR)
    err_exit ("signal: %s", strerror (errno));
}

static int
_ipmi_ping_build (struct ipmidetectd_info *info, uint8_t *buf, unsigned int buflen)
{
  fiid_obj_t obj_rmcp_hdr = NULL;
  fiid_obj_t obj_lan_session_hdr = NULL;
  fiid_obj_t obj_lan_msg_hdr = NULL;
  fiid_obj_t obj_cmd = NULL;
  int len;

  assert (info);
  assert (buf);
  assert (buflen);

  if (!(obj_rmcp_hdr = fiid_obj_create (tmpl_rmcp_hdr)))
    err_exit ("fiid_obj_create: %s", strerror (errno));
  if (!(obj_lan_session_hdr = fiid_obj_create (tmpl_lan_session_hdr)))
    err_exit ("fiid_obj_create: %s", strerror (errno));
  if (!(obj_lan_msg_hdr = fiid_obj_create (tmpl_lan_msg_hdr_rq)))
    err_exit ("fiid_obj_create: %s", strerror (errno));
  if (!(obj_cmd = fiid_obj_create (tmpl_cmd_get_channel_authentication_capabilities_rq)))
    err_exit ("fiid_obj_create: %s", strerror (errno));

  if (fill_rmcp_hdr_ipmi (obj_rmcp_hdr) < 0)
    err_exit ("fill_rmcp_hdr_ipmi: %s", strerror (errno));

  if (fill_lan_session_hdr (IPMI_AUTHENTICATION_TYPE_NONE,
                            0,
                            0,
                            obj_lan_session_hdr) < 0)
    err_exit ("fill_lan_session_hdr: %s", strerror (errno));

  if (fill_lan_msg_hdr (IPMI_SLAVE_ADDRESS_BMC,
                        IPMI_NET_FN_APP_RQ,
                        IPMI_BMC_IPMB_LUN_BMC,
                        info->sequence_number % (IPMI_RQ_SEQ_MAX+1),
                        obj_lan_msg_hdr) < 0)
    err_exit ("fill_lan_msg_hdr: %s", strerror (errno));

  if (fill_cmd_get_channel_authentication_capabilities (IPMI_CHANNEL_NUMBER_CURRENT_CHANNEL,
                                                        IPMI_PRIVILEGE_LEVEL_USER,
                                                        IPMI_GET_IPMI_V15_DATA,
                                                        obj_cmd) < 0)
    err_exit ("fill_cmd_get_channel_authentication_capabilities: %s", strerror (errno));

  if ((len = assemble_ipmi_lan_pkt (obj_rmcp_hdr,
                                    obj_lan_session_hdr,
                                    obj_lan_msg_hdr,
                                    obj_cmd,
                                    NULL,
                                    0,
                                    buf,
                                    buflen,
                                    IPMI_INTERFACE_FLAGS_DEFAULT)) < 0)
    err_exit ("assemble_ipmi_lan_pkt: %s", strerror (errno));

#if 0
  if (cmd_args.debug)
    {
      if (ipmi_dump_lan_packet (STDERR_FILENO,
                                info->hostname,
                                NULL,
                                buf,
                                len,
                                tmpl_lan_msg_hdr_rq,
                                tmpl_cmd_get_channel_authentication_capabilities_rq) < 0)
        err_exit ("ipmi_dump_lan_packet: %s", strerror (errno));
    }
#endif

  fiid_obj_destroy (obj_rmcp_hdr);
  fiid_obj_destroy (obj_lan_session_hdr);
  fiid_obj_destroy (obj_lan_msg_hdr);
  fiid_obj_destroy (obj_cmd);

  info->sequence_number++;
  return (len);
}

static void
_ipmidetectd_send_pings (void)
{
  uint8_t buf[IPMIDETECTD_BUFLEN];
  int len;
  struct ipmidetectd_info *info;
  ListIterator itr;

  assert (nodes);
  assert (nodes_count);

  if (!(itr = list_iterator_create (nodes)))
    err_exit ("list_iterator_create: %s", strerror (errno));

  while ((info = list_next (itr)))
    {
      memset (buf, '\0', IPMIDETECTD_BUFLEN);

      if ((len = _ipmi_ping_build (info, buf, IPMIDETECTD_BUFLEN)) < 0)
        err_exit ("_ipmi_ping_build: %s", strerror (errno));

      if (ipmi_lan_sendto (info->fd,
                           buf,
                           len,
                           0,
                           info->destaddr,
                           info->destaddr_len) < 0)
        err_exit ("ipmi_lan_sendto: %s", strerror (errno));

      if (cmd_args.debug)
        fprintf (stderr, "Ping Request to %s\n", info->hostname);
    }

  list_iterator_destroy (itr);
}

static void
_setup_pfds (struct pollfd *pfds)
{
  unsigned int i;

  assert (pfds);

  for (i = 0; i < fds_count; i++)
    {
      pfds[i].fd = fds[i];
      pfds[i].events = POLLIN;
      pfds[i].revents = 0;
    }

  pfds[fds_count].fd = server_fd;
  pfds[fds_count].events = POLLIN;
  pfds[fds_count].revents = 0;
}

static void
_receive_ping (int fd)
{
  struct sockaddr_in6 from6;
  struct sockaddr *from = (struct sockaddr *)&from6;
  struct ipmidetectd_info *info;
  uint8_t buf[IPMIDETECTD_BUFLEN];
  char ipbuf[IPMIDETECTD_BUFLEN + 1];
  int len;
  socklen_t fromlen = sizeof (struct sockaddr_in6);

  /* We're happy as long as we receive something.  We don't bother
   * checking sequence numbers or anything like that.
   */
  len = ipmi_lan_recvfrom (fd,
                           buf,
                           IPMIDETECTD_BUFLEN,
                           0,
                           from,
                           &fromlen);

  /* achu & hliebig:
   *
   * Premise from ipmitool (http://ipmitool.sourceforge.net/)
   *
   * On some OSes (it seems Unixes), the behavior is to not return
   * port denied errors up to the client for UDP responses (i.e. you
   * need to timeout).  But on some OSes (it seems Windows), the
   * behavior is to return port denied errors up to the user for UDP
   * responses via ECONNRESET or ECONNREFUSED.
   *
   * If this were just the case, we could return or handle errors
   * properly and move on.  However, it's not the case.
   *
   * According to Ipmitool, on some motherboards, both the OS and the
   * BMC are capable of responding to an IPMI request.  That means you
   * can get an ECONNRESET or ECONNREFUSED, then later on, get your
   * real IPMI response.
   *
   * Our solution is copied from Ipmitool, we'll ignore some specific
   * errors and try to read again.
   *
   * If the ECONNREFUSED or ECONNRESET is from the OS, but we will get
   * an IPMI response later, the recvfrom later on gets the packet we
   * want.
   *
   * If the ECONNREFUSED or ECONNRESET is from the OS but there is no
   * BMC (or IPMI disabled, etc.), just do the recvfrom again to
   * eventually get a timeout, which is the behavior we'd like.
   */
  if (len < 0
      && (errno == ECONNRESET
          || errno == ECONNREFUSED))
    return;

  if (len < 0)
    err_exit ("ipmi_lan_recvfrom: %s", strerror (errno));

  memset (ipbuf, '\0', IPMIDETECTD_BUFLEN + 1);
  if (from6.sin6_family == AF_INET6)
    {
      if (!inet_ntop (AF_INET6, &from6.sin6_addr, ipbuf, IPMIDETECTD_BUFLEN))
        err_exit ("inet_ntop: %s", strerror (errno));
    }
  else
    {
      /* memcpy hacks to avoid warnings, i.e.
       * warning: dereferencing pointer 'X' does break strict-aliasing rules
       */
      struct sockaddr_in from4;

      memcpy (&from4, from, fromlen);

      if (!inet_ntop (AF_INET, &from4.sin_addr, ipbuf, IPMIDETECTD_BUFLEN))
        err_exit ("inet_ntop: %s", strerror (errno));
    }

  if ((info = hash_find (nodes_index, ipbuf)))
    {
      if (gettimeofday (&(info->last_received), NULL) < 0)
        err_exit ("gettimeofday: %s", strerror (errno));

      if (cmd_args.debug)
        fprintf (stderr, "Ping Reply from %s\n", info->hostname);
    }
}

static void
_send_ping_data (void)
{
  ListIterator itr;
  struct sockaddr_in6 rhost;
  struct ipmidetectd_info *info;
  socklen_t rhost_len = sizeof (struct sockaddr_in6);
  int rhost_fd;

  assert (nodes);
  assert (nodes_count);

  if ((rhost_fd = accept (server_fd, (struct sockaddr *)&rhost, &rhost_len)) < 0)
    err_exit ("accept: %s", strerror (errno));

  if (cmd_args.debug)
    fprintf (stderr, "Received ipmidetectd server request\n");

  if (!(itr = list_iterator_create (nodes)))
    err_exit ("list_iterator_create: %s", strerror (errno));

  while ((info = list_next (itr)))
    {
      char buf[IPMIDETECTD_BUFLEN];
      int len, n;

      len = snprintf (buf, IPMIDETECTD_BUFLEN, "%s %lu\n", info->hostname, info->last_received.tv_sec);
      if (len >= IPMIDETECTD_BUFLEN)
        err_exit ("len=%d", len);

      if ((n = fd_write_n (rhost_fd, buf, len)) < 0)
        {
          if (errno == EPIPE)
            break;
          else
            err_exit ("fd_write_n: %s", strerror (errno));
        }

      if (n != len)
        err_exit ("fd_write_n: n=%d len=%d", n, len);
    }

  list_iterator_destroy (itr);
  /* ignore potential error, done w/ pipe */
  close (rhost_fd);
}

static void
_signal_handler_callback (int sig)
{
  exit_flag = 0;
}

static void
_ipmidetectd_loop (void)
{
  struct pollfd *pfds = NULL;
  unsigned int i;

  _ipmidetectd_setup ();

  assert (nodes_count);

  /* +1 fd for the server fd */
  if (!(pfds = (struct pollfd *)malloc ((fds_count + 1)*sizeof (struct pollfd))))
    err_exit ("malloc: %s", strerror (errno));

  while (exit_flag)
    {
      struct timeval now, timeout;
      unsigned int timeout_ms;
      int num;

      if (gettimeofday (&now, NULL) < 0)
        err_exit ("gettimeofday: %s", strerror (errno));

      if (timeval_gt (&now, &ipmidetectd_next_send))
        {
          _ipmidetectd_send_pings ();

          if (gettimeofday (&now, NULL) < 0)
            err_exit ("gettimeofday: %s", strerror (errno));

          timeval_add_ms (&now, conf.ipmiping_period, &ipmidetectd_next_send);
        }

      _setup_pfds (pfds);

      timeval_sub (&ipmidetectd_next_send, &now, &timeout);
      timeval_millisecond_calc (&timeout, &timeout_ms);

      if ((num = poll (pfds, fds_count + 1, timeout_ms)) < 0)
        err_exit ("poll: %s", strerror (errno));

      if (num)
        {
          for (i = 0; i < fds_count; i++)
            {
              if (pfds[i].revents & POLLERR
		  || pfds[i].revents & POLLIN)
		_receive_ping (fds[i]);
            }

          if (pfds[fds_count].revents & POLLIN)
            _send_ping_data ();
        }
    }
}

int
main (int argc, char **argv)
{
  err_init (argv[0]);
  err_set_flags (ERROR_STDERR);

  ipmidetectd_argp_parse (argc, argv, &cmd_args);

  ipmidetectd_config_setup (argc, argv);

  if (!cmd_args.debug)
    {
      daemonize_common (IPMIDETECTD_PIDFILE);
      err_set_flags (ERROR_SYSLOG);
    }
  else
    err_set_flags (ERROR_STDERR);

  daemon_signal_handler_setup (_signal_handler_callback);

  /* Call after daemonization, since daemonization closes currently
   * open fds
   */
  if (argv[0][0] == '/')
    argv[0] = strrchr(argv[0], '/') + 1;
  openlog (argv[0], LOG_ODELAY | LOG_PID, LOG_DAEMON);

  _ipmidetectd_loop ();

  return (0);
}
