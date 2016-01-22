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
#include <ppltasks.h>
#include "CppUnitTest.h"
//#include "AppRequest.h"

using namespace concurrency;
using namespace winsdkfb;
using namespace winsdkfb::Graph;
using namespace Platform;
using namespace Windows::Data::Json;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define FBTestRecipient                    L"3597932419087"
#define FBTestRecipient2                   L"1203984335674"
#define FBTestRequestId                    L"0124986269211"
#define FBSuccessfulAppRequestResponse     L"http://whatever?request=" + FBTestRequestId + L"&to[0]=" + FBTestRecipient
#define FBSuccessfulMultiRecipientResponse L"http://whatever?request=" +       \
    FBTestRequestId + L"&to%5B0%5D=" + FBTestRecipient + L"&to%5B1%5D=" +      \
    FBTestRecipient2

#define FBCanceledRequestResponse          L"http://whatever?error_code=4201"  \
    L"&error_message=User+canceled+the+Dialog+flow"
#define ErrorObjectJson L"{\"error\": {\"message\": \"Message describing the " \
    L"error\", \"type\": \"OAuthException\", \"code\": 190, \"error_subcode\"" \
    L":460, \"error_user_title\": \"A title\", \"error_user_msg\": "           \
    L"\"A message\"}}"
#define ErrorJsonMessage L"Message describing the error"
#define ErrorJsonType L"OAuthException"
#define ErrorJsonCode 190
#define ErrorJsonSubcode 460
#define ErrorJsonTitle L"A title"
#define ErrorJsonUserMessage L"A message"
#define FBCanceledRequestMessage           L"User canceled the Dialog flow"
#define FBTestPostId                       L"12037318494044_12045997152819" 
#define FBSuccessfulPostResponse           L"http://whatever?post_id=" +       \
    FBTestPostId
#define FBTestAccessToken                  L"CAADZCu4XMfOgBAOAZBIlSd7tU8psBsTOQq6YoAEphNkBawTi9e"
#define FBTestExpirationTime               L"5099683"
#define FBTestLoginState                   L"."
#define FBSuccesfulLoginResponse           L"http://whatever?access_token=" + \
    FBTestAccessToken + L"&expires_in=" + FBTestExpirationTime + L"&state=" + \
    FBTestLoginState;
#define FBProfilePicResponse               \
            L"{\"data\":{\"height\":100,\"is_silhouette\"" \
            L":false,\"url\":\"https:\\/\\/fbcdn-profile-a.akamaihd.net\\/" \
            L"hprofile-ak-xpf1\\/v\\/t1.0-1\\/c0.0.100.100\\/p100x100\\/40" \
            L"1525_2814484914314_1381291131_n.jpg?oh=f156b6db15f69dead8a4d" \
            L"2a67d0b8e4b&oe=5518FD17&__gda__=1423522289_944b93025baf9c0d4" \
            L"007db567f6eb77b\",\"width\":100}}";
#define FBArrayOfLikes                     \
            L"{\"data\":[{\"category\":\"Clothing\",\"name\":\"Frockasaur"  \
            L"us\",\"created_time\":\"2014-09-01T16:45:17+0000\",\"id\":\"" \
            L"438872976218316\"},{\"category\":\"Media\\/news\\/publishing\"" \
            L",\"name\":\"ClickHole\",\"created_time\":\"2014-06-10T18:32:" \
            L"23+0000\",\"id\":\"1439042583002670\"},{\"category\":\"Kitch" \
            L"en\\/cooking\",\"name\":\"Elevation Rack\",\"created_time\":"  \
            L"\"2014-05-22T22:58:09+0000\",\"id\":\"310515869105473\"},{"   \
            L"\"category\":\"Local business\",\"category_list\":[{\"id\""   \
            L":\"181216025249367\",\"name\":\"Apartment & Condo Building"   \
            L"\"}],\"name\":\"Henningzen' Place\",\"created_time\":\"2014-" \
            L"05-05T02:29:11+0000\",\"id\":\"666926900011900\"}],\"paging"  \
            L"\":{\"cursors\":{\"after\":\"MTA2MjI0NjY2MDc0NjI1\",\"before" \
            L"\":\"NDM4ODcyOTc2MjE4MzE2\"},\"next\":\"https:\\/\\/graph.face" \
            L"book.com\\/v2.1\\/10203673618494044\\/likes?access_token=CAADZC" \
            L"0iddz0tkrNiuUMKGDiLwZDZD&limit=25&after=MTA2MjI0NjY2MDc0NjI1\"}}"

