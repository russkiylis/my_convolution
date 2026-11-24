find_package(Qt5 5.15 REQUIRED COMPONENTS Gui Qml Quick QuickControls2 DataVisualization Concurrent)

add_executable(generator_viewer MACOSX_BUNDLE
    main.cpp
    trackpadorbit.h
    trackpadorbit.cpp
    viewerbackend.h
    viewerbackend.cpp
    viewer.qrc
)
set_target_properties(generator_viewer PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    MACOSX_BUNDLE_BUNDLE_NAME "Визуализатор свёрток"
)
target_link_libraries(generator_viewer PRIVATE
    generator
    Qt5::Gui Qt5::Qml Qt5::Quick Qt5::QuickControls2
    Qt5::DataVisualization Qt5::Concurrent
)
