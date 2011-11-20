/*
 * Scanner3D.cpp
 *
 *  Created on: 20-11-2011
 *      Author: ble
 */

/**
 * Load gtk headers.
 */
#include <gtk/gtk.h>
#include <gtk/gtkmain.h>
#include <gtk/gtkbuilder.h>
#include <gtk/gtkwidget.h>

/**
 * Load local headers.
 */
#include "Scanner3D.h"
#include "ThreadManager.h"

std::auto_ptr<Scanner3D> Scanner3D::mInstance;

Scanner3D* Scanner3D::getInstance()
{
	if(mInstance.get()==NULL)
	{
		mInstance.reset(new Scanner3D());
	}
	return mInstance.get();
}

Scanner3D::Scanner3D()
{

}

Scanner3D::Scanner3D(Scanner3D& obj)
{

}

Scanner3D::~Scanner3D()
{

}

int Scanner3D::Run(int argc, char** argv)
{

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

	window			= GTK_WIDGET(gtk_builder_get_object(builder , "mainWindow" ));

	gtk_builder_connect_signals(builder, NULL);

	g_object_unref(G_OBJECT(builder));

	gtk_widget_show(window);

	gtk_main();


	return 0;
}

int Scanner3D::Scanner3DRun(int argc, char** argv)
{
	return getInstance()->Run(argc, argv);
}
