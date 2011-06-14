/* Copyright (c) 2010-2011 Dovecot authors, see the included COPYING file */

#include "lib.h"
#include "mail-namespace.h"
#include "mail-storage.h"
#include "mail-search-build.h"
#include "doveadm-mail.h"

enum cache_mask {
	CACHE_HDR		= 0x01,
	CACHE_BODY		= 0x02,
	CACHE_RECEIVED_DATE	= 0x04,
	CACHE_SAVE_DATE		= 0x08,
	CACHE_VIRTUAL_SIZE	= 0x10,
	CACHE_PHYSICAL_SIZE	= 0x20,
	CACHE_POP3_UIDL		= 0x40,
	CACHE_GUID		= 0x80
};

static bool fts_is_enabled = FALSE;

static enum cache_mask
cache_fields_get(const struct mailbox_metadata *metadata)
{
	const char *const *cache_fields;
	unsigned int i, count;
	enum cache_mask cache = 0;

	cache_fields = array_get(metadata->cache_fields, &count);
	for (i = 0; i < count; i++) {
		if (strncmp(cache_fields[i], "hdr.", 4) == 0 ||
		    strcmp(cache_fields[i], "date.sent") == 0 ||
		    strcmp(cache_fields[i], "imap.envelope") == 0)
			cache |= CACHE_HDR;
		else if (strcmp(cache_fields[i], "mime.parts") == 0 ||
			 strcmp(cache_fields[i], "imap.body") == 0 ||
			 strcmp(cache_fields[i], "imap.bodystructure") == 0)
			cache |= CACHE_BODY;
		else if (strcmp(cache_fields[i], "date.received") == 0)
			cache |= CACHE_RECEIVED_DATE;
		else if (strcmp(cache_fields[i], "date.save") == 0)
			cache |= CACHE_SAVE_DATE;
		else if (strcmp(cache_fields[i], "size.virtual") == 0)
			cache |= CACHE_VIRTUAL_SIZE;
		else if (strcmp(cache_fields[i], "size.physical") == 0)
			cache |= CACHE_PHYSICAL_SIZE;
		else if (strcmp(cache_fields[i], "pop3.uidl") == 0)
			cache |= CACHE_POP3_UIDL;
		else if (strcmp(cache_fields[i], "guid") == 0)
			cache |= CACHE_GUID;
		else if (doveadm_debug) {
			i_debug("Ignoring unknown cache field: %s",
				cache_fields[i]);
		}
	}
	return cache;
}

static int cache_add(struct mailbox *box, enum cache_mask cache)
{
	struct mailbox_status status;
	struct mailbox_transaction_context *trans;
	struct mail *mail;
	uint32_t seq;
	time_t date;
	uoff_t size;
	const char *str;

	if (doveadm_debug) {
		i_debug("%s: Nothing in mailbox cache, skipping",
			mailbox_get_vname(box));
		return 0;
	}

	/* find the first message we need to index */
	mailbox_get_open_status(box, STATUS_MESSAGES, &status);
	trans = mailbox_transaction_begin(box, 0);
	mail = mail_alloc(trans, 0, NULL);
	for (seq = status.messages; seq > 0; seq--) {
		mail_set_seq(mail, seq);
		if (mail_is_cached(mail))
			break;
	}
	seq++;

	if (doveadm_debug) {
		if (seq > status.messages) {
			i_debug("%s: Cache is already up to date",
				mailbox_get_vname(box));
		} else {
			i_debug("%s: Caching mails seq=%u..%u cache=0x%x",
				mailbox_get_vname(box),
				seq, status.messages, cache);
		}
	}

	for (; seq <= status.messages; seq++) {
		mail_set_seq(mail, seq);

		if ((cache & (CACHE_HDR | CACHE_BODY)) != 0)
			mail_parse(mail, (cache & CACHE_BODY) != 0);
		if ((cache & CACHE_RECEIVED_DATE) != 0)
			(void)mail_get_received_date(mail, &date);
		if ((cache & CACHE_SAVE_DATE) != 0)
			(void)mail_get_save_date(mail, &date);
		if ((cache & CACHE_VIRTUAL_SIZE) != 0)
			(void)mail_get_virtual_size(mail, &size);
		if ((cache & CACHE_PHYSICAL_SIZE) != 0)
			(void)mail_get_physical_size(mail, &size);
		if ((cache & CACHE_POP3_UIDL) != 0) {
			(void)mail_get_special(mail, MAIL_FETCH_UIDL_BACKEND,
					       &str);
		}
		if ((cache & CACHE_GUID) != 0)
			(void)mail_get_special(mail, MAIL_FETCH_GUID, &str);
	}
	mail_free(&mail);
	if (mailbox_transaction_commit(&trans) < 0) {
		i_error("Commiting mailbox %s failed: %s",
			mailbox_get_vname(box),
			mail_storage_get_last_error(mailbox_get_storage(box), NULL));
		return -1;
	}
	return 0;
}

