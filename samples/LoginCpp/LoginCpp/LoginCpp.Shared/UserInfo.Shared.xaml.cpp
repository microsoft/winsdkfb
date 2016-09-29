
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
            // Don't set the UserId, let DataBinding take care of it
            // UserId->Text = user->Id;
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

void LoginCpp::UserInfo::UserLikesButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    LoginCpp::App^ a = dynamic_cast<LoginCpp::App^>(Application::Current);
    Windows::UI::Xaml::Controls::Frame^ f = a->CreateRootFrame();
    f->Navigate(UserLikes::typeid);
}
