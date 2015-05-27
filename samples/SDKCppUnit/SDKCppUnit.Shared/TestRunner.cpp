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
// TestRunner.cpp
// Implementation of the TestRunner class.
//

#include "pch.h"

#include "SDKCppUnitStrings.h"
#include "TestContext.h"
#include "TestPhoto.h"
#include "TestRunner.h"
#include <vector>

using namespace SDKCppUnit;

using namespace concurrency;
using namespace Facebook;
using namespace Facebook::Graph;
using namespace Platform; 
using namespace Platform::Collections;
using namespace std;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::Web::Http;
using namespace Windows::Web::Http::Filters;

const int FBDeleteOpenGraphUserErrorCode = 2904;

TestRunner::TestRunner()
{
}

void TestRunner::RunAllTests()
{
    NotifyTestSuiteStarted(GraphAPISuite);

    NotifyTestCaseStarted(CreateAppTokenTest);

    create_task(TestCreateAppToken())
        .then([=](bool TestResult)
    {
        NotifyTestCaseCompleted(CreateAppTokenTest, TestResult);
    })
    .then([=]() -> task<bool>
    {
        NotifyTestCaseStarted(CreateTestUserTest);
        return TestCreateTestUser();
    })
    .then([=](bool TestResult)
    {
        NotifyTestCaseCompleted(CreateTestUserTest, TestResult);
    })
    .then([=]() -> task < bool >
    {
        NotifyTestCaseStarted(DeleteAllTestUsersTest);
        return TestDeleteAllTestUsers();
    })
    .then([=](bool TestResult)
    {
        NotifyTestCaseCompleted(DeleteAllTestUsersTest, TestResult);
    })
    .then([=]() -> task < bool >
    {
        NotifyTestCaseStarted(UploadPhotoTest);
        return TestUploadPhoto();
    })
    .then([=](bool TestResult)
    {
        NotifyTestCaseCompleted(UploadPhotoTest, TestResult);

        NotifyTestSuiteCompleted(GraphAPISuite);
    });
}

task<bool> TestRunner::TestCreateAppToken(
    )
{
    TestContext* ctx = TestContext::Get();

    return ctx->GetAppToken();
}

task<bool> TestRunner::TestCreateTestUser(
    )
{
    TestContext* ctx = TestContext::Get();

    return ctx->GetAppToken()
    .then([this, ctx](bool result) -> task<FBResult^>
    {
        if (!result)
        {
            throw ref new InvalidArgumentException(TestAppErrorNoToken);
        }

        return ctx->CreateTestUser(nullptr);
    })
    .then([=](FBResult^ result) -> bool
    {
        return (result && result->Succeeded);
    });
}

task<bool> TestRunner::TestDeleteAllTestUsers(
    )
{
    TestContext* ctx = TestContext::Get();

    return ctx->GetAppToken()
    .then([=](bool gotToken) -> task<FBResult^>
    {
        if (!gotToken)
        {
            throw ref new InvalidArgumentException(TestAppErrorNoToken);
        }

        return ctx->CreateTestUser(nullptr);
    })
    .then([=](FBResult^ result) -> task<bool>
    {
        if (result->Succeeded)
        {
            return ctx->GetTestUsers();
        }
        else
        {
            return create_task([]()
            {
                return false;
            });
        }
    })
    .then([=](bool gotTestUsers) -> vector<task<FBResult^>>
    {
        vector<task<FBResult^>> deleteResults;
        if (gotTestUsers)
        {
            IVector<TestUser^>^ users = ctx->TestUsers();
            for (unsigned int i = 0; i < users->Size; i++)
            {
                deleteResults.push_back(ctx->DeleteTestUser(users->GetAt(i)));
            }
        }

        return deleteResults;
    })
    .then([=](vector<task<FBResult^>> results) -> bool
    {
        bool success = true;

        if (results.size() == 0)
        {
            success = false;
        }

        for (unsigned int i = 0; i < results.size(); i++)
        {
            FBResult^ deleteResult = results.at(i).get();
            if (!deleteResult->Succeeded)
            {
                if (deleteResult->ErrorInfo->Code !=
                    FBDeleteOpenGraphUserErrorCode)
                {
                    success = false;
                }
            }
        }

        return success;
    });
}

IAsyncOperation<IRandomAccessStreamWithContentType^>^ 
TestRunner::GetStreamOnFile(
    String^ path
    )
{
    return create_async([=]()
    {
        StorageFolder^ appFolder =
            Windows::ApplicationModel::Package::Current->InstalledLocation;

        return create_task(appFolder->GetFileAsync(FBTestImagePath))
        .then([=](StorageFile^ file) -> IAsyncOperation<IRandomAccessStreamWithContentType^>^
        {
            if (file)
            {
                return file->OpenReadAsync();
            }
            else
            {
                return create_async([]()
                {
                    return (IRandomAccessStreamWithContentType^)nullptr;
                });
            }
        });
    });
}

