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

//
// UserLikes.xaml.h
// Declaration of the UserLikes class
//

#pragma once

#include "UserLikes.g.h"
#include "FBPageBindable.h"

namespace LoginCpp
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
    [Windows::UI::Xaml::Data::Bindable]
    public ref class UserLikes sealed : Windows::UI::Xaml::Data::INotifyPropertyChanged 
	{
	public:
		UserLikes(
            );

        virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler^ PropertyChanged;

        property Windows::Foundation::Collections::IVector<FBPageBindable^>^ Items
        {
            Windows::Foundation::Collections::IVector<FBPageBindable^>^ get();
        }

        property FBPageBindable^ SelectedItem
        {
            FBPageBindable^ get();
        }

    private:
        void GetUserLikes(
            );

        void AddLikes(
            Windows::Foundation::Collections::IVectorView<Object^>^ NewLikes
            );

        void NotifyPropertyChanged(
            Platform::String^ prop
            );

        Facebook::Graph::FBPaginatedArray^ _likes;
        FBPageBindable^ _selectedItem;
        Platform::Collections::Vector<FBPageBindable^>^ _listViewItems;
        void ListView_SelectionChanged(
            Platform::Object^ sender, 
            Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e
            );
    };
}
