cmake_minimum_required(VERSION 3.8)

set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_DISABLE_SOURCE_CHANGES  ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "_CMakePredefinedTargets")

project(Crypto)

if(WIN32)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -W -Werror")
endif()

if(WIN32)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /FR /MP /std:c++latest")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD /std:c++latest")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /incremental")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/tools/cmake/modules)

add_definitions(-D_HAS_AUTO_PTR_ETC=1)

#################
# CMake Modules #
#################

include(AssignSourceGroup)

#############
# Libraries #
#############

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost REQUIRED COMPONENTS log)

############
# Includes #
############

include_directories(
	"${PROJECT_SOURCE_DIR}/include"
    "${PROJECT_SOURCE_DIR}/src"
	# Libraries
	"${Boost_INCLUDE_DIRS}"
)

###########
# Sources #
###########
include(CMakeLists/sources.cmake)

###########
# Headers #
###########
include(CMakeLists/headers.cmake)

##################
# Build and link #
##################

add_executable(Crypto
	${MyTestSrc}
	${MyTestHeaders}
    ${MyTestInternalHeaders}
)

target_link_libraries(Crypto
	${Boost_LIBRARIES}
)

set_target_properties(Crypto PROPERTIES FOLDER "MyTest")
set_target_properties(Crypto PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin")