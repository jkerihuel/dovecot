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
#include "ldap-storage.h"
#include "ldap-sync.h"
#include "index-mail.h"
#include "mail-copy.h"

int ldap_copy(struct mail_save_context *_ctx, struct mail *mail)
{
	struct mailbox_transaction_context *_t = _ctx->transaction;
	struct ldap_mailbox *mbox = (struct ldap_mailbox *)_t->box;
	int ret;

	i_assert((_t->flags & MAILBOX_TRANSACTION_FLAG_EXTERNAL) != 0);

	return mail_storage_copy(_ctx, mail);
}
