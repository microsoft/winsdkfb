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

#pragma once

#include "AppToken.h"
#include "TestUser.h"

namespace SDKCppUnit
{
    class TestContext
    {
    public:
        static TestContext* Get()
        {
            static TestContext* currentContext = new TestContext();
            return currentContext;
        }

        void Init(
            );

        AppToken^ Token();

        Windows::Foundation::Collections::IVector<TestUser^>^ TestUsers();

        concurrency::task<bool> GetAppToken(
            );

        concurrency::task<Facebook::FBResult^> CreateTestUser(
            Windows::Foundation::Collections::PropertySet^ Parameters
            );

        concurrency::task<Facebook::FBResult^> DeleteTestUser(
            TestUser^ user
            );

        concurrency::task<bool> GetTestUsers(
            );

    private:
        concurrency::task<bool> PopulateTestUsersCollection(
            Windows::Foundation::Collections::IVectorView<Platform::Object^>^ TestUsers
            );

        TestContext();
        AppToken^ _Token;
        Platform::Collections::Vector<TestUser^>^ _testUsers;
        Facebook::Graph::FBPaginatedArray^ _pagedResult;
    };
}
