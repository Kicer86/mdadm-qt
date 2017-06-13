
find_package(Qt5Widgets REQUIRED)

set(SRCS
        main.cpp
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
