using System;
using Windows.Foundation.Collections;
using Windows.Storage;
using Windows.Storage.Streams;
using Windows.Storage.Pickers;
using Newtonsoft.Json;
using winsdkfb;
using winsdkfb.Graph;

namespace SampleCode
{
    public static partial class SampleFunc
    {
        static async void PhotoUpload()
        {
            // Read image file into selectedPhoto
            var fop = new FileOpenPicker();
            fop.ViewMode = PickerViewMode.Thumbnail;
            fop.SuggestedStartLocation = PickerLocationId.PicturesLibrary;
            fop.FileTypeFilter.Add(".jpg");
            fop.FileTypeFilter.Add(".png");
            StorageFile selectedPhoto = await fop.PickSingleFileAsync();
            // Create media stream
            IRandomAccessStreamWithContentType stream = await selectedPhoto.OpenReadAsync();
            FBMediaStream fbStream = new FBMediaStream(selectedPhoto.Name, stream);

            // Get current session
            FBSession sess = FBSession.ActiveSession;
            if (sess.LoggedIn)
            {
                // Get current user
                FBUser user = sess.User;

                PropertySet parameters = new PropertySet();
                // Set media stream
                parameters.Add("source", fbStream);

                // Set Graph api path
                string path = "/" + user.Id + "/photos";

                var factory = new FBJsonClassFactory(s => {
                    return JsonConvert.DeserializeObject<FBReturnObject>(s);
                });

                var singleValue = new FBSingleValue(path, parameters, factory);
                var result = await singleValue.PostAsync();
                if (result.Succeeded)
                {
                    var response = result.Object as FBReturnObject;
                }
                else
                {
                    // Posting failed
                }
            }
        }
    }
}