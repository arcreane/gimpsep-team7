#include "image.h"
#include <opencv2/stitching.hpp>
#include "faceDetection.h"

using namespace std;
using namespace cv;

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
			//Displaying edit image in real time and waiting for input_int
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

	std::cout << "Press [C] To CONFIRM " << std::endl;

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
	} else if (input == "N") {
		image = originalImage;   // restores old image
	}
}


void Image::dilationImage() {
	cv::Mat dilatedImage;
	cv::Mat originalImage = image.clone(); // backup
	int dilationSize = 1;
	int key = -1;
	std::string input;

	// create windows
	cv::namedWindow("Dilation Trackbar", cv::WINDOW_AUTOSIZE);
	cv::namedWindow("Original Image", cv::WINDOW_AUTOSIZE);
	cv::namedWindow("Dilated Image", cv::WINDOW_AUTOSIZE);

	cv::createTrackbar("Dilation Size", "Dilation Trackbar", &dilationSize, 20);

	std::cout << "Press [C] To CONFIRM " << std::endl;

	while (true) {
		int kernelSize = 2 * dilationSize + 1;
		cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT,
													cv::Size(kernelSize, kernelSize));
		cv::dilate(originalImage, dilatedImage, element);

		cv::imshow("Original Image", originalImage);
		cv::imshow("Dilated Image", dilatedImage);

		key = cv::waitKey(30);
		if (key == 'c') break; // confirm edit
	}

	image = dilatedImage; // save the dilated image into the object
	cv::destroyAllWindows();

	std::cout << "Do you want to confirm changes? [Y/N]" << std::endl;
	std::cin >> input;
	if (input == "Y") {
		image = dilatedImage;  // save in an object
	} else if (input == "N") {
		image = originalImage;   // restores old image
	} // revert the image

}


int cannyLowThreshold = 100;
int cannyHighThreshold = 200;
bool cannyTrackbarCallbackActive = true;
//These callbacks are used to forbid the user to set a low threshold higher than the high threshold and vice versa
//How it works is if the user tries to set a low threshold higher than the high threshold, the bar freezes not letting it to go higher
//The same happens with the high threshold, but in reverse
//cannyTrackbarCallbackActive is used to avoid crash, since setTrackbarPos moves the bar what triggers the callback again, which makes a recursive loop.
//With cannyTrackbarCallbackActive set on false, the bar position is just corrected once instead of infinite times.
void onLowThresholdChange(int, void*) {
	if (!cannyTrackbarCallbackActive) return;

	cannyTrackbarCallbackActive = false;
	if (cannyLowThreshold > cannyHighThreshold) {
		cannyLowThreshold = cannyHighThreshold; 
		cv::setTrackbarPos("Low Threshold", "Canny Edge Trackbars", cannyLowThreshold);
	}
	cannyTrackbarCallbackActive = true;
}

void onHighThresholdChange(int, void*) {
	if (!cannyTrackbarCallbackActive) return;

	cannyTrackbarCallbackActive = false;
	if (cannyHighThreshold < cannyLowThreshold) {
		cannyHighThreshold = cannyLowThreshold;
		cv::setTrackbarPos("High Threshold", "Canny Edge Trackbars", cannyHighThreshold);
	}
	cannyTrackbarCallbackActive = true;
}

