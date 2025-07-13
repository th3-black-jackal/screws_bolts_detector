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
#include <iostream>
#include <string>

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


static volatile UA_Boolean running = true;
static volatile UA_Boolean gMachineRunning = false; 
static UA_NodeId           gRunNodeId;

extern "C" void stopHandler(int sig) {
    (void)sig;
    running = false;
}


std::string light_pattern_file;


void startMachine(){
	cv::String img_file = "./000.png";
	light_pattern_file = "";
	int method_light = 1;
	int method_seg = 1;
	cv::String background_sub_algo = "MOG2";
	cv::Mat img = cv::imread(img_file, 0);
	
	
	if(img.data == NULL){
		std::cout<<"Error loading image "<<img_file<<std::endl;
		exit(1);
	}
	std::vector<std::string> dataset_sources;
	std::vector<int> labels;
	labels.push_back(0);
	labels.push_back(1);
	labels.push_back(2); 
	std::cout<<std::endl;

	std::string DATASET_ROOT_DIR = "../Dataset/dataset_white_background/data/";
	dataset_sources.push_back(DATASET_ROOT_DIR +  "nut/tuerca_%04d.pgm");
	dataset_sources.push_back(DATASET_ROOT_DIR + "ring/arandela_%04d.pgm");
	dataset_sources.push_back(DATASET_ROOT_DIR + "screw/tornillo_%04d.pgm");

	auto extractor	= std::make_shared<OpenCVFeatureExtractor>();
	auto trainer	= std::make_shared<Chi2SVMTrainer>();
	auto evaluator	= std::make_shared<SimpleAccuracyEvaluator>();
	auto visualizer	= std::make_shared<ScatterPlotVisualizer>();

	TrainingPipeline pipeline(extractor, trainer, evaluator, visualizer);
	auto svm_model = pipeline.run(dataset_sources, labels, light_pattern_file);

}

extern "C" void onWrite(UA_Server *server,
                        const UA_NodeId *sessionId, void *sessionContext,
                        const UA_NodeId *nodeId, void *nodeContext,
                        const UA_NumericRange *range, const UA_DataValue *data)
{
    (void)server; (void)sessionId; (void)sessionContext;
    (void)nodeId; (void)nodeContext; (void)range;

    if (!data || !data->hasValue)
        return;

    if (UA_Variant_isScalar(&data->value) &&
        data->value.type == &UA_TYPES[UA_TYPES_STRING]) {

        UA_String *uaStr = static_cast<UA_String*>(data->value.data);
        std::string cmd(reinterpret_cast<char*>(uaStr->data), uaStr->length);
        std::cout << "Client says: " << cmd << std::endl;
        std::cout.flush();
		startMachine();
    } else {
        std::puts("Client wrote a non-string value");
    }
}


int main(int argc, const char **argv){
	cv::CommandLineParser parser(argc, argv, keys);
	parser.about("Nasser implementation for object classification");
	if(parser.has("help")){
		parser.printMessage();
		return 0;
	}
	runTests();
	
	if(!parser.check()){
		parser.printErrors();
		return 0;
	}

	std::signal(SIGINT, stopHandler);
    std::signal(SIGTERM, stopHandler);

    UA_Server *server = UA_Server_new();
    UA_ServerConfig_setDefault(UA_Server_getConfig(server));

    // Add namespace index 2
    UA_UInt16 nsIdx = UA_Server_addNamespace(server, "Machine Namespace");

    // Create /Machine object
    UA_NodeId machineId;
    UA_ObjectAttributes machineAttr = UA_ObjectAttributes_default;
    machineAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Machine");
    UA_Server_addObjectNode(server,
        UA_NODEID_STRING(nsIdx, "/Machine"),
        UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
        UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
        UA_QUALIFIEDNAME(nsIdx, "Machine"),
        UA_NODEID_NUMERIC(0, UA_NS0ID_FOLDERTYPE),
        machineAttr, nullptr, &machineId);

    // Create /Machine/Status object
    UA_NodeId statusId;
    machineAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Status");
    UA_Server_addObjectNode(server,
        UA_NODEID_STRING(nsIdx, "/Machine/Status"),
        machineId,
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
        UA_QUALIFIEDNAME(nsIdx, "Status"),
        UA_NODEID_NUMERIC(0, UA_NS0ID_FOLDERTYPE),
        machineAttr, nullptr, &statusId);

    // Create /Machine/Command (writable string)
    UA_NodeId cmdNodeId = UA_NODEID_STRING(nsIdx, "/Machine/Command");
    UA_VariableAttributes cmdAttr = UA_VariableAttributes_default;
    UA_String emptyStr = UA_STRING("");
    UA_Variant_setScalar(&cmdAttr.value, &emptyStr, &UA_TYPES[UA_TYPES_STRING]);
    cmdAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Command");
    cmdAttr.dataType = UA_TYPES[UA_TYPES_STRING].typeId;
    cmdAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    UA_Server_addVariableNode(server,
        cmdNodeId,
        machineId,
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
        UA_QUALIFIEDNAME(nsIdx, "Command"),
        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
        cmdAttr, nullptr, nullptr);

    // Create /Machine/Status/Running (read-only boolean)
    UA_NodeId runNodeId = UA_NODEID_STRING(nsIdx, "/Machine/Status/Running");
    UA_VariableAttributes runAttr = UA_VariableAttributes_default;
    UA_Boolean initial = false;
    UA_Variant_setScalar(&runAttr.value, &initial, &UA_TYPES[UA_TYPES_BOOLEAN]);
    runAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Running");
    runAttr.dataType = UA_TYPES[UA_TYPES_BOOLEAN].typeId;
    runAttr.accessLevel = UA_ACCESSLEVELMASK_READ;
    UA_Server_addVariableNode(server,
        runNodeId,
        statusId,
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
        UA_QUALIFIEDNAME(nsIdx, "Running"),
        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
        runAttr, nullptr, nullptr);

    // Set write callback for /Machine/Command
    UA_ValueCallback cb;
    cb.onRead = nullptr;
    cb.onWrite = onWrite;
    UA_Server_setVariableNode_valueCallback(server, cmdNodeId, cb);

    std::puts("Server running on opc.tcp://0.0.0.0:4840");
    std::puts("Write to  ns=2;s=/Machine/Command");
    std::puts("Read     ns=2;s=/Machine/Status/Running");

    UA_Server_run(server, &running);
    UA_Server_delete(server);
    return 0;
	
	//trainer->predict(img, light_pattern_file, svm_model);
	return 0;
}
