/**
 * outliner-opml.c
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

#include "outliner-opml.h"
#include "outliner-document.h"
#include "xml-gnomevfs.h"

#include <libgnomevfs/gnome-vfs.h>
#include <libxml/xmlIO.h>
#include <glib.h>
#include <gtk/gtk.h>

static void
save_recursively(OutlinerDocument *doc, xmlNodePtr parent, GtkTreeIter *iter) 
{
  gboolean val = TRUE;
  gchar *string;
  xmlNodePtr cur;
  GtkTreeIter child;

  while (val) {
    gtk_tree_model_get(GTK_TREE_MODEL(doc), iter, 0, &string, -1);
    cur = xmlNewChild(parent, NULL, "outline", NULL);
    xmlNewProp(cur, "text", string);
    g_free(string);
		
    if (gtk_tree_model_iter_children(GTK_TREE_MODEL(doc), &child, iter))
      save_recursively(doc, cur, &child);
    val = gtk_tree_model_iter_next(GTK_TREE_MODEL(doc), iter);
  }
}

static void
save_head(OutlinerDocument *doc, xmlNodePtr head) 
{
  GString *string = g_string_new(NULL);
  /* gint h,w,t,l;	height, width, top, left */

  /*
  gtk_window_get_size(outliner->mainwindow, &w, &h);
  gtk_window_get_position(outliner->mainwindow, &l, &t);
  outliner->file->w_top = t;
  outliner->file->w_left = l;
  outliner->file->w_bottom = t+h;
  outliner->file->w_right = l+w;
  */

  /* XXX */
  /*
  xmlNewTextChild(head, NULL, "title", outliner->file->title);
  xmlNewTextChild(head, NULL, "ownerName", outliner->file->ownername);
  xmlNewTextChild(head, NULL, "ownerEmail", outliner->file->owneremail);
  g_string_printf(string, "%i", outliner->file->w_top);
  xmlNewTextChild(head, NULL, "windowTop", string->str);
  g_string_printf(string, "%i", outliner->file->w_bottom);
  xmlNewTextChild(head, NULL, "windowBottom", string->str);
  g_string_printf(string, "%i", outliner->file->w_left);
  xmlNewTextChild(head, NULL, "windowLeft", string->str);
  g_string_printf(string, "%i", outliner->file->w_right);
  xmlNewTextChild(head, NULL, "windowRight", string->str);
  g_string_free(string, TRUE);
  */
}

void
outliner_opml_save_file(OutlinerDocument *doc, gchar *filename) 
{
  GnomeVFSHandle *handle;
  GnomeVFSResult result;
  xmlOutputBufferPtr output;
  xmlDocPtr xmldoc;
  xmlNodePtr opml, body, head;
  GtkTreeIter iter;

  result = gnome_vfs_create(&handle, filename, GNOME_VFS_OPEN_WRITE, FALSE, S_IREAD|S_IWRITE);
  if (result == GNOME_VFS_OK) {
    xmldoc = xmlNewDoc(NULL);
    opml = xmlNewDocNode(xmldoc, NULL, "opml", NULL);
    xmlDocSetRootElement(xmldoc, opml);
    head = xmlNewChild(opml, NULL, "head",  NULL);
    body = xmlNewChild(opml, NULL, "body",  NULL);

    save_head(doc, head);

    if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(doc), &iter)) {
      save_recursively(doc, body, &iter);
    }

    output = xml_gnomevfs_create_ouputbuffer(handle);
    xmlSaveFormatFileTo(output, xmldoc, NULL, 1);
    /* XXX */
    /*
    outliner->file->uri = filename;
    outliner_mainwindow_set_title();
    */

    doc->changed= FALSE;
  }
}

static void 
parse_recursively(OutlinerDocument *doc, xmlNodePtr cur, GtkTreeIter *parent) 
{
  GtkTreeIter iter;

  while (cur != NULL) {
    if ((!xmlStrcmp(cur->name, "outline"))) {
      gtk_tree_store_append(GTK_TREE_STORE (doc), &iter, parent);
      gtk_tree_store_set(GTK_TREE_STORE (doc), &iter, 0, xmlGetProp(cur, "text"), -1);
    }
    if (cur->xmlChildrenNode != NULL)
      parse_recursively(doc, cur->xmlChildrenNode, &iter);
    cur = cur->next;
  }
}

static void 
parse_head(OutlinerDocument *doc, xmlDocPtr xmldoc, xmlNodePtr cur) 
{
  while (cur != NULL) {
    /*
    if ((!xmlStrcmp(cur->name, "windowTop")))
      outliner->file->w_top = atoi(xmlNodeListGetString(xmldoc, cur->xmlChildrenNode, 1));
    else if ((!xmlStrcmp(cur->name, "windowLeft")))
      outliner->file->w_left = atoi(xmlNodeListGetString(xmldoc, cur->xmlChildrenNode, 1));
    else if ((!xmlStrcmp(cur->name, "windowRight")))
      outliner->file->w_right = atoi(xmlNodeListGetString(xmldoc, cur->xmlChildrenNode, 1));
    else if ((!xmlStrcmp(cur->name, "windowBottom")))
      outliner->file->w_bottom = atoi(xmlNodeListGetString(xmldoc, cur->xmlChildrenNode, 1));
    if ((!xmlStrcmp(cur->name, "title")))
      outliner->file->title = xmlNodeListGetString(xmldoc, cur->xmlChildrenNode, 1);
    else if ((!xmlStrcmp(cur->name, "ownerName")))
      outliner->file->ownername = xmlNodeListGetString(xmldoc, cur->xmlChildrenNode, 1);
    else if ((!xmlStrcmp(cur->name, "ownerEmail")))
      outliner->file->owneremail = xmlNodeListGetString(xmldoc, cur->xmlChildrenNode, 1);
    */
    cur = cur->next;
  }
}

static int 
yeah(const char *f) 
{
	return 1;
}

void 
outliner_opml_load_file(OutlinerDocument *doc, gchar *filename) 
{
  xmlDocPtr xmldoc;
  xmlNodePtr cur;

  if (xmlRegisterInputCallbacks((xmlInputMatchCallback)yeah, 
       (xmlInputOpenCallback)xml_gnomevfs_openread, 
       (xmlInputReadCallback)xml_gnomevfs_read, 
       (xmlInputCloseCallback)xml_gnomevfs_close) == -1) {
    g_error("Failed to register input callbacks");
    return;
  }
  if ((xmldoc = xmlParseFile(filename)) == NULL) {
    g_error("Failed to parse file");
    return;
  }

  cur = xmlDocGetRootElement(xmldoc);
  if (xmlStrcmp(cur->name, (const xmlChar *) "opml")) {
    g_error("The file is not an OPML file");
    return;
  }

  gtk_tree_store_clear(GTK_TREE_STORE (doc));
  cur = cur->xmlChildrenNode;
  while (cur != NULL) {
    if ((!xmlStrcmp(cur->name, "head")) && cur->xmlChildrenNode != NULL) {
      parse_head(doc, xmldoc, cur->xmlChildrenNode);
    }
    if ((!xmlStrcmp(cur->name, "body")) && cur->xmlChildrenNode != NULL)
      parse_recursively(doc, cur->xmlChildrenNode, NULL);
    cur = cur->next;
  }
  /*
  outliner->file->uri = filename;
  outliner_mainwindow_set_title();	
  */
  doc->changed = FALSE;
}

