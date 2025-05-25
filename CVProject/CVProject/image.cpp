#include "image.h"
#include "library.h"
#include <opencv2/stitching.hpp>
#include "faceDetection.h"

#include <QInputDialog>
#include <QMessageBox>
#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QImage>
#include <QCoreApplication>

#include "neural_mosaic.h"


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

void Image::resizeImage(QDialog* window) {
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

		
		//Save image dialog
		QDialog* save = new QDialog(window);
		save->setWindowTitle("Save Image");
		save->setModal(true);
		save->setAttribute(Qt::WA_DeleteOnClose);
		QHBoxLayout* layoutH = new QHBoxLayout();
		QVBoxLayout* layoutV = new QVBoxLayout(save);
		QLabel* label = new QLabel("Do you want to confirm changes?");
		QPushButton* btnSave = new QPushButton("Save");
		QPushButton* btnNotSave = new QPushButton("Don't save");
		layoutH->addWidget(btnSave);
		layoutH->addWidget(btnNotSave);
		layoutV->addWidget(label, 0, Qt::AlignCenter);
		layoutV->addLayout(layoutH);
		save->setLayout(layoutV);

		QObject::connect(btnSave, &QPushButton::clicked, [=]() {
			this->image = copyImage;
			save->close();
			});
		QObject::connect(btnNotSave, &QPushButton::clicked, [=]() {
			this->image = originalImage;
			save->close();
			});
		save->exec();
	
}

void Image::brightnessImage(QDialog* window) {
		cv::Mat copyImage;
		cv::Mat originalImage;
		std::string input;
		cv::Mat changedImage;

		int brightRange;
		bool activateProportionalScaling = false;

		brightness = 1;
		int key = -1;



		brightRange = QInputDialog::getInt(		//input dialog for the brightness range
			window,                           // Parent widget
			"Brightness Range",         
			"Enter brightness maximun absolute value (0:1000):", // Prompt
			0,                               // Default value
			0,                               // Minimum
			1000,                            // Maximum
			1								 // Step
		);


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



		//Save image dialog
		QDialog* save = new QDialog(window);
		save->setWindowTitle("Save Image");
		save->setModal(true);
		save->setAttribute(Qt::WA_DeleteOnClose);
		QHBoxLayout* layoutH = new QHBoxLayout();
		QVBoxLayout* layoutV = new QVBoxLayout(save);
		QLabel* label = new QLabel("Do you want to confirm changes?");
		QPushButton* btnSave = new QPushButton("Save");
		QPushButton* btnNotSave = new QPushButton("Don't save");
		layoutH->addWidget(btnSave);
		layoutH->addWidget(btnNotSave);
		layoutV->addWidget(label, 0, Qt::AlignCenter);
		layoutV->addLayout(layoutH);
		save->setLayout(layoutV);

		QObject::connect(btnSave, &QPushButton::clicked, [=]() {
			this->image = changedImage.clone();
			save->close();
			});
		QObject::connect(btnNotSave, &QPushButton::clicked, [=]() {
			save->close();
			});
		save->exec();
	
}

