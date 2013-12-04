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

#ifndef	LDAP_STORAGE_H
#define	LDAP_STORAGE_H

#include "ldap-settings.h"

#define	LDAP_STORAGE_NAME "ldap"

#include "index-storage.h"

extern struct mail_vfuncs ldap_mail_vfuncs;

struct ldap_storage {
	struct mail_storage		storage;
	const struct ldap_settings	*set;
};

struct ldap_mailbox {
	struct mailbox		box;
	struct ldap_storage	*storage;
};

#define LDAP_LOG(lvl) \
	i_debug("%s %s: <%s> %s:%d: %s", __DATE__, __TIME__, lvl, __FILE__, __LINE__, __func__);

#ifndef __BEGIN_DECLS
#ifdef __cplusplus
#define __BEGIN_DECLS		extern "C" {
#define __END_DECLS		}
#else
#define __BEGIN_DECLS
#define __END_DECLS
#endif
#endif

__BEGIN_DECLS

int ldap_copy(struct mail_save_context *, struct mail *);

__END_DECLS

#endif /*! LDAP_STORAGE_H */
