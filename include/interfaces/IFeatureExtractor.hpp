#pragma once
#include <string>
#include <vector>
class IFeatureExtractor {
public:
    virtual ~IFeatureExtractor() = default;
    virtual void extract(const std::string& datasetDir,int  label,
                         int    numTest,    std::vector<float>& trainX,
                         std::vector<int>&   trainY,    std::vector<float>& testX,
                         std::vector<float>&   testY,   const std::string&  lightPattern) = 0;
};
