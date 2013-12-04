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

struct mailbox_sync_context *
ldap_storage_sync_init(struct mailbox *box, enum mailbox_sync_flags flags)
{
	struct ldap_mailbox *mbox = (struct ldap_mailbox *)box;
	int ret = 0;

	/*TODO: Implement this function */
	LDAP_LOG("debug");

	return index_mailbox_sync_init(box, flags, ret < 0);
}
