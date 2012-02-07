/*
 * Display3D.cpp
 *
 *  Created on: 21-01-2012
 *      Author: ble
 */

#include <memory.h>
#include <stdio.h>
#include <unistd.h>

//#include <gtkglmm.h>
#include <gdk/gdkgl.h>

#include "Display3D.h"
#include "ThreadManager.h"
#include "Camera.h"

//TODO: Change image size.
#define IMAGE_WIDTH		640
#define IMAGE_HEIGHT	320

class DepthMap : public ThreadRunnable
{
private:

	static std::auto_ptr<DepthMap>				mInstance;

	bool										mRun;

	int											mFPS;

	DataContainer*								mData;

	GtkSpinButton*								mSpinButtonFPS;

	GtkDrawingArea*								mDepthMapDrawingArea;

	DepthMap();

	DepthMap(const DepthMap& obj);

public:

	IplImage*									mDepthImage;

	GdkPixbuf*									mPixbuf;

	virtual 									~DepthMap();

	void										run();

	void										end();

	void										displayDepthMap();

	void										initialize(GtkSpinButton* depthMap3DFPS, GtkDrawingArea* depthMapDrawingArea, DataContainer* data);

	DataContainer*								getData();

	static DepthMap*							getInstance();

	static void 								start3D(GtkButton *button, gpointer   user_data);

	static void									stop3D(GtkButton *button, gpointer   user_data);

	static gboolean 							draw3DCallback(GtkWidget *widget, GdkEventExpose *event, gpointer data);

};

class CalculateDepthMap : public ThreadRunnable
{
private:

	static std::auto_ptr<CalculateDepthMap>		mInstance;

	bool										mRun;
	int											mFPS;
	DataContainer*								mData;
	GtkSpinButton*								mSpinButtonFPS;

	CalculateDepthMap();

	CalculateDepthMap(const CalculateDepthMap& obj);

public:

	virtual 									~CalculateDepthMap();

	void										run();

	void										end();

	void										calculateDepthMap();

	void										initialize(GtkSpinButton* depthMap3DFPS, DataContainer* data);

	DataContainer*								getData();

	static CalculateDepthMap*					getInstance();

};

class Render3D : public ThreadRunnable
{
private:

	static std::auto_ptr<Render3D>				mInstance;

	bool										mRun;
	int											mFPS;
	DataContainer*								mData;
	GtkSpinButton*								mSpinButtonFPS;

	Render3D();

	Render3D(const Render3D& obj);

public:

	virtual 									~Render3D();

	void										run();

	void										end();

	void										initialize(GtkSpinButton* render3DFPS, DataContainer* data);

	DataContainer*								getData();

	static Render3D*							getInstance();

};

DepthMap::DepthMap()
{
	mFPS					= 1000/30;
	mSpinButtonFPS			= NULL;
	mDepthImage				= NULL;
	mPixbuf					= NULL;
	mDepthMapDrawingArea	= NULL;
}

DepthMap::DepthMap(const DepthMap& obj)
{
	mFPS					= 1000/30;
	mSpinButtonFPS			= NULL;
	mDepthImage				= NULL;
	mPixbuf					= NULL;
	mDepthMapDrawingArea	= NULL;
}

DepthMap::~DepthMap()
{
	if(mDepthImage!=NULL)
	{
		cvReleaseData(mDepthImage);
		cvReleaseImage(&mDepthImage);
		mPixbuf				= NULL;
	}
}

void DepthMap::run()
{
	mRun		= true;

	if(mSpinButtonFPS!=NULL)
	{
		mFPS			= 1000/gtk_spin_button_get_value_as_int(mSpinButtonFPS);
	}

	clock_t lastTime			= clock() / (CLOCKS_PER_SEC / 1000);
	clock_t currentTime;

	while(mRun)
	{

		printf("---------------- DepthMap display \n");

		displayDepthMap();

		currentTime		= clock() / (CLOCKS_PER_SEC / 1000);
		if(currentTime-lastTime<mFPS) {
			sleep((mFPS-(currentTime-lastTime)));
		}
		lastTime		= clock() / (CLOCKS_PER_SEC / 1000);
	}

}

void DepthMap::end()
{
	mRun		= false;
}

