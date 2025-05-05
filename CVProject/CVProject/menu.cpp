#include "menu.h"
#include "image.h"
#include "library.h"
#include "magicPainter.h"



void Menu::showMenuForMultipleImages(std::vector<Image> images) { //This function displays operations for a set of images
	int operation;
	Library library;
	bool exitProgram = false;
	while (true) {
		std::cout << "Welcome to the Image Editor" << std::endl;
		std::cout << "Please type number of desired operation" << std::endl;
		std::cout << "   0: exit" << std::endl;
		std::cout << "   1: show images" << std::endl;
		std::cout << "   2: Stitching" << std::endl;

		std::cin >> operation;

		switch (operation)
		{
		case 0:
			exitProgram = true;
			break;
		case 1:
			for (int i = 0; i < images.size(); i++)
			{
				images.at(i).showImage();
			}
			break;
		case 2:
			//TODO stitching function here
			break;
		}

		cv::destroyAllWindows();

		if (exitProgram) {
			break;
		}
	}

	}


void Menu::showMenuForImage(Image *image) { //This function displays operations for a single image
	int typeOfFile; //0 == image, 1 == video, 2==exit
	int operation;
	cv::Mat newImage;
	std::string fileName;
	Library library;
	bool exitProgram = false;
	while (true) {
		std::cout << "Welcome to the Image Editor" << std::endl;
		std::cout << "Please type number of desired operation" << std::endl;
		std::cout << "   0: exit" << std::endl;
		std::cout << "   1: show image" << std::endl;
		std::cout << "   2: read New Image" << std::endl;
		std::cout << "   3: Save Image" << std::endl;
		std::cout << "   4: Dilation/Erosion" << std::endl;
		std::cout << "   5: Resizing" << std::endl;
		std::cout << "   6: Lighten/Darken" << std::endl;
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
			std::cout << "Please type the new file to read" << std::endl;
			std::cin >> fileName;
			newImage = library.getImage(fileName);
			*image = Image(newImage);
			break;
		case 3:
			std::cout << "Please type the name of the file with extension" << std::endl;
			std::cin >> fileName;
			library.saveImage(image->getImage(), fileName);
			break;
		case 4:
// MISSING DILATION JOSE G
			std::cout << "Chosen: Dilation/Erosion" << std::endl;
			//Method for dilation
			image->erosionImage();
			break;

		case 5:
			std::cout << "Chose: Resizing" << std::endl;
			image->resizeImage(); 	//Method for Resizing
			break;

		case 6:
			std::cout << "Chosen: Lighten/Darken" << std::endl;
			image->brightnessImage();
			break;

		case 7:
			std::cout << "CHosen: Canny edge detection" << std::endl;
			break;
		}

		cv::destroyAllWindows();

		if (exitProgram) {
			break;
		}
	}
}

void Menu::showMenuCamera() {
	int operation;
	Library library;
	MagicPainter magicPainter;
	cv::Mat paintedImage;
	std::string fileName;
	std::string input;

	bool exitProgram = false;
	while (true) {
		std::cout << "Welcome to the Image Editor" << std::endl;
		std::cout << "Please type number of desired operation" << std::endl;
		std::cout << "   0: exit" << std::endl;
		std::cout << "   1: Detect new color" << std::endl;
		std::cout << "   2: Start Painting" << std::endl;

		std::cin >> operation;

		switch (operation)
		{
		case 0:
			exitProgram = true;
			break;
		case 1:
			magicPainter.addColour();
			break;
		case 2:
			magicPainter.startPainting(paintedImage);
			cv::imshow("Image", paintedImage);
			cv::waitKey(0);
			std::cout << "Do you want to save the image? [Y/N]" << std::endl;
			std::cin >> input;
			if (input == "Y") {
				std::cout << "Please type the name of the file with extension" << std::endl;
				std::cin >> fileName;
				library.saveImage(paintedImage, fileName);
			}
			break;
		

		}

		cv::destroyAllWindows();

		if (exitProgram) {
			break;
		}
	}

}

void Menu::runMenu() {
	std::string fileName;
	Library library;


	while (true) {
		int typeOfFile; //0 == image, 1 == video, 2==exit
		int operation;
		bool exitProgram = false;
		int numberOfImages;
		cv::Mat libImage;
		Image image;

		std::vector<std::string> imagesNames;
		std::vector<Image> images;

		std::cout << "Welcome to the Image Editor" << std::endl;
		std::cout << "Please select what you will work with" << std::endl;
		std::cout << "   0: exit" << std::endl;
		std::cout << "   1: One Image" << std::endl;
		std::cout << "   2: Multiple Images" << std::endl;
		std::cout << "   3: One Video" << std::endl;
		std::cout << "   4: Magic Painter" << std::endl;

		std::cin >> operation;
		switch (operation)
		{
		case 0:
			exitProgram = true;
			break;
		case 1:
			std::cout << "Please type the file name" << std::endl;
			std::cin >> fileName;
			libImage = library.getImage(fileName); //library reads the image
			image = Image(libImage);
			showMenuForImage(&image);
			break;

		case 2:
			std::cout << "Please enter number of images" << std::endl;
			std::cin >> numberOfImages;
			for (int i = 0; i < numberOfImages; i++) {
				std::cout << "Please file name of image" << i+1<< std::endl;
				std::cin >> fileName;
				imagesNames.push_back(fileName);
			}

			images = library.getImages(imagesNames);

			showMenuForMultipleImages(images);
			break;
		case 3:
			//showMenuForVideoCapture();
			break;
		case 4:
			showMenuCamera();
			break;
		}


		if (exitProgram) {
			break;
		}
	}
}


