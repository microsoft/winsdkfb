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
#include "APIResult.h"
#include "AppToken.h"
#include "SDKCppUnitStrings.h"
#include "TestContext.h"
#include "TestUser.h"

using namespace concurrency;
using namespace Facebook;
using namespace Facebook::Graph;
using namespace Platform;
using namespace Platform::Collections;
using namespace SDKCppUnit;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;

TestContext::TestContext()
{
    Init();
}

AppToken^ TestContext::Token(
    )
{
    return _Token;
}

Windows::Foundation::Collections::IVector<TestUser^>^ TestContext::TestUsers(
    )
{
    return _testUsers;
}

void TestContext::Init(
    )
{
    _Token = nullptr;
    _testUsers = ref new Vector<TestUser^>(0);
    _pagedResult = nullptr;
}

task<bool> TestContext::GetAppToken(
    )
{
    return create_task([this]() -> task<AppToken^>
    {
        if (!_Token)
        {
            return create_task(AppToken::Create());
        }
        else
        {
            return create_task([this]() -> AppToken^
            {
                return _Token;
            });
        }
    })
    .then([this](AppToken^ token) -> bool
    {
        _Token = token;
        return (Token() != nullptr);
    });
}

task<FBResult^> TestContext::CreateTestUser(
    PropertySet^ Parameters
    )
{
    FBSession^ s = FBSession::ActiveSession;
    String^ path = L"/" + s->FBAppId + FBSDKTestUsersPath;

    if (!Parameters)
    {
        Parameters = ref new PropertySet();
    }

    if (!Token())
    {
        throw ref new InvalidArgumentException(TestContextErrorNoToken);
    }

    Parameters->Insert(AccessTokenParameterKey, Token()->Token);
    FBSingleValue^ val = ref new FBSingleValue(path, Parameters,
        ref new FBJsonClassFactory([](String^ JsonText) -> Object^
        {
            return TestUser::FromJson(JsonText);
        }));

    return create_task(val->PostAsync());
}

task<FBResult^> TestContext::DeleteTestUser(
    TestUser^ user
    )
{
    PropertySet^ parameters = ref new PropertySet();
    FBSession^ s = FBSession::ActiveSession;
    String^ path = L"/" + user->Id;

    if (!Token())
    {
        throw ref new InvalidArgumentException(TestContextErrorNoToken);
    }

    parameters->Insert(AccessTokenParameterKey, Token()->Token);
    FBSingleValue^ val = ref new FBSingleValue(path, parameters,
        ref new FBJsonClassFactory([](String^ JsonText) -> Object^
    {
        return APIResult::FromJson(JsonText);
    }));

    return create_task(val->DeleteAsync());
}

task<bool> TestContext::PopulateTestUsersCollection(
    IVectorView<Object^>^ TestUsers
    )
{
    for (IIterator<Object^>^ it = TestUsers->First(); it->HasCurrent;
        it->MoveNext())
    {
        TestUser^ testUser = static_cast<TestUser^>(it->Current);
        if (testUser)
        {
            _testUsers->Append(testUser);
        }
    }

    if (_pagedResult->HasNext)
    {
        return create_task(_pagedResult->NextAsync()).then([this](FBResult^ result) -> task<bool>
        {
            if (result->Succeeded)
            {
                IVectorView<Object^>^ items =
                    static_cast<IVectorView<Object^>^>(result->Object);
                return PopulateTestUsersCollection(items);
            }
            else
            {
                return create_task([]()
                {
                    return false;
                });
            }
        });
    }
    else
    {
        return create_task([=]()
        {
            return true;
        });
    }
}

task<bool> TestContext::GetTestUsers(
    )
{
    return GetAppToken()
        .then([=](bool gotToken)-> task <FBResult^>
    {
        task<bool> result;

        if (!gotToken)
        {
            throw ref new InvalidArgumentException(TestAppErrorNoToken);
        }

        FBSession^ sess = FBSession::ActiveSession;

        PropertySet^ parameters = ref new PropertySet();
        String^ graphPath = sess->FBAppId + FBSDKTestUsersPath;
        parameters->Insert(AccessTokenParameterKey, Token()->Token);

        FBJsonClassFactory^ fact =
            ref new FBJsonClassFactory([](String^ JsonText) -> Object^
        {
            return TestUser::FromJson(JsonText);
        });

        _pagedResult = ref new FBPaginatedArray(graphPath, parameters, fact);
        return create_task(_pagedResult->FirstAsync());
    })
        .then([this](FBResult^ result)
    {
        if (result->Succeeded)
        {
            IVectorView<Object^>^ items = static_cast<IVectorView<Object^>^>
                (result->Object);
            return PopulateTestUsersCollection(items);
        }
        else
        {
            // TODO: Handle FB errors...
            return create_task([]()
            {
                return false;
            });
        }
    });
}
