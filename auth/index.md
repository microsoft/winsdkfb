---
layout: default
title: Authentication
---

## Initializing the Facebook Session
Set the Facebook App ID and Windows Store ID values in active session:

C#:
{% highlight csharp %}
{% include SampleCode/SampleCodeCs/Samples/Auth/Initialization.cs %}
{% endhighlight %}

C++:
{% highlight c++ %}
{% include SampleCode/SampleCode/Samples/Auth/Initialization.cpp %}
{% endhighlight %}

Note: During development, you can always use the PhoneProductID from the manifest to get the WinAppId instead of having one Windows Store ID and one Windows Phone Store ID.For a published app you would need the package SID.

## Login
Use the following code snippet to login to Facebook.
The `sess.LoginAsync()` or `sess->LoginAsync()` call launches the Facebook login dialog box for the user to enter his/her username and password.

C#:
{% highlight csharp %}
{% include SampleCode/SampleCodeCs/Samples/Auth/Login.cs %}
{% endhighlight %}

C++:
{% highlight c++ %}
{% include SampleCode/SampleCode/Samples/Auth/Login.cpp %}
{% endhighlight %}

## Logout
This is simply just calling the `LogoutAsync()` method.

C#:
{% highlight csharp %}
{% include SampleCode/SampleCodeCs/Samples/Auth/Logout.cs %}
{% endhighlight %}

C++:
{% highlight c++ %}
{% include SampleCode/SampleCode/Samples/Auth/Logout.cpp %}
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

C#:
{% highlight csharp %}
{% include SampleCode/SampleCodeCs/Samples/Auth/ProfilePictureControl.cs %}
{% endhighlight %}

C++:
{% highlight c++ %}
{% include SampleCode/SampleCode/Samples/Auth/ProfilePictureControl.cpp %}
{% endhighlight %}

## User Information
Some basic information about the logged in user can directly be accessed through `FBSession.ActiveSession.User`.

C#:
{% highlight csharp %}
{% include SampleCode/SampleCodeCs/Samples/Auth/UserInformation.cs %}
{% endhighlight %}

C++:
{% highlight c++ %}
{% include SampleCode/SampleCode/Samples/Auth/UserInformation.cpp %}
{% endhighlight %}
<br />