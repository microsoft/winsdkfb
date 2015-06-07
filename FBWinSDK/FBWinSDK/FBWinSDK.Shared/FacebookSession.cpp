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

#include <ppltasks.h>

#include "pch.h"
#include "FacebookAccessTokenData.h"
#include "FacebookSession.h"
#include "FacebookAppRequest.h"
#include "FacebookDialog.xaml.h"
#include "FacebookFeedRequest.h"
#include "FacebookPaginatedArray.h"
#include "FacebookResult.h"
#include "FBPermission.h"
#include "FBSingleValue.h"
#include "FBUser.h"
#include "SDKMessage.h"

using namespace concurrency;
using namespace Facebook;
using namespace Facebook::Graph;
using namespace Platform;
using namespace Platform::Collections;
using namespace std;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Data::Json;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Globalization;
using namespace Windows::Security::Authentication::Web;
using namespace Windows::Security::Credentials;
using namespace Windows::Security::Cryptography;
using namespace Windows::Security::Cryptography::DataProtection;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::System;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;

#define INT64_STRING_BUFSIZE 65

namespace Facebook
{
    HANDLE login_evt = NULL;
}

FBSession::FBSession() :
    m_AccessTokenData(nullptr),
    m_AppResponse(nullptr),
    m_loggedIn(false),
    m_FBAppId(nullptr),
    m_WinAppId(nullptr),
    m_permissions(nullptr),
    m_user(nullptr)
{
    m_permissions = ref new Vector<String^>;
	if (!login_evt)
	{
		login_evt = CreateEventEx(NULL, NULL, 0, DELETE | SYNCHRONIZE);
	}
}

Facebook::FBSession::~FBSession()
{
	if (login_evt)
	{
		CloseHandle(login_evt);
		login_evt = NULL;
	}
}

String^ FBSession::FBAppId::get()
{
    if (!m_FBAppId)
    {
        m_FBAppId = ref new String(L"MyApp");
    }

    return m_FBAppId;
}

void FBSession::FBAppId::set(String^ value)
{
    m_FBAppId = value;
}

String^ FBSession::WinAppId::get()
{
    return m_WinAppId;
}

void FBSession::WinAppId::set(String^ value)
{
    m_WinAppId = value;
}

String^ FBSession::AppResponse::get()
{
    return m_AppResponse;
}

bool FBSession::LoggedIn::get()
{
    return m_loggedIn;
}

FBAccessTokenData^ FBSession::AccessTokenData::get()
{
    return m_AccessTokenData;
}

IVectorView<String^>^ FBSession::Permissions::get()
{
    return m_permissions->GetView();
}

Windows::Foundation::DateTime FBSession::Expires::get()
{
    return m_Expires;
}

void FBSession::Expires::set(Windows::Foundation::DateTime value)
{
    m_Expires = value;
}

bool FBSession::IsExpired::get()
{
    bool expired = true;

    Windows::Globalization::Calendar^ cal = ref new Windows::Globalization::Calendar();

    cal->SetToNow();

    DateTime now = cal->GetDateTime();

    if (m_Expires.UniversalTime >= now.UniversalTime)
    {
        expired = true;
    }
    else
    {
        expired = false;
    }

    return expired;
}

FBUser^ FBSession::User::get()
{
    return m_user;
}

void FBSession::AddPermission(
    String^ permission
    )
{
    m_permissions->Append(permission);
}

void FBSession::ResetPermissions(
    )
{
    if (m_permissions)
    {
        m_permissions->Clear();
    }
}

void FBSession::Logout()
{
    m_permissions->Clear();
    m_user = nullptr;
    m_FBAppId = nullptr;
    m_WinAppId = nullptr;
    m_AccessTokenData = nullptr;
    m_AppResponse = nullptr;
    m_loggedIn = false;

    TryDeleteTokenData();
}

task<FBResult^> FBSession::GetUserInfo(
    Facebook::FBAccessTokenData^ TokenData
    )
{
    FBSingleValue^ value = ref new FBSingleValue(
        "/me",
        nullptr,
        ref new FBJsonClassFactory([](String^ JsonText) -> Object^
        {
            return FBUser::FromJson(JsonText);
        }));

    return create_task(value->Get());
}

