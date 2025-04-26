#include "eval/SimpleAccuracyEvaluator.hpp"
float SimpleAccuracyEvaluator::evaluate(cv::ml::StatModel& model,
                                        const cv::Mat&     Xtest,
                                        const cv::Mat&     ytest)
{
    if (Xtest.empty()) return 0.f;
    cv::Mat ypred; model.predict(Xtest, ypred);
    return 100.f * (1.f - static_cast<float>(cv::countNonZero(ypred != ytest))/ytest.rows);
}
