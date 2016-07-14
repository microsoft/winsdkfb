using winsdkfb;
using winsdkfb.Graph;

namespace SampleCode
{
    public static partial class SampleFunc
    {
        static void UserInformation()
        {
            // Get active session
            FBSession sess = FBSession.ActiveSession;
            if (sess.LoggedIn)
            {
                // Get current user
                FBUser user = sess.User;
                string userId = user.Id;
                string username = user.Name;
                string locale = user.Locale;
            }
        }
    }
}