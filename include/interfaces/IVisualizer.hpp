#pragma once
#include <opencv2/opencv.hpp>
#include <optional>
class IVisualizer {
public:
    virtual ~IVisualizer() = default;
    virtual void plot(const cv::Mat& X,
                      const cv::Mat& y,
                      std::optional<float> error = std::nullopt) = 0;
};
