---
layout: default
title: Dialogs
---

## Feed Dialog
The feed dialog allows the app to specify a title, link, and description for a post to the user's feed. The user can enter a custom message before posting it to Facebook.

C#:
{% highlight csharp %}
{% include SampleCode/SampleCodeCs/Samples/Dialogs/FeedDialog.cs %}
{% endhighlight %}

C++:
{% highlight c++ %}
{% include SampleCode/SampleCode/Samples/Dialogs/FeedDialog.cpp %}
{% endhighlight %}

## Request Dialog
The user can invite his/her Facebook friends to the app through the request dialog. Note that your app must be set to type 'Game' when registering with Facebook in order for the request dialog to work.

C#:
{% highlight csharp %}
{% include SampleCode/SampleCodeCs/Samples/Dialogs/RequestDialog.cs %}
{% endhighlight %}

C++:
{% highlight c++ %}
{% include SampleCode/SampleCode/Samples/Dialogs/RequestDialog.cpp %}
{% endhighlight %}

## Send Dialog
Using the send dialog, a user can send private messages to any of his/her facebook friends with a predetermined link specified.

C#:
{% highlight csharp %}
{% include SampleCode/SampleCodeCs/Samples/Dialogs/SendDialog.cs %}
{% endhighlight %}

C++
{% highlight c++ %}
{% include SampleCode/SampleCode/Samples/Dialogs/SendDialog.cpp %}
{% endhighlight %}
<br />