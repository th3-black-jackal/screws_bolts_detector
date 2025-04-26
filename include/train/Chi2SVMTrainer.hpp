#pragma once
#include "interfaces/IModelTrainer.hpp"
class Chi2SVMTrainer final : public IModelTrainer {
public:
    cv::Ptr<cv::ml::StatModel> train(const cv::Mat& X, const cv::Mat& y) override;
};
