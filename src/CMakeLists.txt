
find_package(Qt5Widgets REQUIRED)

option(BUILD_TESTS "Build unit tests" ON)
option(RUN_UNIT_TESTS "Run unit test in post build step" ON)

set(SRCS
        imdadm_process.hpp
        main.cpp
        main_window.cpp
        main_window.hpp
        mdadm_controller.cpp
        mdadm_controller.hpp
        mdadm_debug_layer.cpp
        mdadm_debug_layer.hpp
        mdadm_process.cpp
        mdadm_process.hpp
        objects_ids.hpp
        utils.hpp
        disk_controller.cpp
        disk_controller.hpp
        idisk_filter.hpp
        idevice_visitor.hpp
        ifilesystem.hpp
        iraid_info_provider.hpp
        empty_filter.cpp
        empty_filter.hpp
        filesystem.cpp
        filesystem.hpp
        proc_watcher.cpp
        proc_watcher.hpp
        raid_info.cpp
        raid_info.hpp
        raid_info_provider.cpp
        raid_info_provider.hpp
        raids_model.cpp
        raids_model.hpp
        exclude_used_filter.cpp
        exclude_used_filter.hpp
        create_raid_dialog.cpp
        create_raid_dialog.hpp
        #block devices
        iblock_device.hpp
        block_device.cpp
        block_device.hpp
        disk.cpp
        disk.hpp
)

qt5_wrap_cpp(WRAPPED
                iraid_info_provider.hpp
                iproc_watcher.hpp
)

add_executable(mdadm-qt ${SRCS} ${WRAPPED})

target_include_directories(mdadm-qt
                            SYSTEM
                                PRIVATE
                                    ${Qt5Widgets_INCLUDE_DIRS}
)

target_link_libraries(mdadm-qt
                        Qt5::Widgets
)


# unit tests
if(BUILD_TESTS)

    find_package(GMock)
    find_package(GTest)

    if(GMOCK_FOUND AND GTEST_FOUND)

        find_package(Threads REQUIRED)

        qt5_wrap_cpp(WRAPPED_FOR_TESTS
                        iraid_info_provider.hpp
                        iproc_watcher.hpp
        )

        set(tests_sources

                # unit tests build dependencies
                block_device.cpp
                disk.cpp
                disk_controller.cpp
                empty_filter.cpp
                exclude_used_filter.cpp
                iblock_device_mock.hpp
                ifilesystem_mock.cpp
                ifilesystem_mock.hpp
                imdadm_process_mock.hpp
                iproc_watcher_mock.hpp
                iraid_info_provider_mock.hpp
                mdadm_controller.cpp
                proc_watcher.cpp
                raid_info.cpp
                raid_info.hpp
                raid_info_provider.cpp
                raids_model.cpp
                ${WRAPPED_FOR_TESTS}

                # unit tests utils
                printers_for_gmock.cpp

                # unit tests
                disk_tests.cpp
                disk_controller_tests.cpp
                filters_tests.cpp
                mdadm_controller_tests.cpp
                objects_ids_tests.cpp
                raid_info_tests.cpp
                raid_info_provider_tests.cpp
                raids_model_tests.cpp
        )

        add_executable(mdadm_unit_tests ${tests_sources})        # regular unit tests
        add_executable(mdadm_unit_tests_cc ${tests_sources})     # unit tests with code coverage

        target_compile_options(mdadm_unit_tests_cc PRIVATE -fprofile-arcs -ftest-coverage)

        # include dirs
        target_include_directories(mdadm_unit_tests
                                        SYSTEM PRIVATE
                                            ${GMOCK_INCLUDE_DIRS}
                                            ${GTEST_INCLUDE_DIRS}
                                            ${Qt5Core_INCLUDE_DIRS}
                                            ${Qt5Gui_INCLUDE_DIRS}
        )

        target_include_directories(mdadm_unit_tests_cc
                                        SYSTEM PRIVATE
                                            ${GMOCK_INCLUDE_DIRS}
                                            ${GTEST_INCLUDE_DIRS}
                                            ${Qt5Core_INCLUDE_DIRS}
                                            ${Qt5Gui_INCLUDE_DIRS}
        )

        # link stage
        target_link_libraries(mdadm_unit_tests
                                PRIVATE
                                    ${GMOCK_MAIN_LIBRARY}
                                    ${GMOCK_LIBRARY}
                                    ${GTEST_LIBRARY}
                                    ${CMAKE_THREAD_LIBS_INIT}
                                    Qt5::Core
                                    Qt5::Gui
        )

        target_link_libraries(mdadm_unit_tests_cc
                                PRIVATE
                                    ${GMOCK_MAIN_LIBRARY}
                                    ${GMOCK_LIBRARY}
                                    ${GTEST_LIBRARY}
                                    ${CMAKE_THREAD_LIBS_INIT}
                                    Qt5::Core
                                    Qt5::Gui
                                    gcov
        )

        add_test(NAME mdadm_unit_tests COMMAND mdadm_unit_tests)

        enable_testing()

        # run unit tests after all build steps
        if(RUN_UNIT_TESTS)
            add_custom_command(TARGET mdadm_unit_tests POST_BUILD
                               COMMAND ctest
                               WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            )
        endif()

        # test coverage
        include(CodeCoverage)

        setup_code_coverage()
        setup_target_for_coverage(run_unit_tests_code_coverage mdadm_unit_tests_cc coverage)

    endif(GMOCK_FOUND AND GTEST_FOUND)

endif(BUILD_TESTS)
