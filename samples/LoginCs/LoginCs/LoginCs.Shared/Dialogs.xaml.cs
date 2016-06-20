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
using System.Diagnostics;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace LoginCs
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Dialogs : Page
    {
        public Dialogs()
        {
            this.InitializeComponent();
        }
        private async void FeedDialogButton_Click(Object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            FBSession  s = FBSession.ActiveSession;
            if (!s.LoggedIn)
            {
                Debug.WriteLine("The user is no longer logged in.");
            }
            else
            {
                
                PropertySet parameters = new PropertySet();
                parameters.Add("caption", "I love Brussels Sprouts!");
                parameters.Add("link", "https://en.wikipedia.org/wiki/Brussels_sprout");
                parameters.Add("description", "Om Nom Nom!");
                FBResult result = await s.ShowFeedDialogAsync(parameters);
                Debug.WriteLine("Showed 'Feed' dialog");
            }
        }

        private async void AppRequestsButton_Click(Object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            FBSession  s = FBSession.ActiveSession;
            if (!s.LoggedIn)
            {
                Debug.WriteLine("The user is no longer logged in.");
            }
            else
            {
                PropertySet  parameters = new PropertySet();
                parameters.Add("title", "I love Brussels Sprouts!");
                parameters.Add("message", "Om Nom Nom!");
                FBResult result = await s.ShowRequestsDialogAsync(parameters);
                Debug.WriteLine("Showed 'Requests' dialog.");
            }
        }

        private async void SendDialogButton_Click(Object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            FBSession  s = FBSession.ActiveSession;
            if (!s.LoggedIn)
            {
                Debug.WriteLine("The user is no longer logged in.");
            }
            else
            {
                PropertySet parameters = new PropertySet();
                parameters.Add("link", "http://example.com");
                FBResult result = await s.ShowSendDialogAsync(parameters);
                Debug.WriteLine("Showed 'Send' dialog.");
            }
        }

        private void BackButton_Click(Object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
                Frame.GoBack();
        }
    }
}
