#include "faceDetection.h"
#include <iostream>

void faceDetection::setImage(cv::Mat& image)
{
	this->image = image.clone();
}

int const faceDetection::getFilterId()
{
	return this->filter_id;
}

cv::Mat const faceDetection::getImage()
{
	return this->image;
}

std::vector<cv::Rect> faceDetection::getFaces()
{
	return this->faces;
}

std::vector<cv::Rect> faceDetection::getEyes()
{
	return this->eyes;
}

std::vector<cv::Rect> faceDetection::getSmiles()
{
	return this->smiles;
}

cv::Mat faceDetection::detectFaces()
{
	cv::CascadeClassifier eyesCascade;
	cv::CascadeClassifier smileCascade;
	cv::CascadeClassifier faceCascade;
	eyesCascade.load("../models/haarcascade_eye.xml");
	smileCascade.load("../models/haarcascade_smile.xml");
	faceCascade.load("../models/haarcascade_frontalface_default.xml");

	cv::Mat image_gray;
	cv::Mat image_copy = this->image.clone();
	cv::cvtColor(image_copy, image_gray, cv::COLOR_BGR2GRAY); //Turns to gray to make the detection easier.
	equalizeHist(image_gray, image_gray);
	faceCascade.detectMultiScale(image_gray, this->faces);
	
	//Next loops will of course save the detected faces, with their eyes and smile
	//but also will highlight them with shapes to make sure the detection worked.
	for (const auto& face : this->faces) {
		cv::rectangle(image_copy, face, cv::Scalar(255, 0, 0), 2);
	}

	//For each face detects their eyes and smiles so it just detects them in faces, not other places.
	for (const auto& face : this->faces) {
		std::vector<cv::Rect> eyes_aux;
		cv::Mat faceRegion = image_gray(face);
		eyesCascade.detectMultiScale(faceRegion, eyes_aux);

		for (const auto& eye : eyes_aux) {
			this->eyes.push_back(eye);
			cv::Point center(face.x + eye.x + eye.width / 2, face.y + eye.y + eye.height / 2);
			int radius = cvRound((eye.width + eye.height) * 0.25);
			cv::circle(image_copy, center, radius, cv::Scalar(0, 255, 0), 2);
		}
	}

	for (const auto& face : this->faces) {
		std::vector<cv::Rect> smiles_aux;
		cv::Mat faceRegion = image_gray(face);
		smileCascade.detectMultiScale(faceRegion, smiles_aux);

		for (const auto& smile : smiles_aux) {
			if (smile.y > face.height * 0.6) {
				this->smiles.push_back(smile);
				cv::Point center(face.x + smile.x + smile.width / 2, face.y + smile.y + smile.height / 2);
				int radius = cvRound((smile.width + smile.height) * 0.25);
				cv::circle(image_copy, center, radius, cv::Scalar(0, 0, 255), 2);
			}
		}


	}
	return image_copy;
}

void faceDetection::overlayImage(cv::Point pos)
{	
	//This method will copy the filter on the image pixel by pixel, since copying it directly will result in a crash
	//because png has a 4th channel (BGRA) for alpha (transparent)
	for (int y = 0; y < this->filter.rows; ++y) {
		int fy = y + pos.y;
		if (fy < 0 || fy >= this->image.rows) continue;

		for (int x = 0; x < this->filter.cols; ++x) {
			int fx = x + pos.x;
			if (fx < 0 || fx >= this->image.cols) continue;

			cv::Vec4b pixFilter = this->filter.at<cv::Vec4b>(y, x);
			cv::Vec3b& pixImage = this->image.at<cv::Vec3b>(fy, fx);

			float alpha = pixFilter[3] / 255.0f; //Will result in a value between 1 and 0
			for (int c = 0; c < 3; ++c) {
				//This merge the image pixel with the filter pixel
				//if alpha is 0 it will show the image pixel
				//if alpha is 1 it will show the filter pixel
				//if alpha is between 0 and 1 it will result in a translucid effect
				pixImage[c] = static_cast<uchar>(pixFilter[c] * alpha + pixImage[c] * (1.0f - alpha));
			}
		}
	}

}

