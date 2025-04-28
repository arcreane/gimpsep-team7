#pragma once

#pragma once
#ifndef MENU_H
#define MENU_H

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <iostream>
#include "image.h"

class Menu
{
private:
	//Add parameters for other functions here
public:
	void runMenu();
	void showMenuForImage(Image* image);
	void showMenuForMultipleImages(std::vector<Image> images);
};


#endif