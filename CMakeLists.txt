cmake_minimum_required(VERSION 2.6)
project(cpp_framework)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake_module/)

set( CMAKE_CXX_STANDARD 11 )

option(BUILD_UNIT_TESTS              "build unit tests"              OFF)
option(BUILD_MATLAB_MEX_FUNCTIONS    "build matlab mex functions"    OFF)
option(BUILD_OCTAVE_MEX_FUNCTIONS    "build octave mex functions"    OFF)
option(BUILD_MEX_WITH_STATIC_CPP_LIB "build mex with static c++ lib" OFF)
option(BUILD_WITH_ZLIB               "build the programms with ZLIB" ON)

if(BUILD_UNIT_TESTS)
	find_package(Boost COMPONENTS unit_test_framework filesystem system REQUIRED)
else()
	find_package(Boost COMPONENTS filesystem system REQUIRED)
endif()

find_package(OpenCV REQUIRED)


if(BUILD_WITH_ZLIB)
	find_package(ZLIB REQUIRED)
	include_directories(${ZLIB_INCLUDE_DIRS})
	add_definitions(-DWITH_ZLIB)
endif()


include_directories(${CMAKE_SOURCE_DIR}/cpp_framework/)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${OpenCV_INCLUDE_DIRS})

if(CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS_DEBUG "-Wall -Wextra -Wunreachable-code -Werror ${CMAKE_CXX_FLAGS_DEBUG}")        ## -Weverything
	set(CMAKE_CXX_FLAGS "-std=c++11 -fPIC ${CMAKE_CXX_FLAGS}")

	if(${WIN32})
		set(CMAKE_CXX_FLAGS "-mno-ms-bitfields -static -static-libstdc++ ${CMAKE_CXX_FLAGS}")
	endif()
endif()


file(GLOB sources_b "${CMAKE_CURRENT_SOURCE_DIR}/cpp_framework/*.cpp")
file(GLOB sources "${CMAKE_CURRENT_SOURCE_DIR}/cpp_framework/**/*.cpp")

list(APPEND sources ${sources_b})



add_library(cpp_framework ${sources})


if(BUILD_UNIT_TESTS)
	ADD_DEFINITIONS(-DBOOST_TEST_DYN_LINK)

	file(GLOB sources_test "${CMAKE_CURRENT_SOURCE_DIR}/src_tests/*.cpp")
	add_executable(cpp_framework_test main.cpp ${sources_test})

	target_link_libraries(cpp_framework_test ${Boost_LIBRARIES} )
	target_link_libraries(cpp_framework_test cpp_framework)
	target_link_libraries(cpp_framework_test ${OpenCV_LIBRARIES})
endif()


if(BUILD_MATLAB_MEX_FUNCTIONS)
	find_package(Matlab COMPONENTS MX_LIBRARY REQUIRED)

	matlab_add_mex(NAME readbin SRC matlab/bin_format/readbin.cpp)
	set_target_properties(readbin PROPERTIES COMPILE_DEFINITIONS "MEX_COMPILE")
	if(BUILD_MEX_WITH_STATIC_CPP_LIB)
		set_target_properties(readbin PROPERTIES LINK_FLAGS "-static-libstdc++")
	endif()

	target_include_directories(readbin SYSTEM PRIVATE ${Matlab_INCLUDE_DIRS} PUBLIC ${CMAKE_SOURCE_DIR}/src/)
endif()


if(BUILD_OCTAVE_MEX_FUNCTIONS)
	find_package(Octave COMPONENTS MX_LIBRARY REQUIRED)

	octave_add_oct(oct_readbin SOURCES matlab/bin_format/readbin.cpp EXTENSION mex)
	set_target_properties(oct_readbin PROPERTIES COMPILE_DEFINITIONS "MEX_COMPILE")
	if(BUILD_MEX_WITH_STATIC_CPP_LIB)
		set_target_properties(oct_readbin PROPERTIES LINK_FLAGS "-static-libstdc++")
	endif()
	target_include_directories(oct_readbin SYSTEM PRIVATE ${OCTAVE_INCLUDE_DIRS} PUBLIC ${CMAKE_SOURCE_DIR}/src/)

endif()

