---
layout: default
title: Graph API
---

## Post to Feed
The app should have publish_actions permission granted by the user.
A class will have to be created (`FBReturnObject` in this example) to receive and parse the json response. The response will be an id of type string `{"id": "<id here>"}` if published successfully.
You can find more details [here](https://developers.facebook.com/docs/graph-api/reference/v2.3/user/feed).

C#:

{% highlight csharp %}
using winsdkfb;
using winsdkfb.Graph;
…

// Get active session
FBSession sess = FBSession.ActiveSession;
// If the user is logged in
if(sess.LoggedIn)
{
	// Get current user
	FBUser user = sess.User;

	// Set caption, link and description parameters
	PropertySet parameters = new PropertySet();
	parameters.Add("title", "Microsoft");
	parameters.Add("link", "https://www.microsoft.com/en-us/default.aspx");
	parameters.Add("description", "Microsoft home page");
	// Add post message
	parameters.Add("message", "Posting from my Universal Windows app.");

	// Set Graph api path
	string path = "/" + user.Id + "/feed";

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
{% endhighlight %}

C++:

{% highlight c++ %}
using namespace winsdkfb;
using namespace winsdkfb::Graph;
…
// Get active session
FBSession^ sess = FBSession::ActiveSession;
if (sess->LoggedIn)
{
       // Set caption, link and description parameters
       PropertySet^ parameters = ref new PropertySet();
	   parameters->Insert(L"caption", L"Microsoft");
	   parameters->Insert(L"link",L"https://www.microsoft.com/en-us/default.aspx");
	   parameters->Insert(L"description",L"Microsoft home page");

	   // Add message
	   parameters->Insert(L"message",L"Posting from my Universal Windows app.");

	   //Create Graph API path
	   String^ graphPath = sess->User->Id + L"/feed";

	   // Create a json class factory with a class (FBReturnObject class)
	   // that can receive and parse the json response returned
     FBJsonClassFactory^ fact = ref new FBJsonClassFactory([](String^ JsonText) ->
     Object^
     {
           auto returnObject = ref new FBReturnObject();
           returnObject->Id = Windows::Data::Json::JsonObject::Parse(JsonText)->GetNamedString("id");
           return returnObject;
     });

     FBSingleValue^ sval = ref new FBSingleValue(graphPath, parameters, fact);
     create_task(sval->PostAsync()).then([this](FBResult^ result)
     {
         if (result->Succeeded)
         {
               FBReturnObject^ response = static_cast<FBReturnObject ^>(result->Object);
         }
         else
         {
               // Posting failed
         }
     });

}
{% endhighlight %}

## Custom Stories
Follow the steps [here](https://developers.facebook.com/docs/sharing/opengraph/custom) to configure custom stories in your app on developers.facebook.com
The app should have publish_actions permission granted by the user.
A class will have to be created (`FBReturnObject` in this example) to receive and parse the json response.
The response will be an id of type string `{"id":"<id here>"}` if published successfully.

For the snippet below,

App namespace: `fbsdk_sample_app`

A custom story was created with action type *try* and object type *scenario*.

We define a custom json object parameter `{"title": "Custom Story"}` and set the Graph API path to `userId/fbsdk_sample_app:try`.

This will be published as "*user* tried *scenario* from Sample Application".

C#:

{% highlight csharp %}
using winsdkfb;
using winsdkfb.Graph;
 ...

// Get active session
FBSession sess = FBSession.ActiveSession;

if(sess.LoggedIn)
{
	// Create custom story with action:try and object:scenario
	// This will be published to Facebook as:
	// <user> tried <a scenario> from Sample Application

	// Set parameters for custom story
	PropertySet parameters = new PropertySet();
	// Set object type parameter
	// Object type: scenario
	string customObjectInstance = "{" +
    "\"title\":\"Custom Story\"" +
    "}";

	 parameters.Add("scenario", customObjectInstance);

	// Get current user
	 FBUser user = sess.User;

	// Set Graph api path for custom story (action:try)
	string path = user.Id + "/fbsdk_sample_app:try";

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
{% endhighlight %}

C++

{% highlight c++ %}
using namespace winsdkfb;
using namespace winsdkfb::Graph;
 ...

// Get active session
FBSession^ sess = FBSession::ActiveSession;
if (sess->LoggedIn)
{
       // Create custom story with action:try and object:scenario
       // This will be published to Facebook as :
	   // <user> tried <a scenario> from Sample Application

       // Set parameters for custom story
       PropertySet^ parameters = ref new PropertySet();
       // Set object type parameter
       // Object type: scenario
       String^ customObjectInstance = "{" +
	    "\"type\":\"fbsdk_sample_app:scenario\","+ "\"title\":\"Custom Story\"" +
	    "}";

       parameters->Insert("scenario", customObjectInstance);

       // Get current user
       FBUser^ user = sess->User;

       // Set Graph api path for custom story (action:try)
       String^ path = user->Id + L"/fbsdk_sample_app:try";

       // Create a json class factory with a class (FBReturnObject class)
       // that can receive and parse the json response returned
       FBJsonClassFactory^ fact = ref new FBJsonClassFactory([](String^ JsonText) ->
       Object^
       {
           auto returnObject = ref new FBReturnObject();
           returnObject->Id = Windows::Data::Json::JsonObject::Parse(JsonText)->GetNamedString("id");
           return returnObject;
       });

       FBSingleValue^ sval = ref new FBSingleValue(graphPath, parameters, fact);
       create_task(sval->PostAsync()).then([this](FBResult^ result)
       {
         if (result->Succeeded)
         {
               FBReturnObject^ response = static_cast<FBReturnObject ^>(result->Object);
         }
         else
         {
               // Posting failed
         }
       });
}
{% endhighlight %}

## Upload a Photo
The app should have publish_actions permission granted by the user. A class will have to be created (`FBReturnObject` in this example) to receive and parse the json response. The response will be an id of type string `{"id": "<id here>"}` if published successfully.
You can find more details [here](https://developers.facebook.com/docs/graph-api/reference/user/photos/).

C#:

{% highlight csharp %}
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
{% endhighlight %}

C++:

{% highlight c++ %}
using namespace winsdkfb;
using namespace winsdkfb::Graph;
 ...
StorageFile^ selectedPhoto;
// Read image file into selectedPhoto
…

// Create media stream
create_task(selectedPhoto->OpenReadAsync())
.then([this, selectedPhoto](IRandomAccessStreamWithContentType^ stream)
{
       FBMediaStream^ fbStream = ref new FBMediaStream(selectedPhoto->Name, stream);

       // Get active session
       FBSession^ sess = FBSession::ActiveSession;
       if (sess->LoggedIn)
       {

              PropertySet^ parameters = ref new PropertySet();
              // Set media stream
              parameters->Insert("source", fbStream);

              // Create Graph API path
              String^ graphPath = sess->User->Id + L"/photos";

              // Create a json class factory with a class (FBReturnObject class) that
			       // can receive and parse the json response returned
            FBJsonClassFactory^ fact = ref new FBJsonClassFactory([](String^ JsonText) ->
            Object^
            {
             auto returnObject = ref new FBReturnObject();
             returnObject->Id = Windows::Data::Json::JsonObject::Parse(JsonText)->GetNamedString("id");
             return returnObject;
            });

          FBSingleValue^ sval = ref new FBSingleValue(graphPath, parameters, fact);
          create_task(sval->PostAsync()).then([this](FBResult^ result)
          {
            if (result->Succeeded)
            {
                 FBReturnObject^ response = static_cast<FBReturnObject ^>(result->Object);
            }
            else
            {
                 // Posting failed
            }
          });
      }
});
{% endhighlight %}

## Upload a Video (non-resumable)
The app should have publish_actions permission granted by the user. A class will have to be created (`FBReturnObject` in this example) to receive and parse the json response. The response will be an id of type string `{"id":"<id here>"}` if published successfully. Note that this is for uploading a small sized video all at once (non-resumable). The Facebook Graph API has said non-resumable upload supports videos up to 1GB and 20 minutes long.


C#:

{% highlight csharp %}
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
{% endhighlight %}


## Like Action
The app should have publish_actions permission granted by the user. A class will have to be created (`FBReturnObject` in this example) to receive and parse the json response. The response will be an id of type string `{"id":"<id here>"}` if published successfully.

Note that this is not the same as 'liking' a Facebook Page. If successful, the like action will be published onto the user's activity feed. You need extra permission to post it as an Open Graph object on the user's timeline/news feed. You can find more details [here](https://developers.facebook.com/docs/opengraph/guides/og.likes).

C#:

{% highlight csharp %}
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
{% endhighlight %}

C++:

{% highlight c++ %}
using namespace winsdkfb;
using namespace winsdkfb::Graph;
...

// Get active session
FBSession^ sess = FBSession::ActiveSession;
if (sess->LoggedIn)
{
    // Set parameters
    PropertySet^ parameters = ref new PropertySet();
	// Set Uri to like
    parameters->Insert("object", L"https://www.microsoft.com/en-us/default.aspx");

    // Create Graph API path
    String^ graphPath = sess->User->Id + L"/og.likes";

    // Create a json class factory with a class (FBReturnObject class)
    // that can receive and parse the json response returned
         FBJsonClassFactory^ fact = ref new FBJsonClassFactory([](String^ JsonText) ->
     Object^
     {
           auto returnObject = ref new FBReturnObject();
           returnObject->Id = Windows::Data::Json::JsonObject::Parse(JsonText)->GetNamedString("id");
           return returnObject;
     });

     FBSingleValue^ sval = ref new FBSingleValue(graphPath, parameters, fact);
     create_task(sval->PostAsync()).then([this](FBResult^ result)
     {
         if (result->Succeeded)
         {
               FBReturnObject^ response = static_cast<FBReturnObject ^>(result->Object);
         }
         else
         {
               // Posting failed
         }
     });
}
{% endhighlight %}
<br />