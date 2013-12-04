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
#include "settings-parser.h"
#include "mail-storage-settings.h"
#include "ldap-settings.h"

#include <stddef.h>

#undef DEF
#define	DEF(type, name) \
	{ type, #name, offsetof(struct ldap_settings, name), NULL }

static bool ldap_settings_check(void *_set, pool_t pool, const char **error_r);

static const struct setting_define ldap_setting_defines[] = {
	DEF(SET_STR, ldap_host),
	DEF(SET_UINT, ldap_port),

	DEF(SET_STR_VARS, ldap_user),
	DEF(SET_STR, ldap_password),
	
	SETTING_DEFINE_LIST_END
};

static const struct ldap_settings ldap_default_settings = {
	.ldap_host = "localhost",
	.ldap_port = 389,

	.ldap_user = "",
	.ldap_password = ""
};

static const struct setting_parser_info ldap_setting_parser_info = {
	.module_name = "ldap",
	.defines = ldap_setting_defines,
	.defaults = &ldap_default_settings,

	.type_offset = (size_t)-1,
	.struct_size = sizeof(struct ldap_settings),
	
	.parent_offset = (size_t)-1,
	.parent = &mail_user_setting_parser_info,

	.check_func = ldap_settings_check
};

const struct setting_parser_info *ldap_get_setting_parser_info(void)
{
	return &ldap_setting_parser_info;
}

/* <settings checks> */
static bool ldap_settings_check(void *_set, pool_t pool ATTR_UNUSED,
				const char **error_r)
{
	struct ldap_settings *set = _set;

	if (set->ldap_port == 0 || set->ldap_port > 65535) {
		*error_r = "invalid ldap port";
		return FALSE;
	}

	return TRUE;
}
