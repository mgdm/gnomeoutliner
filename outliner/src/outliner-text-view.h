/**
 * outliner-text-view.h
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

#ifndef OUTLINER_TEXT_VIEW_H
#define OUTLINER_TEXT_VIEW_H

#include <gtk/gtktextview.h>

#define OUTLINER_TYPE_TEXT_VIEW    (outliner_text_view_get_type())
#define OUTLINER_TEXT_VIEW(obj)    (G_TYPE_CHECK_INSTANCE_CAST ((obj), OUTLINER_TYPE_TEXT_VIEW, OutlinerTextView))
#define OUTLINER_IS_TEXT_VIEW(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), OUTLINER_TYPE_TEXT_VIEW))

typedef struct _OutlinerTextView      OutlinerTextView;
typedef struct _OutlinerTextViewClass OutlinerTextViewClass;

struct _OutlinerTextView {
  GtkTextView parent;
};

struct _OutlinerTextViewClass {
  GtkTextViewClass parent_class;
};

#endif /* OUTLINER_TEXT_VIEW_H */
