/*
 * Scanner3D.h
 *
 *  Created on: 20-11-2011
 *      Author: ble
 */

#ifndef SCANNER3D_H_
#define SCANNER3D_H_

#include <stdlib.h>
#include <memory>

/**
 * Scanner3D. Class that include all 3d scanner functionality.
 */
class Scanner3D
{
private:

	/**
	 * Static auto pointer to only one instance of this object.
	 */
	static std::auto_ptr<Scanner3D>		mInstance;

	/*
	 * Method which returns singleton poainter to only one instance of this class.
	 * @return - Returns Scanner3D pointer.
	 */
	static Scanner3D*					getInstance();

	/**
	 * Private constructor.
	 */
	Scanner3D();

	/**
	 * Private copy constructor.
	 */
	Scanner3D(Scanner3D& obj);

	/**
	 * Main method that initialize and run all threads.
	 * @param int - Amount of arguments.
	 * @param char** - Table with string arguments.
	 */
	int									Run(int, char**);

public:

	/**
	 * Public destructor.
	 */
	~Scanner3D();

	/**
	 * Static method that calls Run method from this object.
	 * @param argc - Amount of arguments.
	 * @param argv - Table with string arguments.
	 */
	static int							Scanner3DRun(int argc, char** argv);

};

#endif /* SCANNER3D_H_ */
