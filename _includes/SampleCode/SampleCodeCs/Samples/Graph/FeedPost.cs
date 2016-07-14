using System;
using Windows.Foundation.Collections;
using Newtonsoft.Json;
using winsdkfb;
using winsdkfb.Graph;

namespace SampleCode
{
    public static partial class SampleFunc
    {
        static async void FeedPost()
        {
            // Get active session
            FBSession sess = FBSession.ActiveSession;
            // If the user is logged in
            if (sess.LoggedIn)
            {
                // Get current user
                FBUser user = sess.User;
                // Set caption, link and description parameters
                PropertySet parameters = new PropertySet();
                parameters.Add("title", "Microsoft");
                parameters.Add("link", "https://www.microsoft.com/en-us/default.aspx");
                parameters.Add("description", "Microsoft home page");
                // Add post message
                parameters.Add("message", "Posting from my Universal Windows app.");
                // Set Graph api path
                string path = "/" + user.Id + "/feed";
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
