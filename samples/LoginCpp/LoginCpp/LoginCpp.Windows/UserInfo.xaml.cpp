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
// UserInfo.xaml.cpp
// Implementation of the UserInfo class
//

#include "pch.h"
#include "UserInfo.xaml.h"
#include "UserLikes.xaml.h"

using namespace LoginCpp;

using namespace winsdkfb;
using namespace winsdkfb::Graph;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Display;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The Basic Page item template is documented at http://go.microsoft.com/fwlink/?LinkID=390556

UserInfo::UserInfo()
{
	InitializeComponent();
	SetValue(_defaultViewModelProperty, ref new Platform::Collections::Map<String^, Object^>(std::less<String^>()));
	auto navigationHelper = ref new Common::NavigationHelper(this);
	SetValue(_navigationHelperProperty, navigationHelper);
	navigationHelper->LoadState += ref new Common::LoadStateEventHandler(this, &UserInfo::LoadState);
	navigationHelper->SaveState += ref new Common::SaveStateEventHandler(this, &UserInfo::SaveState);
}

DependencyProperty^ UserInfo::_defaultViewModelProperty =
DependencyProperty::Register("DefaultViewModel",
TypeName(IObservableMap<String^, Object^>::typeid), TypeName(UserInfo::typeid), nullptr);

/// <summary>
/// Used as a trivial view model.
/// </summary>
IObservableMap<String^, Object^>^ UserInfo::DefaultViewModel::get()
{
	return safe_cast<IObservableMap<String^, Object^>^>(GetValue(_defaultViewModelProperty));
}

/// <summary>
/// Gets an implementation of <see cref="NavigationHelper"/> designed to be
/// used as a trivial view model.
/// </summary>
Common::NavigationHelper^ UserInfo::NavigationHelper::get()
{
    return safe_cast<Common::NavigationHelper^>(GetValue(_navigationHelperProperty));
}

#pragma region Navigation support

DependencyProperty^ UserInfo::_navigationHelperProperty =
DependencyProperty::Register("NavigationHelper",
TypeName(Common::NavigationHelper::typeid), TypeName(UserInfo::typeid), nullptr);

/// The methods provided in this section are simply used to allow
/// NavigationHelper to respond to the page's navigation methods.
/// 
/// Page specific logic should be placed in event handlers for the  
/// <see cref="NavigationHelper::LoadState"/>
/// and <see cref="NavigationHelper::SaveState"/>.
/// The navigation parameter is available in the LoadState method 
/// in addition to page state preserved during an earlier session.

void UserInfo::OnNavigatedTo(NavigationEventArgs^ e)
{
	NavigationHelper->OnNavigatedTo(e);
    FBSession^ sess = FBSession::ActiveSession;
    if (sess->LoggedIn)
    {
        FBUser^ user = sess->User;
        if (user)
        {
            UserId->Text = user->Id;
            UserFirstName->Text = user->FirstName;
            UserGender->Text = user->Gender;
            UserLastName->Text = user->LastName;
            UserLink->Text = user->Link;
            UserLocale->Text = user->Locale;
            UserName->Text = user->Name;
            UserTimezone->Text = user->Timezone.ToString();
            UserUpdatedTime->Text = user->UpdatedTime;
            UserVerified->Text = user->Verified.ToString();
            SquarePicture->UserId = user->Id;
        }
    }
}

void UserInfo::OnNavigatedFrom(NavigationEventArgs^ e)
{
	NavigationHelper->OnNavigatedFrom(e);
}

#pragma endregion

/// <summary>
/// Populates the page with content passed during navigation. Any saved state is also
/// provided when recreating a page from a prior session.
/// </summary>
/// <param name="sender">
/// The source of the event; typically <see cref="NavigationHelper"/>
/// </param>
/// <param name="e">Event data that provides both the navigation parameter passed to
/// <see cref="Frame::Navigate(Type, Object)"/> when this page was initially requested and
/// a dictionary of state preserved by this page during an earlier
/// session. The state will be null the first time a page is visited.</param>
void UserInfo::LoadState(Object^ sender, Common::LoadStateEventArgs^ e)
{
    (void)sender;	// Unused parameter
    (void)e;	// Unused parameter
}

/// <summary>
/// Preserves state associated with this page in case the application is suspended or the
/// page is discarded from the navigation cache.  Values must conform to the serialization
/// requirements of <see cref="SuspensionManager::SessionState"/>.
/// </summary>
/// <param name="sender">The source of the event; typically <see cref="NavigationHelper"/></param>
/// <param name="e">Event data that provides an empty dictionary to be populated with
/// serializable state.</param>
void UserInfo::SaveState(Object^ sender, Common::SaveStateEventArgs^ e){
    (void)sender;	// Unused parameter
    (void)e; // Unused parameter
}

void LoginCpp::UserInfo::UserLikesButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    LoginCpp::App^ a = dynamic_cast<LoginCpp::App^>(Application::Current);
    Windows::UI::Xaml::Controls::Frame^ f = a->CreateRootFrame();
    f->Navigate(UserLikes::typeid);
}



void LoginCpp::UserInfo::BackButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    if (Frame->CanGoBack)
    {
        Frame->GoBack();
    }
}
