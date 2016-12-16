//******************************************************************************
//
// Copyright(c) 2016, Facebook, Inc. All rights reserved.
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

#ifndef __NOFBAPPEVENTS__

#include "FacebookAppEvents.h"
#include "FacebookSession.h"
#include "HttpManager.h"
#include "SDKMessage.h"

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
using namespace Windows::Services::Store;
using namespace Windows::Storage;
using namespace Windows::System::UserProfile;

#ifdef DEBUG
#define ALWAYS_LOG_INSTALLS
#endif

// Constants
#define FACEBOOK_ACTIVITIES_PATH L"/activities"
#define FACEBOOK_MOBILE_APP_INSTALL L"MOBILE_APP_INSTALL"
#define FACEBOOK_CUSTOM_APP_EVENTS L"CUSTOM_APP_EVENTS"


#pragma region GetCampaignIdHelpers

String^ GetCampaignId(bool useSimulator)
{
    String^ campaignIdField = "customPolicyField1";
    if (Windows::Foundation::Metadata::ApiInformation::IsTypePresent("Windows.Services.Store.StoreContext"))
    {
        auto ctx = StoreContext::GetDefault();
        StoreProductResult^ productResult = create_task(ctx->GetStoreProductForCurrentAppAsync()).get();
        if (productResult != nullptr && productResult->Product != nullptr)
        {
            for each (StoreSku^ sku in productResult->Product->Skus)
            {
                if (sku->IsInUserCollection)
                {
                    return sku->CollectionData->CampaignId;
                }
            }
            // Yes, it is OK to return "" without checking the license when the user collection is present.
            // the AppLicense fallback below won't apply.
            return "";
        }

        StoreAppLicense^ appLicense = concurrency::create_task(ctx->GetAppLicenseAsync()).get();

        //This backup method is used for purchases that did not have an MSA; there was no user.
        if (appLicense != nullptr && appLicense->ExtendedJsonData != nullptr)
        {
            JsonObject^ json = nullptr;
            if (JsonObject::TryParse(appLicense->ExtendedJsonData, &json))
            {
                if (json->HasKey(campaignIdField))
                {
                    return json->GetNamedString(campaignIdField);
                }
            }
        }
        return "";
    }
    else
    {
        if (useSimulator)
        {
            return create_task(CurrentAppSimulator::GetAppPurchaseCampaignIdAsync()).get();
        }
        else
        {
            return create_task(CurrentApp::GetAppPurchaseCampaignIdAsync()).get();
        }
    }

    return "";
}


task<String^> GetCampaignIdTask(bool useSimulator)
{
    return concurrency::create_task([=]()-> String^
    {
        return GetCampaignId(useSimulator);
    });
}

IAsyncOperation<String^>^ GetCampaignIdAsync(bool useSimulator)
{
    return concurrency::create_async([=]
    {
        return GetCampaignIdTask(useSimulator);
    });
}

#pragma endregion GetCampaignIdHelpers

bool FBSDKAppEvents::_useSimulator = false;

bool FBSDKAppEvents::UseSimulator::get()
{
    return _useSimulator;
}

void FBSDKAppEvents::UseSimulator::set(bool value)
{
    _useSimulator = value;
}

void FBSDKAppEvents::ActivateApp()
{
    FBSession^ session = FBSession::ActiveSession;
    // Try to grab the application id from session.
    String^ appId = session->FBAppId;

    if (appId == nullptr || appId->IsEmpty())
    {
        throw ref new Platform::InvalidArgumentException(SDKMessageMissingAppID);
    }
    //Install tracking should not fail or throw so here we catch
    //and silently ignore most errors..
    try
    {
        create_task(FBSDKAppEvents::PublishInstall(appId));
        create_task(FBSDKAppEvents::LogActivateEvent(appId));
    }
    catch (Platform::Exception^  ex)
    {
#if DEBUG
        throw;
#endif
    }
    catch (...)
    {
    }
}

