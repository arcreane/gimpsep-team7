#pragma once

#pragma once
#ifndef MENU_H
#define MENU_H

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <iostream>

#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMessageBox>
#include "image.h"

class Menu
{
private:
public:
	void runMenu();												//running the menu
	void showMenuForImage(Image* image, QWidget* menuWindow);						//menu for operations for 1 image
	void showMenuCamera();										//camera operations
	void showMenuForMultipleImages(std::vector<Image> images, QWidget* menuWindow);	//Operations with multiple images
};


#endif