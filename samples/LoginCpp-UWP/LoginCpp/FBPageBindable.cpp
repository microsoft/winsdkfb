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
#include "FBPageBindable.h"

using namespace Facebook;
using namespace Facebook::Graph;
using namespace LoginCpp;
using namespace Platform;
using namespace Windows::Data::Json;
using namespace Windows::Foundation::Collections;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::UI::Xaml::Data;

FBPageBindable::FBPageBindable(
    FBPage^ page
    ) :
    _page(page)
{
    ;
}

String^ FBPageBindable::Id::get()
{
    return _page->Id;
}
void FBPageBindable::Id::set(String^ value)
{
    _page->Id = value;
    NotifyPropertyChanged(L"Id");
}

String^ FBPageBindable::Category::get()
{
    return _page->Category;
}
void FBPageBindable::Category::set(String^ value)
{
    _page->Category = value;
    NotifyPropertyChanged(L"Category");
}

int FBPageBindable::Checkins::get()
{
    return _page->Checkins;
}
void FBPageBindable::Checkins::set(int value)
{
    _page->Checkins = value;
    NotifyPropertyChanged(L"Checkins");
}

String^ FBPageBindable::Description::get()
{
    return _page->Description;
}
void FBPageBindable::Description::set(String^ value)
{
    _page->Description = value;
    NotifyPropertyChanged(L"Description");
}

int FBPageBindable::Likes::get()
{
    return _page->Likes;
}
void FBPageBindable::Likes::set(int value)
{
    _page->Likes = value;
    NotifyPropertyChanged(L"Likes");
}

String^ FBPageBindable::Link::get()
{
    return _page->Link;
}
void FBPageBindable::Link::set(String^ value)
{
    _page->Link = value;
    NotifyPropertyChanged(L"Link");
}

String^ FBPageBindable::Name::get()
{
    return _page->Name;
}
void FBPageBindable::Name::set(String^ value)
{
    _page->Name = value;
    NotifyPropertyChanged(L"Name");
}

Object^ FBPageBindable::FromJson(
    String^ JsonText 
    )
{
    FBPageBindable^ result = nullptr;
    Object^ obj = FBPage::FromJson(JsonText);
    if (obj)
    {
        result = ref new FBPageBindable(static_cast<FBPage^>(obj));
    }

    return result;
}

void FBPageBindable::NotifyPropertyChanged(
    String^ prop
    )
{
    CoreApplication::MainView->CoreWindow->Dispatcher->RunAsync(
        Windows::UI::Core::CoreDispatcherPriority::Normal, 
        ref new Windows::UI::Core::DispatchedHandler([this, prop]()
    {
        PropertyChangedEventArgs^ args = ref new PropertyChangedEventArgs(prop);
        PropertyChanged(this, args);
    }));
}


