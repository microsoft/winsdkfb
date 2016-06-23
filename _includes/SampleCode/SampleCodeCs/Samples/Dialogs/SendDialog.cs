using System;
using Windows.Foundation.Collections;
using winsdkfb;

namespace SampleCode
{
    public static partial class SampleFunc
    {
        static async void ShowSendDialog()
        {
            //Get active session
            FBSession sess = FBSession.ActiveSession;

            if (sess.LoggedIn)
            {
                PropertySet parameters = new PropertySet();
                parameters.Add("link", "https://www.microsoft.com/en-us/default.aspx");
                //Display send dialog
                FBResult fbresult = await sess.ShowSendDialogAsync(parameters);
                if (fbresult.Succeeded)
                {
                    //message successfully sent
                }
                else
                {
                    //message failed to send
                }
            }
        }
    }
}