# Add a custom target for the unit tests.
add_custom_target(tests COMMENT "Build all the unit tests.")

# `check` depends on `tests`.
add_dependencies(check tests)

# Glob all tests.
file(GLOB_RECURSE NETLAYER_TEST_SOURCES "*.cpp")

# Include directories.
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/include)
include_directories(${CMAKE_CURRENT_LIST_DIR})

# vrm_cmake_generate_public_header_tests_glob("*.hpp" "${NETLAYER_SOURCE_DIR}/include")
#
# foreach(_t IN LISTS vrm_cmake_out)
#     target_link_libraries(${_t} ${SFML_AUDIO_LIBRARY_RELEASE})
#     target_link_libraries(${_t} ${SFML_GRAPHICS_LIBRARY_RELEASE})
#     target_link_libraries(${_t} ${SFML_WINDOW_LIBRARY_RELEASE})
#     target_link_libraries(${_t} ${SFML_SYSTEM_LIBRARY_RELEASE})
#     target_link_libraries(${_t} ${SFML_NETWORK_LIBRARY_RELEASE})
# endforeach()

# Add all the unit tests.
vrm_cmake_generate_unit_tests(NETLAYER_TEST_SOURCES)

foreach(_t IN LISTS vrm_cmake_out)
    target_link_libraries(${_t} ${SFML_AUDIO_LIBRARY_RELEASE})
    target_link_libraries(${_t} ${SFML_GRAPHICS_LIBRARY_RELEASE})
    target_link_libraries(${_t} ${SFML_WINDOW_LIBRARY_RELEASE})
    target_link_libraries(${_t} ${SFML_SYSTEM_LIBRARY_RELEASE})
    target_link_libraries(${_t} ${SFML_NETWORK_LIBRARY_RELEASE})
    target_link_libraries(${_t} -lpthread)
endforeach()

# TODO: rewrite
# TODO: add tests

# TODO: simplify through vrm_cmake
