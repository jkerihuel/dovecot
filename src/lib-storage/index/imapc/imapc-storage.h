#ifndef IMAPC_STORAGE_H
#define IMAPC_STORAGE_H

#include "index-storage.h"

#define IMAPC_STORAGE_NAME "imapc"
#define IMAPC_INDEX_PREFIX "dovecot.index"

struct imap_arg;
struct imapc_untagged_reply;
struct imapc_command_reply;
struct imapc_mailbox;
struct imapc_storage;

typedef void imapc_storage_callback_t(const struct imapc_untagged_reply *reply,
				      struct imapc_storage *storage);
typedef void imapc_mailbox_callback_t(const struct imapc_untagged_reply *reply,
				      struct imapc_mailbox *mbox);

struct imapc_storage_event_callback {
	const char *name;
	imapc_storage_callback_t *callback;
};

struct imapc_mailbox_event_callback {
	const char *name;
	imapc_mailbox_callback_t *callback;
};

struct imapc_storage {
	struct mail_storage storage;
	const struct imapc_settings *set;

	struct imapc_mailbox_list *list;
	struct imapc_client *client;

	struct imapc_mailbox *cur_status_box;
	struct mailbox_status *cur_status;

	ARRAY_DEFINE(untagged_callbacks, struct imapc_storage_event_callback);
};

struct imapc_mailbox {
	struct mailbox box;
	struct imapc_storage *storage;
	/* primary client connection to mailbox. handles untagged replies. */
	struct imapc_client_mailbox *client_box;
	/* alternative EXAMINEd client connections to mailbox. */
	ARRAY_DEFINE(alt_client_boxes, struct imapc_client_mailbox *);

	struct mail_index_transaction *delayed_sync_trans;
	struct mail_index_view *sync_view, *delayed_sync_view;
	struct timeout *to_idle_check, *to_idle_delay;

	struct mail *cur_fetch_mail;

	ARRAY_DEFINE(untagged_callbacks, struct imapc_mailbox_event_callback);
	ARRAY_DEFINE(resp_text_callbacks, struct imapc_mailbox_event_callback);

	unsigned int opening:1;
	unsigned int new_msgs:1;
};

struct imapc_simple_context {
	struct imapc_storage *storage;
	int ret;
};

struct mail_save_context *
imapc_save_alloc(struct mailbox_transaction_context *_t);
int imapc_save_begin(struct mail_save_context *ctx, struct istream *input);
int imapc_save_continue(struct mail_save_context *ctx);
int imapc_save_finish(struct mail_save_context *ctx);
void imapc_save_cancel(struct mail_save_context *ctx);
int imapc_copy(struct mail_save_context *ctx, struct mail *mail);

int imapc_transaction_save_commit_pre(struct mail_save_context *ctx);
void imapc_transaction_save_commit_post(struct mail_save_context *ctx,
					struct mail_index_transaction_commit_result *result);
void imapc_transaction_save_rollback(struct mail_save_context *ctx);

struct mail_search_context *
imapc_search_init(struct mailbox_transaction_context *t,
		  struct mail_search_args *args,
		  const enum mail_sort_type *sort_program);
int imapc_search_deinit(struct mail_search_context *_ctx);
bool imapc_search_next_nonblock(struct mail_search_context *_ctx,
				struct mail *mail, bool *tryagain_r);
bool imapc_search_next_update_seq(struct mail_search_context *_ctx);
void imapc_fetch_mail_update(struct mail *mail,
			     const struct imapc_untagged_reply *reply,
			     const struct imap_arg *args);

void imapc_copy_error_from_reply(struct imapc_storage *storage,
				 enum mail_error default_error,
				 const struct imapc_command_reply *reply);
void imapc_simple_context_init(struct imapc_simple_context *sctx,
			       struct imapc_storage *storage);
void imapc_simple_run(struct imapc_simple_context *sctx);
void imapc_simple_callback(const struct imapc_command_reply *reply,
			   void *context);
void imapc_async_stop_callback(const struct imapc_command_reply *reply,
			       void *context);
int imapc_mailbox_get_client_box(struct imapc_mailbox *mbox,
				 struct imapc_client_mailbox **client_box_r);
int imapc_mailbox_commit_delayed_trans(struct imapc_mailbox *mbox,
				       bool *changes_r);

void imapc_storage_register_untagged(struct imapc_storage *storage,
				     const char *name,
				     imapc_storage_callback_t *callback);
void imapc_mailbox_register_untagged(struct imapc_mailbox *mbox,
				     const char *name,
				     imapc_mailbox_callback_t *callback);
void imapc_mailbox_register_resp_text(struct imapc_mailbox *mbox,
				      const char *key,
				      imapc_mailbox_callback_t *callback);

void imapc_mailbox_register_callbacks(struct imapc_mailbox *mbox);

#endif