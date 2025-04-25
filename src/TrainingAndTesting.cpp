#include <TrainingAndTesting.hpp>



TrainingAndTesting::TrainingAndTesting(){
	this->miw = new MultipleImageWindow("MainWindow", 2, 2, cv::WINDOW_AUTOSIZE);
	this->features_extractor = new ImageExtractFeatures();
}

void plotTrainingData(cv::Mat trainingData, cv::Mat labels, float *error=NULL){
	cv::Scalar green(0, 255, 0), blue(255, 0, 0), red(0, 0, 255);
	float area_max, ar_max, area_min, ar_min;
	area_max = ar_max = 0;
	area_min = ar_min = 99999999;
	for(int i = 0;i < trainingData.rows;i++){
		float area = trainingData.at<float>(i, 0);
		float ar = trainingData.at<float>(i, 1);
		if(area > area_max)
			area_max = area;
		if(ar > ar_max)
			ar_max = ar;
		if(area < area_min)
			area_min = area;
		if(ar < ar_min)
			ar_min = ar;	
	}
	cv::Mat plot = cv::Mat::zeros(512, 512, CV_8UC3);
	for(int i = 0;i < trainingData.rows;i++){	
		float area = trainingData.at<float>(i, 0);
		float ar = trainingData.at<float>(i, 1);
		
		
		int x = (int)(512.0f*((area - area_min) / (area_max - area_min))); 
		int y = (int)(512.0f*((ar - ar_min) / (ar_max-ar_min)));
		int point_size = (int)512/100;
		int label = labels.at<int>(i);
		
		cv::Scalar color;
		if(label == 0){
			color = green;
			cv::circle(plot, cv::Point(x, y), 3, color, -1, 8);
		}
		else if (label == 1){
			color = blue;
			cv::ellipse(plot, cv::Point(x, y), cv::Size(point_size, point_size),0, 0, 360, color);
		}
		else if (label == 2){
			color = red;
			cv::rectangle(plot, cv::Point(x, y), cv::Point(x + point_size, y + point_size),  color);
		}
		
	}
	if (error != NULL){
		std::stringstream ss;
		ss <<"Error: "<< *error << "\%";
		cv::putText(plot, ss.str().c_str(), cv::Point(20, 512-40), cv::FONT_HERSHEY_SIMPLEX, 0.75, cv::Scalar(200, 200, 200), 1, cv::LINE_AA);
	}
	cv::imshow("Plot", plot);
	cv::waitKey(0);
}

/*
 *
 * Use two lists (`dataset_sources` which contain dirs to images, each dir contain images with only one class, and the other list `labels` which contains integers reprsent labels) 
 * please note that the order of labels MUST be the same as the order of images sources, until I finish the dictionary
 */

/*
 * TODO: Use dictionary with key is the label and the value is the dataset source.
 */
cv::Ptr<cv::ml::SVM> TrainingAndTesting::trainAndTest(std::vector<std::string> &dataset_sources, std::vector<int> &labels, std::string light_pattern_file){
	cv::Ptr<cv::ml::SVM> svm;
	std::vector<float> trainingData;
	std::vector<int> responsesData;
	std::vector<float> testData;
	std::vector<float> testResponsesData;
	int num_for_tests = 20;
	//ImageExtractFeatures *features_extractor = new ImageExtractFeatures();
	ImageExtractFeatures *features_extractor = this->features_extractor;    //To use custom extractors like the one in Fuzzing
	for(int i = 0;i < dataset_sources.size();i++) {
		features_extractor->readFolderAndExtractFeatures(dataset_sources[i], labels[i], 
				num_for_tests, trainingData, responsesData, testData, testResponsesData, light_pattern_file);
	}
	std::cout<<"Num of traingin examples: "<<responsesData.size()<<std::endl;
	std::cout<<"Num of testing examples: "<<testResponsesData.size()<<std::endl;
	cv::Mat trainingDataMat(trainingData.size()/2, 2, CV_32FC1, &trainingData[0]);
	cv::Mat responses(responsesData.size(), 1, CV_32SC1, &responsesData[0]);
	cv::Mat testDataMat(testData.size() / 2, 2, CV_32FC1, &testData[0]);
	cv::Mat testResponses(testResponsesData.size(), 1, CV_32FC1, &testResponsesData[0]);
	svm = cv::ml::SVM::create();
	svm->setType(cv::ml::SVM::C_SVC);
	svm->setKernel(cv::ml::SVM::CHI2);
	svm->setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER, 100, 1e-6));
	std::cout<<"Start traingin"<<std::endl;	
	svm->train(trainingDataMat, cv::ml::ROW_SAMPLE, responses);
	if(testResponsesData.size() > 0){
		std::cout<<"Evaluation"<<std::endl;
		std::cout<<"=========="<<std::endl;
		cv::Mat testPredict;
		svm->predict(testDataMat, testPredict);
		std::cout<<"Prediction done"<<std::endl;
		cv::Mat errorMat = testPredict != testResponses;
		float error = 100.0f * countNonZero(errorMat) / testResponsesData.size();
		std::cout<<"Error: "<<error<<"\%"<<std::endl;
		plotTrainingData(trainingDataMat, responses, &error);
	}
	else {
		plotTrainingData(trainingDataMat, responses);
	}
	return svm;
}

void TrainingAndTesting::predict(cv::Mat img, std::string light_pattern_file, cv::Ptr<cv::ml::SVM> svm){
	cv::Scalar green(0, 255, 0), blue(255, 0, 0), red(0, 0, 255);
	cv::Scalar color;
	ImageExtractFeatures *features_extractor = new ImageExtractFeatures();
	std::vector<int> pos_top, pos_left;
	std::vector<std::vector<float>> features = features_extractor->extractFeatures(img, light_pattern_file, &pos_left, &pos_top);
	for(int i = 0;i < features.size();i++){
		cv::Mat trainginDataMat(1, 2, CV_32FC1, &features[i][0]);
		float result = svm->predict(trainginDataMat);
		std::stringstream ss;
		if(result == 0){
			color = green;
			ss << "NUT";
		}
		else if(result == 1){
			color = blue;
			ss << "RING";
		}
		else if(result == 2){
			color = red;
			ss<< "SCREW";
		}
		cv::putText(img, ss.str(), cv::Point2d(pos_left[i], pos_top[i]), cv::FONT_HERSHEY_SIMPLEX, 0.4, color);
	}
	cv::imshow("Detected items", img);
	cv::waitKey(0);
}



void TrainingAndTesting::setFeatureExtractor(ImageExtractFeatures *extractor){
	this->features_extractor = extractor;
}