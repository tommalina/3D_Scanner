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
#include "Camera.h"

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

	static CalibrateSample* 					getInstance();

	static void 								startCalibrateSample(GtkButton *button, gpointer   user_data);

	static void									stopCalibrateSample(GtkButton *button, gpointer   user_data);

};

class CalibrateCalculate : public ThreadRunnable
{
private:

	static std::auto_ptr<CalibrateCalculate>	mInstance;

	int											mFPS;

	int											mSampleAmount;

	DataContainer*								mData;

	bool										mRun;

	GtkSpinButton* 								mFpsCalibration;

	GtkSpinButton* 								mCalibrationAmount;

	CalibrateCalculate();

	CalibrateCalculate(const CalibrateCalculate& obj);

	void										calculate();

public:

	static CalibrateCalculate*					getInstance();

	virtual 									~CalibrateCalculate();

	void										initialize(GtkSpinButton* fpsCalibration, GtkSpinButton* calibrationAmount, DataContainer* data);

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

}

CalibrateSample::~CalibrateSample()
{

}

void CalibrateSample::run()
{

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

	mData->getCalibrateChessboardW().lockData();
	*(mData->getCalibrateChessboardW())		= mChessboardW;
	mData->getCalibrateChessboardW().unlockData();

	mData->getCalibrateChessboardH().lockData();
	*(mData->getCalibrateChessboardH())		= mChessboardH;
	mData->getCalibrateChessboardH().unlockData();

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

	*(mData->drawChessboard())		= false;

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
		ThreadManager::startThread(getInstance(),90);
		ThreadManager::startThread(CalibrateCalculate::getInstance(),90);
		*(CalibrateSample::getInstance()->getData()->drawChessboard())		= true;
	}
}

