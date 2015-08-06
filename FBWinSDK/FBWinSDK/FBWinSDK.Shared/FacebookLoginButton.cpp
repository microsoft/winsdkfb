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
#include "FacebookLoginButton.h"
#include "FacebookSession.h"

using namespace Concurrency;
using namespace Facebook;
using namespace Facebook::Graph;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;

FBLoginButton::FBLoginButton() :
//    _loginBehavior(SessionLoginBehavior::SessionLoginBehaviorWithFallbackToWebView),
    _permissions(nullptr)
{
    String^ styleKey = FBLoginButton::typeid->FullName;
    this->DefaultStyleKey = styleKey;
    _permissions = ref new Vector<String^>();
}

void FBLoginButton::OnApplyTemplate(
    )
{
    Button::OnApplyTemplate();

    // TODO: Do we need to save the token here and unregister the event handler
    // at some point?
    this->Click += ref new RoutedEventHandler(this, &FBLoginButton::OnClick);
}

// TODO: Implement behaviors.  These will have to be plumbed down into the
// Session as well.
//SessionLoginBehavior FBLoginButton::LoginBehavior::get()
//{
//    return _loginBehavior;
//}
//
//void FBLoginButton::LoginBehavior::set(SessionLoginBehavior value)
//{
//    _loginBehavior = value;
//}
//

IVector<String^>^ FBLoginButton::Permissions::get()
{
    return _permissions;
}

void FBLoginButton::Permissions::set(IVector<String^>^ Values)
{
    _permissions->Clear();
    IIterator<String^>^ it = nullptr;
    for (it = Values->First(); it->HasCurrent; it->MoveNext())
    {
        String^ value = it->Current;
        _permissions->Append(value);
    }
}

void FBLoginButton::InitWithPermissions(
    IVector<String^>^ permissions
    )
{
    if (!_permissions)
    {
        _permissions = ref new Vector<String^>(0);
    }

    _permissions->Clear();

    for (IIterator<String^>^ iter = permissions->First();
        iter->HasCurrent;
        iter->MoveNext())
    {
        _permissions->Append(iter->Current);
    }
}

void FBLoginButton::SetSessionPermissions(
    )
{
    FBSession^ s = FBSession::ActiveSession;
    s->ResetPermissions();
    if (_permissions)
    {
        IIterator<String^>^ iter = nullptr;
        for (iter = _permissions->First(); iter->HasCurrent; iter->MoveNext())
        {
            s->AddPermission(iter->Current);
        }
    }
}

void FBLoginButton::OnClick(
    Object^ sender,
    RoutedEventArgs^ e
    )
{
    FBSession^ s = FBSession::ActiveSession;

    if (s->LoggedIn == true)
    {
        ;
    }
    else
    {
        SetSessionPermissions();

        create_task(s->LoginAsync())
            .then([=](FBResult^ result)
        {
            if (result->Succeeded)
            {
                FBUser^ user = static_cast<FBUser^>(result->Object);
            }
            else
            {
                ;
            }
        });
    }
}
