//******************************************************************************
//
// Copyright(c) 2016, Facebook, Inc.All rights reserved.
//
// You are hereby granted a non-exclusive, worldwide, royalty-free license to 
// use, copy, modify, and distribute this software in source code or binary form 
// for use in connection with the web services and APIs provided by Facebook.
//
// As with any software that integrates with the Facebook platform, your use of 
// this software is subject to the Facebook Developer Principles and Policies 
// [http://developers.facebook.com/policy/]. This copyright notice shall be 
// included in all copies or substantial portions of the software. 
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE 
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN 
// THE SOFTWARE.
//
//******************************************************************************

#include "pch.h"

#include "FBSDKAppEvents.h"
#include "FacebookClient.h"

using namespace concurrency;
using namespace std;
using namespace winsdkfb;

using namespace Platform;
using namespace Windows::ApplicationModel::Resources;
using namespace Windows::ApplicationModel::Store;
using namespace Windows::Data::Json;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Globalization;
using namespace Windows::Storage;
using namespace Windows::System::UserProfile;

#define FBActivitiesPath L"/activities"
#define FBMobileAppInstall L"MOBILE_APP_INSTALL"
#define FBCustomAppEvent L"CUSTOM_APP_EVENTS"
#define FBAppIDName L"FBApplicationId"

/*
* To integrate install tracking for mobile app install ads,
* call this method when the app is launched.
*/
void FBSDKAppEvents::ActivateApp()
{
    // Try to grab the application id from resource file.
    String^ appId;
    try {
        ResourceLoader^ rl = ResourceLoader::GetForCurrentView();
        appId = rl->GetString(FBAppIDName);
    }
    catch (Exception^ e) {
        throw ref new NotImplementedException(
            FBAppIDName + L" needs to be added to resource file."
        );
    }

    if (!appId) {
        throw ref new NotImplementedException(
            FBAppIDName + L" cannot contain empty value"
        );
    }

    create_task(FBSDKAppEvents::PublishInstall(appId));
    create_task(FBSDKAppEvents::LogActivateEvent(appId));
}

/*
 * Publish an install event to the Facebook graph endpoint.
 * Write the timestamp to localSettings so we only trigger this once.
 */
IAsyncAction^ FBSDKAppEvents::PublishInstall(
    String^ AppId
    )
{
    String^ lastPingKey = L"LastAttributionPing" + AppId;
    String^ lastResponseKey = L"LastInstallResponse" + AppId;
    ApplicationDataContainer^ localSettings = ApplicationData::Current->LocalSettings;
    String^ pingTime = safe_cast<String^>(localSettings->Values->Lookup(lastPingKey));

    return create_async([=]() -> void
    {
        if (!pingTime) {
            create_task(FBSDKAppEvents::LogInstallEvent(AppId))
                .then([=](String^ lastAttributionResponse) -> void
            {
                // Set last ping time
                Calendar^ calendar = ref new Calendar();
                calendar->SetToNow();
                INT64 universaltime = calendar->GetDateTime().UniversalTime;
                localSettings->Values->Insert(
                    lastPingKey,
                    dynamic_cast<PropertyValue^>(
                        PropertyValue::CreateString(universaltime.ToString())
                    )
                );

                // Set last response
                localSettings->Values->Insert(
                    lastResponseKey,
                    dynamic_cast<PropertyValue^>(PropertyValue::CreateString(lastAttributionResponse))
                );

#ifdef _DEBUG
                String^ msg = L"Mobile App Install Response: " + lastAttributionResponse + L"\n"
                    L"Mobile App Install Ping Time: " + calendar->GetDateTime() + L"\n";
                OutputDebugString(msg->Data());
#endif
            });
        }
    });
}

/*
 * Logs an install event to the Facebook graph endpoint.
 * The user will be looked up using idfa or windows_attribution_id
 */
IAsyncOperation<String^>^ FBSDKAppEvents::LogInstallEvent(
    String^ AppId
    )
{
    String^ path = AppId + FBActivitiesPath;
    PropertySet^ parameters = ref new PropertySet();
    parameters->Insert(L"event", FBMobileAppInstall);
    parameters->Insert(L"advertiser_id", AdvertisingManager::AdvertisingId);
    parameters->Insert(
        L"advertiser_tracking_enabled",
        AdvertisingManager::AdvertisingId->IsEmpty() ? "0" : "1"
    );

    return create_async([=]() -> task<String^>
    {
        return create_task(CurrentApp::GetAppPurchaseCampaignIdAsync())
            .then([=](String^ campaignID) -> task<String^>
        {
            parameters->Insert(L"windows_attribution_id", campaignID);
            return create_task([=]() -> IAsyncOperation<String^>^
            {
                return FBClient::PostTaskAsync(path, parameters);
            });
        });
    });
}

/*
 * Logs a custom app event to the Facebook graph endpoint.
 */
IAsyncAction^ FBSDKAppEvents::LogActivateEvent(
    String^ AppId
    )
{
    String^ path = AppId + FBActivitiesPath;
    PropertySet^ parameters = ref new PropertySet();
    parameters->Insert(L"event", FBCustomAppEvent);
    parameters->Insert(L"custom_events", FBSDKAppEvents::GetActivateAppJson());
    parameters->Insert(L"advertiser_id", AdvertisingManager::AdvertisingId);
    parameters->Insert(
        L"advertiser_tracking_enabled",
        AdvertisingManager::AdvertisingId->IsEmpty() ? "0" : "1"
    );

    return create_async([=]()
    {
        return create_task(FBClient::PostTaskAsync(path, parameters))
            .then([=](String^ response) -> void
        {
#ifdef _DEBUG
            String^ msg = L"Custom App Event Response: " + response;
            OutputDebugString(msg->Data());
#endif
        });
    });
}

/*
 * Creates a JSON array encapsulating the activate app event
 */
String^ FBSDKAppEvents::GetActivateAppJson() {
    JsonArray^ customEvents = ref new JsonArray();
    JsonObject^ activateJson = ref new JsonObject();
    activateJson->SetNamedValue(
        L"_eventName",
        JsonValue::CreateStringValue(L"fb_mobile_activate_app")
    );
    customEvents->Append(activateJson);
    return customEvents->ToString();
}