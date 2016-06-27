---
layout: default
title: Dialogs
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

## Feed Dialog
The feed dialog allows the app to specify a title, link, and description for a post to the user's feed. The user can enter a custom message before posting it to Facebook.

{% highlight csharp %}
{% include SampleCode/SampleCodeCs/Samples/Dialogs/FeedDialog.cs %}
{% endhighlight %}

{% highlight c++ %}
{% include SampleCode/SampleCode/Samples/Dialogs/FeedDialog.cpp %}
{% endhighlight %}

## Request Dialog
The user can invite his/her Facebook friends to the app through the request dialog. Note that your app must be set to type 'Game' when registering with Facebook in order for the request dialog to work.

{% highlight csharp %}
{% include SampleCode/SampleCodeCs/Samples/Dialogs/RequestDialog.cs %}
{% endhighlight %}

{% highlight c++ %}
{% include SampleCode/SampleCode/Samples/Dialogs/RequestDialog.cpp %}
{% endhighlight %}

## Send Dialog
Using the send dialog, a user can send private messages to any of his/her facebook friends with a predetermined link specified.

{% highlight csharp %}
{% include SampleCode/SampleCodeCs/Samples/Dialogs/SendDialog.cs %}
{% endhighlight %}

{% highlight c++ %}
{% include SampleCode/SampleCode/Samples/Dialogs/SendDialog.cpp %}
{% endhighlight %}
<br />