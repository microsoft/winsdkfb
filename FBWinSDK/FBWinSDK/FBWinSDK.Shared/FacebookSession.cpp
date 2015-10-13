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
#include "FacebookError.h"
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
extern const wchar_t* ErrorObjectJson;

namespace Facebook
{
    HANDLE login_evt = NULL;
}

FBSession::FBSession() :
    _AccessTokenData(nullptr),
    _AppResponse(nullptr),
    _loggedIn(false),
    _FBAppId(nullptr),
    _WinAppId(nullptr),
    _user(nullptr)
{
    if (!login_evt)
    {
        login_evt = CreateEventEx(NULL, NULL, 0, DELETE | SYNCHRONIZE);
    }
    _showingDialog = FALSE;
    _APIMajorVersion = 2;
    _APIMinorVersion = 1;
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
    if (!_FBAppId)
    {
        _FBAppId = ref new String(L"<INSERT YOUR APP ID HERE>");

#ifdef _DEBUG
        OutputDebugString(L"!!! Missing App ID.  Update your app to use a valid FB App ID in order for the FB API's to succeed");
#endif
    }

    return _FBAppId;
}

void FBSession::FBAppId::set(String^ value)
{
    _FBAppId = value;
}

String^ FBSession::WinAppId::get()
{
    return _WinAppId;
}

void FBSession::WinAppId::set(String^ value)
{
    _WinAppId = value;
}

String^ FBSession::AppResponse::get()
{
    return _AppResponse;
}

bool FBSession::LoggedIn::get()
{
    return _loggedIn;
}

FBAccessTokenData^ FBSession::AccessTokenData::get()
{
    return _AccessTokenData;
}

void FBSession::AccessTokenData::set(FBAccessTokenData^ value)
{
    _AccessTokenData = value;
}

FBUser^ FBSession::User::get()
{
    return _user;
}

