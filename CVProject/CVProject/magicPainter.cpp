#include "magicPainter.h"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>

cv::Point MagicPainter::getContourPoint(cv::Mat mask, cv::Mat img) {
	std::vector<std::vector<cv::Point>> contours;

	cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE); //gets the contours

	std::vector<std::vector<cv::Point>> conPoly(contours.size());
	std::vector<cv::Rect> boundRect(contours.size());

	cv::Point myPoint(0,0); //where we will draw

	for (int i = 0; i < contours.size(); i++) {
		int area = cv::contourArea(contours[i]);
		std::cout << area << std::endl; //Get the area for filtering out small contours (noise)



		if (area > 1000) { //the area filter
			float perimeter = cv::arcLength(contours[i], true);
			cv::approxPolyDP(contours[i], conPoly[i], 0.02 * perimeter, true); //approximation of shape

			std::cout << conPoly[i] << std::endl;
			boundRect[i] = cv::boundingRect(conPoly[i]); //bounding rectangle of the approximation

			myPoint.x = boundRect[i].x + boundRect[i].width / 2; //Divided by 2 bc draw from the center of boundingRect and not the edge
			myPoint.y = boundRect[i].y; //This will mark the top of it
			
		}
	}

	return myPoint;
}


void MagicPainter::addColour() {
	cv::VideoCapture cap(0);
	cv::Mat img, imgHSV, mask;
	cv::Scalar colorValue;
	std::vector<int> colorMask;
	std::string key;
	bool keepPainting = true;
	int hmin = 0, smin = 0, vmin = 0;
	int hmax = 179, smax = 255, vmax = 255;
	int red = 0, green = 0, blue = 0;


	//Sets parameters for the mask
	cv::namedWindow("Trackbars" ); 
	cv::createTrackbar("Hue Min", "Trackbars", &hmin, 179);
	cv::createTrackbar("Hue Max", "Trackbars", &hmax, 179);
	cv::createTrackbar("Sat Min", "Trackbars", &smin, 255);
	cv::createTrackbar("Sat Max", "Trackbars", &smax, 255);
	cv::createTrackbar("Val Min", "Trackbars", &vmin, 255);
	cv::createTrackbar("Val Max", "Trackbars", &vmax, 255);
	cv::resizeWindow("Trackbars", cv::Size(400, 400));

	//Sets parameters for the colour that it will paint
	cv::namedWindow("Trackbars Color", cv::WINDOW_NORMAL); 
	cv::createTrackbar("Red", "Trackbars Color", &red, 255);
	cv::createTrackbar("Green", "Trackbars Color", &green, 255);
	cv::createTrackbar("Blue", "Trackbars Color", &blue, 255);
	cv::resizeWindow("Trackbars Color", cv::Size(400, 400));

	//Windows 
	cv::namedWindow("Mask", cv::WINDOW_NORMAL); 
	cv::namedWindow("Image", cv::WINDOW_NORMAL); 
	cv::namedWindow("Color", cv::WINDOW_NORMAL); 

	//Adjusting window position
	cv::moveWindow("Mask", 100, 100);
	cv::moveWindow("Image", 450, 100);
	cv::moveWindow("Color", 100, 500);
	cv::moveWindow("Trackbars", 1200, 100);
	cv::moveWindow("Trackbars Color", 1200, 400);



	while (true) {

		cv::Scalar lower(hmin, smin, vmin);
		cv::Scalar upper(hmax, smax, vmax);
		cap.read(img);
		cv::cvtColor(img, imgHSV, cv::COLOR_BGR2HSV); //Changes the imagae to hsv
		cv::inRange(imgHSV, lower, upper, mask); //outputs the mask

		cv::Mat colorImg(600, 600, CV_8UC3 , cv::Scalar(blue, green, red));

		imshow("Image", img);
		imshow("Mask", mask);
		imshow("Color", colorImg);

		key = cv::waitKey(27);
		if (key == "c") {
			break;
		}
	}
	//Saves the color mask and scalar to class variables
	colorMask = { hmin, smin, vmin, hmax, smax, vmax };
	colorValue = cv::Scalar(blue,green,red);
	colorMasks.push_back(colorMask);
	colorValues.push_back(colorValue);

	cv::destroyAllWindows();

}

void MagicPainter::findColour(cv::Mat img) {
	cv::Mat imgHSV;
	cv::cvtColor(img, imgHSV, cv::COLOR_BGR2HSV);
	for (int i = 0; i < colorMasks.size(); i++) {
		cv::Mat mask;
		cv::Scalar lower(colorMasks[i][0], colorMasks[i][1], colorMasks[i][2]);
		cv::Scalar upper(colorMasks[i][3], colorMasks[i][4], colorMasks[i][5]);
		cv::inRange(imgHSV, lower, upper, mask);
		imshow("Mask for " + i, mask);
		cv::Point drawPoint = getContourPoint(mask, img); //Get point where it will draw

		if (drawPoint.x != 0 && drawPoint.y != 0) { //Check that it has detected a color
			colorPoints.push_back({ drawPoint.x, drawPoint.y, i }); //Push back the vectors
		}
	}
}

void MagicPainter::drawOnCanvas(cv::Mat img) {
	for (int i = 0; i < colorPoints.size(); i++) {
		cv::circle(img, cv::Point(colorPoints[i][0],colorPoints[i][1]), 10, colorValues[colorPoints[i][2]],cv::FILLED);
	}

}
cv::Mat MagicPainter::startPainting(cv::Mat &finalImg) {

	cv::VideoCapture cap(0);
	cv::Mat img;
	std::string key;
	bool keepPainting = true;

	cv::namedWindow("Image");

	while (keepPainting) {
		cap.read(img);
		findColour(img);
		drawOnCanvas(img);

	    cv::imshow("Image", img);


		key = cv::waitKey(27);
		if (key == "c") {
			keepPainting = false;
			finalImg = img.clone();
		}
	}
	cv::destroyAllWindows();

	return finalImg;
}