#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>

#include <iostream>

static int brightness;

void static lighting() {
	while (true) {
		std::string fileName;
		cv::Mat copyImage;
		std::string input;
		std::string textOfWindow1 = "Press [C] to confirm edit";
		std::string textOfWindow2 = "press[P] to swap between proportional";
		std::string textOfWindow3 = "resizing and separate resizing";
		cv::Mat changedImage;

		int brightRange; 
		bool activateProportionalScaling = false;

		brightness = 1;
		int key = -1;

		std::cout << "Welcome to the Lighting Function" << std::endl;
		std::cout << "Please enter file name" << std::endl;
		std::cin >> fileName; //Personalised file name entry

		cv::Mat image = cv::imread("../img/" + fileName);
		if (image.empty())
		{
			std::cout << "Could not read the image: " << std::endl;
			break;

		}

		std::cout << "Please enter brightness range" << std::endl;
		std::cin >> brightRange; //Personalised brightness range entry


		//Creating windows
		cv::namedWindow("Original Image", cv::WINDOW_AUTOSIZE);
		cv::namedWindow("New Image", cv::WINDOW_AUTOSIZE);
		cv::namedWindow("Trackbar Brightness", cv::WINDOW_AUTOSIZE);

		//Creating trackbar with range
		cv::createTrackbar("Brightness", "Trackbar Brightness", &brightness, brightRange);
		cv::setTrackbarMin("Brightness", "Trackbar Brightness", -brightRange);

		cv::imshow("Original Image", image);
		copyImage = image;

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

		//Final Dialog
		cv::destroyAllWindows();

		std::cout << "Do you want to save the image? [Y/N]" << std::endl;
		std::cin >> input;
		if (input == "Y") {
			std::cout << "Type the name of the new image (with extension)" << std::endl;
			std::cin >> input;
			cv::imwrite("../img/" + input, changedImage);
		}

		std::cout << "Do you want to go to main menu? [Y/N]" << std::endl;
		std::cin >> input;

		if (input == "Y") {
			break;
		}
	}
	
}