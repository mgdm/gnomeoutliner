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
#include "outliner-marshal.h"

#ifdef HAVE_CONFIG_H
 #include <config.h>
#endif

#include <glib/gi18n.h>
#include <gtk/gtk.h>

#define OUTLINER_DOCUMENT_GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((obj), OUTLINER_TYPE_DOCUMENT, OutlinerDocumentPrivate))

typedef struct _OutlinerDocumentPrivate OutlinerDocumentPrivate;
struct _OutlinerDocumentPrivate {
  gboolean changed;

  gint w_top, w_left, w_right, w_bottom;

  GArray  *expanded;
  GSList  *column_names;
  GString *title;
  GString *uri;
  GString *author;
  GString *email;
};

static GtkTreeStoreClass *parent_class = NULL;

static void outliner_document_init       (OutlinerDocument      *doc);
static void outliner_document_class_init (OutlinerDocumentClass *class);
static void outliner_document_finalize   (GObject               *object);

enum {
  ROW_MOVED,
  LAST_SIGNAL
};


static guint outliner_document_signals[LAST_SIGNAL] = { 0 }; 

static void
copy_subtree (GtkTreeStore *store, GtkTreeIter *olditer, GtkTreeIter *newparent, GtkTreeIter *newsibling)
{
  GtkTreeIter newiter, child;
  GtkTreePath *oldpath, *newpath;
  gchar *string;
  gboolean *status;
  GHashTable *attr_hash;
  gint i;

  if (newsibling)
    gtk_tree_store_insert_after(store, &newiter, newparent, newsibling);
  else
    gtk_tree_store_append(store, &newiter, newparent);

  gtk_tree_model_get(GTK_TREE_MODEL (store), olditer, COL_TEXT, &string, -1);
  gtk_tree_store_set(store, &newiter, COL_TEXT, string, -1);
  gtk_tree_model_get(GTK_TREE_MODEL (store), olditer, COL_STATUS, &status, -1);
  gtk_tree_store_set(store, &newiter, COL_STATUS, status, -1);
  gtk_tree_model_get(GTK_TREE_MODEL (store), olditer, COL_OTHER, &attr_hash, -1);
  gtk_tree_store_set(store, &newiter, COL_OTHER, attr_hash, -1);

  for (i = gtk_tree_model_iter_n_children(GTK_TREE_MODEL(store), olditer); i > 0; i--) {
    gtk_tree_model_iter_nth_child(GTK_TREE_MODEL(store), &child, olditer, i-1);
    copy_subtree (store, &child, &newiter, NULL);
  }

  oldpath = gtk_tree_model_get_path(GTK_TREE_MODEL (store), olditer);
  newpath = gtk_tree_model_get_path(GTK_TREE_MODEL (store), &newiter);
  g_signal_emit(G_OBJECT (store), outliner_document_signals[ROW_MOVED], 0, oldpath, newpath);
  gtk_tree_path_free(oldpath);
  gtk_tree_path_free(newpath);

}

static void
remove_subtree (GtkTreeStore *store, GtkTreeIter *olditer)
{
  GtkTreeIter child;

  while (gtk_tree_model_iter_children(GTK_TREE_MODEL (store), &child, olditer))
    remove_subtree (store, &child);

  gtk_tree_store_remove(store, olditer);
}

static void
move_subtree (GtkTreeStore *store, GtkTreeIter *olditer, GtkTreeIter *newparent, GtkTreeIter *newsibling)
{
  copy_subtree (store, olditer, newparent, newsibling);
  remove_subtree (store, olditer);
}

void
outliner_document_indent (OutlinerDocument *doc, GtkTreePath *path, gpointer data)
{
  GtkTreeIter cur, prev, new;


  GtkTreeModel *model = GTK_TREE_MODEL (doc);
  GtkTreeStore *store = GTK_TREE_STORE (doc);

  gtk_tree_model_get_iter(model, &cur, path);
  if (!gtk_tree_path_prev(path))
    return;
  gtk_tree_model_get_iter(model, &prev, path);

  move_subtree (store, &cur, &prev, NULL);

  outliner_document_set_changed(doc, TRUE);
}

void
outliner_document_unindent (OutlinerDocument *doc, GtkTreePath *path, gpointer data)
{
  GtkTreeIter cur, parent, grandparent, new;

  GtkTreeModel *model = GTK_TREE_MODEL (doc);
  GtkTreeStore *store = GTK_TREE_STORE (doc);

  gtk_tree_model_get_iter(model, &cur, path);
  if (!gtk_tree_model_iter_parent(model, &parent, &cur))
    return;

  if (gtk_tree_model_iter_parent(model, &grandparent, &parent))
    move_subtree (store, &cur, &grandparent, &parent);
  else
    move_subtree (store, &cur, NULL, &parent);

outliner_document_set_changed(doc, TRUE);
}

