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

#include "APIResult.h"
#include "AppToken.h"
#include "MainPage.xaml.h"
#include "TestContext.h"

using namespace SDKCppUnit;

using namespace concurrency;
using namespace Platform;
using namespace Platform::Collections;
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
using namespace winsdkfb;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

#define FBAppIDName L"FBApplicationId"
#define FBStoreAppIDName L"FBWindowsAppId"
#define ItemsName L"Items"
#define TestsPassedName L"TestsPassed"
#define TestsFailedName L"TestsFailed"

MainPage::MainPage()
{
	InitializeComponent();

    // Assumes the Facebook App ID and Windows Phone Store ID have been saved
    // in the default resource file.
    ResourceLoader^ rl = ResourceLoader::GetForCurrentView();
    if (rl)
    {
        FBSession^ s = FBSession::ActiveSession;

        String^ appId = rl->GetString(FBAppIDName);
        String^ winAppId = rl->GetString(FBStoreAppIDName);


        // IDs are both sent to FB app, so it can validate us.
        s->FBAppId = appId;
        s->WinAppId = winAppId;
    }

    // Don't forget this!  Nothing in your UI will be bound, and you will be
    // left to wonder why.
    DataContext = this;

    _testRunner = ref new TestRunner();

    _testRunner->TestSuiteStarted += 
        ref new TestSuiteStartedHandler(this, 
            &SDKCppUnit::MainPage::OnTestSuiteStarted);
    _testRunner->TestSuiteCompleted +=
        ref new TestSuiteCompletedHandler(this,
        &SDKCppUnit::MainPage::OnTestSuiteCompleted);
    _testRunner->TestCaseStarted +=
        ref new TestCaseStartedHandler(this,
        &SDKCppUnit::MainPage::OnTestCaseStarted);
    _testRunner->TestCaseCompleted +=
        ref new TestCaseCompletedHandler(this,
        &SDKCppUnit::MainPage::OnTestCaseCompleted);

    _testsPassed = 0;
    _testsFailed = 0;
    _items = ref new Vector<String^>(0);
    NotifyPropertyChanged(TestsPassedName);
    NotifyPropertyChanged(TestsFailedName);
}

IVector<String^>^ MainPage::Items::get()
{
    return _items;
}

void MainPage::OnTestSuiteStarted(
    String^ SuiteName
    )
{
    String^ msg = L"TestSuiteStarted: " + SuiteName;

    _items->Append(msg);

    OKButton->IsEnabled = false;

    NotifyPropertyChanged(ItemsName);
}

void MainPage::OnTestSuiteCompleted(
    String^ SuiteName
    )
{
    AddItem(L"TestSuiteCompleted: " + SuiteName);

    OKButton->IsEnabled = true;
}

void MainPage::OnTestCaseStarted(
    String^ TestCaseName
    )
{
    AddItem(L"TestCaseStarted: " + TestCaseName);
}

void MainPage::OnTestCaseCompleted(
    String^ TestCaseName,
    bool TestSucceeded
    )
{
    String^ resultString = nullptr;

    if (TestSucceeded)
    {
        _testsPassed++;
        NotifyPropertyChanged(TestsPassedName);
    }
    else
    {
        _testsFailed++;
        NotifyPropertyChanged(TestsFailedName);
    }

    if (TestSucceeded)
    {
        resultString = L"SUCCEEDED";
    }
    else
    {
        resultString = L"FAILED";
    }

    AddItem("TestCaseCompleted: " + TestCaseName + L", result: " + 
        resultString);
}

int MainPage::TestsPassed::get(
    )
{
    return _testsPassed;
}

int MainPage::TestsFailed::get(
    )
{
    return _testsFailed;
}

void MainPage::OK_Click(Object^ sender, RoutedEventArgs^ e)
{
    _items->Clear();
    _testRunner->RunAllTests();

    /*
    create_task(ctx->GetAppToken(), task_continuation_context::use_current())
    .then([this, ctx](bool result) -> IAsyncOperation<FBResult^>^
    {
        if (!result)
        {
            throw ref new InvalidArgumentException(TestAppErrorNoToken);
        }

        return ctx->CreateTestUser();
    }, task_options(task_continuation_context::use_current()))
    .then([this, ctx](FBResult^ result) -> TestUser^
    {
        TestUser^ user = nullptr;

        if (result->Succeeded)
        {
            user = static_cast<TestUser^>(result->Object);
        }
        
        OKButton->IsEnabled = true;

        return user;
    }, task_options(task_continuation_context::use_current()))
    .then([this, ctx](TestUser^ user)
    {
        IAsyncOperation<FBResult^>^ result = nullptr;
        if (user)
        {
            result = ctx->DeleteTestUser(user);
        }
        else
        {
            result = nullptr;
        }
        return result;
    }, task_options(task_continuation_context::use_current()))
    .then([this, ctx](FBResult^ result)
    {
        if (result && result->Succeeded)
        {
            APIResult^ apiCall = static_cast<APIResult^>(result->Object);
        }
    });
    */
}

void MainPage::NotifyPropertyChanged(
    String^ prop
    )
{
    CoreApplication::MainView->CoreWindow->Dispatcher->RunAsync(
        Windows::UI::Core::CoreDispatcherPriority::Normal,
        ref new Windows::UI::Core::DispatchedHandler([this, prop]()
    {
        PropertyChangedEventArgs^ args = ref new PropertyChangedEventArgs(prop);
        PropertyChanged(this, args);
    }));
}

void MainPage::AddItem(
    String^ Item
    )
{
    _items->Append(Item);

    NotifyPropertyChanged(ItemsName);
}
