

set(OPENCV_VER opencv)

pkg_check_modules(OPENCV opencv4)
if (OPENCV_FOUND)
    set(OPENCV_VER opencv4)
endif(OPENCV_FOUND)

rock_library(video_capture_vlc
    SOURCES VlcCapture.cpp
    HEADERS VlcCapture.h
    DEPS_PKGCONFIG ${OPENCV_VER}
    LIBS vlc harfbuzz freetype)

Rock_executable(video_capture_vlc_stream Main.cpp
    DEPS video_capture_vlc)

