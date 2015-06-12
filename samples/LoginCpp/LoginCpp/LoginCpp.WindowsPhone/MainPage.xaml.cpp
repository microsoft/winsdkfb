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

//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"
#include "OptionsPage.xaml.h"

using namespace LoginCpp;

using namespace concurrency;
using namespace Platform;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::ApplicationModel::Resources;
using namespace Windows::Globalization;
using namespace Facebook;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

#define FBAppIDName L"FBApplicationId"
#define FBPhoneAppIDName L"FBWinPhoneAppId"


MainPage::MainPage()
{
	InitializeComponent();

    // Assumes the Facebook App ID and Windows Phone Store ID have been saved
    // in the default resource file.
    ResourceLoader^ rl = ResourceLoader::GetForCurrentView();

    FBSession^ s = FBSession::ActiveSession;

    String^ appId = rl->GetString(FBAppIDName);
    String^ winAppId = rl->GetString(FBPhoneAppIDName);


    // IDs are both sent to FB app, so it can validate us.
    s->FBAppId = appId;
    s->WinAppId = winAppId;
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void MainPage::OnNavigatedTo(NavigationEventArgs^ e)
{
    (void)e;	// Unused parameter

    // TODO: Prepare page for display here.

    // TODO: If your application contains multiple pages, ensure that you are
    // handling the hardware Back button by registering for the
    // Windows::Phone::UI::Input::HardwareButtons.BackPressed event.
    // If you are using the NavigationHelper provided by some templates,
    // this event is handled for you.
    FBSession^ sess = FBSession::ActiveSession;

    if (sess->LoggedIn)
    {
        LoginButton->Content = L"Logout";
        Calendar^ cal = ref new Calendar();
        cal->SetDateTime(sess->AccessTokenData->ExpirationDate);

        ResponseText->Text = sess->AccessTokenData->AccessToken;

        ExpirationDate->Text = cal->DayOfWeekAsString() + "," + cal->YearAsString() + "/" +
            cal->MonthAsNumericString() + "/" + cal->DayAsString() + ", " +
            cal->HourAsPaddedString(2) + ":" + cal->MinuteAsPaddedString(2) + ":" +
            cal->SecondAsPaddedString(2);
    }
}

void LoginCpp::MainPage::LoginButton_Click(
    Platform::Object^ sender,
    Windows::UI::Xaml::RoutedEventArgs^ e
    )
{
    // Get the active session, set it up with the parameters it needs, and 
    // start Login process.
    FBSession^ sess = FBSession::ActiveSession;

    if (sess->LoggedIn)
    {
        sess->Logout();
        LoginButton->Content = L"Login";
        LoginCpp::App^ a = dynamic_cast<LoginCpp::App^>(Application::Current);
        Windows::UI::Xaml::Controls::Frame^ f = a->CreateRootFrame();
        f->Navigate(MainPage::typeid);
    }
    else
    {
        sess->AddPermission("public_profile");
        sess->AddPermission("user_friends");
        sess->AddPermission("user_likes");
        sess->AddPermission("user_groups");
        sess->AddPermission("user_location");

        create_task(sess->LoginAsync()).then([=](FBResult^ result)
        {
            if (result->Succeeded)
            {
                LoginButton->Content = L"Logout";

                // We're redirecting to a page that shows simple user info, so 
                // have to dispatch back to the UI thread.
                CoreWindow^ wind = CoreApplication::MainView->CoreWindow;

                if (wind)
                {
                    CoreDispatcher^ disp = wind->Dispatcher;
                    if (disp)
                    {
                        disp->RunAsync(
                            Windows::UI::Core::CoreDispatcherPriority::Normal,
                            ref new Windows::UI::Core::DispatchedHandler([this]()
                        {
                            LoginCpp::App^ a = dynamic_cast<LoginCpp::App^>(Application::Current);
                            Windows::UI::Xaml::Controls::Frame^ f = a->CreateRootFrame();
                            f->Navigate(OptionsPage::typeid);
                        }));
                    }
                }
            }
        });
    }
}
