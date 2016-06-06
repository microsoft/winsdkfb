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
#include "DialogUrlBuilder.h"
#include "FacebookClient.h"

using namespace winsdkfb;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Security::Authentication::Web;
using namespace Platform;

Uri^ DialogUrlBuilder::BuildFeedDialogUrl(
    PropertySet^ Parameters
    )
{
    FBSession^ sess = FBSession::ActiveSession;
    String^ apiVersion = L"";
    if (sess->APIMajorVersion)
    {
        apiVersion = L"/v" + sess->APIMajorVersion.ToString() + L"." + sess->APIMinorVersion.ToString() + L"/";
    }
    String^ dialogUriString =
		FacebookDialog::GetFBServerUrl() + apiVersion + L"dialog/feed?access_token=" +
        sess->AccessTokenData->AccessToken +
        L"&redirect_uri=" + GetRedirectUriString(L"feed") +
        L"&display=popup" +
        L"&app_id=" + sess->FBAppId;
    String^ queryString = FBClient::ParametersToQueryString(Parameters);
    if (queryString->Length() > 0)
    {
        dialogUriString += "&" + queryString;
    }

    return ref new Uri(dialogUriString);
}

Uri^ DialogUrlBuilder::BuildRequestsDialogUrl(
    PropertySet^ Parameters
    )
{
    FBSession^ sess = FBSession::ActiveSession;
    String^ apiVersion = L"";
    if (sess->APIMajorVersion)
    {
        apiVersion = L"/v" + sess->APIMajorVersion.ToString() + L"." + sess->APIMinorVersion.ToString() + L"/";
    }
    String^ dialogUriString =
		FacebookDialog::GetFBServerUrl() + apiVersion + L"dialog/apprequests?access_token=" +
        sess->AccessTokenData->AccessToken +
        L"&redirect_uri=" + GetRedirectUriString(L"requests") +
        L"&display=popup" +
        L"&app_id=" + sess->FBAppId;
    String^ queryString = FBClient::ParametersToQueryString(Parameters);
    if (queryString->Length() > 0)
    {
        dialogUriString += "&" + queryString;
    }

    return ref new Uri(dialogUriString);
}

Uri^ DialogUrlBuilder::BuildSendDialogUrl(
    PropertySet^ Parameters
    )
{
    FBSession^ sess = FBSession::ActiveSession;
    String^ apiVersion = L"";
    if (sess->APIMajorVersion)
    {
        apiVersion = L"/v" + sess->APIMajorVersion.ToString() + L"." + sess->APIMinorVersion.ToString() + L"/";
    }
    String^ dialogUriString =
        FacebookDialog::GetFBServerUrl() + apiVersion + L"dialog/send?access_token=" +
        sess->AccessTokenData->AccessToken +
        L"&redirect_uri=" + GetRedirectUriString(L"send") +
        L"&display=popup" +
        L"&app_id=" + sess->FBAppId;
    String^ queryString = FBClient::ParametersToQueryString(Parameters);
    if (queryString->Length() > 0)
    {
        dialogUriString += "&" + queryString;
    }

    return ref new Uri(dialogUriString);
}

String^ DialogUrlBuilder::GetRedirectUriString(
    String^ FacebookDialogName
    )
{
    FBSession^ sess = FBSession::ActiveSession;
    String^ result;
    if (sess->LastSuccessfulDialogBasedLoginType == SessionLoginBehavior::WebAuth)
    {
        if (sess->WebAuthDialogRedirectUrl == nullptr)
        {
            result = WebAuthenticationBroker::GetCurrentApplicationCallbackUri()->DisplayUri;
        }
        else
        {
            result = sess->WebAuthDialogRedirectUrl;
        }
    }
    else
    {
        result = sess->WebViewRedirectDomain + sess->WebViewRedirectPath;
        result = Uri::EscapeComponent(result); // TODO should this be done for all possible return values?
    }
    OutputDebugString(String::Concat(String::Concat(L"Redirect URI is ", result), L"\n")->Data());
    return result;
}
