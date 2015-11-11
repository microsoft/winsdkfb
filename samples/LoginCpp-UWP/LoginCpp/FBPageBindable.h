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


#pragma once

namespace LoginCpp
{
	[Windows::Foundation::Metadata::WebHostHidden]
    [Windows::UI::Xaml::Data::Bindable]
    public ref class FBPageBindable sealed :
        Windows::UI::Xaml::Data::INotifyPropertyChanged
    {
        public:
            virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler^ 
                PropertyChanged;

            static Object^ FromJson(
                Platform::String^ JsonText 
                );

            property Platform::String^ Id
            {
                Platform::String^ get();
                void set(Platform::String^ value);
            }

            property Platform::String^ Category
            {
                Platform::String^ get();
                void set(Platform::String^ value);
            }

            property int Checkins
            {
                int get();
                void set(int value);
            }

            property Platform::String^ Description
            {
                Platform::String^ get();
                void set(Platform::String^ value);
            }

            property int Likes
            {
                int get();
                void set(int value);
            }

            property Platform::String^ Link
            {
                Platform::String^ get();
                void set(Platform::String^ value);
            }

            property Platform::String^ Name
            {
                Platform::String^ get();
                void set(Platform::String^ value);
            }

        private:
            FBPageBindable(
                winsdkfb::Graph::FBPage^ page
                );

            void NotifyPropertyChanged(
                Platform::String^ prop
                );
            
            winsdkfb::Graph::FBPage^ _page;
    };
}



