#ifndef IMAGEEXTRACTFEATURES
#define IMAGEEXTRACTFEATURES

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <MultipleImageWindow.h>
#include <ImagePreprocessing.hpp>


class ImageExtractFeatures{
	public:
		ImageExtractFeatures();
		//The light pattern file can be empty, in that case we will create it from image, but the code is not ready yet
		std::vector<std::vector<float>> extractFeatures(cv::Mat img, std::string light_pattern_file="", std::vector<int> *left=NULL, std::vector<int> *top=NULL);
		virtual bool readFolderAndExtractFeatures(std::string folder, int label, int num_for_tests, std::vector<float> &trainingData, 
				std::vector<int> &responsesData, std::vector<float> &testingData, std::vector<float> &testingResponsesData, std::string light_pattern_file);

	private:
		MultipleImageWindow *miw;
};

#endif
