#pragma once

#include <QDialog>
#include <QVBoxLayout>
#include <QPushButton>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
class faceDetection
{
private:
	cv::Mat filter;
	int filter_id;
	cv::Mat image;
	std::vector<cv::Rect> faces;
	std::vector<cv::Rect> eyes;
	std::vector<cv::Rect> smiles;
public:
	void setImage(cv::Mat& image);
	int const getFilterId();
	cv::Mat const getImage();
	std::vector<cv::Rect> getFaces();
	std::vector<cv::Rect> getEyes();
	std::vector<cv::Rect> getSmiles();

	cv::Mat detectFaces();
	void selectFilter(QDialog* window);
	void overlayImage(cv::Point pos);
	void applyFilterEyes(int yOffset = 0);
	void applyFilterSmile(double scale = 1, int yOffset = 0);
	void applyFilter();
};