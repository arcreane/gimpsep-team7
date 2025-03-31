#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

int main()
{
	cv::Mat image = cv::imread("../img/lena.jpg");
	if (image.empty())
	{
		std::cout << "Could not read the image: " << std::endl;
		return 1;
	}
	cv::namedWindow("Display window");
	cv::imshow("Display window", image);
	cv::waitKey(0);
	return 0;
}