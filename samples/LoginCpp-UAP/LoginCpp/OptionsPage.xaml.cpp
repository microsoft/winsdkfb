//
// OptionsPage.xaml.cpp
// Implementation of the OptionsPage class
//

#include "pch.h"
#include "OptionsPage.xaml.h"

using namespace LoginCpp;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

OptionsPage::OptionsPage()
{
	InitializeComponent();
}

void OptionsPage::userInfo_OnClicked(
    Platform::Object^ sender, 
    Windows::UI::Xaml::RoutedEventArgs^ e)
{
}

void OptionsPage::dialogs_OnClicked(
    Platform::Object^ sender, 
    Windows::UI::Xaml::RoutedEventArgs^ e
    )
{
}
