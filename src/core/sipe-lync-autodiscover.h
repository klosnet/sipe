/**
 * @file sipe-lync-autodiscover.h
 *
 * pidgin-sipe
 *
 * Copyright (C) 2016 SIPE Project <http://sipe.sourceforge.net/>
 *
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

/* Forward declarations */
struct sipe_core_private;

/* Lync data determined by autodiscover */
struct sipe_lync_autodiscover_data {
    const gchar *server; /* NULL when failed */
    guint        port;
};

/**
 * Lync autodiscover callback
 *
 * @param sipe_private  SIPE core private data
 * @param lync_data     Lync autodiscover data (NULL when aborted)
 * @param callback_data callback data
 */
typedef void (sipe_lync_autodiscover_callback)(struct sipe_core_private *sipe_private,
					       const struct sipe_lync_autodiscover_data *lync_data,
					       gpointer callback_data);

/**
 * Trigger Lync autodiscover
 *
 * @param sipe_private  SIPE core private data
 * @param callback      callback function
 * @param callback_data callback data
 */
void sipe_lync_autodiscover_start(struct sipe_core_private *sipe_private,
				  sipe_lync_autodiscover_callback *callback,
				  gpointer callback_data);

/**
 * Initialize Lync autodiscover data
 *
 * @param sipe_private SIPE core private data
 */
void sipe_lync_autodiscover_init(struct sipe_core_private *sipe_private);

/**
 * Free Lync autodiscover data
 *
 * @param sipe_private SIPE core private data
 */
void sipe_lync_autodiscover_free(struct sipe_core_private *sipe_private);