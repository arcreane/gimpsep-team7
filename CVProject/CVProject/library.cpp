#include "library.h"
#include "image.h"
cv::Mat Library::getImage(std::string name) { //Gets an image
    cv::Mat image = cv::imread("../img/"+name);
    return image;
}
std::vector<Image> Library::getImages(std::vector<std::string> names) //Gets a vector of images
{ 
	std::vector<Image> images;
	for (std::string name : names) {
		cv::Mat imageFile = getImage(name);
        Image image = Image(imageFile);
		images.push_back(image);
	}

	return images;
}
cv::VideoCapture Library::getVideo(std::string name) //Gets a video
{
	cv::VideoCapture cap(name);
	if (!cap.isOpened())
	{
		throw std::runtime_error("Cannot open video: " + name);
	}
	return cap;
}
void Library::saveImage(cv::Mat image, std::string fileName) //exports image
{
	cv::imwrite("../img/"+fileName, image);
}

void Library::saveVideo(std::string filename, std::vector<cv::Mat> frames, double fps) //exports video
{
    if (frames.empty())
    {
        throw std::runtime_error("No frames to write!");
    }

    cv::Size frameSize(frames[0].cols, frames[0].rows);

    cv::VideoWriter writer;
    writer.open(
        filename,
        cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), // default codec
        fps,
        frameSize,
        true // color video
    );

    if (!writer.isOpened())
    {
        throw std::runtime_error("Cannot open video file for writing: " + filename);
    }

    for (const auto& frame : frames)
    {
        if (frame.size() != frameSize)
        {
            throw std::runtime_error("Frame sizes are not consistent");
        }
        writer.write(frame);
    }
}
