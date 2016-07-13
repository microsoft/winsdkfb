#include "pch.h"
#include "FBReturnObject.h"

using namespace Windows::Foundation::Collections;
using namespace Platform;
using namespace concurrency;
using namespace winsdkfb;
using namespace winsdkfb::Graph;


namespace SampleCode
{
    // Get active session
    void GraphFeedPost()
    {
        FBSession^ sess = FBSession::ActiveSession;
        if (sess->LoggedIn)
        {
            // Set caption, link and description parameters
            PropertySet^ parameters = ref new PropertySet();
            parameters->Insert(L"caption", L"Microsoft");
            parameters->Insert(L"link", L"https://www.microsoft.com/en-us/default.aspx");
            parameters->Insert(L"description", L"Microsoft home page");

            // Add message
            parameters->Insert(L"message", L"Posting from my Universal Windows app.");

            //Create Graph API path
            String^ graphPath = sess->User->Id + L"/feed";

            // Create a json class factory with a class (FBReturnObject class)
            // that can receive and parse the json response returned
            FBJsonClassFactory^ fact = ref new FBJsonClassFactory([](String^ JsonText) ->
                Object^
            {
                auto returnObject = ref new FBReturnObject();
                returnObject->Id = Windows::Data::Json::JsonObject::Parse(JsonText)->GetNamedString("id");
                return returnObject;
            });

            FBSingleValue^ sval = ref new FBSingleValue(graphPath, parameters, fact);
            create_task(sval->PostAsync()).then([=](FBResult^ result)
            {
                if (result->Succeeded)
                {
                    FBReturnObject^ response = static_cast<FBReturnObject ^>(result->Object);
                }
                else
                {
                    // Posting failed
                }
            });

        }
    }
}
