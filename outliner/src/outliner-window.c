/**
 * outliner-window.c
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
#include "outliner-actions.h"
#include "outliner-view.h"
#include "outliner-document.h"

#ifdef HAVE_CONFIG_H
 #include <config.h>
#endif

#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <libgnomeui/gnome-stock-icons.h>
#include <gdk/gdkkeysyms.h>

#define OUTLINER_WINDOW_GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((obj), OUTLINER_TYPE_WINDOW, OutlinerWindowPrivate))

typedef struct _OutlinerWindowPrivate OutlinerWindowPrivate;
struct _OutlinerWindowPrivate {
  GtkUIManager   *merge;
  GtkActionGroup *action_group;

  GtkWidget *vbox;
  GtkWidget *sw;
  GtkWidget *view;
  GtkWidget *statusbar;
};

static GtkWindowClass *parent_class = NULL;

static void outliner_window_init       (OutlinerWindow      *window);
static void outliner_window_class_init (OutlinerWindowClass *window);
static void outliner_window_finalize   (GObject             *object);

static GtkActionEntry action_entries[] = {
  { "FileMenuAction", NULL, N_("_File") },
  { "EditMenuAction", NULL, N_("_Edit") },
  { "OutlineMenuAction", NULL, N_("_Outline") },
  { "HelpMenuAction", NULL, N_("_Help") },

  /* File Menu */
  { "NewAction", GTK_STOCK_NEW, NULL, NULL,
    N_("Create a new outline"), G_CALLBACK (outliner_action_new) },
  { "OpenAction", GTK_STOCK_OPEN, NULL, NULL,
    N_("Open an outline"), G_CALLBACK (outliner_action_open) },
  { "OpenLocationAction", NULL, N_("Open _Location..."), "<ctrl>L",
    N_("Open an outline at a specified location"), G_CALLBACK (outliner_action_open_location) },

  { "SaveAction", GTK_STOCK_SAVE, NULL, NULL,
    N_("Save this outline to current file"), G_CALLBACK (outliner_action_save) },
  { "SaveAsAction", GTK_STOCK_SAVE_AS, NULL, NULL,
    N_("Save this outline to a file"), G_CALLBACK (outliner_action_save_as) },
  { "ExportAction", NULL, N_("E_xport"), "<ctrl>X",
    N_("Export this outline to another format"), G_CALLBACK (outliner_action_export) },

  { "PrintPreviewAction", GTK_STOCK_PRINT_PREVIEW, NULL, NULL,
    NULL, G_CALLBACK (outliner_action_dummy) },
  { "PrintAction", GTK_STOCK_PRINT, NULL, NULL,
    N_("Print this outline"), G_CALLBACK (outliner_action_dummy) },

  { "PropertiesAction", GTK_STOCK_PROPERTIES, NULL, NULL,
    N_("Show the properties of this outline"), G_CALLBACK (outliner_action_properties) },

  { "QuitAction", GTK_STOCK_QUIT, NULL, NULL,
    N_("Quit Gnome Outliner"), G_CALLBACK(outliner_action_quit) },

  /* Edit Menu */
  { "UndoAction", GTK_STOCK_UNDO, NULL, "<ctrl>Z",
    N_("Undo the last action"), G_CALLBACK (outliner_action_dummy) },
  { "RedoAction", GTK_STOCK_REDO, NULL, "<shift><ctrl>Z",
    N_("Redo the undone action"), G_CALLBACK (outliner_action_dummy) },
  { "CutAction", GTK_STOCK_CUT, NULL, NULL,
    N_("Cut selection to the clipboard"), G_CALLBACK (outliner_action_dummy) },
  { "CopyAction", GTK_STOCK_COPY, NULL, NULL,
    N_("Copy selection to the clipboard"), G_CALLBACK (outliner_action_dummy) },
  { "PasteAction", GTK_STOCK_PASTE, NULL, NULL,
    N_("Paste the clipboard contents"), G_CALLBACK (outliner_action_dummy) },
  { "DeleteAction", GTK_STOCK_DELETE, NULL, NULL,
    N_("Delete selection"), G_CALLBACK (outliner_action_delete_item) },
  { "SelectAllAction", NULL, N_("Select _All"), "<ctrl>A",
    N_("Select entire outline"), G_CALLBACK (outliner_action_select_all) },
  { "PreferencesAction", GTK_STOCK_PREFERENCES, NULL, NULL,
    N_("Edit application preferences"), G_CALLBACK (outliner_action_dummy) },

  /* Outline Menu */
  { "AddItemAction", GTK_STOCK_ADD, N_("_Add Item"), NULL,
    N_("Add a new item"), G_CALLBACK (outliner_action_add_item) },
  { "IndentAction", GTK_STOCK_GO_FORWARD, N_("_Indent"), "Tab",
    N_("Indent the current selection"), G_CALLBACK (outliner_action_indent) },
  { "UnindentAction", GTK_STOCK_GO_BACK, N_("_Unindent"), "<shift>Tab",
    N_("Unindent the current selection"), G_CALLBACK (outliner_action_unindent) },
  { "MoveUpAction", GTK_STOCK_GO_UP, N_("Move _Up"), "<alt>Up",
    N_("Move the current selection up"), G_CALLBACK (outliner_action_move_up) },
  { "MoveDownAction", GTK_STOCK_GO_DOWN, N_("Move _Down"), "<alt>Down",
    N_("Move the current selection down"), G_CALLBACK (outliner_action_move_down) },

  { "ExpandAllAction", NULL, N_("E_xpand All"), NULL,
    N_("Expand all items"), G_CALLBACK (outliner_action_expand_all) },
  { "CollapseAllAction", NULL, N_("_Collapse All"), NULL,
    N_("Collapse all items"), G_CALLBACK (outliner_action_collapse_all) },

  /* Help Menu */
  { "AboutAction", GNOME_STOCK_ABOUT, NULL, NULL,
    N_("About Gnome Outliner"), G_CALLBACK (outliner_action_about) }
};

