/**
 * outliner-actions.c
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

#include "outliner-window.h"
#include "outliner-fprops-dialog.h"
#include "outliner-view.h"
#include "outliner-document.h"
#include "outliner-file.h"

#ifdef HAVE_CONFIG_H
 #include <config.h>
#endif

#include <libgnomeui/gnome-about.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>

void
outliner_action_dummy (GtkAction      *action,
                       OutlinerWindow *window)
{
  const gchar *name =  gtk_action_get_name(action);
  g_message(name);
}


/* file actions */
void
outliner_action_new (GtkAction *action, OutlinerWindow *window)
{
  OutlinerView *view;
  OutlinerDocument *doc;

  view = outliner_window_get_view(window);
  doc = outliner_view_get_document(view);

  outliner_file_save_changed(window, doc);

  gtk_tree_store_clear(GTK_TREE_STORE (doc));
  outliner_view_add_item(view);
}

void
outliner_action_open (GtkAction *action, OutlinerWindow *window)
{
  OutlinerView *view;
  OutlinerDocument *doc;

  view = outliner_window_get_view(window);
  doc = outliner_view_get_document(view);

  outliner_file_save_changed(window, doc);

  outliner_file_open(window, doc);
}

void
outliner_action_open_location (GtkAction *action, OutlinerWindow *window)
{
  OutlinerView *view;
  OutlinerDocument *doc;

  view = outliner_window_get_view(window);
  doc = outliner_view_get_document(view);

  outliner_file_save_changed(window, doc);
  /* TODO: open doc from location */
}

void
outliner_action_save (GtkAction *action, OutlinerWindow *window)
{
  OutlinerView *view;
  OutlinerDocument *doc;

  view = outliner_window_get_view(window);
  doc = outliner_view_get_document(view);

  outliner_file_save(window, doc);
}

void
outliner_action_save_as (GtkAction *action, OutlinerWindow *window)
{
  OutlinerView *view;
  OutlinerDocument *doc;

  view = outliner_window_get_view(window);
  doc = outliner_view_get_document(view);

  outliner_file_save_as(window, doc);
}

void
outliner_action_export (GtkAction *action, OutlinerWindow *window)
{
  OutlinerView *view;
  OutlinerDocument *doc;

  view = outliner_window_get_view(window);
  doc = outliner_view_get_document(view);

  /* TODO: export the doc */
}

void
outliner_action_properties (GtkAction *action, OutlinerWindow *window)
{
  OutlinerView *view;
  OutlinerDocument *doc;
  GtkWidget *dialog;

  view = outliner_window_get_view(window);
  doc = outliner_view_get_document(view);

  outliner_fprops_dialog_new(doc);
}

void
outliner_action_quit (GtkAction *action, OutlinerWindow *window)
{
  OutlinerView *view;
  OutlinerDocument *doc;

  view = outliner_window_get_view(window);
  doc = outliner_view_get_document(view); 
  outliner_file_save_changed(window, doc);
  if (doc->changed)
    g_printf("doch changed\n");
  else
    g_printf("doch NOT changed\n");
  gtk_main_quit();

}


/* outline actions */
void
outliner_action_add_item (GtkAction *action, OutlinerWindow *window)
{
  OutlinerView *view;
  view = outliner_window_get_view(window);
  outliner_view_add_item(view);
}

void
outliner_action_indent (GtkAction *action, OutlinerWindow *window)
{
  OutlinerView *view;
  view = outliner_window_get_view(window);
  outliner_view_foreach_selected_subtree(view, outliner_document_indent, FALSE, NULL);
}

void
outliner_action_unindent (GtkAction *action, OutlinerWindow *window)
{
  OutlinerView *view;
  view = outliner_window_get_view(window);
  outliner_view_foreach_selected_subtree(view, outliner_document_unindent, TRUE, NULL);
}

void
outliner_action_move_up (GtkAction *action, OutlinerWindow *window)
{
  OutlinerView *view;
  view = outliner_window_get_view(window);
  outliner_view_foreach_selected_subtree(view, outliner_document_move_up, FALSE, NULL);
}

void
outliner_action_move_down (GtkAction *action, OutlinerWindow *window)
{
  OutlinerView *view;
  view = outliner_window_get_view(window);
  outliner_view_foreach_selected_subtree(view, outliner_document_move_down, TRUE, NULL);
}

void
outliner_action_expand_all (GtkAction *action, OutlinerWindow *window)
{
  OutlinerView *view;
  view = outliner_window_get_view(window);
  gtk_tree_view_expand_all(GTK_TREE_VIEW (view));
}

void
outliner_action_collapse_all (GtkAction *action, OutlinerWindow *window)
{
  OutlinerView *view;
  view = outliner_window_get_view(window);
  gtk_tree_view_collapse_all(GTK_TREE_VIEW (view));
}

void
outliner_action_delete_item (GtkAction *action, OutlinerWindow *window)
{
  OutlinerView *view;
  view = outliner_window_get_view(window);
  outliner_view_foreach_selected_subtree(view, outliner_document_delete_item, TRUE, NULL);
}

void
outliner_action_select_all (GtkAction *action, OutlinerWindow *window)
{
  OutlinerView *view;
  view = outliner_window_get_view(window);
  gtk_tree_selection_select_all(gtk_tree_view_get_selection(GTK_TREE_VIEW (view)));
}


/* help actions */
void
outliner_action_about (GtkAction *action, OutlinerWindow *window)
{
  static GtkWidget *about_dialog = NULL;

  const gchar *authors[] = {
    "Steven Garrity <steven@silverorange.com>",
    "Dan Korostelev <dan@ats.energo.ru>",
    "Nathan Fredrickson <nathan@silverorange.com>",
    NULL
  };

  const gchar *documenters[] = {
    NULL
  };

  if (about_dialog)
    {
      gtk_window_present (GTK_WINDOW (about_dialog));
      return;
    }

  about_dialog = gnome_about_new ("Gnome Outliner", PACKAGE_VERSION,
                  "Copyright \xc2\xa9 2004",
                  _("Gnome Outliner is a simple outliner editor for organization and note taking."),
                  authors, documenters,
                  NULL, NULL);

  gtk_window_set_transient_for (GTK_WINDOW (about_dialog), GTK_WINDOW (window));
  g_object_add_weak_pointer (G_OBJECT (about_dialog),
		  	     (gpointer *) &about_dialog);

  gtk_widget_show (about_dialog);
}

