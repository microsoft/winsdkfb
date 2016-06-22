// Get active session
FBSession sess = FBSession.ActiveSession;

if(sess.LoggedIn)
{
	// Get current user
	FBUser user = sess.User;
// Set profile pic
ProfilePic.UserId = user.Id;
}
