/**
 * @file purple-dbus-bindings.c
 *
 * pidgin-sipe
 *
 * Copyright (C) 2017 SIPE Project <http://sipe.sourceforge.net/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <glib.h>

#include "purple-dbus.h"

/*
 * The contents of this file need to be updated when any line which starts
 * with DBUS_EXPORT in purple-private.h gets added/removed/changed.
 *
 * You'll need access to the Pidgin source code to update this file:
 *
 *    $ python <path to pidgin>/libpurple/dbus-analyze-functions.py \
 *             --export-only                                        \
 *             src/purple/purple-dbus.h                             \
 *             >> src/purple/purple-dbus-bindings.c
 *
 * You'll have to edit the contents manually after running the above command.
 */

/*
 * The generated xxx_DBUS() functions need to be copied here
 */
static DBusMessage*
sipe_join_conference_uri_DBUS(DBusMessage *message_DBUS, DBusError *error_DBUS) {
	DBusMessage *reply_DBUS;
	dbus_int32_t account_ID;
	PurpleAccount *account;
	const char *uri;
	dbus_message_get_args(message_DBUS, error_DBUS, DBUS_TYPE_INT32, &account_ID, DBUS_TYPE_STRING, &uri, DBUS_TYPE_INVALID);
	CHECK_ERROR(error_DBUS);
	PURPLE_DBUS_ID_TO_POINTER(account, account_ID, PurpleAccount, error_DBUS);
	uri = (uri && uri[0]) ? uri : NULL;
	sipe_join_conference_uri(account, uri);
	reply_DBUS = dbus_message_new_method_return (message_DBUS);
	dbus_message_append_args(reply_DBUS, DBUS_TYPE_INVALID);
	return reply_DBUS;
}

/*
 * The contents of bindings_DBUS[] need to be copied here
 */
PurpleDBusBinding sipe_purple_dbus_bindings[] = {
	{"SipeJoinConferenceUri", "in\0i\0account\0in\0s\0uri\0", sipe_join_conference_uri_DBUS},
	{NULL, NULL, NULL}
};

/*
  Local Variables:
  mode: c
  c-file-style: "bsd"
  indent-tabs-mode: t
  tab-width: 8
  End:
*/