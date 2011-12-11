/*
 * DisplayMain.cpp
 *
 *  Created on: 28-11-2011
 *      Author: ble
 */

#include "DisplayMain.h"

#include <stdio.h>
#include <unistd.h>
#include <glibmm.h>
#include <gtkmm.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

DisplayMain::DisplayMain(unsigned int fps, GtkWidget *DACamera1, GtkDrawingArea *DACamera2)
{
	mRun		= true;
	mFPS		= fps;
	mDACamera1	= DACamera1;
	mDACamera2	= DACamera2;
	mCamera1	= NULL;
	mCamera2	= NULL;
	mImage		= NULL;
}

DisplayMain::~DisplayMain()
{
	if(mCamera1!=NULL)
	{
		delete mCamera1;
	}
	mCamera1		= NULL;
	if(mCamera2!=NULL)
	{
		delete mCamera2;
	}
	mCamera2		= NULL;
}

void DisplayMain::run()
{

	if(mDACamera1!=NULL)
	{
		//g_signal_connect (GTK_OBJECT(mDACamera1), "expose-event", G_CALLBACK (displayCallback), NULL);
	}

	mCamera1					= new Camera(0);



	clock_t lastTime			= clock() / (CLOCKS_PER_SEC / 1000);
	clock_t currentTime;

	while(mRun)
	{

		display();

		currentTime		= clock() / (CLOCKS_PER_SEC / 1000);
		if(currentTime-lastTime<mFPS) {
			usleep((mFPS-(currentTime-lastTime))*1000);
		}
		lastTime		= clock() / (CLOCKS_PER_SEC / 1000);
	}

}

void DisplayMain::end()
{
	mRun		= false;
}

void DisplayMain::display()
{

	mCamera1->captureImage();
	//if(mImage!=NULL)
	//{
	//	cvReleaseImage(&mImage);
	//	mImage			= NULL;
	//}
	mImage				= cvCreateImage( cvSize( mDACamera1->allocation.width/2, mDACamera1->allocation.height/2 ), IPL_DEPTH_8U, 3 );
	cvResize(mCamera1->getImage(), mImage);
	cvCvtColor(mImage, mImage, CV_BGR2RGB);

	GdkPixbuf *pixbuf = gdk_pixbuf_new_from_data ((guchar*)mImage->imageData, GDK_COLORSPACE_RGB, FALSE, mImage->depth, mImage->width, mImage->height, (mImage->widthStep), NULL, NULL);

	gtk_image_set_from_pixbuf(GTK_IMAGE(mDACamera1), pixbuf);

	gtk_widget_queue_draw (mDACamera1);

	//cvReleaseImage(&desImage);
}

gboolean DisplayMain::displayCallback(GtkWidget *object, GdkEventWindowState *event,  gpointer user_data)
{

	printf("-------------------- callback !!! \n");

	return true;
}

