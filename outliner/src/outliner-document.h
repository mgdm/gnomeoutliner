/**
 * outliner-document.h
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

#ifndef OUTLINER_DOCUMENT_H
#define OUTLINER_DOCUMENT_H

#include <gtk/gtktreestore.h>

#define OUTLINER_TYPE_DOCUMENT    (outliner_document_get_type ())
#define OUTLINER_DOCUMENT(obj)    (G_TYPE_CHECK_INSTANCE_CAST ((obj), OUTLINER_TYPE_DOCUMENT, OutlinerDocument))
#define OUTLINER_IS_DOCUMENT(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), OUTLINER_TYPE_DOCUMENT))

typedef struct _OutlinerDocument      OutlinerDocument;
typedef struct _OutlinerDocumentClass OutlinerDocumentClass;

struct _OutlinerDocument {
  GtkTreeStore parent;
};

struct _OutlinerDocumentClass {
  GtkTreeStoreClass parent_class;

  void (*row_moved) (OutlinerDocument *doc, GtkTreePath *oldpath, GtkTreePath *newpath);
};

enum {
  COL_TEXT = 0,
  COL_STATUS,
  COL_OTHER,
  NUM_COLS
};


GType             outliner_document_get_type (void);
OutlinerDocument *outliner_document_new      (void);

void outliner_document_indent      (OutlinerDocument *doc, GtkTreePath *path, gpointer data);
void outliner_document_unindent    (OutlinerDocument *doc, GtkTreePath *path, gpointer data);
void outliner_document_move_down   (OutlinerDocument *doc, GtkTreePath *path, gpointer data);
void outliner_document_move_up     (OutlinerDocument *doc, GtkTreePath *path, gpointer data);
void outliner_document_delete_item (OutlinerDocument *doc, GtkTreePath *path, gpointer data);

/*Accesor methods for document's attributes */
const GString* outliner_document_get_title        (OutlinerDocument *doc);
const GString* outliner_document_get_author       (OutlinerDocument *doc);
const GString* outliner_document_get_email        (OutlinerDocument *doc);
const GString* outliner_document_get_uri          (OutlinerDocument *doc);
gboolean       outliner_document_get_changed      (OutlinerDocument *doc);
gint           outliner_document_get_w_top        (OutlinerDocument *doc); 
gint           outliner_document_get_w_left       (OutlinerDocument *doc); 
gint           outliner_document_get_w_right      (OutlinerDocument *doc); 
gint           outliner_document_get_w_bottom     (OutlinerDocument *doc); 
const GArray*  outliner_document_get_expanded     (OutlinerDocument *doc); 
const GSList*  outliner_document_get_column_names (OutlinerDocument *doc);

/*Modifier methods for document's attributes */
void outliner_document_set_title        (OutlinerDocument *doc, gchar *title);
void outliner_document_set_author       (OutlinerDocument *doc, gchar *author);
void outliner_document_set_email        (OutlinerDocument *doc, gchar *email);
void outliner_document_set_uri          (OutlinerDocument *doc, gchar *uri);
void outliner_document_set_changed      (OutlinerDocument *doc, gboolean changed);
void outliner_document_set_w_top        (OutlinerDocument *doc, gint w_top); 
void outliner_document_set_w_left       (OutlinerDocument *doc, gint w_left); 
void outliner_document_set_w_right      (OutlinerDocument *doc, gint w_right); 
void outliner_document_set_w_bottom     (OutlinerDocument *doc, gint w_bottom); 
void outliner_document_set_expanded     (OutlinerDocument *doc, GArray *expanded); 
void outliner_document_set_column_names (OutlinerDocument *doc, GSList *column_names); 

#endif
