cmake_minimum_required(VERSION 3.15)

project(force VERSION 1.0
              DESCRIPTION "Free Opensource Calculation Engine."
              LANGUAGES C CXX)

file(GLOB FMATH_HEADER           "include/fmath/*.hpp")

set(INC_PATH "include/")

source_group(fmath           FILES ${FMATH_HEADER})

add_executable            (force_math_test "test/math_test.cpp" ${FMATH_HEADER})
target_compile_features   (force_math_test PUBLIC cxx_std_23)
target_include_directories(force_math_test PUBLIC ${INC_PATH})