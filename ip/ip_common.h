/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _IP_COMMON_H_
#define _IP_COMMON_H_

#include <stdbool.h>
#include <sys/shm.h>
#include <sys/ipc.h>

#include "json_print.h"

#define COMMAND_NAME "ip"

struct link_filter {
	int ifindex;
	int family;
	int oneline;
	int showqueue;
	inet_prefix pfx;
	int scope, scopemask;
	int flags, flagmask;
	int up;
	char *label;
	int flushed;
	char *flushb;
	int flushp;
	int flushe;
	int group;
	int master;
	char *kind;
	char *slave_kind;
	int target_nsid;
};

struct nic_info{
	int if_index[1024];
	char if_name[1024][20];
	int if_number[1024];
};

struct nic_info nic_info;

int get_operstate(const char *name);//
int print_linkinfo(struct nlmsghdr *n, void *arg);//
int print_addrinfo(struct nlmsghdr *n, void *arg);//
void ipaddr_reset_filter(int oneline, int ifindex);

void netns_nsid_socket_init(void);//
int coll_name(char **argv);//
int do_netns(int argc, char **argv);//
int back_netns(int argc, char **argv);//
int get_vnic(void);

void vrf_reset(void);

int ip_link_list(req_filter_fn_t filter_fn, struct nlmsg_chain *linfo);//
void free_nlmsg_chain(struct nlmsg_chain *info);

extern struct rtnl_handle rth;

struct iplink_req {
	struct nlmsghdr		n;
	struct ifinfomsg	i;
	char			buf[1024];
};

int iplink_parse(int argc, char **argv, struct iplink_req *req, char **type);

#ifndef	INFINITY_LIFE_TIME
#define     INFINITY_LIFE_TIME      0xFFFFFFFFU
#endif

#ifndef LABEL_MAX_MASK
#define     LABEL_MAX_MASK          0xFFFFFU
#endif

int set_iflist(struct nlmsghdr *n, void *arg, char *index, char *name, int *number);
void make_iflist();
void search_name(int number);

#define DEFAULT_KEY "back_to_default_nns"
#define ANOTHER_KEY "go_to_another_nns"

#endif /* _IP_COMMON_H_ */
