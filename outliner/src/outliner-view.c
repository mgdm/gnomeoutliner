/**
 * outliner-view.c
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

#include "outliner-view.h"
#include "outliner-document.h"
#include "outliner-cell-renderer.h"

#ifdef HAVE_CONFIG_H
 #include <config.h>
#endif

#include <glib/gi18n.h>
#include <gtk/gtk.h>

#define OUTLINER_VIEW_GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((obj), OUTLINER_TYPE_VIEW, OutlinerViewPrivate))

typedef struct _OutlinerViewPrivate OutlinerViewPrivate;
struct _OutlinerViewPrivate {
  OutlinerDocument  *document;
  GtkCellRenderer   *renderer;
  GtkTreeViewColumn *column;
  GtkTreeSelection  *sel;
};

static GtkTreeViewClass *parent_class = NULL;

static void outliner_view_init       (OutlinerView      *view);
static void outliner_view_class_init (OutlinerViewClass *class);
static void outliner_view_finalize   (GObject           *object);

void
outliner_view_add_item (OutlinerView *view, gboolean child)
{
  OutlinerViewPrivate *priv = OUTLINER_VIEW_GET_PRIVATE (view);
  GtkTreeView *tree = GTK_TREE_VIEW (view);
  GtkTreeStore *store = GTK_TREE_STORE (priv->document);

  GtkTreeIter iter, parent, sub, isroot;
  GtkTreePath *path;

  /* If there's not any iters in tree - create a new and select it */
  if (!gtk_tree_model_get_iter_first(GTK_TREE_MODEL(store), &isroot)) {
    gtk_tree_store_append(store, &iter, NULL);
  } else {
    if (child) {	/* Creating child of selected iter */
      gtk_tree_selection_get_selected(priv->sel, NULL, &parent);
      gtk_tree_store_append(store, &iter, &parent);
      path = gtk_tree_model_get_path(GTK_TREE_MODEL(store), &parent);
      gtk_tree_view_expand_all(tree);//_row(tree, path, TRUE);
    } else {
      /* We creating an iter under selected iter with the same parent,
         but we need to check if selecetd item has any parent... */
      gtk_tree_selection_get_selected(priv->sel, NULL, &sub);
      path = gtk_tree_model_get_path(GTK_TREE_MODEL(store), &sub);
      if (gtk_tree_path_get_depth(path) > 1) {
        gtk_tree_path_up(path);
        gtk_tree_model_get_iter(GTK_TREE_MODEL(store), &parent, path);
        gtk_tree_store_insert_after(store, &iter, &parent, &sub);
      } else
        gtk_tree_store_insert_after(store, &iter, NULL, &sub);
    }
  }
  gtk_tree_selection_select_iter(priv->sel, &iter);
  path = gtk_tree_model_get_path(GTK_TREE_MODEL(store), &iter);
  gtk_tree_view_set_cursor_on_cell(tree, path, gtk_tree_view_get_column(tree, 0), NULL, TRUE);
}

OutlinerDocument *
outliner_view_get_document (OutlinerView *view) {
  OutlinerViewPrivate *priv = OUTLINER_VIEW_GET_PRIVATE (view);
  return OUTLINER_DOCUMENT (priv->document);
}

void save_celltext(GtkCellRenderer *rend, gchar *path, gchar *string, OutlinerView *view) {
  GtkTreeIter iter;
  OutlinerViewPrivate *priv = OUTLINER_VIEW_GET_PRIVATE (view);
  GtkTreeStore *store = GTK_TREE_STORE (priv->document);

  gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(store), &iter, path);
  gtk_tree_store_set(store, &iter, 0, string, -1);

}


/*-------------*/

static void
outliner_view_init (OutlinerView *view)
{

}

static void
outliner_view_class_init (OutlinerViewClass *class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);
  parent_class = g_type_class_peek_parent (class);

  object_class->finalize = outliner_view_finalize;

  g_type_class_add_private (class, sizeof (OutlinerViewPrivate));
}

GtkWidget *
outliner_view_new (OutlinerDocument *doc)
{
  OutlinerView *view;
  GtkTreeView *tree;
  OutlinerViewPrivate *priv;

  g_return_val_if_fail (doc != NULL, NULL);

  view = OUTLINER_VIEW (g_object_new (OUTLINER_TYPE_VIEW, NULL));
  g_return_val_if_fail (view != NULL, NULL);
  tree = GTK_TREE_VIEW (view);
  
  gtk_tree_view_set_headers_visible (tree, FALSE);

  gtk_tree_view_set_model (tree, GTK_TREE_MODEL(doc));
  g_message("set model");

  priv = OUTLINER_VIEW_GET_PRIVATE (view);
  priv->document = doc;

  priv->column = gtk_tree_view_column_new();
  gtk_tree_view_append_column(tree, priv->column);

  priv->renderer = g_object_new (OUTLINER_TYPE_CELL_RENDERER_TEXT, NULL);
  gtk_tree_view_column_pack_start(priv->column, priv->renderer, TRUE);
  gtk_tree_view_column_add_attribute(priv->column, priv->renderer, "text", 0);
  g_object_set(priv->renderer, "editable", TRUE, NULL);
  g_signal_connect(priv->renderer, "edited", G_CALLBACK(save_celltext), view);
  g_message("setup column");

  priv->sel = gtk_tree_view_get_selection(tree);
  gtk_tree_selection_set_mode(priv->sel, GTK_SELECTION_BROWSE);

  return GTK_WIDGET (view);
}

static void
outliner_view_finalize (GObject *object)
{
  OutlinerViewPrivate *priv = OUTLINER_VIEW_GET_PRIVATE (object);

  /*
  g_object_unref (priv->action_group);
  g_object_unref (priv->merge);
  */

  G_OBJECT_CLASS (parent_class)->finalize (object);
}

G_DEFINE_TYPE (OutlinerView, outliner_view, GTK_TYPE_TREE_VIEW);
