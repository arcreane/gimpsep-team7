#pragma once
#include <opencv2/opencv.hpp>

class NeuralMosaicEffect {
public:
    NeuralMosaicEffect(int rows = 10, int cols = 10);
    cv::Mat apply(const cv::Mat& input);

private:
    int rows_, cols_;
    cv::Mat applyEffect(const cv::Mat& tile, int effect);

};

// Optional backward-compatible wrapper
cv::Mat applyNeuralMosaic(const cv::Mat& input, int rows = 10, int cols = 10);
