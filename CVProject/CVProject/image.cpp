#include "image.h"


Image::Image() //Empty image
{
	width = 0;
	height = 0;
	size = 0;
	brightness = 0;
	image = cv::Mat(); 
}

Image::Image(cv::Mat imageT) {
	//Initialization of values
	image = imageT;
	width = image.cols;
	height = image.rows;
	size = 100;
	brightness = 1;
}

void Image::showImage() {
	//Shows the image, and closes it when a key is pressed
	cv::namedWindow("Image");
	cv::imshow("Image", image);
	std::cout << "Press any key to continue" << std::endl;
	cv::waitKey(0);

	cv::destroyAllWindows();
}

cv::Mat Image::getImage() {
	return image;
}

void Image::saveImage() {
	std::string input;

	//Displays the image
	cv::namedWindow("Image");
	cv::imshow("Image", image);
	cv::waitKey(0);
	std::cout << "Do you want to save the image? [Y/N]" << std::endl;
	std::cin >> input;
	if (input == "Y") {
		std::cout << "Type the name of the new image (with extension)" << std::endl;
		std::cin >> input;
		cv::imwrite("../img/" + input, image);
	}
	else if (input == "N") {
		std::cout << "Going to main menu" << std::endl;
	}
	else {
		std::cout << "Unexpected input, going to main menu" << std::endl;
	}

	cv::destroyAllWindows();
}

void Image::resizeImage() {
	while (true) {
		cv::Mat copyImage;
		cv::Mat originalImage;
		std::string input;
		std::string textOfWindow1 = "Press [C] to confirm edit";
		std::string textOfWindow2 = "press[P] to swap between proportional";
		std::string textOfWindow3 = "resizing and separate resizing";
		bool activateProportionalScaling = false;
		bool resizingFinished = false;
		int key = -1;

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



		cv::createTrackbar("Width", "Trackbar Width", &width, 4000);
		cv::createTrackbar("Height", "Trackbar Height", &height, 4000);


		//Button that doesn't work
		//cv::createButton("Confirm Size", onConfirmButton, 0, false);

		copyImage = image;
		originalImage = image;
		cv::imshow("Original Image", image);
		cv::imshow("TextWindow", imageText);

		while (!resizingFinished && !image.empty()) {
			if (!activateProportionalScaling) {
				//Resizing with width and height

				int safeWidth = std::max(1, width); //Avoids crash with resize of 0
				int safeHeight = std::max(1, height); //Avoids crash with resize of 0

				cv::resize(image, copyImage, cv::Size(safeWidth, safeHeight), 0, 0);
			}
			else {

				//Resizing with proportional size
				double safeSize = std::max(1, size) / 100.0; //Avoids crash with resize of 0
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

		image = copyImage;


		cv::destroyAllWindows();
		std::cout << "Do you want to confirm changes? [Y/N]" << std::endl;
		std::cin >> input;
		if (input == "N") {
			image = originalImage;
			width = image.cols;
			height = image.rows;
		}

		std::cout << "Do you want to go to main menu? [Y/N]" << std::endl;
		std::cin >> input;

		if (input == "Y") {
			break;
		}
	}
}

void Image::brightnessImage() {
	while (true) {
		cv::Mat copyImage;
		cv::Mat originalImage;
		std::string input;
		cv::Mat changedImage;

		int brightRange;
		bool activateProportionalScaling = false;

		brightness = 1;
		int key = -1;


		std::cout << "Please enter brightness range" << std::endl;
		std::cin >> brightRange; //Personalised brightness range entry


		//Creating windows
		cv::namedWindow("Original Image", cv::WINDOW_AUTOSIZE);
		cv::namedWindow("New Image", cv::WINDOW_AUTOSIZE);
		cv::namedWindow("Trackbar Brightness", cv::WINDOW_AUTOSIZE);

		//Creating trackbar with range
		cv::createTrackbar("Brightness", "Trackbar Brightness", &brightness, brightRange);
		cv::setTrackbarMin("Brightness", "Trackbar Brightness", -brightRange);

		//Displaying original
		cv::imshow("Original Image", image);
		copyImage = image;
		originalImage = image;

		std::cout << "Press [C] To CONFIRM " << std::endl;

		while (true) {
			copyImage.convertTo(changedImage, copyImage.type(), 1, brightness);
			cv::imshow("New Image", changedImage);

			//Confirms edit
			key = cv::waitKey(30);
			if (key == 'c') {
				break; //Press c to exit
			}
		}

		image = changedImage;

		//Final Dialog
		cv::destroyAllWindows();

		std::cout << "Do you want to confirm changes? [Y/N]" << std::endl;
		std::cin >> input;
		if (input == "N") {
			image = originalImage;
		}

		std::cout << "Do you want to go to main menu? [Y/N]" << std::endl;
		std::cin >> input;

		if (input == "Y") {
			break;
		}
	}
}

void Image::erosionImage() {
	// jose g
	cv::Mat originalImage = image;
	cv::Mat processedImage;
	std::string input;
	int erosionSize = 1;
	int key = -1;

	// windows creation
	cv::namedWindow("Original Image", cv::WINDOW_AUTOSIZE);
	cv::namedWindow("Eroded Image", cv::WINDOW_AUTOSIZE);
	cv::namedWindow("Erosion Trackbar", cv::WINDOW_AUTOSIZE);

	// trackbar creation
	cv::createTrackbar("Erosion Size", "Erosion Trackbar", &erosionSize, 20);

	cv::imshow("Original Image", originalImage);

	while (true) {
		int safeSize = std::max(1, erosionSize);

		// applying erosion
		cv::Mat element = cv::getStructuringElement(
			cv::MORPH_RECT,
			cv::Size(2 * safeSize + 1, 2 * safeSize + 1),
			cv::Point(safeSize, safeSize)
		);
		cv::erode(originalImage, processedImage, element);

		// result
		cv::imshow("Eroded Image", processedImage);

		//waiting for confirm
		key = cv::waitKey(30);
		if (key == 'c') break;
	}

	// confirm/discard
	cv::destroyAllWindows();
	std::cout << "Do you want to confirm changes? [Y/N]" << std::endl;
	std::cin >> input;

	if (input == "Y") {
		image = processedImage;  // save in object
	} else {
		image = originalImage;   // restores old image
	}
}
