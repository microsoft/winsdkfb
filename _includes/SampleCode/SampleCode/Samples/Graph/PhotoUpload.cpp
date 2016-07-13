#include "pch.h"
#include "FBReturnObject.h"

using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::Storage::Pickers;
using namespace Platform;
using namespace concurrency;
using namespace winsdkfb;
using namespace winsdkfb::Graph;

namespace SampleCode
{
    void UploadPhoto()
    {
        StorageFile^ selectedPhoto;
        // Read image file into selectedPhoto
        FileOpenPicker^ fop = ref new FileOpenPicker();
        fop->ViewMode = PickerViewMode::Thumbnail;
        fop->SuggestedStartLocation = PickerLocationId::PicturesLibrary;
        fop->FileTypeFilter->Append(L".jpg");
        fop->FileTypeFilter->Append(L".png");
        create_task(fop->PickSingleFileAsync()).then([&](StorageFile^ storageFile)
        {
            selectedPhoto = storageFile;
            return storageFile->OpenReadAsync();
        }).then([=, &selectedPhoto](IRandomAccessStreamWithContentType^ stream)
        {
            // Create media stream
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
        });
    }
}
