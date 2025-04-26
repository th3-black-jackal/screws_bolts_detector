#ifndef TRAININGANDTESTING
#define TRAININGANDTESTING


#include <iostream>
#include <string>
#include <sstream>
#include <string>
#include <vector>
#include <MultipleImageWindow.h>
#include <ImageExtractFeatures.hpp>
#include <ImagePreprocessing.hpp>


class TrainingAndTesting{
	public:
		TrainingAndTesting();
		cv::Ptr<cv::ml::SVM> trainAndTest(std::vector<std::string> &dataset_sources, std::vector<int> &labels, std::string light_pattern_file);
		void predict(cv::Mat img, std::string light_pattern_file, cv::Ptr<cv::ml::SVM> svm);
		void setFeatureExtractor(ImageExtractFeatures *extractor);
	private:
		MultipleImageWindow *miw;
		ImageExtractFeatures *features_extractor;
};
#endif
