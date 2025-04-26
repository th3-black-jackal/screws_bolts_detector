#include "train/Chi2SVMTrainer.hpp"
cv::Ptr<cv::ml::StatModel> Chi2SVMTrainer::train(const cv::Mat& X, const cv::Mat& y)
{
    auto svm = cv::ml::SVM::create();
    svm->setType(cv::ml::SVM::C_SVC);
    svm->setKernel(cv::ml::SVM::CHI2);
    svm->setTermCriteria({cv::TermCriteria::MAX_ITER, 100, 1e-6});
    svm->train(X, cv::ml::ROW_SAMPLE, y);
    return svm;
}