void Image::erosionImage(QDialog* window) {
	cv::Mat originalImage = this->image.clone();
	cv::Mat processedImage;
	int erosionSize = 1;
	int key = -1;

	// Create windows
	cv::namedWindow("Original Image", cv::WINDOW_AUTOSIZE);
	cv::namedWindow("Eroded Image", cv::WINDOW_AUTOSIZE);
	cv::namedWindow("Erosion Trackbar", cv::WINDOW_AUTOSIZE);

	cv::createTrackbar("Erosion Size", "Erosion Trackbar", &erosionSize, 20);
	cv::imshow("Original Image", originalImage);

	// Help dialog
	QDialog* pressC = new QDialog(window);
	pressC->setWindowTitle("How to confirm");
	pressC->setModal(true);
	pressC->setAttribute(Qt::WA_DeleteOnClose);
	QVBoxLayout* layout = new QVBoxLayout(pressC);
	QLabel* label1 = new QLabel("Adjust erosion size using the trackbar.");
	QLabel* label2 = new QLabel("Press [C] to confirm when finished (Close this window to continue)");
	QPushButton* btnClose = new QPushButton("Close");
	layout->addWidget(label1);
	layout->addWidget(label2);
	layout->addWidget(btnClose);
	pressC->setLayout(layout);
	QObject::connect(btnClose, &QPushButton::clicked, [=]() {
		pressC->close();
	});
	pressC->exec();

	// Processing loop
	while (true) {
		int safeSize = std::max(1, erosionSize);

		cv::Mat element = cv::getStructuringElement(
			cv::MORPH_RECT,
			cv::Size(2 * safeSize + 1, 2 * safeSize + 1),
			cv::Point(safeSize, safeSize)
		);
		cv::erode(originalImage, processedImage, element);
		cv::imshow("Eroded Image", processedImage);

		key = cv::waitKey(30);
		if (key == 'c') break;
	}
	cv::destroyAllWindows();

	// Save image dialog
	QDialog* save = new QDialog(window);
	save->setWindowTitle("Save Image");
	save->setModal(true);
	save->setAttribute(Qt::WA_DeleteOnClose);
	QHBoxLayout* layoutH = new QHBoxLayout();
	QVBoxLayout* layoutV = new QVBoxLayout(save);
	QLabel* label = new QLabel("Do you want to overwrite the image with the erosion result or save it as a new image?");
	QPushButton* btnOverwrite = new QPushButton("Overwrite");
	QPushButton* btnSave = new QPushButton("Save as new");
	QPushButton* btnNotSave = new QPushButton("Don't save");
	layoutH->addWidget(btnOverwrite);
	layoutH->addWidget(btnSave);
	layoutH->addWidget(btnNotSave);
	layoutV->addWidget(label, 0, Qt::AlignCenter);
	layoutV->addLayout(layoutH);
	save->setLayout(layoutV);

	QObject::connect(btnOverwrite, &QPushButton::clicked, [=]() {
		this->image = processedImage.clone();
		save->close();
	});
	QObject::connect(btnSave, &QPushButton::clicked, [=]() {
		QString fileName = QFileDialog::getSaveFileName(save, "Save the Image");
		if (!fileName.isEmpty()) {
			Library library;
			std::string filePath = fileName.toUtf8().constData();
			cv::imwrite(filePath, processedImage);
		}
		save->close();
	});
	QObject::connect(btnNotSave, &QPushButton::clicked, [=]() {
		save->close();
	});
	save->exec();
}



