//
// Dialogs.xaml.cpp
// Implementation of the Dialogs class
//

#include "pch.h"
#include "Dialogs.xaml.h"

using namespace LoginCpp;

using namespace concurrency;
using namespace Facebook;
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

Dialogs::Dialogs()
{
	InitializeComponent();
}


void LoginCpp::Dialogs::FeedDialogButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    FBSession^ s = FBSession::ActiveSession;
    PropertySet^ params = ref new PropertySet();
    params->Insert(L"caption", L"I love Brussels Sprouts!");
    params->Insert(L"link", L"https://en.wikipedia.org/wiki/Brussels_sprout");
    params->Insert(L"description", L"Om Nom Nom!");

    create_task(s->ShowFeedDialog(params))
        .then([=](FBResult^ Response)
    {
        OutputDebugString(L"Showed 'Feed' dialog.\n");
    });
}


void LoginCpp::Dialogs::AppRequestsButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    FBSession^ s = FBSession::ActiveSession;
    PropertySet^ params = ref new PropertySet();

    params->Insert(L"title", L"I love Brussels Sprouts!");
    params->Insert(L"message", L"Om Nom Nom!");

    create_task(s->ShowRequestsDialog(params))
        .then([=](FBResult^ Response)
    {
        OutputDebugString(L"Showed 'Requests' dialog.\n");
    });
}