IAsyncAction^ FBSession::LogoutAsync()
{
    _user = nullptr;
    _FBAppId = nullptr;
    _WinAppId = nullptr;
    _AccessTokenData = nullptr;
    _AppResponse = nullptr;
    _loggedIn = false;

    return TryDeleteTokenData();
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

    return create_task(value->GetAsync());
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
        .then([](task<IBuffer^> clearBufferTask) -> FBResult^
        {
            FBResult^ cachedResult = nullptr;
            IBuffer^ clearBuffer = nullptr;

            try
            {
                clearBuffer = clearBufferTask.get();
            }
            catch (InvalidArgumentException^ ex)
            {
#ifdef _DEBUG
                OutputDebugString(L"Couldn't decrypt cached token.  Continuing without cached token data.\n");
#endif
            }

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
        create_task([=]() -> task<IBuffer^>
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

IAsyncAction^ FBSession::TryDeleteTokenData(
    )
{
    StorageFolder^ folder = ApplicationData::Current->LocalFolder;
#ifdef _DEBUG
    String^ msg = L"Deleting cached token from " + folder->Path + L"\n";
    OutputDebugString(msg->Data());
#endif
    return create_async([=]()
    {
        return create_task(MyTryGetItemAsync(folder, "FBSDKData"))
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
#ifdef _DEBUG
                OutputDebugString(L"Deleting cached token file failed!\n");
#endif
            }
        });
    });
}

IAsyncOperation<FBResult^>^ FBSession::ShowFeedDialogAsync(
    PropertySet^ Parameters
    )
{
    _dialog = ref new FacebookDialog();

    _showingDialog = TRUE;

    auto callback = ref new DispatchedHandler(
        [=]()
    {
        try
        {
            _dialog->ShowFeedDialog(Parameters);
        }
        catch(Exception^ ex)
        {
            _showingDialog = FALSE;
        }
    });

    Windows::UI::Core::CoreWindow^ wnd = CoreApplication::MainView->CoreWindow;

    IAsyncAction^ waiter = wnd->Dispatcher->RunAsync(
        Windows::UI::Core::CoreDispatcherPriority::Normal,
        callback);

    // create a task that will wait for the login control to finish doing what it was doing
    IAsyncOperation<FBResult^>^ task = concurrency::create_async(
        [=]()
    {
        FBResult^ dialogResponse = nullptr;

        // TODO: Is there a better way to do this?  I was using an event, but
        // the concurrency event object was deprecated in the Win10 SDK tools.
        // Switched to plain old Windows event, but that didn't work at all,
        // so polling for now.
        while (_showingDialog && !dialogResponse)
        {
            dialogResponse = _dialog->GetDialogResponse();
            Sleep(0);
        }

        if (!_showingDialog)
        {
            FBError^ err = FBError::FromJson(ref new String(ErrorObjectJson));
            dialogResponse = ref new FBResult(err);
        }

        _showingDialog = FALSE;
        _dialog = nullptr;
        return dialogResponse;
    });

    return task;
}

IAsyncOperation<FBResult^>^ FBSession::ShowRequestsDialogAsync(
    PropertySet^ Parameters
    )
{
    _dialog = ref new FacebookDialog();

    _showingDialog = TRUE;

    auto callback = ref new DispatchedHandler(
        [=]()
    {
        try
        {
            _dialog->ShowRequestsDialog(Parameters);
        }
        catch(Exception^ ex)
        {
            _showingDialog = FALSE;
        }
    });

    Windows::UI::Core::CoreWindow^ wnd = CoreApplication::MainView->CoreWindow;

    IAsyncAction^ waiter = wnd->Dispatcher->RunAsync(
        Windows::UI::Core::CoreDispatcherPriority::Normal,
        callback);

    // create a task that will wait for the login control to finish doing what it was doing
    IAsyncOperation<FBResult^>^ task = concurrency::create_async(
        [=]() 
    {
        FBResult^ dialogResponse = nullptr;

        // TODO: Is there a better way to do this?  I was using an event, but
        // the concurrency event object was deprecated in the Win10 SDK tools.
        // Switched to plane old Windows event, but that didn't work at all,
        // so polling for now.
        while (_showingDialog && !dialogResponse)
        {
            dialogResponse = _dialog->GetDialogResponse();
            Sleep(0);
        }

        if (!_showingDialog)
        {
            FBError^ err = FBError::FromJson(ref new String(ErrorObjectJson));
            dialogResponse = ref new FBResult(err);
        }

        _showingDialog = FALSE;
        _dialog = nullptr;
        return dialogResponse;
    });

    return task;
}

task<FBResult^> FBSession::ShowLoginDialog(
    PropertySet^ Parameters
    )
{
    _dialog = ref new FacebookDialog();

    _showingDialog = TRUE;

    auto callback = ref new DispatchedHandler(
        [=]()
    {
        try
        {
            _dialog->ShowLoginDialog(Parameters);
        }
        catch (Exception^ ex)
        {
            _showingDialog = FALSE;
        }
    });

    Windows::UI::Core::CoreWindow^ wnd = 
        Windows::ApplicationModel::Core::CoreApplication::MainView->CoreWindow;

    IAsyncAction^ waiter = wnd->Dispatcher->RunAsync(
        Windows::UI::Core::CoreDispatcherPriority::Normal,
        callback);

    // create a task that will wait for the login control to finish doing what it was doing
    return create_task([=]()
    {
        FBResult^ dialogResponse = nullptr;

        // TODO: Is there a better way to do this?  I was using an event, but
        // the concurrency event object was deprecated in the Win10 SDK tools.
        // Switched to plane old Windows event, but that didn't work at all,
        // so polling for now.
        while (_showingDialog && !dialogResponse)
        {
            dialogResponse = _dialog->GetDialogResponse();
            Sleep(0);
        } 

        if (_showingDialog)
        {
            if (dialogResponse->Succeeded)
            {
                AccessTokenData =
                    static_cast<FBAccessTokenData^>(dialogResponse->Object);
            }
        }
        else
        {
            FBError^ err = FBError::FromJson(ref new String(ErrorObjectJson));
            dialogResponse = ref new FBResult(err);
        }

        _showingDialog = FALSE;
        _dialog = nullptr;
        return dialogResponse;
    });
}

task<FBResult^> FBSession::GetAppPermissions(
    )
{
    FBPaginatedArray^ permArr = ref new FBPaginatedArray(
        L"/" + _user->Id + L"/permissions",
        nullptr,
        ref new FBJsonClassFactory([](String^ JsonText) -> Object^
        {
            return FBPermission::FromJson(JsonText);
        }));

    return create_task(permArr->FirstAsync())
        .then([this, permArr](FBResult^ result)
    {
        if (result->Succeeded)
        {
            IVectorView<Object^>^ perms = 
                static_cast<IVectorView<Object^>^>(result->Object);
            _AccessTokenData->SetPermissions(perms);
        }

        return ref new FBResult(_user);
    });
}

#define ScopeKey        L"scope"
#define DisplayKey      L"display"
#define ResponseTypeKey L"response_type"
#define DefaultScope    L"public_profile,email,user_friends"
#define DefaultDisplay  L"popup"
#define DefaultResponse L"token"
#define AuthTypeKey     L"auth_type"
#define Rerequest       L"rerequest"

Uri^ FBSession::BuildLoginUri(
    PropertySet^ Parameters
    )
{
    FBSession^ s = FBSession::ActiveSession;
    String^ apiVersion = L"";
    if (APIMajorVersion)
    {
        apiVersion = L"v" + APIMajorVersion.ToString() + L"." + APIMinorVersion.ToString() + L"/";
    }
    String^ uriString = L"https://" + 
        L"www.facebook.com/" + apiVersion + L"dialog/oauth?client_id=" + s->FBAppId;

    // Use some reasonable default login parameters
    String^ scope = DefaultScope;
    String^ displayType = DefaultDisplay;
    String^ responseType = DefaultResponse;

    uriString += L"&redirect_uri=" + GetRedirectUriString();

    // Enumerate through all the parameters
    IIterator<IKeyValuePair<String^, Object^>^>^ first = Parameters->First();
    while (first && (first->HasCurrent))
    {
        String^ Key = first->Current->Key;
        String^ Value = dynamic_cast<String^>(first->Current->Value);
        if (Value)
        {
            if (!String::CompareOrdinal(Key, ScopeKey))
            {
                scope = Value;
            }
            else if (!String::CompareOrdinal(Key, DisplayKey))
            {
                displayType = Value;
            }
            else if (!String::CompareOrdinal(Key, ResponseTypeKey))
            {
                responseType = Value;
            }
            else
            {
                uriString += L"&" + Key + L"=" + Value;
            }
        }

        first->MoveNext();
    }

    uriString += L"&" + ScopeKey + L"=" + scope + L"&" + DisplayKey + L"=" +
        displayType + L"&" + ResponseTypeKey + L"=" + responseType;

    return ref new Uri(uriString);
}

String^ FBSession::GetRedirectUriString(
    )
{
    Uri^ endURI = WebAuthenticationBroker::GetCurrentApplicationCallbackUri();
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
        _AccessTokenData = static_cast<FBAccessTokenData^>(loginResult->Object);
        _loggedIn = true;
        TrySaveTokenData();
        innerResult = GetUserInfo(_AccessTokenData);
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
        _user = static_cast<FBUser^>(loginResult->Object);
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
    PropertySet^ Parameters
    )
{
    task<void> authTask = create_task(
        CoreApplication::MainView->CoreWindow->Dispatcher->RunAsync(
        Windows::UI::Core::CoreDispatcherPriority::Normal,
        ref new Windows::UI::Core::DispatchedHandler([=]()
    {
        _loginTask = create_task(
            WebAuthenticationBroker::AuthenticateAsync(
            WebAuthenticationOptions::None, BuildLoginUri(Parameters),
            ref new Uri(GetRedirectUriString())))
        .then([this](WebAuthenticationResult^ authResult) -> task<FBResult^>
        {
            return ProcessAuthResult(authResult);
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
            result = _loginTask.get();
        }
        catch (Exception^ ex)
        {
            throw ref new InvalidArgumentException(SDKMessageLoginFailed);
        }

        return result;
    });
}

task<FBResult^> FBSession::RunWebViewLoginOnUIThread(
    PropertySet^ Parameters
    )
{
    task<void> authTask = create_task(
        CoreApplication::MainView->CoreWindow->Dispatcher->RunAsync(
            Windows::UI::Core::CoreDispatcherPriority::Normal,
            ref new Windows::UI::Core::DispatchedHandler([=]()
    {
        _loginTask = ShowLoginDialog(Parameters);
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
            result = _loginTask.get();
        }
        catch (Exception^ ex)
        {
            throw ref new InvalidArgumentException(SDKMessageLoginFailed);
        }

        return result;
    });
}

IAsyncOperation<FBResult^>^ FBSession::LoginAsync(
    FBPermissions^ Permissions,
	SessionLoginMethod method
    )
{
    _dialog = ref new FacebookDialog();

    return create_async([=]()
    {
        PropertySet^ parameters = ref new PropertySet();
        if (Permissions)
        {
            parameters->Insert(L"scope", Permissions->ToString());
        }

        if (LoggedIn)
        {
            parameters->Insert(L"auth_type", L"rerequest");
        }

        return create_task([=]() -> FBResult^
        {
            FBResult^ result = nullptr;
            task<FBResult^> authTask;
            switch (method) 
            {
            case SessionLoginMethodDefault: 
                authTask = TryLoginViaWebView(parameters);
                result = authTask.get();
                if (!result)
                {
                    authTask = TryLoginViaWebAuthBroker(parameters);
                    result = authTask.get();
                }
                break;
            case SessionLoginMethodWebView: 
                authTask = TryLoginViaWebView(parameters);
                break;
            case SessionLoginMethodWebAuthBroker: 
                authTask = TryLoginViaWebAuthBroker(parameters);
                break;
            }
            return result;
        })
        .then([this](FBResult^ graphResult) -> task<FBResult^>
        {
            return TryGetUserInfoAfterLogin(graphResult);
        })
        .then([this](FBResult^ userInfoResult) -> task<FBResult^>
        {
            return TryGetAppPermissionsAfterLogin(userInfoResult);
        })
        .then([=](FBResult^ finalResult)
        {
            if (!finalResult->Succeeded)
            {
                _loggedIn = false;
                AccessTokenData = nullptr;
            }

            return finalResult;
        });
    });
}

task<FBResult^> FBSession::TryLoginViaWebView(
    PropertySet^ Parameters
    )
{
    FBSession^ sess = FBSession::ActiveSession;

    return create_task([=]() -> task<FBResult^>
    {
        task<FBResult^> graphTask = create_task([]() -> FBResult^
        {
            return nullptr;
        });

        if (!IsRerequest(Parameters))
        {
            graphTask = CheckForExistingToken();
        }

        return graphTask;
    })
    .then([this](FBResult^ oauthResult) -> task<FBResult^>
    {
        task<FBResult^> graphTask = create_task([]() -> FBResult^
        {
            return nullptr;
        });

        if (oauthResult && oauthResult->Succeeded)
        {
            Facebook::FBAccessTokenData^ tokenData = 
                static_cast<Facebook::FBAccessTokenData^>(oauthResult->Object);
            if (!tokenData->IsExpired())
            {
                AccessTokenData = tokenData;
                graphTask = create_task([=]() -> FBResult^
                {
                    return ref new FBResult(AccessTokenData);
                });
            }
        }

        return graphTask;
    })
    .then([=](FBResult^ graphResult)
    {
        FBResult^ loginResult = nullptr;

        if (graphResult && graphResult->Succeeded)
        {
            loginResult = graphResult;
        }
        else
        {
            loginResult = RunWebViewLoginOnUIThread(Parameters).get();
        }

        return loginResult;
    });
}

task<FBResult^> FBSession::TryLoginViaWebAuthBroker(
    PropertySet^ Parameters
    )
{
    FBSession^ sess = FBSession::ActiveSession;

    IAsyncOperation<FBResult^>^ result = nullptr;

    return create_task([=]() -> task<FBResult^>
    {
        task<FBResult^> graphTask = create_task([]() -> FBResult^
        {
            return nullptr;
        });

        if (!IsRerequest(Parameters))
        {
            graphTask = CheckForExistingToken();
        }

        return graphTask;
    })
    .then([=](FBResult^ oauthResult) -> task<FBResult^>
    {
        task<FBResult^> graphTask = create_task([]() -> FBResult^
        {
            return nullptr;
        });

        if (oauthResult && oauthResult->Succeeded)
        {
            Facebook::FBAccessTokenData^ tokenData = 
                static_cast<Facebook::FBAccessTokenData^>(oauthResult->Object);
            if (!tokenData->IsExpired())
            {
                AccessTokenData = tokenData;
                graphTask = create_task([=]() -> FBResult^
                {
                    return ref new FBResult(AccessTokenData);
                });
            }
        }

        return graphTask;
    })
    .then([=](FBResult^ graphResult)
    {
        task<FBResult^> loginResult;

        if (graphResult && graphResult->Succeeded)
        {
            loginResult = create_task([=]()
            {
                return graphResult;
            });
        }
        else
        {
            loginResult = RunOAuthOnUiThread(Parameters);
        }

        return loginResult;
    });
}

BOOL FBSession::IsRerequest(
    PropertySet^ Parameters
    )
{
    BOOL isRerequest = FALSE;

    if (Parameters && Parameters->HasKey(AuthTypeKey))
    {
        String^ Value = static_cast<String^>(Parameters->Lookup(AuthTypeKey));

        if (!String::CompareOrdinal(Value, Rerequest))
        {
            isRerequest = TRUE;
        }
    }

    return isRerequest;
}

void FBSession::SetAPIVersion(
    int MajorVersion,
    int MinorVersion
    )
{
    _APIMajorVersion = MajorVersion;
    _APIMinorVersion = MinorVersion;
}

int FBSession::APIMajorVersion::get()
{
    return _APIMajorVersion;
}

int FBSession::APIMinorVersion::get()
{
    return _APIMinorVersion;
}
