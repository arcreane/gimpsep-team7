#pragma once
#include "faceSwap.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/objdetect.hpp>

using namespace std;

bool FaceSwap::detectFace(const cv::Mat& img, cv::Rect& faceRect) {
	// loads the Haar Cascade model
	static cv::CascadeClassifier faceDetector("haarcascade_frontalface_default.xml");

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
	cv::rectangle(mask, faceRect, cv::Scalar(255), cv::FILLED);

	return mask;
}

cv::Mat FaceSwap::blendFace(const cv::Mat& warpedFace, const cv::Mat& targetImg, const cv::Mat& mask) {
	cv::Moments m = cv::moments(mask, true);
	cv::Point center(cvRound(m.m10 / m.m10), cvRound(m.m01 / m.m00));
	cv::Mat output;

	cv::seamlessClone(warpedFace, targetImg, mask, center, output, cv::NORMAL_CLONE);

	return output;
}
