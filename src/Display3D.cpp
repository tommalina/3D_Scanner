/*
 * Display3D.cpp
 *
 *  Created on: 21-01-2012
 *      Author: ble
 */

#include <memory.h>
#include <stdio.h>
#include <unistd.h>

#include <GL/gl.h>
#include <gtkglmm.h>
#include <gdk/gdkgl.h>
#include <gtk/gtkgl.h>

#include "Display3D.h"
#include "ThreadManager.h"
#include "Camera.h"

//TODO: Change image size.
#define IMAGE_WIDTH		640
#define IMAGE_HEIGHT	320

#define ALPHA 0.5

class DepthMap: public ThreadRunnable {
private:

	static std::auto_ptr<DepthMap> mInstance;

	bool mRun;

	int mFPS;

	DataContainer* mData;

	GtkSpinButton* mSpinButtonFPS;

	GtkDrawingArea* mDepthMapDrawingArea;

	DepthMap();

	DepthMap(const DepthMap& obj);

public:

	IplImage* mDepthImage;

	GdkPixbuf* mPixbuf;

	virtual ~DepthMap();

	void run();

	void end();

	void displayDepthMap();

	void initialize(GtkSpinButton* depthMap3DFPS,
			GtkDrawingArea* depthMapDrawingArea, DataContainer* data);

	DataContainer* getData();

	static DepthMap* getInstance();

	static void start3D(GtkButton *button, gpointer user_data);

	static void stop3D(GtkButton *button, gpointer user_data);

	static gboolean draw3DCallback(GtkWidget *widget, GdkEventExpose *event,
			gpointer data);

};

class CalculateDepthMap: public ThreadRunnable {
private:

	static std::auto_ptr<CalculateDepthMap> mInstance;

	bool mRun;
	int mFPS;
	DataContainer* mData;
	GtkSpinButton* mSpinButtonFPS;

	CalculateDepthMap();

	CalculateDepthMap(const CalculateDepthMap& obj);

public:

	virtual ~CalculateDepthMap();

	void run();

	void end();

	void calculateDepthMap();

	void initialize(GtkSpinButton* depthMap3DFPS, DataContainer* data);

	DataContainer* getData();

	static CalculateDepthMap* getInstance();

};

class Render3D: public ThreadRunnable {
private:

	static std::auto_ptr<Render3D> mInstance;

	bool mRun;
	int mFPS;
	DataContainer* mData;
	GtkSpinButton* mSpinButtonFPS;
	GtkHBox* mDrawingArea;

	Render3D();

	Render3D(const Render3D& obj);

public:

	int mMapWidth;
	int mMapHeight;
	float mAngle;
	float** mMap;
	void createMap(int width, int height, CvMat* map);

	virtual ~Render3D();

	void run();

	void end();

	void initialize(GtkHBox* scene3D, GtkSpinButton* render3DFPS,
			DataContainer* data);

	DataContainer* getData();

	static Render3D* getInstance();

	static gboolean glConfigure(GtkWidget *da, GdkEventConfigure *event,
			gpointer user_data);

	static gboolean glExpose(GtkWidget *da, GdkEventExpose *event,
			gpointer user_data);

};

DepthMap::DepthMap() {
	mFPS = 1000 / 30;
	mSpinButtonFPS = NULL;
	mDepthImage = NULL;
	mPixbuf = NULL;
	mDepthMapDrawingArea = NULL;
}

DepthMap::DepthMap(const DepthMap& obj) {
	mFPS = 1000 / 30;
	mSpinButtonFPS = NULL;
	mDepthImage = NULL;
	mPixbuf = NULL;
	mDepthMapDrawingArea = NULL;
}

DepthMap::~DepthMap() {
	if (mDepthImage != NULL)
	{
		cvReleaseData(mDepthImage);
		cvReleaseImage(&mDepthImage);
		mPixbuf = NULL;
	}
}

