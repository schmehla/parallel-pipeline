cmake_minimum_required (VERSION 3.22)

project(pipline VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

#add_compile_options(-fsanitize=address)
#add_link_options(-fsanitize=address)

add_executable(generate_pipeline data/generate_pipeline.cpp)
target_include_directories(generate_pipeline PRIVATE .)

add_executable(naive_timing eval/naive_timing.cpp)
target_include_directories(naive_timing PRIVATE .)

add_executable(tbb_timing eval/tbb_timing.cpp)
target_link_libraries(tbb_timing ${CMAKE_CURRENT_SOURCE_DIR}/tbb/my_installed_onetbb/lib/libtbb.so)
target_include_directories(tbb_timing PRIVATE .)
target_include_directories(tbb_timing PUBLIC tbb/oneTBB/include)

add_executable(static_parallel_timing eval/static_parallel_timing.cpp)
target_include_directories(static_parallel_timing PRIVATE .)

add_executable(dynamic_parallel_timing eval/dynamic_parallel_timing.cpp)
target_include_directories(dynamic_parallel_timing PRIVATE .)

add_executable(tests tests/test.cpp)
target_link_libraries(tests ${CMAKE_CURRENT_SOURCE_DIR}/tbb/my_installed_onetbb/lib/libtbb.so)
target_include_directories(tests PRIVATE .)
target_include_directories(tests PUBLIC tbb/oneTBB/include)