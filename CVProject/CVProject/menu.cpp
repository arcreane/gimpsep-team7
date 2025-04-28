#include "menu.h"
#include "image.h"

void Menu::runMenu() {
	std::string fileName;

	std::cout << "Please type the file name" << std::endl;
	std::cin >> fileName;
	image = new Image(fileName);

	while (true) {
		int typeOfFile; //0 == image, 1 == video, 2==exit
		int operation;
		bool exitProgram = false;


		std::cout << "Welcome to the Image Editor" << std::endl;
		std::cout << "Please type number of desired operation" << std::endl;
		std::cout << "   0: exit" << std::endl;
		std::cout << "   1: show image" << std::endl;
		std::cout << "   2: read New Image" << std::endl;
		std::cout << "   3: Save Image" << std::endl;
		std::cout << "   4: Dilation/Erosion" << std::endl;
		std::cout << "   5: Resizing" << std::endl;
		std::cout << "   6: Lighten/Darken" << std::endl;
		std::cout << "   7: Panorama/Stitching" << std::endl;
		std::cout << "   8: Canny edge detection" << std::endl;
		std::cin >> operation;

		switch (operation)
		{
		case 0:
			exitProgram = true;
			break;
		case 1:
			image->showImage();
			break;
		case 2:
			std::cout << "Please type the file name" << std::endl;
			std::cin >> fileName;
			image = new Image(fileName);
			break;
		case 3:
			image->saveImage();
			break;
		case 4:
			std::cout << "Chosen: Dilation/Erosion" << std::endl;
			//Function for dilation
			break;

		case 5:
			std::cout << "Chose: Resizing" << std::endl;
			image->resizeImage(); 	//Function for Resizing
			break;

		case 6:
			std::cout << "Chosen: Lighten/Darken" << std::endl;
			image->brightnessImage();
			break;

		case 7:
			std::cout << "Chosen: Panorama/Stitching" << std::endl;
			break;

		case 8:
			std::cout << "CHosen: Canny edge detection" << std::endl;
			break;
		}

		cv::destroyAllWindows();

		if (exitProgram) {
			break;
		}
	}
}