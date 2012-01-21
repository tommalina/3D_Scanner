/*
 * Display3D.h
 *
 *  Created on: 21-01-2012
 *      Author: ble
 */

#ifndef DISPLAY3D_H_
#define DISPLAY3D_H_

#include "ThreadManager.h"
#include "Scanner3D.h"
#include "Camera.h"
#include "DataContainer.h"

#include <glibmm.h>
#include <gtkmm.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

class Display3D
{
public:

	static void initializeDisplay3DModule(GtkButton* start3D, GtkButton* stop3D, GtkDrawingArea *depthMap, GtkDrawingArea *view3D, GtkSpinButton* calculate3DFPS, GtkSpinButton* view3DFPS, DataContainer* data);

};

#endif /* DISPLAY3D_H_ */