String^ FBSession::PermissionsToString()
{
    String^ permissionsString = ref new String();

    for (unsigned int i = 0; i < m_permissions->Size; i++)
    {
        if (i)
        {
            permissionsString += ",";
        }

        permissionsString += m_permissions->GetAt(i);
    }

    return permissionsString;
}

IAsyncOperation<IStorageItem^>^ FBSession::MyTryGetItemAsync(
    StorageFolder^ folder,
    String^ itemName
    )
{
    return create_async([=]
    {
        return create_task([=]() -> task<IStorageItem^>
        {
            return create_task(folder->GetItemAsync(itemName));
        })
        .then([=](task<IStorageItem^> folderTask) -> IStorageItem^
        {
            try
            {
                return folderTask.get();
            }
            catch (Exception^ e)
            {
                return nullptr;
            }
        });
    }); 
}

task<FBResult^> FBSession::CheckForExistingToken(
    )
{
    task<FBResult^> result;
    if (LoggedIn)
    {
        FBResult^ authResult = ref new FBResult(this->AccessTokenData);
        result = create_task([=]()
        {
            return authResult;
        });
    }
    else 
    {
        StorageFolder^ folder = ApplicationData::Current->LocalFolder;
        result = create_task(MyTryGetItemAsync(folder, "FBSDKData"))
        .then([=](IStorageItem^ item) 
        {
            task<IBuffer^> bufTask;
            StorageFile^ file = dynamic_cast<StorageFile^>(item);
            if (file)
            {
                bufTask = create_task(FileIO::ReadBufferAsync(file));
            }
            else
            {
                bufTask = create_task([]() -> IBuffer^
                {
                    return nullptr;
                });
            }
            return bufTask;
        })
        .then([](IBuffer^ protectedBuffer)
        {
            IBuffer^ clearBuffer = nullptr;
            task<IBuffer^> decryptTask;

            if (protectedBuffer)
            {
                DataProtectionProvider^ provider = 
                    ref new DataProtectionProvider();

                decryptTask = create_task(provider->UnprotectAsync(
                    protectedBuffer));
            }
            else
            {
                decryptTask = create_task([]() -> IBuffer^
                {
                    return nullptr;
                });
            }
            return decryptTask;
        })
        .then([](IBuffer^ clearBuffer) -> FBResult^
        {
            FBResult^ cachedResult = nullptr;

            if (clearBuffer)
            {
                String^ clearText = CryptographicBuffer::ConvertBinaryToString(
                    BinaryStringEncoding::Utf16LE, clearBuffer);
                
                if (clearText)
                {
                    wstring vals(clearText->Data());
                    size_t pos = vals.find(L",");

                    if (pos != wstring::npos)
                    {
                        String^ accessToken = ref new String(vals.substr(0, pos).c_str());
                        String^ expirationString = ref new String(vals.substr(pos + 1, wstring::npos).c_str());
                        DateTime expirationTime;

                        String^ msg = L"Access Token: " + accessToken + L"\n";
                        OutputDebugString(msg->Data());

                        expirationTime.UniversalTime = _wtoi64(expirationString->Data());
                        Facebook::FBAccessTokenData^ cachedData = 
                            ref new Facebook::FBAccessTokenData(
                                accessToken, expirationTime, ref new String());
                        cachedResult = ref new FBResult(cachedData);
                    }
                }
            }
            
            return cachedResult;
        });
    }

    return result;
}

void FBSession::TrySaveTokenData(
    )
{
    if (this->LoggedIn)
    {
        create_task([this]() -> task<IBuffer^>
        {
            wchar_t buffer[INT64_STRING_BUFSIZE];
            DataProtectionProvider^ provider = ref new DataProtectionProvider(L"LOCAL=user");
            _i64tow_s(
                this->AccessTokenData->ExpirationDate.UniversalTime, 
                    buffer, INT64_STRING_BUFSIZE, 10);
            String^ tokenData = this->AccessTokenData->AccessToken + 
                "," + ref new String(buffer);
            IBuffer^ dataBuff = 
                CryptographicBuffer::ConvertStringToBinary(tokenData, 
                    BinaryStringEncoding::Utf16LE);
    
            IAsyncOperation<IBuffer^>^ protectOp = provider->ProtectAsync(dataBuff);
            return create_task(protectOp);
        })
        .then([](IBuffer^ protectedData)
        {
            StorageFolder^ folder = ApplicationData::Current->LocalFolder;

            return create_task(folder->CreateFileAsync("FBSDKData",
                CreationCollisionOption::OpenIfExists))
            .then([protectedData](StorageFile^ file)
            {
                if (file)
                {
                    FileIO::WriteBufferAsync(file, protectedData);
                }
            });
        });
    }
}

