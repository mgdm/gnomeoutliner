/**
 * outliner-fprops-dialog.h
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

#ifndef OUTLINER_FPROPS_DIALOG_H
#define OUTLINER_FPROPS_DIALOG_H

#include "outliner-window.h"
#include "outliner-view.h"
#include "outliner-document.h"

#include <gtk/gtkwindow.h>

#define OUTLINER_TYPE_FPROPS_DIALOG (outliner_fprops_dialog_get_type ())

typedef struct _OutlinerFPropsDialog      OutlinerFPropsDialog;
typedef struct _OutlinerFPropsDialogClass OutlinerFPropsDialogClass;

struct _OutlinerFPropsDialog {
  GtkWindow parent;
};

struct _OutlinerFPropsDialogClass {
  GtkWindowClass parent_class;
};

GType         outliner_fprops_dialog_get_type (void);
GtkWidget    *outliner_fprops_dialog_new      (OutlinerDocument *tmpdoc);


#endif