void Image::cannyEdgeDetection()
{
	cv::Mat originalImage = image;
	cv::Mat processedImage;
	std::string input;
	int kernelSize = 3;
	int key = -1;
	// windows creation
	cv::namedWindow("Original Image", cv::WINDOW_AUTOSIZE);
	cv::namedWindow("Canny Edged Image", cv::WINDOW_AUTOSIZE);
	cv::namedWindow("Canny Edge Trackbars", cv::WINDOW_AUTOSIZE);
	// trackbar creation
	cv::createTrackbar("Low Threshold", "Canny Edge Trackbars", &cannyLowThreshold, 255, onLowThresholdChange);
	cv::createTrackbar("High Threshold", "Canny Edge Trackbars", &cannyHighThreshold, 255, onHighThresholdChange);
	cv::createTrackbar("Kernel Size", "Canny Edge Trackbars", &kernelSize, 7);
	cv::imshow("Original Image", originalImage);
	while (true) {
		int safeLowThreshold = std::max(1, cannyLowThreshold);
		int safeHighThreshold = std::max(1, cannyHighThreshold);
		// kernel size must be odd and greater than 3
		if (kernelSize < 3) {
			kernelSize = 3;
		}
		if (kernelSize % 2 == 0) {
			kernelSize++;
		}
		int safeKernelSize = kernelSize;
		// applying canny edge detection
		cv::Canny(originalImage, processedImage, safeLowThreshold, safeHighThreshold, safeKernelSize,true);
		// result
		cv::imshow("Canny Edged Image", processedImage);
		//waiting for confirm
		key = cv::waitKey(30);
		if (key == 'c') break;
	}

	//save edge detection
	std::cout << "Do you want to save the changes? [Y/N]" << std::endl;
	std::cin >> input;
	//Since this new image is just an edge detection, we're not going to overwrite the original image by default
	//Instead, we're going to ask the user if they want to save it in other file or overwrite if they want
	if (input == "Y") {
		std::cout << "Do you want to overwrite the image with the canny detection one? [Y/N]" << std::endl;
		std::cin >> input;
		if (input == "Y") {
			image = processedImage; 
		}
		else if (input == "N") {
			std::cout << "The image will be saved inside images folder instead." << std::endl;
			std::cout << "Type the name of the new image(with extension)" << std::endl;
			std::cin >> input;
			cv::imwrite("../img/" + input, processedImage);
		}
	}
}


void Image::stitchImages(const vector<Mat>& images) {
	Mat pano;
	Ptr<Stitcher> stitcher = Stitcher::create(Stitcher::PANORAMA);

	Stitcher::Status status = stitcher->stitch(images, pano);

	if (status != Stitcher::OK) {
		cout << "\n\nError during stitching. Error code: " << int(status) << endl;
		cout << "Try using better images with more texture or overlap.\n\n" << endl;
		return;
	}

	image = pano;
	width = image.cols;
	height = image.rows;

	cout << "Panorama created successfully" << endl;

	namedWindow("Panorama", WINDOW_NORMAL);
	imshow("Panorama", pano);
	cv::waitKey(0);
	destroyAllWindows();

	string save;
	cout << "Do you want to save a panorama?" << endl;
	cin >> save;

	if (save == "y" || save == "Y") {
		string fileName;

		cout << "Enter filename (with extension): ";
		cin >> fileName;

		imwrite("../img/" + fileName, pano);
		cout << "Image saved to ../img/" + fileName  << "\n\n" << endl;
	}
}

void Image::faceDetectionAndFilters() {
	faceDetection fD; //Separated class to keep code clean
	bool exitProgram = false;

	fD.setImage(this->image); //First load the image in the class
	cv::Mat facesDetected = fD.detectFaces(); //Detect face in the image
	cv::imshow("Face Detection", facesDetected);
	cv::waitKey(0);
	//Checks if the window was closed manually, if not, it closes itself
	if (cv::getWindowProperty("Face Detection", cv::WND_PROP_VISIBLE) >= 1) {
		cv::destroyWindow("Face Detection");
	}

	while (!exitProgram) {
		fD.setImage(this->image); //Update the image after every loop to apply the changes.
		exitProgram = fD.selectFilter(); //Method returns true if the user selected to exit the program
		if (!exitProgram){
			fD.applyFilter();
			imshow("Filtered Image", fD.getImage());
			cv::waitKey(0);
			if (cv::getWindowProperty("Filtered Image", cv::WND_PROP_VISIBLE) >= 1) {
				cv::destroyWindow("Filtered Image");
			}
			std::string input;
			std::cout << "Do you want to apply the filter? [Y/N]" << std::endl;
			std::cin >> input;
			if (input == "Y") {
				this->image = fD.getImage();
			}
		} else break;
	}
	cv::destroyAllWindows();
}