void FBSession::TryDeleteTokenData(
    )
{
    StorageFolder^ folder = ApplicationData::Current->LocalFolder;
    create_task(MyTryGetItemAsync(folder, "FBSDKData"))
    .then([=](IStorageItem^ item) -> task<void>
    {
        if (item)
        {
            return create_task(item->DeleteAsync());
        }
        else
        {
            return create_task([]()
            {
                ;
            });
        }
    })
    .then([](task<void> deleteTask)
    {
        try
        {
            deleteTask.get();
        }
        catch (...)
        {
            //Do nothing here, trying to delete the cache file is a "fire and
            //forget" operation.  If it fails, we'll pick up bad token data at
            //next login, fail the login and retry, then attempt to cache new
            //valid token data.
            ;
        }
    });
}

Windows::Foundation::IAsyncAction^ FBSession::ShowFeedDialog(
    )
{
    FacebookDialog^ requestsControl = ref new FacebookDialog();
    Platform::String^ errorMessage = nullptr;
    std::function<void ()>&& action = nullptr;

    auto callback = ref new Windows::UI::Core::DispatchedHandler(
        [requestsControl, &errorMessage, action, this]()
    {
        Windows::UI::Core::CoreWindow^ wnd1 = CoreApplication::MainView->CoreWindow;
        
        Popup^ popup = ref new Popup();
        popup->HorizontalAlignment = Windows::UI::Xaml::HorizontalAlignment::Stretch;
        popup->VerticalAlignment = Windows::UI::Xaml::VerticalAlignment::Stretch;


        try
        {
            Grid^ myGrid = ref new Grid();

            // TODO: (sanjeevd) hard coded, remove and fix these
            myGrid->Margin =  Windows::UI::Xaml::Thickness(0,0,0,0);
            myGrid->Height = wnd1->Bounds.Height;
            myGrid->Width = wnd1->Bounds.Width;
           
            requestsControl->Width = myGrid->Width;

            myGrid->Children->Append(requestsControl);
            popup->Child = myGrid;
            popup->IsOpen = true;

            requestsControl->ShowFeedDialog(popup);
        }
        catch(Exception^ ex)
        {
            // TODO: (sanjeevd) remove the line below. Currently leaving for sake of debugging
            int x = 10;
        }
    });

    Windows::UI::Core::CoreWindow^ wnd = Windows::ApplicationModel::Core::CoreApplication::MainView->CoreWindow;

    IAsyncAction^ waiter = wnd->Dispatcher->RunAsync(
        Windows::UI::Core::CoreDispatcherPriority::Normal,
        callback);

    // create a task that will wait for the login control to finish doing what it was doing
    IAsyncAction^ task = concurrency::create_async(
        [this, &callback, requestsControl]() 
    {
        // wait for the browser to fire that the login is done
		if (login_evt)
		{
			WaitForSingleObjectEx(login_evt, INFINITE, FALSE);
		}
	});

    return task;
}

Windows::Foundation::IAsyncAction^ FBSession::ShowRequestsDialog(
    )
{
    FacebookDialog^ requestsControl = ref new FacebookDialog();
    Platform::String^ errorMessage = nullptr;
    std::function<void ()>&& action = nullptr;

    auto callback = ref new Windows::UI::Core::DispatchedHandler(
        [requestsControl, &errorMessage, action, this]()
    {
        Windows::UI::Core::CoreWindow^ wnd1 = CoreApplication::MainView->CoreWindow;
        
        Popup^ popup = ref new Popup();
        popup->HorizontalAlignment = Windows::UI::Xaml::HorizontalAlignment::Stretch;
        popup->VerticalAlignment = Windows::UI::Xaml::VerticalAlignment::Stretch;


        try
        {
            Grid^ myGrid = ref new Grid();

            // TODO: (sanjeevd) hard coded, remove and fix these
            myGrid->Margin =  Windows::UI::Xaml::Thickness(0,0,0,0);
            myGrid->Height = wnd1->Bounds.Height;
            myGrid->Width = wnd1->Bounds.Width;
           
            requestsControl->Width = myGrid->Width;

            myGrid->Children->Append(requestsControl);
            popup->Child = myGrid;
            popup->IsOpen = true;

            requestsControl->ShowRequestsDialog(popup);
        }
        catch(Exception^ ex)
        {
            // TODO: (sanjeevd) remove the line below. Currently leaving for sake of debugging
            int x = 10;
        }
    });

    Windows::UI::Core::CoreWindow^ wnd = Windows::ApplicationModel::Core::CoreApplication::MainView->CoreWindow;

    IAsyncAction^ waiter = wnd->Dispatcher->RunAsync(
        Windows::UI::Core::CoreDispatcherPriority::Normal,
        callback);

    // create a task that will wait for the login control to finish doing what it was doing
    IAsyncAction^ task = concurrency::create_async(
        [this, &callback, requestsControl]() 
    {
        // wait for the browser to fire that the login is done
		if (login_evt)
		{
			WaitForSingleObjectEx(login_evt, INFINITE, FALSE);
		}
	});

    return task;
}

