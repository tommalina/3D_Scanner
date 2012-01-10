/*
 * Calibration.cpp
 *
 *  Created on: 10-01-2012
 *      Author: ble
 */

#include "Calibration.h"
#include "ThreadManager.h"

void Calibration::initializeCalibrationModule()
{

}

class Calibrate : public ThreadRunnable
{

private:

public:

	Calibrate();
	virtual ~Calibrate();

	void run();
	void end();

};
