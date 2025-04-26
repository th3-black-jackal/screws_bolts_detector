#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <TrainingAndTesting.hpp>
#include <AListADT.hpp>
#include <run_tests.cpp>
#include <boost/lambda/lambda.hpp>
#include <boost/json.hpp>
#include <iomanip>


/*
 * Please Allah, 
 * forigve me for my all sins, 
 * I'm such a weak person who can not determine his destination, 
 * I enjoy the journey but it's too hard, I'm only seeking forgiveness from you
 */

namespace json = boost::json;
/*
json::value parse_file(char const *filename){
	file f(filename, "r");
	json::stream_parser p;
	json::error_code ec;
	do
	{
		char buf[4096];
		auto const nread = f.read(buf, sizeof(buf));
		p.write(buf, nread, ec);
	}
	while( !f.eof() );
	if(ec)
		return nullptr;
	p.finish( ec );
		return nullptr;
	if(ec)
		return nullptr;
	return p.release();
}
*/

json::value readJSONFile(std::string file_name){
	std::ifstream inFile {file_name};
	if(!inFile){
		std::cout <<"Faield to open file"<< file_name <<std::endl;
		
	}
	json::stream_parser p;
	json::error_code ec;
	std::string line {};
	std::string file_body {};
	size_t count {};
	size_t perline {6};
	char c;
	while(getline(inFile, line)){
		if(line.empty())
			continue;
		std::istringstream iss(line);
		file_body += line;
	}
	p.write(file_body.data(), file_body.length(), ec);
	inFile.close();
	return p.release();
}


void pretty_print(std::ostream& os, json::value const& jv, std::string *indent = nullptr){
	std::string indent_;
	if(!indent)
		indent = &indent_;
	switch(jv.kind()){
		case json::kind::object: 
			{
				os << "{\n";
				indent->append(4, ' ');
				auto const& obj = jv.get_object();
				if(!obj.empty()){
					auto it = obj.begin();
					for(;;){
						os << *indent << json::serialize(it->key())<<" : ";
						pretty_print(os, it->value(), indent);
						if(++it == obj.end())
							break;
						os << ",\n";
					}
				}
				os << "\n";
				indent->resize(indent->size() - 4);
				os << *indent << "}";
				break;
			}
		case json::kind::array:
			{
				os << "[\n";
				indent->append(4, ' ');
				auto const &arr = jv.get_array();
				if(!arr.empty()){
					auto it = arr.begin();
					for(;;)
					{
						os << *indent;
						pretty_print(os, *it, indent);
						if(++it == arr.end())
							break;
						os << ",\n";
					}
				}
				os << "\n";
				indent->resize(indent->size() - 4);
				os << *indent << "]";
				break;
			}
		case json::kind::string:
			{
				os << json::serialize(jv.get_string());
				break;
			}
		case json::kind::uint64:
			{
				os << jv.get_uint64();
				break;
			}
		case json::kind::int64:
			{
				os << jv.get_int64();
				break;
			}
		case json::kind::double_:
			{
				os << jv.get_double();
				break;
			}
		case json::kind::bool_:
			{
				if(jv.get_bool())
					os << "true";
				else
					os << "false";
				break;
			}
		case json::kind::null:
			os << "null";
			break;
	}
	if(indent->empty())
		os << "\n";

}

void add_config_to_cache(std::vector<std::string> &cache_keys, std::vector<std::string> &cache_values, json::value const& jv){
	switch(jv.kind()){
		case json::kind::object: 
			{
				auto const& obj = jv.get_object();
				if(!obj.empty()){
					auto it = obj.begin();
					for(;;){
						cache_keys.push_back(json::serialize(it->key()));
						add_config_to_cache(cache_keys, cache_values, it->value());
						if(++it == obj.end())
							break;
					}
				}
				break;
			}
		case json::kind::string:
			{
				cache_values.push_back(json::serialize(jv.get_string()));
				break;
			}
		case json::kind::bool_:
			{
				if(jv.get_bool())
					cache_values.push_back("true");
				else
					cache_values.push_back("false");
				break;
			}
		case json::kind::null:
			cache_values.push_back("null");
			break;
	}
}

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
	runTests();
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
	labels.push_back(0);
	labels.push_back(1);
	labels.push_back(2);
	auto const jv = readJSONFile("test_json.json");
	auto const obj = jv.get_object();

	pretty_print(std::cout, jv); 
	std::vector<std::string> cache_keys;
	std::vector<std::string> cache_values;
	add_config_to_cache(cache_keys, cache_values,jv);
	for(auto key: cache_keys){
		std::cout<<key<<" ";
	}
	std::cout<<std::endl;
	for(auto value: cache_values){
		std::cout<<value<<" ";
	}
	std::cout<<std::endl;

	std::string DATASET_ROOT_DIR = "../Dataset/dataset_white_background/data/";
	dataset_sources.push_back(DATASET_ROOT_DIR +  "nut/tuerca_%04d.pgm");
	dataset_sources.push_back(DATASET_ROOT_DIR + "ring/arandela_%04d.pgm");
	dataset_sources.push_back(DATASET_ROOT_DIR + "screw/tornillo_%04d.pgm");
	
	TrainingAndTesting *trainer = new TrainingAndTesting();
	cv::Ptr<cv::ml::SVM> svm_model = trainer->trainAndTest(dataset_sources, labels, light_pattern_file);
	trainer->predict(img, light_pattern_file, svm_model);
	return 0;
}




