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

class CalibrateSample : public ThreadRunnable
{

private:

	static std::auto_ptr<CalibrateSample>		mInstance;
	bool										mRun;
	DataContainer*								mData;
	int											mDelay;
	int											mSampleAmount;
	int											mChessboardW;
	int											mChessboardH;
	GtkSpinButton*								mCalibrationAmont;
	GtkSpinButton*								mCalibrationDelay;
	GtkSpinButton*								mCalibrationChessboardW;
	GtkSpinButton*								mCalibrationChessboardH;
	GtkButton*									mStartButton;
	GtkButton*									mStopButton;

	CalibrateSample();

	CalibrateSample(const CalibrateSample& obj);

public:

	void 										initialize(GtkSpinButton* calibrationAmount, GtkSpinButton* calibrationDelay, GtkSpinButton* calibrationChessboardW, GtkSpinButton* calibrationChessboardH , GtkButton* startCalibration, GtkButton* stopCalibration, DataContainer* data);

	virtual 									~CalibrateSample();

	void										run();
	void										end();

	DataContainer*								getData();

	static CalibrateSample*						instance;

	static CalibrateSample* 					getInstance();

	static void 								startCalibrateSample(GtkButton *button, gpointer   user_data);

	static void									stopCalibrateSample(GtkButton *button, gpointer   user_data);

};

class CalibrateCalculate : public ThreadRunnable
{
private:

	static std::auto_ptr<CalibrateCalculate>	mInstance;

	int											mFPS;

	DataContainer*								mData;

	bool										mRun;

	CalibrateCalculate();

	CalibrateCalculate(const CalibrateCalculate& obj);

	void										calculate();

public:

	static CalibrateCalculate*					getInstance();

	virtual 									~CalibrateCalculate();

	void										initialize(GtkSpinButton* fpsCamera, DataContainer* data);

	void										run();

	void										end();

};

CalibrateSample::CalibrateSample()
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

CalibrateSample::CalibrateSample(const CalibrateSample& obj)
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

void CalibrateSample::initialize(GtkSpinButton* calibrationAmount, GtkSpinButton* calibrationDelay, GtkSpinButton* calibrationChessboardW, GtkSpinButton* calibrationChessboardH, GtkButton* startCalibration, GtkButton* stopCalibration, DataContainer* data)
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

CalibrateSample::~CalibrateSample()
{

}

void CalibrateSample::run()
{
	mRun				= true;

	int countAmount		= 0;

	while(mRun)
	{

		printf("CalibrateSample ---------------- Run ; countAmount = %d \n",countAmount);

		if(++countAmount>=mSampleAmount)
		{
			break;
		}
		sleep(mDelay*1000);
	}

	ThreadManager::endNowThread(CalibrateCalculate::getInstance());

}

void CalibrateSample::end()
{
	mRun		= false;
	printf("CalibrateSample ---------------- End \n");
}

DataContainer* CalibrateSample::getData()
{
	return mData;
}

std::auto_ptr<CalibrateSample> CalibrateSample::mInstance;

CalibrateSample* CalibrateSample::getInstance()
{
	if(mInstance.get()==NULL)
	{
		mInstance.reset(new CalibrateSample());
	}
	return mInstance.get();
}

void CalibrateSample::startCalibrateSample(GtkButton *button, gpointer   user_data)
{
	bool canStart		= false;

	getInstance()->getData()->getImageLeftGrayRef().lockData();
	getInstance()->getData()->getImageRightGrayRef().lockData();
	canStart	= getInstance()->getData()->getImageLeftGrayRef().getPtr() != NULL && getInstance()->getData()->getImageRightGrayRef().getPtr() != NULL;
	getInstance()->getData()->getImageRightGrayRef().unlockData();
	getInstance()->getData()->getImageLeftGrayRef().unlockData();

	if(canStart)
	{
		ThreadManager::endNowThread(getInstance());
		ThreadManager::endNowThread(CalibrateCalculate::getInstance());
		ThreadManager::addThread(getInstance());
		ThreadManager::addThread(CalibrateCalculate::getInstance());
		ThreadManager::startThread(getInstance());
		ThreadManager::startThread(CalibrateCalculate::getInstance());
	}
}

void CalibrateSample::stopCalibrateSample(GtkButton *button, gpointer   user_data)
{
	ThreadManager::endNowThread(getInstance());
	ThreadManager::endNowThread(CalibrateCalculate::getInstance());
}

CalibrateCalculate::CalibrateCalculate()
{
	mFPS		= 1000/30;
	mData		= NULL;
	mRun		= false;
}

CalibrateCalculate::CalibrateCalculate(const CalibrateCalculate& obj)
{
	mFPS		= 1000/30;
	mData		= NULL;
	mRun		= false;
}

CalibrateCalculate::~CalibrateCalculate()
{

}

void CalibrateCalculate::initialize(GtkSpinButton* fpsCamera, DataContainer* data)
{
	if(fpsCamera!=NULL)
	{
		mFPS		= 1000/gtk_spin_button_get_value_as_int(fpsCamera);
	}
	mData		= data;
	mRun		= true;
}

void CalibrateCalculate::run()
{
	clock_t lastTime			= clock() / (CLOCKS_PER_SEC / 1000);
	clock_t currentTime;

	mRun		= true;

	while (mRun)
	{

		calculate();

		currentTime		= clock() / (CLOCKS_PER_SEC / 1000);
		if(currentTime-lastTime<mFPS) {
			sleep((mFPS-(currentTime-lastTime)));
		}
		lastTime		= clock() / (CLOCKS_PER_SEC / 1000);

	}
}

void CalibrateCalculate::end()
{
	mRun		= false;
	printf("CalibrateCalculate ---------------- End \n");
}

void CalibrateCalculate::calculate()
{
	printf("CalibrateCalculate ---------------- calculate \n");
}

std::auto_ptr<CalibrateCalculate> CalibrateCalculate::mInstance;

CalibrateCalculate* CalibrateCalculate::getInstance()
{
	if(mInstance.get()==NULL)
	{
		mInstance.reset(new CalibrateCalculate());
	}
	return mInstance.get();
}

void Calibration::initializeCalibrationModule(GtkSpinButton* cameraFPS ,GtkSpinButton* calibrationAmount, GtkSpinButton* calibrationDelay, GtkSpinButton* calibrationChessboardW, GtkSpinButton* calibrationChessboardH, GtkButton* startCalibration, GtkButton* stopCalibration, DataContainer* data)
{
	CalibrateSample::getInstance()->initialize(calibrationAmount, calibrationDelay, calibrationChessboardW, calibrationChessboardH, startCalibration, stopCalibration, data);
	CalibrateCalculate::getInstance()->initialize(cameraFPS, data);

	if(startCalibration!=NULL)
	{
		g_signal_connect (G_OBJECT (startCalibration), "clicked", G_CALLBACK (CalibrateSample::startCalibrateSample), NULL);
	}

	if(stopCalibration!=NULL)
	{
		g_signal_connect (G_OBJECT (stopCalibration), "clicked", G_CALLBACK (CalibrateSample::stopCalibrateSample), NULL);
	}
}