void DepthMap::run() {
	mRun = true;

	if (mSpinButtonFPS != NULL)
	{
		mFPS = 1000 / gtk_spin_button_get_value_as_int(mSpinButtonFPS);
	}

	clock_t lastTime = clock() / (CLOCKS_PER_SEC / 1000);
	clock_t currentTime;

	while (mRun) {

		printf("---------------- DepthMap display \n");

		displayDepthMap();

		currentTime = clock() / (CLOCKS_PER_SEC / 1000);
		if (currentTime - lastTime < mFPS) {
			sleep((mFPS - (currentTime - lastTime)));
		}
		lastTime = clock() / (CLOCKS_PER_SEC / 1000);
	}

}

void DepthMap::end() {
	mRun = false;
}

void DepthMap::displayDepthMap() {

	gdk_threads_enter();

	mData->getDepthMap().lockData();

	if (mData->getDepthMap().getPtr() != NULL)
	{

		if (!mDepthImage) {
			mDepthImage = cvCreateImage(cvSize(IMAGE_WIDTH, IMAGE_HEIGHT),
					IPL_DEPTH_8U, 3);
		}

		CvMat* vdisp = cvCreateMat(IMAGE_HEIGHT, IMAGE_WIDTH, CV_8U);
		cvNormalize(mData->getDepthMap().getPtr(), vdisp, 0, 256, CV_MINMAX);
		cvCvtColor(vdisp, mDepthImage, /*CV_BGR2RGB*/CV_GRAY2RGB);
		cvReleaseMat(&vdisp);
	}

	mData->getDepthMap().unlockData();

	if (mDepthImage && !mPixbuf) {
		mPixbuf = gdk_pixbuf_new_from_data((guchar*) mDepthImage->imageData,
				GDK_COLORSPACE_RGB, FALSE, mDepthImage->depth,
				mDepthImage->width, mDepthImage->height, mDepthImage->widthStep,
				NULL, NULL);
	}

	gtk_widget_queue_draw(GTK_WIDGET(mDepthMapDrawingArea));

	gdk_threads_leave();

}

DataContainer* DepthMap::getData() {
	return mData;
}

void DepthMap::initialize(GtkSpinButton* depthMap3DFPS,
		GtkDrawingArea* depthMapDrawingArea, DataContainer* data) {
	mSpinButtonFPS = depthMap3DFPS;
	mData = data;
	mDepthMapDrawingArea = depthMapDrawingArea;

}

std::auto_ptr<DepthMap> DepthMap::mInstance;

DepthMap* DepthMap::getInstance() {
	if (mInstance.get() == NULL)
	{
		mInstance.reset(new DepthMap());
	}
	return mInstance.get();
}

void DepthMap::start3D(GtkButton *button, gpointer user_data) {
	if (DepthMap::getInstance()->getData()->getCalibrateX1().getPtr() != NULL
			&& DepthMap::getInstance()->getData()->getCalibrateY1().getPtr()
					!= NULL
			&& DepthMap::getInstance()->getData()->getCalibrateX2().getPtr()
					!= NULL
			&& DepthMap::getInstance()->getData()->getCalibrateY2().getPtr()
					!= NULL)
			{
		DepthMap::getInstance()->getData()->getStart3D().lockData();

		if (!*(DepthMap::getInstance()->getData()->getStart3D().getPtr())) {
			ThreadManager::addThread(DepthMap::getInstance());
			ThreadManager::addThread(CalculateDepthMap::getInstance());
			ThreadManager::startThread(DepthMap::getInstance(), 0);
			ThreadManager::startThread(CalculateDepthMap::getInstance(), 90);
			ThreadManager::addThread(Render3D::getInstance());
			ThreadManager::startThread(Render3D::getInstance(),90);
			*(DepthMap::getInstance()->getData()->getStart3D().getPtr()) = true;
		}

		DepthMap::getInstance()->getData()->getStart3D().unlockData();

	}

	ThreadManager::addThread(Render3D::getInstance());
	ThreadManager::startThread(Render3D::getInstance(),90);

}

