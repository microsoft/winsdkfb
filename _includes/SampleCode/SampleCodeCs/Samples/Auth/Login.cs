// Get active session
FBSession sess = FBSession.ActiveSession;

// Add permissions required by the app
List<String> permissionList = new List<String>();
permissionList.Add("public_profile");
permissionList.Add("user_friends");
permissionList.Add("user_likes");
permissionList.Add("user_groups");
permissionList.Add("user_location");
permissionList.Add("user_photos");
permissionList.Add("publish_actions");
FBPermissions permissions = new FBPermissions(permissionList);

// Login to Facebook
FBResult result = await sess.LoginAsync(permissions);

if (result.Succeeded)
{
	//Login successful
}
else
{
	//Login failed
}
