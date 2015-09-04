cmake_minimum_required(VERSION 2.8)
project(DCServer)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../../SSVCMake/cmake/modules/;${CMAKE_SOURCE_DIR}/../extlibs/SSVCMake/cmake/modules/;${CMAKE_MODULE_PATH}")
include(SSVCMake)

SSVCMake_setDefaults()
find_package(Boost)
SSVCMake_findSFML()
SSVCMake_findExtlibIn(SSVUtils "../..")
SSVCMake_findExtlibIn(SSVStart "../..")
# SSVCMake_findExtlib(SSVEntitySystem)
# SSVCMake_findExtlib(SSVSCollision)
# SSVCMake_findExtlib(SSVMenuSystem)

include_directories("./DCServer/")
add_executable(${PROJECT_NAME} ${SRC_LIST})
SSVCMake_linkSFML()

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/_RELEASE/)
