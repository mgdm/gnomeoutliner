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
  gchar *uri;
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
  gint i;

  if (newsibling)
    gtk_tree_store_insert_after(store, &newiter, newparent, newsibling);
  else
    gtk_tree_store_append(store, &newiter, newparent);

  gtk_tree_model_get(GTK_TREE_MODEL (store), olditer, 0, &string, -1);
  gtk_tree_store_set(store, &newiter, 0, string, -1);

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

  doc->changed=TRUE;
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

  doc->changed=TRUE;
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

  doc->changed=TRUE;
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

  doc->changed=TRUE;
}

void
outliner_document_delete_item (OutlinerDocument *doc, GtkTreePath *path, gpointer data)
{
  GtkTreeIter cur;

  GtkTreeModel *model = GTK_TREE_MODEL (doc);
  GtkTreeStore *store = GTK_TREE_STORE (doc);

  gtk_tree_model_get_iter(model, &cur, path);
  gtk_tree_store_remove(store, &cur);

  doc->changed=TRUE;
}


gchar* 
outliner_document_get_uri     (OutlinerDocument *doc)
{
  OutlinerDocumentPrivate *priv = OUTLINER_DOCUMENT_GET_PRIVATE (doc);
  return priv->uri;
}

void   
outliner_document_set_uri     (OutlinerDocument *doc, const gchar *new_uri)
{
  OutlinerDocumentPrivate *priv = OUTLINER_DOCUMENT_GET_PRIVATE (doc);
  if (priv->uri != NULL)
    g_free(priv->uri);
  priv->uri = g_strdup(new_uri);

}




/*-------------*/

static void
outliner_document_init (OutlinerDocument *doc)
{
  OutlinerDocumentPrivate *priv = OUTLINER_DOCUMENT_GET_PRIVATE (doc);
  priv->uri = NULL;
  doc->changed = FALSE;
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

  GType columns[] = { G_TYPE_STRING };

  gtk_tree_store_set_column_types (GTK_TREE_STORE (doc), 1, columns);

  return doc;
}

static void
outliner_document_finalize (GObject *object)
{
  OutlinerDocumentPrivate *priv = OUTLINER_DOCUMENT_GET_PRIVATE (object);
  if (priv->uri != NULL)
    g_free(priv->uri);
  G_OBJECT_CLASS (parent_class)->finalize (object);
}



G_DEFINE_TYPE (OutlinerDocument, outliner_document, GTK_TYPE_TREE_STORE);
