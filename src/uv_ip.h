/* IP-related utils. */

#ifndef UV_IP_H_
#define UV_IP_H_

#include <netinet/in.h>

/* Split @address into @host and @service. */
int uvIpAddrSplit(const char *address,
                  char *host,
                  size_t host_size,
                  char *service,
                  size_t service_size);

struct addrinfo;

/* Synchronoues resolve hostname to IP address */
int uvIpResolveBindAddresses(const char *address, struct addrinfo **ai_result);

/* Split @address into @host and @port and populate @addr accordingly. */
int uvIpParse(const char *address, struct sockaddr_in *addr);

#endif /* UV_IP_H */
