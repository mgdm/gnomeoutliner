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
  GtkCellRenderer   *text_renderer;
  GtkCellRenderer   *toggle_renderer;
  GtkTreeViewColumn *column;
  GtkTreeSelection  *sel;
};

static GtkTreeViewClass *parent_class = NULL;

static void outliner_view_init       (OutlinerView      *view);
static void outliner_view_class_init (OutlinerViewClass *class);
static void outliner_view_finalize   (GObject           *object);

OutlinerDocument *
outliner_view_get_document (OutlinerView *view)
{
  OutlinerViewPrivate *priv = OUTLINER_VIEW_GET_PRIVATE (view);
  return OUTLINER_DOCUMENT (priv->document);
}

void
outliner_view_add_item (OutlinerView *view)
{
  OutlinerDocument *doc;
  GtkTreeIter cur, new;
  GtkTreePath *path;
  GtkTreeSelection *sel;

  doc = outliner_view_get_document(view);

  if (outliner_view_get_last_selected(view, &cur)) {
    if (gtk_tree_model_iter_has_child(GTK_TREE_MODEL (doc), &cur))
      gtk_tree_store_prepend(GTK_TREE_STORE (doc), &new, &cur);
    else
      gtk_tree_store_insert_after(GTK_TREE_STORE (doc), &new, NULL, &cur);
  }
  else
    gtk_tree_store_prepend(GTK_TREE_STORE (doc), &new, NULL);

  sel = gtk_tree_view_get_selection(GTK_TREE_VIEW (view));
  gtk_tree_selection_select_iter(sel, &new);
  path = gtk_tree_model_get_path(GTK_TREE_MODEL (doc), &new);
  gtk_tree_view_expand_to_path(GTK_TREE_VIEW (view), path);
  gtk_tree_view_set_cursor_on_cell(GTK_TREE_VIEW (view), path,
      gtk_tree_view_get_column(GTK_TREE_VIEW (view), 0), NULL, TRUE);
  gtk_tree_path_free(path);

  outliner_document_set_changed(doc, TRUE);
}

void
outliner_view_foreach_selected_subtree (OutlinerView *view, OutlinerViewForeachFunc func,
                                        gboolean backwards, gpointer data)
{
  OutlinerDocument *doc;
  GtkTreePath *path, *ancestor = NULL;
  GtkTreeSelection *sel;
  GList *sellist, *reflist = NULL, *li;
  GtkTreeRowReference *rowref;

  doc = outliner_view_get_document(view);

  sel = gtk_tree_view_get_selection(GTK_TREE_VIEW (view));
  sellist = gtk_tree_selection_get_selected_rows(sel, NULL);

  for (li = sellist; li!= NULL; li = g_list_next(li)) {
    path = (GtkTreePath *)(li->data);
    if (ancestor==NULL || !gtk_tree_path_is_descendant(path, ancestor)) {
       if (backwards)
         reflist = g_list_prepend(reflist, gtk_tree_row_reference_new(GTK_TREE_MODEL (doc), path));
       else
         reflist = g_list_append(reflist, gtk_tree_row_reference_new(GTK_TREE_MODEL (doc), path));
       ancestor = path;
    }
  }
  g_list_foreach(sellist, (GFunc)gtk_tree_path_free, NULL);
  g_list_free(sellist);

  for (li = reflist; li!= NULL; li = g_list_next(li)) {
    rowref = (GtkTreeRowReference *)(li->data);
    path = gtk_tree_row_reference_get_path(rowref);
    (*func)(doc, path, data);
  }
  g_list_foreach(reflist, (GFunc)gtk_tree_row_reference_free, NULL);
  g_list_free(reflist);
}

gboolean
outliner_view_get_last_selected (OutlinerView *view, GtkTreeIter *item)
{
  OutlinerDocument *doc;
  GtkTreePath *path;
  GtkTreeSelection *sel;
  GList *sellist, *li;
  gboolean ret = FALSE;

  doc = outliner_view_get_document(view);

  sel = gtk_tree_view_get_selection(GTK_TREE_VIEW (view));
  sellist = gtk_tree_selection_get_selected_rows(sel, NULL);

  if (g_list_length(sellist) > 0) {
    li = g_list_last(sellist);
    path = (GtkTreePath *)(li->data);
    gtk_tree_model_get_iter(GTK_TREE_MODEL (doc), item, path);
    ret = TRUE;
  }

  g_list_foreach(sellist, (GFunc)gtk_tree_path_free, NULL);
  g_list_free(sellist);
  return ret;
}

static void
save_celltext(GtkCellRenderer *rend, gchar *path, gchar *string, OutlinerView *view)
{
  GtkTreeIter iter;
  OutlinerViewPrivate *priv = OUTLINER_VIEW_GET_PRIVATE (view);
  GtkTreeStore *store = GTK_TREE_STORE (priv->document);

  gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(store), &iter, path);
  gtk_tree_store_set(store, &iter, COL_TEXT, string, -1);
}

