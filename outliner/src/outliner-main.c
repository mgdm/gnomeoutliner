/**
 * outliner-main.c
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

#ifdef HAVE_CONFIG_H
 #include <config.h>
#endif

#include <gtk/gtk.h>
#include <libgnome/gnome-program.h>
#include <libgnomeui/gnome-ui-init.h>

int
main (int argc, char *argv[])
{
  GtkWidget *win;

  gtk_init (&argc, &argv);
  gnome_program_init(PACKAGE_NAME, PACKAGE_VERSION,
		     LIBGNOMEUI_MODULE, argc, argv, NULL);

  win = outliner_window_new ();
  gtk_window_present (GTK_WINDOW (win));

  g_signal_connect (win, "destroy",
		    gtk_main_quit, NULL);

  gtk_main ();
}
