#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include "resize.cpp"

int main()
{
	while (true) {
		std::string fileName;
		int typeOfFile; //0 == image, 1 == video, 2==exit
		int operation;
		bool exitProgram = false;

		std::cout << "Welcome to the Image Editor" << std::endl;

		std::cout << "HighGUI backend: " << cv::getBuildInformation() << std::endl;

	

		std::cout << "Please type number of desired operation" << std::endl;
		std::cout << "0: exit" << std::endl;
		std::cout << "1: Dilation/Erosion" << std::endl;
		std::cout << "2: Resizing" << std::endl;
		std::cout << "3: Lighten/Darken" << std::endl;
		std::cout << "4: Panorama/Stitching" << std::endl;
		std::cout << "5: Canny edge detection" << std::endl;
		std::cin >> operation;

		switch (operation)
		{
		case 0:
			exitProgram = true;
			break;
		case 1:
			std::cout << "Chosen: Dilation/Erosion" << std::endl;
			//Function for dilation
			break;

		case 2:
			std::cout << "Chose: Resizing" << std::endl;
			resizeImage(); 	//Function for Resizing

			break;

		case 3:
			std::cout << "Chosen: Lighten/Darken" << std::endl;
			break;

		case 4:
			std::cout << "Chosen: Panorama/Stitching" << std::endl;
			break;

		case 5:
			std::cout << "CHosen: Canny edge detection" << std::endl;
			break;
		}

		cv::destroyAllWindows();

		if (exitProgram) {
			break;
		}
	}

	return 0;
}