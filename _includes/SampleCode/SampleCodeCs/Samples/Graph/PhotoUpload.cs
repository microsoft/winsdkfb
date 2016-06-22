using winsdkfb;
using winsdkfb.Graph;
 ...

StorageFile selectedPhoto;
// Read image file into selectedPhoto
…

// Create media stream
IRandomAccessStreamWithContentType stream = await selectedPhoto.OpenReadAsync();
FBMediaStream fbStream = new FBMediaStream(selectedPhoto.Name, stream);

// Get current session
FBSession sess = FBSession.ActiveSession;
if(sess.LoggedIn)
{
    // Get current user
    FBUser user = sess.User;

PropertySet parameters = new PropertySet();
// Set media stream
parameters.Add("source", fbStream);

    // Set Graph api path
    string path = "/" + user.Id + "/photos";

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
