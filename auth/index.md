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
{% include SampleCode/SampleCodeCs/Samples/Auth/Initialization.cs %}
{% endhighlight %}

{% highlight c++ %}
{% include SampleCode/SampleCode/Samples/Auth/Initialization.cpp %}
{% endhighlight %}

Note: During development, you can always use the PhoneProductID from the manifest to get the WinAppId instead of having one Windows Store ID and one Windows Phone Store ID.For a published app you would need the package SID.

## Login
Use the following code snippet to login to Facebook.
The `sess.LoginAsync()` or `sess->LoginAsync()` call launches the Facebook login dialog box for the user to enter his/her username and password.

{% highlight csharp %}
{% include SampleCode/SampleCodeCs/Samples/Auth/Login.cs %}
{% endhighlight %}

{% highlight c++ %}
{% include SampleCode/SampleCode/Samples/Auth/Login.cpp %}
{% endhighlight %}

## Logout
This is simply just calling the `LogoutAsync()` method.

{% highlight csharp %}
{% include SampleCode/SampleCodeCs/Samples/Auth/Logout.cs %}
{% endhighlight %}

{% highlight c++ %}
{% include SampleCode/SampleCode/Samples/Auth/Logout.cpp %}
{% endhighlight %}

## Profile Picture Control
The SDK provides a ProfilePictureControl that can be populated with the logged in user's profile picture.

XAML:
{% highlight xml %}
{% include SampleCode/SampleCode/Samples/Auth/ProfilePictureControlSample.xaml %}
{% endhighlight %}

<br />

{% highlight csharp %}
{% include SampleCode/SampleCodeCs/Samples/Auth/ProfilePictureControlSample.xaml.cs %}
{% endhighlight %}

{% highlight c++ %}
{% include SampleCode/SampleCode/Samples/Auth/ProfilePictureControlSample.xaml.cpp %}
{% endhighlight %}

## User Information
Some basic information about the logged in user can directly be accessed through `FBSession.ActiveSession.User`.

{% highlight csharp %}
{% include SampleCode/SampleCodeCs/Samples/Auth/UserInformation.cs %}
{% endhighlight %}

{% highlight c++ %}
{% include SampleCode/SampleCode/Samples/Auth/UserInformation.cpp %}
{% endhighlight %}
<br />