IAsyncOperation<FBResult^>^ TestRunner::UploadPhotoFromStream(
    String^ Path,
    IRandomAccessStreamWithContentType^ Stream,
    PropertySet^ Parameters
    )
{
    if (Stream)
    {
        FBMediaStream^ streamWrapper = ref new FBMediaStream(FBTestImageName,
            Stream);
        Parameters->Insert(SourceParameterKey, streamWrapper);

        FBSingleValue^ sval = ref new FBSingleValue(Path, Parameters,
            ref new FBJsonClassFactory(TestPhoto::FromJson));
        return sval->Post();
    }
    else
    {
        return create_async([]()
        {
            return (FBResult^)nullptr;
        });
    }
}

IAsyncOperation<FBResult^>^ TestRunner::GetExtendedPhotoInfoFor(
    FBResult^ Result,
    PropertySet^ Parameters
    )
{
    IAsyncOperation<FBResult^>^ op = nullptr;

    if (Result->Succeeded)
    {
        TestPhoto^ photo = static_cast<TestPhoto^>(Result->Object);
        String^ path = L"/" + photo->Id;

        FBSingleValue^ sval = ref new FBSingleValue(path, Parameters,
            ref new FBJsonClassFactory(TestPhoto::FromJson));
        op = sval->Get();
    }

    return op;
}

task<bool> TestRunner::TestUploadPhoto(
    )
{
    TestContext* ctx = TestContext::Get();
    PropertySet^ parameters = ref new PropertySet();
    FBSession^ sess = FBSession::ActiveSession;
    String^ graphPath = sess->FBAppId + FBSDKTestUsersPath;

    return ctx->GetAppToken()
    .then([=](bool gotToken) -> task<FBResult^>
    {
        if (!gotToken)
        {
            throw ref new InvalidArgumentException(TestAppErrorNoToken);
        }

        parameters->Insert(L"permissions", 
            L"public_profile,publish_actions,user_photos");

        return ctx->CreateTestUser(parameters);
    })
        .then([=](FBResult^ result) -> IAsyncOperation<IRandomAccessStreamWithContentType^>^
    {
        IAsyncOperation<IRandomAccessStreamWithContentType^>^ op = nullptr;

        parameters->Clear();
        if (result->Succeeded)
        {
            TestUser^ user = static_cast<TestUser^>(result->Object);
            String^ path = "/" + user->Id + "/photos";
            parameters->Insert(AccessTokenParameterKey, user->AccessToken);
            parameters->Insert(PathParameterKey, path);
            StorageFolder^ appFolder =
                Windows::ApplicationModel::Package::Current->InstalledLocation;
            op = GetStreamOnFile(FBTestImagePath);
        }

        return op;
    })
    .then([=](IRandomAccessStreamWithContentType^ stream) -> IAsyncOperation<FBResult^>^
    {
        String^ path =
            static_cast<String^>(parameters->Lookup(PathParameterKey));
        parameters->Remove(PathParameterKey);

        return UploadPhotoFromStream(path, stream, parameters);
    })
    .then([=](FBResult^ result) -> IAsyncOperation<FBResult^>^
    {
        return GetExtendedPhotoInfoFor(result, parameters);
    })
    .then([=](FBResult^ result) -> bool
    {
        return result->Succeeded;
    });
}

void TestRunner::NotifyTestSuiteStarted(
    String^ SuiteName
    )
{
    CoreApplication::MainView->CoreWindow->Dispatcher->RunAsync(
        Windows::UI::Core::CoreDispatcherPriority::Normal,
        ref new Windows::UI::Core::DispatchedHandler([this, SuiteName]()
    {
        TestSuiteStarted(SuiteName);
    }));
}

void TestRunner::NotifyTestSuiteCompleted(
    Platform::String^ SuiteName
    )
{
    CoreApplication::MainView->CoreWindow->Dispatcher->RunAsync(
        Windows::UI::Core::CoreDispatcherPriority::Normal,
        ref new Windows::UI::Core::DispatchedHandler([this, SuiteName]()
    {
        TestSuiteCompleted(SuiteName);
    }));
}

void TestRunner::NotifyTestCaseStarted(
    Platform::String^ CaseName
    )
{
    CoreApplication::MainView->CoreWindow->Dispatcher->RunAsync(
        Windows::UI::Core::CoreDispatcherPriority::Normal,
        ref new Windows::UI::Core::DispatchedHandler([this, CaseName]()
    {
        TestCaseStarted(CaseName);
    }));
}

void TestRunner::NotifyTestCaseCompleted(
    Platform::String^ CaseName,
    bool TestResult
    )
{
    CoreApplication::MainView->CoreWindow->Dispatcher->RunAsync(
        Windows::UI::Core::CoreDispatcherPriority::Normal,
        ref new Windows::UI::Core::DispatchedHandler(
    [this, CaseName, TestResult]()
    {
        TestCaseCompleted(CaseName, TestResult);
    }));
}
