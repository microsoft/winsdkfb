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
#include "AppToken.h"

using namespace concurrency;
using namespace Facebook;
using namespace Facebook::Graph;
using namespace Platform;
using namespace SDKCppUnit;
using namespace Windows::Data::Json;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;

#define FBSDKTokenServiceHost "fbwinsdk.azure-mobile.net"
#define FBSDKTokenApiPath     "/api/get_token"
#define AccessTokenKey        "access_token"

AppToken::AppToken(
    )
{
    _Token = nullptr;
}

IAsyncOperation<AppToken^>^ AppToken::CreateAsync(
    )
{
    PropertySet^ parameters = ref new PropertySet();
    String^ result = nullptr;

    FBSession^ sess = FBSession::ActiveSession;

    parameters->Insert("request_host", FBSDKTokenServiceHost);
    parameters->Insert("access_token", "");
    parameters->Insert("id", sess->FBAppId);

    FBSingleValue^ sval = ref new FBSingleValue(FBSDKTokenApiPath,
        parameters,
        ref new FBJsonClassFactory([] (String^ JsonText) -> JsonObject^
    {
        JsonObject^ obj = nullptr;
        if (!JsonObject::TryParse(JsonText, &obj))
        {
            obj = nullptr;
        }

        return obj;
    }));

    return create_async([=]() -> task<AppToken^>
    {
        return create_task(sval->GetAsync())
            .then([=](FBResult^ result) -> AppToken^
        {
            AppToken^ token = nullptr;

            if (result->Succeeded)
            {
                JsonObject^ obj = static_cast<JsonObject^>(result->Object);
                if (obj->HasKey(AccessTokenKey))
                {
                    token = ref new AppToken();
                    token->_Token = obj->GetNamedString(AccessTokenKey);
                }
            }
            return token;
        });
    });
}

String^ AppToken::Token::get(
    )
{
    return _Token;
}

