using winsdkfb;
...
FBSession sess = FBSession.ActiveSession;
sess.FBAppID = "<Facebook App ID>";
sess.WinAppId = "Windows or Windows Phone Store ID depending on target device";
