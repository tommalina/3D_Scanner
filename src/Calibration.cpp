/*
 * Calibration.cpp
 *
 *  Created on: 10-01-2012
 *      Author: ble
 */

#include "Calibration.h"
#include "ThreadManager.h"

void Calibration::initializeCalibrationModule(GtkSpinButton* calibrationAmount, GtkSpinButton* calibrationDelay, GtkButton* startCalibration, GtkButton* stopCalibration, DataContainer* data)
{

}

class Calibrate : public ThreadRunnable
{

private:

	DataContainer*		mData;
	int					mDelay;
	int					mSampleAmount;
	GtkSpinButton*		mCalibrationAmont;
	GtkSpinButton*		mCalibrationDelay;
	GtkButton*			mStartButton;
	GtkButton*			mStopButton;

public:

	Calibrate(GtkSpinButton* calibrationAmount, GtkSpinButton* calibrationDelay, GtkButton* startCalibration, GtkButton* stopCalibration, DataContainer* data);
	virtual ~Calibrate();

	void		run();
	void		end();

	static Calibrate*	instance;

};

Calibrate::Calibrate(GtkSpinButton* calibrationAmount, GtkSpinButton* calibrationDelay, GtkButton* startCalibration, GtkButton* stopCalibration, DataContainer* data)
{
	mData				= data;
	mCalibrationAmont	= calibrationAmount;
	mCalibrationDelay	= calibrationDelay;
	mStartButton		= startCalibration;
	mStopButton			= stopCalibration;

	if(mCalibrationAmont!=NULL)
	{
		mSampleAmount		= gtk_spin_button_get_value_as_int(mCalibrationAmont);
	}

	if(mCalibrationDelay!=NULL)
	{
		mDelay				= gtk_spin_button_get_value_as_int(mCalibrationDelay);
	}
}

Calibrate::~Calibrate()
{

}

void Calibrate::run()
{

}

void Calibrate::end()
{

}
