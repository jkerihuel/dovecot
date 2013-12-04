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

#ifndef	LDAP_SETTINGS_H
#define	LDAP_SETTINGS_H

struct ldap_settings {
	const char *ldap_host;
	unsigned int ldap_port;

	const char *ldap_user;
	const char *ldap_password;
};

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

const struct setting_parser_info *ldap_get_setting_parser_info(void);

__END_DECLS


#endif /* !LDAP_SETTINGS_H */
