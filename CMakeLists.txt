cmake_minimum_required(VERSION 2.8)
project(GGJ2015)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../SSVCMake/cmake/modules/;${CMAKE_SOURCE_DIR}/extlibs/SSVCMake/cmake/modules/;${CMAKE_MODULE_PATH}")
include(SSVCMake)

SSVCMake_setDefaults()
SSVCMake_findSFML()
SSVCMake_findExtlib(SSVUtils)
SSVCMake_findExtlib(SSVStart)
# SSVCMake_findExtlib(SSVEntitySystem)
# SSVCMake_findExtlib(SSVSCollision)
# SSVCMake_findExtlib(SSVMenuSystem)

include_directories("./GGJ2015/")
add_executable(${PROJECT_NAME} ${SRC_LIST})
SSVCMake_linkSFML()

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/_RELEASE/)