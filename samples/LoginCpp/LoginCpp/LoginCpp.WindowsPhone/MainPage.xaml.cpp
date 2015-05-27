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

void MainPage::StartLogin(
    FBSession^ Session
    )
{
    // Assumes the Facebook App ID and Windows Phone Store ID have been saved
    // in the default resource file.
    ResourceLoader^ rl = ResourceLoader::GetForCurrentView();

    String^ appId = rl->GetString(FBAppIDName);
    String^ winAppId = rl->GetString(FBPhoneAppIDName);


    // IDs are both sent to FB app, so it can validate us.
    Session->FBAppId = appId;
    Session->WinAppId = winAppId;

    // These are the default permissions, needed to retrieve user info.
    Session->AddPermission("public_profile");
    //    Session->AddPermission("email");
    Session->AddPermission("user_friends");
    Session->AddPermission("user_likes");
    Session->AddPermission("user_groups");

    // Launches a URI to redirect to the FB app, which will log us in
    // and return the result via our registered protocol.
    task<FBResult^> cachedLogin = create_task(Session->LoginAndContinue());

    cachedLogin.then([](task<FBResult^> result)
    {
        try
        {
            FBResult^ unused = result.get();
        }
        catch (COMException^ ex)
        {
            ;
        }
    });
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
        StartLogin(sess);
    }
}
