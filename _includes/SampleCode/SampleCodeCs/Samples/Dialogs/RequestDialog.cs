using System;
using Windows.Foundation.Collections;
using winsdkfb;

namespace SampleCode
{
    public static partial class SampleFunc
    {
        static async void ShowRequestDialog()
        {
            // Get active session
            FBSession sess = FBSession.ActiveSession;

            if (sess.LoggedIn)
            {
                // Set parameters
                PropertySet parameters = new PropertySet();
                // Set message
                parameters.Add("message", "Try this sample.");
                // Display feed dialog
                FBResult fbresult = await sess.ShowRequestsDialogAsync(parameters);
                if (fbresult.Succeeded)
                {
                    // Requests sent
                }
                else
                {
                    // Sending requests failed
                }
            }
        }
    }
}