//******************************************************************************
//
// Copyright (c) 2015 Microsoft Corporation. All rights reserved.
//
// This code is licensed under the MIT License (MIT).
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
//******************************************************************************

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

using Facebook;
using Facebook.Graph;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace LoginCs
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class UserInfo : Page
    {
        public UserInfo()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            if (FBSession.ActiveSession.LoggedIn)
            {
                if (FBSession.ActiveSession.User != null)
                {
                    FBUser user = FBSession.ActiveSession.User;
                    UserId.Text = user.Id;
                    UserFirstName.Text = user.FirstName;
                    UserGender.Text = user.Gender;
                    UserLastName.Text = user.LastName;
                    UserLink.Text = user.Link.ToString();
                    UserLocale.Text = user.Locale;
                    UserName.Text = user.Name;
                    UserTimezone.Text = user.Timezone.ToString();
                    UserUpdatedTime.Text = user.UpdatedTime;
                    UserVerified.Text = user.Verified.ToString();
                    SquarePicture.UserId = user.Id;
                }
            }
        }
        private void UserLikesButton_Click(object sender, RoutedEventArgs e)
        {
            Frame.Navigate(typeof(UserLikes));
        }

        private void BackButton_Click(object sender, RoutedEventArgs e)
        {
            Frame.GoBack();
        }
    }
}
