#pragma once
#include <opencv2/opencv.hpp>
class IModelEvaluator {
public:
    virtual ~IModelEvaluator() = default;
    virtual float evaluate(cv::ml::StatModel& model,
                           const cv::Mat&     Xtest,
                           const cv::Mat&     ytest) = 0;
};
