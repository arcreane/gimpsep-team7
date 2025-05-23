#include "menu.h"
#include "image.h"
#include "library.h"
#include "magicPainter.h"
#include <qapplication.h>


#include <qfiledialog.h>

using namespace std;
using namespace cv;

void Menu::showMenuForMultipleImages(std::vector<Image> images, QWidget *menuWindow) { //This function displays operations for a set of images
	QDialog* window = new QDialog(menuWindow);
	window->setWindowTitle("Multiple Images Menu");
	window->setModal(true);

	QVBoxLayout* layout = new QVBoxLayout(window);
	QPushButton* btnShowImages = new QPushButton("Show Images");
	QPushButton* btnStitch = new QPushButton("Stitch Images");
	QPushButton* btnExit = new QPushButton("Back to Main Menu");

	layout->addWidget(btnShowImages);
	layout->addWidget(btnStitch);
	layout->addWidget(btnExit);

	QObject::connect(btnShowImages, &QPushButton::clicked, [=]() {
		for (Image img : images) {
			img.showImage(); // reuses OpenCV's imshow
		}
		});

	QObject::connect(btnStitch, &QPushButton::clicked, [=]() {
		std::vector<cv::Mat> imageMats;
		for ( Image img : images) {
			imageMats.push_back(img.getImage());
		}

		Image result;
		result.stitchImages(imageMats); // your existing method
		});

	QObject::connect(btnExit, &QPushButton::clicked, window, &QWidget::close);

	window->setLayout(layout);
	window->exec(); // if QDialog; or show() if QWidget

	}


void Menu::showMenuForImage(Image *image, QWidget* menuWindow) { //This function displays operations for a single image
	
	QDialog* window = new QDialog(menuWindow);
	window->setWindowTitle("One Image Menu");
	window->setModal(true);

	QVBoxLayout* layout = new QVBoxLayout(window);
	QLabel* label = new QLabel("Select an option");
	QPushButton* btnShow = new QPushButton("Show Image");
	QPushButton* btnReadNew = new QPushButton("Read New Image");
	QPushButton* btnSave = new QPushButton("Save Image");
	QPushButton* btnErosion = new QPushButton("Erosion");
	QPushButton* btnResize = new QPushButton("Resizing");
	QPushButton* btnLight = new QPushButton("Lighten/Darken");
	QPushButton* btnDilation = new QPushButton("Dilation");
	QPushButton* btnCEdge = new QPushButton("Canny Edge Detection");
	QPushButton* btnNMosaic = new QPushButton("Neural Mosaic");
	QPushButton* btnFilters = new QPushButton("Face Detection and Filters");
	QPushButton* btnExit = new QPushButton("Back to Main Menu");

	layout->addWidget(label, 0, Qt::AlignCenter);
	layout->addWidget(btnShow);
	layout->addWidget(btnReadNew);
	layout->addWidget(btnSave);
	layout->addWidget(btnErosion);
	layout->addWidget(btnResize);
	layout->addWidget(btnLight);
	layout->addWidget(btnDilation);
	layout->addWidget(btnCEdge);
	layout->addWidget(btnNMosaic);
	layout->addWidget(btnFilters);
	layout->addWidget(btnExit);

	QObject::connect(btnShow, &QPushButton::clicked, [=]() {
			image->showImage();
		});

	QObject::connect(btnReadNew, &QPushButton::clicked, [=]() {
		QString fileName = QFileDialog::getOpenFileName(window, "Select a new Image"); //Prompts user to search for the image
		if (!fileName.isEmpty()) {
			Library library;
			std::string filePath = fileName.toUtf8().constData();
			cv::Mat newImage = library.getImage(filePath);
			if (newImage.empty()) {
				QMessageBox::warning(window, "Error", QString("Image %1 could not be loaded.").arg(fileName));
			}
			else {
				*image = Image(newImage);
			}

		}
		});

	QObject::connect(btnSave, &QPushButton::clicked, [=]() {
		QString fileName = QFileDialog::getSaveFileName(window, "Save the Image");
		if (!fileName.isEmpty()) {
			Library library;
			std::string filePath = fileName.toUtf8().constData();
			library.saveImage(image->getImage(), filePath);
		}
		});

	QObject::connect(btnErosion, &QPushButton::clicked, [=]() {
		window->setEnabled(false);
		image->erosionImage();
		window->setEnabled(true);
		});

	QObject::connect(btnResize, &QPushButton::clicked, [=]() {
		window->setEnabled(false);
		image->resizeImage(window);
		window->setEnabled(true);
		});

	QObject::connect(btnLight, &QPushButton::clicked, [=]() {
		window->setEnabled(false);
		image->brightnessImage(window);
		window->setEnabled(true);
		});

	QObject::connect(btnDilation, &QPushButton::clicked, [=]() {
		window->setEnabled(false);
		image->dilationImage();
		window->setEnabled(true);
		});

	QObject::connect(btnCEdge, &QPushButton::clicked, [=]() {
		window->setEnabled(false);
		image->cannyEdgeDetection(window);
		window->setEnabled(true);
		});

	QObject::connect(btnNMosaic, &QPushButton::clicked, [=]() {
		window->setEnabled(false);
		image->neuralMosaic();
		window->setEnabled(true);
		});

	QObject::connect(btnFilters, &QPushButton::clicked, [=]() {
		window->setEnabled(false);
		image->faceDetectionAndFilters(window);
		window->setEnabled(true);
		});

	QObject::connect(btnExit, &QPushButton::clicked, window, &QWidget::close);

	window->setLayout(layout);
	window->exec();
	
	// if QDialog; or show() if QWidget
	/*
	int operation;
	Library library;
	std::string fileName;
	bool exitProgram = false;
	cv::Mat newImage;
	while (true) {
		std::cout << "Welcome to the Image Editor" << std::endl;
		std::cout << "Please type number of desired operation" << std::endl;
		std::cout << "   0: exit" << std::endl;
		std::cout << "   1: show image" << std::endl;
		std::cout << "   2: read New Image" << std::endl;
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
	*/
}