void CalibrateSample::stopCalibrateSample(GtkButton *button, gpointer   user_data)
{
	ThreadManager::endNowThread(getInstance());
	ThreadManager::endNowThread(CalibrateCalculate::getInstance());
	*(CalibrateSample::getInstance()->getData()->drawChessboard())		= false;
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

void CalibrateCalculate::initialize(GtkSpinButton* fpsCalibration, GtkSpinButton* calibrationAmount, DataContainer* data)
{
	mFpsCalibration			= fpsCalibration;
	mCalibrationAmount		= calibrationAmount;
	mData					= data;
	mRun					= true;
}

void CalibrateCalculate::run()
{
	clock_t lastTime			= clock() / (CLOCKS_PER_SEC / 1000);
	clock_t currentTime;

	mRun		= true;

	if(mFpsCalibration!=NULL)
	{
		mFPS				= 1000/gtk_spin_button_get_value_as_int(mFpsCalibration);
	}

	if(mCalibrationAmount!=NULL)
	{
		mSampleAmount		= gtk_spin_button_get_value_as_int(mCalibrationAmount);
	}

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
	int i, tempCalibrateChessboardW,tempCalibrateChessboardH,tempCalibrateChessboard;

	IplImage *tempGrayLeft		= NULL;
	IplImage *tempGrayRight		= NULL;

	if(mData->getImageLeftGrayRef().tryLockData())
	{
		tempGrayLeft		= cvCreateImage(cvSize(mData->getImageLeftGrayRef()->width,mData->getImageLeftGrayRef()->height),mData->getImageLeftGrayRef()->depth,mData->getImageLeftGrayRef()->nChannels);
		cvCopy(mData->getImageLeftGrayRef().getPtr(),tempGrayLeft);
		mData->getImageLeftGrayRef().unlockData();
	}

	if(mData->getImageRightGrayRef().tryLockData())
	{
		tempGrayRight		= cvCreateImage(cvSize(mData->getImageRightGrayRef()->width,mData->getImageRightGrayRef()->height),mData->getImageRightGrayRef()->depth,mData->getImageRightGrayRef()->nChannels);
		cvCopy(mData->getImageRightGrayRef().getPtr(),tempGrayRight);
		mData->getImageRightGrayRef().unlockData();
	}

	tempCalibrateChessboardW		= *mData->getCalibrateChessboardW().getPtr();
	tempCalibrateChessboardH		= *mData->getCalibrateChessboardH().getPtr();

	tempCalibrateChessboard			= tempCalibrateChessboardW*tempCalibrateChessboardH;

	/** Start calibrate. */

	if(tempGrayLeft&&tempGrayRight)
	{
		CvPoint2D32f tempLeft[tempCalibrateChessboard];
		CvPoint2D32f tempRight[tempCalibrateChessboard];

		int resultLeft	= 0, countLeft = 0, resultRight = 0, countRight = 0;

		resultLeft 		= cvFindChessboardCorners( tempGrayLeft, cvSize(tempCalibrateChessboardW, tempCalibrateChessboardH), &tempLeft[0], &countLeft, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_NORMALIZE_IMAGE);
		resultRight 	= cvFindChessboardCorners( tempGrayRight, cvSize(tempCalibrateChessboardW, tempCalibrateChessboardH), &tempRight[0], &countRight, CV_CALIB_CB_ADAPTIVE_THRESH |	CV_CALIB_CB_NORMALIZE_IMAGE);

		{
			// Copy current corners table to global table.

			if(mData->getCalibrateDataAccess().tryLockData()) {

				if(mData->getCalibrateCurrentPointsLeft().getPtr()!=NULL) {
					delete [] mData->getCalibrateCurrentPointsLeft().getPtr();
				}
				mData->getCalibrateCurrentPointsLeft().setPtr(new CvPoint2D32f[tempCalibrateChessboard]);

				if(mData->getCalibrateCurrentPointsRight().getPtr()!=NULL) {
					delete [] mData->getCalibrateCurrentPointsRight().getPtr();
				}
				mData->getCalibrateCurrentPointsRight().setPtr(new CvPoint2D32f[tempCalibrateChessboard]);

				CvPoint2D32f *tempSrcLeft,*tempSrcRight,*tempOutLeft,*tempOutRight;

				tempSrcLeft		= tempLeft;
				tempSrcRight	= tempRight;
				tempOutLeft		= mData->getCalibrateCurrentPointsLeft().getPtr();
				tempOutRight	= mData->getCalibrateCurrentPointsRight().getPtr();

				for(i = 0 ; i < tempCalibrateChessboard ; ++i) {
					*tempOutLeft		= *tempSrcLeft;
					*tempOutRight		= *tempSrcRight;
					++tempSrcLeft;
					++tempSrcRight;
					++tempOutLeft;
					++tempOutRight;
				}

				*mData->getCalibrateCurrentCornersLeft().getPtr()		= countLeft;
				*mData->getCalibrateCurrentCornersRight().getPtr()		= countRight;
				*mData->getCalibrateResultLeft().getPtr()				= resultLeft;
				*mData->getCalibrateResultRight().getPtr()				= resultRight;

				mData->getCalibrateDataAccess().unlockData();
			}

		}

	}

	if(tempGrayLeft)
	{
		cvReleaseData(tempGrayLeft);
		cvReleaseImage(&tempGrayLeft);
	}

	if(tempGrayRight)
	{
		cvReleaseData(tempGrayRight);
		cvReleaseImage(&tempGrayRight);
	}

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

void Calibration::initializeCalibrationModule(GtkSpinButton* calibrationFPS ,GtkSpinButton* calibrationAmount, GtkSpinButton* calibrationDelay, GtkSpinButton* calibrationChessboardW, GtkSpinButton* calibrationChessboardH, GtkButton* startCalibration, GtkButton* stopCalibration, DataContainer* data)
{
	CalibrateSample::getInstance()->initialize(calibrationAmount, calibrationDelay, calibrationChessboardW, calibrationChessboardH, startCalibration, stopCalibration, data);
	CalibrateCalculate::getInstance()->initialize(calibrationFPS, calibrationAmount, data);

	if(startCalibration!=NULL)
	{
		g_signal_connect (G_OBJECT (startCalibration), "clicked", G_CALLBACK (CalibrateSample::startCalibrateSample), NULL);
	}

	if(stopCalibration!=NULL)
	{
		g_signal_connect (G_OBJECT (stopCalibration), "clicked", G_CALLBACK (CalibrateSample::stopCalibrateSample), NULL);
	}
}

