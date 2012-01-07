/*
 * DisplayMain.h
 *
 *  Created on: 28-11-2011
 *      Author: ble
 */

#ifndef DISPLAYMAIN_H_
#define DISPLAYMAIN_H_

#include "ThreadManager.h"
#include "Scanner3D.h"
#include "Camera.h"

#include <glibmm.h>
#include <gtkmm.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

/**
 * Class used to display two images from camera.
 */

class CameraGdkDisplay : public ThreadRunnable
{
private:

	/**
	 * If true then thread which dispaly images is runnig.
	 */
	bool						mRun;

	/**
	 * Amount of frames in one second.
	 */
	unsigned int				mFPS;

	/**
	 * Pointer to camera 1 drawing area.
	 */
	GtkDrawingArea*				mDrawingArea1;

	/**
	 * Pointer to camera 2 drawing area.
	 */
	GtkDrawingArea*				mDrawingArea2;

	/**
	 * Pointer to spin button of first camera.
	 */
	GtkSpinButton*				mSpinButton1;

	/**
	 * Pointer to spin button of second camera.
	 */
	GtkSpinButton*				mSpinButton2;

	/**
	 * Pointer to instance of this object.
	 */
	static CameraGdkDisplay*	mInstance;

	/**
	 * Main method which get images and convert it to display in gtk window.
	 */
	void 						display();

	/**
	 * "Camera 1" drawing area callback method.
	 */
	static gboolean 			drawCallback1(GtkWidget *widget, GdkEventExpose *event, gpointer data);

	/**
	 * "Camera 2" drawing area callback method.
	 */
	static gboolean 			drawCallback2(GtkWidget *widget, GdkEventExpose *event, gpointer data);

	/**
	 * Spin button change callback method.
	 */
	static void					changeCallback1(GtkSpinButton *spinbutton, gpointer data);

	/**
	 * Spin button change callback metohd.
	 */
	static void					changeCallback2(GtkSpinButton *spinbutton, gpointer data);

public:

	Camera*						mCamera1;
	Camera*						mCamera2;
	IplImage*					mImage1;
	IplImage*					mImage2;
	GdkPixbuf*					mPixbuf1;
	GdkPixbuf*					mPixbuf2;
	int							mCameraId1;
	int							mCameraId2;
	boost::mutex				mMutex;

	CameraGdkDisplay(unsigned int fps, GtkDrawingArea* drawingArea1, GtkDrawingArea* drawingArea2, GtkSpinButton* spinButton1, GtkSpinButton* spinButton2);

	virtual ~CameraGdkDisplay();

	void run();

	void end();

};

#endif /* DISPLAYMAIN_H_ */
