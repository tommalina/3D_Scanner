/*
 * Display3D.cpp
 *
 *  Created on: 21-01-2012
 *      Author: ble
 */

#include <memory.h>
#include <stdio.h>
#include <unistd.h>

#include "Display3D.h"
#include "ThreadManager.h"
#include "Camera.h"

class DepthMap : public ThreadRunnable
{
private:

	static std::auto_ptr<DepthMap>				mInstance;

	bool										mRun;
	int											mFPS;
	DataContainer*								mData;
	GtkSpinButton*								mSpinButtonFPS;

	DepthMap();

	DepthMap(const DepthMap& obj);

public:

	virtual 									~DepthMap();

	void										run();

	void										end();

	void										calculateDisplayDepthMap();

	void										initialize(GtkSpinButton* depthMap3DFPS, DataContainer* data);

	DataContainer*								getData();

	static DepthMap*							getInstance();

	static void 								start3D(GtkButton *button, gpointer   user_data);

	static void									stop3D(GtkButton *button, gpointer   user_data);

	static gboolean 							draw3DCallback(GtkWidget *widget, GdkEventExpose *event, gpointer data);

};

DepthMap::DepthMap()
{
	mFPS				= 30;
	mSpinButtonFPS		= NULL;
}

DepthMap::DepthMap(const DepthMap& obj)
{
	mFPS				= 30;
	mSpinButtonFPS		= NULL;
}

DepthMap::~DepthMap()
{

}

void DepthMap::run()
{
	mRun		= true;

	if(mSpinButtonFPS!=NULL)
	{
		mFPS			= gtk_spin_button_get_value_as_int(mSpinButtonFPS);
	}

	clock_t lastTime			= clock() / (CLOCKS_PER_SEC / 1000);
	clock_t currentTime;

	while(mRun)
	{

		calculateDisplayDepthMap();

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

DataContainer* DepthMap::getData()
{
	return mData;
}


void DepthMap::calculateDisplayDepthMap() {

}

void DepthMap::initialize(GtkSpinButton* depthMap3DFPS, DataContainer* data)
{
	mSpinButtonFPS		= depthMap3DFPS;
	mData				= data;

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
		bool canStart		= false;
		DepthMap::getInstance()->getData()->getStart3D().lockData();
		canStart		= *(DepthMap::getInstance()->getData()->getStart3D().getPtr());
		DepthMap::getInstance()->getData()->getStart3D().unlockData();

		if(canStart)
		{
			ThreadManager::addThread(DepthMap::getInstance());
			ThreadManager::startThread(DepthMap::getInstance(), 90);
		}

	}
}

void DepthMap::stop3D(GtkButton *button, gpointer   user_data)
{
	ThreadManager::endNowThread(DepthMap::getInstance());
	DepthMap::getInstance()->getData()->getStart3D().lockData();
	*(DepthMap::getInstance()->getData()->getStart3D().getPtr())		= false;
	DepthMap::getInstance()->getData()->getStart3D().unlockData();
}

gboolean DepthMap::draw3DCallback(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{

	return TRUE;
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

	DepthMap::getInstance()->initialize(calculate3DFPS, data);

}

