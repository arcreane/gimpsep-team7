#pragma once
#include "faceSwap.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/objdetect.hpp>

using namespace std;

bool FaceSwap::detectFace(const cv::Mat& img, cv::Rect& faceRect) {
	// loads the Haar Cascade model
	static cv::CascadeClassifier faceDetector;
	faceDetector.load("C:/Users/sharu/source/repos/gimpsep-team7/CVProject/files/model/haarcascade_frontalface_default.xml");

	if (faceDetector.empty()) {
		return false;
	}

	cv::Mat gray;
	cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);

	vector<cv::Rect> faces;

	faceDetector.detectMultiScale(gray, faces, 1.1, 3, 0, cv::Size(100, 100));

	if (faces.empty()) {
		return false;
	}

	faceRect = faces[0];

	return true;
}

cv::Mat FaceSwap::warpFace(const cv::Mat& srcImg, const cv::Rect& srcRect,
	const cv::Rect& dstRect, const cv::Size& dstSize) {

	vector<cv::Point2f> srcPts = {
		{ (float)srcRect.x, (float)srcRect.y },
		{ (float)(srcRect.x + srcRect.width), (float)srcRect.y },
		{ (float)(srcRect.x + srcRect.width), (float)(srcRect.y + srcRect.height) },
		{ (float)srcRect.x, (float)(srcRect.y + srcRect.height) }
	};

	vector<cv::Point2f> dstPts = {
	{ (float)dstRect.x, (float)dstRect.y },
	{ (float)(dstRect.x + dstRect.width), (float)dstRect.y },
	{ (float)(dstRect.x + dstRect.width), (float)(dstRect.y + dstRect.height) },
	{ (float)dstRect.x, (float)(dstRect.y + dstRect.height) }
	};

	cv::Mat M = cv::getPerspectiveTransform(srcPts, dstPts);
	cv::Mat warped;

	cv::warpPerspective(srcImg, warped, M, dstSize, cv::INTER_LINEAR, cv::BORDER_REFLECT);

	return warped;
}

cv::Mat FaceSwap::createMask(const cv::Rect& faceRect, const cv::Size& imageSize) {
	cv::Mat mask = cv::Mat::zeros(imageSize, CV_8UC1);
	cv::Point center(faceRect.x + faceRect.width / 2, faceRect.y + faceRect.height / 2);
	cv::Size axes(faceRect.width / 2, faceRect.height / 2);
	cv::ellipse(mask, center, axes, 0, 0, 360, cv::Scalar(255), cv::FILLED);
	return mask;
}


cv::Mat FaceSwap::blendFace(const cv::Mat& warpedFace, const cv::Mat& targetImg, const cv::Mat& mask) {
	if (mask.empty() || mask.type() != CV_8UC1) {
		std::cerr << "Error: Invalid mask format." << std::endl;
		return targetImg.clone();
	}

	cv::Moments m = cv::moments(mask, true);
	if (m.m00 == 0) {
		std::cerr << "Error: Mask area is zero." << std::endl;
		return targetImg.clone();
	}

	cv::Point center(m.m10 / m.m00, m.m01 / m.m00);

	if (center.x < 0 || center.y < 0 || center.x >= targetImg.cols || center.y >= targetImg.rows) {
		std::cerr << "Error: Center point is out of bounds: " << center << std::endl;
		return targetImg.clone();
	}

	std::cout << "warpedFace size: " << warpedFace.size() << std::endl;
	std::cout << "targetImg size: " << targetImg.size() << std::endl;
	std::cout << "mask size: " << mask.size() << std::endl;
	std::cout << "Center: " << center << std::endl;

	cv::Mat output;
	try {
		cv::seamlessClone(warpedFace, targetImg, mask, center, output, cv::NORMAL_CLONE);
	}
	catch (const cv::Exception& e) {
		std::cerr << "cv::seamlessClone failed:" << e.what() << std::endl;
			return targetImg.clone();
	}

	return output;
}

