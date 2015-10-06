/**
 * @file sipe-ft-lync.c
 *
 * pidgin-sipe
 *
 * Copyright (C) 2014-2015 SIPE Project <http://sipe.sourceforge.net/>
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
#include <stdlib.h>
#include <string.h>

#include "sip-transport.h"
#include "sipe-backend.h"
#include "sipe-common.h"
#include "sipe-core.h"
#include "sipe-core-private.h"
#include "sipe-ft-lync.h"
#include "sipe-media.h"
#include "sipe-mime.h"
#include "sipe-utils.h"
#include "sipe-xml.h"
#include "sipmsg.h"

struct sipe_file_transfer_lync {
	struct sipe_file_transfer public;

	gchar *sdp;
	gchar *file_name;
	gchar *id;
	gsize file_size;
	guint request_id;

	struct sipe_media_call *call;
};
#define SIPE_FILE_TRANSFER         ((struct sipe_file_transfer *) ft_private)
#define SIPE_FILE_TRANSFER_PRIVATE ((struct sipe_file_transfer_lync *) ft)

static void
sipe_file_transfer_lync_free(struct sipe_file_transfer_lync *ft_private)
{
	g_free(ft_private->file_name);
	g_free(ft_private->sdp);
	g_free(ft_private->id);
	g_free(ft_private);
}

static void
send_ms_filetransfer_msg(char *body, struct sipe_file_transfer_lync *ft_private,
			 TransCallback callback)
{
	sip_transport_info(sipe_media_get_sipe_core_private(ft_private->call),
			   "Content-Type: application/ms-filetransfer+xml\r\n",
			   body,
			   sipe_media_get_sip_dialog(ft_private->call),
			   callback);

	g_free(body);
}

static void
send_ms_filetransfer_response(struct sipe_file_transfer_lync *ft_private,
			      const gchar *code, const gchar *reason,
			      TransCallback callback)
{
	static const gchar *RESPONSE_STR =
			"<response xmlns=\"http://schemas.microsoft.com/rtc/2009/05/filetransfer\" requestId=\"%d\" code=\"%s\" %s%s%s/>";

	send_ms_filetransfer_msg(g_strdup_printf(RESPONSE_STR,
						 ft_private->request_id, code,
						 reason ? "reason=\"" : "",
						 reason ? reason : "",
						 reason ? "\"" : ""),
				 ft_private, callback);
}

static void
mime_mixed_cb(gpointer user_data, const GSList *fields, const gchar *body,
	      gsize length)
{
	struct sipe_file_transfer_lync *ft_private = user_data;
	const gchar *ctype = sipe_utils_nameval_find(fields, "Content-Type");

	/* Lync 2010 file transfer */
	if (g_str_has_prefix(ctype, "application/ms-filetransfer+xml")) {
		sipe_xml *xml = sipe_xml_parse(body, length);
		const sipe_xml *node;

		const gchar *request_id_str = sipe_xml_attribute(xml, "requestId");
		if (request_id_str) {
			ft_private->request_id = atoi(request_id_str);
		}

		node = sipe_xml_child(xml, "publishFile/fileInfo/name");
		if (node) {
			ft_private->file_name = sipe_xml_data(node);
		}

		node = sipe_xml_child(xml, "publishFile/fileInfo/id");
		if (node) {
			ft_private->id = sipe_xml_data(node);
		}

		node = sipe_xml_child(xml, "publishFile/fileInfo/size");
		if (node) {
			gchar *size_str = sipe_xml_data(node);
			if (size_str) {
				ft_private->file_size = atoi(size_str);
				g_free(size_str);
			}
		}
	} else if (g_str_has_prefix(ctype, "application/sdp")) {
		ft_private->sdp = g_strndup(body, length);
	}
}

static void
candidate_pair_established_cb(SIPE_UNUSED_PARAMETER struct sipe_media_call *call,
			      struct sipe_media_stream *stream)
{
	struct sipe_file_transfer_lync *ft_private;
	static const gchar *DOWNLOAD_FILE_REQUEST =
		"<request xmlns=\"http://schemas.microsoft.com/rtc/2009/05/filetransfer\" requestId=\"%d\">"
			"<downloadFile>"
				"<fileInfo>"
					"<id>%s</id>"
					"<name>%s</name>"
				"</fileInfo>"
			"</downloadFile>"
		"</request>";

	g_return_if_fail(sipe_strequal(stream->id, "data"));

	ft_private = sipe_media_stream_get_data(stream);

	send_ms_filetransfer_response(ft_private, "success", NULL, NULL);

	send_ms_filetransfer_msg(g_strdup_printf(DOWNLOAD_FILE_REQUEST,
						 ++ft_private->request_id,
						 ft_private->id,
						 ft_private->file_name),
				 ft_private, NULL);
}

static void
ft_lync_incoming_init(struct sipe_file_transfer *ft,
		      SIPE_UNUSED_PARAMETER const gchar *filename,
		      SIPE_UNUSED_PARAMETER gsize size,
		      SIPE_UNUSED_PARAMETER const gchar *who)
{
	struct sipe_media_call *call = SIPE_FILE_TRANSFER_PRIVATE->call;

	if (call) {
		sipe_backend_media_accept(call->backend_private, TRUE);
	}
}

static void
ft_lync_deallocate(struct sipe_file_transfer *ft)
{
	struct sipe_media_call *call = SIPE_FILE_TRANSFER_PRIVATE->call;

	if (call) {
		sipe_backend_media_hangup(call->backend_private, TRUE);
	}
	sipe_file_transfer_lync_free(SIPE_FILE_TRANSFER_PRIVATE);
}

void
process_incoming_invite_ft_lync(struct sipe_core_private *sipe_private,
				struct sipmsg *msg)
{
	struct sipe_file_transfer_lync *ft_private;
	struct sipe_media_call *call;
	struct sipe_media_stream *stream;

	ft_private = g_new0(struct sipe_file_transfer_lync, 1);
	sipe_mime_parts_foreach(sipmsg_find_header(msg, "Content-Type"),
				msg->body, mime_mixed_cb, ft_private);

	if (!ft_private->file_name || !ft_private->file_size || !ft_private->sdp) {
		sip_transport_response(sipe_private, msg, 488, "Not Acceptable Here", NULL);
		sipe_file_transfer_lync_free(ft_private);
		return;
	}

	/* Replace multipart message body with the selected SDP part and
	 * initialize media session as if invited to a media call. */
	g_free(msg->body);
	msg->body = ft_private->sdp;
	msg->bodylen = strlen(msg->body);
	ft_private->sdp = NULL;

	ft_private->call = process_incoming_invite_call(sipe_private, msg);
	if (!ft_private->call) {
		sip_transport_response(sipe_private, msg, 500, "Server Internal Error", NULL);
		sipe_file_transfer_lync_free(ft_private);
		return;
	}

	call = ft_private->call;
	call->candidate_pair_established_cb = candidate_pair_established_cb;

	ft_private->public.ft_init = ft_lync_incoming_init;
	ft_private->public.ft_deallocate = ft_lync_deallocate;

	stream = sipe_core_media_get_stream_by_id(call, "data");
	sipe_media_stream_add_extra_attribute(stream, "recvonly", NULL);
	sipe_media_stream_set_data(stream, ft_private, NULL);

	sipe_backend_ft_incoming(SIPE_CORE_PUBLIC, SIPE_FILE_TRANSFER,
				 call->with, ft_private->file_name,
				 ft_private->file_size);
}

/*
  Local Variables:
  mode: c
  c-file-style: "bsd"
  indent-tabs-mode: t
  tab-width: 8
  End:
*/