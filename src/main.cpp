/*
 * main.cpp
 *
 *  Created on: 10-09-2011
 *      Author: ble
 */

#include "Scanner3D.h"
#include "ThreadManager.h"

extern "C" {
	static void mainWindowDestroy(GtkObject *object, gpointer user_data);
}

int main(int argc, char** argv)
{
	return Scanner3D::Scanner3DRun(argc, argv);
}

static void mainWindowDestroy(GtkObject *object, gpointer user_data)
{
	// Stop all application threads.

	ThreadManager::endNowAll();

	gtk_main_quit ();
}
