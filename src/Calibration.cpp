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

//TODO: Change image size.
#define IMAGE_WIDTH		640
#define IMAGE_HEIGHT	320

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

	mRun							= true;

	int countAmount					= 0;
	int tempCalibrateChessboard		= mChessboardW*mChessboardH;
	int tempCalibrateChessboardW	= mChessboardW;
	int tempCalibrateChessboardH	= mChessboardH;

	CvPoint2D32f calibratePointsLeft[tempCalibrateChessboard*mSampleAmount], calibratePointsRight[tempCalibrateChessboard*mSampleAmount];

	while(mRun)
	{

		printf("CalibrateSample ---------------- Run ; countAmount = %d \n",countAmount);

		if(countAmount>=mSampleAmount)
		{
			break;
		}

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

		if(tempGrayRight&&tempGrayLeft)
		{

			int i, countLeft, countRight , resultLeft, resultRight;
			CvPoint2D32f tempLeft[tempCalibrateChessboard], tempRight[tempCalibrateChessboard];
			bool gotData		=  false;

			if(mData->getCalibrateDataAccess().tryLockData())
			{
				countLeft						= *mData->getCalibrateCurrentCornersLeft();
				countRight						= *mData->getCalibrateCurrentCornersRight();
				resultLeft						= *mData->getCalibrateResultLeft();
				resultRight						= *mData->getCalibrateResultRight();

				CvPoint2D32f *srcPtrLeft = mData->getCalibrateCurrentPointsLeft().getPtr(), *srcPtrRight = mData->getCalibrateCurrentPointsRight().getPtr();

				if(srcPtrLeft!=NULL&&srcPtrRight!=NULL)
				{
					CvPoint2D32f *outPtrLeft			= &tempLeft[0];
					CvPoint2D32f *outPtrRight			= &tempRight[0];
					for(i = 0 ; i < tempCalibrateChessboard; ++i)
					{
						*(++outPtrLeft)		= *(++srcPtrLeft);
						*(++outPtrRight)	= *(++srcPtrRight);
					}

					gotData							= true;
				}

				mData->getCalibrateDataAccess().unlockData();
			}


			if(gotData&&resultLeft&&countLeft==tempCalibrateChessboard&&resultRight&&countRight==tempCalibrateChessboard)
			{
				bool calibrateCalculate		= false;

				cvFindCornerSubPix( tempGrayLeft, &tempLeft[0], countLeft, cvSize(11, 11), cvSize(-1,-1), cvTermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS, 30, 0.01) );
				cvFindCornerSubPix( tempGrayRight, &tempRight[0], countRight, cvSize(11, 11), cvSize(-1,-1), cvTermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS, 30, 0.01) );

				for(i = 0 ; i < tempCalibrateChessboard ; ++i) {
					calibratePointsLeft[countAmount*tempCalibrateChessboard+i]				= tempLeft[i];
					calibratePointsRight[countAmount*tempCalibrateChessboard+i]				= tempRight[i];
				}
				++countAmount;
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

		sleep(mDelay*1000);
	}

	// Calculate calibration from samples.
	if(mRun)
	{

		int i, j;

		bool isVerticalStereo		= false;
		int useUncalibrated			= 2;

		CvPoint3D32f linesLeft[tempCalibrateChessboard*mSampleAmount];
		CvPoint3D32f linesRight[tempCalibrateChessboard*mSampleAmount];

		double M1[3][3], M2[3][3], D1[5], D2[5];
		double R[3][3], T[3], E[3][3], F[3][3];
		CvMat _M1 	= cvMat(3, 3, CV_64F, M1 );
		CvMat _M2 	= cvMat(3, 3, CV_64F, M2 );
		CvMat _D1 	= cvMat(1, 5, CV_64F, D1 );
		CvMat _D2 	= cvMat(1, 5, CV_64F, D2 );
		CvMat _R 	= cvMat(3, 3, CV_64F, R );
		CvMat _T 	= cvMat(3, 1, CV_64F, T );
		CvMat _E 	= cvMat(3, 3, CV_64F, E );
		CvMat _F 	= cvMat(3, 3, CV_64F, F );

		CvPoint3D32f	objectPoints[tempCalibrateChessboard*mSampleAmount];
		int				npoints[mSampleAmount];

		for( i = 0; i < tempCalibrateChessboardH; ++i )
			for( j = 0; j < tempCalibrateChessboardW; ++j )
				objectPoints[i*tempCalibrateChessboardW + j] =		cvPoint3D32f(i, j, 0);

		for( i = 1; i < mSampleAmount; ++i )
			for(j = 0 ; j < tempCalibrateChessboard ; ++j)
				objectPoints[i*tempCalibrateChessboard+j]		= objectPoints[j];

		for(i = 0 ; i < mSampleAmount ; ++i)
			npoints[i]		= tempCalibrateChessboard;

		CvMat _objectPoints 	= cvMat(1, tempCalibrateChessboard*mSampleAmount, CV_32FC3, &objectPoints[0] );
		CvMat _imagePoints1 	= cvMat(1, tempCalibrateChessboard*mSampleAmount, CV_32FC2, &calibratePointsLeft[0] );
		CvMat _imagePoints2 	= cvMat(1, tempCalibrateChessboard*mSampleAmount, CV_32FC2, &calibratePointsRight[0] );
		CvMat _npoints 			= cvMat(1, mSampleAmount, CV_32S, &npoints[0] );

		cvSetIdentity(&_M1);
		cvSetIdentity(&_M2);
		cvZero(&_D1);
		cvZero(&_D2);

		// Calibrate.
		//TODO: Change image size.
		cvStereoCalibrate( &_objectPoints, &_imagePoints1, &_imagePoints2, &_npoints, &_M1, &_D1, &_M2, &_D2, cvSize(IMAGE_WIDTH, IMAGE_HEIGHT), &_R, &_T, &_E, &_F, cvTermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS, 100, 1e-5), CV_CALIB_FIX_ASPECT_RATIO + CV_CALIB_ZERO_TANGENT_DIST + CV_CALIB_SAME_FOCAL_LENGTH );

		_imagePoints1 		= cvMat(1, tempCalibrateChessboard*mSampleAmount, CV_32FC2, &calibratePointsLeft[0] );
		_imagePoints2 		= cvMat(1, tempCalibrateChessboard*mSampleAmount, CV_32FC2, &calibratePointsRight[0] );

		CvMat _L1 			= cvMat(1, tempCalibrateChessboard*mSampleAmount, CV_32FC3, &linesLeft[0]);
		CvMat _L2 			= cvMat(1, tempCalibrateChessboard*mSampleAmount, CV_32FC3, &linesRight[0]);
		cvUndistortPoints( &_imagePoints1, &_imagePoints1, &_M1, &_D1, 0, &_M1 );
		cvUndistortPoints( &_imagePoints2, &_imagePoints2, &_M2, &_D2, 0, &_M2 );
		cvComputeCorrespondEpilines( &_imagePoints1, 1, &_F, &_L1 );
		cvComputeCorrespondEpilines( &_imagePoints2, 2, &_F, &_L2 );

		CvMat* mx1			= cvCreateMat(IMAGE_HEIGHT, IMAGE_WIDTH, CV_32F);
		CvMat* my1			= cvCreateMat(IMAGE_HEIGHT, IMAGE_WIDTH, CV_32F);
		CvMat* mx2			= cvCreateMat(IMAGE_HEIGHT, IMAGE_WIDTH, CV_32F);
		CvMat* my2			= cvCreateMat(IMAGE_HEIGHT, IMAGE_WIDTH, CV_32F);

		double R1[3][3], R2[3][3], P1[3][4], P2[3][4];
		CvMat _R1 			= cvMat(3, 3, CV_64F, R1);
		CvMat _R2 			= cvMat(3, 3, CV_64F, R2);

		if( useUncalibrated == 0 ) {
			CvMat _P1 = cvMat(3, 4, CV_64F, P1);
			CvMat _P2 = cvMat(3, 4, CV_64F, P2);
			cvStereoRectify( &_M1, &_M2, &_D1, &_D2, cvSize(IMAGE_WIDTH,IMAGE_HEIGHT), &_R, &_T, &_R1, &_R2, &_P1, &_P2, 0, 0/*CV_CALIB_ZERO_DISPARITY*/ );
			isVerticalStereo = fabs(P2[1][3]) > fabs(P2[0][3]);
			//Precompute maps for cvRemap()
			cvInitUndistortRectifyMap(&_M1,&_D1,&_R1,&_P1,mx1,my1);
			cvInitUndistortRectifyMap(&_M2,&_D2,&_R2,&_P2,mx2,my2);
		} else
		if( useUncalibrated == 1 || useUncalibrated == 2 ) {

			double H1[3][3], H2[3][3], iM[3][3];
			CvMat _H1 = cvMat(3, 3, CV_64F, H1);
			CvMat _H2 = cvMat(3, 3, CV_64F, H2);
			CvMat _iM = cvMat(3, 3, CV_64F, iM);

			if( useUncalibrated == 2 )
				cvFindFundamentalMat( &_imagePoints1, &_imagePoints2, &_F);

			cvStereoRectifyUncalibrated( &_imagePoints1, &_imagePoints2, &_F, cvSize(IMAGE_WIDTH,IMAGE_HEIGHT), &_H1, &_H2, 3);

			cvInvert(&_M1, &_iM);
			cvMatMul(&_H1, &_M1, &_R1);
			cvMatMul(&_iM, &_R1, &_R1);
			cvInvert(&_M2, &_iM);
			cvMatMul(&_H2, &_M2, &_R2);
			cvMatMul(&_iM, &_R2, &_R2);

			cvInitUndistortRectifyMap(&_M1,&_D1,&_R1,&_M1,mx1,my1);
			cvInitUndistortRectifyMap(&_M2,&_D1,&_R2,&_M2,mx2,my2);

		}

		mData->getCalibrateX1().lockData();
		if(mData->getCalibrateX1().getPtr()!=NULL) {
			CvMat* tempPtr		= mData->getCalibrateX1().getPtr();
			cvReleaseMat(&tempPtr);
			mData->getCalibrateX1().setPtr(NULL);
		}
		mData->getCalibrateX1().setPtr(mx1);
		mData->getCalibrateX1().unlockData();

		mData->getCalibrateY1().lockData();
		if(mData->getCalibrateY1().getPtr()!=NULL) {
			CvMat* tempPtr		= mData->getCalibrateY1().getPtr();
			cvReleaseMat(&tempPtr);
			mData->getCalibrateY1().setPtr(NULL);
		}
		mData->getCalibrateY1().setPtr(my1);
		mData->getCalibrateY1().unlockData();

		mData->getCalibrateX2().lockData();
		if(mData->getCalibrateX2().getPtr()!=NULL) {
			CvMat* tempPtr		= mData->getCalibrateX2().getPtr();
			cvReleaseMat(&tempPtr);
			mData->getCalibrateX2().setPtr(NULL);
		}
		mData->getCalibrateX2().setPtr(mx2);
		mData->getCalibrateX2().unlockData();

		mData->getCalibrateY2().lockData();
		if(mData->getCalibrateY2().getPtr()!=NULL) {
			CvMat* tempPtr		= mData->getCalibrateY2().getPtr();
			cvReleaseMat(&tempPtr);
			mData->getCalibrateY2().setPtr(NULL);
		}
		mData->getCalibrateY2().setPtr(my2);
		mData->getCalibrateY2().unlockData();

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

	getInstance()->getData()->drawChessboard().lockData();
	bool startCalibrate		= *(getInstance()->getData()->drawChessboard().getPtr());
	canStart				= canStart && !startCalibrate;
	getInstance()->getData()->drawChessboard().unlockData();

	if(canStart)
	{
		ThreadManager::endNowThread(getInstance());
		ThreadManager::endNowThread(CalibrateCalculate::getInstance());
		ThreadManager::addThread(getInstance());
		ThreadManager::addThread(CalibrateCalculate::getInstance());
		ThreadManager::startThread(getInstance(),90);
		ThreadManager::startThread(CalibrateCalculate::getInstance(),90);
		*(getInstance()->getData()->drawChessboard())		= true;
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
	int i,tempCalibrateChessboardW,tempCalibrateChessboardH,tempCalibrateChessboard;

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