void DepthMap::stop3D(GtkButton *button, gpointer user_data) {
	ThreadManager::endNowThread(DepthMap::getInstance());
	ThreadManager::endNowThread(CalculateDepthMap::getInstance());
	ThreadManager::endNowThread(Render3D::getInstance());
	DepthMap::getInstance()->getData()->getStart3D().lockData();
	*(DepthMap::getInstance()->getData()->getStart3D().getPtr()) = false;
	DepthMap::getInstance()->getData()->getStart3D().unlockData();
}

gboolean DepthMap::draw3DCallback(GtkWidget *widget, GdkEventExpose *event,
		gpointer data) {
	if (mInstance->mPixbuf == NULL)
	{
		return FALSE;
	}

	GdkPixbuf* sPixbuf = gdk_pixbuf_scale_simple(mInstance->mPixbuf,
			widget->allocation.width, widget->allocation.height,
			GDK_INTERP_NEAREST);
	gdk_draw_pixbuf(widget->window,
			widget->style->fg_gc[gtk_widget_get_state(widget)], sPixbuf, 0, 0,
			0, 0, widget->allocation.width, widget->allocation.height,
			GDK_RGB_DITHER_NORMAL, 0, 0);
	if (sPixbuf != NULL)
	{
		gdk_pixbuf_unref(sPixbuf);
	}

	return TRUE;
}

CalculateDepthMap::CalculateDepthMap() {
	mFPS = 1000 / 30;
	mSpinButtonFPS = NULL;
}

CalculateDepthMap::CalculateDepthMap(const CalculateDepthMap& obj) {
	mFPS = 1000 / 30;
	mSpinButtonFPS = NULL;
}

CalculateDepthMap::~CalculateDepthMap() {

}

void CalculateDepthMap::run() {
	mRun = true;

	if (mSpinButtonFPS != NULL)
	{
		mFPS = 1000 / gtk_spin_button_get_value_as_int(mSpinButtonFPS);
	}

	clock_t lastTime = clock() / (CLOCKS_PER_SEC / 1000);
	clock_t currentTime;

	while (mRun) {

		calculateDepthMap();

		currentTime = clock() / (CLOCKS_PER_SEC / 1000);
		if (currentTime - lastTime < mFPS) {
			sleep((mFPS - (currentTime - lastTime)));
		}
		lastTime = clock() / (CLOCKS_PER_SEC / 1000);
	}

}

void CalculateDepthMap::end() {
	mRun = false;
}

DataContainer* CalculateDepthMap::getData() {
	return mData;
}

