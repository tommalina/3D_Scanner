/*
 * CameraGdkDisplay.cpp
 *
 *  Created on: 28-11-2011
 *      Author: ble
 */

#include "CameraGdkDisplay.h"

#include <stdio.h>
#include <unistd.h>

CameraGdkDisplay::CameraGdkDisplay(unsigned int fps, GtkDrawingArea* drawingArea1, GtkDrawingArea* drawingArea2)
{
	mInstance			= this;
	mRun				= true;
	mFPS				= fps;
	mCamera1			= NULL;
	mCamera2			= NULL;
	mImage1				= NULL;
	mImage2				= NULL;
	mPixbuf1			= NULL;
	mPixbuf2			= NULL;
	mDrawingArea1		= drawingArea1;
	mDrawingArea2		= drawingArea2;

	if(mDrawingArea1!=NULL)
	{
		 g_signal_connect (G_OBJECT (mDrawingArea1), "expose_event", G_CALLBACK (drawCallback), NULL);
	}
}

CameraGdkDisplay::~CameraGdkDisplay()
{
	if(mCamera1!=NULL)
	{
		delete mCamera1;
	}
	if(mImage1!=NULL) {
		cvReleaseData(mImage1);
		cvReleaseImage(&mImage1);
	}
}

void CameraGdkDisplay::run()
{

	mCamera1					= new Camera(0);

	clock_t lastTime			= clock() / (CLOCKS_PER_SEC / 1000);
	clock_t currentTime;

	while (mRun)
	{

		//mMutex.lock();

		display();

		//mMutex.unlock();

		currentTime		= clock() / (CLOCKS_PER_SEC / 1000);
		if(currentTime-lastTime<mFPS) {
			usleep((mFPS-(currentTime-lastTime))*1000);
		}
		lastTime		= clock() / (CLOCKS_PER_SEC / 1000);

	}

}

void CameraGdkDisplay::end()
{
	gdk_threads_leave();
	mRun		= false;
}

void CameraGdkDisplay::display()
{

	mCamera1->captureImage();

	if(!mCamera1->getImage()||!mDrawingArea1)
	{
		return;
	}

	if(!mImage1)
	{
		mImage1				= cvCreateImage( cvSize( mCamera1->getImage()->width, mCamera1->getImage()->height ), IPL_DEPTH_8U, 3 );
	}

	cvCvtColor(mCamera1->getImage(), mImage1, CV_BGR2RGB);

	gdk_threads_enter();

	if(!mPixbuf1)
	{
		mPixbuf1 = gdk_pixbuf_new_from_data ((guchar*)mImage1->imageData, GDK_COLORSPACE_RGB, FALSE, mImage1->depth, mImage1->width, mImage1->height, mImage1->widthStep, NULL, NULL);
	}

	gtk_widget_queue_draw(GTK_WIDGET(mDrawingArea1));

	gdk_threads_leave();

}

CameraGdkDisplay* CameraGdkDisplay::mInstance		= NULL;

gboolean CameraGdkDisplay::drawCallback (GtkWidget *widget, GdkEventExpose *event, gpointer data)
{

	if(mInstance->mPixbuf1==NULL)
	{
		return FALSE;
	}

	GdkPixbuf* sPixbuf1		= gdk_pixbuf_scale_simple(mInstance->mPixbuf1, widget->allocation.width, widget->allocation.height, GDK_INTERP_NEAREST);
	gdk_draw_pixbuf ( widget->window , widget->style->fg_gc[gtk_widget_get_state (widget)] , sPixbuf1, 0, 0, 0, 0, widget->allocation.width, widget->allocation.height, GDK_RGB_DITHER_NORMAL, 0, 0 );
	if(sPixbuf1!=NULL)
	{
		gdk_pixbuf_unref(sPixbuf1);
	}

	return TRUE;
}