void DepthMap::displayDepthMap()
{

	gdk_threads_enter();

	mData->getDepthMap().lockData();

	if(mData->getDepthMap().getPtr()!=NULL)
	{

		if(!mDepthImage)
		{
			mDepthImage				= cvCreateImage( cvSize( IMAGE_WIDTH, IMAGE_HEIGHT ), IPL_DEPTH_8U, 3 );
		}


		CvMat* vdisp 		= cvCreateMat( IMAGE_HEIGHT, IMAGE_WIDTH, CV_8U );
		cvNormalize( mData->getDepthMap().getPtr(), vdisp, 0, 256, CV_MINMAX );
		cvCvtColor(vdisp, mDepthImage, /*CV_BGR2RGB*/CV_GRAY2RGB);
		cvReleaseMat(&vdisp);
	}

	mData->getDepthMap().unlockData();

	if(mDepthImage&&!mPixbuf)
	{
		mPixbuf = gdk_pixbuf_new_from_data ((guchar*)mDepthImage->imageData, GDK_COLORSPACE_RGB, FALSE, mDepthImage->depth, mDepthImage->width, mDepthImage->height, mDepthImage->widthStep, NULL, NULL);
	}

	gtk_widget_queue_draw(GTK_WIDGET(mDepthMapDrawingArea));

	gdk_threads_leave();

}

DataContainer* DepthMap::getData()
{
	return mData;
}

void DepthMap::initialize(GtkSpinButton* depthMap3DFPS, GtkDrawingArea* depthMapDrawingArea, DataContainer* data)
{
	mSpinButtonFPS			= depthMap3DFPS;
	mData					= data;
	mDepthMapDrawingArea	= depthMapDrawingArea;

}

std::auto_ptr<DepthMap> DepthMap::mInstance;

DepthMap* DepthMap::getInstance()
{
	if(mInstance.get()==NULL)
	{
		mInstance.reset(new DepthMap());
	}
	return mInstance.get();
}

void DepthMap::start3D(GtkButton *button, gpointer   user_data)
{
	if(DepthMap::getInstance()->getData()->getCalibrateX1().getPtr()!=NULL&&DepthMap::getInstance()->getData()->getCalibrateY1().getPtr()!=NULL&&DepthMap::getInstance()->getData()->getCalibrateX2().getPtr()!=NULL&&DepthMap::getInstance()->getData()->getCalibrateY2().getPtr()!=NULL)
	{
		DepthMap::getInstance()->getData()->getStart3D().lockData();

		if(!*(DepthMap::getInstance()->getData()->getStart3D().getPtr()))
		{
			ThreadManager::addThread(DepthMap::getInstance());
			ThreadManager::addThread(CalculateDepthMap::getInstance());
			ThreadManager::startThread(DepthMap::getInstance(), 0);
			ThreadManager::startThread(CalculateDepthMap::getInstance(),90);
			*(DepthMap::getInstance()->getData()->getStart3D().getPtr())		= true;
		}

		DepthMap::getInstance()->getData()->getStart3D().unlockData();

	}
}

void DepthMap::stop3D(GtkButton *button, gpointer   user_data)
{
	ThreadManager::endNowThread(DepthMap::getInstance());
	ThreadManager::endNowThread(CalculateDepthMap::getInstance());
	DepthMap::getInstance()->getData()->getStart3D().lockData();
	*(DepthMap::getInstance()->getData()->getStart3D().getPtr())		= false;
	DepthMap::getInstance()->getData()->getStart3D().unlockData();
}

gboolean DepthMap::draw3DCallback(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
	if(mInstance->mPixbuf==NULL)
	{
		return FALSE;
	}

	GdkPixbuf* sPixbuf		= gdk_pixbuf_scale_simple(mInstance->mPixbuf, widget->allocation.width, widget->allocation.height, GDK_INTERP_NEAREST);
	gdk_draw_pixbuf ( widget->window , widget->style->fg_gc[gtk_widget_get_state (widget)] , sPixbuf, 0, 0, 0, 0, widget->allocation.width, widget->allocation.height, GDK_RGB_DITHER_NORMAL, 0, 0 );
	if(sPixbuf!=NULL)
	{
		gdk_pixbuf_unref(sPixbuf);
	}

	return TRUE;
}

CalculateDepthMap::CalculateDepthMap()
{
	mFPS				= 1000/30;
	mSpinButtonFPS		= NULL;
}

CalculateDepthMap::CalculateDepthMap(const CalculateDepthMap& obj)
{
	mFPS				= 1000/30;
	mSpinButtonFPS		= NULL;
}

CalculateDepthMap::~CalculateDepthMap()
{

}

