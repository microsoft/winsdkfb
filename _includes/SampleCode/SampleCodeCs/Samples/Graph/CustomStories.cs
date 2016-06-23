using System;
using Windows.Foundation.Collections;
using Newtonsoft.Json;
using winsdkfb;
using winsdkfb.Graph;

namespace SampleCode
{
    public static partial class SampleFunc
    {
        static async void CustomStory()
        {
            // Get active session
            FBSession sess = FBSession.ActiveSession;
            if (sess.LoggedIn)
            {
                // Create custom story with action:try and object:scenario
                // This will be published to Facebook as:
                // <user> tried <a scenario> from Sample Application

                // Set parameters for custom story
                PropertySet parameters = new PropertySet();
                // Set object type parameter
                // Object type: scenario
                string customObjectInstance = "{" +
                    "\"title\":\"Custom Story\"" +
                    "}";
                parameters.Add("scenario", customObjectInstance);
                // Get current user
                FBUser user = sess.User;
                // Set Graph api path for custom story (action:try)
                string path = user.Id + "/fbsdk_sample_app:try";
                var factory = new FBJsonClassFactory(s =>
                {
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