static const gchar *ui_info =
"<ui>"
"<menubar>"
"  <menu name=\"File\" action=\"FileMenuAction\">"
"     <menuitem name=\"New\" action=\"NewAction\"/>"
"     <menuitem name=\"Open\" action=\"OpenAction\"/>"
"     <menuitem name=\"OpenLocation\" action=\"OpenLocationAction\"/>"
"     <separator name=\"Sep1\"/>"
"     <menuitem name=\"Save\" action=\"SaveAction\"/>"
"     <menuitem name=\"SaveAs\" action=\"SaveAsAction\"/>"
"     <menuitem name=\"Export\" action=\"ExportAction\"/>"
"     <separator name=\"Sep2\"/>"
"     <menuitem name=\"PrintPreview\" action=\"PrintPreviewAction\"/>"
"     <menuitem name=\"Print\" action=\"PrintAction\"/>"
"     <separator name=\"Sep3\"/>"
"     <menuitem name=\"Properties\" action=\"PropertiesAction\"/>"
"     <separator name=\"Sep4\"/>"
"     <menuitem name=\"Quit\" action=\"QuitAction\"/>"
"  </menu>"
"  <menu name=\"Edit\" action=\"EditMenuAction\">"
"     <menuitem name=\"Undo\" action=\"UndoAction\"/>"
"     <menuitem name=\"Redo\" action=\"RedoAction\"/>"
"     <separator name=\"Sep1\"/>"
"     <menuitem name=\"Cut\" action=\"CutAction\"/>"
"     <menuitem name=\"Copy\" action=\"CopyAction\"/>"
"     <menuitem name=\"Paste\" action=\"PasteAction\"/>"
"     <menuitem name=\"Delete\" action=\"DeleteAction\"/>"
"     <separator name=\"Sep2\"/>"
"     <menuitem name=\"SelectAll\" action=\"SelectAllAction\"/>"
"     <separator name=\"Sep3\"/>"
"     <menuitem name=\"Preferences\" action=\"PreferencesAction\"/>"
"  </menu>"
"  <menu name=\"Outline\" action=\"OutlineMenuAction\">"
"     <menuitem name=\"AddItem\" action=\"AddItemAction\"/>"
"     <menuitem name=\"Indent\" action=\"IndentAction\"/>"
"     <menuitem name=\"Unindent\" action=\"UnindentAction\"/>"
"     <menuitem name=\"MoveUp\" action=\"MoveUpAction\"/>"
"     <menuitem name=\"MoveDown\" action=\"MoveDownAction\"/>"
"     <separator name=\"Sep2\"/>"
"     <menuitem name=\"ExpandAll\" action=\"ExpandAllAction\"/>"
"     <menuitem name=\"CollapseAll\" action=\"CollapseAllAction\"/>"
"  </menu>"
"  <menu name=\"Help\" action=\"HelpMenuAction\">"
"     <menuitem name=\"About\" action=\"AboutAction\"/>"
"  </menu>"
"</menubar>"
"<toolbar>"
"  <toolitem name=\"New\" action=\"NewAction\"/>"
"  <toolitem name=\"Open\" action=\"OpenAction\"/>"
"  <toolitem name=\"Save\" action=\"SaveAction\"/>"
"  <separator/>"
"  <toolitem name=\"Unindent\" action=\"UnindentAction\"/>"
"  <toolitem name=\"Indent\" action=\"IndentAction\"/>"
"  <toolitem name=\"MoveUp\" action=\"MoveUpAction\"/>"
"  <toolitem name=\"MoveDown\" action=\"MoveDownAction\"/>"
"</toolbar>"
"</ui>";

