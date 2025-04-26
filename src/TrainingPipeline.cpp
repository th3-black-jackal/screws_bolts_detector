#include "pipeline/TrainingPipeline.hpp"
#include <cassert>
#include <stdexcept>
TrainingPipeline::TrainingPipeline(std::shared_ptr<IFeatureExtractor> extractor,
                                   std::shared_ptr<IModelTrainer>     trainer,
                                   std::shared_ptr<IModelEvaluator>   evaluator,
                                   std::shared_ptr<IVisualizer>       visualizer)
    : extractor_(std::move(extractor)),trainer_(std::move(trainer)),
      evaluator_(std::move(evaluator)),visualizer_(std::move(visualizer)){}

cv::Ptr<cv::ml::StatModel>
TrainingPipeline::run(const std::vector<std::string>& datasetDirs,
                      const std::vector<int>& labels,
                      const std::string&      lightPattern)
{
    if(datasetDirs.size()!=labels.size())
        throw std::invalid_argument("datasetDirs and labels size mismatch");

    std::vector<float> XtrainVals,XtestVals; std::vector<int> yTrainVals,yTestVals;
    constexpr int kTestPerDir=20;
    for(std::size_t i=0;i<datasetDirs.size();++i)
        extractor_->extract(datasetDirs[i],labels[i],kTestPerDir,
                            XtrainVals,yTrainVals,XtestVals,yTestVals,lightPattern);
    if(XtrainVals.empty()) throw std::runtime_error("No training samples");
    int featDim=2; assert(XtrainVals.size()%featDim==0);
    cv::Mat Xtrain((int)XtrainVals.size()/featDim,featDim,CV_32FC1,XtrainVals.data());
    cv::Mat ytrain((int)yTrainVals.size(),1,CV_32SC1,yTrainVals.data());
    auto model=trainer_->train(Xtrain,ytrain);

    float acc=0.f;
    if(!XtestVals.empty()){
        cv::Mat Xtest((int)XtestVals.size()/featDim,featDim,CV_32FC1,XtestVals.data());
        cv::Mat ytest((int)yTestVals.size(),1,CV_32SC1,yTestVals.data());
        acc=evaluator_->evaluate(*model,Xtest,ytest);
    }
    visualizer_->plot(Xtrain,ytrain,acc>0?std::optional<float>{acc}:std::nullopt);

    std::vector<float>().swap(XtrainVals);std::vector<int>().swap(yTrainVals);
    std::vector<float>().swap(XtestVals); std::vector<int>().swap(yTestVals);
    return model;
}
