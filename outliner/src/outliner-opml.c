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
#include "outliner-window.h"
#include "xml-gnomevfs.h"

#include <libgnomevfs/gnome-vfs.h>
#include <libxml/xmlIO.h>
#include <glib.h>
#include <gtk/gtk.h>

static void
save_attrs(gchar *name, gchar *value, xmlNodePtr cur)
{
  xmlNewProp(cur, name, value);
}

static void
save_recursively(OutlinerDocument *doc, xmlNodePtr parent, GtkTreeIter *iter) 
{
  gboolean val = TRUE;
  gchar *string;
  gboolean status;
  GHashTable *attr_hash;
  xmlNodePtr cur;
  GtkTreeIter child;

  while (val) {
    cur = xmlNewChild(parent, NULL, "outline", NULL);
    gtk_tree_model_get(GTK_TREE_MODEL(doc), iter, COL_TEXT, &string, -1);
    xmlNewProp(cur, "text", string);
    g_free(string);

    gtk_tree_model_get(GTK_TREE_MODEL(doc), iter, COL_STATUS, &status, -1);
    switch (status)
      {
        case TRUE:
        {
          xmlNewProp(cur, "_status", "checked");
          break;
        }
        case FALSE:
        {
          xmlNewProp(cur, "_status", "unchecked");
          break;
        }
        default:
        {
          xmlNewProp(cur, "_status", "none");
          break;
        }
      }

    gtk_tree_model_get(GTK_TREE_MODEL(doc), iter, COL_OTHER, &attr_hash, -1);
    g_hash_table_foreach(attr_hash, (GHFunc)save_attrs, cur);


    if (gtk_tree_model_iter_children(GTK_TREE_MODEL(doc), &child, iter))
      save_recursively(doc, cur, &child);
    val = gtk_tree_model_iter_next(GTK_TREE_MODEL(doc), iter);
  }
}

static void
save_head(OutlinerWindow *window, OutlinerDocument *doc, xmlNodePtr head) 
{
  GString *string = g_string_new(NULL);
  gint h,w,t,l;      /* height, width, top, left */

  gtk_window_get_size(GTK_WINDOW(window), &w, &h);
  gtk_window_get_position(GTK_WINDOW(window), &l, &t);
  outliner_document_set_w_top (doc,  t);
  outliner_document_set_w_left (doc,  l);
  outliner_document_set_w_bottom (doc,  t+h);
  outliner_document_set_w_right (doc,  l+w);

  /* XXX */
  
  xmlNewTextChild(head, NULL, "title", outliner_document_get_title(doc)->str );
  xmlNewTextChild(head, NULL, "ownerName", outliner_document_get_author(doc)->str );
  xmlNewTextChild(head, NULL, "ownerEmail", outliner_document_get_email(doc)->str );
  
  g_string_printf(string, "%i", outliner_document_get_w_top(doc));
  xmlNewTextChild(head, NULL, "windowTop", string->str);
  g_string_printf(string, "%i", outliner_document_get_w_bottom(doc));
  xmlNewTextChild(head, NULL, "windowBottom", string->str);
  g_string_printf(string, "%i", outliner_document_get_w_left(doc));
  xmlNewTextChild(head, NULL, "windowLeft", string->str);
  g_string_printf(string, "%i", outliner_document_get_w_right(doc));
  xmlNewTextChild(head, NULL, "windowRight", string->str);
  g_string_free(string, TRUE);
  
}

void
outliner_opml_save_file(OutlinerWindow *window, OutlinerDocument *doc, gchar *filename) 
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

    save_head(window, doc, head);

    if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(doc), &iter)) {
      save_recursively(doc, body, &iter);
    }

    output = xml_gnomevfs_create_ouputbuffer(handle);
    xmlSaveFormatFileTo(output, xmldoc, NULL, 1);
    /* XXX */

    outliner_document_set_uri(doc, filename);
    /*
    outliner_mainwindow_set_title();
    */

    outliner_document_set_changed (doc, FALSE);
  }
}

