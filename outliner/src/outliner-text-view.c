/**
 * outliner-text-view.c
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

#include "outliner-text-view.h"

#include <glib-object.h>

#include <gdk/gdkkeysyms.h>

#include <gtk/gtk.h>

static void outliner_text_view_init (OutlinerTextView *view);
static void outliner_text_view_class_init (OutlinerTextViewClass *class);

static void outliner_text_view_cell_editable_iface_init (GtkCellEditableIface *iface);
static void outliner_text_view_start_editing (GtkCellEditable *editable,
							GdkEvent *event);

static gboolean
outliner_text_view_editable_key_press (GtkWidget *widget, GdkEventKey *event)
{
  switch (event->keyval)
    {
      case GDK_Escape:
      {
        gtk_cell_editable_editing_done (GTK_CELL_EDITABLE (widget));
        gtk_cell_editable_remove_widget (GTK_CELL_EDITABLE (widget));
        return TRUE;
        break;
      }
      default:
        return FALSE;
	    break;
    }
}

static void
outliner_text_view_editable_focus_out (GtkWidget *widget, GdkEventFocus *event)
{
  gtk_cell_editable_editing_done (GTK_CELL_EDITABLE (widget));
  gtk_cell_editable_remove_widget (GTK_CELL_EDITABLE (widget));
}

static void
outliner_text_view_start_editing (GtkCellEditable *editable, GdkEvent *event)
{
  g_signal_connect (editable, "key-press-event",
		    G_CALLBACK (outliner_text_view_editable_key_press), NULL);
  g_signal_connect (editable, "focus-out-event",
		    G_CALLBACK (outliner_text_view_editable_focus_out), NULL);
}

static void
outliner_text_view_init (OutlinerTextView *view)
{
  /* dummy */
}

static void
outliner_text_view_class_init (OutlinerTextViewClass *class)
{
  /* dummy */
}

static void
outliner_text_view_cell_editable_iface_init (GtkCellEditableIface *iface)
{
  iface->start_editing = outliner_text_view_start_editing;
}

G_DEFINE_TYPE_WITH_CODE (OutlinerTextView, outliner_text_view, GTK_TYPE_TEXT_VIEW,
			 G_IMPLEMENT_INTERFACE (GTK_TYPE_CELL_EDITABLE,
				 		outliner_text_view_cell_editable_iface_init));