IAsyncAction^ FBSDKAppEvents::PublishInstall(
    String^ AppId
    )
{
    String^ lastPingKey = L"LastAttributionPing" + AppId;
    String^ lastResponseKey = L"LastInstallResponse" + AppId;
    ApplicationDataContainer^ settings = FBSession::DataContainer;
    String^ pingTime = safe_cast<String^>(settings->Values->Lookup(lastPingKey));

    return create_async([=]() -> void
    {
#ifndef ALWAYS_LOG_INSTALLS
        if (!pingTime )
#endif
        {
            create_task(FBSDKAppEvents::LogInstallEvent(AppId))
                .then([=](String^ lastAttributionResponse) -> void
            {
                // Set last ping time
                Calendar^ calendar = ref new Calendar();
                calendar->SetToNow();
                INT64 universaltime = calendar->GetDateTime().UniversalTime;
                settings->Values->Insert(
                    lastPingKey,
                    dynamic_cast<PropertyValue^>(
                        PropertyValue::CreateString(universaltime.ToString())
                    )
                );

                // Set last response
                settings->Values->Insert(
                    lastResponseKey,
                    dynamic_cast<PropertyValue^>(PropertyValue::CreateString(lastAttributionResponse))
                );

#ifdef DEBUG
                String^ msg = L"Mobile App Install Response: " + lastAttributionResponse + L"\n"
                    L"Mobile App Install Ping Time: " + calendar->GetDateTime() + L"\n";
                OutputDebugString(msg->Data());
#endif
            });
        }
    });
}

IAsyncOperation<String^>^ FBSDKAppEvents::LogInstallEvent(
    String^ AppId
)
{
    String^ path = AppId + FACEBOOK_ACTIVITIES_PATH;
    PropertySet^ parameters = ref new PropertySet();
    parameters->Insert(L"event", FACEBOOK_MOBILE_APP_INSTALL);
    parameters->Insert(L"advertiser_id", AdvertisingManager::AdvertisingId);
    parameters->Insert(
        L"advertiser_tracking_enabled",
        AdvertisingManager::AdvertisingId->IsEmpty() ? "0" : "1"
    );

    return create_async([=]() -> task<String^>
    {
        return create_task([=]() -> String^
        {
            try
            {
                String^ campaignID = GetCampaignIdTask(FBSDKAppEvents::UseSimulator).get();
                parameters->Insert(L"windows_attribution_id", campaignID);
                String ^postResult = create_task(HttpManager::Instance->PostTaskAsync(path, parameters->GetView())).get();
                return postResult;
            }
            catch (Platform::Exception^ ex)
            {
                OutputDebugString(L"This can happen when the app is not yet published. If that is the case, ignore it.");
                OutputDebugString(ex->Message->Data());
            }

            OutputDebugString(L"This value must be replaced");
            //TODO: what is right default value?
            return ref new String(L"");
        });
    });
}

IAsyncAction^ FBSDKAppEvents::LogActivateEvent(
    String^ AppId
    )
{
    String^ path = AppId + FACEBOOK_ACTIVITIES_PATH;
    PropertySet^ parameters = ref new PropertySet();
    parameters->Insert(L"event", FACEBOOK_CUSTOM_APP_EVENTS);
    parameters->Insert(L"custom_events", FBSDKAppEvents::GetActivateAppJson());
    parameters->Insert(L"advertiser_id", AdvertisingManager::AdvertisingId);
    parameters->Insert(
        L"advertiser_tracking_enabled",
        AdvertisingManager::AdvertisingId->IsEmpty() ? "0" : "1"
    );

    return create_async([=]()
    {
        return create_task(HttpManager::Instance->PostTaskAsync(path, parameters->GetView()))
            .then([=](String^ response) -> void
        {
#ifdef _DEBUG
            String^ msg = L"Custom App Event Response: " + response;
            OutputDebugString(msg->Data());
#endif
        });
    });
}

String^ FBSDKAppEvents::GetActivateAppJson()
{
    JsonArray^ customEvents = ref new JsonArray();
    JsonObject^ activateJson = ref new JsonObject();
    activateJson->SetNamedValue(
        L"_eventName",
        JsonValue::CreateStringValue(L"fb_mobile_activate_app")
    );
    customEvents->Append(activateJson);
    return customEvents->ToString();
}

#endif //__NOFBAPPEVENTS__