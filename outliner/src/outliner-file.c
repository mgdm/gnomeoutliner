/**
 * outliner-file.c
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

#include "outliner-file.h"
#include "outliner-opml.h"
#include "outliner-document.h"
#include "outliner-window.h"

#include <glib.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>

void
outliner_file_save_as(OutlinerWindow *window, OutlinerDocument *doc) 
{
  GtkWidget *dialog, *overwrite;
  gboolean done = FALSE;

  dialog = gtk_file_chooser_dialog_new(_("Save file"), GTK_WINDOW (window), 
            GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, 
            GTK_STOCK_SAVE, GTK_RESPONSE_OK, NULL);

  gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_OK);

  /* TODO: HIGify */
  overwrite = gtk_message_dialog_new(GTK_WINDOW(dialog), GTK_DIALOG_DESTROY_WITH_PARENT, 
                GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, _("File already exists, overwrite?"));
  gtk_widget_hide(overwrite);
  gtk_dialog_set_default_response(GTK_DIALOG (dialog), GTK_RESPONSE_NO);

  while (!done) {
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_CANCEL)
      done = TRUE;
    else {
      if (gnome_vfs_uri_exists(gnome_vfs_uri_new(gtk_file_chooser_get_uri(GTK_FILE_CHOOSER(dialog))))) {
        if (gtk_dialog_run(GTK_DIALOG(overwrite)) == GTK_RESPONSE_YES) {
          outliner_opml_save_file(window, doc, gtk_file_chooser_get_uri(GTK_FILE_CHOOSER(dialog)));
          done = TRUE;
        }
        gtk_widget_hide(overwrite);
      } else {
        outliner_opml_save_file(window, doc, gtk_file_chooser_get_uri(GTK_FILE_CHOOSER(dialog)));
        done = TRUE;
      }
    }
  }
  gtk_widget_destroy(overwrite);
  gtk_widget_destroy(dialog);
}

void 
outliner_file_save(OutlinerWindow *window, OutlinerDocument *doc) 
{
  const GString* uri = outliner_document_get_uri(doc);
  if (uri->len != 0)
    outliner_opml_save_file(window, doc, uri->str);
  else
    outliner_file_save_as(window, doc);
}

void
outliner_file_save_changed(OutlinerWindow *window, OutlinerDocument *doc) 
{
  GtkWidget *save_file_question;

  if (outliner_document_get_changed(doc) == FALSE)
    return;

  save_file_question = gtk_message_dialog_new(GTK_WINDOW(window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, _("File changed, save file?"));
  gtk_dialog_set_default_response(GTK_DIALOG (save_file_question), GTK_RESPONSE_YES);
  if (gtk_dialog_run(GTK_DIALOG(save_file_question)) == GTK_RESPONSE_YES)
    outliner_file_save(window, doc);

  gtk_widget_destroy(save_file_question);

}



void 
outliner_file_open(OutlinerWindow *window, OutlinerDocument *doc) 
{
  GtkWidget *dialog;
  GdkScreen *screen;
  gint w, h;	/* width, height */

  dialog = gtk_file_chooser_dialog_new(_("Open file"), GTK_WINDOW (window), 
             GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, 
             GTK_STOCK_OPEN, GTK_RESPONSE_OK, NULL);

  gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_OK);

  if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
    outliner_opml_load_file(doc, gtk_file_chooser_get_uri(GTK_FILE_CHOOSER(dialog)));

    /* We shouldn't resize a window larger than the screen size */
    screen = gtk_window_get_screen(GTK_WINDOW(window));
    if (outliner_document_get_w_right(doc) > gdk_screen_get_width(screen))
      outliner_document_set_w_right(doc, gdk_screen_get_width(screen) - 10);
    if (outliner_document_get_w_bottom(doc) > gdk_screen_get_height(screen))
        outliner_document_set_w_bottom(doc, gdk_screen_get_height(screen) - 10);

    w = outliner_document_get_w_right(doc) - outliner_document_get_w_left(doc);
    h = outliner_document_get_w_bottom(doc) - outliner_document_get_w_top(doc);
    gtk_window_resize(GTK_WINDOW(window), w, h);
    gtk_window_move(GTK_WINDOW(window), outliner_document_get_w_left(doc), outliner_document_get_w_top(doc));

  }
  gtk_widget_destroy(dialog);
}

