//
// OptionsPage.xaml.h
// Declaration of the OptionsPage class
//

#pragma once

#include "OptionsPage.g.h"

namespace LoginCpp
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class OptionsPage sealed
	{
	public:
		OptionsPage();

    private:
        void userInfo_OnClicked(
            Platform::Object^ sender, 
            Windows::UI::Xaml::RoutedEventArgs^ e
            );

        void dialogs_OnClicked(
            Platform::Object^ sender, 
            Windows::UI::Xaml::RoutedEventArgs^ e
            );
    };
}
