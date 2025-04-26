CMAKE_MINIMUM_REQUIRED (VERSION 3.22)
CMAKE_POLICY (SET CMP0012 NEW)
PROJECT(Object_Classification)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
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

ADD_LIBRARY(core_interfaces INTERFACE)
TARGET_INCLUDE_DIRECTORIES(core_interfaces INTERFACE ${PROJECT_SOURCE_DIR}/include)

add_library(model_train          src/Chi2SVMTrainer.cpp)
target_link_libraries(model_train PUBLIC core_interfaces ${OpenCV_LIBS})

add_library(model_eval           src/SimpleAccuracyEvaluator.cpp)
target_link_libraries(model_eval PUBLIC core_interfaces ${OpenCV_LIBS})

add_library(plot_vis             src/ScatterPlotVisualizer.cpp)
target_link_libraries(plot_vis   PUBLIC core_interfaces ${OpenCV_LIBS})

add_library(pipeline             src/TrainingPipeline.cpp)
target_link_libraries(pipeline 
  PUBLIC
        core_interfaces
        model_train
        model_eval
        plot_vis
        ${OpenCV_LIBS})

SET(EXTRA_LIBS
        pipeline                                  
        training_and_testing 
        image_extract_features
        image_segmentation
         image_preprocessing
        multiple_image_window 
        alist_adt
        run_tests alist_adt_test 
        error
        ${OpenCV_LIBS} 
        ${Boost_LIBRARIES} 
        ${Python3_LIBRARIES})
ADD_EXECUTABLE(${PROJECT_NAME} main.cpp)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} PRIVATE ${EXTRA_LIBS})


if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
add_subdirectory(fuzzing)
endif()