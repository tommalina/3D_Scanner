/*
 * CameraGdkDisplay.cpp
 *
 *  Created on: 28-11-2011
 *      Author: ble
 */

#include "CameraGdkDisplay.h"

#include <stdio.h>
#include <unistd.h>

CameraGdkDisplay::CameraGdkDisplay(unsigned int fps, DataContainer* data, GtkDrawingArea* drawingArea1, GtkDrawingArea* drawingArea2, GtkSpinButton* spinButton1, GtkSpinButton* spinButton2)
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
	mSpinButton1		= spinButton1;
	mSpinButton2		= spinButton2;
	mCameraId1			= 0;
	mCameraId2			= 0;
	mData				= data;

	if(mDrawingArea1!=NULL)
	{
		 g_signal_connect (G_OBJECT (mDrawingArea1), "expose_event", G_CALLBACK (drawCallback1), NULL);
	}
	if(mDrawingArea2!=NULL)
	{
		 g_signal_connect (G_OBJECT (mDrawingArea2), "expose_event", G_CALLBACK (drawCallback2), NULL);
	}
	if(mSpinButton1!=NULL)
	{
		mCameraId1		= gtk_spin_button_get_value_as_int(mSpinButton1);
		g_signal_connect (G_OBJECT (mSpinButton1), "value-changed", G_CALLBACK (changeCallback1), NULL);
	}
	if(mSpinButton2!=NULL)
	{
		mCameraId2		= gtk_spin_button_get_value_as_int(mSpinButton2);
		g_signal_connect (G_OBJECT (mSpinButton2), "value-changed", G_CALLBACK (changeCallback2), NULL);
	}
}

CameraGdkDisplay::~CameraGdkDisplay()
{

	if(mCamera1!=NULL)
	{
		delete mCamera1;
	}
	if(mCamera2!=NULL)
	{
		delete mCamera2;
	}
	if(mImage1!=NULL)
	{
		cvReleaseData(mImage1);
		cvReleaseImage(&mImage1);
	}
	if(mImage2!=NULL)
	{
		cvReleaseData(mImage2);
		cvReleaseImage(&mImage2);
	}
}

void CameraGdkDisplay::run()
{

	mCamera1					= new Camera(mCameraId1);
	mCamera2					= new Camera(mCameraId2);

	clock_t lastTime			= clock() / (CLOCKS_PER_SEC / 1000);
	clock_t currentTime;

	while (mRun)
	{

		display();

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

	gdk_threads_enter();

	mCamera1->captureImage();
	mCamera2->captureImage();

	if(mCamera1->getImage()&&mDrawingArea1)
	{

		if(!mImage1)
		{
			mImage1				= cvCreateImage( cvSize( mCamera1->getImage()->width, mCamera1->getImage()->height ), IPL_DEPTH_8U, 3 );
		}

		cvCvtColor(mCamera1->getImage(), mImage1, CV_BGR2RGB);

		if(!mPixbuf1)
		{
			mPixbuf1 = gdk_pixbuf_new_from_data ((guchar*)mImage1->imageData, GDK_COLORSPACE_RGB, FALSE, mImage1->depth, mImage1->width, mImage1->height, mImage1->widthStep, NULL, NULL);
		}

		gtk_widget_queue_draw(GTK_WIDGET(mDrawingArea1));

		if(mData->getImageLeftRef().tryLockData())
		{
			if(!mData->getImageLeftRef().getPtr())
			{
				mData->getImageLeftRef().setPtr(cvCreateImage( cvSize( mCamera1->getImage()->width, mCamera1->getImage()->height ), mCamera1->getImage()->depth, mCamera1->getImage()->nChannels ));
			}
			cvCopy(mCamera1->getImage(), mData->getImageLeftRef().getPtr());
			mData->getImageLeftRef().unlockData();
		}

	}

	if(mCamera2->getImage()&&mDrawingArea2)
	{

		if(!mImage2)
		{
			mImage2				= cvCreateImage( cvSize( mCamera2->getImage()->width, mCamera2->getImage()->height ), IPL_DEPTH_8U, 3 );
		}

		cvCvtColor(mCamera2->getImage(), mImage2, CV_BGR2RGB);

		if(!mPixbuf2)
		{
			mPixbuf2 = gdk_pixbuf_new_from_data ((guchar*)mImage2->imageData, GDK_COLORSPACE_RGB, FALSE, mImage2->depth, mImage2->width, mImage2->height, mImage2->widthStep, NULL, NULL);
		}

		gtk_widget_queue_draw(GTK_WIDGET(mDrawingArea2));

		if(mData->getImageRightRef().tryLockData())
		{
			if(!mData->getImageRightRef().getPtr())
			{
				mData->getImageRightRef().setPtr(cvCreateImage( cvSize( mCamera1->getImage()->width, mCamera1->getImage()->height ), mCamera1->getImage()->depth, mCamera1->getImage()->nChannels ));
			}
			cvCopy(mCamera1->getImage(), mData->getImageLeftRef().getPtr());
			mData->getImageRightRef().unlockData();
		}

	}

	gdk_threads_leave();
}

CameraGdkDisplay* CameraGdkDisplay::mInstance		= NULL;

gboolean CameraGdkDisplay::drawCallback1 (GtkWidget *widget, GdkEventExpose *event, gpointer data)
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

gboolean CameraGdkDisplay::drawCallback2 (GtkWidget *widget, GdkEventExpose *event, gpointer data)
{

	if(mInstance->mPixbuf2==NULL)
	{
		return FALSE;
	}

	GdkPixbuf* sPixbuf2		= gdk_pixbuf_scale_simple(mInstance->mPixbuf2, widget->allocation.width, widget->allocation.height, GDK_INTERP_NEAREST);
	gdk_draw_pixbuf ( widget->window , widget->style->fg_gc[gtk_widget_get_state (widget)] , sPixbuf2, 0, 0, 0, 0, widget->allocation.width, widget->allocation.height, GDK_RGB_DITHER_NORMAL, 0, 0 );
	if(sPixbuf2!=NULL)
	{
		gdk_pixbuf_unref(sPixbuf2);
	}

	return TRUE;
}

void CameraGdkDisplay::changeCallback1(GtkSpinButton *spinbutton, gpointer data)
{

	mInstance->mCameraId1		= gtk_spin_button_get_value_as_int(spinbutton);

	if(mInstance->mCamera1!=NULL)
	{
		delete mInstance->mCamera1;
	}
	if(mInstance->mImage1!=NULL)
	{
		cvReleaseData(mInstance->mImage1);
		cvReleaseImage(&mInstance->mImage1);
		mInstance->mImage1		= NULL;
		mInstance->mPixbuf1		= NULL;
	}

	mInstance->mCamera1		= new Camera(mInstance->mCameraId1);
}

void CameraGdkDisplay::changeCallback2(GtkSpinButton *spinbutton, gpointer data)
{
	mInstance->mCameraId2		= gtk_spin_button_get_value_as_int(spinbutton);

	if(mInstance->mCamera2!=NULL)
	{
		delete mInstance->mCamera2;
	}
	if(mInstance->mImage2!=NULL)
	{
		cvReleaseData(mInstance->mImage2);
		cvReleaseImage(&mInstance->mImage2);
		mInstance->mImage2		= NULL;
		mInstance->mPixbuf2		= NULL;
	}

	mInstance->mCamera2		= new Camera(mInstance->mCameraId2);
}