void
outliner_document_move_up (OutlinerDocument *doc, GtkTreePath *path, gpointer data)
{
  GtkTreeIter cur, prev;

  GtkTreeModel *model = GTK_TREE_MODEL (doc);
  GtkTreeStore *store = GTK_TREE_STORE (doc);

  gtk_tree_model_get_iter(model, &cur, path);
  if (!gtk_tree_path_prev(path))
    return;
  gtk_tree_model_get_iter(model, &prev, path);

  /* this will fail on top-level items: waiting on gnome bug #139785 */
  gtk_tree_store_swap(store, &cur, &prev);

  outliner_document_set_changed(doc, TRUE);
}

void
outliner_document_move_down (OutlinerDocument *doc, GtkTreePath *path, gpointer data)
{
  GtkTreeIter cur, next;

  GtkTreeModel *model = GTK_TREE_MODEL (doc);
  GtkTreeStore *store = GTK_TREE_STORE (doc);

  gtk_tree_model_get_iter(model, &cur, path);
  next = cur;
  gtk_tree_model_iter_next(model, &next);

  /* this will fail on top-level items: waiting on gnome bug #139785 */
  gtk_tree_store_swap(store, &cur, &next);

  outliner_document_set_changed(doc, TRUE);
}

void
outliner_document_delete_item (OutlinerDocument *doc, GtkTreePath *path, gpointer data)
{
  GtkTreeIter cur;

  GtkTreeModel *model = GTK_TREE_MODEL (doc);
  GtkTreeStore *store = GTK_TREE_STORE (doc);

  gtk_tree_model_get_iter(model, &cur, path);
  gtk_tree_store_remove(store, &cur);

  outliner_document_set_changed(doc, TRUE);
}


/* Accesor methods  */

const GString*
outliner_document_get_title   (OutlinerDocument *doc)
{
  OutlinerDocumentPrivate *priv = OUTLINER_DOCUMENT_GET_PRIVATE(doc);
  return priv->title;
}

const GString*
outliner_document_get_author   (OutlinerDocument *doc)
{
  OutlinerDocumentPrivate *priv = OUTLINER_DOCUMENT_GET_PRIVATE(doc);
  return priv->author;
}

const GString*
outliner_document_get_email   (OutlinerDocument *doc)
{
  OutlinerDocumentPrivate *priv = OUTLINER_DOCUMENT_GET_PRIVATE(doc);
  return priv->email;
}

const GString*
outliner_document_get_uri   (OutlinerDocument *doc)
{
  OutlinerDocumentPrivate *priv = OUTLINER_DOCUMENT_GET_PRIVATE(doc);
  return priv->uri;
}

gboolean
outliner_document_get_changed  (OutlinerDocument *doc)
{
  OutlinerDocumentPrivate *priv = OUTLINER_DOCUMENT_GET_PRIVATE(doc);
  return priv->changed;
}

gint 
outliner_document_get_w_top   (OutlinerDocument *doc)
{
  OutlinerDocumentPrivate *priv = OUTLINER_DOCUMENT_GET_PRIVATE(doc);
  return priv->w_top;
} 

gint
outliner_document_get_w_left  (OutlinerDocument *doc)
{
  OutlinerDocumentPrivate *priv = OUTLINER_DOCUMENT_GET_PRIVATE(doc);
  return priv->w_left;
} 

gint
outliner_document_get_w_right (OutlinerDocument *doc)
{
  OutlinerDocumentPrivate *priv = OUTLINER_DOCUMENT_GET_PRIVATE(doc);
  return priv->w_right;
} 

gint 
outliner_document_get_w_bottom   (OutlinerDocument *doc)
{
  OutlinerDocumentPrivate *priv = OUTLINER_DOCUMENT_GET_PRIVATE(doc);
  return priv->w_bottom;
} 

const GArray*  
outliner_document_get_expanded(OutlinerDocument *doc)
{
  OutlinerDocumentPrivate *priv = OUTLINER_DOCUMENT_GET_PRIVATE(doc);
  return priv->expanded;
} 

const GSList*  
outliner_document_get_column_names (OutlinerDocument *doc)
{
  OutlinerDocumentPrivate *priv = OUTLINER_DOCUMENT_GET_PRIVATE(doc);
  return priv->column_names;
} 



/* Modifier  methods*/

void
outliner_document_set_title   (OutlinerDocument *doc, gchar *title)
{
  OutlinerDocumentPrivate *priv = OUTLINER_DOCUMENT_GET_PRIVATE(doc);
  GString *new_title = g_string_new(title);

  g_string_free(priv->title, TRUE);
  priv->title = new_title;
  priv->changed = TRUE;
}

void
outliner_document_set_author   (OutlinerDocument *doc, gchar *author)
{
  OutlinerDocumentPrivate *priv = OUTLINER_DOCUMENT_GET_PRIVATE(doc);
  GString *new_author = g_string_new(author);
  g_string_free(priv->author, TRUE);
  priv->author = new_author;
  priv->changed = TRUE;
}

