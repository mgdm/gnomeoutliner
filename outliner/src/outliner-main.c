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
#include "outliner-opml.h"
#include "outliner-view.h"

#ifdef HAVE_CONFIG_H
 #include <config.h>
#endif

#include <gtk/gtk.h>
#include <libgnome/gnome-program.h>
#include <libgnomeui/gnome-ui-init.h>
#include <getopt.h>
#include <libgnomevfs/gnome-vfs-utils.h>

void parse_command_line(int argc, char *argv[]);
void parse_command_line_file_arg(OutlinerWindow* window, int argc, char *argv[]);

static char* short_options="";

static struct option long_options[] =
  {
    {0, 0, 0, 0}
  };


int
main (int argc, char *argv[])
{
  GtkWidget *win;

  gtk_init (&argc, &argv);

  gnome_program_init(PACKAGE_NAME, PACKAGE_VERSION,
		     LIBGNOMEUI_MODULE, argc, argv, NULL);

  parse_command_line(argc, argv);

  win = outliner_window_new ();
  gtk_window_present (GTK_WINDOW (win));
  g_signal_connect (win, "destroy",gtk_main_quit, NULL);
 
  parse_command_line_file_arg((OutlinerWindow*)  win, argc, argv);
  
  gtk_main ();


}

void parse_command_line(int argc, char *argv[])
{
  int c, option_index;

  while (-1 != (c = getopt_long (argc, argv, short_options, long_options, &option_index))) {
    switch (c) {
      case '?':
        if (isprint (optopt))
          g_fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          g_fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
        exit(1);
	  
      default:
        g_assert_not_reached();
    }
  }
}

void parse_command_line_file_arg(OutlinerWindow *window, int argc, char *argv[])
{
  if (optind < argc) {
    int i;
    OutlinerView* view =  outliner_window_get_view(window);
    OutlinerDocument* doc = outliner_view_get_document(view);
    gchar *filename = gnome_vfs_make_uri_from_shell_arg((const gchar*) argv[optind]);

    outliner_opml_load_file(doc, filename);
    g_free(filename);

    for (i=optind+1; i < argc; i++) {
      GError *err = NULL;
      gchar *command= "";
      int j ;

      for (j=0; j < optind; j++)
        command = g_strjoin(" ", command, argv[j]);

      command = g_strjoin(" ", command, argv[i]);

      if (g_spawn_command_line_async (command, &err) == FALSE) {
        g_fprintf (stderr, "Error spawning %s: %s", command, err->message);
        exit(1);
      }

      g_free(command);
    }
  }
}

