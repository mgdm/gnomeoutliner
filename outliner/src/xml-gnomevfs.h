/**
 * xml-gnomevfs.h
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

#ifndef OUTLINER_XML_GNOMEVFS_H
#define OUTLINER_XML_GNOMEVFS_H

#include <libgnomevfs/gnome-vfs.h>
#include <libxml/xmlIO.h>
#include <glib.h>

GnomeVFSHandle *xml_gnomevfs_openread(gchar *filename);
gint xml_gnomevfs_read(GnomeVFSHandle *handle, gchar *buf, gint len);
gint xml_gnomevfs_close(GnomeVFSHandle *handle);
gint xml_gnomevfs_write(GnomeVFSHandle *handle, const gchar *buf, gint len);
xmlOutputBufferPtr xml_gnomevfs_create_ouputbuffer(GnomeVFSHandle *handle);

#endif