void CalculateDepthMap::run()
{
	mRun		= true;

	if(mSpinButtonFPS!=NULL)
	{
		mFPS			= 1000/gtk_spin_button_get_value_as_int(mSpinButtonFPS);
	}

	clock_t lastTime			= clock() / (CLOCKS_PER_SEC / 1000);
	clock_t currentTime;

	while(mRun)
	{

		printf("---------------- DepthMap calculate \n");

		calculateDepthMap();

		currentTime		= clock() / (CLOCKS_PER_SEC / 1000);
		if(currentTime-lastTime<mFPS) {
			sleep((mFPS-(currentTime-lastTime)));
		}
		lastTime		= clock() / (CLOCKS_PER_SEC / 1000);
	}

}

void CalculateDepthMap::end()
{
	mRun		= false;
}

DataContainer* CalculateDepthMap::getData()
{
	return mData;
}

void CalculateDepthMap::calculateDepthMap() {

	IplImage *tempGrayLeft		= NULL;
	IplImage *tempGrayRight		= NULL;

	CvMat *x1, *x2, *y1, *y2;

	mData->getCalibrateX1().lockData();
	x1		= mData->getCalibrateX1().getPtr();
	mData->getCalibrateX1().unlockData();

	mData->getCalibrateX2().lockData();
	x2		= mData->getCalibrateX2().getPtr();
	mData->getCalibrateX2().unlockData();

	mData->getCalibrateY1().lockData();
	y1		= mData->getCalibrateY1().getPtr();
	mData->getCalibrateY1().unlockData();

	mData->getCalibrateY2().lockData();
	y2		= mData->getCalibrateY2().getPtr();
	mData->getCalibrateY2().unlockData();

	if(mData->getImageLeftGrayRef().tryLockData())
	{
		tempGrayLeft		= cvCreateImage(cvSize(mData->getImageLeftGrayRef()->width,mData->getImageLeftGrayRef()->height),mData->getImageLeftGrayRef()->depth,mData->getImageLeftGrayRef()->nChannels);
		cvCopy(mData->getImageLeftGrayRef().getPtr(),tempGrayLeft);
		mData->getImageLeftGrayRef().unlockData();
	}

	if(mData->getImageRightGrayRef().tryLockData())
	{
		tempGrayRight		= cvCreateImage(cvSize(mData->getImageRightGrayRef()->width,mData->getImageRightGrayRef()->height),mData->getImageRightGrayRef()->depth,mData->getImageRightGrayRef()->nChannels);
		cvCopy(mData->getImageRightGrayRef().getPtr(),tempGrayRight);
		mData->getImageRightGrayRef().unlockData();
	}

	if(tempGrayLeft!=NULL&&tempGrayRight!=NULL)
	{

		CvStereoBMState *BMState = cvCreateStereoBMState();
		if(BMState!=0) {

			CvMat* imgLeft 		= cvCreateMat( IMAGE_HEIGHT, IMAGE_WIDTH, CV_8U );
			CvMat* imgRight 	= cvCreateMat( IMAGE_HEIGHT, IMAGE_WIDTH, CV_8U );
			CvMat* disp 		= cvCreateMat( IMAGE_HEIGHT, IMAGE_WIDTH, CV_16S );
			//CvMat* vdisp 		= cvCreateMat( IMAGE_HEIGHT, IMAGE_WIDTH, CV_8U );

			cvRemap( tempGrayLeft, imgLeft, x1, y1 );
			cvRemap( tempGrayRight, imgRight, x2, y2 );


			BMState->preFilterSize		= 41;
			BMState->preFilterCap		= 31;
			BMState->SADWindowSize		= 41;
			BMState->minDisparity		= -64;
			BMState->numberOfDisparities= 128;
			BMState->textureThreshold	= 10;
			BMState->uniquenessRatio	= 15;


			// Orginal

			/*
			BMState->preFilterSize			= 31;
			BMState->preFilterCap			= 31;
			BMState->SADWindowSize			= 41;//255;
			BMState->minDisparity			= -192;
			BMState->numberOfDisparities	= 192;
			BMState->textureThreshold		= 10;
			BMState->uniquenessRatio		= 15;
			*/

			bool isVerticalStereo		= false;
			int useUncalibrated			= 2;

			if( !isVerticalStereo || useUncalibrated != 0 )
			{
				cvFindStereoCorrespondenceBM( imgLeft, imgRight, disp, BMState);

				// hack to get rid of small-scale noise
				cvErode(disp, disp, NULL, 2);
				cvDilate(disp, disp, NULL, 2);

				// show in 3D

				//CvMat* xyz = cvCreateMat(disp->rows, disp->cols, CV_32FC3);
				//CvMat* dispn = cvCreateMat( IMAGE_HEIGHT, IMAGE_WIDTH, CV_32F );

				//CvMat Q;
				//cvInitMatHeader(&Q,4,4,CV_32FC1,_Q);

				//cvConvertScale(disp, dispn, 1.0/16);

				//cvReprojectImageTo3D(dispn, xyz, &Q);

				//cvShowImage(WINDOW_3D,xyz);

				//cvReleaseMat(&xyz);
				//cvReleaseMat(&dispn);

			}

			if(imgLeft!=NULL) {
				cvReleaseMat(&imgLeft);
			}

			if(imgRight!=NULL) {
				cvReleaseMat(&imgRight);
			}

			mData->getDepthMap().lockData();
			CvMat* tempPtr		= mData->getDepthMap().getPtr();
			if(tempPtr!=NULL)
			{
				cvReleaseMat(&tempPtr);
			}
			mData->getDepthMap().setPtr(disp);
			mData->getDepthMap().unlockData();


			/*
			if(svTemp->imgLeft!=NULL) {
				cvReleaseMat(&svTemp->imgLeft);
				svTemp->imgLeft		= NULL;
			}

			if(svTemp->imgRight!=NULL) {
				cvReleaseMat(&svTemp->imgRight);
				svTemp->imgRight	= NULL;
			}

			if(svTemp->disp!=NULL) {
				cvReleaseMat(&svTemp->disp);
				svTemp->disp	= NULL;
			}

			svTemp->imgLeft		= imgLeft;
			svTemp->imgRight	= imgRight;
			svTemp->disp		= disp;
			*/

			cvReleaseStereoBMState(&BMState);
		}


	}

	if(tempGrayLeft)
	{
		cvReleaseData(tempGrayLeft);
		cvReleaseImage(&tempGrayLeft);
	}

	if(tempGrayRight)
	{
		cvReleaseData(tempGrayRight);
		cvReleaseImage(&tempGrayRight);
	}

}

