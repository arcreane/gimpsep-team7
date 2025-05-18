#pragma once

#ifndef FACESWAP_H
#define FACESWAP_H

#include <opencv2/opencv.hpp>

class FaceSwap {
private:
	cv::CascadeClassifier faceDetector;

public:
	//FaceSwap();

	// detects a single face bounding box in the image using Haar cascades
	bool detectFace(const cv::Mat& img, cv::Rect& faceRect);
	cv::Mat warpFace(const cv::Mat& srcImg, const cv::Rect& srcRect,
		const cv::Rect& dstRect, const cv::Size& dstSize);
	cv::Mat createMask(const cv::Rect& faceRect, const cv::Size& imageSize);
	cv::Mat blendFace(const cv::Mat& warpedFace, const cv::Mat& targetImg, const cv::Mat& mask);
};

#endif