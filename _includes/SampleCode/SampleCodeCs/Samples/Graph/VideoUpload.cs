using System;
using Windows.Foundation.Collections;
using Windows.Storage.Pickers;
using Newtonsoft.Json;
using winsdkfb;
using winsdkfb.Graph;

namespace SampleCode
{
    public static partial class SampleFunc
    {
        static async void VideoUpload()
        {
            var fop = new FileOpenPicker();
            fop.ViewMode = PickerViewMode.Thumbnail;
            fop.SuggestedStartLocation = PickerLocationId.VideosLibrary;
            fop.FileTypeFilter.Add(".mp4");

            var storageFile = await fop.PickSingleFileAsync();
            var stream = await storageFile.OpenReadAsync();
            var mediaStream = new FBMediaStream(storageFile.Name, stream);

            FBSession sess = FBSession.ActiveSession;
            if (sess.LoggedIn)
            {
                var user = sess.User;
                var parameters = new PropertySet();
                parameters.Add("title", "Test video");
                parameters.Add("source", mediaStream);
                string path = "/" + user.Id + "/videos";

                var factory = new FBJsonClassFactory(s => {
                    return JsonConvert.DeserializeObject<FBReturnObject>(s);
                });

                var singleValue = new FBSingleValue(path, parameters, factory);
                var result = await singleValue.PostAsync();
                if (result.Succeeded)
                {
                    var photoResponse = result.Object as FBReturnObject;
                }
            }
        }
    }
}