void faceDetection::applyFilterEyes(int yOffset)
{	
	cv::Rect eye1 = this->eyes[0];
	cv::Rect eye2 = this->eyes[1];
	cv::Rect face = this->faces[0];
	double aspectRatio = this->filter.cols / (double)this->filter.rows; //Keeps the filter as it is avoiding it to distort
	int newfilterWidth = (eye2.x + eye2.width - eye1.x) * 1.5;
	int newfilterHeight = static_cast<int>(newfilterWidth / aspectRatio);
	cv::resize(this->filter, this->filter, cv::Size(newfilterWidth, newfilterHeight));

	//Finds the center of each eye and then the center of them
	int eyeCenterY0 = eye1.y + eye1.height / 2;
	int eyeCenterY1 = eye2.y + eye2.height / 2;
	int eyeCenterX0 = eye1.x + eye1.width / 2;
	int eyeCenterX1 = eye2.x + eye2.width / 2;

	int centerX = face.x + (eyeCenterX0 + eyeCenterX1) / 2;
	int centerY = face.y + (eyeCenterY0 + eyeCenterY1) / 2;

	//From the middle point of the eyes, it goes to the upper left corner of where the filter will be to start drawing it
	int filterX = centerX - this->filter.cols / 2;
	int filterY = centerY - this->filter.rows / 2;
	filterY += yOffset; //Adds a Y offset to adjust the glasses position

	overlayImage(cv::Point(filterX, filterY));
}

void faceDetection::applyFilterSmile(double scale, int yOffset)
{
	cv::Rect smile = this->smiles[0];
	cv::Rect face = this->faces[0];
	double aspectRatio = this->filter.cols / (double)this->filter.rows;
	int newfilterWidth = smile.width * scale; //Adjust the scale of the filter to make it as we want
	int newfilterHeight = static_cast<int>(newfilterWidth / aspectRatio);
	cv::resize(this->filter, this->filter, cv::Size(newfilterWidth, newfilterHeight));
	
	//Same as eyes, finds the center of the smile and goes to the upper left corner
	int centerX = face.x + smile.x + smile.width / 2;
	int centerY = face.y + smile.y + smile.height / 2;
	int filterX = centerX - this->filter.cols / 2;
	int filterY = centerY - this->filter.rows / 2;
	filterY += yOffset; //Adds a Y offset to adjust it in case we want to place it under or over the smile
	overlayImage(cv::Point(filterX, filterY));
}

