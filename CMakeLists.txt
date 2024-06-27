cmake_minimum_required(VERSION 3.15)

project(force VERSION 1.0
              DESCRIPTION "Free Opensource Calculation Engine."
              LANGUAGES C CXX)

file(GLOB FORCE_HEADER       "include/force/*.hpp")
file(GLOB FORCE_MEDIA_HEADER "include/force/media/*.hpp")

set(INC_PATH "include/")
source_group(force       FILES ${FORCE_HEADER})
source_group(force/media FILES ${FORCE_MEDIA_HEADER})

add_executable            (force_math_test "test/math_test.cpp" ${FORCE_HEADER} ${FORCE_MEDIA_HEADER})
target_compile_features   (force_math_test PUBLIC cxx_std_23)
target_include_directories(force_math_test PUBLIC ${INC_PATH})