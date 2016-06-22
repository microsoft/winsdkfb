#include "pch.h"
#include "FBReturnObject.h"

using namespace Windows::Foundation::Collections;
using namespace Platform;
using namespace concurrency;
using namespace winsdkfb;
using namespace winsdkfb::Graph;

namespace SampleCode
{
    void LikeAction()
    {
        // Get active session
        FBSession^ sess = FBSession::ActiveSession;
        if (sess->LoggedIn)
        {
            // Set parameters
            PropertySet^ parameters = ref new PropertySet();
            // Set Uri to like
            parameters->Insert("object", L"https://www.microsoft.com/en-us/default.aspx");

            // Create Graph API path
            String^ graphPath = sess->User->Id + L"/og.likes";

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
