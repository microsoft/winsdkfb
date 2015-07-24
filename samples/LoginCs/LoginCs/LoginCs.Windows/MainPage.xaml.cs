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
using Windows.Security.Authentication.Web;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

using Facebook;
using Windows.Globalization;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace LoginCs
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        public MainPage()
        {
            this.InitializeComponent();
        }

        private const string _granted = "granted";

        private string[] requested_permissions =
        {
            "public_profile",
            "email",
            "user_friends",
            "publish_actions"
        };

        private string BuildPermissionsString()
        {
            string permissions = "";

            for (uint i = 0; i < requested_permissions.Length; i++)
            {
                if (i > 0)
                {
                    permissions += ",";
                }

                permissions += requested_permissions[i];
            }

            return permissions;
        }

        private bool DidGetRequestedPermissions(
            )
        {
            bool gotPermissions = false;
            uint grantedCount = 0;

            if (FBSession.ActiveSession.AccessTokenData != null)
            {
                IReadOnlyDictionary<string, string> Permissions = 
                    FBSession.ActiveSession.AccessTokenData.Permissions;

                for (uint i = 0; i < requested_permissions.Length; i++)
                {
                    if (Permissions.ContainsKey(requested_permissions[i]))
                    {
                        if (String.CompareOrdinal(Permissions[requested_permissions[i]], _granted) == 0)
                        {
                            grantedCount++;
                        }
                    }
                }

                if (grantedCount == requested_permissions.Length)
                {
                    gotPermissions = true;
                }
            }

            return gotPermissions;
        }

        private async void LoginToFB()
        {
            Uri endURI = 
            WebAuthenticationBroker.GetCurrentApplicationCallbackUri();
            string uriString = endURI.ToString();
            PropertySet parameters = new PropertySet();

            parameters.Add(new KeyValuePair<String, Object>("scope", 
                BuildPermissionsString()));

            FBResult result = await FBSession.ActiveSession.LoginAsync(parameters);
            if ((!result.Succeeded) &&
                (((ErrorCode)result.ErrorInfo.Code == ErrorCode.ErrorCodeOauthException) && 
                 ((ErrorSubcode)result.ErrorInfo.Subcode == ErrorSubcode.ErrorSubcodeSessionInvalidated)))
            {
                parameters.Add(new KeyValuePair<String, Object>("auth_type", "rerequest"));
                result = await FBSession.ActiveSession.LoginAsync(parameters);
            }

            if (result.Succeeded)
            {
                if (DidGetRequestedPermissions())
                {
                    Frame.Navigate(typeof(UserInfo));
                }
                else
                {
                    if (!parameters.ContainsKey("auth_type"))
                    {
                        parameters.Add(new KeyValuePair<String, Object>("auth_type", "rerequest"));
                    }
                    result = await FBSession.ActiveSession.LoginAsync(parameters);
                    if (result.Succeeded)
                    {
                        Frame.Navigate(typeof(UserInfo));
                    }
                }
            }
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.
        /// This parameter is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            // TODO: Prepare page for display here.

            // TODO: If your application contains multiple pages, ensure that 
            // you are handling the hardware Back button by registering for the
            // Windows.Phone.UI.Input.HardwareButtons.BackPressed event.
            // If you are using the NavigationHelper provided by some templates,
            // this event is handled for you.
            FBSession sess = FBSession.ActiveSession;

            if (sess.LoggedIn)
            {
                LoginButton.Content = "Logout";
                Calendar cal = new Calendar();
                cal.SetDateTime(sess.AccessTokenData.ExpirationDate);

                ResponseText.Text = sess.AccessTokenData.AccessToken;

                ExpirationDate.Text = cal.DayOfWeekAsString() + "," +
                    cal.YearAsString() + "/" + cal.MonthAsNumericString() +
                    "/" + cal.DayAsString() + ", " + cal.HourAsPaddedString(2) +
                    ":" + cal.MinuteAsPaddedString(2) + ":" +
                    cal.SecondAsPaddedString(2);
            }
            else
            {
                App.InitializeFBSession();
            }
        }

        private async void login_OnClicked(object sender, RoutedEventArgs e)
        {
            FBSession sess = FBSession.ActiveSession;
            if (sess.LoggedIn)
            {
                LoginButton.Content = "Login";
                await sess.Logout();
                //Navigate back to same page, to clear out logged in info.
                App.RootFrame.Navigate(typeof(MainPage));
            }
            else
            {
                LoginButton.Content = "Logout";
                LoginToFB();
            }
        }
    }
}
