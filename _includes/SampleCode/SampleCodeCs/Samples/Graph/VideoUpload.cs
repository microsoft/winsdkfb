using winsdkfb;
using winsdkfb.Graph;
...
var fop = new FileOpenPicker();
fop.ViewMode = PickerViewMode.Thumbnail;
fop.SuggestedStartLocation = PickerLocationId.PicturesLibrary;
fop.FileTypeFilter.Add(".mp4");
fop.FileTypeFilter.Add(".jpeg");
fop.FileTypeFilter.Add(".png");

var storageFile = await fop.PickSingleFileAsync();
var stream = await storageFile.OpenReadAsync();
var mediaStream = new FBMediaStream(storageFile.Name, stream);

FBSession sess = FBSession.ActiveSession;
if(sess.LoggedIn)
{
    var user = sess.User;
var parameters = new PropertySet();
parameters.Add("title", "Test video");
    parameters.Add("source", mediaStream);
    string path = "/" + user.Id + "/videos";

var factory = new FBJsonClassFactory(s => {
    return JsonConvert.DeserializeObject<FBReturnObject>(s);
});

var singleValue = new FBSingleValue(path, parameters, factory);
var result = await singleValue.PostAsync();
    if (result.Succeeded)
    {
        var photoResponse = result.Object as FBReturnObject;
    }
}
...
public class FBReturnObject
{
    public string Id { get; set; }
    public string Post_Id { get; set; }
}
