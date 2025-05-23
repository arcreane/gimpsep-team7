#include "menu.h"
#include "image.h"
#include "library.h"
#include "magicPainter.h"
#include "background.h"

using namespace std;
using namespace cv;

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
			cout << "Stitching images." << endl;
			vector<Mat> imageMats;

			for (Image& img : images) {
				imageMats.push_back(img.getImage());
			}


			Image panoImage;
			panoImage.stitchImages(imageMats);
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
		std::cout << "   0: Exit" << std::endl;
		std::cout << "   1: Show image" << std::endl;
		std::cout << "   2: Read New Image" << std::endl;
		std::cout << "   3: Save Image" << std::endl;
		std::cout << "   4: Erosion" << std::endl;
		std::cout << "   5: Resizing" << std::endl;
		std::cout << "   6: Lighten/Darken" << std::endl;
		std::cout << "   7: Dilation" << std::endl;
		std::cout << "   8: Canny edge detection" << std::endl;
		std::cout << "   9: Neural Mosaic" << std::endl;
		std::cout << "   10: Face detection and filters" << std::endl;
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
			std::cout << "Chosen: Erosion" << std::endl;
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
			std::cout << "Chosen: Dilation" << std::endl;
			image->dilationImage();
			break;
		case 8:
			std::cout << "Chosen: Canny edge detection" << std::endl;
			image->cannyEdgeDetection();
			break;
		case 9:
			std::cout << "Chosen: Neural Mosaic" << std::endl;
			image->neuralMosaic();
			break;
		case 10:
			std::cout << "Chosen: Face detection and filters" << std::endl;
			image->faceDetectionAndFilters();
			break;
		default:
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
		std::cout << "   5: Change background" << std::endl;

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
			if (libImage.empty()) break;
			image = Image(libImage);
			showMenuForImage(&image);
			break;

		case 2: {
			std::cout << "Please enter number of images" << std::endl;
			std::cin >> numberOfImages;
			for (int i = 0; i < numberOfImages; i++) {
				std::cout << "Please file name of image" << i + 1 << std::endl;
				std::cin >> fileName;
				imagesNames.push_back(fileName);
			}

			images = library.getImages(imagesNames);

			bool hasEmpty = false;
			for (size_t i = 0; i < images.size(); ++i) {
				if (images[i].getImage().empty()) {
					std::cout << "Warning: Image " << imagesNames[i] << " could not be loaded or is empty." << std::endl;
					hasEmpty = true;
				}
			}
			if (hasEmpty) {
				std::cout << "Cannot continue with empty images. Please check filenames." << std::endl;
				break;
			}

			showMenuForMultipleImages(images);
			break;
		}
		case 3:
			//showMenuForVideoCapture();
			break;
		case 4:
			showMenuCamera();
			break;
		case 5:
			Background back;
			if (back.loadBackground()) {
				back.run();
			}
			break;
		}


		if (exitProgram) {
			break;
		}
	}
}


