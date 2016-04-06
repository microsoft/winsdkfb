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
    if (sess->LoginMethod == SessionLoginBehavior::WebAuth)
    {
        if (sess->WebAuthDialogRedirectUrl == nullptr)
        {
            return WebAuthenticationBroker::GetCurrentApplicationCallbackUri()->DisplayUri;
        }
        else
        {
            return sess->WebAuthDialogRedirectUrl;
        }
    }

    //
    // This looks strange, but is correct.  One side or the other of this 
    // conversation has a problem with all the other types of redirect
    // protocol/URIs accepted for apps, so we're left with always redirecting
    // to the login_success page on FB, then canceling the redirect in our
    // NavigationStarted event handler, for all dialogs.
    // 
    String^ result = FacebookDialog::GetFBServerUrl() + FACEBOOK_LOGIN_SUCCESS_PATH;
        
    result = Uri::EscapeComponent(result);

    //DebugPrintLine(L"Redirect URI is " + result);
    return result;
}
