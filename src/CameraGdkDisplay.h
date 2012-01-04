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

class CameraGdkDisplay : public ThreadRunnable
{
private:

	bool						mRun;
	unsigned int				mFPS;
	GtkDrawingArea*				mDrawingArea1;
	GtkDrawingArea*				mDrawingArea2;
	GtkSpinButton*				mSpinButton1;
	GtkSpinButton*				mSpinButton2;

	static CameraGdkDisplay*	mInstance;

	void 						display();

	static gboolean 			drawCallback1(GtkWidget *widget, GdkEventExpose *event, gpointer data);
	static gboolean 			drawCallback2(GtkWidget *widget, GdkEventExpose *event, gpointer data);
	static void					changeCallback1(GtkSpinButton *spinbutton, gpointer data);
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
