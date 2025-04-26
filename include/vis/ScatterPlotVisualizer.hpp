#pragma once
#include "interfaces/IVisualizer.hpp"
class ScatterPlotVisualizer final : public IVisualizer {
public:
    void plot(const cv::Mat& X, const cv::Mat& y,
              std::optional<float> error = std::nullopt) override;
};