void
outliner_document_set_email   (OutlinerDocument *doc, gchar *email)
{
  OutlinerDocumentPrivate *priv = OUTLINER_DOCUMENT_GET_PRIVATE(doc);
  GString *new_email = g_string_new(email);
  g_string_free(priv->email, TRUE);
  priv->email = new_email;
  priv->changed = TRUE;
}

void
outliner_document_set_uri   (OutlinerDocument *doc, gchar *uri)
{
  OutlinerDocumentPrivate *priv = OUTLINER_DOCUMENT_GET_PRIVATE(doc);
  GString *new_uri = g_string_new(uri);
  g_string_free(priv->uri, TRUE);
  priv->uri = new_uri;
}

void outliner_document_set_expanded(OutlinerDocument *doc, GArray *expanded)
{
  OutlinerDocumentPrivate *priv = OUTLINER_DOCUMENT_GET_PRIVATE(doc);
  GArray *new_expanded = g_array_sized_new(FALSE, FALSE, sizeof(gint),expanded->len);
  g_array_insert_vals (new_expanded, 0, expanded->data, expanded->len);
  g_array_free(priv->expanded, TRUE); 
  priv->expanded = new_expanded;
}

void
outliner_document_set_changed  (OutlinerDocument *doc, gboolean changed)
{
  OutlinerDocumentPrivate *priv = OUTLINER_DOCUMENT_GET_PRIVATE(doc);
  priv->changed = changed;
}

void
outliner_document_set_w_top   (OutlinerDocument *doc, gint w_top)
{
  OutlinerDocumentPrivate *priv = OUTLINER_DOCUMENT_GET_PRIVATE(doc);
  priv->w_top = w_top;
}

void 
outliner_document_set_w_left  (OutlinerDocument *doc, gint w_left)
{
  OutlinerDocumentPrivate *priv = OUTLINER_DOCUMENT_GET_PRIVATE(doc);
  priv->w_left = w_left;
}

void 
outliner_document_set_w_right (OutlinerDocument *doc, gint w_right)
{
  OutlinerDocumentPrivate *priv = OUTLINER_DOCUMENT_GET_PRIVATE(doc);
  priv->w_right = w_right;
}

void
outliner_document_set_w_bottom(OutlinerDocument *doc, gint w_bottom)
{
  OutlinerDocumentPrivate *priv = OUTLINER_DOCUMENT_GET_PRIVATE(doc);
  priv->w_bottom = w_bottom;
}

void outliner_document_set_column_names(OutlinerDocument *doc, GSList *column_names)
{
  OutlinerDocumentPrivate *priv = OUTLINER_DOCUMENT_GET_PRIVATE(doc);
  GSList* new_column_names = g_slist_copy(column_names);
  g_slist_free(priv->column_names);
  priv->column_names = new_column_names;
}

/*-------------*/

static void
outliner_document_init (OutlinerDocument *doc)
{
  OutlinerDocumentPrivate *priv = OUTLINER_DOCUMENT_GET_PRIVATE(doc);
  priv->column_names = NULL;
  priv->title = g_string_new("Untitled");
  priv->author = g_string_new(NULL);
  priv->email = g_string_new(NULL);
  priv->uri = g_string_new(NULL);
  priv->changed = FALSE;
}

static void
outliner_document_class_init (OutlinerDocumentClass *class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);
  parent_class = g_type_class_peek_parent (class);

  object_class->finalize = outliner_document_finalize;

  outliner_document_signals[ROW_MOVED] =
    g_signal_new ("row_moved",
                  OUTLINER_TYPE_DOCUMENT,
                  G_SIGNAL_RUN_FIRST,
                  G_STRUCT_OFFSET (OutlinerDocumentClass, row_moved),
                  NULL, NULL,
                  outliner_marshal_VOID__POINTER_POINTER,
                  G_TYPE_NONE, 2,
                  G_TYPE_POINTER, G_TYPE_POINTER);

  g_type_class_add_private (class, sizeof (OutlinerDocumentPrivate));
}

OutlinerDocument *
outliner_document_new (void)
{
  OutlinerDocument *doc;

  doc = g_object_new (OUTLINER_TYPE_DOCUMENT,
		              NULL);

  GType columns[] = { G_TYPE_STRING, G_TYPE_BOOLEAN, G_TYPE_POINTER };

  gtk_tree_store_set_column_types (GTK_TREE_STORE (doc), NUM_COLS, columns);

  return doc;
}

static void
outliner_document_finalize (GObject *object)
{
  OutlinerDocumentPrivate *priv = OUTLINER_DOCUMENT_GET_PRIVATE (object);

  G_OBJECT_CLASS (parent_class)->finalize (object);
}

G_DEFINE_TYPE (OutlinerDocument, outliner_document, GTK_TYPE_TREE_STORE);
