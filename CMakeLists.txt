cmake_minimum_required(VERSION 3.14)

project(envparse VERSION 0.0.1 LANGUAGES CXX)

add_library(envparse INTERFACE)
add_library(envparse::envparse ALIAS envparse)

include(CMakeDependentOption)

# https://cmake.org/cmake/help/latest/variable/PROJECT_IS_TOP_LEVEL.html
string(COMPARE EQUAL ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR} PROJECT_IS_TOP_LEVEL)

option(BUILD_SHARED_LIBS "Build using shared libraries" ${PROJECT_IS_TOP_LEVEL})
option(ENVPARSE_INSTALL "Generate and install envparse target" ${PROJECT_IS_TOP_LEVEL})
option(ENVPARSE_TEST "Build and perform envparse tests" ${PROJECT_IS_TOP_LEVEL})
cmake_dependent_option(ENVPARSE_ENABLE_UBSAN_IN_DEBUG_BUILD "Enable UndefinedBehaviorSanitizer in debug build" ON ENVPARSE_TEST OFF)

# The implementation generally assumes a platform that implements at least C++17 support
target_compile_features(envparse INTERFACE "cxx_std_17")

# Setup include directory
add_subdirectory(include)

if (ENVPARSE_TEST)
    enable_testing()
    add_subdirectory(test)
endif()

if (ENVPARSE_INSTALL)
    include(GNUInstallDirs)
    include(CMakePackageConfigHelpers)

    install(TARGETS envparse DESTINATION ${CMAKE_INSTALL_LIBDIR})
    install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/envparse" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

    set(export_name "envparseConfig")
    set(namespace "envparse::")
    set(cmake_files_install_dir ${CMAKE_INSTALL_DATADIR}/cmake/envparse)

    install(TARGETS envparse EXPORT ${export_name} INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
    install(EXPORT ${export_name} NAMESPACE ${namespace} DESTINATION ${cmake_files_install_dir})
    export(TARGETS envparse NAMESPACE ${namespace} FILE ${export_name}.cmake)

    set(envparse_config_version "${CMAKE_CURRENT_BINARY_DIR}/envparseConfigVersion.cmake")

    write_basic_package_version_file(${envparse_config_version} COMPATIBILITY SameMajorVersion ARCH_INDEPENDENT)

    install(FILES ${envparse_config_version} DESTINATION ${cmake_files_install_dir})
endif()
