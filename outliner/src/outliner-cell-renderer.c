/**
 * outliner-cell-renderer.c
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

#include "outliner-cell-renderer.h"
#include "outliner-text-view.h"

#include <glib-object.h>

#include <gtk/gtk.h>

static void outliner_cell_renderer_text_init (OutlinerCellRendererText *renderer);
static void outliner_cell_renderer_text_class_init (OutlinerCellRendererTextClass *class);

static void
outliner_cell_renderer_text_size_request (GtkCellEditable *editable, GtkRequisition *req, gpointer data)
{
  g_message("got size request: %d %d\n", req->width, req->height);
}

static void
outliner_cell_renderer_text_size_allocate (GtkCellEditable *editable, GtkAllocation *alloc, gpointer data)
{
  g_message("got size allocate: %d %d\n", alloc->width, alloc->height);
}

static void
outliner_cell_renderer_text_editing_done (GtkCellEditable *editable, gpointer data)
{
  const gchar *path, *new_text;
  GtkTextBuffer *buffer;
  GtkTextIter start, end;

  path = g_object_get_data (G_OBJECT (editable), "gtk-cell-renderer-text-path");

  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (editable));
  gtk_text_buffer_get_bounds (buffer, &start, &end);

  new_text = gtk_text_buffer_get_text (buffer, &start, &end, TRUE);

  g_signal_emit_by_name (data, "edited", path, new_text, 0);
}

static GtkCellEditable *
outliner_cell_renderer_text_start_editing (GtkCellRenderer      *cell,
                                           GdkEvent             *event,
                                           GtkWidget            *widget,
                                           const gchar          *path,
                                           GdkRectangle         *background_area,
                                           GdkRectangle         *cell_area,
                                           GtkCellRendererState  flags)
{
  GtkCellRendererText *celltext;
  GtkWidget           *textview;
  GtkTextBuffer       *textbuffer;
  GtkTextIter          textiter;

  celltext = GTK_CELL_RENDERER_TEXT (cell);

  /* If the cell isn't editable we return NULL. */
  if (celltext->editable == FALSE)
    return NULL;

  textview = g_object_new (OUTLINER_TYPE_TEXT_VIEW,
                           NULL);

  if (celltext->text) {
    textbuffer = gtk_text_buffer_new (NULL);
    gtk_text_buffer_get_start_iter (textbuffer, &textiter);
    gtk_text_buffer_insert (textbuffer, &textiter, celltext->text, -1);

    gtk_text_view_set_buffer (GTK_TEXT_VIEW (textview), textbuffer);
  }

  g_object_set_data_full (G_OBJECT (textview), "gtk-cell-renderer-text-path", g_strdup (path), g_free);

  //g_signal_emit_by_name (textview, "select-all", TRUE, NULL);

  gtk_widget_show (textview);
  g_signal_connect (textview, "editing-done",
                    G_CALLBACK (outliner_cell_renderer_text_editing_done),
                    celltext);
  g_signal_connect (textview, "size-request",
                    G_CALLBACK (outliner_cell_renderer_text_size_request),
                    celltext);
  g_signal_connect (textview, "size-allocate",
                    G_CALLBACK (outliner_cell_renderer_text_size_allocate),
                    celltext);
  
  return GTK_CELL_EDITABLE (textview);
}

static void
outliner_cell_renderer_text_init (OutlinerCellRendererText *renderer)
{
  /* dummy */
}

static void
outliner_cell_renderer_text_class_init (OutlinerCellRendererTextClass *class)
{
  GtkCellRendererClass *cell_class = GTK_CELL_RENDERER_CLASS (class);

  cell_class->start_editing = outliner_cell_renderer_text_start_editing;
}

G_DEFINE_TYPE (OutlinerCellRendererText, outliner_cell_renderer_text, GTK_TYPE_CELL_RENDERER_TEXT);
