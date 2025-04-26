#pragma once
#include <opencv2/opencv.hpp>
class IModelTrainer {
public:
    virtual ~IModelTrainer() = default;
    virtual cv::Ptr<cv::ml::StatModel> train(const cv::Mat& X, const cv::Mat& y) = 0;
};