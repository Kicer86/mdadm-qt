
find_package(Qt5Widgets REQUIRED)

find_package(GMock)
find_package(GTest)

set(SRCS
        imdadm_process.hpp
        main.cpp
        main_window.cpp
        main_window.hpp
        mdadm_controller.cpp
        mdadm_controller.hpp
        mdadm_process.cpp
        mdadm_process.hpp
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

if(GMOCK_FOUND)

    find_package(Threads REQUIRED)

    add_executable(mdadm_controller_tests mdadm_controller_tests.cpp)

    target_link_libraries(mdadm_controller_tests ${GMOCK_MAIN_LIBRARY} ${GMOCK_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})

endif()
