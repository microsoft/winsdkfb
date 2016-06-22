---
layout: default
title: Authentication
---

<script type="text/javascript" src="../js/language_toggle.js"></script>
<script type="text/javascript">
    window.onload = function() {
        var radios = document.language_select_form.lang_select;
        for (var i = 0; i < radios.length; ++i) {
            radios[i].onclick = function() {
                run_language_pref_update(this.value);
            };
        }
        run_default_for_page_load();
    };
</script>

<form name="language_select_form">
    <label>Show documentation for language:</label>
    <br />
    <label>C++</label>
    <input type="radio" name="lang_select" value="cpp" checked="checked" />
    <label>C#</label>
    <input type="radio" name="lang_select" value="c_sharp" />
</form>

## Initializing the Facebook Session
Set the Facebook App ID and Windows Store ID values in active session:

{% highlight csharp %}
using winsdkfb;
...
FBSession sess = FBSession.ActiveSession;
sess.FBAppID = "<Facebook App ID>";
sess.WinAppId = "Windows or Windows Phone Store ID depending on target device";
{% endhighlight %}

{% highlight c++ %}
using namespace winsdkfb;
...
FBSession^ sess = FBSession::ActiveSession;
sess->FBAppId = "<Facebook App ID>";
sess->WinAppId = "<Windows or Windows Phone Store ID depending on the target device";
{% endhighlight %}

Note: During development, you can always use the PhoneProductID from the manifest to get the WinAppId instead of having one Windows Store ID and one Windows Phone Store ID.For a published app you would need the package SID.

## Login
Use the following code snippet to login to Facebook.
The sess.LoginAsync() or sess->LoginAsync() call launches the Facebook login dialog box for the user to enter his/her username and password.

{% highlight csharp %}
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
{% endhighlight %}

{% highlight c++ %}
// Get active session
FBSession^ sess = FBSession::ActiveSession;

// Add permissions required by the app
Vector<String^>^ permissionList = ref new Vector<String^>(); ("public_profile");
permissionList->Append(L"public_profile");
permissionList->Append(L"user_friends");
permissionList->Append(L"user_likes");
permissionList->Append(L"user_groups");
permissionList->Append(L"user_location");
permissionList->Append(L"user_photos");
permissionList->Append(L"publish_actions");
FBPermissions^ permissions = ref new FBPermissions(permissionList->GetView());

// Login to Facebook
create_task(sess->LoginAsync(permissions)).then([=](FBResult^ result)
{
       if (result->Succeeded)
       {
       // Login succeeded
…
       }
       else
       {
       // Login failed
…
       }
});
{% endhighlight %}

## Logout
This is simply just calling the LogoutAsync() method.

{% highlight csharp %}
FBSession sess = FBSession.ActiveSession;
await sess.LogoutAsync();
{% endhighlight %}

{% highlight c++ %}
FBSession^ sess = FBSession::ActiveSession;
sess->LogoutAsync();
{% endhighlight %}

## Profile Picture Control
The SDK provides a ProfilePictureControl that can be populated with the logged in user's profile picture.

XAML:
{% highlight xml %}
<Page
    …
    xmlns:fbsdk="using:winsdkfb"
    … >
<fbsdk:ProfilePictureControl x:Name="ProfilePic" Width="120" Height="120"Margin="659,270,661,570" />
...
</Page>

{% endhighlight %}

<br />

{% highlight csharp %}
// Get active session
FBSession sess = FBSession.ActiveSession;

if(sess.LoggedIn)
{
	// Get current user
	FBUser user = sess.User;
	// Set profile pic
	ProfilePic.UserId = user.Id;
}
{% endhighlight %}

{% highlight c++ %}
// Get active session
FBSession^ sess = FBSession::ActiveSession;

if(sess->LoggedIn)
{
	// Get current user
	FBUser^ user = sess->User;
	// Set profile pic
	ProfilePic->UserId = user->Id;
}
{% endhighlight %}

## User Information
Some basic information about the logged in user can directly be accessed through FBSession.ActiveSession.User.

{% highlight csharp %}
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
{% endhighlight %}

{% highlight c++ %}
// Get active session
FBSession^ sess = FBSession::ActiveSession;
if (sess->LoggedIn)
{
	FBUser^ user = sess->User;
	if (user)
	{
		String^ userId = L"Id : " + user->Id;
		String^ username = L"Name : " + user->Name;
		String^ locale = L"Locale : " + user->Locale;
	}
}
{% endhighlight %}