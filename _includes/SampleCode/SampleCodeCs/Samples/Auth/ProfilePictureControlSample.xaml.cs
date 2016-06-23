using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using winsdkfb;
using winsdkfb.Graph;

namespace SampleCode
{
    public sealed partial class ProfilePictureControlSample : Page
    {
        public ProfilePictureControlSample()
        {
            this.InitializeComponent();
            // Get active session
            FBSession sess = FBSession.ActiveSession;
            if(sess.LoggedIn)
            {
                // Get current user
                FBUser user = sess.User;
                // Set profile pic
                ProfilePic.UserId = user.Id;
            }
        }
    }
}
