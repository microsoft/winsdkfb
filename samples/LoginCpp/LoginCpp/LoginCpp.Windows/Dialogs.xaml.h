//
// Dialogs.xaml.h
// Declaration of the Dialogs class
//

#pragma once

#include "Dialogs.g.h"

namespace LoginCpp
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class Dialogs sealed
	{
	public:
		Dialogs();
    private:
        void FeedDialogButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void AppRequestsButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}
