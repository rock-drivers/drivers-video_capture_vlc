#include <boost/test/unit_test.hpp>
#include <video_capture_vlc/Dummy.hpp>

using namespace video_capture_vlc;

BOOST_AUTO_TEST_CASE(it_should_not_crash_when_welcome_is_called)
{
    video_capture_vlc::DummyClass dummy;
    dummy.welcome();
}
