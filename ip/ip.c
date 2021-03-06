/*
 * ip.c		"ip" utility frontend.
 *
 *		This program is free software; you can redistribute it and/or
 *		modify it under the terms of the GNU General Public License
 *		as published by the Free Software Foundation; either version
 *		2 of the License, or (at your option) any later version.
 *
 * Authors:	Alexey Kuznetsov, <kuznet@ms2.inr.ac.ru>
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>

#include "utils.h"
#include "ip_common.h"

int preferred_family = AF_UNSPEC;
int human_readable;
int use_iec;
int show_stats;
int show_details;
int oneline;
int brief;
int json;
int timestamp;
int force;
int max_flush_loops = 10;
int batch_mode;
bool do_all;

struct rtnl_handle rth = { .fd = -1 };

static void usage(void) __attribute__((noreturn));

static void usage(void)
{
	fprintf(stderr,
		"Usage: ip [ OPTIONS ] OBJECT { COMMAND | help }\n"
		"       ip [ -force ] -batch filename\n"
		"where  OBJECT := { link | address | addrlabel | route | rule | neigh | ntable |\n"
		"                   tunnel | tuntap | maddress | mroute | mrule | monitor | xfrm |\n"
		"                   netns | l2tp | fou | macsec | tcp_metrics | token | netconf | ila |\n"
		"                   vrf | sr | nexthop | mptcp }\n"
		"       OPTIONS := { -V[ersion] | -s[tatistics] | -d[etails] | -r[esolve] |\n"
		"                    -h[uman-readable] | -iec | -j[son] | -p[retty] |\n"
		"                    -f[amily] { inet | inet6 | mpls | bridge | link } |\n"
		"                    -4 | -6 | -I | -D | -M | -B | -0 |\n"
		"                    -l[oops] { maximum-addr-flush-attempts } | -br[ief] |\n"
		"                    -o[neline] | -t[imestamp] | -ts[hort] | -b[atch] [filename] |\n"
		"                    -rc[vbuf] [size] | -n[etns] name | -N[umeric] | -a[ll] |\n"
		"                    -c[olor]}\n");
	exit(-1);
}

static int do_help(int argc, char **argv)
{
	usage();
	return 0;
}

static const struct cmd {
	const char *cmd;
	int (*func)(int argc, char **argv);
} cmds[] = {
	{ "address",	coll_name },
	{ "netns",	do_netns },
	{ "help",	do_help },
	{ 0 }
};

static int do_cmd(const char *argv0, int argc, char **argv, bool final)
{
	const struct cmd *c;

	for (c = cmds; c->cmd; ++c) {
		if (matches(argv0, c->cmd) == 0)
			return -(c->func(argc-1, argv+1));
	}

	return EXIT_FAILURE;
}


int main(int argc, char **argv)
{
	char *basename;
	int color = 0;
	int id;
	char *adr;


	drop_cap();

	basename = strrchr(argv[0], '/');
	if (basename == NULL)
		basename = argv[0];
	else
		basename++;

	_SL_ = oneline ? "\\" : "\n";

	check_enable_color(color, json);

	if (rtnl_open(&rth, 0) < 0)
		exit(1);

	rtnl_set_strict_dump(&rth);
	printf("do exec\n");
	if(argc==2&&!strcmp(argv[2],ANOTHER_KEY)){
		char *new_argv[4];
		*new_argv[0]=*argv[1];
		*new_argv[1]=COMMAND_NAME;
		*new_argv[2]=ANOTHER_KEY;
		do_netns(3, new_argv);
	}
	else if(argc==2&&strcmp(argv[2],ANOTHER_KEY)) get_vnic();
	else if(argc==4&&strcmp(argv[3],DEFAULT_KEY)) coll_name(argv);
	else printf("No command\n");

	do_cmd(argv[1], argc-1, argv+1, true);

	return 0;
	rtnl_close(&rth);
	usage();
}
