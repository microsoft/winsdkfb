// Get active session
FBSession sess = FBSession.ActiveSession;
if(sess.LoggedIn)
{
	// Get current user
	FBUser user = sess.User;
string userId = user.Id;
string username = user.Name;
string locale = user.Locale;
}
