#ifndef IMAPC_CLIENT_PRIVATE_H
#define IMAPC_CLIENT_PRIVATE_H

#include "imapc-client.h"

struct imapc_client_connection {
	struct imapc_connection *conn;
	struct imapc_client_mailbox *box;
};

struct imapc_client {
	pool_t pool;
	struct imapc_client_settings set;
	struct ssl_iostream_context *ssl_ctx;

	imapc_untagged_callback_t *untagged_callback;
	void *untagged_context;

	ARRAY_DEFINE(conns, struct imapc_client_connection *);

	struct ioloop *ioloop;

	unsigned int stop_now:1;
};

struct imapc_client_mailbox {
	struct imapc_client *client;
	struct imapc_connection *conn;
	struct imapc_seqmap *seqmap;

	void *untagged_box_context;
	unsigned int pending_box_command_count;
};

#endif