void Image::dilationImage(QDialog* window) {
	cv::Mat originalImage = this->image.clone();
	cv::Mat processedImage;
	int dilationSize = 1;
	int key = -1;

	// Create windows
	cv::namedWindow("Original Image", cv::WINDOW_AUTOSIZE);
	cv::namedWindow("Dilated Image", cv::WINDOW_AUTOSIZE);
	cv::namedWindow("Dilation Trackbar", cv::WINDOW_AUTOSIZE);

	cv::createTrackbar("Dilation Size", "Dilation Trackbar", &dilationSize, 20);
	cv::imshow("Original Image", originalImage);

	// Help dialog
	QDialog* pressC = new QDialog(window);
	pressC->setWindowTitle("How to confirm");
	pressC->setModal(true);
	pressC->setAttribute(Qt::WA_DeleteOnClose);
	QVBoxLayout* layout = new QVBoxLayout(pressC);
	QLabel* label1 = new QLabel("Adjust dilation size using the trackbar.");
	QLabel* label2 = new QLabel("Press [C] to confirm when you're satisfied (Close this window to continue)");
	QPushButton* btnClose = new QPushButton("Close");
	layout->addWidget(label1);
	layout->addWidget(label2);
	layout->addWidget(btnClose);
	pressC->setLayout(layout);
	QObject::connect(btnClose, &QPushButton::clicked, [=]() {
		pressC->close();
	});
	pressC->exec();

	// Processing loop
	while (true) {
		int safeSize = std::max(1, dilationSize);

		cv::Mat element = cv::getStructuringElement(
			cv::MORPH_RECT,
			cv::Size(2 * safeSize + 1, 2 * safeSize + 1),
			cv::Point(safeSize, safeSize)
		);
		cv::dilate(originalImage, processedImage, element);
		cv::imshow("Dilated Image", processedImage);

		key = cv::waitKey(30);
		if (key == 'c') break;
	}
	cv::destroyAllWindows();

	// Save image dialog
	QDialog* save = new QDialog(window);
	save->setWindowTitle("Save Image");
	save->setModal(true);
	save->setAttribute(Qt::WA_DeleteOnClose);
	QHBoxLayout* layoutH = new QHBoxLayout();
	QVBoxLayout* layoutV = new QVBoxLayout(save);
	QLabel* label = new QLabel("Do you want to overwrite the image with the dilation result or save it as a new image?");
	QPushButton* btnOverwrite = new QPushButton("Overwrite");
	QPushButton* btnSave = new QPushButton("Save as new");
	QPushButton* btnNotSave = new QPushButton("Don't save");
	layoutH->addWidget(btnOverwrite);
	layoutH->addWidget(btnSave);
	layoutH->addWidget(btnNotSave);
	layoutV->addWidget(label, 0, Qt::AlignCenter);
	layoutV->addLayout(layoutH);
	save->setLayout(layoutV);

	QObject::connect(btnOverwrite, &QPushButton::clicked, [=]() {
		this->image = processedImage.clone();
		save->close();
	});
	QObject::connect(btnSave, &QPushButton::clicked, [=]() {
		QString fileName = QFileDialog::getSaveFileName(save, "Save the Image");
		if (!fileName.isEmpty()) {
			Library library;
			std::string filePath = fileName.toUtf8().constData();
			cv::imwrite(filePath, processedImage);
		}
		save->close();
	});
	QObject::connect(btnNotSave, &QPushButton::clicked, [=]() {
		save->close();
	});
	save->exec();
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

void Image::cannyEdgeDetection(QDialog* window)
{	
	cv::Mat originalImage = this->image.clone();
	cv::Mat processedImage;
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
	
	//Help dialog
	QDialog* pressC = new QDialog(window);
	pressC->setWindowTitle("How to confirm");
	pressC->setModal(true);
	pressC->setAttribute(Qt::WA_DeleteOnClose);
	QVBoxLayout* layout = new QVBoxLayout(pressC);
	QLabel* labelc1 = new QLabel("Adjust thresholds until desired edges visualization");
	QLabel* labelc2 = new QLabel("Press [C] to confirm when you're finished (Close this window to continue)");
	QPushButton* btnClose = new QPushButton("Close");
	layout->addWidget(labelc1);
	layout->addWidget(labelc2);
	layout->addWidget(btnClose);
	pressC->setLayout(layout);
	QObject::connect(btnClose, &QPushButton::clicked, [=]() {
		pressC->close();
		});
	pressC->exec();
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
	cv::destroyAllWindows();

	//Save image dialog
	QDialog* save = new QDialog(window);
	save->setWindowTitle("Save Image");
	save->setModal(true);
	save->setAttribute(Qt::WA_DeleteOnClose);
	QHBoxLayout* layoutH = new QHBoxLayout();
	QVBoxLayout* layoutV = new QVBoxLayout(save);
	QLabel* label = new QLabel("Do you want to overwrite the image with the Canny Detection or save it as a new image?");
	QPushButton* btnOverwrite = new QPushButton("Overwrite");
	QPushButton* btnSave = new QPushButton("Save as new");
	QPushButton* btnNotSave = new QPushButton("Don't save");
	layoutH->addWidget(btnOverwrite);
	layoutH->addWidget(btnSave);
	layoutH->addWidget(btnNotSave);
	layoutV->addWidget(label, 0, Qt::AlignCenter);
	layoutV->addLayout(layoutH);
	save->setLayout(layoutV);

	QObject::connect(btnOverwrite, &QPushButton::clicked, [=]() {
		this->image = processedImage.clone();
		save->close();
		});
	/*QObject::connect(btnSave, &QPushButton::clicked, [=]() {
		QString fileName = QFileDialog::getSaveFileName(save, "Save the Image");
		if (!fileName.isEmpty()) {
			Library library;
			std::string filePath = fileName.toUtf8().constData();
			library.saveImage(processedImage, filePath);
		}
		save->close();
		});*/
	QObject::connect(btnNotSave, &QPushButton::clicked, [=]() {
		save->close();
		});
	save->exec();

}


void Image::stitchImages(const vector<Image>& images, QDialog* window) {
	vector<Mat> imgs;
	imgs.reserve(images.size());

	for (const auto& img : images) {
		imgs.push_back(img.image);
	}


	Mat pano;
	Ptr<Stitcher> stitcher = Stitcher::create(Stitcher::PANORAMA);

	Stitcher::Status status = stitcher->stitch(imgs, pano);

	if (status != Stitcher::OK) {
		QMessageBox::critical(window, "Error during stitching.", "Panorama could not be created.\nTry using better images with more texture or overlap.");
		return;
	}

	image = pano;
	width = image.cols;
	height = image.rows;

	// transfer pano in rgb
	cv::Mat pano_rgb;
	cv::cvtColor(pano, pano_rgb, cv::COLOR_BGR2RGB);

	QImage qimg(pano.data, pano.cols, pano.rows, pano.step, QImage::Format_BGR888);
	QPixmap pixmap = QPixmap::fromImage(qimg.copy()); // a copy

	// show image in a qt dialog
	QDialog* dialog = new QDialog(window);
	dialog->setWindowTitle("Panorama Result");

	QVBoxLayout* layout = new QVBoxLayout(dialog);
	QLabel* label = new QLabel;
	label->setPixmap(pixmap.scaled(800, 600, Qt::KeepAspectRatio));
	layout->addWidget(label);

	dialog->setLayout(layout);
	dialog->exec();

	//delete dialog;

		// saving
	QMessageBox::StandardButton reply = QMessageBox::question(
		window, "Save Panorama", "Do you want to save the panorama?",
		QMessageBox::Yes | QMessageBox::No
	);

	if (reply == QMessageBox::Yes) {
		QString filename;
		bool ok;

		QString exeDir = QCoreApplication::applicationDirPath();
		QDir imgDir(exeDir);
		imgDir.cd("../../files/img");

		do {
			filename = QInputDialog::getText(
				window, "Save Image", "Enter filename (with .jpg or .png extension):",
				QLineEdit::Normal, filename, &ok
			);

			if (!ok) {
				// User canceled input dialog
				return;
			}

			QString trimmed = filename.trimmed();
			QString extension = QFileInfo(trimmed).suffix().toLower();

			if (trimmed.isEmpty()) {
				QMessageBox::critical(window, "Save Error", "Filename cannot be empty.");
			}
			else if (extension != "jpg" && extension != "png") {
				QMessageBox::critical(window, "Save Error", "File extension must be .jpg or .png.");
				trimmed.clear(); // Force another loop iteration
			}
			else {
				QString fullPath = imgDir.filePath(trimmed);

				if (QFile::exists(fullPath)) {
					QMessageBox::StandardButton overwriteReply = QMessageBox::question(
						window, "File Exists",
						"A file with this name already exists.\nDo you want to overwrite it?",
						QMessageBox::Yes | QMessageBox::No
					);

					if (overwriteReply == QMessageBox::No) {
						continue; // back to input
					}
				}

				filename = trimmed; // valid, trimmed, and confirmed
				break; // exit loop
			}

		} while (true);


		if (ok) {
			QByteArray utf8 = filename.toUtf8();
			std::string cleanedStr(utf8.constData(), static_cast<size_t>(utf8.size()));
			std::string path = "../img/" + cleanedStr;

			QString qPath = imgDir.filePath(filename);

			if (pano.empty()) {
				QMessageBox::critical(window, "Save Error", "Cannot save empty image.");
				return;
			}

			bool success = cv::imwrite(path, pano);
			if (success) {
				QMessageBox::information(window, "Saved", "Image saved to:\n" + qPath);
			}
			else {
				QMessageBox::critical(window, "Save Error", "Failed to save image to:\n" + qPath);
			}

		}
	}

	image = pano;
}

void Image::faceDetectionAndFilters(QDialog* window) {
	faceDetection fD; //Separated class to keep code clean
	bool boolDetectOk = true;

	fD.setImage(this->image); //First load the image in the class
	cv::Mat facesDetected = fD.detectFaces(); //Detect face in the image
	cv::imshow("Face Detection", facesDetected);
	//Checks if the window was closed manually, if not, it closes itself
	QDialog* detectOk = new QDialog(window);
	detectOk->setWindowTitle("Detection Validation");
	detectOk->setModal(true);
	detectOk->setAttribute(Qt::WA_DeleteOnClose);
	QHBoxLayout* layoutOkH = new QHBoxLayout();
	QVBoxLayout* layoutOkV = new QVBoxLayout(detectOk);
	QLabel* labelOk = new QLabel("Is the detection ok? If not please select other image");
	QPushButton* btnConfirm = new QPushButton("Confirm");
	QPushButton* btnBack = new QPushButton("Go Back");
	layoutOkH->addWidget(btnConfirm);
	layoutOkH->addWidget(btnBack);
	layoutOkV->addWidget(labelOk);
	layoutOkV->addLayout(layoutOkH);
	detectOk->setLayout(layoutOkV);
	QObject::connect(btnConfirm, &QPushButton::clicked, [=]() {
		detectOk->close();
		});
	QObject::connect(btnBack, &QPushButton::clicked, [&]() {
		detectOk->close();
		boolDetectOk = false;
		});
	detectOk->exec();
	if (cv::getWindowProperty("Face Detection", cv::WND_PROP_VISIBLE) >= 1) {
		cv::destroyWindow("Face Detection");
	}
	if (!boolDetectOk) return;

	QDialog* save = new QDialog(window);
	save->setWindowTitle("Save Image");
	save->setModal(true);
	QHBoxLayout* layoutH = new QHBoxLayout();
	QVBoxLayout* layoutV = new QVBoxLayout(save);
	QLabel* label = new QLabel("Do you want to save the image with filter?");
	QPushButton* btnYes = new QPushButton("Yes");
	QPushButton* btnNo = new QPushButton("No");
	layoutH->addWidget(btnYes);
	layoutH->addWidget(btnNo);
	layoutV->addWidget(label);
	layoutV->addLayout(layoutH);
	save->setLayout(layoutV);

	QObject::connect(btnYes, &QPushButton::clicked, [&]() {
		this->image = fD.getImage().clone();
		save->close();
		});
	QObject::connect(btnNo, &QPushButton::clicked, [=]() {
		save->close();
		});

	while (fD.getFilterId() != -1) {
		fD.setImage(this->image); //Update the image after every loop to apply the changes.
		fD.selectFilter(window);
		if (fD.getFilterId() != -1) { //If the filter is -1, it means user chose to exit the filter selection
			fD.applyFilter();
			imshow("Filtered Image", fD.getImage());
			save->exec();
			if (cv::getWindowProperty("Filtered Image", cv::WND_PROP_VISIBLE) >= 1) {
				cv::destroyWindow("Filtered Image");
			}
		} else break;
	}
	cv::destroyAllWindows();
}



// Helper to convert cv::Mat to QImage
QImage matToQImage(const cv::Mat& mat) {
	if(mat.channels() == 3) {
		return QImage(mat.data, mat.cols, mat.rows, static_cast<int>(mat.step), QImage::Format_BGR888).copy();
	} else if(mat.channels() == 1) {
		return QImage(mat.data, mat.cols, mat.rows, static_cast<int>(mat.step), QImage::Format_Grayscale8).copy();
	}
	return QImage();
}

void Image::neuralMosaic(QDialog* window) {
    bool ok;
    int rows = QInputDialog::getInt(window, "Neural Mosaic", "Enter number of rows:", 10, 1, 100, 1, &ok);
    if (!ok) return;
    int cols = QInputDialog::getInt(window, "Neural Mosaic", "Enter number of columns:", 10, 1, 100, 1, &ok);
    if (!ok) return;

    std::unique_ptr<cv::Mat> result = std::make_unique<cv::Mat>(applyNeuralMosaic(this->image, rows, cols));
    if (result->empty()) {
        QMessageBox::warning(window, "Error", "Resulting image is empty!");
        return;
    }

    // Help dialog
    QDialog* pressC = new QDialog(window);
    pressC->setWindowTitle("Neural Mosaic Preview");
    pressC->setModal(true);
    pressC->setAttribute(Qt::WA_DeleteOnClose);
    QVBoxLayout* layoutHelp = new QVBoxLayout(pressC);
    QLabel* help1 = new QLabel("You are previewing the result of Neural Mosaic.");
    QLabel* help2 = new QLabel("Click 'Confirm' to continue, or 'Cancel' to discard.");
    QPushButton* btnCloseHelp = new QPushButton("Continue");
    layoutHelp->addWidget(help1);
    layoutHelp->addWidget(help2);
    layoutHelp->addWidget(btnCloseHelp);
    QObject::connect(btnCloseHelp, &QPushButton::clicked, [=]() {
        pressC->accept();
    });
    pressC->exec();

    // Show preview
    QDialog previewDialog(window);
    previewDialog.setWindowTitle("Neural Mosaic Result");
    previewDialog.setModal(true);
    QVBoxLayout* vLayout = new QVBoxLayout(&previewDialog);
    QLabel* label = new QLabel(&previewDialog);

    QImage qimg = matToQImage(*result);
    if (qimg.isNull()) {
        QMessageBox::warning(window, "Error", "Unable to display preview image.");
        return;
    }

    label->setPixmap(QPixmap::fromImage(qimg));
    vLayout->addWidget(label);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    QPushButton* btnConfirm = new QPushButton("Confirm", &previewDialog);
    QPushButton* btnCancel = new QPushButton("Cancel", &previewDialog);
    btnLayout->addStretch();
    btnLayout->addWidget(btnConfirm);
    btnLayout->addWidget(btnCancel);
    vLayout->addLayout(btnLayout);

    int dialogResult = -1;
    QObject::connect(btnConfirm, &QPushButton::clicked, [&]() {
        dialogResult = 0;
        previewDialog.accept();
    });
    QObject::connect(btnCancel, &QPushButton::clicked, [&]() {
        dialogResult = 1;
        previewDialog.reject();
    });

    if (previewDialog.exec() != QDialog::Accepted || dialogResult != 0) {
        return;
    }

    // Save image dialog
    QDialog saveDialog(window);
    saveDialog.setWindowTitle("Save Image");
    saveDialog.setModal(true);

    QVBoxLayout* layoutV = new QVBoxLayout(&saveDialog);
    QHBoxLayout* layoutH = new QHBoxLayout();

    QLabel* label2 = new QLabel("Do you want to overwrite the image with the Mosaic or save it as a new image?", &saveDialog);
    QPushButton* btnOverwrite = new QPushButton("Overwrite", &saveDialog);
    QPushButton* btnSave = new QPushButton("Save as new", &saveDialog);
    QPushButton* btnNotSave = new QPushButton("Don't save", &saveDialog);

    layoutH->addWidget(btnOverwrite);
    layoutH->addWidget(btnSave);
    layoutH->addWidget(btnNotSave);
    layoutV->addWidget(label2, 0, Qt::AlignCenter);
    layoutV->addLayout(layoutH);

    bool saveConfirmed = false;

    QObject::connect(btnOverwrite, &QPushButton::clicked, [&]() {
        this->image = result->clone();
        saveConfirmed = true;
        saveDialog.accept();
    });

    QObject::connect(btnSave, &QPushButton::clicked, [&]() {
        QString fileName = QFileDialog::getSaveFileName(&saveDialog, "Save the Image");
        if (!fileName.isEmpty()) {
            cv::Mat saveResult = *result;
            if (saveResult.depth() != CV_8U) {
                double minVal, maxVal;
                cv::minMaxLoc(saveResult, &minVal, &maxVal);
                double scale = (maxVal > minVal) ? (255.0 / (maxVal - minVal)) : 1.0;
                saveResult.convertTo(saveResult, CV_8U, scale, -minVal * scale);
            }
            cv::imwrite(fileName.toStdString(), saveResult);
            saveConfirmed = true;
        }
        saveDialog.accept();
    });

    QObject::connect(btnNotSave, &QPushButton::clicked, [&]() {
        saveDialog.reject();
    });

    saveDialog.exec();
}
