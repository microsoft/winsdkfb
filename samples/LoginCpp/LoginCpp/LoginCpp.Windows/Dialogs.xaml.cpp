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
// Dialogs.xaml.cpp
// Implementation of the Dialogs class
//

#include "pch.h"
#include "Dialogs.xaml.h"

using namespace LoginCpp;

using namespace concurrency;
using namespace winsdkfb;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

Dialogs::Dialogs()
{
    InitializeComponent();
}


void LoginCpp::Dialogs::FeedDialogButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    FBSession^ s = FBSession::ActiveSession;

    if (!s->LoggedIn)
    {
        OutputDebugString(L"The user is no longer logged in.\n");
    }
    else
    {
        PropertySet^ params = ref new PropertySet();
        params->Insert(L"caption", L"I love Brussels Sprouts!");
        params->Insert(L"link", L"https://en.wikipedia.org/wiki/Brussels_sprout");
        params->Insert(L"description", L"Om Nom Nom!");

        create_task(s->ShowFeedDialogAsync(params))
            .then([=](FBResult^ Response)
        {
            OutputDebugString(L"Showed 'Feed' dialog.\n");
        });
    }
}


void LoginCpp::Dialogs::AppRequestsButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    FBSession^ s = FBSession::ActiveSession;

    if (!s->LoggedIn)
    {
        OutputDebugString(L"The user is no longer logged in.\n");
    }
    else
    {
        PropertySet^ params = ref new PropertySet();

        params->Insert(L"title", L"I love Brussels Sprouts!");
        params->Insert(L"message", L"Om Nom Nom!");

        create_task(s->ShowRequestsDialogAsync(params))
            .then([=](FBResult^ Response)
        {
            OutputDebugString(L"Showed 'Requests' dialog.\n");
        });
    }
}
