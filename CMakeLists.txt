cmake_minimum_required(VERSION 2.6)
project(cpp_framework)

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



add_library(cpp_framework ${sources})


add_executable(cpp_framework_test main.cpp)
target_link_libraries(cpp_framework_test cpp_framework)

