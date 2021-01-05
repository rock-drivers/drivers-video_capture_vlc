rock_library(video_capture_vlc
    SOURCES VlcCapture.cpp
    HEADERS VlcCapture.h
    DEPS_PKGCONFIG opencv
    LIBS vlc harfbuzz freetype)

Rock_executable(video_capture_vlc_stream Main.cpp
    DEPS video_capture_vlc)

