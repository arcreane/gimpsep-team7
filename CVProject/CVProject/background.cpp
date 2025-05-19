#include "background.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

// this is a onstructor: loads Haar cascade classifier and opens default camera
Background::Background() {

    string path = "../models/haarcascade_frontalface_default.xml";


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

bool Background::loadBackground() {
    std::string fileName;
    std::cout << "Enter background image file name with extension: ";
    std::cin >> fileName;

    const std::string fullPath = "../img/" + fileName;

    backImage = cv::imread(fullPath, cv::IMREAD_UNCHANGED);
    if (backImage.empty()) {
        std::cerr << "\nError loading background image: '" << fileName << "' . Returning to menu.\n" << std::endl;
        return false;
    }
    return true;
}

void Background::run() {
    cv::Mat frame, output;
    int photoCount = 0;

    while (true) {
        cap >> frame;
        if (frame.empty()) break;

        // the process of face detection and changing background
        processFrame(frame, output);
        cv::imshow("Background Replacement", output);

        char c = (char)cv::waitKey(30);
        if (c == 'q' || c == 27) {
            break;
        }

        // after pressing space decide to save a shot or not
        else if (c == ' ') {
            std::cout << "Do you want to save this shot? (Y/N): ";

            char choice;
            std::cin >> choice;

            if (choice == 'Y' || choice == 'y') {
                std::string filename;
                std::cout << "Enter filename (with extension): ";
                std::cin >> filename;

                std::string path = "../img/" + filename;
                if (cv::imwrite(path, output)) {
                    std::cout << "Saved to " << path << std::endl;
                }
                else {
                    std::cerr << "Error saving " << path << std::endl;
                }
            }
            else {
                break;
            }
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