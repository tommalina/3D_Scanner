/*
 * Camera.cpp
 *
 *  Created on: 04-04-2011
 *      Author: ble
 */

#include "Camera.h"

Camera::Camera(int cameraId) {
	image			= NULL;
	imageGray		= NULL;
	cameraError		= false;
	camera			= cvCaptureFromCAM(cameraId);
	if(camera==NULL) {
		cameraError		= true;
	}
}

Camera::Camera(int cameraId, int width, int height) {
	image			= NULL;
	imageGray		= NULL;
	cameraError		= false;
	camera			= cvCaptureFromCAM(cameraId);
	cvSetCaptureProperty(camera ,CV_CAP_PROP_FRAME_WIDTH,width);
	cvSetCaptureProperty(camera ,CV_CAP_PROP_FRAME_HEIGHT,height);
	if(camera==NULL) {
		cameraError		= true;
	}
}

Camera::~Camera() {
	if(image!=NULL) {
		cvReleaseData(image);
		cvReleaseImage(&image);
	}
	if(imageGray!=NULL) {
		cvReleaseData(imageGray);
		cvReleaseImage(&imageGray);
	}
	if(camera!=NULL)
		cvReleaseCapture(&camera);
}

void Camera::captureImage() {
	image		= cvQueryFrame(camera);
}

IplImage* Camera::getImage() {
	return image;
}

IplImage* Camera::getImageGray() {
	if(image!=NULL) {
		if(image->depth==1) {
			return image;
		} else {
			if(imageGray==NULL) {
				imageGray		= cvCreateImage(cvGetSize(image),IPL_DEPTH_8U,1);
			}
			cvCvtColor(image,imageGray,CV_BGR2GRAY);
			return imageGray;
		}
	}
	return NULL;
}

bool Camera::isError() {
	return cameraError;
}
