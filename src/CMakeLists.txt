rock_library(video_capture_vlc
    SOURCES VlcCapture.cpp
    HEADERS VlcCapture.h
    DEPS_PKGCONFIG opencv
    LIBS vlc)

Rock_executable(video_capture_vlc_stream stream.cpp
    DEPS video_capture_vlc)

