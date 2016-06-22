#include "pch.h"
using namespace winsdkfb;
using namespace winsdkfb::Graph;
...

// Get active session
FBSession^ sess = FBSession::ActiveSession;
if (sess->LoggedIn)
{
    // Create custom story with action:try and object:scenario
    // This will be published to Facebook as :
    // <user> tried <a scenario> from Sample Application

    // Set parameters for custom story
    PropertySet^ parameters = ref new PropertySet();
    // Set object type parameter
    // Object type: scenario
    String^ customObjectInstance = "{" +
        "\"type\":\"fbsdk_sample_app:scenario\"," + "\"title\":\"Custom Story\"" +
        "}";

    parameters->Insert("scenario", customObjectInstance);

    // Get current user
    FBUser^ user = sess->User;

    // Set Graph api path for custom story (action:try)
    String^ path = user->Id + L"/fbsdk_sample_app:try";

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
    create_task(sval->PostAsync()).then([this](FBResult^ result)
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
