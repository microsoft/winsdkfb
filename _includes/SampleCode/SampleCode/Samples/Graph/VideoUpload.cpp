#include "pch.h"
#include "FBReturnObject.h"

using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::Storage::Streams;
using namespace Windows::Data::Json;
using namespace Windows::Foundation::Collections;
using namespace concurrency;
using namespace Platform;
using namespace winsdkfb;
using namespace winsdkfb::Graph;

namespace SampleCode
{
    void UploadVideo()
    {
        FileOpenPicker^ fop = ref new FileOpenPicker();
        fop->ViewMode = PickerViewMode::Thumbnail;
        fop->SuggestedStartLocation = PickerLocationId::VideosLibrary;
        fop->FileTypeFilter->Append(L".mp4");
        StorageFile^ storageFile;

        create_task(fop->PickSingleFileAsync()).then([=, &storageFile](StorageFile^ tempStorageFile)
        {
            storageFile = tempStorageFile;
            return tempStorageFile->OpenReadAsync();
        }).then([=](IRandomAccessStreamWithContentType^ stream)
        {
            FBMediaStream^ mediaStream = ref new FBMediaStream(storageFile->Name, stream);
            FBSession^ sess = FBSession::ActiveSession;
            if (sess->LoggedIn)
            {
                FBUser^ user = sess->User;
                PropertySet^ parameters = ref new PropertySet();
                parameters->Insert(L"title", L"Test video");
                parameters->Insert(L"source", mediaStream);
                String^ path = L"/" + user->Id + L"/videos";

                FBJsonClassFactory^ factory = ref new FBJsonClassFactory([](String^ s)
                {
                    JsonObject^ jsonObject = JsonObject::Parse(s);
                    auto returnObject = ref new FBReturnObject();
                    returnObject->Id = jsonObject->GetNamedString(L"id");
                    returnObject->Post_Id = jsonObject->GetNamedString(L"post_id");
                    return returnObject;
                });

                FBSingleValue^ singleValue = ref new FBSingleValue(path, parameters, factory);
                return create_task(singleValue->PostAsync());
            }
            return create_task([]()
            {
                return ref new FBResult(ref new FBError(0, L"Not logged in", L"Log in first"));
            });
        }).then([=](FBResult^ result)
        {
            if (result->Succeeded)
            {
                FBReturnObject^ response = static_cast<FBReturnObject^>(result->Object);
            }
        }); 
    }
}

