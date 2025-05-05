#pragma once
#ifndef MAGICPAINTER_H
#define MAGICPAINTER_H

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <iostream>

class MagicPainter
{
private:
	std::vector<std::vector<int>> colorMasks; //Stores masks for the colors as <hmin, smin, vmin, hmax, smax, vmax>
	std::vector<cv::Scalar> colorValues;      //Scalar values for rgb
	std::vector<std::vector<int>> colorPoints;//Each vector has values for position x,y and i for the index of the colour. {x,y,i}
public:
	void addColour();
	cv::Mat startPainting(cv::Mat &finalImg);
	void findColour(cv::Mat img);
	cv::Point getContourPoint(cv::Mat imgDil, cv::Mat img);
	void drawOnCanvas(cv::Mat img);
};

#endif