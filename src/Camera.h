/*
 * Camera.h
 *
 *  Created on: 04-04-2011
 *      Author: ble
 */

#ifndef CAMERA_H_
#define CAMERA_H_

#include <cv.h>
#include <highgui.h>
#include <cvtypes.h>

class Camera {
private:
	CvCapture*			camera;
	IplImage*			image;
	IplImage*			imageGray;
	bool				cameraError;

public:
	Camera(int cameraId);
	Camera(int cameraId, int width, int height);
	~Camera();

	void			captureImage();

	IplImage*		getImage();
	IplImage*		getImageGray();

	bool			isError();

};

#endif /* CAMERA_H_ */
