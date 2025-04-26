#pragma once
#include "interfaces/IFeatureExtractor.hpp"
#include "ImageExtractFeatures.hpp"

class OpenCVFeatureExtractor final : public IFeatureExtractor {
    public:
        explicit OpenCVFeatureExtractor() = default;
        void extract(const std::string &datasetDir, int label,
        int numTest,    std::vector<float> &trainX,
        std::vector<int> &trainY,   std::vector<float> &testX,
        std::vector<float> &testY,  const std::string &lightPattern
        ) override {
            impl_.readFolderAndExtractFeatures(datasetDir, label, numTest,
                                                trainX, trainY, testX, testY,
                                                lightPattern);
        }
    private:
        ImageExtractFeatures impl_;
};