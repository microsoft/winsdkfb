// Get active session
FBSession sess = FBSession.ActiveSession;

if (sess.LoggedIn)
{
	// Set caption, link and description parameters
	PropertySet parameters = new PropertySet();
parameters.Add("title", "Microsoft");
	parameters.Add("link", "https://www.microsoft.com/en-us/default.aspx");
	parameters.Add("description", "Microsoft home page");
	//Display feed dialog
	FBResult fbresult = await sess.ShowFeedDialogAsync(parameters);
	if (fbresult.Succeeded)
	{
		//Posting succeeded
	}
	else
	{
		//Posting failed
	}
}
