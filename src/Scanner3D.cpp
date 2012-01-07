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
#include "CameraGdkDisplay.h"
#include "MainDataContainer.h"

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
	mData		= new MainDataContainer();
}

Scanner3D::Scanner3D(Scanner3D& obj)
{

}

Scanner3D::~Scanner3D()
{
	if(mData!=NULL)
	{
		delete mData;
	}
}

int Scanner3D::Run(int argc, char** argv)
{

	// Initialize UI from xml file.

	GtkBuilder		*builder;
	GtkWidget		*window;
	GtkDrawingArea	*camera1;
	GtkDrawingArea	*camera2;
	GtkSpinButton	*spinButton1;
	GtkSpinButton	*spinButton2;

	GError			*error		= NULL;

	g_thread_init( NULL );

	gdk_threads_init();

	gtk_init(&argc, &argv);

	builder			= gtk_builder_new();

	if(!gtk_builder_add_from_file(builder, "res/ui/ui.glade", &error)) {
		g_warning("%s",error->message);
		g_free(error);
		return(1);
	}

	window			= GTK_WIDGET(gtk_builder_get_object(builder , "mainWindow" ));
	g_signal_connect (GTK_OBJECT(window), "destroy", G_CALLBACK (mainWindowDestroy), NULL);

	camera1			= GTK_DRAWING_AREA(gtk_builder_get_object(builder, "camera1"));

	camera2			= GTK_DRAWING_AREA(gtk_builder_get_object(builder, "camera2"));

	spinButton1		= GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "cameraId1"));

	spinButton2		= GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "cameraId2"));

	gtk_builder_connect_signals(builder, NULL);

	g_object_unref(G_OBJECT(builder));

	gtk_widget_show(window);

	// Start application threads that will support all 2D and 3D calculations.

	CameraGdkDisplay cameraGdkDisplay(25, mData, camera1, camera2, spinButton1, spinButton2);

	ThreadManager::addThread(&cameraGdkDisplay);

	ThreadManager::startAll();

	// Start UI main GTK thread.

	gdk_threads_enter();

	gtk_main();

	gdk_threads_leave();

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
