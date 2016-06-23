#include "pch.h"
#include "ProfilePictureControlSample.xaml.h"

using namespace SampleCode;
using namespace Windows::Foundation::Collections;
using namespace Platform;
using namespace concurrency;
using namespace winsdkfb;
using namespace winsdkfb::Graph;

ProfilePictureControlSample::ProfilePictureControlSample()
{
	InitializeComponent();
    // Get active session
    FBSession^ sess = FBSession::ActiveSession;
    if (sess->LoggedIn)
    {
        // Get current user
        FBUser^ user = sess->User;
        // Set profile pic
        ProfilePic->UserId = user->Id;
    }
}