std::auto_ptr<CalculateDepthMap> CalculateDepthMap::mInstance;

CalculateDepthMap* CalculateDepthMap::getInstance()
{
	if(mInstance.get()==NULL)
	{
		mInstance.reset(new CalculateDepthMap());
	}
	return mInstance.get();
}

void CalculateDepthMap::initialize(GtkSpinButton* depthMap3DFPS, DataContainer* data)
{
	mSpinButtonFPS		= depthMap3DFPS;
	mData				= data;

}

Render3D::Render3D()
{
	mRun		= true;
}

Render3D::Render3D(const Render3D& obj)
{
	mRun		= true;
}

Render3D::~Render3D()
{

}

void Render3D::run()
{
	mRun		= true;

	if(mSpinButtonFPS!=NULL)
	{
		mFPS			= 1000/gtk_spin_button_get_value_as_int(mSpinButtonFPS);
	}

	clock_t lastTime			= clock() / (CLOCKS_PER_SEC / 1000);
	clock_t currentTime;

	while(mRun)
	{

		printf("---------------- Render 3D \n");

		currentTime		= clock() / (CLOCKS_PER_SEC / 1000);
		if(currentTime-lastTime<mFPS) {
			sleep((mFPS-(currentTime-lastTime)));
		}
		lastTime		= clock() / (CLOCKS_PER_SEC / 1000);
	}

}

void Render3D::end()
{
	mRun		= false;
}

std::auto_ptr<Render3D> Render3D::mInstance;

Render3D* Render3D::getInstance()
{
	if(mInstance.get()==NULL)
	{
		mInstance.reset(new Render3D());
	}
	return mInstance.get();
}

void Render3D::initialize(GtkSpinButton* render3DFPS, DataContainer* data)
{
	mSpinButtonFPS		= render3DFPS;
	mData				= data;

}


void Display3D::initializeDisplay3DModule(GtkButton* start3D, GtkButton* stop3D, GtkDrawingArea *depthMap, GtkDrawingArea *view3D, GtkSpinButton* calculate3DFPS, GtkSpinButton* view3DFPS, DataContainer* data)
{
	if(start3D!=NULL)
	{
		g_signal_connect (G_OBJECT (start3D), "clicked", G_CALLBACK (DepthMap::start3D), NULL);
	}

	if(stop3D!=NULL)
	{
		g_signal_connect (G_OBJECT (stop3D), "clicked", G_CALLBACK (DepthMap::stop3D), NULL);
	}

	if(depthMap!=NULL)
	{
		g_signal_connect (G_OBJECT (depthMap), "expose_event", G_CALLBACK (DepthMap::draw3DCallback), NULL);
	}

	DepthMap::getInstance()->initialize(view3DFPS, depthMap, data);
	CalculateDepthMap::getInstance()->initialize(calculate3DFPS, data);
	Render3D::getInstance()->initialize(view3DFPS, data);

}

