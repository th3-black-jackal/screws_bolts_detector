#pragma once
#include <memory>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include "interfaces/IFeatureExtractor.hpp"
#include "interfaces/IModelTrainer.hpp"
#include "interfaces/IModelEvaluator.hpp"
#include "interfaces/IVisualizer.hpp"
class TrainingPipeline {
public:
    TrainingPipeline(std::shared_ptr<IFeatureExtractor> extractor,
                     std::shared_ptr<IModelTrainer>     trainer,
                     std::shared_ptr<IModelEvaluator>   evaluator,
                     std::shared_ptr<IVisualizer>       visualizer);
    cv::Ptr<cv::ml::StatModel> run(const std::vector<std::string>& datasetDirs,
                                   const std::vector<int>&         labels,
                                   const std::string&              lightPattern);
private:
    std::shared_ptr<IFeatureExtractor> extractor_;
    std::shared_ptr<IModelTrainer>     trainer_;
    std::shared_ptr<IModelEvaluator>   evaluator_;
    std::shared_ptr<IVisualizer>       visualizer_;
};
