#include "background.h"
#include "library.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

#include <qapplication.h>
#include <QDir>
#include <qfiledialog.h>
#include <QMessageBox>

// this is a onstructor: loads Haar cascade classifier and opens default camera
Background::Background() {

    std::string path = "../models/haarcascade_frontalface_default.xml";


    // the face detection model loading
    if (!faceCascade.load(path)) {
        std::cerr << "Error loading cascade file: " << path << std::endl;
        throw std::runtime_error("Cascade load failure");
    }

    cap.open(0);

    if (!cap.isOpened()) {
        std::cerr << "Error opening camera" << std::endl;
        throw std::runtime_error("Camera open failure");
    }
}

bool Background::loadBackground(QWidget* parent) {

    QString qPath = QFileDialog::getOpenFileName(
        parent,
        "Select background image",
        QDir::currentPath()         
    );
    if (qPath.isEmpty()) {
        return false;
    }

    std::string path = qPath.toUtf8().constData();

    backImage = cv::imread(path, cv::IMREAD_UNCHANGED);
    if (backImage.empty()) {
        QMessageBox::warning(
            parent,
            "Error Loading Background",
            QString("Could not load image:\n%1").arg(qPath)
        );
        return false;
    }

    return true;
}

void Background::run() {
	cv::Mat frame, output;

	while (true) {
		cap >> frame;
		if (frame.empty()) break;

		// ваша логика замены фона:
		processFrame(frame, output);

		cv::imshow("Background Replacement", output);
		char c = (char)cv::waitKey(30);

		if (c == 'q' || c == 27) {
			// Q или Esc — выходим
			break;
		}
		else if (c == ' ') {
			auto ret = QMessageBox::question(
				nullptr,
				"Save this shot?",
				"Do you want to save the current frame?",
				QMessageBox::Yes | QMessageBox::No
			);
			if (ret == QMessageBox::Yes) {
				// Открываем QFileDialog для выбора имени/пути
				QString savePath = QFileDialog::getSaveFileName(
					nullptr,
					"Save Image",
					QDir::currentPath(),
					"Images (*.png *.jpg *.bmp)"
				);
				if (!savePath.isEmpty()) {
					bool ok = cv::imwrite(savePath.toStdString(), output);
					if (ok) {
						QMessageBox::information(
							nullptr,
							"Saved",
							"Image successfully saved to:\n" + savePath
						);
					}
					else {
						QMessageBox::critical(
							nullptr,
							"Save Error",
							"Failed to save image to:\n" + savePath
						);
					}
				}
				// После сохранения продолжаем цикл (не выходим)
			}
			// если ответ No — просто продолжаем цикл
		}
	}

	cap.release();
	cv::destroyAllWindows();
}

//void Background::run() {
//    cv::Mat frame, output;
//    int photoCount = 0;
//
//    while (true) {
//        cap >> frame;
//        if (frame.empty()) break;
//
//        // the process of face detection and changing background
//        processFrame(frame, output);
//        cv::imshow("Background Replacement", output);
//
//        char c = (char)cv::waitKey(30);
//        if (c == 'q' || c == 27) {
//            break;
//        }
//
//        // after pressing space decide to save a shot or not
//        else if (c == ' ') {
//            std::cout << "Do you want to save this shot? (Y/N): ";
//
//            char choice;
//            std::cin >> choice;
//
//            if (choice == 'Y' || choice == 'y') {
//                std::string filename;
//                std::cout << "Enter filename (with extension): ";
//                std::cin >> filename;
//
//                std::string path = "../img/" + filename;
//                if (cv::imwrite(path, output)) {
//                    std::cout << "Saved to " << path << std::endl;
//                }
//                else {
//                    std::cerr << "Error saving " << path << std::endl;
//                }
//            }
//            else {
//                break;
//            }
//        }
//    }
//    cap.release();
//    cv::destroyAllWindows();
//}

void Background::processFrame(const cv::Mat& frame, cv::Mat& output) {
    // detect faces using Haar cascade
    std::vector<cv::Rect> faces;
    faceCascade.detectMultiScale(frame, faces, 1.1, 3);

    // create a mask, the sane size as a frame
    cv::Mat mask = cv::Mat::zeros(frame.size(), CV_8UC1);

    for (auto& f : faces) {
        // region to include the head and neck
        cv::Rect region(
            std::max(0, f.x - f.width / 4),
            std::max(0, f.y - f.height / 4),
            std::min(frame.cols - f.x, f.width + f.width / 2),
            std::min(frame.rows - f.y, f.height + f.height / 2)
        );
        // fill that region in the mask with white
        cv::rectangle(mask, region, cv::Scalar(255), cv::FILLED);
    }
    // add a border around face
    cv::GaussianBlur(mask, mask, cv::Size(21, 21), 0);

    cv::Mat background;
    if (!backImage.empty()) {
        cv::resize(backImage, background, frame.size());
    }
    else {
        cv::GaussianBlur(frame, background, cv::Size(55, 55), 0);
    }

    // mask composite
    cv::Mat front, back, invMask;

    frame.copyTo(front, mask);

    cv::bitwise_not(mask, invMask);
    background.copyTo(back, invMask);

    // combine both layers front and back
    output = front + back;
}