---
layout: default
title: Dialogs
---

##Feed Dialog
The feed dialog allows the app to specify a title, link, and description for a post to the user's feed. The user can enter a custom message before posting it to Facebook.
These dialogs work.

C#:
{% highlight csharp %}
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
{% endhighlight %}

C++:
{% highlight c++ %}
// Get active session
FBSession^ sess = FBSession::ActiveSession;
if (sess->LoggedIn)
{
       // Set caption, link and description parameters
       PropertySet^ parameters = ref new PropertySet();
       parameters->Insert(L"caption", L"Microsoft");
       parameters->Insert(L"link", L"https://www.microsoft.com/en-us/default.aspx");
       parameters->Insert(L"description", L"Microsoft home page");
       // Display feed dialog
       create_task(sess->ShowFeedDialog(parameters)).then([=](FBResult^ result)
       {
              if (result->Succeeded)
              {
                     // Posting succeeded
              }
              else
              {
                     // Posting failed
              }
       });
}
{% endhighlight %}

##Request Dialog
The user can invite his/her Facebook friends to the app through the request dialog. Note that your app must be set to type 'Game' when registering with Facebook in order for the request dialog to work.

C#:
{% highlight csharp %}
// Get active session
FBSession sess = FBSession.ActiveSession;
 
if (sess.LoggedIn)
{
	// Set parameters
	PropertySet parameters = new PropertySet();
	// Set message
	parameters.Add("message", "Try this sample.");
	// Display feed dialog
	FBResult fbresult = await sess.ShowRequestsDialogAsync(parameters);
	if (fbresult.Succeeded)
	{
		// Requests sent
	}
	else
	{
		// Sending requests failed
	}
}
{% endhighlight %}

C++:
{% highlight c++ %}
// Get active session
FBSession^ sess = FBSession::ActiveSession;
if (sess->LoggedIn)
{
	// Set parameters
    PropertySet^ parameters = ref new PropertySet();
	// Set message
    parameters->Insert(L"message", L"Try this sample.");
    // Display requests dialog
   	create_task(sess->ShowRequestsDialog(parameters)).then([=](FBResult^ result)
    {
        if (result->Succeeded)
        {
       		// Requests sent
        }
        else
        {
       		// Sending requests failed
        }
    });
}
{% endhighlight %}

##Send Dialog
Using the send dialog, a user can send private messages to any of his/her facebook friends with a predetermined link specified.

C#:
{% highlight csharp %}
//Get active session
FBSession sess = FBSession.ActiveSession;

if (sess.LoggedIn)
{
	PropertySet parameters = new PropertySet();
	parameters.Add("link", "https://www.microsoft.com/en-us/default.aspx");
	//Display send dialog
	FBResult fbresult = await sess.ShowSendDialogAsync(parameters);
	if (fbresult.Succeeded)
	{
	    //message successfully sent
	}
	else
	{
	    //message failed to send
	}
}
{% endhighlight %}

C++
{% highlight c++ %}
//Get active session
FBSession^ sess = FBSession::ActiveSession;
if(sess->LoggedIn)
{
	PropertySet^ parameters = ref new PropertySet();
	parameters->Insert(L"link",L"https://www.microsoft.com/en-us/default.aspx");
	//Display send dialog
	create_task(sess->ShowSendDialog(parameters)).then([=](FBResult^ result))
	{
		if(result->Succeeded)
		{
			//message successfully sent
		}
		else
		{
			//message failed to send
		}
	});
}
{% endhighlight %}