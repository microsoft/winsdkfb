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

#pragma once

#define FACEBOOK_LOGIN_SUCCESS_PATH  L"/connect/login_success.html"
#define FACEBOOK_DESKTOP_SERVER_NAME L"https://www.facebook.com"
#define FACEBOOK_MOBILE_SERVER_NAME  L"https://m.facebook.com"
#define FACEBOOK_LOGOUT_PATH  L"/logout.php"
#define FACEBOOK_DIALOG_CLOSE_PATH   L"/dialog/close"

#define ScopeKey        L"scope"
#define DisplayKey      L"display"
#define ResponseTypeKey L"response_type"
#define DefaultScope    L"public_profile,email,user_friends"
#define DefaultDisplay  L"popup"
#define DefaultResponse L"token"

#define AuthTypeKey     L"auth_type"
#define Rerequest       L"rerequest"
#define RedirectUriKey  L"redirect_uri"

#define FBAccountProvider L"https://www.facebook.com"

#define TICKS_PER_SECOND    10000000 
#define SECONDS_PER_MINUTE  60
#define _90_MINUTES_IN_TICKS (90 * SECONDS_PER_MINUTE * TICKS_PER_SECOND)

#define SDK_APP_DATA_CONTAINER "winsdkfb"
#define GRANTED_PERMISSIONS_KEY "granted_permissions"
