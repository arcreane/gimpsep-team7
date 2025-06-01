#include "neural_mosaic.h"
#include <opencv2/photo.hpp>
#include <numeric>
#include <algorithm>
#include <cstdlib>
#include <ctime>

// Optional wrapper
cv::Mat applyNeuralMosaic(const cv::Mat& input, int rows, int cols) {
    NeuralMosaicEffect mosaic(rows, cols);
    return mosaic.apply(input);
}


NeuralMosaicEffect::NeuralMosaicEffect(int rows, int cols)
    : rows_(rows), cols_(cols) {
    srand(static_cast<unsigned>(time(NULL)));
}

cv::Mat NeuralMosaicEffect::apply(const cv::Mat& input) {
    cv::Mat output = input.clone();

    std::vector<cv::Point2f> points;
    for (int i = 0; i < rows_ * cols_; ++i)
        points.emplace_back(rand() % input.cols, rand() % input.rows);

    cv::Subdiv2D subdiv(cv::Rect(0, 0, input.cols, input.rows));
    for (const auto& p : points) subdiv.insert(p);

    std::vector<std::vector<cv::Point2f>> facets;
    std::vector<cv::Point2f> centers;
    subdiv.getVoronoiFacetList({}, facets, centers);

    for (const auto& facet : facets) {
        std::vector<cv::Point> polygon;
        for (const auto& pt : facet) polygon.emplace_back(pt);

        cv::Mat mask = cv::Mat::zeros(input.size(), CV_8UC1);
        cv::fillConvexPoly(mask, polygon, 255);

        cv::Rect bounds = cv::boundingRect(polygon) & cv::Rect(0, 0, input.cols, input.rows);
        if (bounds.width <= 1 || bounds.height <= 1) continue;

        cv::Mat tile;
        input(bounds).copyTo(tile, mask(bounds));

        int effectID = rand() % 22;
        cv::Mat filtered = applyEffect(tile, effectID);

        filtered.copyTo(output(bounds), mask(bounds));

    }

    return output;
}

