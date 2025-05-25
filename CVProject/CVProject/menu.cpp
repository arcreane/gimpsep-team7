#include "menu.h"
#include "image.h"
#include "library.h"
#include "magicPainter.h"
#include <qapplication.h>
#include "background.h"

#include <qfiledialog.h>

using namespace std;
using namespace cv;

void Menu::showMenuForMultipleImages(std::vector<Image> images, QWidget* menuWindow) { //This function displays operations for a set of images
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

	Image stitch;
	QObject::connect(btnStitch, &QPushButton::clicked, [window, &stitch, &images]() {
		window->setEnabled(false);
		stitch.stitchImages(images, window);
		window->setEnabled(true);
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
	QPushButton* btnExport = new QPushButton("Export Image");
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
	layout->addWidget(btnExport);
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

	QObject::connect(btnExport, &QPushButton::clicked, [image, window]() {
		QString fileName = QFileDialog::getSaveFileName(window, "Export the Image");
		if (!fileName.isEmpty()) {
			Library library;
			std::string filePath = fileName.toUtf8().constData();
			library.saveImage(image->getImage(), filePath);
		}
		});

	QObject::connect(btnErosion, &QPushButton::clicked, [=]() {
		window->setEnabled(false);
		image->erosionImage(window);
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
		image->dilationImage(window);
		window->setEnabled(true);
		});

	QObject::connect(btnCEdge, &QPushButton::clicked, [=]() {
		window->setEnabled(false);
		image->cannyEdgeDetection(window);
		window->setEnabled(true);
		});

	QObject::connect(btnNMosaic, &QPushButton::clicked, [=]() {
		window->setEnabled(false);
		image->neuralMosaic(window);
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

	Library library;
	MagicPainter magicPainter;
	cv::Mat paintedImage;


	QObject::connect(btnAddColour, &QPushButton::clicked, [window, &magicPainter]() { //Capturing by reference instead of by value
		window->setEnabled(false);

		// Help dialog
		QDialog* pressC = new QDialog(window);
		pressC->setWindowTitle("Color Selection Tutorial");
		pressC->setModal(true);
		pressC->setAttribute(Qt::WA_DeleteOnClose);
		QVBoxLayout* layoutHelp = new QVBoxLayout(pressC);
		QLabel* help1 = new QLabel("Move the trackbars so that the only white thing in the mask is the color you with to paint with.");
		QLabel* help2 = new QLabel("Move the trackbars colors to determine the color you want to paint, shown in the window color.");
		QLabel* help3 = new QLabel("Press [C] once you have the color and mask that you desire.");

		QPushButton* btnCloseHelp = new QPushButton("Continue");
		layoutHelp->addWidget(help1);
		layoutHelp->addWidget(help2);
		layoutHelp->addWidget(help3);
		layoutHelp->addWidget(btnCloseHelp);
		QObject::connect(btnCloseHelp, &QPushButton::clicked, [=]() {
			pressC->accept();
			});
		pressC->exec();

		magicPainter.addColour();		
		window->setEnabled(true);
		});

	QObject::connect(btnStartPainting, &QPushButton::clicked, [window, &magicPainter, &paintedImage, &library]() {
		window->setEnabled(false);

		// Help dialog
		QDialog* pressC = new QDialog(window);
		pressC->setWindowTitle("Color Selection Tutorial");
		pressC->setModal(true);
		pressC->setAttribute(Qt::WA_DeleteOnClose);
		QVBoxLayout* layoutHelp = new QVBoxLayout(pressC);
		QLabel* help1 = new QLabel("Move around the object you detected in order to paint with it.");
		QLabel* help2 = new QLabel("Press [C] once you have the color and mask that you desire.");

		QPushButton* btnCloseHelp = new QPushButton("Continue");
		layoutHelp->addWidget(help1);
		layoutHelp->addWidget(help2);
		layoutHelp->addWidget(btnCloseHelp);
		QObject::connect(btnCloseHelp, &QPushButton::clicked, [=]() {
			pressC->accept();
			});
		pressC->exec();



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
	QPushButton* btnBackground = new QPushButton("Dynamic Background");
	QPushButton* btnExit = new QPushButton("Exit");

	layout->addWidget(btnOneImage);
	layout->addWidget(btnMultipleImages);
	layout->addWidget(btnMagicPainter);
	layout->addWidget(btnBackground);
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
				showMenuForImage(&image, menuWindow);
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

	QObject::connect(btnBackground, &QPushButton::clicked, [=]() {
		menuWindow->hide();
		Background back;
		if (back.loadBackground(menuWindow)) {
			back.run(menuWindow);
		}
		menuWindow->show();
		});

	QObject::connect(btnExit, &QPushButton::clicked, [=]() {
		menuWindow->close();
		});

	menuWindow->show();
}