task<FBResult^> FBSession::ShowLoginDialog(
    )
{
    FacebookDialog^ loginControl = ref new FacebookDialog();
    Platform::String^ errorMessage = nullptr;
    std::function<void()>&& action = nullptr;

    auto callback = ref new Windows::UI::Core::DispatchedHandler(
        [loginControl, &errorMessage, action, this]()
    {
        Windows::UI::Core::CoreWindow^ wnd1 = CoreApplication::MainView->CoreWindow;

        Popup^ popup = ref new Popup();
        popup->HorizontalAlignment = Windows::UI::Xaml::HorizontalAlignment::Stretch;
        popup->VerticalAlignment = Windows::UI::Xaml::VerticalAlignment::Stretch;


        try
        {
            Grid^ myGrid = ref new Grid();

            // TODO: (sanjeevd) hard coded, remove and fix these
            myGrid->Margin = Windows::UI::Xaml::Thickness(0, 0, 0, 0);
            myGrid->Height = wnd1->Bounds.Height;
            myGrid->Width = wnd1->Bounds.Width;

            loginControl->Width = myGrid->Width;

            myGrid->Children->Append(loginControl);
            popup->Child = myGrid;
            popup->IsOpen = true;

            loginControl->ShowLoginDialog(popup);
        }
        catch (Exception^ ex)
        {
            // TODO: (sanjeevd) remove the line below. Currently leaving for sake of debugging
            int x = 10;
        }
    });

    Windows::UI::Core::CoreWindow^ wnd = 
        Windows::ApplicationModel::Core::CoreApplication::MainView->CoreWindow;

    IAsyncAction^ waiter = wnd->Dispatcher->RunAsync(
        Windows::UI::Core::CoreDispatcherPriority::Normal,
        callback);

    // create a task that will wait for the login control to finish doing what it was doing
    return create_task([this, &callback, loginControl]()
    {
        // wait for the browser to fire that the login is done
        if (login_evt)
        {
            WaitForSingleObjectEx(login_evt, INFINITE, FALSE);
        }

        //TODO: finish implementation, i.e. extract token and return a real value.
        return (FBResult^)nullptr;
    });
}

task<FBResult^> FBSession::GetAppPermissions(
    )
{
    FBPaginatedArray^ permArr = ref new FBPaginatedArray(
        L"/" + m_user->Id + L"/permissions",
        nullptr,
        ref new FBJsonClassFactory([](String^ JsonText) -> Object^
        {
            return FBPermission::FromJson(JsonText);
        }));

    return create_task(permArr->First())
        .then([this, permArr](FBResult^ result)
    {
        if (result->Succeeded)
        {
            IVectorView<Object^>^ perms = 
                static_cast<IVectorView<Object^>^>(result->Object);
            m_AccessTokenData->AddPermissions(perms);
        }

        return ref new FBResult(m_user);
    });
}

#if WINAPI_FAMILY==WINAPI_FAMILY_PHONE_APP

