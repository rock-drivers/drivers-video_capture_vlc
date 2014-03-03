rock_library(video_capture_vlc
    SOURCES Dummy.cpp
    HEADERS Dummy.hpp)

rock_executable(video_capture_vlc_bin Main.cpp
    DEPS video_capture_vlc)

