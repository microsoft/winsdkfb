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
#include "UserInfo.xaml.h"
#include "OptionsPage.xaml.h"

using namespace concurrency;
using namespace Facebook;
using namespace LoginCpp;
using namespace Platform;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Resources;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Security::Authentication::Web;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

#define FBAppIDName L"FBApplicationId"
#define FBStoreAppIDName L"FBWindowsAppId"

MainPage::MainPage()
{
	InitializeComponent();

	FBSession^ s = FBSession::ActiveSession;

	String^ whatever = WebAuthenticationBroker::GetCurrentApplicationCallbackUri()->DisplayUri + L"\n";
	OutputDebugString(whatever->Data());

	// Assumes the Facebook App ID and Windows Phone Store ID have been saved
	// in the default resource file.
	// TODO: Commenting this out for now - resource loader isn't working for me in UWP app.
	ResourceLoader^ rl = ResourceLoader::GetForCurrentView();

	String^ appId = rl->GetString(FBAppIDName);
	String^ winAppId = rl->GetString(FBStoreAppIDName);

	// IDs are both sent to FB app, so it can validate us.
	s->FBAppId = appId;
	s->WinAppId = winAppId;
}

BOOL MainPage::DidGetAllRequestedPermissions(
    )
{
    // TODO: Need to actually check permissions here.
    return FALSE;
}

void MainPage::NavigateToOptionsPage()
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

void MainPage::login_OnClicked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	FBSession^ sess = FBSession::ActiveSession;

	if (sess->LoggedIn)
	{
		sess->Logout();
		LoginButton->Content = L"Logout";
		LoginCpp::App^ a = dynamic_cast<LoginCpp::App^>(Application::Current);
		Windows::UI::Xaml::Controls::Frame^ f = a->CreateRootFrame();
		f->Navigate(MainPage::typeid);
	}
	else
	{
		//sess->AddPermission("public_profile");
		//sess->AddPermission("user_friends");
		//sess->AddPermission("user_likes");
		//sess->AddPermission("user_groups");
		//sess->AddPermission("user_location");

        PropertySet^ parameters = ref new PropertySet();

        parameters->Insert(L"scope",
            L"public_profile,user_friends,user_likes,user_groups,user_location");

		create_task(sess->LoginAsync(parameters)).then([=](FBResult^ result)
		{
			if (result->Succeeded)
			{
                if (DidGetAllRequestedPermissions())
                {
                    NavigateToOptionsPage();
                }
                else
                {
                    PropertySet^ reRequestParams = ref new PropertySet();

                    reRequestParams->Insert(L"scope",
                        L"public_profile,user_friends,user_likes,user_groups,user_location");
                    reRequestParams->Insert(L"auth_type", L"rerequest");
                    create_task(sess->LoginAsync(reRequestParams)).then([=](FBResult^ result)
                    {
                        if (result->Succeeded)
                        {
                            NavigateToOptionsPage();
                        }
                    });
                }
			}
		});
	}
}
