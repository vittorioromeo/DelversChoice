cmake_minimum_required(VERSION 3.0)

# Include SSVCmake.
list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_SOURCE_DIR}/../SSVCMake/cmake/modules/"
    "${CMAKE_SOURCE_DIR}/extlibs/SSVCMake/cmake/modules/")

include(SSVCMake)

# Basic project setup.
vrm_cmake_init_project(NetLayer)

SSVCMake_findSFML()
vrm_cmake_find_extlib(vrm_pp)
vrm_cmake_find_extlib(SSVUtils)
vrm_cmake_find_extlib(SSVStart)

# Alias the include directory.
set(NETLAYER_INC_DIR "${NETLAYER_SOURCE_DIR}/include/")
set(NETLAYER_SRC_DIR "${NETLAYER_SOURCE_DIR}/src/")

# Other compiler flags.
vrm_cmake_add_common_compiler_flags()
vrm_cmake_add_compiler_flag_pthread()

# Setup user options.
vrm_cmake_add_option_memcheck()
vrm_cmake_add_option_no_exceptions()
vrm_cmake_add_option_werror()

# The `check` target runs all tests and examples.
vrm_check_target()

# Setup subdirectories.
add_subdirectory(test)

# Create header-only install target (automatically glob)
# vrm_cmake_header_only_install_glob("${NETLAYER_INC_DIR}" "include")

file(GLOB_RECURSE SRC_LIST "${NETLAYER_INC_DIR}/*" "${NETLAYER_SRC_DIR}/*")
add_executable(${PROJECT_NAME} ${SRC_LIST})

SSVCMake_linkSFML()
vrm_cmake_link_pthread()

    target_link_libraries(${PROJECT_NAME} sqlpp-mysql)
    target_link_libraries(${PROJECT_NAME} mysqlclient)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/_RELEASE/)

# TODO: simplify through vrm_cmake
