#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include "resize.cpp"
#include "lighting.cpp"
#include "menu.h"
int main()
{
	Menu menu =Menu();
	menu.runMenu();
	return 0;
}