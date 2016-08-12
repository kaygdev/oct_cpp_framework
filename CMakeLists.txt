cmake_minimum_required(VERSION 2.6)
project(cpp_framework)

find_package(Boost COMPONENTS unit_test_framework REQUIRED)
find_package(OpenCV REQUIRED)


include_directories(${CMAKE_SOURCE_DIR}/cpp_framework/)
include_directories(${Boost_INCLUDE_DIRS})

if(CMAKE_COMPILER_IS_GNUCXX)
	set(compiler_flags_debug "-Wall -Wextra -Wunreachable-code -Werror")        ## -Weverything
	set(CMAKE_CXX_FLAGS "-std=c++11")

	if(${WIN32})
		set(CMAKE_CXX_FLAGS "-mno-ms-bitfields -static -static-libstdc++ ${CMAKE_CXX_FLAGS}")
	endif()
endif()


file(GLOB sources_b "${CMAKE_CURRENT_SOURCE_DIR}/cpp_framework/*.cpp")
file(GLOB sources "${CMAKE_CURRENT_SOURCE_DIR}/cpp_framework/**/*.cpp")

list(APPEND sources ${sources_b})



if(compiler_flags_debug)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${compiler_flags_debug}")
endif()
if(compiler_flags_release)
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${compiler_flags_release}")
endif()
if(compiler_flags)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${compiler_flags}")
endif()


add_library(cpp_framework main.cpp ${sources})


ADD_DEFINITIONS(-DBOOST_TEST_DYN_LINK)

file(GLOB sources_test "${CMAKE_CURRENT_SOURCE_DIR}/src_tests/*.cpp")
add_executable(cpp_framework_test ${sources_test})

target_link_libraries(cpp_framework_test ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} )
target_link_libraries(cpp_framework_test cpp_framework)
target_link_libraries(cpp_framework_test ${OpenCV_LIBRARIES})