OutlinerView *
outliner_window_get_view (OutlinerWindow *window) {
  OutlinerWindowPrivate *priv = OUTLINER_WINDOW_GET_PRIVATE (window);
  return OUTLINER_VIEW (priv->view);
}

static void
set_tip (GtkWidget      *proxy,
         OutlinerWindow *window)
{
  OutlinerWindowPrivate *priv;
  GtkAction   *action;
  gchar *tooltip;

  priv = OUTLINER_WINDOW_GET_PRIVATE (window);

  action = g_object_get_data (G_OBJECT (proxy), "gtk-action");
  g_return_if_fail (action != NULL);

  g_object_get (G_OBJECT (action), "tooltip", &tooltip, NULL);

  if (tooltip) {
    gtk_statusbar_push (GTK_STATUSBAR (priv->statusbar), 0, tooltip);
    g_free (tooltip);
  }
}

static void
unset_tip (GtkWidget          *proxy,
           OutlinerWindow *window)
{
  OutlinerWindowPrivate *priv = OUTLINER_WINDOW_GET_PRIVATE (window);

  gtk_statusbar_pop (GTK_STATUSBAR (priv->statusbar), 0);
}

static void
merge_connect_proxy (GtkUIManager *merge,
                     GtkAction    *action,
                     GtkWidget    *proxy,
                     gpointer      data)
{
  gchar *tooltip;

  if (GTK_IS_MENU_ITEM (proxy)) 
    {
      g_object_get (G_OBJECT (action), "tooltip", &tooltip, NULL);

      if (tooltip) {
        g_object_set_data (G_OBJECT (proxy), "action-tooltip", tooltip);

        g_signal_connect (proxy, "select", G_CALLBACK (set_tip), data);
        g_signal_connect (proxy, "deselect", G_CALLBACK (unset_tip), data);
      }
    }
}

