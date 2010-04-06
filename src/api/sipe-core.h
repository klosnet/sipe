/**
 * @file sipe-core.h
 *
 * pidgin-sipe
 *
 * Copyright (C) 2010 SIPE Project <http://sipe.sourceforge.net/>
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

/**
 * Opaque data type for backend private data.
 * The backend is responsible to allocate and free it.
 */
struct sipe_backend_private;

/**
 * Public part of the Sipe data structure
 *
 * This part contains the information needed by the core and the backend.
 */
struct sipe_core_public {
	/**
	 * This points to the private data for the backend.
	 * The backend is responsible to allocate and free it.
	 */ 
	struct sipe_backend_private *backend_private;
};

/**
 * Initialize & destroy functions for the SIPE core
 * Should be called on loading and unloading of the plugin.
 */
void sipe_core_init(void);
void sipe_core_destroy(void);

/** Utility functions exported by the core to backends ***********************/
gboolean sipe_strequal(const gchar *left, const gchar *right);

/*****************************************************************************/

/**
 * Other functions (need to be sorted once structure becomes clear.
 */

/* Get translated about string. Must be g_free'd(). */
gchar *sipe_core_about(void);

/* menu actions */
void sipe_core_update_calendar(struct sipe_core_public *sipe_public);
void sipe_core_reset_status(struct sipe_core_public *sipe_public);

/* buddy actions */
/**
 * Get status text for buddy.
 *
 * @param sipe_public Sipe core public data structure.
 * @param name        backend-specific buddy name.
 * @param status_id   backend-specific buddy status ID.
 * @param status_text backend-specific buddy status text for ID.
 *
 * @return HTML status text for the buddy or NULL. Must be g_free()'d.
 */
gchar *sipe_core_buddy_status(struct sipe_core_public *sipe_public,
			      const gchar *name,
			      const gchar *status_id,
			      const gchar *status_name);

/**
 * Return a list with buddy information label/text pairs
 *
 * @param sipe_public Sipe core public data structure.
 * @param name        backend-specific buddy name.
 * @param status_text backend-specific buddy status text for ID.
 * @param is_online   backend considers buddy to be online.
 *
 * @return GSList of struct sipe_buddy_info or NULL. Must be freed by caller.
 */
struct sipe_buddy_info {    /* must be g_free()'d */
	const gchar *label;
	gchar *text;        /* must be g_free()'d */
};
GSList *sipe_core_buddy_info(struct sipe_core_public *sipe_public,
			     const gchar *name,
			     const gchar *status_name,
			     gboolean is_online);

void sipe_core_contact_allow_deny(struct sipe_core_public *sipe_public,
				  const gchar *who, gboolean allow);
void sipe_core_group_set_user(struct sipe_core_public *sipe_public,
			      const gchar * who);

/**
 * Create a new chat
 */
void sipe_core_chat_create(struct sipe_core_public *sipe_public, int id,
			   const char *name);

/*
  Local Variables:
  mode: c
  c-file-style: "bsd"
  indent-tabs-mode: t
  tab-width: 8
  End:
*/
