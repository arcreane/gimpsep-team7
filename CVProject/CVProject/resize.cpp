
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>

#include <iostream>


static int width;
static int height;
static int size;
static bool resizingFinished = false;


void static resizeImage() {
	while (true){
	std::string fileName;
	cv::Mat copyImage;
	std::string input;
	std::string textOfWindow1 = "Press [C] to confirm edit";
	std::string textOfWindow2 = "press[P] to swap between proportional";
	std::string textOfWindow3 = "resizing and separate resizing";
	bool activateProportionalScaling = false;


	resizingFinished = false;
	int key = -1;
	
	std::cout << "Welcome to the Resizing Function" << std::endl;
	std::cout << "Please enter file name" << std::endl;
	std::cin >> fileName; //Personalised file name entry


	cv::Mat image = cv::imread("../img/" + fileName);
	if (image.empty())
	{
		std::cout << "Could not read the image: " << std::endl;
		break;

	}

	//creating windows
	cv::namedWindow("Original Image", cv::WINDOW_AUTOSIZE);
	cv::namedWindow("New Image", cv::WINDOW_AUTOSIZE);
	cv::namedWindow("Trackbar Width", cv::WINDOW_AUTOSIZE);
	cv::namedWindow("Trackbar Height", cv::WINDOW_AUTOSIZE);

	cv::namedWindow("TextWindow", cv::WINDOW_AUTOSIZE);

	//Adding text
	cv::Mat imageText = cv::Mat::zeros(400, 600, CV_8UC3);  // blank image for text
	cv::putText(imageText,
		textOfWindow1,					//Text
		cv::Point(50, 70),				//Position
		cv::FONT_HERSHEY_SIMPLEX,		//Font
		0.7,							//Font Scale
		cv::Scalar(255, 255, 255),		//Color (white)
		2);								//thickness
	cv::putText(imageText,
		textOfWindow2,					//Text
		cv::Point(50, 100),				//Position
		cv::FONT_HERSHEY_SIMPLEX,		//Font
		0.7,							//Font Scale
		cv::Scalar(255, 255, 255),		//Color (white)
		2);								//thickness

	cv::putText(imageText,
		textOfWindow3,					//Text
		cv::Point(50, 130),				//Position
		cv::FONT_HERSHEY_SIMPLEX,		//Font
		0.7,							//Font Scale
		cv::Scalar(255, 255, 255),		//Color (white)
		2);								//thickness

	width = image.cols;
	height = image.rows;
	size = 100;

	cv::createTrackbar("Width", "Trackbar Width", &width, 4000);
	cv::createTrackbar("Height", "Trackbar Height", &height, 4000);


	//Button that doesn't work
	//cv::createButton("Confirm Size", onConfirmButton, 0, false);

	copyImage = image;
	cv::imshow("Original Image", image);
	cv::imshow("TextWindow", imageText);

	while (!resizingFinished && !image.empty()) {
		if (!activateProportionalScaling) {
			//Resizing with width and height

			int safeWidth = std::max(1, width); //Avoids crash with resize of 0
			int safeHeight = std::max(1, height); //Avoids crash with resize of 0

			cv::resize(image, copyImage, cv::Size(safeWidth, safeHeight), 0, 0);
		} else {

			//Resizing with proportional size
			double safeSize = std::max(1, size)/100.0; //Avoids crash with resize of 0
			cv::resize(image, copyImage, cv::Size(), safeSize, safeSize);

		}
		//Displaying edit image in real time and waiting for input
		cv::imshow("New Image", copyImage);
		key = cv::waitKey(30);
		std::cout << key << std::endl;

		if (key == 'p') { //Press p to change the type of display for the type of resizing
			activateProportionalScaling = !activateProportionalScaling;
			if (activateProportionalScaling) {
				//Case changing to proportional
				cv::destroyWindow("Trackbar Width");
				cv::destroyWindow("Trackbar Height");

				cv::namedWindow("Trackbar Proportional", cv::WINDOW_AUTOSIZE);
				cv::createTrackbar("Size", "Trackbar Proportional", &size, 400);

			}
			else {
				//Case changing to non proportional resizing
				cv::destroyWindow("Trackbar Proportional");

				cv::namedWindow("Trackbar Width", cv::WINDOW_AUTOSIZE);
				cv::namedWindow("Trackbar Height", cv::WINDOW_AUTOSIZE);

				cv::createTrackbar("Width", "Trackbar Width", &width, 4000);
				cv::createTrackbar("Height", "Trackbar Height", &height, 4000);

			}
			//Resets values so that image doesn't change sizes after changing resizing modes
			width = copyImage.cols;
			height = copyImage.rows;
			size = 100;
			image = copyImage;

		}

		if (key == 'c') {
			resizingFinished = true; //Press c to cfinish editing
		}
		std::cout << key << std::endl;
	}

	cv::destroyAllWindows();

	std::cout << "Do you want to save the image? [Y/N]" << std::endl;
	std::cin >> input;
	if (input == "Y") {
		std::cout << "Type the name of the new image (with extension)" << std::endl;
		std::cin >> input;
		cv::imwrite("../img/" + input, copyImage);
	}

	std::cout << "Do you want to go to main menu? [Y/N]" << std::endl;
	std::cin >> input;

		if (input == "Y") {
			break;
		}
	}

	
}