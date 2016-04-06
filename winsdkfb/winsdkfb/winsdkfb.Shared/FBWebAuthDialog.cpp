//******************************************************************************
//
// Copyright (c) 2016 Microsoft Corporation. All rights reserved.
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
#include "FBWebAuthDialog.h"
#include "DialogUrlBuilder.h"
#include "FacebookFeedRequest.h"
#include "FacebookAppRequest.h"
#include "FacebookSendRequest.h"

using namespace winsdkfb;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace concurrency;
using namespace Windows::Security::Authentication::Web;
using namespace Platform;

Windows::Foundation::IAsyncOperation<FBResult^>^ FBWebAuthDialog::ShowFeedDialog(PropertySet^ Parameters)
{
    Uri^ uri = DialogUrlBuilder::BuildFeedDialogUrl(Parameters);
    return ShowDialog(uri, [=](String^ ResponseData)
    {
        FBResult^ result = nullptr;
        FBFeedRequest^ request = FBFeedRequest::FromFeedDialogResponse(ref new Uri(ResponseData));
        if (request)
        {
            result = ref new FBResult(request);
        }
        else
        {
            result = ref new FBResult(FBWebAuthDialog::MalformedResponseError(ResponseData));
        }
        return result;
    });
}

Windows::Foundation::IAsyncOperation<FBResult^>^ FBWebAuthDialog::ShowRequestsDialog(PropertySet^ Parameters)
{
    Uri^ uri = DialogUrlBuilder::BuildRequestsDialogUrl(Parameters);
    return ShowDialog(uri, [=](String^ ResponseData)
    {
        FBResult^ result = nullptr;
        FBAppRequest^ request = FBAppRequest::FromRequestDialogResponse(ref new Uri(ResponseData));
        if (request)
        {
            result = ref new FBResult(request);
        }
        else
        {
            result = ref new FBResult(FBWebAuthDialog::MalformedResponseError(ResponseData));
        }
        return result;
    });
}

Windows::Foundation::IAsyncOperation<FBResult^>^ FBWebAuthDialog::ShowSendDialog(PropertySet^ Parameters)
{
    Uri^ uri = DialogUrlBuilder::BuildSendDialogUrl(Parameters);
    return ShowDialog(uri, [=](String^ ResponseData)
    {
        return ref new FBResult(ref new FBSendRequest());
    });
}

Windows::Foundation::IAsyncOperation<FBResult^>^ FBWebAuthDialog::ShowDialog(Uri^ uri, std::function<FBResult^ (String^)> SuccessResultFunc)
{
    Uri^ endUri = WebAuthenticationBroker::GetCurrentApplicationCallbackUri();
    task<FBResult^> dialogTask = create_task(
        WebAuthenticationBroker::AuthenticateAsync(WebAuthenticationOptions::None,
        uri,
        endUri)).then([=](WebAuthenticationResult^ authResult)
    {
        FBResult^ result = nullptr;
        switch (authResult->ResponseStatus)
        {
        case WebAuthenticationStatus::ErrorHttp:
            result = ref new FBResult(ref new FBError(0,
                L"WebAuthenticationBroker error",
                L"ErrorHttp"));
            break;
        case WebAuthenticationStatus::UserCancel:
            result = ref new FBResult(FBWebAuthDialog::UserCanceledError());
            break;
        case WebAuthenticationStatus::Success:
            result = SuccessResultFunc(authResult->ResponseData);
            break;
        default:
            result = ref new FBResult(ref new FBError(0,
                L"WebAuthenticationBroker error",
                L"Unhandled ResponseStatus"));
            break;
        }
        return result;
    });
    return create_async([=]()
        {
            return dialogTask.get();
        });
}

FBError^ FBWebAuthDialog::UserCanceledError()
{
    String^ errorJson = L"{\"error\": {\"message\": "
        L"\"Operation Canceled\", \"type\": "
        L"\"OAuthException\", \"code\": 4201, "
        L"\"error_user_msg\": \"User canceled the Dialog flow\""
        L"}}";
    return FBError::FromJson(errorJson);
}

FBError^ FBWebAuthDialog::MalformedResponseError(String^ ResponseText)
{
    return ref new FBError(0,
        L"Malformed Response",
        L"Cannot format response correctly. Response text is: " + ResponseText);
}