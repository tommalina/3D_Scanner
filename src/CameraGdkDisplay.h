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
	Camera*						mCamera1;
	Camera*						mCamera2;
	GtkDrawingArea*				mDrawingArea1;
	GtkDrawingArea*				mDrawingArea2;

	void 						display();

	static gboolean 			drawCallback(GtkWidget *widget, GdkEventExpose *event, gpointer data);

public:

	IplImage*					mImage1;
	IplImage*					mImage2;
	GdkPixbuf*					mPixbuf1;
	GdkPixbuf*					mPixbuf2;

	static CameraGdkDisplay*	mInstance;

	CameraGdkDisplay(unsigned int fps, GtkDrawingArea* drawingArea1, GtkDrawingArea* drawingArea2);

	virtual ~CameraGdkDisplay();

	void run();

	void end();

};

#endif /* DISPLAYMAIN_H_ */