static void
save_status(GtkCellRenderer *rend, gchar *path, OutlinerView *view)
{
  GtkTreeIter iter;
  gboolean status;

  OutlinerViewPrivate *priv = OUTLINER_VIEW_GET_PRIVATE (view);
  GtkTreeStore *store = GTK_TREE_STORE (priv->document);

  gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(store), &iter, path);

  status = gtk_cell_renderer_toggle_get_active(rend);
  gtk_tree_store_set(store, &iter, COL_STATUS, !status, -1);
  gtk_cell_renderer_toggle_set_active(rend, !status);
}

static void
row_moved_cb (OutlinerDocument *doc, GtkTreePath *oldpath, GtkTreePath *newpath, gpointer data)
{
  GtkTreeView *tree;
  GtkTreeSelection *sel;
  OutlinerView *view = data;
  GtkTreeIter iter;

  tree = GTK_TREE_VIEW (view);
  sel = gtk_tree_view_get_selection(tree);

  g_message("row-moved: %s -> %s", gtk_tree_path_to_string(oldpath), gtk_tree_path_to_string(newpath));

  if (gtk_tree_selection_path_is_selected(sel, oldpath)) {
    gtk_tree_view_expand_to_path(tree, newpath);
    gtk_tree_view_collapse_row(tree, newpath);
    gtk_tree_selection_select_path(sel, newpath);
	gtk_tree_view_scroll_to_cell(tree, newpath, NULL, FALSE, 0, 0);
  }

  if (gtk_tree_view_row_expanded(tree, oldpath))
    gtk_tree_view_expand_row(tree, newpath, FALSE);

}

static void
row_deleted_cb (GtkTreeModel *model, GtkTreePath *path, gpointer data)
{
  GtkTreeView *tree;
  GtkTreeSelection *sel;
  OutlinerView *view = data;
  OutlinerViewPrivate *priv = OUTLINER_VIEW_GET_PRIVATE (view);

  tree = GTK_TREE_VIEW (view);
  sel = gtk_tree_view_get_selection(tree);

  if (gtk_tree_selection_count_selected_rows(sel) == 0) {
    //gtk_tree_selection_select_path(sel, path);
	gtk_tree_view_set_cursor(tree, path, priv->column, TRUE);
  }
}

/*
static void
expand (OutlinerView *view)
{

  gint row = 1;
  gint i;
      for(i = 0; i < expanded->len ; i++) {
        if (g_array_index(expanded, gint, i) == row)
          gtk_tree_store(
      }

}
*/

/*-------------*/

static void
outliner_view_init (OutlinerView *view)
{
  gtk_tree_view_set_reorderable (view, TRUE);
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
  GtkTreeIter root, a, b, c;

  g_return_val_if_fail (doc != NULL, NULL);

  view = OUTLINER_VIEW (g_object_new (OUTLINER_TYPE_VIEW, NULL));
  g_return_val_if_fail (view != NULL, NULL);
  tree = GTK_TREE_VIEW (view);
  
  gtk_tree_view_set_headers_visible (tree, FALSE);

  gtk_tree_view_set_model (tree, GTK_TREE_MODEL(doc));
  g_message("set model");

  priv = OUTLINER_VIEW_GET_PRIVATE (view);
  priv->document = doc; 
  g_signal_connect(G_OBJECT (priv->document), "row_moved",
                   G_CALLBACK(row_moved_cb), view);
  g_signal_connect(G_OBJECT (priv->document), "row-deleted",
                   G_CALLBACK(row_deleted_cb), view);

  priv->column = gtk_tree_view_column_new();
  priv->text_renderer = g_object_new (OUTLINER_TYPE_CELL_RENDERER_TEXT, NULL);
  priv->toggle_renderer = gtk_cell_renderer_toggle_new();
  gtk_tree_view_column_pack_end(priv->column, priv->text_renderer, TRUE);
  gtk_tree_view_column_add_attribute(priv->column, priv->text_renderer, "text", COL_TEXT);
  gtk_tree_view_column_pack_end(priv->column, priv->toggle_renderer, FALSE);
  gtk_tree_view_column_add_attribute(priv->column, priv->toggle_renderer, "active", COL_STATUS);
  gtk_tree_view_insert_column (GTK_TREE_VIEW (view), priv->column, -1);
  g_object_set(priv->text_renderer, "editable", TRUE, NULL);
  g_signal_connect(priv->text_renderer, "edited", G_CALLBACK(save_celltext), view);
  g_signal_connect(priv->toggle_renderer, "toggled", G_CALLBACK(save_status), view);
  g_message("setup column");

  priv->sel = gtk_tree_view_get_selection(tree);
  gtk_tree_selection_set_mode(priv->sel, GTK_SELECTION_MULTIPLE);

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