cv::Mat NeuralMosaicEffect::applyEffect(const cv::Mat& tile, int effect) {
    cv::Mat result;
    switch (effect) {
        case 0: cv::stylization(tile, result); break;
        case 1: cv::detailEnhance(tile, result, 10, 0.15f); break;
        case 2: cv::edgePreservingFilter(tile, result, cv::RECURS_FILTER, 60, 0.4f); break;
        case 3: {
            cv::Mat safe;
            tile.convertTo(safe, CV_8UC3);
            cv::bilateralFilter(safe, result, 9, 150, 150);
            break;
        }
        case 4:
        case 10:
        case 12: {
            cv::Mat temp1, temp2;
            tile.convertTo(temp1, CV_8UC3);
            for (int i = 0; i < 3; ++i) {
                cv::bilateralFilter(temp1, temp2, 9, 75, 75);
                temp1 = temp2.clone();
            }
            result = temp1;
            break;
        }
        case 5: {
            cv::Mat graySketch, colorSketch;
            cv::pencilSketch(tile, graySketch, colorSketch, 60, 0.07f, 0.1f);
            result = colorSketch;
            break;
        }
        case 6: cv::medianBlur(tile, result, 7); break;
        case 7: {
            cv::Mat gray, color;
            cv::pencilSketch(tile, gray, color, 60, 0.1f, 0.03f);
            result = color;
            break;
        }
        case 8: {
            cv::Mat hsv;
            tile.convertTo(result, CV_8UC3);
            cv::cvtColor(result, hsv, cv::COLOR_BGR2HSV);
            for (int i = 0; i < hsv.rows; ++i)
                for (int j = 0; j < hsv.cols; ++j)
                    hsv.at<cv::Vec3b>(i, j)[1] *= 0.5;
            cv::cvtColor(hsv, result, cv::COLOR_HSV2BGR);
            break;
        }
        case 9: cv::boxFilter(tile, result, -1, cv::Size(7, 7)); break;
        case 11: {
            cv::Mat gray;
            cv::cvtColor(tile, gray, cv::COLOR_BGR2GRAY);
            cv::cvtColor(gray, result, cv::COLOR_GRAY2BGR);
            cv::edgePreservingFilter(result, result, cv::RECURS_FILTER, 50, 0.2f);
            break;
        }
        case 13: {
            result = tile.clone();
            for (int i = 0; i < result.rows; ++i)
                for (int j = 0; j < result.cols; ++j)
                    for (int c = 0; c < 3; ++c)
                        result.at<cv::Vec3b>(i, j)[c] &= 0xE0;
            break;
        }
        case 14: {
            cv::Mat gray, highContrast;
            cv::cvtColor(tile, gray, cv::COLOR_BGR2GRAY);
            cv::threshold(gray, highContrast, 128, 255, cv::THRESH_BINARY);
            cv::cvtColor(highContrast, result, cv::COLOR_GRAY2BGR);
            break;
        }
        case 15: {
            cv::Mat reshaped = tile.reshape(1, tile.rows * tile.cols);
            reshaped.convertTo(reshaped, CV_32F);
            cv::Mat labels, centers;
            cv::kmeans(reshaped, 8, labels,
                       cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 10, 1.0),
                       3, cv::KMEANS_PP_CENTERS, centers);
            centers.convertTo(centers, CV_8U);
            cv::Mat quantized(tile.size(), tile.type());
            for (int i = 0; i < tile.rows * tile.cols; ++i)
                quantized.at<cv::Vec3b>(i / tile.cols, i % tile.cols) =
                    centers.row(labels.at<int>(i));
            result = quantized;
            break;
        }
        case 16: {
            cv::Mat lab;
            cv::cvtColor(tile, lab, cv::COLOR_BGR2Lab);
            std::vector<cv::Mat> ch;
            cv::split(lab, ch);
            ch[1] += 20; ch[2] += 20;
            cv::merge(ch, lab);
            cv::cvtColor(lab, result, cv::COLOR_Lab2BGR);
            cv::stylization(result, result);
            break;
        }
        case 17: {
            cv::Mat hsv;
            cv::cvtColor(tile, hsv, cv::COLOR_BGR2HSV);
            for (int i = 0; i < hsv.rows; ++i)
                for (int j = 0; j < hsv.cols; ++j)
                    hsv.at<cv::Vec3b>(i, j)[0] = (hsv.at<cv::Vec3b>(i, j)[0] + 60) % 180;
            cv::cvtColor(hsv, result, cv::COLOR_HSV2BGR);
            break;
        }
        case 18: {
            cv::Mat hsv;
            cv::cvtColor(tile, hsv, cv::COLOR_BGR2HSV);
            for (int i = 0; i < hsv.rows; ++i)
                for (int j = 0; j < hsv.cols; ++j) {
                    hsv.at<cv::Vec3b>(i, j)[0] = (hsv.at<cv::Vec3b>(i, j)[0] / 30) * 30;
                    hsv.at<cv::Vec3b>(i, j)[1] = std::min<int>(hsv.at<cv::Vec3b>(i, j)[1] * 1.3, 255);
                }
            cv::cvtColor(hsv, result, cv::COLOR_HSV2BGR);
            break;
        }
        case 19: {
            cv::Mat overlay(tile.size(), tile.type(), cv::Scalar(20, 60, 180));
            cv::addWeighted(tile, 0.6, overlay, 0.4, 0.0, result);
            break;
        }
        case 20: {
            cv::Mat inverted;
            cv::bitwise_not(tile, inverted);
            cv::addWeighted(tile, 0.5, inverted, 0.7, 10, result);
            break;
        }
        case 21: {
            cv::Mat hsv, edges;
            cv::cvtColor(tile, hsv, cv::COLOR_BGR2HSV);
            for (int i = 0; i < hsv.rows; ++i)
                for (int j = 0; j < hsv.cols; ++j)
                    hsv.at<cv::Vec3b>(i, j)[1] = 255;
            cv::cvtColor(hsv, result, cv::COLOR_HSV2BGR);
            cv::Mat gray;
            cv::cvtColor(result, gray, cv::COLOR_BGR2GRAY);
            cv::Canny(gray, edges, 100, 200);
            cv::cvtColor(edges, edges, cv::COLOR_GRAY2BGR);
            cv::addWeighted(result, 0.8, edges, 0.5, 0, result);
            break;
        }
        default:
            result = tile.clone();
            break;
    }
    return result;
}
