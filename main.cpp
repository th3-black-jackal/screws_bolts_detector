#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <TrainingAndTesting.hpp>
#include <AListADT.hpp>
#include <boost/lambda/lambda.hpp>

/*
 * Please Allah, 
 * forigve me for my all sins, 
 * I'm such a weak person who can not determine his destination, 
 * I enjoy the journey but it's too hard, I'm only seeking forgiveness from you
 */

const char* keys = {
	"{help h usage ? | | print this message}"
		"{@image || image to process}"
		"{@lightPattern || Image light pattern to apply to image input}"
		"{lightMethod | 1 | Method to remove background light, 0 difference, 1 div}"
		"{segMethod | 1 | Method to segment: 1 connected Components, 2 connected components with stats, 3 find Contours}"
		"{algo | MOG2 | Background substraction method (KNN, MOG2) }"
};

std::string light_pattern_file;

int main(int argc, const char **argv){
	cv::CommandLineParser parser(argc, argv, keys);
	parser.about("Nasser implementation for object classification");
	if(parser.has("help")){
		parser.printMessage();
		return 0;
	}
	
	cv::String img_file  = parser.get<cv::String>(0);
	light_pattern_file = parser.get<cv::String>(1);
	int method_light = parser.get<int>("lightMethod");
	int method_seg = parser.get<int>("segMethod");
	cv::String background_sub_algo = parser.get<cv::String>("algo");
	
	if(!parser.check()){
		parser.printErrors();
		return 0;
	}
	cv::Mat img = cv::imread(img_file, 0);
	
	
	if(img.data == NULL){
		std::cout<<"Error loading image "<<img_file<<std::endl;
		return -1;
	}
	std::vector<std::string> dataset_sources;
	std::vector<int> labels;
	ImageExtractFeatures *features_extractor = new ImageExtractFeatures();
	features_extractor->extractFeatures(img);
	labels.push_back(0);
	labels.push_back(1);
	labels.push_back(2);

	std::string DATASET_ROOT_DIR = "../Dataset/dataset_white_background/data/";
	dataset_sources.push_back(DATASET_ROOT_DIR +  "nut/tuerca_%04d.pgm");
	dataset_sources.push_back(DATASET_ROOT_DIR + "ring/arandela_%04d.pgm");
	dataset_sources.push_back(DATASET_ROOT_DIR + "screw/tornillo_%04d.pgm");
	dataset_sources.push_back("../Dataset/MvTecDataset/screw/train/good/");
	TrainingAndTesting *trainer = new TrainingAndTesting();
	cv::Ptr<cv::ml::SVM> svm_model = trainer->trainAndTest(dataset_sources, labels, light_pattern_file);
	trainer->predict(img, light_pattern_file, svm_model);
	return 0;
}




