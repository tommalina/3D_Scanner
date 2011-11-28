/*
 * Scanner3D.h
 *
 *  Created on: 20-11-2011
 *      Author: ble
 */

#ifndef SCANNER3D_H_
#define SCANNER3D_H_

#include <gtk/gtk.h>
#include <gtk/gtkmain.h>
#include <gtk/gtkbuilder.h>
#include <gtk/gtkwidget.h>

#include <stdlib.h>
#include <memory>

//TODO: I need to comment all members in this class.

class Scanner3D
{
private:

	static std::auto_ptr<Scanner3D>		mInstance;

	static Scanner3D*					getInstance();

	Scanner3D();

	Scanner3D(Scanner3D& obj);

	int									Run(int, char**);

public:

	~Scanner3D();

	static int							Scanner3DRun(int argc, char** argv);

};

#endif /* SCANNER3D_H_ */
