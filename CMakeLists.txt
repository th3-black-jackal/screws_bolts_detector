CMAKE_MINIMUM_REQUIRED (VERSION 3.22)
CMAKE_POLICY (SET CMP0012 NEW)
PROJECT(Object_Classification)
FIND_PACKAGE(OpenCV 4.4.0 REQUIRED)
MESSAGE ("OpenCV Version: ${OpenCV_VERSION}")
INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})
MESSAGE ("OpenCV include: ${OpenCV_INCLUDE_DIRS}")
FIND_PACKAGE(Boost 1.8 COMPONENTS ALL)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)

find_package(Python3 COMPONENTS Development REQUIRED)
INCLUDE_DIRECTORIES(${Python3_INCLUDE_DIRS})

#Add src as include dir to avoid linking errors with template classses
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/class_templates)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/tests)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/tests/include)
ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(class_templates)
ADD_SUBDIRECTORY(tests)
ADD_SUBDIRECTORY(tests/src)
SET(EXTRA_LIBS ${OpenCV_LIBS} training_and_testing image_extract_features 
	image_segmentation image_preprocessing multiple_image_window alist_adt run_tests alist_adt_test error ${Boost_LIBRARIES})
ADD_EXECUTABLE(${PROJECT_NAME} main.cpp)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${EXTRA_LIBS})


if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
add_subdirectory(fuzzing)
endif()