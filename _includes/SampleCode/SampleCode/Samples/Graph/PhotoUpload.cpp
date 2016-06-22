#include "pch.h"
using namespace winsdkfb;
using namespace winsdkfb::Graph;
...
StorageFile^ selectedPhoto;
// Read image file into selectedPhoto
...

// Create media stream
create_task(selectedPhoto->OpenReadAsync())
.then([this, selectedPhoto](IRandomAccessStreamWithContentType^ stream)
{
    FBMediaStream^ fbStream = ref new FBMediaStream(selectedPhoto->Name, stream);

    // Get active session
    FBSession^ sess = FBSession::ActiveSession;
    if (sess->LoggedIn)
    {

        PropertySet^ parameters = ref new PropertySet();
        // Set media stream
        parameters->Insert("source", fbStream);

        // Create Graph API path
        String^ graphPath = sess->User->Id + L"/photos";

        // Create a json class factory with a class (FBReturnObject class) that
        // can receive and parse the json response returned
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
});
