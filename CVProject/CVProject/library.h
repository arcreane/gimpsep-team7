#pragma once
#pragma once

#pragma once
#ifndef LIBRARY_H
#define LIBRARY_H

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <iostream>
#include <vector>
#include "image.h"

class Library
{
private:
	//Add parameters for other functions here
public:
	cv::Mat getImage(std::string name);
	std::vector<Image> getImages(std::vector<std::string> names);
	cv::VideoCapture getVideo(std::string name);

	void saveImage(cv::Mat image, std::string fileName);
	void saveVideo(std::string filename, std::vector<cv::Mat> frames, double fps);
};

#endif