static void 
parse_recursively(OutlinerDocument *doc, xmlNodePtr cur, GtkTreeIter *parent) 
{
  GtkTreeIter iter;
  gboolean status;
  gchar *string;
  xmlAttr *cur_attr;
  GHashTable *attr_hash;

  while (cur != NULL) {
    status = FALSE;
    attr_hash = g_hash_table_new(g_str_hash, g_str_equal);
    if ((!xmlStrcmp(cur->name, "outline"))) {
      gtk_tree_store_append(GTK_TREE_STORE (doc), &iter, parent);
      cur_attr = cur->properties;
      while (cur_attr != NULL) {
        if ((!xmlStrcmp(cur_attr->name, "text"))) {
          gtk_tree_store_set(GTK_TREE_STORE (doc), &iter, COL_TEXT, xmlGetProp(cur, "text"), -1);
        }
        else if ((!xmlStrcmp(cur_attr->name, "_status"))) {
          string = xmlGetProp(cur, "_status");

          /* TODO: handle none/indeterminate values */
          if (g_ascii_strncasecmp(string, "checked", strlen(string)) == 0)
            status = TRUE;
          else if (g_ascii_strncasecmp(string, "unchecked", strlen(string)) == 0)
              status = FALSE;
          gtk_tree_store_set(GTK_TREE_STORE (doc), &iter, COL_STATUS, status, -1);       
        }
        else {
          if (!g_slist_find(outliner_document_get_column_names(doc), cur_attr->name)) {
            g_slist_append(outliner_document_get_column_names(doc), cur_attr->name);
          }
          g_hash_table_insert(attr_hash, g_strdup(cur_attr->name), g_strdup(xmlGetProp(cur, cur_attr->name)));
        }
        cur_attr = cur_attr->next;
      }
      /* Is this the right way to store an object? */
      gtk_tree_store_set(GTK_TREE_STORE (doc), &iter, COL_OTHER, attr_hash, -1);
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
    
    if ((!xmlStrcmp(cur->name, "windowTop")))
      outliner_document_set_w_top (doc,  atoi(xmlNodeListGetString(xmldoc, cur->xmlChildrenNode, 1)));
    else if ((!xmlStrcmp(cur->name, "windowLeft")))
      outliner_document_set_w_left (doc,  atoi(xmlNodeListGetString(xmldoc, cur->xmlChildrenNode, 1)));
    else if ((!xmlStrcmp(cur->name, "windowRight")))
      outliner_document_set_w_right (doc,  atoi(xmlNodeListGetString(xmldoc, cur->xmlChildrenNode, 1)));
    else if ((!xmlStrcmp(cur->name, "windowBottom")))
      outliner_document_set_w_bottom (doc,  atoi(xmlNodeListGetString(xmldoc, cur->xmlChildrenNode, 1)));
    
    if ((!xmlStrcmp(cur->name, "title")))
      outliner_document_set_title(doc, xmlNodeListGetString(xmldoc, cur->xmlChildrenNode, 1));
    else if ((!xmlStrcmp(cur->name, "ownerName")))
      outliner_document_set_author(doc, xmlNodeListGetString(xmldoc, cur->xmlChildrenNode, 1));
    else if ((!xmlStrcmp(cur->name, "ownerEmail")))
      outliner_document_set_email(doc, xmlNodeListGetString(xmldoc, cur->xmlChildrenNode, 1));

    if ((!xmlStrcmp(cur->name, "expansionState")))
    {
      gchar **rows;
      gint i;
      GArray* new_expanded = g_array_new(FALSE, FALSE, sizeof(gint));
      rows = g_strsplit(xmlNodeListGetString(xmldoc, cur->xmlChildrenNode, 1), ",", 0);
      for(i = 0; rows[i] != NULL; i++)
        g_array_append_val(new_expanded, rows[i]);
      g_array_free(new_expanded, FALSE);
      g_strfreev(rows);
    }

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

  outliner_document_set_uri(doc, filename);
  /*
  outliner_mainwindow_set_title();	
  */
  outliner_document_set_changed (doc, FALSE);
}

