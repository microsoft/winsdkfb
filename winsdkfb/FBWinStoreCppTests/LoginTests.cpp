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
#include "CppUnitTest.h"
#include <Windows.h>

using namespace Platform;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Windows::ApplicationModel::Store;
using namespace Windows::Foundation::Collections;
using namespace winsdkfb;
using namespace winsdkfb::Graph;

#define FBArrayOfLikes                                                        \
            L"{\"data\":[{\"category\":\"Clothing\",\"name\":\"Frockasaur"    \
            L"us\",\"created_time\":\"2014-09-01T16:45:17+0000\",\"id\":\""   \
            L"438872976218316\"},{\"category\":\"Media\\/news\\/publishing\"" \
            L",\"name\":\"ClickHole\",\"created_time\":\"2014-06-10T18:32:"   \
            L"23+0000\",\"id\":\"1439042583002670\"},{\"category\":\"Kitch"   \
            L"en\\/cooking\",\"name\":\"Elevation Rack\",\"created_time\":"   \
            L"\"2014-05-22T22:58:09+0000\",\"id\":\"310515869105473\"},{"     \
            L"\"category\":\"Local business\",\"category_list\":[{\"id\""     \
            L":\"181216025249367\",\"name\":\"Apartment & Condo Building"     \
            L"\"}],\"name\":\"Henningzen' Place\",\"created_time\":\"2014-"   \
            L"05-05T02:29:11+0000\",\"id\":\"666926900011900\"}],\"paging"    \
            L"\":{\"cursors\":{\"after\":\"MTA2MjI0NjY2MDc0NjI1\",\"before"   \
            L"\":\"NDM4ODcyOTc2MjE4MzE2\"},\"next\":\"https:\\/\\/graph.face" \
            L"book.com\\/v2.1\\/10203673618494044\\/likes?access_token=CAADZ" \
            L"0iddz0tkrNiuUMKGDiLwZDZD&limit=25&after=MTA2MjI0NjY2MDc0NjI1\"}}"

namespace FBWinStoreCppTests
{
    TEST_CLASS(LoginTests)
    {
    public:
        TEST_METHOD(LoginViaAppWithUI)
        {
            wchar_t buf[1024];
            Guid gID = CurrentApp::AppId;
            _snwprintf_s(buf, 1024, _TRUNCATE, L"AppId is %s\n", gID.ToString()->Data());
            OutputDebugString(buf);
        }
        TEST_METHOD(CallSessionStartOpenViaApp)
        {
            FBSession^ sess = FBSession::ActiveSession;
            sess->FBAppId = "0123456789";
            sess->WinAppId = "";
        }
        TEST_METHOD(ParseArrayOfLikes)
        {
            String^ fbResponse = FBArrayOfLikes;
            FBPaginatedArray^ arr = ref new FBPaginatedArray(
                L"Unused",
                nullptr,
                ref new FBJsonClassFactory([](String^ JsonText) -> Object^
            {
                return FBPage::FromJson(JsonText);
            }));

            IVectorView<Object^>^ result = arr->ObjectArrayFromWebResponse(
                fbResponse, 
                ref new FBJsonClassFactory([](String^ JsonText) -> Object^
            {
                return FBPage::FromJson(JsonText);
            }));
        }
    };
}
