cmake_minimum_required(VERSION 2.6)
project(cpp_framework)

option(BUILD_UNIT_TESTS "build unit tests" OFF)

if(BUILD_UNIT_TESTS)
	find_package(Boost COMPONENTS unit_test_framework filesystem system REQUIRED)
else()
	find_package(Boost COMPONENTS filesystem system REQUIRED)
endif()

find_package(OpenCV REQUIRED)



include_directories(${CMAKE_SOURCE_DIR}/cpp_framework/)
include_directories(${Boost_INCLUDE_DIRS})

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

