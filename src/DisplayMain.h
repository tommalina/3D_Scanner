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

class DisplayMain : public ThreadRunnable
{
private:

	bool				mRun;
	unsigned int		mFPS;
	GtkWidget*			mDACamera1;
	GtkDrawingArea*		mDACamera2;
	Camera*				mCamera1;
	Camera*				mCamera2;
	IplImage* 			mImage;

	void 				display();
	static gboolean		displayCallback(GtkWidget *object, GdkEventWindowState *event,  gpointer user_data);

public:

	DisplayMain(unsigned int fps, GtkWidget *DACamera1, GtkDrawingArea *DACamera2);

	virtual ~DisplayMain();

	void run();

	void end();

};

#endif /* DISPLAYMAIN_H_ */
