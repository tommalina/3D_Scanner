/*
 * Calibration.cpp
 *
 *  Created on: 10-01-2012
 *      Author: ble
 */

#include <memory.h>
#include <stdio.h>
#include <unistd.h>

#include "Calibration.h"
#include "ThreadManager.h"

class Calibrate : public ThreadRunnable
{

private:

	static std::auto_ptr<Calibrate>		mInstance;
	bool								mRun;
	DataContainer*						mData;
	int									mDelay;
	int									mSampleAmount;
	int									mChessboardW;
	int									mChessboardH;
	GtkSpinButton*						mCalibrationAmont;
	GtkSpinButton*						mCalibrationDelay;
	GtkSpinButton*						mCalibrationChessboardW;
	GtkSpinButton*						mCalibrationChessboardH;
	GtkButton*							mStartButton;
	GtkButton*							mStopButton;

	Calibrate();

	Calibrate(const Calibrate& obj);

public:

	void 								initialize(GtkSpinButton* calibrationAmount, GtkSpinButton* calibrationDelay, GtkSpinButton* calibrationChessboardW, GtkSpinButton* calibrationChessboardH , GtkButton* startCalibration, GtkButton* stopCalibration, DataContainer* data);

	virtual 							~Calibrate();

	void								run();
	void								end();

	static Calibrate*					instance;

	static Calibrate* 					getInstance();

	static void 						startCalibrate(GtkButton *button, gpointer   user_data);

	static void							stopCalibrate(GtkButton *button, gpointer   user_data);

};

Calibrate::Calibrate()
{
	mData					= NULL;
	mCalibrationAmont		= NULL;
	mCalibrationDelay		= NULL;
	mCalibrationChessboardH	= NULL;
	mCalibrationChessboardW	= NULL;
	mStartButton			= NULL;
	mStopButton				= NULL;
	mRun					= true;
}

Calibrate::Calibrate(const Calibrate& obj)
{
	mData					= NULL;
	mCalibrationAmont		= NULL;
	mCalibrationDelay		= NULL;
	mCalibrationChessboardH	= NULL;
	mCalibrationChessboardW	= NULL;
	mStartButton			= NULL;
	mStopButton				= NULL;
	mRun					= true;
}

void Calibrate::initialize(GtkSpinButton* calibrationAmount, GtkSpinButton* calibrationDelay, GtkSpinButton* calibrationChessboardW, GtkSpinButton* calibrationChessboardH, GtkButton* startCalibration, GtkButton* stopCalibration, DataContainer* data)
{
	mData					= data;
	mCalibrationAmont		= calibrationAmount;
	mCalibrationDelay		= calibrationDelay;
	mCalibrationChessboardW	= calibrationChessboardW;
	mCalibrationChessboardH	= calibrationChessboardH;
	mStartButton			= startCalibration;
	mStopButton				= stopCalibration;

	if(mCalibrationAmont!=NULL)
	{
		mSampleAmount		= gtk_spin_button_get_value_as_int(mCalibrationAmont);
	}

	if(mCalibrationDelay!=NULL)
	{
		mDelay				= gtk_spin_button_get_value_as_int(mCalibrationDelay);
	}

	if(mCalibrationChessboardW!=NULL)
	{
		mChessboardW		= gtk_spin_button_get_value_as_int(mCalibrationChessboardW);
	}

	if(mCalibrationChessboardH!=NULL)
	{
		mChessboardH		= gtk_spin_button_get_value_as_int(mCalibrationChessboardH);
	}
}

Calibrate::~Calibrate()
{

}

void Calibrate::run()
{
	mRun				= true;

	int countAmount		= 0;

	while(mRun)
	{

		printf("Calibrate ---------------- Run ; countAmount = %d \n",countAmount);

		if(++countAmount>=mSampleAmount)
		{
			break;
		}
		sleep(mDelay*1000);
	}
}

void Calibrate::end()
{
	mRun		= false;
	printf("Calibrate ---------------- End \n");
}

std::auto_ptr<Calibrate> Calibrate::mInstance;

Calibrate* Calibrate::getInstance()
{
	if(mInstance.get()==NULL)
	{
		mInstance.reset(new Calibrate());
	}
	return mInstance.get();
}

void Calibrate::startCalibrate(GtkButton *button, gpointer   user_data)
{
	ThreadManager::endNowThread(getInstance());
	ThreadManager::addThread(getInstance());
	ThreadManager::startThread(getInstance());
}

void Calibrate::stopCalibrate(GtkButton *button, gpointer   user_data)
{
	ThreadManager::endNowThread(getInstance());
}

void Calibration::initializeCalibrationModule(GtkSpinButton* calibrationAmount, GtkSpinButton* calibrationDelay, GtkSpinButton* calibrationChessboardW, GtkSpinButton* calibrationChessboardH, GtkButton* startCalibration, GtkButton* stopCalibration, DataContainer* data)
{
	Calibrate::getInstance()->initialize(calibrationAmount, calibrationDelay, calibrationChessboardW, calibrationChessboardH, startCalibration, stopCalibration, data);

	if(startCalibration!=NULL)
	{
		g_signal_connect (G_OBJECT (startCalibration), "clicked", G_CALLBACK (Calibrate::startCalibrate), NULL);
	}

	if(stopCalibration!=NULL)
	{
		g_signal_connect (G_OBJECT (stopCalibration), "clicked", G_CALLBACK (Calibrate::stopCalibrate), NULL);
	}
}

