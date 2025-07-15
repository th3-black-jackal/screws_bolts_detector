#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <TrainingAndTesting.hpp>
#include <AListADT.hpp>
#include "run_tests.cpp"
#include <boost/lambda/lambda.hpp>
#include <boost/json.hpp>
#include <iomanip>
#include "pipeline/TrainingPipeline.hpp"
#include "train/Chi2SVMTrainer.hpp"
#include "vis/ScatterPlotVisualizer.hpp"
#include "eval/SimpleAccuracyEvaluator.hpp"
#include "extract/OpenCVFeatureExtractor.hpp"
#include "ImageExtractFeatures.hpp"

#include <open62541/plugin/log_stdout.h>
#include <open62541/server.h>
#include <open62541/server_config_default.h>
#include <csignal>
#include <filesystem>
#include "OpcUaServer.hpp"

/*
 * Please Allah, 
 * forigve me for my all sins, 
 * I'm such a weak person who can not determine his destination, 
 * I enjoy the journey but it's too hard, I'm only seeking forgiveness from you
 */


static OpcUaServer *g_server = nullptr;

static void stopHandler(int)
{
    if (g_server)
        g_server->stop();
}


std::string light_pattern_file;

namespace fs = std::filesystem;

/* Utility – throws instead of std::exit() */
static cv::Mat loadGray(const fs::path &file)
{
    const cv::Mat img = cv::imread(file.string(), cv::IMREAD_GRAYSCALE);
    if (img.empty())
        throw std::runtime_error("Cannot load image: " + file.string());
    return img;
}
void startMachine()
{
    try {
        /* 1. Configuration ------------------------------------------------ */
        constexpr auto kImgFile        = "./000.png";
        constexpr auto kDatasetRoot    = "../Dataset/dataset_white_background/data";
        const std::array<std::string,3> kClasses = { "nut/tuerca_%04d.pgm",
                                                     "ring/arandela_%04d.pgm",
                                                     "screw/tornillo_%04d.pgm" };
        const std::array<int,3>        kLabels  = { 0, 1, 2 };

        /* 2. Sanity check reference image --------------------------------- */
        const cv::Mat img = loadGray(kImgFile);        // throws if missing
        std::cout << "[INFO] Loaded " << kImgFile
                  << "  ( " << img.cols << "×" << img.rows << " )\n";

        /* 3. Build the list of dataset templates -------------------------- */
        std::vector<std::string> sources;
        sources.reserve(kClasses.size());
        for (const auto &clsPattern : kClasses)
            sources.emplace_back(
                (fs::path(kDatasetRoot) / clsPattern).string());

        /* 4. Assemble pipeline components --------------------------------- */
        auto extractor   = std::make_shared<OpenCVFeatureExtractor>();
        auto trainer     = std::make_shared<Chi2SVMTrainer>();
        auto evaluator   = std::make_shared<SimpleAccuracyEvaluator>();
        auto visualizer  = std::make_shared<ScatterPlotVisualizer>();

        TrainingPipeline pipeline(extractor, trainer, evaluator, visualizer);

        /* 5. Run training -------------------------------------------------- */
        const std::string emptyPattern;          // no light-pattern file
        auto svmModel = pipeline.run(sources,
                                     {kLabels.begin(), kLabels.end()},
                                     emptyPattern);

        std::cout << "[INFO] Training complete – SVM stored in memory\n";
    }
    catch (const std::exception &ex) {
        std::cerr << "[ERROR] " << ex.what() << '\n';
    }
}




int main(int argc, const char **argv){
	runTests();
    OpcUaServer server;           
    g_server = &server;

    std::signal(SIGINT,  stopHandler);
    std::signal(SIGTERM, stopHandler);

    /* optional: react to running changes */
    server.onRunningChanged([](bool on){
        std::cout << "Machine is now " << (on ? "ON\n" : "OFF\n");
        startMachine();
        
    });

    return server.run(), 0;
	
	//trainer->predict(img, light_pattern_file, svm_model);
	
}