void Menu::showMenuCamera(QWidget* menuWindow) {
	


	QDialog* window = new QDialog(menuWindow);
	window->setWindowTitle("One Image Menu");
	window->setModal(true);

	QVBoxLayout* layout = new QVBoxLayout(window);
	QLabel* label = new QLabel("Select an option");
	QPushButton* btnAddColour = new QPushButton("Add Colour");
	QPushButton* btnStartPainting = new QPushButton("Start Painting");
	QPushButton* btnExit = new QPushButton("Exit");

	layout->addWidget(label,0, Qt::AlignCenter);
	layout->addWidget(btnAddColour);
	layout->addWidget(btnStartPainting);
	layout->addWidget(btnExit);

	//int operation;
	Library library;
	MagicPainter magicPainter;
	cv::Mat paintedImage;
	//std::string fileName;
	//std::string input;


	QObject::connect(btnAddColour, &QPushButton::clicked, [window, &magicPainter]() { //Capturing by reference instead of by value
		window->setEnabled(false);
		magicPainter.addColour();		
		window->setEnabled(true);
		});

	QObject::connect(btnStartPainting, &QPushButton::clicked, [window, &magicPainter, &paintedImage, &library]() {
		window->setEnabled(false);
		magicPainter.startPainting(paintedImage);

		QDialog* save = new QDialog(window);

		save->setWindowTitle("Save Image");
		save->setModal(true);
		save->setAttribute(Qt::WA_DeleteOnClose);

		QHBoxLayout* layoutH = new QHBoxLayout();
		QVBoxLayout* layoutV = new QVBoxLayout(save);
		QLabel* label = new QLabel("Do you want save the image?");
		QPushButton* btnSave = new QPushButton("Save");
		QPushButton* btnNotSave = new QPushButton("Don't save");

		layoutH->addWidget(btnSave);
		layoutH->addWidget(btnNotSave);
		layoutV->addWidget(label, 0, Qt::AlignCenter);
		layoutV->addLayout(layoutH);
		save->setLayout(layoutV);

		QObject::connect(btnSave, &QPushButton::clicked, [&library, window, &paintedImage, save]() {
		QString fileName = QFileDialog::getSaveFileName(save, "Save the Image");


		if (!fileName.isEmpty()) {
			std::string filePath = fileName.toUtf8().constData();
			library.saveImage(paintedImage.clone(), filePath);

		}
		save->close();
		});
		QObject::connect(btnNotSave, &QPushButton::clicked, [=]() {
			save->close();
			});

		save->exec();

		window->setEnabled(true);
		});


	QObject::connect(btnExit, &QPushButton::clicked, window, &QWidget::close);

	window->setLayout(layout);
	window->exec();

	/* 
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
	*/

}

