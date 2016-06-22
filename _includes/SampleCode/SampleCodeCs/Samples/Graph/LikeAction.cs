using winsdkfb;
using winsdkfb.Graph;
 ...

// Get active session
FBSession sess = FBSession.ActiveSession;
if(sess.LoggedIn)
{
	//Get current user
	FBUser user = sess.User;

// Set parameters
PropertySet parameters = new PropertySet();
// Set Uri to like
parameters.Add("object", "https://www.microsoft.com/en-us/default.aspx");

	// Set Graph api path
	string path = user.Id + "/og.likes";

var factory = new FBJsonClassFactory(s => {
    return JsonConvert.DeserializeObject<FBReturnObject>(s);
});

var singleValue = new FBSingleValue(path, parameters, factory);
var result = await singleValue.PostAsync();
    if (result.Succeeded)
    {
        var response = result.Object as FBReturnObject;
    }
    else
    {
        // Posting failed
    }
}
...
public class FBReturnObject
{
    public string Id { get; set; }
    public string Post_Id { get; set; }
}
