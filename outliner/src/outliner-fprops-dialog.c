/**
 * outliner-fprops-dialog.c
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

#include "outliner-fprops-dialog.h"
#include "outliner-document.h"

#include <libgnomeui/gnome-stock-icons.h>
#include <gtk/gtk.h>

#define OUTLINER_FPROPS_DIALOG_GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((obj), OUTLINER_TYPE_FPROPS_DIALOG, OutlinerFPropsDialogPrivate))

typedef struct _OutlinerFPropsDialogPrivate OutlinerFPropsDialogPrivate;
struct _OutlinerFPropsDialogPrivate {
  GtkWidget *window;

  GtkWidget *title_label;
  GtkWidget *file_label;
  GtkWidget *created_label;
  GtkWidget *modified_label;
  GtkWidget *author_label;
  GtkWidget *email_label;

  GtkWidget *title_entry;
  GtkWidget *file_label2;
  GtkWidget *created_label2;
  GtkWidget *modified_label2;
  GtkWidget *author_entry;
  GtkWidget *email_entry;
};

static GtkWindowClass *parent_class = NULL;
static OutlinerDocument *doc = NULL;

static void outliner_fprops_dialog_init     (OutlinerFPropsDialog *dialog);
static void outliner_fprops_dialog_set      (OutlinerFPropsDialog *dialog);
static void outliner_fprops_dialog_get      (OutlinerFPropsDialog *dialog);
static void outliner_fprops_dialog_finalize (GObject              *object);


static void
outliner_fprops_dialog_init (OutlinerFPropsDialog *dialog)
{
  GtkWidget *table;

  OutlinerFPropsDialogPrivate *priv = OUTLINER_FPROPS_DIALOG_GET_PRIVATE (dialog);

  if (priv->window)
    {
      gtk_window_present (GTK_WINDOW(priv->window));
      return;
    }


  priv->window = gtk_dialog_new_with_buttons ("File Properties",
                   GTK_WINDOW(dialog), GTK_DIALOG_DESTROY_WITH_PARENT,
                   GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE, NULL);

  g_signal_connect_swapped (priv->window, "response", 
                  G_CALLBACK (outliner_fprops_dialog_get), dialog);

  table = gtk_table_new (8, 2, FALSE);
  gtk_table_set_row_spacings (GTK_TABLE(table), 5);
  gtk_table_set_col_spacings (GTK_TABLE(table), 10);

  gtk_box_pack_end (GTK_BOX(GTK_DIALOG(priv->window)->vbox),
                    table, TRUE, TRUE, 0);

  priv->title_label = gtk_label_new ("Title:");
  gtk_misc_set_alignment (GTK_MISC(priv->title_label), 0, 0.5);
  gtk_table_attach (GTK_TABLE(table), priv->title_label, 0, 1, 0, 1, GTK_FILL, GTK_EXPAND, 0, 0);


  priv->file_label = gtk_label_new ("File:");
  gtk_misc_set_alignment (GTK_MISC(priv->file_label), 0, 0.5);
  gtk_table_attach (GTK_TABLE(table), priv->file_label, 0, 1, 1, 2, GTK_FILL, GTK_EXPAND, 0, 0);

  priv->created_label = gtk_label_new ("Created:");
  gtk_misc_set_alignment (GTK_MISC(priv->created_label), 0, 0.5);
  gtk_table_attach (GTK_TABLE(table), priv->created_label, 0, 1, 3, 4, GTK_FILL, GTK_EXPAND, 0, 0);


  priv->modified_label = gtk_label_new ("Modified:");
  gtk_misc_set_alignment (GTK_MISC(priv->modified_label), 0, 0.5);
  gtk_table_attach (GTK_TABLE(table), priv->modified_label, 0, 1, 4, 5, GTK_FILL, GTK_EXPAND, 0, 0);

  priv->author_label = gtk_label_new ("Author:");
  gtk_misc_set_alignment (GTK_MISC(priv->author_label), 0, 0.5);
  gtk_table_attach (GTK_TABLE(table), priv->author_label, 0, 1, 6, 7, GTK_FILL, GTK_EXPAND, 0, 0);

  priv->email_label = gtk_label_new ("Author Email:");
  gtk_misc_set_alignment (GTK_MISC(priv->email_label), 0, 0.5);
  gtk_table_attach (GTK_TABLE(table), priv->email_label, 0, 1, 7, 8, GTK_FILL, GTK_EXPAND, 0, 0);

  priv->title_entry = gtk_entry_new ();
  gtk_entry_set_text(GTK_ENTRY(priv->title_entry), doc->title->str);
  gtk_table_attach (GTK_TABLE(table), priv->title_entry, 1, 2, 0, 1, GTK_EXPAND, GTK_EXPAND, 0, 0);

  priv->file_label2 = gtk_label_new (NULL);
  gtk_label_set_text(GTK_LABEL(priv->file_label2), doc->uri->str);
  gtk_label_set_selectable(GTK_LABEL(priv->file_label2), TRUE);
  gtk_table_attach (GTK_TABLE(table), priv->file_label2, 1, 2, 1, 2, GTK_EXPAND, GTK_EXPAND, 0, 0);

  priv->created_label2 = gtk_label_new (NULL);
  gtk_label_set_selectable(GTK_LABEL(priv->created_label2), TRUE);
  gtk_table_attach (GTK_TABLE(table), priv->created_label2, 1, 2, 3, 4, GTK_EXPAND, GTK_EXPAND, 0, 0);

  priv->modified_label2 = gtk_label_new (NULL);
  gtk_label_set_selectable(GTK_LABEL(priv->modified_label2), TRUE);
  gtk_table_attach (GTK_TABLE(table), priv->modified_label2, 1, 2, 4, 5, GTK_EXPAND, GTK_EXPAND, 0, 0);

  priv->author_entry = gtk_entry_new ();
  gtk_entry_set_text(GTK_ENTRY(priv->author_entry), doc->author->str);
  gtk_table_attach (GTK_TABLE(table), priv->author_entry, 1, 2, 6, 7, GTK_EXPAND, GTK_EXPAND, 0, 0);

  priv->email_entry = gtk_entry_new ();
  gtk_entry_set_text(GTK_ENTRY(priv->email_entry), doc->email->str);
  gtk_table_attach (GTK_TABLE(table), priv->email_entry, 1, 2, 7, 8, GTK_EXPAND, GTK_EXPAND, 0, 0);

  gtk_widget_show_all (priv->window);
}


static void
outliner_fprops_dialog_class_init (OutlinerFPropsDialogClass *class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);
  parent_class = g_type_class_peek_parent (class);

  object_class->finalize = outliner_fprops_dialog_finalize;

  g_type_class_add_private (class, sizeof (OutlinerFPropsDialogPrivate));
}


GtkWidget *
outliner_fprops_dialog_new (OutlinerDocument *tmpdoc)
{
  doc = tmpdoc;
  GtkWidget *dialog = g_object_new (OUTLINER_TYPE_FPROPS_DIALOG, NULL);

  return dialog;
}


static void
outliner_fprops_dialog_get (OutlinerFPropsDialog *dialog)
{
  OutlinerFPropsDialogPrivate *priv = OUTLINER_FPROPS_DIALOG_GET_PRIVATE (dialog);

  outliner_document_set_title(doc, gtk_entry_get_text(GTK_ENTRY(priv->title_entry)));
  outliner_document_set_author(doc, gtk_entry_get_text(GTK_ENTRY(priv->author_entry)));
  outliner_document_set_email(doc, gtk_entry_get_text(GTK_ENTRY(priv->email_entry)));

  gtk_widget_destroy(GTK_WIDGET(dialog));
}

static void
outliner_fprops_dialog_finalize (GObject *object)
{
  OutlinerFPropsDialogPrivate *priv = OUTLINER_FPROPS_DIALOG_GET_PRIVATE (object);

  G_OBJECT_CLASS (parent_class)->finalize (object);
}


G_DEFINE_TYPE (OutlinerFPropsDialog, outliner_fprops_dialog, GTK_TYPE_WINDOW);
