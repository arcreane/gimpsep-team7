#pragma once
#ifndef IMAGE_H
#define IMAGE_H

#include <QDialog>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFileDialog>
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
	void resizeImage(QDialog* window);
	void brightnessImage(QDialog* window);

	void erosionImage(QDialog* window);  // GUI-based erosion function
	void dilationImage(QDialog* window);

	void cannyEdgeDetection(QDialog* window);
	void faceDetectionAndFilters(QDialog* window);

	//void stitchImages(QDialog* window);
	//void stitchImages(const vector<Image>& images, QDialog* window);
	void stitchImages(const vector<Image>& images, QDialog* parentWindow);

	void neuralMosaic(QDialog* window);


	void showImage();
	cv::Mat getImage();
};


#endif
