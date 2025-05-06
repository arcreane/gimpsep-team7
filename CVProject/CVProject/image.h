#pragma once
#ifndef IMAGE_H
#define IMAGE_H

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <iostream>

using namespace std;
using namespace cv;

class Image
{
private:
	int width;
	int height;
	int size;
	int brightness;
	cv::Mat image;
	//Add parameters for other functions here
public:
	Image();
	Image(cv::Mat image);
	void resizeImage();
	void brightnessImage();

	void erosionImage();  // GUI-based erosion function
	void dilationImage();

	void cannyEdgeDetection();

	void stitchImages(const vector<Mat>& images);

	void showImage();
	void saveImage();
	cv::Mat getImage();
};


#endif