Uri^ FBSession::BuildLoginUri(
    )
{
    String^ uriString = 
        "https://m.facebook.com/v2.1/dialog/oauth?redirect_uri=" + 
        Uri::EscapeComponent(GetRedirectUriString()) + "&display=touch&state=" +
        "%7B%220is_active_session%22%3A1%2C%22is_open_session%22%3A1%2C%22com." + 
        "facebook.sdk_client_state%22%3A1%2C%223_method%22%3A%22browser_auth" +
        "%22%7D";
    String^ uriStringEnd = "&type=user_agent&client_id=" + m_FBAppId + 
        "&sdk=ios";
    String^ permissionsString = PermissionsToString();
 
    if (!permissionsString->IsEmpty())
    {
        uriString += L"&scope=" + permissionsString;
    }
    uriString += uriStringEnd;

    OutputDebugString(uriString->Data());
    OutputDebugString(L"\n");

    return ref new Uri(uriString);
}

String^ FBSession::GetRedirectUriString(
    )
{
    return L"fb" + FBAppId + "://authorize";
}

Uri^ FBSession::RemoveJSONFromBrowserResponseUri(
    Uri^ responseUri
    )
{
    wstring cleanUri = responseUri->AbsoluteUri->Data();
    wstring JSONStart = L"/#state";
    wstring JSONEnd = L"}&";
    
    size_t startPos = cleanUri.find(JSONStart);
    if (startPos != wstring::npos)
    {
        size_t endPos = cleanUri.find(JSONEnd);
        if (endPos != wstring::npos)
        {
            wstring front = cleanUri.substr(0, startPos) + L"?";
            wstring back = cleanUri.substr(endPos + JSONEnd.size(), wstring::npos);
            cleanUri = front + back;
        }
    }

    return ref new Uri(ref new String(cleanUri.c_str()));
}

int64 FBSession::SecondsTilTokenExpires(
    DateTime Expiration
    )
{
    Calendar^ cal = ref new Calendar();
    cal->SetToNow();
    DateTime now = cal->GetDateTime();

    int64 ticksTilExpired = Expiration.UniversalTime - now.UniversalTime;
    if (ticksTilExpired < 0)
    {
        ticksTilExpired = 0;
    }

    return ticksTilExpired / 10000000;
}

IAsyncOperation<FBResult^>^ FBSession::LoginAndContinue()
{
    // Check for valid FB App ID
    // Check for valid Windows App ID
    // Check for permissions
    // Try to see what's up with app package
    return create_async([=] () -> task<FBResult^>
    {
        FBResult^ authResult = nullptr;

        return CheckForExistingToken()
        .then([this](FBResult^ authResult)
        {
            task<FBResult^> loginTask;
            if (authResult && authResult->Succeeded)
            {
                m_AccessTokenData = static_cast<Facebook::FBAccessTokenData^>(authResult->Object);
                m_loggedIn = true;
                loginTask = GetUserInfo(m_AccessTokenData);
            }
            else
            {
                loginTask = create_task([]() -> FBResult^
                {
                    return nullptr;
                });
            }

            return loginTask;
        })
        .then([this](FBResult^ graphResult) -> task<FBResult^>
        {
            task<FBResult^> innerResult;
            Uri^ uriToLaunch = nullptr;

            if (graphResult && graphResult->Succeeded)
            {
                wchar_t buffer[INT64_STRING_BUFSIZE];
                m_user = static_cast<FBUser^>(graphResult->Object);
                innerResult = GetAppPermissions();
                TrySaveTokenData();
                _i64tow_s(
                    SecondsTilTokenExpires(m_AccessTokenData->ExpirationDate),
                        buffer, INT64_STRING_BUFSIZE, 10);
                String^ launchString = GetRedirectUriString() + 
                    L"?access_token=" + m_AccessTokenData->AccessToken +
                    L"&expires_in=" + ref new String(buffer) +
                    L"&state=.";
                uriToLaunch = ref new Uri(launchString);
                innerResult = create_task([=]() -> FBResult^
                {
                    return graphResult;
                });
            }
            else
            {
                m_AccessTokenData = nullptr;
                innerResult = create_task([]() -> FBResult^
                {
                    return nullptr;
                });
                uriToLaunch = BuildLoginUri();
            }

            CoreWindow^ wind = CoreApplication::MainView->CoreWindow;
            if (wind)
            {
                CoreDispatcher^ disp = wind->Dispatcher;
                if (disp)
                {
                    disp->RunAsync(
                        Windows::UI::Core::CoreDispatcherPriority::Normal, 
                        ref new Windows::UI::Core::DispatchedHandler([uriToLaunch, this]()
                    {
                        // No cached token, launch the FB app
                        concurrency::task<bool> launchUriOperation(
                            Windows::System::Launcher::LaunchUriAsync(uriToLaunch));
                        launchUriOperation.then([](bool launchResult)
                        {
                            if (launchResult)
                            {
                                OutputDebugString(L"launchResult is TRUE\n");
                            }
                            {
                                OutputDebugString(L"launchResult is FALSE\n");
                            }
                        })
                        .then([](task<void>t)
                        {
                            try
                            {
                                t.get();
                                OutputDebugString(L"FB launch succeeded\n");
                            }
                            catch (Platform::COMException^ e)
                            {
                                OutputDebugString(L"ERROR: FB app launch failed\n");
                                OutputDebugString(e->Message->Data());
                            }
                        });
                    }));
                }
            }

            return innerResult;
        });
    });
}