void CalculateDepthMap::calculateDepthMap() {

	IplImage *tempGrayLeft = NULL;
	IplImage *tempGrayRight = NULL;

	CvMat *x1, *x2, *y1, *y2;

	mData->getCalibrateX1().lockData();
	x1 = mData->getCalibrateX1().getPtr();
	mData->getCalibrateX1().unlockData();

	mData->getCalibrateX2().lockData();
	x2 = mData->getCalibrateX2().getPtr();
	mData->getCalibrateX2().unlockData();

	mData->getCalibrateY1().lockData();
	y1 = mData->getCalibrateY1().getPtr();
	mData->getCalibrateY1().unlockData();

	mData->getCalibrateY2().lockData();
	y2 = mData->getCalibrateY2().getPtr();
	mData->getCalibrateY2().unlockData();

	if (mData->getImageLeftGrayRef().tryLockData()) {
		tempGrayLeft = cvCreateImage(
				cvSize(mData->getImageLeftGrayRef()->width,
						mData->getImageLeftGrayRef()->height),
				mData->getImageLeftGrayRef()->depth,
				mData->getImageLeftGrayRef()->nChannels);
		cvCopy(mData->getImageLeftGrayRef().getPtr(), tempGrayLeft);
		mData->getImageLeftGrayRef().unlockData();
	}

	if (mData->getImageRightGrayRef().tryLockData()) {
		tempGrayRight = cvCreateImage(
				cvSize(mData->getImageRightGrayRef()->width,
						mData->getImageRightGrayRef()->height),
				mData->getImageRightGrayRef()->depth,
				mData->getImageRightGrayRef()->nChannels);
		cvCopy(mData->getImageRightGrayRef().getPtr(), tempGrayRight);
		mData->getImageRightGrayRef().unlockData();
	}

	if (tempGrayLeft != NULL && tempGrayRight != NULL)
	{

		CvStereoBMState *BMState = cvCreateStereoBMState();
		if (BMState != 0) {

			CvMat* imgLeft = cvCreateMat(IMAGE_HEIGHT, IMAGE_WIDTH, CV_8U);
			CvMat* imgRight = cvCreateMat(IMAGE_HEIGHT, IMAGE_WIDTH, CV_8U);
			CvMat* disp = cvCreateMat(IMAGE_HEIGHT, IMAGE_WIDTH, CV_16S);
			//CvMat* vdisp 		= cvCreateMat( IMAGE_HEIGHT, IMAGE_WIDTH, CV_8U );

			cvRemap(tempGrayLeft, imgLeft, x1, y1);
			cvRemap(tempGrayRight, imgRight, x2, y2);

			BMState->preFilterSize = 41;
			BMState->preFilterCap = 31;
			BMState->SADWindowSize = 41;
			BMState->minDisparity = -64;
			BMState->numberOfDisparities = 128;
			BMState->textureThreshold = 10;
			BMState->uniquenessRatio = 15;

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

			bool isVerticalStereo = false;
			int useUncalibrated = 2;

			if (!isVerticalStereo || useUncalibrated != 0) {
				cvFindStereoCorrespondenceBM(imgLeft, imgRight, disp, BMState);

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

			if (imgLeft != NULL) {
				cvReleaseMat(&imgLeft);
			}

			if (imgRight != NULL) {
				cvReleaseMat(&imgRight);
			}

			mData->getDepthMap().lockData();
			CvMat* tempPtr = mData->getDepthMap().getPtr();
			if (tempPtr != NULL)
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

	if (tempGrayLeft) {
		cvReleaseData(tempGrayLeft);
		cvReleaseImage(&tempGrayLeft);
	}

	if (tempGrayRight) {
		cvReleaseData(tempGrayRight);
		cvReleaseImage(&tempGrayRight);
	}

}

std::auto_ptr<CalculateDepthMap> CalculateDepthMap::mInstance;

CalculateDepthMap* CalculateDepthMap::getInstance() {
	if (mInstance.get() == NULL)
	{
		mInstance.reset(new CalculateDepthMap());
	}
	return mInstance.get();
}

void CalculateDepthMap::initialize(GtkSpinButton* depthMap3DFPS,
		DataContainer* data) {
	mSpinButtonFPS = depthMap3DFPS;
	mData = data;

}

Render3D::Render3D() {
	mRun = true;
	mMap = NULL;
	mMapWidth = 0;
	mMapHeight = 0;
}

Render3D::Render3D(const Render3D& obj) {
	mRun = true;
	mMap = NULL;
	mMapWidth = 0;
	mMapHeight = 0;
}

Render3D::~Render3D() {
	if (mMap != NULL)
	{
		for (int i = 0; i < mMapHeight; ++i) {
			delete[] mMap[i];
		}
		delete[] mMap;
	}
}

void Render3D::run() {
	mRun = true;

	gdk_threads_enter();

	gtk_widget_realize(GTK_WIDGET(mDrawingArea));

	gdk_threads_leave();

	if (mSpinButtonFPS != NULL)
	{
		mFPS = 1000 / gtk_spin_button_get_value_as_int(mSpinButtonFPS);
	}

	clock_t lastTime = clock() / (CLOCKS_PER_SEC / 1000);
	clock_t currentTime;


	while (mRun) {

		if(mData->getDepthMap().tryLockData())
		{
			createMap(IMAGE_WIDTH, IMAGE_HEIGHT, mData->getDepthMap().getPtr());
			mData->getDepthMap().unlockData();
		}

		gdk_threads_enter();

		gtk_widget_queue_draw(GTK_WIDGET(mDrawingArea));

		gdk_threads_leave();

		currentTime = clock() / (CLOCKS_PER_SEC / 1000);
		if (currentTime - lastTime < mFPS) {
			sleep((mFPS - (currentTime - lastTime)));
		}
		lastTime = clock() / (CLOCKS_PER_SEC / 1000);
	}

}

void Render3D::end() {
	mRun = false;
}

void Render3D::createMap(int width, int height, CvMat* map) {

	//if(map==NULL) return;

	if (width != 0 && height != 0) {

		if (mMapWidth != width || mMapHeight != height) {
			if (mMap != NULL)
			{
				for (int i = 0; i < mMapHeight; ++i) {
					delete[] mMap[i];
				}
				delete[] mMap;
			}

			mMapWidth = width;
			mMapHeight = height;

			mMap = new float*[mMapHeight];
			for (int i = 0; i < mMapHeight; ++i) {
				mMap[i] = new float[mMapWidth];
			}

		}

		if (mMap != NULL)
		{
			for (int x = 0; x < mMapWidth; x++) {
				for (int y = 0; y < mMapHeight; y++) {

					if(x<mMapWidth/2&&y<mMapHeight/2)
					{
						mMap[y][x] = (rand() % 500);
					}
					else
					{
						mMap[y][x] = 0;
					}

					//CvScalar s;
					//s = cvGet2D(map,y,x);
					//mMap[y][x] = s.val[0]+s.val[1]+s.val[2];
				}
			}

			/*
			// Smooth terrain (reduce jaggedness)
			for (int n = 0; n < 2; n++) {
				for (int x = 0; x < mMapWidth; x++) {
					for (int y = 0; y < mMapHeight; y++) {
						for (int a = -1; a < 2; a++) {
							for (int b = -1; b < 2; b++) {
								if (x + a >= 0 && x + a < mMapWidth
										&& y + b >= 0 && y + b < mMapHeight) {
									mMap[y][x] += mMap[y + b][x + a];
								}
							}
						}
						mMap[y][x] *= .075;
					}
				}
			}
			*/

		}

	}
}

std::auto_ptr<Render3D> Render3D::mInstance;

Render3D* Render3D::getInstance() {
	if (mInstance.get() == NULL)
	{
		mInstance.reset(new Render3D());
	}
	return mInstance.get();
}

gboolean Render3D::glConfigure(GtkWidget *da, GdkEventConfigure *event,
		gpointer user_data) {

	GdkGLContext *glcontext = gtk_widget_get_gl_context(da);
	GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable (da);

	if (!gdk_gl_drawable_gl_begin(gldrawable, glcontext)) {
		g_assert_not_reached ();
	}

	glLoadIdentity();
	glViewport(0, 0, da->allocation.width, da->allocation.height);
	glOrtho(-500, 500, -500, 500, -20050, 10000);
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	const float bgColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);
	// Setup fog
	//glEnable(GL_FOG);
	//glFogi(GL_FOG_MODE, GL_EXP2);
	//glFogf(GL_FOG_DENSITY, .02);
	//glFogfv(GL_FOG_COLOR, bgColor);

	glEnable(GL_NORMALIZE);

	// Setup Lights
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glShadeModel(GL_SMOOTH);

	// Setup Materials
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial( GL_FRONT, GL_AMBIENT );

	//glScalef(10., 10., 10.);

	gdk_gl_drawable_gl_end(gldrawable);

	return TRUE;
}

gboolean Render3D::glExpose(GtkWidget *da, GdkEventExpose *event,
		gpointer user_data) {

	GdkGLContext *glcontext = gtk_widget_get_gl_context(da);
	GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable (da);

	if (!gdk_gl_drawable_gl_begin(gldrawable, glcontext)) {
		g_assert_not_reached ();
	}

	/* draw in here */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();

	float** map = getInstance()->mMap;

	if (map != NULL)
	{

		glColor4f(1.0f,0.0f,0.0f,1.0f);
		glRotatef(75, 1.0, 0.0, 0.0);
		glRotatef(30, 0.0f, 1.0f, 0.0f);
		glTranslatef(-getInstance()->mMapWidth / 2, -200, -getInstance()->mMapHeight / 2);

		GLfloat light_position[ 4 ] = { 2.0, 0.0, 5.0, 0.0 };
		glLightfv( GL_LIGHT0, GL_POSITION, light_position );

		// Terrain rendering routine goes here
		glBegin(GL_QUADS);
		for (int x = 0; x < getInstance()->mMapWidth - 1; x++) {
			for (int y = 0; y < getInstance()->mMapHeight - 1; y++) {
				glVertex3f(x + 0, getInstance()->mMap[y + 1][x + 0], y + 1);
				glVertex3f(x + 1, getInstance()->mMap[y + 1][x + 1], y + 1);
				glVertex3f(x + 1, getInstance()->mMap[y + 0][x + 1], y + 0);
				glVertex3f(x + 0, getInstance()->mMap[y + 0][x + 0], y + 0);
			}
		}
		glEnd();
	}

	glPopMatrix();

	if (gdk_gl_drawable_is_double_buffered(gldrawable))
		gdk_gl_drawable_swap_buffers(gldrawable);

	else
		glFlush();

	gdk_gl_drawable_gl_end(gldrawable);

	return TRUE;
}

void Render3D::initialize(GtkHBox* scene3D, GtkSpinButton* render3DFPS,
		DataContainer* data) {
	mSpinButtonFPS = render3DFPS;
	mData = data;
	mDrawingArea = scene3D;

}

void Display3D::initializeDisplay3DModule(GtkButton* start3D, GtkButton* stop3D,
		GtkDrawingArea *depthMap, GtkHBox *view3D,
		GtkSpinButton* calculate3DFPS, GtkSpinButton* view3DFPS,
		DataContainer* data) {
	if (start3D != NULL)
	{
		g_signal_connect(G_OBJECT (start3D), "clicked",
				G_CALLBACK (DepthMap::start3D), NULL);
	}

	if (stop3D != NULL)
	{
		g_signal_connect(G_OBJECT (stop3D), "clicked",
				G_CALLBACK (DepthMap::stop3D), NULL);
	}

	if (depthMap != NULL)
	{
		g_signal_connect(G_OBJECT (depthMap), "expose_event",
				G_CALLBACK (DepthMap::draw3DCallback), NULL);
	}

	if (view3D != NULL)
	{

		gdk_threads_enter();

		GtkWidget *da = gtk_drawing_area_new();

		//if(gtk_widget_get_realized(GTK_WIDGET(da))) gtk_widget_unrealize(GTK_WIDGET(da));

		gtk_widget_set_events(GTK_WIDGET(da), GDK_EXPOSURE_MASK);

		GdkGLConfig *glconfig;
		glconfig = gdk_gl_config_new_by_mode(
				GdkGLConfigMode(
						GDK_GL_MODE_RGB | GDK_GL_MODE_DEPTH
								| GDK_GL_MODE_DOUBLE));

		if (!glconfig) {
			g_assert_not_reached ();
		}

		if (!gtk_widget_set_gl_capability(GTK_WIDGET(da), glconfig, NULL, TRUE,
				GDK_GL_RGBA_TYPE)) {
			g_assert_not_reached ();
		}

		g_signal_connect(G_OBJECT (da), "configure-event",
				G_CALLBACK (Render3D::glConfigure), NULL);

		g_signal_connect(G_OBJECT (da), "expose-event",
				G_CALLBACK (Render3D::glExpose), NULL);

		gtk_box_pack_end(GTK_BOX(view3D), da, TRUE, TRUE, 0);

		gtk_widget_show(GTK_WIDGET(da));

		gdk_threads_leave();

	}

	DepthMap::getInstance()->initialize(view3DFPS, depthMap, data);
	CalculateDepthMap::getInstance()->initialize(calculate3DFPS, data);
	Render3D::getInstance()->initialize(view3D, view3DFPS, data);

}

