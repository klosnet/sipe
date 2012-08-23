/**
 * @file telepathy-main.c
 *
 * pidgin-sipe
 *
 * Copyright (C) 2012 SIPE Project <http://sipe.sourceforge.net/>
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <glib-object.h>
#include <telepathy-glib/base-connection-manager.h>
#include <telepathy-glib/run.h>

#include "sipe-backend.h"
#include "telepathy-private.h"

G_BEGIN_DECLS
/*
 * Connection manager type - data structures
 */
typedef struct _SipeConnectionManagerClass {
	TpBaseConnectionManagerClass parent_class;
} SipeConnectionManagerClass;

typedef struct _SipeConnectionManager {
	TpBaseConnectionManager parent;
} SipeConnectionManager;

/*
 * Connection manager type - type macros
 */
static GType sipe_connection_manager_get_type(void) G_GNUC_CONST;
#define SIPE_TYPE_CONNECTION_MANAGER \
	(sipe_connection_manager_get_type())
#define SIPE_CONNECTION_MANAGER(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST((obj), SIPE_TYPE_CONNECTION_MANAGER, \
				    SipeConnectionManager))
#define SIPE_CONNECTION_MANAGER_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST((klass), SIPE_TYPE_CONNECTION_MANAGER, \
				 SipeConnectionManagerClass))
#define SIPE_IS_CONNECTION_MANAGER(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE((obj), SIPE_TYPE_CONNECTION_MANAGER))
#define SIPE_IS_CONNECTION_MANAGER_CLASS(klass)\
	(G_TYPE_CHECK_CLASS_TYPE((klass), SIPE_TYPE_CONNECTION_MANAGER))
#define SIPE_CONNECTION_MANAGER_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS ((obj), SIPE_TYPE_CONNECTION_MANAGER, \
				    SipeConnectionManagerClass))
G_END_DECLS


/*
 * Connection manager type - implementation
 */
G_DEFINE_TYPE(SipeConnectionManager,
	      sipe_connection_manager,
	      TP_TYPE_BASE_CONNECTION_MANAGER)

static void sipe_connection_manager_constructed(GObject *object)
{
	SipeConnectionManager *self   = SIPE_CONNECTION_MANAGER(object);
	TpBaseConnectionManager *base = (TpBaseConnectionManager *) self;

	/* always chain up to the parent constructor first */
	G_OBJECT_CLASS(sipe_connection_manager_parent_class)->constructed(object);

	sipe_telepathy_protocol_init(base);
}

static void sipe_connection_manager_class_init(SipeConnectionManagerClass *klass)
{
	GObjectClass *object_class               = G_OBJECT_CLASS(klass);
	TpBaseConnectionManagerClass *base_class = (TpBaseConnectionManagerClass *)klass;

	object_class->constructed   = sipe_connection_manager_constructed;

	base_class->new_connection  = NULL;
	base_class->cm_dbus_name    = SIPE_TELEPATHY_DOMAIN;
	base_class->protocol_params = NULL;
}

static void sipe_connection_manager_init(SipeConnectionManager *self)
{
	(void)self;
}


/*
 * Entry point
 */
static TpBaseConnectionManager *construct_cm(void)
{
	return((TpBaseConnectionManager *)
	       g_object_new(SIPE_TYPE_CONNECTION_MANAGER, NULL));
}

int main(int argc, char *argv[])
{
	int rc;

	g_type_init();
	sipe_telepathy_debug_init();

	sipe_backend_debug(SIPE_DEBUG_LEVEL_INFO,
			   "initializing - version %s",
			   PACKAGE_VERSION);

	rc = tp_run_connection_manager(SIPE_TELEPATHY_DOMAIN,
				       PACKAGE_VERSION,
				       construct_cm,
				       argc,
				       argv);

	sipe_telepathy_debug_finalize();
	return(rc);
}

/*
  Local Variables:
  mode: c
  c-file-style: "bsd"
  indent-tabs-mode: t
  tab-width: 8
  End:
*/
