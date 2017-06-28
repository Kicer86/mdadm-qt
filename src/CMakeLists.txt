
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
        disk_filter.cpp
        disk_filter.hpp
)

add_executable(mdadm-qt ${SRCS})

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

    add_executable(mdadm_controller_tests
                        mdadm_controller_tests.cpp
                        mdadm_controller.cpp
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
                                ${CMAKE_THREAD_LIBS_INIT}
                                Qt5::Core
    )

endif()
