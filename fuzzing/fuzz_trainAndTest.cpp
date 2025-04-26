#include "TrainingAndTesting.hpp"
#include <fuzzer/FuzzedDataProvider.h>
#include <string>
#include <vector>


class MockImageExtractorFeatures : public ImageExtractFeatures{
    public:
        MockImageExtractorFeatures() = default;
        bool readFolderAndExtractFeatures(
            std::string folder, int label, int num_for_tests,
            std::vector<float> &trainingData, std::vector<int> &responsesData,
            std::vector<float> &testingData, std::vector<float> &testingResponsesData, 
            std::string light_pattern_file) override {
                scratch_float_.reserve(22);   
                scratch_int_.reserve(12);

                for (int i = 0; i < 10; ++i) {
                    float area         = static_cast<float>(i + 1);
                    float aspect_ratio = 1.0f + 0.1f * i;
                    trainingData.push_back(area);
                    trainingData.push_back(aspect_ratio);

                    scratch_float_.push_back(area);
                    scratch_float_.push_back(aspect_ratio);
                    responsesData.push_back(label);
                    scratch_int_.push_back(label);
                }

                for (int i = 0; i < num_for_tests; ++i) {
                    testingData.push_back(2.0f);
                    testingData.push_back(1.5f);
                    testingResponsesData.push_back(label);

                    scratch_float_.push_back(2.0f);
                    scratch_float_.push_back(1.5f);
                    scratch_int_.push_back(label);
                }
            }
        void clearState() noexcept {
                std::vector<float>().swap(scratch_float_);
                std::vector<int>().swap(scratch_int_);
                }
    private:
            std::vector<float> scratch_float_;
            std::vector<int>   scratch_int_;
};

static TrainingAndTesting gTnT;
static MockImageExtractorFeatures gMock;

void handlException(const std::string &context){
    try{
        throw;
    } catch(const cv::Exception &e) {
         std::cerr << "[OpenCV Exception - " << context << "]\n"
                  << "Message: " << e.what() << "\n"
                  << "Code: " << e.code << "\n"
                  << "Function: " << e.func << "\n"
                  << "File: " << e.file << "\n"
                  << "Line: " << e.line << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[Standard Exception - " << context << "] " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "[Unknown Exception - " << context << "] Unhandled error during fuzzing." << std::endl;
    }
}

void fuzzLabelMismatch(TrainingAndTesting &tnt, FuzzedDataProvider &fdp){
    int num_dirs = fdp.ConsumeIntegralInRange<int>(1, 5);
    std::vector<std::string> dataset_sources;
    std::vector<int> labels;
    for(int i=0;i < num_dirs; ++i){
        dataset_sources.push_back(fdp.ConsumeRandomLengthString(20));
    } 
    int label_count = fdp.ConsumeIntegralInRange<int>(1, num_dirs + 2);
    for(int i=0; i < label_count;++i){
        labels.push_back(fdp.ConsumeIntegral<int>());
    }
    std::string light_pattern_file = fdp.ConsumeRandomLengthString(64);
    try{
        gTnT.trainAndTest(dataset_sources, labels, light_pattern_file);
    }  catch (...) {
        handlException("fuzzLabMismatch");
    }
}

void fuzzExtremeLabels(TrainingAndTesting &tnt, FuzzedDataProvider &fdp){
    int num_dirs = fdp.ConsumeIntegralInRange<int>(1, 5);
    std::vector<std::string> dataset_sources;
    std::vector<int> labels;
      for (int i = 0; i < num_dirs; ++i) {
        dataset_sources.push_back(fdp.ConsumeRandomLengthString(20));
        labels.push_back(fdp.ConsumeIntegralInRange<int>(-10000, 10000));
    }
    std::string light_pattern_file = fdp.ConsumeRandomLengthString(64);
    try {
        gTnT.trainAndTest(dataset_sources, labels, light_pattern_file);
    }  catch (...) {
        handlException("fuzzExtremeLabels");
    }
}

void fuzzEmptyInputs(TrainingAndTesting &tnt){
    std::vector<std::string> dataset_sources;
    std::vector<int> labels;
    std::string light_pattern_file;
     try {
        gTnT.trainAndTest(dataset_sources, labels, light_pattern_file);
    } catch (const cv::Exception& e) {
        handlException("fuzzEmptyInputs");
    }
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size){
    if(size > 4096) return 0; //to mitigate fuzzer out memory 
    gMock.clearState();
    FuzzedDataProvider fdp(data, size);
    int num_dirs = fdp.ConsumeIntegralInRange<int>(1, 5);
    std::string path = fdp.ConsumeBytesAsString(100);
    std::vector<std::string> dataset_sources;
    std::vector<int> labels;
    for(int i = 0;i < num_dirs;++i){
        std::string fake_path = fdp.ConsumeRandomLengthString(20);
        int label = fdp.ConsumeIntegralInRange<int>(0, 2);
        dataset_sources.push_back(fake_path);
        labels.push_back(label);
    }
    std::string light_pattern = fdp.ConsumeRemainingBytesAsString();
    gTnT.setFeatureExtractor(&gMock);
   try {
    gTnT.trainAndTest(dataset_sources, labels, light_pattern);
    } catch (const cv::Exception& e) {
        std::cerr << "[OpenCV Exception]\n"
        << "Message: " << e.what() << "\n"
        << "Code: " << e.code << "\n"
        << "Function: " << e.func << "\n"
        << "File: " << e.file << "\n"
        << "Line: " << e.line << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "[Standard Exception] " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "[Unknown Exception] An unknown error occurred during fuzzing." << std::endl;
        }

    return 0;
}