bool FBSession::IsLoginResponse(
    String^ Response
    )
{
    return (Response == L"authorize");
}

bool FBSession::IsFeedDialogResponse(
    String^ Response
    )
{
    return (Response == L"feed_success");
}

bool FBSession::IsRequestDialogResponse(
    String^ Response
    )
{
    return (Response == L"requests_success");
}

IAsyncOperation<FBResult^>^ FBSession::ContinueAction(
    ProtocolActivatedEventArgs^ p
    )
{
    String^ uriString = p->Uri->Host;
    IAsyncOperation<FBResult^>^ result;
    
    if (IsLoginResponse(uriString))
    {
        result = ContinueLogin(p->Uri);
    }
    else if (IsFeedDialogResponse(uriString))
    {
        result = ContinuePostToFeed(p->Uri);
    }
    else if (IsRequestDialogResponse(uriString))
    {
        result = ContinueAppRequest(p->Uri);
    }
    else
    {
        return create_async([]() -> task<FBResult^>
        {
            return create_task([]() -> FBResult^
            {
                return nullptr;
            });
        });
    }

    return result;
}

IAsyncOperation<FBResult^>^ FBSession::ContinueLogin(
    Uri^ Response 
    )
{
    return create_async([=]() -> task<FBResult^>
    {
        if (LoggedIn)
        {
            return create_task([=]() -> FBResult^
            {
                return ref new FBResult(m_user);
            });
        }
        else
        {
            Uri^ cleanUri = RemoveJSONFromBrowserResponseUri(Response);
            FBError^ loginError = FBError::FromUri(cleanUri);
            Facebook::FBAccessTokenData^ tokenData = Facebook::FBAccessTokenData::FromUri(cleanUri);

            return create_task([loginError, tokenData, this]() -> task<FBResult^>
            {
                task<FBResult^> graphTask;
                if (loginError)
                {
                    graphTask = create_task([=]() -> FBResult^
                    {
                        return ref new FBResult(loginError);
                    });
                }
                else if (tokenData)
                {
                    m_AccessTokenData = tokenData;
                    m_loggedIn = true;
                    TrySaveTokenData();
                    graphTask = GetUserInfo(tokenData);
                }
                else
                {
                    graphTask = create_task([]() -> FBResult^
                    {
                        return nullptr;
                    });
                }

                return graphTask;
            })
            .then([tokenData, this](FBResult^ loginResult) -> FBResult^
            {
                if (loginResult && loginResult->Succeeded)
                {
                    m_user = static_cast<FBUser^>(loginResult->Object);
                }

                return loginResult;
            });
        }
    });
}

IAsyncOperation<FBResult^>^ FBSession::ContinuePostToFeed(
    Uri^ Response 
    )
{
    String^ msg = L"Feed response: " + Response->DisplayUri + L"\n";
    OutputDebugString(msg->Data());

    return create_async([this, Response]() -> task<FBResult^>
    {
        return create_task([this, Response]() -> FBResult^
        {
            FBFeedRequest^ info = 
                FBFeedRequest::FromFeedDialogResponse(Response);
            FBResult^ result = nullptr;

            if (info)
            {
                result = ref new FBResult(info);
            }
            else
            {
                FBError^ err = FBError::FromUri(Response);
                if (!err)
                {
                    String^ ErrorObjectJson = L"{\"error\": {\"message\": "
                        L"\"Operation Canceled\", \"type\": "
                        L"\"OAuthException\", \"code\": 4201, "
                        L"\"error_user_msg\": \"User canceled the Dialog flow\""
                        L"}}";

                    //No post ID, and no error returned - user canceled the 
                    //dialog, so fake up a proper error
                    err = FBError::FromJson(ErrorObjectJson);
                }
                result = ref new FBResult(err);
            }

            return result;
        });
    });
}

