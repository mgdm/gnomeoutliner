/**
 * outliner-document.c
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

#include "outliner-document.h"

#ifdef HAVE_CONFIG_H
 #include <config.h>
#endif

#include <glib/gi18n.h>
#include <gtk/gtk.h>

#define OUTLINER_DOCUMENT_GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((obj), OUTLINER_TYPE_DOCUMENT, OutlinerDocumentPrivate))

typedef struct _OutlinerDocumentPrivate OutlinerDocumentPrivate;
struct _OutlinerDocumentPrivate {
  int dummy; /* XXX */
};

static GtkTreeStoreClass *parent_class = NULL;

static void outliner_document_init       (OutlinerDocument      *doc);
static void outliner_document_class_init (OutlinerDocumentClass *class);
static void outliner_document_finalize   (GObject               *object);


/*-------------*/

static void
outliner_document_init (OutlinerDocument *doc)
{
  //OutlinerDocumentPrivate *priv = OUTLINER_DOCUMENT_GET_PRIVATE (doc);

}

static void
outliner_document_class_init (OutlinerDocumentClass *class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);
  parent_class = g_type_class_peek_parent (class);

  object_class->finalize = outliner_document_finalize;

  g_type_class_add_private (class, sizeof (OutlinerDocumentPrivate));
}

OutlinerDocument *
outliner_document_new (void)
{
  OutlinerDocument *doc;

  doc = g_object_new (OUTLINER_TYPE_DOCUMENT,
		              NULL);

  GType columns[] = { G_TYPE_STRING };

  gtk_tree_store_set_column_types (GTK_TREE_STORE (doc), 1, columns);

  return doc;
}

static void
outliner_document_finalize (GObject *object)
{
  OutlinerDocumentPrivate *priv = OUTLINER_DOCUMENT_GET_PRIVATE (object);

  G_OBJECT_CLASS (parent_class)->finalize (object);
}

G_DEFINE_TYPE (OutlinerDocument, outliner_document, GTK_TYPE_TREE_STORE);
