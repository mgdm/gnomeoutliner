/**
 * outliner-actions.h
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

#ifndef OUTLINER_ACTIONS_H
#define OUTLINER_ACTIONS_H

#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <libgnomeui/gnome-about.h>
#include <libgnomeui/gnome-stock-icons.h>

/* action callbacks */
void outliner_action_about     (GtkAction *action, OutlinerWindow *window);
void outliner_action_new_child (GtkAction *action, OutlinerWindow *window);
void outliner_action_indent    (GtkAction *action, OutlinerWindow *window);

void outliner_action_new       (GtkAction *action, OutlinerWindow *window);
void outliner_action_open      (GtkAction *action, OutlinerWindow *window);
void outliner_action_open_location (GtkAction *action, OutlinerWindow *window);
void outliner_action_save      (GtkAction *action, OutlinerWindow *window);
void outliner_action_save_as   (GtkAction *action, OutlinerWindow *window);
void outliner_action_export    (GtkAction *action, OutlinerWindow *window);

void outliner_action_dummy     (GtkAction *action, OutlinerWindow *window);

#endif
