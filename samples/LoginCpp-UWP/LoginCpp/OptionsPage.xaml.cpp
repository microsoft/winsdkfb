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
// OptionsPage.xaml.cpp
// Implementation of the OptionsPage class
//

#include "pch.h"
#include "OptionsPage.xaml.h"
#include "UserInfo.xaml.h"
#include "Dialogs.xaml.h"

using namespace LoginCpp;

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

OptionsPage::OptionsPage()
{
	InitializeComponent();
}

void OptionsPage::userInfo_OnClicked(
    Platform::Object^ sender, 
    Windows::UI::Xaml::RoutedEventArgs^ e)
{
    LoginCpp::App^ a = dynamic_cast<LoginCpp::App^>(Application::Current);
    Windows::UI::Xaml::Controls::Frame^ f = a->CreateRootFrame();
    f->Navigate(UserInfo::typeid);
}

void OptionsPage::dialogs_OnClicked(
    Platform::Object^ sender, 
    Windows::UI::Xaml::RoutedEventArgs^ e
    )
{
    LoginCpp::App^ a = dynamic_cast<LoginCpp::App^>(Application::Current);
    Windows::UI::Xaml::Controls::Frame^ f = a->CreateRootFrame();
    f->Navigate(Dialogs::typeid);
}
