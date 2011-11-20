/*
 * main.cpp
 *
 *  Created on: 10-09-2011
 *      Author: ble
 */


#include "ThreadManager.h"
#include <stdio.h>
#include <gtk/gtk.h>
#include <gtk/gtkmain.h>
#include <gtk/gtkbuilder.h>
#include <gtk/gtkwidget.h>

using namespace std;

int main(int argc, char** argv)
{
	//TODO: In this place will be run two threads, that will support 3D rendering.

	GtkBuilder		*builder;
	GtkWidget		*window;
	GError			*error		= NULL;

	gtk_init(&argc, &argv);

	builder			= gtk_builder_new();

	if(!gtk_builder_add_from_file(builder, "res/ui/ui.glade", &error)) {
		g_warning("%s",error->message);
		g_free(error);
		return(1);
	}

	window		= GTK_WIDGET(gtk_builder_get_object(builder , "mainWindow" ));

	gtk_builder_connect_signals(builder, NULL);

	g_object_unref(G_OBJECT(builder));

	gtk_widget_show(window);

	gtk_main();

	return 0;
}