static void
merge_disconnect_proxy (GtkUIManager *merge,
                        GtkAction    *action,
                        GtkWidget    *proxy,
                        gpointer      data)
{
  gchar *tooltip;

  if (GTK_IS_MENU_ITEM (proxy)) 
    {
      g_signal_handlers_disconnect_by_func (proxy, set_tip, data);
      g_signal_handlers_disconnect_by_func (proxy, unset_tip, data);

      tooltip = g_object_get_data (G_OBJECT (proxy), "action-tooltip");
      g_warning ("discon: %s", tooltip);
      g_free (tooltip);
    }
}

static void
merge_add_widget (GtkUIManager *merge,
                  GtkWidget    *widget,
                  GtkContainer *container)
{
  gtk_box_pack_start (GTK_BOX (container), widget, FALSE, FALSE, 0);
  gtk_widget_show (widget);
}

#define IS_TAB(x) ((x == GDK_Tab) || (x == GDK_ISO_Left_Tab) || (x == GDK_KP_Tab))

static gboolean
key_press_cb(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
  OutlinerWindow *window = data;
  OutlinerWindowPrivate *priv = OUTLINER_WINDOW_GET_PRIVATE (window);

  /* intercept <shift>Tab */
  if (IS_TAB(event->keyval) && (event->state & GDK_SHIFT_MASK)) {
    outliner_action_unindent(gtk_ui_manager_get_action(priv->merge, "/ui/Outline/UnindentAction"), window);
    return TRUE;
  }
  /* intercept Tab */
  else if (IS_TAB(event->keyval)) {
    outliner_action_indent(gtk_ui_manager_get_action(priv->merge, "/ui/Outline/IndentAction"), window);
    return TRUE;
  }
  /* intercept Return */
  else if (event->keyval == GDK_Return) {
    outliner_action_add_item(gtk_ui_manager_get_action(priv->merge, "/ui/Outline/UnindentAction"), window);
    return TRUE;
  }
  /* intercept Delete */
  else if (event->keyval == GDK_Delete) {
    outliner_action_delete_item(gtk_ui_manager_get_action(priv->merge, "/ui/Outline/DeleteAction"), window);
    return TRUE;
  }
  return FALSE;
}

static gboolean
on_delete(GtkWidget *widget, gpointer data)
{
  OutlinerWindow *window = (OutlinerWindow*) widget;
  outliner_action_quit(NULL, window);

}

static void
outliner_onDragDataReceived(GtkWidget *widget, GdkDragContext *context,
                            int x, int y, GtkSelectionData *seldata, guint info,
                            guint time, gpointer userdata)
{
  OutlinerWindow   *window;
  OutlinerView     *view;
  OutlinerDocument *doc;
  GtkTreeIter cur, new;

  window = OUTLINER_WINDOW(userdata);
  view = outliner_window_get_view(window);
  doc = outliner_view_get_document(view);

  if (outliner_view_get_last_selected(view, &cur)) {
    if (gtk_tree_model_iter_has_child(GTK_TREE_MODEL (doc), &cur))
      gtk_tree_store_prepend(GTK_TREE_STORE (doc), &new, &cur);
    else
      gtk_tree_store_insert_after(GTK_TREE_STORE (doc), &new, NULL, &cur);
  }
  else
    gtk_tree_store_prepend(GTK_TREE_STORE (doc), &new, NULL);

  gtk_tree_store_set(GTK_TREE_STORE(doc), &new, COL_TEXT, (gchar*)seldata->data, -1);
}

