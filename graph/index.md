---
layout: default
title: Graph API
---

## A Helper Class

The examples on this page use a helper class, `FBReturnObject`, to represent the data returned by a Graph API call.

C#

{% highlight csharp %}
{% include SampleCode/SampleCodeCs/Samples/FBReturnObject.cs %}
{% endhighlight %}

C++:

{% highlight c++ %}
{% include SampleCode/SampleCode/Samples/FBReturnObject.h %}
{% endhighlight %}

## Post to Feed
The app should have publish_actions permission granted by the user.
The response will be an id of type string `{"id": "<id here>"}` if published successfully.
You can find more details [here](https://developers.facebook.com/docs/graph-api/reference/v2.3/user/feed).

C#:

{% highlight csharp %}
{% include SampleCode/SampleCodeCs/Samples/Graph/FeedPost.cs %}
{% endhighlight %}

C++:

{% highlight c++ %}
{% include SampleCode/SampleCode/Samples/Graph/FeedPost.cpp %}
{% endhighlight %}

## Custom Stories
Follow the steps [here](https://developers.facebook.com/docs/sharing/opengraph/custom) to configure custom stories in your app on developers.facebook.com
The app should have publish_actions permission granted by the user.
The response will be an id of type string `{"id":"<id here>"}` if published successfully.

For the snippet below,

App namespace: `fbsdk_sample_app`

A custom story was created with action type *try* and object type *scenario*.

We define a custom json object parameter `{"title": "Custom Story"}` and set the Graph API path to `userId/fbsdk_sample_app:try`.

This will be published as "*user* tried *scenario* from Sample Application".

C#:

{% highlight csharp %}
{% include SampleCode/SampleCodeCs/Samples/Graph/CustomStories.cs %}
{% endhighlight %}

C++

{% highlight c++ %}
{% include SampleCode/SampleCode/Samples/Graph/CustomStories.cpp %}
{% endhighlight %}

## Upload a Photo
The app should have publish_actions permission granted by the user. The response will be an id of type string `{"id": "<id here>"}` if published successfully.
You can find more details [here](https://developers.facebook.com/docs/graph-api/reference/user/photos/).

C#:

{% highlight csharp %}
{% include SampleCode/SampleCodeCs/Samples/Graph/PhotoUpload.cs %}
{% endhighlight %}

C++:

{% highlight c++ %}
{% include SampleCode/SampleCode/Samples/Graph/PhotoUpload.cpp %}
{% endhighlight %}

## Upload a Video (non-resumable)
The app should have publish_actions permission granted by the user. The response will be an id of type string `{"id":"<id here>"}` if published successfully. Note that this is for uploading a small sized video all at once (non-resumable). The Facebook Graph API has said non-resumable upload supports videos up to 1GB and 20 minutes long.


C#:

{% highlight csharp %}
{% include SampleCode/SampleCodeCs/Samples/Graph/VideoUpload.cs %}
{% endhighlight %}


## Like Action
The app should have publish_actions permission granted by the user. The response will be an id of type string `{"id":"<id here>"}` if published successfully.

Note that this is not the same as 'liking' a Facebook Page. If successful, the like action will be published onto the user's activity feed. You need extra permission to post it as an Open Graph object on the user's timeline/news feed. You can find more details [here](https://developers.facebook.com/docs/opengraph/guides/og.likes).

C#:

{% highlight csharp %}
{% include SampleCode/SampleCodeCs/Samples/Graph/LikeAction.cs %}
{% endhighlight %}

C++:

{% highlight c++ %}
{% include SampleCode/SampleCode/Samples/Graph/LikeAction.cpp %}
{% endhighlight %}
<br />