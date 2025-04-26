CMAKE_MINIMUM_REQUIRED (VERSION 3.22)
CMAKE_POLICY (SET CMP0012 NEW)
PROJECT(Object_Classification)
FIND_PACKAGE(OpenCV 4.4.0 REQUIRED)
MESSAGE ("OpenCV Version: ${OpenCV_VERSION}")
MESSAGE ("OpenCV include: ${OpenCV_INCLUDE_DIRS}")
FIND_PACKAGE(Boost 1.8 COMPONENTS ALL)

FIND_PACKAGE(Python3 COMPONENTS Development REQUIRED)

# ── Include paths ────────────────────────────────────────────────────────
INCLUDE_DIRECTORIES(
    ${OpenCV_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${Python3_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/class_templates
    ${PROJECT_SOURCE_DIR}/tests
    ${PROJECT_SOURCE_DIR}/tests/include
)
ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(class_templates)
ADD_SUBDIRECTORY(tests)
ADD_SUBDIRECTORY(tests/src)
SET(EXTRA_LIBS 
	${OpenCV_LIBS} 
	${Boost_LIBRARIES}
	${Python3_LIBRARIES}
	training_and_testing 
	image_extract_features 
	image_segmentation
	image_preprocessing 
	multiple_image_window 
	alist_adt run_tests 
	alist_adt_test 
	error 
	)
ADD_EXECUTABLE(${PROJECT_NAME} main.cpp)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${EXTRA_LIBS})


if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
add_subdirectory(fuzzing)
endif()