static void
outliner_window_init (OutlinerWindow *window)
{
  GError *error = NULL;
  OutlinerWindowPrivate *priv = OUTLINER_WINDOW_GET_PRIVATE (window);

  priv->vbox = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (window), priv->vbox);
  gtk_widget_show (priv->vbox);

  priv->statusbar = gtk_statusbar_new ();
  gtk_box_pack_end (GTK_BOX (priv->vbox), priv->statusbar, FALSE, TRUE, 0);
  gtk_widget_show (priv->statusbar);

  priv->sw = gtk_scrolled_window_new (0, 0);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(priv->sw),
                                 GTK_POLICY_AUTOMATIC,
                                 GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(priv->sw),
                                      GTK_SHADOW_IN);

  priv->view = outliner_view_new (outliner_document_new ());
  gtk_container_add (GTK_CONTAINER (priv->sw), priv->view);
  gtk_box_pack_end (GTK_BOX (priv->vbox), priv->sw, TRUE, TRUE, 0);
  gtk_widget_show (priv->view);
  gtk_widget_show (priv->sw);

  priv->action_group = gtk_action_group_new ("OutlinerWindowActions");
  gtk_action_group_add_actions (priv->action_group, action_entries,
      G_N_ELEMENTS (action_entries), window);

  priv->merge = gtk_ui_manager_new ();
  gtk_ui_manager_insert_action_group (priv->merge, priv->action_group, 0);
  g_signal_connect (priv->merge, "add-widget",
      G_CALLBACK (merge_add_widget), priv->vbox);
  g_signal_connect (priv->merge, "connect-proxy",
      G_CALLBACK (merge_connect_proxy), window);

  /* Set up drag and drop from external apps */
  enum {
    TARGET_STRING,
    TARGET_URL
  };

  static GtkTargetEntry targetentries[] =
  {
    { "STRING",        0, TARGET_STRING },
    { "text/plain",    0, TARGET_STRING },
    { "text/uri-list", 0, TARGET_URL },
  };

  gtk_drag_dest_set(GTK_WIDGET(window), GTK_DEST_DEFAULT_ALL, targetentries, 3,
                    GDK_ACTION_COPY|GDK_ACTION_MOVE|GDK_ACTION_LINK);

  g_signal_connect(window, "drag_data_received",
                   G_CALLBACK(outliner_onDragDataReceived), window);

  if (!gtk_ui_manager_add_ui_from_string (priv->merge, ui_info, -1, &error)) {
    g_message ("%s", error->message);
    g_error_free (error);
    gtk_exit(0);
  }

  /*
  if (!gtk_ui_manager_add_ui_from_file (priv->merge, PACKAGE_DATA_DIR"ui.xml", &error)) {
    g_message ("%s", error->message);
    g_error_free (error);
    g_message ("Trying source dir instead: %s", PACKAGE_SOURCE_DIR"/data/ui.xml");
    if (!gtk_ui_manager_add_ui_from_file (priv->merge, PACKAGE_SOURCE_DIR"/data/ui.xml", &error)) {
      g_message ("%s", error->message);
      g_error_free (error);
      gtk_exit(0);
    }
  }
  */

  gtk_window_add_accel_group(GTK_WINDOW (window), 
      gtk_ui_manager_get_accel_group(priv->merge));

  gtk_ui_manager_ensure_update(priv->merge);

  gtk_window_set_title(GTK_WINDOW (window), _("Gnome Outliner"));
  gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);

  gtk_widget_show(GTK_WIDGET(window));
  outliner_view_add_item(OUTLINER_VIEW(priv->view));

  g_signal_connect(G_OBJECT (priv->view), "key_press_event",
                   G_CALLBACK(key_press_cb), window);
  g_signal_connect(G_OBJECT (window), "delete_event", G_CALLBACK(on_delete), NULL);
}

static void
outliner_window_class_init (OutlinerWindowClass *class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);
  parent_class = g_type_class_peek_parent (class);

  object_class->finalize = outliner_window_finalize;

  g_type_class_add_private (class, sizeof (OutlinerWindowPrivate));
}

GtkWidget *
outliner_window_new (void)
{
  return g_object_new (OUTLINER_TYPE_WINDOW, NULL);
}

static void
outliner_window_finalize (GObject *object)
{
  OutlinerWindowPrivate *priv = OUTLINER_WINDOW_GET_PRIVATE (object);

  g_object_unref (priv->action_group);
  g_object_unref (priv->merge);

  G_OBJECT_CLASS (parent_class)->finalize (object);
}

G_DEFINE_TYPE (OutlinerWindow, outliner_window, GTK_TYPE_WINDOW);