void faceDetection::selectFilter(QDialog* window)
{
	QDialog* filterSel = new QDialog(window);
	filterSel->setWindowTitle("Filter Selection");
	filterSel->setModal(true);
	filterSel->setAttribute(Qt::WA_DeleteOnClose);
	QVBoxLayout* layout = new QVBoxLayout(window);
	QPushButton* btnPixelated = new QPushButton("Pixelated Sunglasses");
	QPushButton* btnPink = new QPushButton("Pink Sunglasses");
	QPushButton* btnTranparent = new QPushButton("Transparent Sunglasses");
	QPushButton* btnSmallMoustache = new QPushButton("Small Moustache");
	QPushButton* btnBigMoustache = new QPushButton("Big Moustache");
	QPushButton* btnDogTongue = new QPushButton("Dog Tongue");
	QPushButton* btnExit = new QPushButton("Exit");
	layout->addWidget(btnPixelated);
	layout->addWidget(btnPink);
	layout->addWidget(btnTranparent);
	layout->addWidget(btnSmallMoustache);
	layout->addWidget(btnBigMoustache);
	layout->addWidget(btnDogTongue);
	layout->addWidget(btnExit);
	QObject::connect(btnPixelated, &QPushButton::clicked, [=]() {
		this->filter_id = 1;
		this->filter = cv::imread("../img/pixelated_sunglasses.png", cv::IMREAD_UNCHANGED);
		if (this->filter.empty()) {
			this->filter_id = -1; //Set to -1 to avoid applying the filter
			layout->replaceWidget(btnPixelated, new QPushButton("Error loading filter image"));
		}
		else {
			filterSel->close();
		}
		});
	QObject::connect(btnPink, &QPushButton::clicked, [=]() {
		this->filter_id = 2;
		this->filter = cv::imread("../img/pink_sunglasses.png", cv::IMREAD_UNCHANGED);
		if (this->filter.empty()) {
			this->filter_id = -1; //Set to -1 to avoid applying the filter
			layout->replaceWidget(btnPixelated, new QPushButton("Error loading filter image"));
		}
		filterSel->close();
		});
	QObject::connect(btnTranparent, &QPushButton::clicked, [=]() {
		this->filter_id = 3;
		this->filter = cv::imread("../img/transparent_glasses.png", cv::IMREAD_UNCHANGED);
		if (this->filter.empty()) {
			this->filter_id = -1; //Set to -1 to avoid applying the filter
			layout->replaceWidget(btnPixelated, new QPushButton("Error loading filter image"));
		}
		filterSel->close();
		});
	QObject::connect(btnSmallMoustache, &QPushButton::clicked, [=]() {
		this->filter_id = 4;
		this->filter = cv::imread("../img/moustache.png", cv::IMREAD_UNCHANGED);
		if (this->filter.empty()) {
			this->filter_id = -1; //Set to -1 to avoid applying the filter
			layout->replaceWidget(btnPixelated, new QPushButton("Error loading filter image"));
		}
		filterSel->close();
		});
	QObject::connect(btnBigMoustache, &QPushButton::clicked, [=]() {
		this->filter_id = 5;
		this->filter = cv::imread("../img/moustache.png", cv::IMREAD_UNCHANGED);
		if (this->filter.empty()) {
			this->filter_id = -1; //Set to -1 to avoid applying the filter
			layout->replaceWidget(btnPixelated, new QPushButton("Error loading filter image"));
		}
		filterSel->close();
		});
	QObject::connect(btnDogTongue, &QPushButton::clicked, [=]() {
		this->filter_id = 6;
		this->filter = cv::imread("../img/dog.png", cv::IMREAD_UNCHANGED);
		if (this->filter.empty()) {
			this->filter_id = -1; //Set to -1 to avoid applying the filter
			layout->replaceWidget(btnPixelated, new QPushButton("Error loading filter image"));
		}
		filterSel->close();
		});
	QObject::connect(btnExit, &QPushButton::clicked, [=]() {
		this->filter_id = -1;
		this->filter.release();
		filterSel->close();
		});
	filterSel->setLayout(layout);
	filterSel->exec(); // if QDialog; or show() if QWidget

	/*
	int input;
	std::cout << "Select filters to apply to face/s" << std::endl;
	std::cout << "0. Exit" << std::endl;
	std::cout << "1. Pixelated Sunglasses" << std::endl;
	std::cout << "2. Pink Sunglasses" << std::endl;
	std::cout << "3. Translucid Sunglasses" << std::endl;
	std::cout << "4. Small moustache" << std::endl;
	std::cout << "5. Big moustache" << std::endl;
	std::cout << "6. Dog tongue" << std::endl;
	
	std::cin >> input;
	switch (input)
	{
	case 0:
		std::cout << "Exiting face detection and filters." << std::endl;
		return true;
		break;
	case 1:
		this->filter_id = input;
		this->filter = cv::imread("../img/pixelated_sunglasses.png", cv::IMREAD_UNCHANGED);
		if (this->filter.empty()) {
			std::cerr << "Error loading filter image" << std::endl;
			input = -1;
		}
		break;
	case 2:
		this->filter_id = input;
		this->filter = cv::imread("../img/pink_sunglasses.png", cv::IMREAD_UNCHANGED);
		if (this->filter.empty()) {
			std::cerr << "Error loading filter image" << std::endl;
			input = -1;
		}
		break;
	case 3:
		this->filter_id = input;
		this->filter = cv::imread("../img/transparent_glasses.png", cv::IMREAD_UNCHANGED);
		if (this->filter.empty()) {
			std::cerr << "Error loading filter image" << std::endl;
			input = -1;
		}
		break;
	case 4:
	case 5:
		this->filter_id = input;
		this->filter = cv::imread("../img/moustache.png", cv::IMREAD_UNCHANGED);
		if (this->filter.empty()) {
			std::cerr << "Error loading filter image" << std::endl;
			input = -1;
		}
		break;
	case 6:
		this->filter_id = input;
		this->filter = cv::imread("../img/dog.png", cv::IMREAD_UNCHANGED);
		if (this->filter.empty()) {
			std::cerr << "Error loading filter image" << std::endl;
			input = -1;
		}
		break;
	default:
		break;
	}
	*/
}

void faceDetection::applyFilter() {
	switch (this->filter_id)
	{
	case 1:
	case 2:
	case 3:
		if (this->eyes.size() < 2) {
			std::cerr << "Not enough eyes detected" << std::endl;
			return;
		}
		applyFilterEyes(10);
		break;
	case 4:
		if (this->smiles.empty()) {
			std::cerr << "No smile detected" << std::endl;
			return;
		}
		applyFilterSmile(0.75, -10);
		break;
	case 5:
		if (this->smiles.empty()) {
			std::cerr << "No smile detected" << std::endl;
			return;
		}
		applyFilterSmile(1.5);
		break;
	case 6:
		if (this->smiles.empty()) {
			std::cerr << "No smile detected" << std::endl;
			return;
		}
		applyFilterSmile(1, 50);
		break;
	default:
		std::cerr << "No valid filter selected";
	}
}