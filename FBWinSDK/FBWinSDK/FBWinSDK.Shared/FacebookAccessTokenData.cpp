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
using namespace Facebook;
using namespace Facebook::Graph;
using namespace concurrency;
using namespace std;

#define SECS_TO_HNS   10000000

FBAccessTokenData::FBAccessTokenData(
    String^ AccessToken,
    String^ Expiration,
    String^ State
    ) :
    m_accessToken(AccessToken),
    m_appId(nullptr),
    m_permissions(nullptr),
    m_userId(nullptr)
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
    m_accessToken(AccessToken),
    m_appId(nullptr),
    m_permissions(nullptr),
    m_userId(nullptr),
    m_expirationDate(Expiration)
{
#ifdef _DEBUG
    DebugPrintExpirationTime();
#endif
}

String^ FBAccessTokenData::AccessToken::get()
{
    return m_accessToken;
}

String^ FBAccessTokenData::AppID::get()
{
    return m_appId;
}

DateTime FBAccessTokenData::ExpirationDate::get()
{
    return m_expirationDate;
}

IMapView<String^, String^>^ FBAccessTokenData::Permissions::get()
{
    return m_permissions->GetView();
}

String^ FBAccessTokenData::UserID::get()
{
    return m_userId;
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

void FBAccessTokenData::AddPermissions(
    IVectorView<Object^>^ perms
    )
{
    m_permissions = ref new Map<String^, String^>();
    for (unsigned int i = 0; i < perms->Size; i++)
    {
        FBPermission^ perm = 
            static_cast<FBPermission^>(perms->GetAt(i));
        m_permissions->Insert(perm->Permission, perm->Status);
    }
}

void FBAccessTokenData::CalculateExpirationDateTime(
    String^ Expiration
    )
{
    Calendar^ cal = ref new Calendar();
    ULONGLONG numSecs = 0;
    ULONGLONG numTicks = 0;
    HRESULT hr = S_OK;

    numSecs = (ULONGLONG)_wtoi64(Expiration->Data());

    // Default to expiring 'now' if we can't convert to the proper time.  This
    // may save us some trouble later.
    cal->SetToNow();
    m_expirationDate = cal->GetDateTime();
    // Convert to ticks
    hr = ULongLongMult(numSecs, SECS_TO_HNS, &numTicks);
    if (SUCCEEDED(hr))
    {
        ULONGLONG expirationTimeInTicks = 0;
        // Add ticks to current time
        hr = ULongLongAdd(numTicks, cal->GetDateTime().UniversalTime, 
            &expirationTimeInTicks);
        if (SUCCEEDED(hr))
        {
            // If we haven't overflowed a signed int64, it's safe to cast and
            // we now have an accurate expiration DateTime.
            if (expirationTimeInTicks < INT64_MAX)
            {
                m_expirationDate.UniversalTime = (int64)expirationTimeInTicks;
            }
        }
    }
}

#ifdef _DEBUG
void FBAccessTokenData::DebugPrintExpirationTime(
    )
{
    DateTimeFormatter^ dtfDay = ref new DateTimeFormatter(YearFormat::Default,
        MonthFormat::Default, DayFormat::Default, DayOfWeekFormat::Default);
    DateTimeFormatter^ dtfTime = ref new DateTimeFormatter(HourFormat::Default,
        MinuteFormat::Default, SecondFormat::Default);
    String^ msgString = L"Token expires at " + dtfDay->Format(m_expirationDate) +
        L", " + dtfTime->Format(m_expirationDate) + L"\n";
    OutputDebugString(msgString->Data());
}
#endif