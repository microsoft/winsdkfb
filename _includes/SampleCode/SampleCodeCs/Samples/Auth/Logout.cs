using System;
using winsdkfb;

namespace SampleCode
{
    public static partial class SampleFunc
    {
        static async void Logout()
        {
            FBSession sess = FBSession.ActiveSession;
            await sess.LogoutAsync();
        }
    }
}
