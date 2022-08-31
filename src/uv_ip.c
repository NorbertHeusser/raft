#include <stdlib.h>
#include <string.h>

#include <uv.h>

#include "../include/raft.h"

#include "uv_ip.h"

static const char *strCpyUntil(char *target,
                               const char *source,
                               size_t target_size,
                               char separator)
{
    size_t i;
    for (i = 0; i < target_size; ++i) {
        if (!source[i] || source[i] == separator) {
            target[i] = 0;
            return source + i;
        } else {
            target[i] = source[i];
        }
    }
    return NULL;
}

int uvIpAddrSplit(const char *address,
                  char *host,
                  size_t host_size,
                  char *service,
                  size_t service_size)
{
    char colon = ':';
    const char *service_ptr = NULL;

    if (host) {
        service_ptr = strCpyUntil(host, address, host_size, colon);
        if (!service_ptr) {
            return RAFT_NAMETOOLONG;
        }
    }
    if (service) {
        if (!service_ptr) {
            service_ptr = strchr(address, colon);
        }
        if (!service_ptr || *service_ptr == 0 || *(++service_ptr) == 0) {
            service_ptr = "8080";
        }
        if (!strCpyUntil(service, service_ptr, service_size, 0)) {
            return RAFT_NAMETOOLONG;
        }
    }
    return 0;
}

/* Synchronoues resolve hostname to IP address */
int uvIpResolveBindAddresses(const char *address, struct addrinfo **ai_result)
{
	  static struct addrinfo hints = {
			.ai_flags = AI_V4MAPPED | AI_ADDRCONFIG | AI_PASSIVE,
			.ai_family = AF_INET,
			.ai_socktype = SOCK_STREAM,
			.ai_protocol = 0};
    char hostname[NI_MAXHOST];
    char service[NI_MAXSERV];
	  int rv;

		rv = uvIpAddrSplit( address, hostname, sizeof(hostname), service, sizeof(service));
		if (rv != 0) {
			return rv;
		}

		rv = getaddrinfo( hostname, service, &hints, ai_result);
		if (rv != 0) {
			return RAFT_NOCONNECTION;
		}

		return 0;
}


int uvIpParse(const char *address, struct sockaddr_in *addr)
{
    char buf[256];
    size_t n;
    char *host;
    char *port;
    char *colon = ":";
    int rv;

    /* TODO: turn this poor man parsing into proper one */
    n = sizeof(buf) - 1;
    strncpy(buf, address, n);
    buf[n] = '\0';
    host = strtok(buf, colon);
    port = strtok(NULL, ":");
    if (port == NULL) {
        port = "8080";
    }

    rv = uv_ip4_addr(host, atoi(port), addr);
    if (rv != 0) {
        return RAFT_NOCONNECTION;
    }

    return 0;
}