namespace FBWinPhoneStoreCppTests
{
    TEST_CLASS(UnitTest1)
    {
    public:
        TEST_METHOD(ParseSuccessfulFBAppRequestOneRecipient)
        {
            Uri^ fbResponse = ref new Uri(FBSuccessfulAppRequestResponse);
            winsdkfb::FBAppRequest^ info = winsdkfb::FBAppRequest::FromRequestDialogResponse(fbResponse);
            Assert::IsNotNull(info);

            Assert::IsTrue(String::CompareOrdinal(info->RequestId, ref new String(FBTestRequestId)) == 0);

            String^ recipient = info->RecipientIds->GetAt(0);
            Assert::IsTrue(String::CompareOrdinal(recipient, ref new String(FBTestRecipient)) == 0);
        }
        TEST_METHOD(ParseSuccessfulFBAppRequestMultipleRecipients)
        {
            Uri^ fbResponse = ref new Uri(FBSuccessfulMultiRecipientResponse);
            winsdkfb::FBAppRequest^ info = winsdkfb::FBAppRequest::FromRequestDialogResponse(fbResponse);
            Assert::IsNotNull(info);

            Assert::IsTrue(String::CompareOrdinal(info->RequestId, ref new String(FBTestRequestId)) == 0);

            String^ recipient = info->RecipientIds->GetAt(0);
            Assert::IsTrue(String::CompareOrdinal(recipient, ref new String(FBTestRecipient)) == 0);
            
            recipient = info->RecipientIds->GetAt(1);
            Assert::IsTrue(String::CompareOrdinal(recipient, ref new String(FBTestRecipient2)) == 0);
        }
        TEST_METHOD(ParseCanceledFBAppRequest)
        {
            Uri^ fbResponse = ref new Uri(FBCanceledRequestResponse);
            winsdkfb::FBAppRequest^ info = winsdkfb::FBAppRequest::FromRequestDialogResponse(fbResponse);
            Assert::IsNull(info);
        }
        TEST_METHOD(ParseSuccessfulFBFeedPostResponse)
        {
            Uri^ fbResponse = ref new Uri(FBSuccessfulPostResponse);
            FBFeedRequest^ info = FBFeedRequest::FromFeedDialogResponse(fbResponse);
            Assert::IsNotNull(info);

            Assert::AreEqual(info->PostId, ref new String(FBTestPostId));
        }
        TEST_METHOD(ParseCanceledFeedRequest)
        {
            Uri^ fbResponse = ref new Uri(FBCanceledRequestResponse);
            FBFeedRequest^ info = FBFeedRequest::FromFeedDialogResponse(fbResponse);
            Assert::IsNull(info);
        }
        TEST_METHOD(ParseErrorFromJson)
        {
            FBError^ e = FBError::FromJson(ErrorObjectJson);
            Assert::IsNotNull(e);
            Assert::IsNotNull(e->Message);
            Assert::IsNotNull(e->Type);
            Assert::IsNotNull(e->ErrorUserTitle);
            Assert::IsNotNull(e->ErrorUserMessage);
            Assert::AreEqual(e->Message, ref new String(ErrorJsonMessage));
            Assert::AreEqual(e->Type, ref new String(ErrorJsonType));
            Assert::AreEqual(e->Code, ErrorJsonCode);
            Assert::AreEqual(e->Subcode, ErrorJsonSubcode);
            Assert::AreEqual(e->ErrorUserTitle, ErrorJsonTitle);
            Assert::AreEqual(e->ErrorUserMessage, ErrorJsonUserMessage);
        }
        TEST_METHOD(ParseErrorFromQueryString)
        {
            String^ fbResponse = FBCanceledRequestResponse;
            Uri^ u = ref new Uri(fbResponse);
            FBError^ err = FBError::FromUri(u);
            Assert::IsNotNull(err);
            Assert::AreEqual(err->Code, 4201);
            Assert::AreEqual(ref new String(FBCanceledRequestMessage), err->Message);
        }
        TEST_METHOD(ParseErrorFromSuccessResponse)
        {
            String^ fbResponse = FBSuccessfulPostResponse;
            Uri^ u = ref new Uri(fbResponse);
            FBError^ err = FBError::FromUri(u);
            Assert::IsNull(err);
        }
        TEST_METHOD(ParseTokenInfoFromSuccessfulLogin)
        {
            String^ fbResponse = FBSuccesfulLoginResponse;
            Uri^ u = ref new Uri(fbResponse);
            FBAccessTokenData^ tokenData = FBAccessTokenData::FromUri(u);
            Assert::IsNotNull(tokenData);
            Assert::AreEqual(ref new String(FBTestAccessToken), tokenData->AccessToken);
        }
        TEST_METHOD(ParseTokenInfoFromFailedLogin)
        {
            String^ fbResponse = FBCanceledRequestResponse;
            Uri^ u = ref new Uri(fbResponse);
            FBAccessTokenData^ tokenData = FBAccessTokenData::FromUri(u);
            Assert::IsNull(tokenData);
        }
        TEST_METHOD(ParseProfilePictureInfoFromJson)
        {
            String^ fbResponse = FBProfilePicResponse;

            // Profile picture is an FBSingleValue response, so use the FBProfilePictureData to make the similar process
            FBProfilePictureData^ data = safe_cast<FBProfilePictureData^>(FBProfilePictureData::FromJson(fbResponse));
            Assert::IsNotNull(data);
            Assert::IsNotNull(data->Data);
            Assert::IsNotNull(data->Data->Url);
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
