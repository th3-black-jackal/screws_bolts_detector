#include <ImageExtractFeatures.hpp>


ImageExtractFeatures::ImageExtractFeatures(){
	this->miw = new MultipleImageWindow("MainWindow", 2, 2, cv::WINDOW_AUTOSIZE);
}

std::vector<std::vector<float>> ImageExtractFeatures::extractFeatures(cv::Mat img, std::string light_pattern_file, std::vector<int> *left, std::vector<int> *top){	
	//It should now take a normal image and make the preprocessing step here
	ImagePreprocessing *image_preprocessor = new ImagePreprocessing(light_pattern_file, img);
	cv::Mat pre_processed_image = image_preprocessor->preprocessImage();
	std::vector<std::vector<float>> output;
	std::vector<cv::Vec4i> hierarchy;
	std::vector<std::vector<cv::Point>> contours;
	cv::Mat input = pre_processed_image.clone();
	cv::findContours(input, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE);
	if(contours.size() == 0){
		return output;
	}
	cv::RNG rng(0xFFFFFFFF);
	for(int i = 0;i < contours.size(); i++){
		cv::Mat mask = cv::Mat::zeros(img.rows, img.cols, CV_8UC1);
		cv::drawContours(mask, contours, i, cv::Scalar(1), cv::FILLED, cv::LINE_8, hierarchy, 1);
		cv::Scalar area_s = cv::sum(mask);
		float area  = area_s[0];	
		if(area>500) {
			cv::RotatedRect r = cv::minAreaRect(contours[i]);
			float width = r.size.width;
			float height = r.size.height;
			float ar = (width<height) ? height / width:width / height;
			std::vector<float> row;
			row.push_back(area);
			row.push_back(ar);
			output.push_back(row);
			if(left != NULL){
				left->push_back((int) r.center.x);
			}
			if(top != NULL){
				top->push_back((int) r.center.y);
			}
			
			this->miw->addImage("Extract features", mask*255);
			this->miw->render();
			cv::waitKey(10);
			
		}
	}
	return output;
}

bool ImageExtractFeatures::readFolderAndExtractFeatures(std::string folder, int label, int num_for_tests, 
		std::vector<float> &trainingData, 
		std::vector<int> &responsesData, std::vector<float> &testingData, 
		std::vector<float> &testingResponsesData, std::string light_pattern_file){
	cv::VideoCapture images;
	std::cout<<"Start reading "<<folder<<std::endl;
	std::cout<<"Num for tests: "<<num_for_tests<<std::endl;
	if(images.open(folder) == false){
		std::cout<<"Can not open the folder images"<<std::endl;
		return false;
	}
		
	cv::Mat frame;
	int img_index = 0;
	while(images.read(frame)){
		cv::imshow("Original image", frame);
		cv::waitKey(10);	
		std::vector<std::vector<float>> features = this->extractFeatures(frame, light_pattern_file);
		for(int i = 0;i < features.size(); i++){
			std::cout<<"Image index: "<<img_index<<std::endl;
			if(img_index >= num_for_tests){
				trainingData.push_back(features[i][0]);
				trainingData.push_back(features[i][1]);
				responsesData.push_back(label);
			}
			else {
				testingData.push_back(features[i][0]);
				testingData.push_back(features[i][1]);
				testingResponsesData.push_back((float) label);
			}
		}
		img_index++;

	}
	return true;
}


