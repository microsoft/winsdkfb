using winsdkfb;

namespace SampleCode
{
    public static partial class SampleFunc
    {
        static void Initialization()
        {
            FBSession sess = FBSession.ActiveSession;
            sess.FBAppId = "<Facebook App ID>";
            sess.WinAppId = "Windows or Windows Phone Store ID depending on target device";
        }
    }
}
