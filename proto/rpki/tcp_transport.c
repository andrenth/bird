/*
 *	BIRD -- An implementation of the TCP protocol for the RPKI protocol transport
 *
 *	(c) 2015 CZ.NIC
 *	(c) 2015 Pavel Tvrdik <pawel.tvrdik@gmail.com>
 *
 *	This file was a part of RTRlib: http://rpki.realmv6.org/
 *
 *	Can be freely distributed and used under the terms of the GNU GPL.
 */

#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "proto/rpki/rpki.h"
#include "sysdep/unix/unix.h"

static int
rpki_tr_tcp_open(struct rpki_tr_sock *tr)
{
  sock *sk = tr->sk;

  sk->type = SK_TCP_ACTIVE;

  if (sk_open(sk) != 0)
    return RPKI_TR_ERROR;

  return RPKI_TR_SUCCESS;
}

static const char *
rpki_tr_tcp_ident(struct rpki_tr_sock *tr)
{
  struct rpki_cache *cache = tr->cache;
  struct rpki_config *cf = (void *) cache->p->p.cf;

  if (tr->ident != NULL)
    return tr->ident;

  const char *host = cf->hostname;
  ip_addr ip = cf->ip;
  u16 port = cf->port;

  size_t colon_and_port_len = 6; /* max ":65535" */
  size_t ident_len;
  if (host)
    ident_len = strlen(host) + colon_and_port_len + 1;
  else
    ident_len = IPA_MAX_TEXT_LENGTH + colon_and_port_len + 1;

  char *ident = mb_alloc(cache->pool, ident_len);
  if (host)
    bsnprintf(ident, ident_len, "%s:%u", host, port);
  else
    bsnprintf(ident, ident_len, "%I:%u", ip, port);

  tr->ident = ident;
  return tr->ident;
}

/**
 * rpki_tr_tcp_init - initializes the RPKI transport structure for a TCP connection
 * @tr: allocated RPKI transport structure
 */
void
rpki_tr_tcp_init(struct rpki_tr_sock *tr)
{
  tr->open_fp = &rpki_tr_tcp_open;
  tr->ident_fp = &rpki_tr_tcp_ident;
}
