project(unittests)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(. SRC_LIST)
FILE(GLOB_RECURSE SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
FILE(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)
FILE(GLOB_RECURSE HEADERS_TOBETESTED ${CMAKE_CURRENT_SOURCE_DIR}/../robocommon/*.h) #to be tested headers
add_executable(${PROJECT_NAME} ${SOURCE} ${HEADERS} ${HEADERS_TOBETESTED})
target_link_libraries(${PROJECT_NAME} gtest_main gmock gmock_main)

# C++11
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
		message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wno-missing-field-initializers -Wno-type-limits -Wno-sign-compare -Wno-missing-braces")

#gtest
add_subdirectory(gmock-1.7.0)

#includes
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../robocommon/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/gmock-1.7.0/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/gmock-1.7.0/gtest/include)

IF(CMAKE_BUILD_TYPE MATCHES Debug)
	add_definitions(-DDEBUG)
ENDIF()