void Menu::runMenu() {
	cv::Mat img;
	Image image;
	QWidget* menuWindow = new QWidget;
	menuWindow->setWindowTitle("Image Editor - Main Menu");

	QVBoxLayout* layout = new QVBoxLayout(menuWindow);

	QPushButton* btnOneImage = new QPushButton("One Image");
	QPushButton* btnMultipleImages = new QPushButton("Multiple Images");
	QPushButton* btnMagicPainter = new QPushButton("Magic Painter");
	QPushButton* btnExit = new QPushButton("Exit");

	layout->addWidget(btnOneImage);
	layout->addWidget(btnMultipleImages);
	layout->addWidget(btnMagicPainter);
	layout->addWidget(btnExit);

		QObject::connect(btnOneImage, &QPushButton::clicked, [=]() {
		QString fileName = QFileDialog::getOpenFileName(menuWindow, "Select an Image"); //Prompts user to search for the image
		if (!fileName.isEmpty()) {
			Library library;
			std::string filePath = fileName.toUtf8().constData();
			cv::Mat img = library.getImage(filePath);
			Image image(img);
			if (!img.empty()) {
				menuWindow->hide(); // hide main menu
				showMenuForImage(&image, menuWindow); // TODO change to UI
				menuWindow->show(); // resume when sub-menu closes
			}
			else {
				QMessageBox::warning(menuWindow, "Error", QString("Image %1 could not be loaded.").arg(fileName));
			}
		}
		});

		QObject::connect(btnMultipleImages, &QPushButton::clicked, [=]() {
		QStringList fileNames = QFileDialog::getOpenFileNames(menuWindow, "Select Multiple Images"); //Prompts for the user to select multiple images
		if (!fileNames.isEmpty()) {
			Library library;
			std::vector<std::string> names;
			for (const QString& fileName : fileNames) {
				std::string filePath = fileName.toUtf8().constData();
				names.push_back(filePath);
	
			}

			std::vector<Image> images = library.getImages(names);

			bool hasEmpty = false;
			for (size_t i = 0; i < images.size(); ++i) {
				if (images[i].getImage().empty()) {
					QMessageBox::warning(menuWindow, "Error", QString("Image %1 could not be loaded.").arg(fileNames[i]));
					hasEmpty = true;
				}
			}
			if (!hasEmpty) {
				menuWindow->hide();  // hide main menu
				showMenuForMultipleImages(images, menuWindow); // launch sub-menu
				menuWindow->show();  // resume when sub-menu closes
			}
		}
		});

	QObject::connect(btnMagicPainter, &QPushButton::clicked, [=]() {
		menuWindow->hide(); // hide main menu
		showMenuCamera(menuWindow); // reuse existing logic
		menuWindow->show(); // resume when sub-menu closes
		});

	QObject::connect(btnExit, &QPushButton::clicked, [=]() {
		menuWindow->close();
		});

	menuWindow->show();
}


