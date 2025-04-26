#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <regex>
#include "AListADTTest.hpp"


/*
 * Nasser 24-08-2022 20:49
 * As the system consists of many components, I need a detailed report about the errors, so I will try to create a custom class that handles excpetion, the class will contain
 * -Function name
 * -Error message
 * -Error level
 *  Any way, unlike assertion I want to continue running tests to get an overview of the failers of the system, and if the errors are greater than 1 then I will ask the user
 *  to check the PDF report
 */


/*
* Nasser 26-04-2025
* I'm starting the project again
*/


void generateStartupErrors(const std::vector<Error>& errors);
void printErrorsVector(const std::vector<Error>& errors);


int runTests(){
	/*
	 * I will use a string exception for now, until I create the custom exception class
	 */
	std::shared_ptr<std::vector<Error>> errors = std::make_shared<std::vector<Error>>();
	AListADTTest *listTest = new AListADTTest(errors); 
	listTest->runTests();
	if(errors->size() > 0){
		std::cout<<"Couldn't start the system please check the error report"<<std::endl;
		generateStartupErrors(*errors.get());
	}
	return 0;

}


void generateStartupErrors(const std::vector<Error>& errors){
	std::vector<Error> tags_errors;
	std::string rows = "";
	std::string matching_variable = "errors";	
	for(int i = 0;i < errors.size(); i++){
		tags_errors.push_back(errors[i]);
		std::string row = "<tr><td>" + errors[i].getErrorFunction() + "</td><td>" + errors[i].getErrorMessage() + "</tr><td>";
		rows += row;
	}
	printErrorsVector(tags_errors);
	std::string report_template {"system_startup_report.html"};
	std::ifstream inFile {report_template};
	if(!inFile){
		std::cout <<"Faield to open file"<< report_template <<std::endl;
		
	}
	std::string line {};
	std::string report_body {};
	size_t count {};
	size_t perline {6};
	char c;
	while(getline(inFile, line)){
		if(line.empty()){
			continue;
		}
		std::istringstream iss(line);
		
	        report_body += line;	
	}
	inFile.close();
	std::regex errors_regx ("(\\{){2}(\\s)*" + matching_variable + "(\\s)*(\\}){2}");
	std::string rendered_errors = std::regex_replace(report_body, errors_regx, rows);	
	std::ofstream outReportStream {"rendered_report.html"};
	outReportStream<<rendered_errors;
	outReportStream.close();
}


void printErrorsVector(const std::vector<Error>& errors){
	for(int i = 0;i < errors.size();i++){
		std::string error_msg = errors[i].getErrorMessage();
		std::cout<<errors[i].getErrorMessage()<<std::endl;
	}
}
