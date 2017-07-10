
find_package(Qt5Widgets REQUIRED)

set(SRCS
        imdadm_process.hpp
        main.cpp
        main_window.cpp
        main_window.hpp
        mdadm_controller.cpp
        mdadm_controller.hpp
        mdadm_process.cpp
        mdadm_process.hpp
        utils.hpp
        disk.cpp
        disk.hpp
        disk_controller.cpp
        disk_controller.hpp
        idisk_filter.hpp
        empty_filter.cpp
        empty_filter.hpp
        exclude_used_filter.cpp
        exclude_used_filter.hpp
        create_raid_dialog.cpp
        create_raid_dialog.hpp
)

qt5_wrap_cpp(WRAPPED mdadm_controller.hpp)

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
find_package(GMock)
find_package(GTest)

if(GMOCK_FOUND AND GTEST_FOUND)

    find_package(Threads REQUIRED)

    qt5_wrap_cpp(WRAPPED_FOR_TESTS mdadm_controller.hpp)

    add_executable(mdadm_controller_tests
                        imdadm_process_mock.hpp
                        mdadm_controller_tests.cpp
                        mdadm_controller.cpp
                        ${WRAPPED_FOR_TESTS}
    )

    target_include_directories(mdadm_controller_tests
                                    SYSTEM PRIVATE
                                        ${GMOCK_INCLUDE_DIRS}
                                        ${GTEST_INCLUDE_DIRS}
                                        ${Qt5Core_INCLUDE_DIRS}
    )

    target_link_libraries(mdadm_controller_tests
                            PRIVATE
                                ${GMOCK_MAIN_LIBRARY}
                                ${GMOCK_LIBRARY}
                                ${GTEST_LIBRARY}
                                ${CMAKE_THREAD_LIBS_INIT}
                                Qt5::Core
    )

    add_test(NAME mdadm_controller_tests
             COMMAND mdadm_controller_tests
    )

    enable_testing()

    add_custom_target(RunUnitTests ALL
                      COMMAND ctest
                      DEPENDS mdadm_controller_tests
                      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )

endif()
