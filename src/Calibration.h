/*
 * Calibration.h
 *
 *  Created on: 10-01-2012
 *      Author: ble
 */

#ifndef CALIBRATION_H_
#define CALIBRATION_H_

#include <glibmm.h>
#include <gtkmm.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#include "DataContainer.h"

class Calibration
{
public:

	static void initializeCalibrationModule(GtkSpinButton* calibrationFPS, GtkSpinButton* calibrationAmount, GtkSpinButton* calibrationDelay, GtkSpinButton* calibrationChessboardW, GtkSpinButton* calibrationChessboardH, GtkButton* startCalibration, GtkButton* stopCalibration, DataContainer* data);
};

#endif /* CALIBRATION_H_ */
