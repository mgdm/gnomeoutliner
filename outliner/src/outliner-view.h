/**
 * outliner-view.h
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

#ifndef OUTLINER_VIEW_H
#define OUTLINER_VIEW_H

#include <gtk/gtktreeview.h>
#include "outliner-document.h"

#define OUTLINER_TYPE_VIEW    (outliner_view_get_type ())
#define OUTLINER_VIEW(obj)    (G_TYPE_CHECK_INSTANCE_CAST ((obj), OUTLINER_TYPE_VIEW, OutlinerView))
#define OUTLINER_IS_VIEW(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), OUTLINER_TYPE_VIEW))

typedef struct _OutlinerView      OutlinerView;
typedef struct _OutlinerViewClass OutlinerViewClass;

struct _OutlinerView {
  GtkTreeView parent;
};

struct _OutlinerViewClass {
  GtkTreeViewClass parent_class;
};

GType             outliner_view_get_type     (void);
GtkWidget        *outliner_view_new          (OutlinerDocument *doc);
OutlinerDocument *outliner_view_get_document (OutlinerView *view);
void              outliner_view_add_item     (OutlinerView *view);

typedef void (* OutlinerViewForeachFunc) (OutlinerDocument *doc,
                                          GtkTreePath *path,
                                          gpointer data);

void outliner_view_foreach_selected_subtree (OutlinerView *view, OutlinerViewForeachFunc func,
                                             gboolean backwards, gpointer data);

void outliner_view_indent (OutlinerView *view, GtkTreePath *path, gpointer data);

gboolean outliner_view_get_last_selected (OutlinerView *view, GtkTreeIter *item);

#endif
