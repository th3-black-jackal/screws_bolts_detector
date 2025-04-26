#pragma once
#include "interfaces/IModelEvaluator.hpp"
class SimpleAccuracyEvaluator final : public IModelEvaluator {
public:
    float evaluate(cv::ml::StatModel& model,
                   const cv::Mat&     Xtest,
                   const cv::Mat&     ytest) override;
};
