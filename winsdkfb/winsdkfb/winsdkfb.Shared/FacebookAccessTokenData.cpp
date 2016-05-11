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
#include <Intsafe.h>
#include "FacebookSession.h"
#include "FacebookAccessTokenData.h"
#include "FacebookPaginatedArray.h"
#include "FBPermission.h"

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Globalization;
using namespace Windows::Globalization::DateTimeFormatting;
using namespace winsdkfb;
using namespace winsdkfb::Graph;
using namespace concurrency;
using namespace std;

FBAccessTokenData::FBAccessTokenData(
    String^ AccessToken,
    String^ Expiration,
    String^ State
    ) :
    _accessToken(AccessToken),
    _appId(nullptr),
    _grantedPermissions(nullptr),
    _declinedPermissions(nullptr),
    _userId(nullptr)
{
    if (Expiration)
    {
        CalculateExpirationDateTime(Expiration);
    }
#ifdef _DEBUG
    DebugPrintExpirationTime();
#endif
}

FBAccessTokenData::FBAccessTokenData(
    String^ AccessToken,
    DateTime Expiration,
    String^ State
    ) :
    _accessToken(AccessToken),
    _appId(nullptr),
    _grantedPermissions(nullptr),
    _declinedPermissions(nullptr),
    _userId(nullptr),
    _expirationDate(Expiration)
{
#ifdef _DEBUG
    DebugPrintExpirationTime();
#endif
    InitPermissions();
}

String^ FBAccessTokenData::AccessToken::get()
{
    return _accessToken;
}

String^ FBAccessTokenData::AppID::get()
{
    return _appId;
}

DateTime FBAccessTokenData::ExpirationDate::get()
{
    return _expirationDate;
}

FBPermissions^ FBAccessTokenData::GrantedPermissions::get()
{
    return _grantedPermissions;
}

FBPermissions^ FBAccessTokenData::DeclinedPermissions::get()
{
    return _declinedPermissions;
}

String^ FBAccessTokenData::UserID::get()
{
    return _userId;
}


void FBAccessTokenData::InitPermissions()
{
    Vector<String^>^ v = ref new Vector<String^>(0);

    _grantedPermissions  = ref new FBPermissions(v->GetView());
    _declinedPermissions = ref new FBPermissions(v->GetView());
}

WwwFormUrlDecoder^ FBAccessTokenData::ParametersFromResponse(
    Uri^ Response
    )
{
    WwwFormUrlDecoder^ parameters = Response->QueryParsed;
    if (!parameters->Size)
    {
        // Facebook sometimes returns access token, etc., in the Uri fragment
        // rather than the query string.  WinRT only lets you parse a query
        // string from a full Uri, so we'll mock one up with the fragment from
        // the original response as the query string, then parse that.
        // 
        // Note that the Uri::Fragment property includes the leading '#'
        // character, inconveniently, so we have to strip this character or
        // we'll just end up with a Uri with the same fragment and an empty
        // query.
        wstring frag = Response->Fragment->Data();

        String^ responseString = "file:///dev/null?" + ref new String(
            frag.substr(1).c_str());
        OutputDebugString(responseString->Data());
        OutputDebugString(L"\n");

        Uri^ newResponse = ref new Uri(responseString);

        parameters = newResponse->QueryParsed;
    }

    return parameters;
}

FBAccessTokenData^ FBAccessTokenData::FromUri(
    Uri^ Response
    )
{
    bool gotToken = false;
    bool gotExpiration = false;
    bool gotState = false;
    bool gotBadField = false;
    String^ token;
    String^ expiration = nullptr;
    String^ state = nullptr;
    FBAccessTokenData^ data = nullptr;

    WwwFormUrlDecoder^ decoder = FBAccessTokenData::ParametersFromResponse(
        Response);

    for (unsigned int i = 0; i < decoder->Size; i++)
    {
        IWwwFormUrlDecoderEntry^ entry = decoder->GetAt(i);

        if (entry->Name->Equals(L"access_token"))
        {
            token = entry->Value;
            gotToken = true;
        }
        else if (entry->Name->Equals(L"expires_in"))
        {
            expiration = entry->Value;
            gotExpiration = true;
        }
        else if (entry->Name->Equals(L"state"))
        {
            state = entry->Value;
            gotState = true;
        }
        else
        {
            gotBadField = true;
        }
    }

    if (gotToken && gotExpiration && !gotBadField)
    {
        data = ref new FBAccessTokenData(token, expiration, state);
    }

    return data;
}

bool FBAccessTokenData::IsExpired(
    )
{
    bool expired = true;
    Calendar^ cal = ref new Calendar();
    cal->SetToNow();
    expired = (cal->CompareDateTime(ExpirationDate) >= 0);

    return expired;
}

void FBAccessTokenData::SetPermissions(
    IVectorView<Object^>^ perms
    )
{
    _grantedPermissions = nullptr;
    _declinedPermissions = nullptr;
    Vector<String^>^ granted = ref new Vector<String^>(0);
    Vector<String^>^ declined = ref new Vector<String^>(0);

    for (unsigned int i = 0; i < perms->Size; i++)
    {
        FBPermission^ perm = static_cast<FBPermission^>(perms->GetAt(i));

        if (!String::CompareOrdinal(perm->Status, L"granted"))
        {
            granted->Append(perm->Permission);
        }
        else if (!String::CompareOrdinal(perm->Status, L"declined"))
        {
            declined->Append(perm->Permission);
        }
    }

    _grantedPermissions = ref new FBPermissions(granted->GetView());
    _declinedPermissions = ref new FBPermissions(declined->GetView());
}

void FBAccessTokenData::CalculateExpirationDateTime(
    String^ Expiration
    )
{
    Calendar^ cal = ref new Calendar();
    int numSecs = _wtoi(Expiration->Data());
    cal->SetToNow();
    cal->AddSeconds(numSecs);
    _expirationDate = cal->GetDateTime();
}

#ifdef _DEBUG
void FBAccessTokenData::DebugPrintExpirationTime(
    )
{
    DateTimeFormatter^ dtfDay = ref new DateTimeFormatter(YearFormat::Default,
        MonthFormat::Default, DayFormat::Default, DayOfWeekFormat::Default);
    DateTimeFormatter^ dtfTime = ref new DateTimeFormatter(HourFormat::Default,
        MinuteFormat::Default, SecondFormat::Default);
    String^ msgString = L"Token expires at " + dtfDay->Format(_expirationDate) +
        L", " + dtfTime->Format(_expirationDate) + L"\n";
    OutputDebugString(msgString->Data());
}
#endif
