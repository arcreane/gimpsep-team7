#pragma once
#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <opencv2/opencv.hpp>

class Background {
public:
    Background();
    bool loadBackground();
    void run();

private:
    cv::Mat backImage;
    cv::CascadeClassifier faceCascade;
    cv::VideoCapture cap;
    void processFrame(const cv::Mat& frame, cv::Mat& output);
};

#endif