IAsyncOperation<FBResult^>^ FBSession::ContinueAppRequest(
    Uri^ Response 
    )
{
    String^ msg = L"Request response: " + Response->DisplayUri + L"\n";
    OutputDebugString(msg->Data());

    return create_async([this, Response]() -> task<FBResult^>
    {
        return create_task([this, Response]() -> FBResult^
        {
            FBError^ err = FBError::FromUri(Response);
            FBAppRequest^ info = 
                FBAppRequest::FromRequestDialogResponse(Response);
            FBResult^ result = nullptr;

            if (err)
            {
                result = ref new FBResult(err);
            }
            else if (info)
            {
                result = ref new FBResult(info);
            }

            return result;
        });
    });
}

#else

Uri^ FBSession::BuildLoginUri(
    )
{
    String^ uriString = L"https://www.facebook.com/dialog/oauth?client_id=" +
        m_FBAppId;
    String^ permissionsString = PermissionsToString();

    uriString += L"&redirect_uri=" + Uri::EscapeComponent(
        GetRedirectUriString()) + L"%2fauth";

    if (!permissionsString->IsEmpty())
    {
        uriString += L"&scope=" + permissionsString;
    }

    uriString += L"&display=popup&response_type=token";

    return ref new Uri(uriString);
}

String^ FBSession::GetRedirectUriString(
    )
{
    Uri^ endURI = WebAuthenticationBroker::GetCurrentApplicationCallbackUri();
    String^ blerg = endURI->DisplayUri;
    OutputDebugString(blerg->Data());
    OutputDebugString(L"\n");
    return endURI->DisplayUri;
}

task<FBResult^> FBSession::ProcessAuthResult(
	WebAuthenticationResult^ authResult
	)
{
	return create_task([=]() -> FBResult^
	{
		FBResult^ result = nullptr;
		String^ uriString = nullptr;
		FBAccessTokenData^ tokenData = nullptr;
		Uri^ uri = nullptr;
		switch (authResult->ResponseStatus)
		{
		case WebAuthenticationStatus::ErrorHttp:
			//TODO: need a real error code
			result = ref new FBResult(ref new FBError(0,
				L"Communication error",
				L"An Http error occurred"));
			break;
		case WebAuthenticationStatus::Success:
			//TODO: need a real error code
			uriString = authResult->ResponseData;
			uri = ref new Uri(uriString);
			tokenData = FBAccessTokenData::FromUri(uri);
			if (!tokenData)
			{
				result = ref new FBResult(FBError::FromUri(uri));
			}
			else
			{
				result = ref new FBResult(tokenData);
			}
			break;
		case WebAuthenticationStatus::UserCancel:
			result = ref new FBResult(ref new FBError(0,
				L"User canceled",
				L"The login operation was canceled"));
			break;
		}

		return result;
	});
}

task<FBResult^> FBSession::TryGetUserInfoAfterLogin(
	FBResult^ loginResult
	)
{
	task<FBResult^> innerResult;

	if (loginResult && loginResult->Succeeded)
	{
		m_AccessTokenData = static_cast<FBAccessTokenData^>(loginResult->Object);
		m_loggedIn = true;
		TrySaveTokenData();
		innerResult = GetUserInfo(m_AccessTokenData);
	}
	else
	{
		innerResult = create_task([=]()
		{
			return loginResult;
		});
	}

	return innerResult;
}

task<FBResult^> FBSession::TryGetAppPermissionsAfterLogin(
	FBResult^ loginResult
	)
{
	task<FBResult^> finalResult;
	if (loginResult->Succeeded)
	{
		m_user = static_cast<FBUser^>(loginResult->Object);
		finalResult = GetAppPermissions();
	}
	else
	{
		finalResult = create_task([=]()
		{
			return loginResult;
		});
	}

	return finalResult;
}

