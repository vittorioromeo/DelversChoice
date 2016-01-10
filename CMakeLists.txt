cmake_minimum_required(VERSION 3.0)

# Include SSVCmake.
list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_SOURCE_DIR}/../SSVCMake/cmake/modules/"
    "${CMAKE_SOURCE_DIR}/extlibs/SSVCMake/cmake/modules/")

include(SSVCMake)

# Basic project setup.
vrm_cmake_init_project(GGJ2015)
vrm_cmake_find_extlib(vrm_pp)
vrm_cmake_find_extlib(SSVUtils)
vrm_cmake_find_extlib(SSVStart)
SSVCMake_findSFML()

SSVCMake_setDefaults()
vrm_cmake_add_common_compiler_flags()

include_directories("./GGJ2015/")
add_executable(${PROJECT_NAME} ${SRC_LIST})
SSVCMake_linkSFML()

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/_RELEASE/)
