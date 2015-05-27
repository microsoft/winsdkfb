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
// MainPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "MainPage.g.h"
#include "TestRunner.h"

namespace SDKCppUnit
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    [Windows::UI::Xaml::Data::Bindable]
    public ref class MainPage sealed : Windows::UI::Xaml::Data::INotifyPropertyChanged
	{
	public:
		MainPage(
            );

        virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler^ PropertyChanged;

        property Windows::Foundation::Collections::IVector<Platform::String^>^ Items
        {
            Windows::Foundation::Collections::IVector<Platform::String^>^ get();
        }

        void OnTestSuiteStarted(
            Platform::String^ SuiteName
            );
        
        void OnTestSuiteCompleted(
            Platform::String^ SuiteName
            );

        void OnTestCaseStarted(
            Platform::String^ TestCaseName
            );

        void OnTestCaseCompleted(
            Platform::String^ TestCaseName,
            bool TestSucceeded
            );

        property int TestsPassed
        {
            int get();
        }

        property int TestsFailed
        {
            int get();
        }

    private:
        void OK_Click(
            Platform::Object^ sender, 
            Windows::UI::Xaml::RoutedEventArgs^ e
            );

        void NotifyPropertyChanged(
            Platform::String^ prop
            );

        void AddItem(
            Platform::String^ Item
            );

        TestRunner^ _testRunner;
        int _testsPassed;
        int _testsFailed;
        Platform::Collections::Vector<Platform::String^>^ _items;
    };
}
