/*
 * LDAP Storage Module
 *
 * Copyright (c) 2013 Julien Kerihuel <jkerihuel@zentyal.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 *
 */

#include "lib.h"
#include "mailbox-list-private.h"
#include "ldap-storage.h"
#include "ldap-sync.h"
#include "index-mail.h"

#define	LDAP_LIST_CONTEXT(obj) \
	MODULE_CONTEXT(obj, ldap_mailbox_list_module)

extern struct mail_storage ldap_storage;
extern struct mailbox ldap_mailbox;

static MODULE_CONTEXT_DEFINE_INIT(ldap_mailbox_list_module,
				  &mailbox_list_module_register);

static struct mail_storage *ldap_storage_alloc(void)
{
	struct ldap_storage *storage;
	pool_t pool;

	LDAP_LOG("debug")

	/* FIXME: Use accurate alloconly_create size */
	pool = pool_alloconly_create("ldap storage", 2048);
	storage = p_new(pool, struct ldap_storage, 1);
	storage->storage = ldap_storage;
	storage->storage.pool = pool;
	return &storage->storage;
}

static int
ldap_storage_create(struct mail_storage *_storage, 
		    struct mail_namespace *ns ATTR_UNUSED,
		    const char **error_r ATTR_UNUSED)
{
	struct ldap_storage *storage = (struct ldap_storage *)_storage;
	/* struct mailbox_list *list = ns->list; */
	
	LDAP_LOG("debug")

	storage->set = mail_storage_get_driver_settings(_storage);

	/* TODO: Connect to LDAP server */

	return 0;
}

static void ldap_storage_destroy(struct mail_storage *_storage)
{
	/* struct ldap_storage *storage = (struct ldap_storage *)_storage; */

	/* TODO: Disconnect from LDAP server */
	
	LDAP_LOG("debug")

	index_storage_destroy(_storage);
}

static void 
ldap_storage_get_list_settings(const struct mail_namespace *ns ATTR_UNUSED,
			       struct mailbox_list_settings *set)
{
	LDAP_LOG("debug")

	if (set->layout == NULL)
		set->layout = MAILBOX_LIST_NAME_LDAP;
}

static struct mailbox *
ldap_mailbox_alloc(struct mail_storage *storage, struct mailbox_list *list,
		   const char *vname, enum mailbox_flags flags)
{
	struct ldap_mailbox *mbox;
	pool_t pool;

	LDAP_LOG("debug");

	/* FIXME: use proper sized allocation */
	pool = pool_alloconly_create("ldap mailbox", 1024*4);
	mbox = p_new(pool, struct ldap_mailbox, 1);
	mbox->box = ldap_mailbox;
	mbox->box.pool = pool;
	mbox->box.storage = storage;
	mbox->box.list = list;
	mbox->box.mail_vfuncs = &ldap_mail_vfuncs;

	index_storage_mailbox_alloc(&mbox->box, vname, flags, MAIL_INDEX_PREFIX);

	mbox->storage = (struct ldap_storage *)storage;

	/* TODO: Implement the callbacks in ldap-mailbox.c */
	/* ldap_mailbox_register_callbacks(mbox); */
	return &mbox->box;
}

static int ldap_mailbox_open(struct mailbox *box)
{
	/* TODO: Implement the function */
	LDAP_LOG("debug");
	return 0;
}

static int
ldap_mailbox_update(struct mailbox *box, const struct mailbox_update *update)
{
	/* TODO: Implement the function */
	LDAP_LOG("debug");
	return 0;
}

static int 
ldap_mailbox_create(struct mailbox *box, const struct mailbox_update *update,
		    bool directory)
{
	/* TODO: Implement the function */
	LDAP_LOG("debug");
	return 0;
}



struct mail_storage ldap_storage = {
	.name = LDAP_STORAGE_NAME,
	.class_flags = MAIL_STORAGE_CLASS_FLAG_HAVE_MAIL_GUIDS |
		MAIL_STORAGE_CLASS_FLAG_HAVE_MAIL_SAVE_GUIDS |
		MAIL_STORAGE_CLASS_FLAG_BINARY_DATA,
	.v = {
		ldap_get_setting_parser_info,
		ldap_storage_alloc,
		ldap_storage_create,
		ldap_storage_destroy,
		NULL,
		ldap_storage_get_list_settings,
		NULL,
		ldap_mailbox_alloc,
		NULL
	}
};

static void ldap_notify_changes(struct mailbox *box)
{
	/* TODO: Implement the function */
	LDAP_LOG("debug");
	return;
}

struct mailbox ldap_mailbox = {
	.v = {
		index_storage_is_readonly,
		index_storage_mailbox_enable,
		index_storage_mailbox_exists,
		ldap_mailbox_open,
		index_storage_mailbox_close,
		index_storage_mailbox_free,
		ldap_mailbox_create,
		ldap_mailbox_update,
		index_storage_mailbox_delete,
		index_storage_mailbox_rename,
		index_storage_get_status,
		index_mailbox_get_metadata,
		index_storage_set_subscribed,
		index_storage_attribute_set,
		index_storage_attribute_get,
		index_storage_attribute_iter_init,
		index_storage_attribute_iter_next,
		index_storage_attribute_iter_deinit,
		index_storage_list_index_has_changed,
		index_storage_list_index_update_sync,
		ldap_storage_sync_init,
		index_mailbox_sync_next,
		index_mailbox_sync_deinit,
		NULL,
		ldap_notify_changes,
		index_transaction_begin,
		index_transaction_commit,
		index_transaction_rollback,
		NULL,
		index_mail_alloc,
		index_storage_search_init,
		index_storage_search_deinit,
		index_storage_search_next_nonblock,
		index_storage_search_next_update_seq,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		ldap_copy,
		NULL,
		NULL,
		NULL,
		index_storage_is_inconsistent
	}
};