static int fts_update(struct mailbox *box)
{
	struct mailbox_status status;
	struct mailbox_transaction_context *t;
	struct mail_search_args *search_args;
	struct mail_search_arg *arg;
	struct mail_search_context *ctx;
	struct mail *mail;
	int ret;

	if (!fts_is_enabled)
		return 0;

	mailbox_get_open_status(box, STATUS_MESSAGES, &status);

	/* a bit kludgy way to trigger the full text search update:
	   search for a string in the last message */
	t = mailbox_transaction_begin(box, 0);
	search_args = mail_search_build_init();
	mail_search_build_add_seqset(search_args,
				     status.messages, status.messages);
	arg = mail_search_build_add(search_args, SEARCH_BODY_FAST);
	arg->value.str = "xyzzy";

	ctx = mailbox_search_init(t, search_args, NULL, 0, NULL);
	mail_search_args_unref(&search_args);

	while (mailbox_search_next(ctx, &mail)) {
	}

	ret = mailbox_search_deinit(&ctx);
	if (mailbox_transaction_commit(&t) < 0)
		ret = -1;
	return ret;
}

static int
cmd_index_box(const struct mailbox_info *info)
{
	struct mailbox *box;
	struct mailbox_metadata metadata;
	enum cache_mask cache;
	int ret = 0;

	box = mailbox_alloc(info->ns->list, info->name,
			    MAILBOX_FLAG_KEEP_RECENT |
			    MAILBOX_FLAG_IGNORE_ACLS);

	if (mailbox_sync(box, MAILBOX_SYNC_FLAG_FULL_READ) < 0) {
		i_error("Syncing mailbox %s failed: %s", info->name,
			mail_storage_get_last_error(mailbox_get_storage(box), NULL));
		mailbox_free(&box);
		return -1;
	}
	if (mailbox_get_metadata(box, MAILBOX_METADATA_CACHE_FIELDS,
				 &metadata) < 0) {
		i_error("Metadata lookup from mailbox %s failed: %s", info->name,
			mail_storage_get_last_error(mailbox_get_storage(box), NULL));
		mailbox_free(&box);
		return -1;
	}

	cache = cache_fields_get(&metadata);
	ret = cache_add(box, cache);

	if (fts_update(box) < 0)
		ret = -1;

	mailbox_free(&box);
	return ret;
}

static void
cmd_index_run(struct doveadm_mail_cmd_context *ctx, struct mail_user *user)
{
	const enum mailbox_list_iter_flags iter_flags =
		MAILBOX_LIST_ITER_RAW_LIST |
		MAILBOX_LIST_ITER_NO_AUTO_BOXES |
		MAILBOX_LIST_ITER_RETURN_NO_FLAGS |
		MAILBOX_LIST_ITER_STAR_WITHIN_NS;
	const enum namespace_type ns_mask =
		NAMESPACE_PRIVATE | NAMESPACE_SHARED | NAMESPACE_PUBLIC;
	struct mailbox_list_iterate_context *iter;
	const struct mailbox_info *info;

	if (mail_user_plugin_getenv(user, "fts") != NULL) T_BEGIN {
		const char *const *plugins;

		plugins = t_strsplit(user->set->mail_plugins, " ");
		for (; *plugins != NULL; plugins++) {
			if (strncmp(*plugins, "fts", 3) == 0)
				fts_is_enabled = TRUE;
		}
	} T_END;

	iter = mailbox_list_iter_init_namespaces(user->namespaces, ctx->args,
						 ns_mask, iter_flags);
	while ((info = mailbox_list_iter_next(iter)) != NULL) {
		if ((info->flags & (MAILBOX_NOSELECT |
				    MAILBOX_NONEXISTENT)) == 0) T_BEGIN {
			(void)cmd_index_box(info);
		} T_END;
	}
	if (mailbox_list_iter_deinit(&iter) < 0)
		i_error("Listing mailboxes failed");
}

static void cmd_index_init(struct doveadm_mail_cmd_context *ctx ATTR_UNUSED,
			   const char *const args[])
{
	if (args[0] == NULL)
		doveadm_mail_help_name("index");
}

static struct doveadm_mail_cmd_context *cmd_index_alloc(void)
{
	struct doveadm_mail_cmd_context *ctx;

	ctx = doveadm_mail_cmd_alloc(struct doveadm_mail_cmd_context);
	ctx->v.init = cmd_index_init;
	ctx->v.run = cmd_index_run;
	return ctx;
}

struct doveadm_mail_cmd cmd_index = {
	cmd_index_alloc, "index", "<mailbox>"
};
