# Fuzzing harness setup
add_executable(fuzz_train fuzz_trainAndTest.cpp)

# Enable libFuzzer and sanitizers
target_compile_options(fuzz_train PRIVATE -fsanitize=fuzzer,address -O1 -g -std=c++17)
target_link_options(fuzz_train PRIVATE -fsanitize=fuzzer,address)

# Include all necessary headers and link to your modules
target_include_directories(fuzz_train PRIVATE 
    ${OpenCV_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${Python3_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/class_templates
)

target_link_libraries(fuzz_train PRIVATE 
    ${OpenCV_LIBS}
    ${Boost_LIBRARIES}
    ${Python3_LIBRARIES}
    pipeline
    training_and_testing
    image_extract_features
    image_segmentation
    image_preprocessing
    multiple_image_window
    alist_adt
)
