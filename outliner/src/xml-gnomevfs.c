/**
 * xml-gnomevfs.c
 *
 * Gnome Outliner is the legal property of its developers, whose names are
 * too numerous to list here.  Please refer to the COPYRIGHT file distributed
 * with this source distribution.
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
 *
 */

#include "xml-gnomevfs.h"

/*
	Here are GnomeVFS wrappers for libXML IO layer
*/

/* open for reading wrapper */
GnomeVFSHandle *xml_gnomevfs_openread(gchar *filename) {
	g_message("gnomevfs-openread");
	GnomeVFSHandle *handle;
	if (gnome_vfs_open(&handle, filename, GNOME_VFS_OPEN_READ) == GNOME_VFS_OK)
		return handle;
	else
		return NULL;
};

/* read wrapper */
gint xml_gnomevfs_read(GnomeVFSHandle *handle, gchar *buf, gint len) {
	g_message("gnomevfs-read");
	GnomeVFSFileSize bread;
	if (gnome_vfs_read(handle, buf, len, &bread) == GNOME_VFS_OK)
		return (int)bread;
    else
		return -1;
};

/* close wrapper */
gint xml_gnomevfs_close(GnomeVFSHandle *handle) {
	g_message("gnomevfs-close");
	if (gnome_vfs_close(handle) == GNOME_VFS_OK)
		return 0;
	else
		return -1;
};

/* write wrapper */
gint xml_gnomevfs_write(GnomeVFSHandle *handle, const gchar *buf, gint len) {
	g_message("gnomevfs-write");
	GnomeVFSFileSize written;
	if (gnome_vfs_write(handle, buf, len, &written) == GNOME_VFS_OK)
		return (gint)written;
	else
		return -1;
}

/* This function creates an output buffer for LibXML */
xmlOutputBufferPtr xml_gnomevfs_create_ouputbuffer(GnomeVFSHandle *handle) {
	xmlOutputBufferPtr ret;

	xmlRegisterDefaultOutputCallbacks();

	if (handle == NULL) return NULL;
	if ((ret = xmlAllocOutputBuffer(xmlGetCharEncodingHandler(XML_CHAR_ENCODING_UTF8))) != NULL) {
		ret->context = handle;
		ret->writecallback = (xmlOutputWriteCallback)xml_gnomevfs_write;
		ret->closecallback = (xmlOutputCloseCallback)xml_gnomevfs_close;
		g_message("GNOMEVFS output buffer ready");
	}

	return ret;
}
