#include "../lib/runner.h"
#include "../../src/uv_ip.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <arpa/inet.h>

/******************************************************************************
 *
 * Fixture
 *
 *****************************************************************************/

struct fixture
{
		struct addrinfo *address_infos;
};


static void *setUp(MUNIT_UNUSED const MunitParameter params[], MUNIT_UNUSED void *user_data)
{
    struct fixture *f = munit_malloc(sizeof *f);
		f->address_infos = NULL;
		return f;
}

static void tearDown(MUNIT_UNUSED void *data)
{
	struct fixture *f = data;

	if (f->address_infos) {
		  freeaddrinfo(f->address_infos);
	}
	free(f);
}

/******************************************************************************
 *
 * raft_io->recover()
 *
 *****************************************************************************/

SUITE(ip)

/* If the instance has been already initialized, an error is returned. */
/* A new configuration is saved as last entry on disk. */
TEST(ip, resolveBindLocalhost, setUp, tearDown, 0, NULL)
{
  	struct fixture *f = data;
		char address_str[INET_ADDRSTRLEN];
		const char* rp;
		int rv;

		rv = uvIpResolveBindAddresses("localhost:9000", &f->address_infos);
		munit_assert_int(rv, ==, 0);
		munit_assert_not_null(f->address_infos);
		munit_assert_null(f->address_infos->ai_next);
		munit_assert_int(f->address_infos->ai_family, ==, AF_INET);
		munit_assert_int(f->address_infos->ai_addrlen, ==, sizeof(struct sockaddr_in));

		rp = inet_ntop(AF_INET, &((struct sockaddr_in*)f->address_infos->ai_addr)->sin_addr, address_str, sizeof(address_str));
		munit_assert_not_null(rp);
		munit_assert_string_equal(rp, "127.0.0.1");
		
    return 0;
}