task<FBResult^> FBSession::RunOAuthOnUiThread(
    )
{
	task<void> authTask = create_task(
		CoreApplication::MainView->CoreWindow->Dispatcher->RunAsync(
		Windows::UI::Core::CoreDispatcherPriority::Normal,
		ref new Windows::UI::Core::DispatchedHandler([this]() 
	{
		m_loginTask = create_task(
			WebAuthenticationBroker::AuthenticateAsync(
			WebAuthenticationOptions::None, BuildLoginUri(),
			ref new Uri(GetRedirectUriString())))
		.then([this](WebAuthenticationResult^ authResult) -> task<FBResult^>
		{
			return ProcessAuthResult(authResult);
		})
		.then([this](FBResult^ graphResult) -> task<FBResult^>
		{
			return TryGetUserInfoAfterLogin(graphResult);
		})
		.then([this](FBResult^ loginResult) -> task<FBResult^>
		{
			return TryGetAppPermissionsAfterLogin(loginResult);
		});
	})));

	return create_task([=](void)
	{
		try
		{
			authTask.get();
		}
		catch (Exception^ ex)
		{
			throw ref new InvalidArgumentException(SDKMessageLoginFailed);
		}
	})
	.then([this]() -> FBResult^
	{
		FBResult^ result = nullptr;

		try
		{
			result = m_loginTask.get();
		}
		catch (Exception^ ex)
		{
			throw ref new InvalidArgumentException(SDKMessageLoginFailed);
		}

		return result;
	});
}

IAsyncOperation<FBResult^>^ FBSession::LoginAsync(
    )
{
    return create_async([=]() -> FBResult^
    {
        FBResult^ result = nullptr;

        task<FBResult^> loginTask = TryLoginViaWebView();
        result = loginTask.get();
        if (!result)
        {
            loginTask = TryLoginViaWebAuthBroker();
            result = loginTask.get();
        }

        return result;
    });
}

task<FBResult^> FBSession::TryLoginViaWebView(
    )
{
    FBSession^ sess = FBSession::ActiveSession;

    IAsyncOperation<FBResult^>^ result = nullptr;

    return CheckForExistingToken()
        .then([this](FBResult^ oauthResult) -> task<FBResult^>
    {
        task<FBResult^> graphTask;
        if (oauthResult && oauthResult->Succeeded)
        {
            Facebook::FBAccessTokenData^ tokenData = 
                static_cast<Facebook::FBAccessTokenData^>(oauthResult->Object);
            if (!tokenData->IsExpired())
            {
                m_AccessTokenData = tokenData;
                m_loggedIn = true;
                graphTask = GetUserInfo(m_AccessTokenData);
            }
            else
            {
                graphTask = create_task([]() -> FBResult^
                {
                    return nullptr;
                });
            }
        }
        else
        {
            graphTask = create_task([]() -> FBResult^
            {
                return nullptr;
            });
        }

        return graphTask;
    })
    .then([this](FBResult^ graphResult)
    {
        if (graphResult && graphResult->Succeeded)
        {
            m_user = static_cast<FBUser^>(graphResult->Object);
            return GetAppPermissions();
        }
        else
        {
            return ShowLoginDialog();
        }
    });
}

task<FBResult^> FBSession::TryLoginViaWebAuthBroker(
    )
{
    FBSession^ sess = FBSession::ActiveSession;

    IAsyncOperation<FBResult^>^ result = nullptr;

    return CheckForExistingToken()
    .then([this](FBResult^ oauthResult) -> task<FBResult^>
    {
        task<FBResult^> graphTask;
        if (oauthResult && oauthResult->Succeeded)
        {
            Facebook::FBAccessTokenData^ tokenData = static_cast<Facebook::FBAccessTokenData^>(oauthResult->Object);
            if (!tokenData->IsExpired())
            {
                m_AccessTokenData = tokenData;
                m_loggedIn = true;
                graphTask = GetUserInfo(m_AccessTokenData);
            }
            else
            {
                graphTask = create_task([]() -> FBResult^
                {
                    return nullptr;
                });
            }
        }
        else
        {
            graphTask = create_task([]() -> FBResult^
            {
                return nullptr;
            });
        }

        return graphTask;
    })
    .then([this](FBResult^ graphResult)
    {
        if (graphResult && graphResult->Succeeded)
        {
            m_user = static_cast<FBUser^>(graphResult->Object);
            return GetAppPermissions();
        }
        else
        {
            return RunOAuthOnUiThread();
        }
    });
}

#endif
