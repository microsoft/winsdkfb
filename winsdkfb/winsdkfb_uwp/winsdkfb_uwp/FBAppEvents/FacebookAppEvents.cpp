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

#ifdef DEBUG 
#define ALWAYSLOGINSTALLS 1 
#endif 

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
using namespace Windows::Storage;
using namespace Windows::System::UserProfile;

// Constants
#define FACEBOOK_ACTIVITIES_PATH L"/activities"
#define FACEBOOK_MOBILE_APP_INSTALL L"MOBILE_APP_INSTALL"
#define FACEBOOK_CUSTOM_APP_EVENTS L"CUSTOM_APP_EVENTS"


/*
 * To integrate install tracking for mobile app install ads,
 * call this method when the app is launched.
*/
void FBSDKAppEvents::ActivateApp()
{
	FBSession^ session = FBSession::ActiveSession;	 
	// Try to grab the application id from session.
	String^ appId = session->FBAppId;

	if ( appId == nullptr || appId->IsEmpty() )
		throw ref new Platform::InvalidArgumentException(SDKMessageMissingAppID); 
	/* 
		Install tracking should not fail or throw so here we catch 
		and silently ignore most errors.. 
	 */  
	try
	{
		create_task(FBSDKAppEvents::PublishInstall(appId));
		create_task(FBSDKAppEvents::LogActivateEvent(appId));
	}
	catch ( Platform::Exception^  ex ) 
	{
#if DEBUG 
		throw ex; 
#endif 
	}
	catch (...)
	{
	} 
}

bool FBSDKAppEvents::useSimulator = false; 
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
    ApplicationDataContainer^ settings = FBSession::DataContainer;
    String^ pingTime = safe_cast<String^>(settings->Values->Lookup(lastPingKey));

    return create_async([=]() -> void
    {
#if defined (DEBUG) && defined(ALWAYSLOGINSTALLS ) 
		if (!pingTime || true) { 
#else 
        if (!pingTime ) {
#endif 
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
		return create_task([=]()  
		{
			if (FBSDKAppEvents::UseSimulator)
			{
				return CurrentAppSimulator::GetAppPurchaseCampaignIdAsync();
			} 
			else
			{  
				return CurrentApp::GetAppPurchaseCampaignIdAsync(); 
			}
		}).then([=](task<String^> getCampaignIdTask) -> task<String^>
		{
			try
			{
				String^ campaignID = getCampaignIdTask.get();
				parameters->Insert(L"windows_attribution_id", campaignID);
				return create_task([=]() -> IAsyncOperation<String^>^
				{
					return HttpManager::Instance->PostTaskAsync(path, parameters);
				});
			}
			catch (Platform::Exception^ ex)
			{
				OutputDebugString(L"This happens when app is not yet published");
				OutputDebugString(ex->Message->Data());
			}
			 
			///Passing default value since we did not make network call 
			return create_task([]() -> String^
			{
				OutputDebugString(L"This value must be replaced");
				//TODO: what is right default value? 				
				return ref new String(L"");				 
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
        return create_task(HttpManager::Instance->PostTaskAsync(path, parameters))
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