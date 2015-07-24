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
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Resources;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Facebook;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

#define FBAppIDName L"FBApplicationId"
#define FBStoreAppIDName L"FBWindowsAppId"
#define PermissionGranted L"granted"

const wchar_t* requested_permissions[] =
{
	L"public_profile",
	L"user_friends",
	L"user_likes",
	L"user_groups",
	L"user_location"
};

MainPage::MainPage()
{
	InitializeComponent();
	SetSessionAppIds();
}

void MainPage::SetSessionAppIds()
{
	FBSession^ s = FBSession::ActiveSession;

	// Assumes the Facebook App ID and Windows Phone Store ID have been saved
	// in the default resource file.
	ResourceLoader^ rl = ResourceLoader::GetForCurrentView();

	String^ appId = rl->GetString(FBAppIDName);
	String^ winAppId = rl->GetString(FBStoreAppIDName);

	// IDs are both sent to FB app, so it can validate us.
	s->FBAppId = appId;
	s->WinAppId = winAppId;
}

String^ MainPage::BuildPermissionsString(
	)
{
	String^ result = ref new String();

	for (unsigned int i = 0; i < ARRAYSIZE(requested_permissions); i++)
	{
		if (i)
		{
			result += L",";
		}

		result += ref new String(requested_permissions[i]);
	}

	return result;
}

BOOL MainPage::DidGetAllRequestedPermissions(
	)
{
	BOOL success = FALSE;
	FBAccessTokenData^ data = FBSession::ActiveSession->AccessTokenData;
	unsigned int grantedCount = 0;

	if (data)
	{
		for (unsigned int i = 0; i < ARRAYSIZE(requested_permissions); i++)
		{
			String^ perm = ref new String(requested_permissions[i]);
			if (data->Permissions && (data->Permissions->HasKey(perm)))
			{
				String^ Value = data->Permissions->Lookup(perm);
				if (!String::CompareOrdinal(Value, PermissionGranted))
				{
					grantedCount++;
				}
			}
		}

		if (grantedCount == ARRAYSIZE(requested_permissions))
		{
			success = TRUE;
		}
	}

	return success;
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

task<FBResult^> MainPage::LoginViaRerequest(
	PropertySet^ Parameters
	)
{
	Parameters->Insert(L"auth_type", L"rerequest");
	return create_task(FBSession::ActiveSession->Logout())
		.then([=]()
	{
		SetSessionAppIds();

		return FBSession::ActiveSession->LoginAsync(Parameters);
	});
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
		PropertySet^ parameters = ref new PropertySet();

		parameters->Insert(L"scope", BuildPermissionsString());

		create_task(sess->LoginAsync(parameters)).then([=](FBResult^ result)
		{
			task<FBResult^> nextResult = create_task([=]()
			{
				return result;
			});

			if ((!result->Succeeded) &&
				((result->ErrorInfo->Code == 190) && (result->ErrorInfo->Subcode == 466)))
			{
				nextResult = LoginViaRerequest(parameters);
			}

			return nextResult;
		})
			.then([=](FBResult^ loginResult)
		{
			task<FBResult^> finalResult = create_task([=]()
			{
				return loginResult;
			});

			if (loginResult->Succeeded)
			{
				if (!DidGetAllRequestedPermissions())
				{
					finalResult = LoginViaRerequest(parameters);
				}
			}

			return finalResult;
		})
			.then([=](FBResult^ finalResult)
		{
			if (finalResult->Succeeded)
			{
				NavigateToOptionsPage();
			}
		});
	}
}

