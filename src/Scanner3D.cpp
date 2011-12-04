/*
 * Scanner3D.cpp
 *
 *  Created on: 20-11-2011
 *      Author: ble
 */

/**
 * Load local headers.
 */
#include "Scanner3D.h"
#include "ThreadManager.h"
#include "DisplayMain.h"

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

	// Initialize UI from xml file.

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
	g_signal_connect (GTK_OBJECT(window), "destroy", G_CALLBACK (mainWindowDestroy), NULL);

	gtk_builder_connect_signals(builder, NULL);

	g_object_unref(G_OBJECT(builder));

	gtk_widget_show(window);

	// Start application threads that will support all 2D and 3D calculations.

	DisplayMain displayMain;

	ThreadManager::addThread(&displayMain);

	ThreadManager::startAll();

	// Start UI main GTK thread.

	gtk_main();

	return 0;
}

int Scanner3D::Scanner3DRun(int argc, char** argv)
{
	return getInstance()->Run(argc, argv);
}

void Scanner3D::mainWindowDestroy(GtkObject *object, gpointer user_data)
{
	// Stop all application threads.

	ThreadManager::endNowAll();

	gtk_main_quit ();
}
