#v1.0 copyright Comine.com 20240603M1137

cmake_minimum_required(VERSION 3.28)

########################################################
project(MEdit)

###################################
# C++ Version
###################################
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)


###################################
# Build Src Files
###################################
file(GLOB MSource "src/*.h" "src/*.cpp" "src/*.c" )
add_executable(${PROJECT_NAME} )
target_sources(${PROJECT_NAME} PRIVATE ${MSource} )

###################################
install(TARGETS ${PROJECT_NAME} DESTINATION bin)


