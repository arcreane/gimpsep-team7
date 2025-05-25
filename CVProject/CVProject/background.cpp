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

void Background::run(QWidget* parent) {
    cv::Mat frame, output;

    while (true) {
        cap >> frame;
        if (frame.empty()) break;

        // ваша логика замены фона:
        processFrame(frame, output);

        cv::imshow("Capture - SPACE. EXIT - ESC", output);
        char c = (char)cv::waitKey(30);

        if (c == 'q' || c == 27) {
            // Q или Esc — выходим
            break;
        }
        else if (c == 32) {

            // Save image dialog
            QDialog saveDialog(parent);
            saveDialog.setWindowTitle("Save Image");
            saveDialog.setModal(true);

            QVBoxLayout* layoutV = new QVBoxLayout(&saveDialog);
            QHBoxLayout* layoutH = new QHBoxLayout();

            QLabel* label2 = new QLabel("Do you want to save the image?", &saveDialog);
            QPushButton* btnSave = new QPushButton("Save as new", &saveDialog);
            QPushButton* btnNotSave = new QPushButton("Don't save", &saveDialog);

            layoutH->addWidget(btnSave);
            layoutH->addWidget(btnNotSave);
            layoutV->addWidget(label2, 0, Qt::AlignCenter);
            layoutV->addLayout(layoutH);

            bool saveConfirmed = false;

            QObject::connect(btnSave, &QPushButton::clicked, [&]() {
                while (true) {
                    QString fileName = QFileDialog::getSaveFileName(&saveDialog, "Save the Image");

                    if (fileName.isEmpty()) {
                        // User canceled the dialog
                        break;
                    }

                    QString trimmed = fileName.trimmed();
                    QString extension = QFileInfo(trimmed).suffix().toLower();

                    if (extension != "jpg" && extension != "png") {
                        QMessageBox::critical(parent, "Save Error", "File extension must be .jpg or .png.");
                        trimmed.clear(); // Force another loop iteration
                        //saveDialog.reject();
                    }
                    else {
                        QByteArray utf8 = trimmed.toUtf8();
                        std::string path(utf8.constData(), static_cast<size_t>(utf8.size()));

                        cv::imwrite(path, output);
                        saveConfirmed = true;
                        saveDialog.accept();
                        break;
                    }
                }
                });


            QObject::connect(btnNotSave, &QPushButton::clicked, [&]() {
                saveDialog.reject();
                });

            saveDialog.exec();

        }
    }

    cap.release();
    cv::destroyAllWindows();
}

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
