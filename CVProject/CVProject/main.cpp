#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include <qapplication.h>
#include <qpushbutton.h>


#include "resize.cpp"
#include "lighting.cpp"
#include "menu.h"
int main(int argc, char* argv[])
{

	QApplication app(argc, argv);

	Menu menu =Menu();
	menu.runMenu();

	
	return app.exec();
}