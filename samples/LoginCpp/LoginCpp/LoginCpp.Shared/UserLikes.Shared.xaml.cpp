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
#include "UserLikes.xaml.h"

using namespace LoginCpp;

using namespace concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace winsdkfb;
using namespace winsdkfb::Graph;

IVector<FBPageBindable^>^ UserLikes::Items::get()
{
    return _listViewItems;
}

FBPageBindable^ UserLikes::SelectedItem::get()
{
    return _selectedItem;
}


void UserLikes::AddLikes(
    IVectorView<Object^>^ NewLikes
    )
{
    for (IIterator<Object^>^ it = NewLikes->First(); it->HasCurrent; it->MoveNext())
    {
        FBPageBindable^ page = static_cast<FBPageBindable^>(it->Current);
        if (page)
        {
            String^ msg = L"Found page: " + page->Name + "\n";
            OutputDebugString(msg->Data());
            _listViewItems->Append(page);
        }
    }
    // go through the paginated calls here so that we don't have to deal with task chaining
    if (_likes->HasNext)
    {
        create_task(_likes->NextAsync()).then([this](FBResult^ result)
        {
            if (result->Succeeded)
            {
                IVectorView<Object^>^ items = static_cast<IVectorView<Object^>^> (result->Object);
                if (items->Size > 0)
                {
                    AddLikes(items);
                }

            }
        });
    }
}

void UserLikes::GetUserLikes(
    )
{
    FBSession^ sess = FBSession::ActiveSession;
    if (sess->LoggedIn)
    {
        String^ graphPath = sess->User->Id + L"/likes";
        FBJsonClassFactory^ fact =
            ref new FBJsonClassFactory([](String^ JsonText) -> Object^
        {
            return FBPageBindable::FromJson(JsonText);
        });

        _likes = ref new FBPaginatedArray(graphPath, nullptr, fact);
        create_task(_likes->FirstAsync()).then([this](FBResult^ result)
        {
            if (result->Succeeded)
            {
                BadResultsTextBlock->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
                LikesListView->Visibility = Windows::UI::Xaml::Visibility::Visible;
                IVectorView<Object^>^ items = static_cast<IVectorView<Object^>^> (result->Object);
                if (items->Size > 0)
                {
                    AddLikes(items);
                }
                else
                {
                    LikesListView->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
                    BadResultsTextBlock->Visibility = Windows::UI::Xaml::Visibility::Visible;
                    BadResultsTextBlock->Text = L"No User likes found";
                }
            }
            else
            {
                LikesListView->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
                BadResultsTextBlock->Visibility = Windows::UI::Xaml::Visibility::Visible;
                BadResultsTextBlock->Text = result->ErrorInfo->Message;
            }
        });
    }
}

void UserLikes::ListView_SelectionChanged(
    Object^ sender, 
    SelectionChangedEventArgs^ e
    )
{
    FBPageBindable^ selected = static_cast<FBPageBindable^>(e->AddedItems->GetAt(0));
    if (selected)
    {
        FBSingleValue^ val = ref new FBSingleValue(L"/" + selected->Id,
            nullptr,
            ref new FBJsonClassFactory([](String^ JsonText) -> Object^
        {
            return FBPageBindable::FromJson(JsonText);
        }));

        create_task(val->GetAsync())
        .then([this](FBResult^ result)
        {
            if (result->Succeeded)
            {
                _selectedItem = static_cast<FBPageBindable^>(result->Object);
                NotifyPropertyChanged("SelectedItem");
            }
        });
    }
}

void UserLikes::NotifyPropertyChanged(
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

