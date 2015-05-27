//******************************************************************************
//
// Copyright (c) 2015 Microsoft Corporation. All rights reserved.
//
// This code is licensed under the MIT License (MIT).
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
//******************************************************************************

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using Microsoft.VisualStudio.TestPlatform.UnitTestFramework;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Data.Json;
using Facebook;
using Facebook.Graph;
using Windows.Storage;
using Windows.Storage.Streams;

namespace FBWinStoreCsTests
{

    [TestClass]
    public class UnitTest1
    {
        const int SAMPLE_PAGE_INDEX  = 0;
        const int SAMPLE_PAGE2_INDEX = 1;
        const int SAMPLE_PAGE3_INDEX = 2;
        const int SAMPLE_PAGE4_INDEX = 3;
        const int TEST_RETRY_DELAY   = 1000;

        const int FBDeleteOpenGraphUserErrorCode = 2904;

        const string SampleJsonPage =
            "{\"category\":\"Clothing\",\"name\":\"Frockasaurus\",\"created_t" +
            "ime\":\"2014-09-01T16:45:17+0000\",\"id\":\"438872976218316\"}";

        const string SampleJsonPage2 =
            "{\"category\":\"Media\\/news\\/publishing\",\"name\":\"ClickHole" +
            "\",\"created_time\":\"2014-06-10T18:32:23+0000\",\"id\":\"143904" +
            "2583002670\"}";

        const string SampleJsonPage3 = 
            "{\"category\":\"Kitchen\\/cooking\",\"name\":\"Elevation Rack\"," +
            "\"created_time\":\"2014-05-22T22:58:09+0000\",\"id\":\"310515869" +
            "105473\"}";

        const string SampleJsonPage4 =
            "{\"category\":\"Local business\",\"category_list\":[{\"id\":\"18" +
            "1216025249367\",\"name\":\"Apartment & Condo Building\"}],\"name" +
            "\":\"Henningzen' Place\",\"created_time\":\"2014-05-05T02:29:11+" +
            "0000\",\"id\":\"666926900011900\"}";

        const string PagingObjectJson = 
            "{\"cursors\":{\"after\":\"MTA2MjI0NjY2MDc0NjI1\",\"before\":\"ND" +
            "M4ODcyOTc2MjE4MzE2\"},\"next\":\"https:\\/\\/graph.facebook.com"  +
            "\\/v2.1\\/10203673618494044\\/likes?access_token=CAADZ0iddz0tkrN" +
            "iuUMKGDiLwZDZD&limit=25&after=MTA2MjI0NjY2MDc0NjI1\"}";

        const string ErrorObjectJson =
            "{\"error\": {\"message\": \"Message describing the error\", " +
            "\"type\": \"OAuthException\", \"code\": 190, \"error_subcode\": " +
            "460, \"error_user_title\": \"A title\", \"error_user_msg\": \"A " +
            "message\"}}";

        const string ErrorJsonMessage = "Message describing the error";

        const string ErrorJsonType = "OAuthException";

        int ErrorJsonCode = 190;

        int ErrorJsonSubcode = 460;

        const string ErrorJsonTitle = "A title";

        const string ErrorJsonUserMessage = "A message";

        const string FBCanceledRequestResponse = 
            "http://whatever?error_code=4201&"
            + "error_message=User+canceled+the+Dialog+flow";

        const string FBCanceledRequestMessage = "User canceled the Dialog flow";

        string[] PageNames = 
        {
            "Frockasaur",
            "ClickHole",
            "Elevation Rack",
            "Henningzen' Place"
        };

        string[] PageCategories = 
        {
            "Clothing",
            "Media\\/news\\/publishing",
            "Kitchen\\/cooking",
            "Local business"
        };

        string[] PageIds =
        {
            "438872976218316",
            "1439042583002670",
            "310515869105473",
            "666926900011900"
        };

        // TODO: May need to change this at some point in the future.  This is 
        // the app Id for the LoginTester app in the FBSDK project 
        const string TestAppId = "1406183856361054";

        const string FBSDKTokenServiceHost = "fbwinsdk.azure-mobile.net";
        const string FBSDKTokenApiPath = "/api/get_token";
        const string FBFeedPath = "/feed";
        const string FBSDKTestMessage = "This is a test message";
        const string FBSDKTestUsersPath = "/accounts/test-users";
        const string FBTestImageName = "animal-1409304111INq.jpg";
        const string FBTestImagePath = "Images\\" + FBTestImageName;
        const string FBTestPhotoUploadPermissions = "public_profile," +
            "publish_actions,user_photos";
        const string FBCustomObjectInstance =
            "{" +
            "\"type\":\"logincs:noun\"," +
            "\"title\":\"generic_title\"" +
            "}";

        [TestMethod]
        public void tryCreateFBPage()
        {
            string[] pages =
            {
                SampleJsonPage,
                SampleJsonPage2,
                SampleJsonPage3,
                SampleJsonPage4
            };

            for (int i = SAMPLE_PAGE_INDEX; i <= SAMPLE_PAGE4_INDEX; i++)
            {
                object obj = FBPage.FromJson(pages[i]);
                FBPage page = (FBPage)obj;
                Assert.IsNotNull(obj);
                Assert.IsNotNull(page);

                StringAssert.Equals(page.Name, PageNames[i]);
                StringAssert.Equals(page.Category, PageCategories[i]);
                StringAssert.Equals(page.Id, PageIds[i]);
            }
        }
        [TestMethod]
        public void tryCreatePageViaClassFactory()
        {
            string[] pages =
            {
                SampleJsonPage,
                SampleJsonPage2,
                SampleJsonPage3,
                SampleJsonPage4
            };

            for (int i = SAMPLE_PAGE_INDEX; i <= SAMPLE_PAGE4_INDEX; i++)
            {
                FBJsonClassFactory fact = new FBJsonClassFactory(
                    (JsonText) => FBPage.FromJson(JsonText));
                object obj = fact(pages[i]);
                FBPage page = (FBPage)obj;
                Assert.IsNotNull(obj);
                Assert.IsNotNull(page);

                StringAssert.Equals(page.Name, PageNames[i]);
                StringAssert.Equals(page.Category, PageCategories[i]);
                StringAssert.Equals(page.Id, PageIds[i]);
            }
        }
        [TestMethod]
        public void tryCreateFBPaging()
        {
            Object obj = FBPaging.FromJson(PagingObjectJson);
            FBPaging paging = (FBPaging)obj;
            Assert.IsNotNull(obj);
            Assert.IsNotNull(paging);
        }

        [TestMethod]
        public void parseArrayOfLikes()
        {
            string FBArrayOfLikes =
                "{\"data\":[" + SampleJsonPage + "," + SampleJsonPage2 + "," +
                SampleJsonPage3 + "," + SampleJsonPage4 + "],\"paging\":" +
                PagingObjectJson + "}";

            string fbResponse = FBArrayOfLikes;
            FBPaginatedArray arr = new FBPaginatedArray("Unused", null,
                new FBJsonClassFactory(
                    (JsonText) => FBPage.FromJson(JsonText)));

            IReadOnlyList<object> result = arr.ObjectArrayFromWebResponse(
                fbResponse,
                new FBJsonClassFactory(
                    (JsonText) => FBPage.FromJson(JsonText)));
            for (int i = 0; i < result.Count; i++)
            {
                object obj = result[i];
                FBPage page = (FBPage)result[i];
                Assert.IsNotNull(obj);
                Assert.IsNotNull(page);
                StringAssert.Equals(page.Name, PageNames[i]);
            }
        }

        [TestMethod]
        public void parseErrorFromJson()
        {
            FBError e = FBError.FromJson(ErrorObjectJson);
            Assert.IsNotNull(e);
            Assert.IsNotNull(e.Message);
            Assert.IsNotNull(e.Type);
            Assert.IsNotNull(e.ErrorUserTitle);
            Assert.IsNotNull(e.ErrorUserMessage);
            Assert.AreEqual(e.Message, ErrorJsonMessage);
            Assert.AreEqual(e.Type, ErrorJsonType);
            Assert.AreEqual(e.Code, ErrorJsonCode);
            Assert.AreEqual(e.Subcode, ErrorJsonSubcode);
            Assert.AreEqual(e.ErrorUserTitle, ErrorJsonTitle);
            Assert.AreEqual(e.ErrorUserMessage, ErrorJsonUserMessage);
        }

        [TestMethod]
        public void parseErrorFromQueryString()
        {
            FBError err = FBError.FromUri(new Uri(FBCanceledRequestResponse));
            Assert.IsNotNull(err);
            Assert.AreEqual(err.Code, 4201);
            Assert.AreEqual(FBCanceledRequestMessage, err.Message);
        }

        private async Task<string> getAppToken()
        {
            PropertySet parameters = new PropertySet();
            string result = null;
 
            parameters.Add("request_host", FBSDKTokenServiceHost);
            parameters.Add("access_token", "");
            parameters.Add("id", TestAppId);
            
            FBSingleValue sval = new FBSingleValue(FBSDKTokenApiPath,
                parameters, 
                new FBJsonClassFactory((JsonText) => 
            {
                JsonObject obj = null;
                if (!JsonObject.TryParse(JsonText, out obj))
                {
                    obj = null;
                }

                return obj;
            }));

            FBResult fbresult = await sval.Get();

            if (fbresult.Succeeded)
            {
                JsonObject obj = (JsonObject)fbresult.Object;
                if (obj.Keys.Contains("access_token"))
                { 
                    result = obj.GetNamedString("access_token"); 
                }
            }

            return result;
        }

        private async Task<FBTestUser> createTestUser(
            PropertySet Parameters 
            )
        {
            FBTestUser user = null;
            bool success = false;

            // If you're creating/deleting test users frequently, like say in
            // a unit test suite, Facebook will occasionally fail this call with 
            // the error code 1, type "OAuthException" and the message "An 
            // unknown error occurred".  Throwing in a short delay and retrying
            // almost always alleviates the problem, so adding a few retries
            // here greatly increases the robustness of the test suite.
            for (int retries = 0; (success == false) && (retries < 5); 
                retries++)
            {
                string path = "/" + TestAppId + FBSDKTestUsersPath;

                FBSingleValue sval = new FBSingleValue(path, Parameters,
                    new FBJsonClassFactory(FBTestUser.FromJson));

                FBResult fbresult = await sval.Post();
    
                if ((fbresult.Succeeded == false) || (fbresult.Object == null))
                {
                    await Task.Delay(TEST_RETRY_DELAY);
                }
                else
                {
                    try
                    {
                        user = (FBTestUser)fbresult.Object;
                        success = true;
                    }
                    catch (InvalidCastException)
                    {
                        Assert.IsTrue(false, "Item returned is not expected type" +
                            " (FBTestUser)");
                    }
                }
            }

            Assert.IsNotNull(user);

            return user;
        }

        private async Task<FBObject> postToFeed(
            FBTestUser User,
            string Message
            )
        {
            PropertySet parameters = new PropertySet();

            parameters.Add("access_token", User.AccessToken);
            parameters.Add("message", Message);

            string path = "/" + User.Id + FBFeedPath;

            FBSingleValue sval = new FBSingleValue(path, parameters,
                new FBJsonClassFactory(FBObject.FromJson));

            FBResult fbresult = await sval.Post();

            return (FBObject)fbresult.Object;
        }

        public async Task<IReadOnlyList<Object>> getListOfTestUsers()
        {
            string token = await getAppToken();
            PropertySet parameters = new PropertySet();
            string path = "/" + TestAppId + FBSDKTestUsersPath;
            //Assert.IsNotNull(token);

            parameters.Add("access_token", Uri.EscapeUriString(token));

            FBPaginatedArray arr = new FBPaginatedArray(path, parameters,
                new FBJsonClassFactory(FBTestUser.FromJson));

            FBResult result = await arr.First();
            //Assert.IsTrue(result.Succeeded);

            IReadOnlyList<Object> users = null;

            try
            {
                users = (IReadOnlyList<Object>)result.Object; 
            }
            catch (InvalidCastException)
            {
                Assert.IsTrue(false, "Item returned is not expected type" +
                    " (IReadOnlyList<Object>)");
            }

            return users;
        }

        public async Task<FBResult> deleteTestUser(
            FBTestUser user
            )
        {
            string token = await getAppToken();
            PropertySet parameters = new PropertySet();
            string path = "/" + user.Id;

            parameters.Add("access_token", Uri.EscapeUriString(token));

            FBSingleValue sval = new FBSingleValue(path, parameters,
                new FBJsonClassFactory(FBSuccess.FromJson));

            return await sval.Delete();
        }

        public async Task deleteTestUsers()
        {
            IReadOnlyList<Object> users = await getListOfTestUsers();
            Assert.IsNotNull(users);
            List<Task<FBResult>> results = new List<Task<FBResult>>();

            for (int i = 0; i < users.Count; i++)
            {
                try
                {
                    results.Add(deleteTestUser((FBTestUser)users[i]));
                }
                catch (InvalidCastException)
                {
                    Assert.IsTrue(false, "Item returned is not expected type" +
                        " (FBTestUser)");
                }
            }

            for (int i = 0; i < users.Count; i++)
            {
                FBResult result = await results[i];
                
                if (result.Succeeded == false)
                {
                    Assert.IsNotNull(result.ErrorInfo);
                    // "You cannot delete the Open Graph Test User for your app."
                    // We should always successfully delete the user, except for
                    // the special "Open Graph Test User".
                    Assert.AreEqual(result.ErrorInfo.Code, 
                        FBDeleteOpenGraphUserErrorCode);
                }

                try
                {
                    FBSuccess success = (FBSuccess)result.Object;
                }
                catch (InvalidCastException)
                {
                    Assert.IsTrue(false, "Item returned is not expected type" +
                        " (FBSuccess)");
                }
            }
        }

        public async Task<FBResult> publishCustomUserObject(
            FBTestUser user
            )
        {
            PropertySet parameters = new PropertySet();
            string path = user.Id + "/objects/logincs:noun";

            Assert.IsNotNull(user.AccessToken);
            parameters.Add("access_token", user.AccessToken);
            parameters.Add("object", FBCustomObjectInstance);

            FBSingleValue sval = new FBSingleValue(path, parameters,
                new FBJsonClassFactory(FBObject.FromJson));

            return await sval.Post();
        }

        public async Task<FBResult> publishCustomStory(
            FBTestUser user,
            FBObject customObject
            )
        {
            PropertySet parameters = new PropertySet();
            string path = user.Id + "/logincs:verb";

            parameters.Add("noun", customObject.Id);
            parameters.Add("access_token", user.AccessToken);

            FBSingleValue sval = new FBSingleValue(path, parameters,
                new FBJsonClassFactory(FBObject.FromJson));

            return await sval.Post();
        }

        [TestMethod]
        public async Task testGetAppToken()
        {
            string token = await getAppToken();

            Assert.IsNotNull(token);
        }

        [TestMethod]
        public async Task testPostToTestUserFeed()
        {
            string appToken = await getAppToken();
            PropertySet parameters = new PropertySet();
            string permissions = "public_profile,user_friends,publish_actions";
            string token = await getAppToken();

            parameters.Add("access_token", appToken);
            parameters.Add("permissions", permissions);
            parameters.Add("installed", "true");

            FBTestUser user = await createTestUser(parameters);
            Assert.IsNotNull(user);

            FBObject post = await postToFeed(user, FBSDKTestMessage);
            Assert.IsNotNull(post);

            await deleteTestUser(user);
        }

        [TestMethod]
        public async Task testListTestUsers()
        {
            string token = await getAppToken();
            PropertySet parameters = new PropertySet();
            string path = "/" + TestAppId + FBSDKTestUsersPath;
            Assert.IsNotNull(token);
            bool found = false;

            parameters.Add("access_token", Uri.EscapeUriString(token));

            //Ensure we have at least one test user!
            FBTestUser user = await createTestUser(parameters);
            
            FBPaginatedArray arr = new FBPaginatedArray(path, parameters,
                new FBJsonClassFactory(FBTestUser.FromJson));

            FBResult result = await arr.First();
            Assert.IsTrue(result.Succeeded);

            IReadOnlyList<Object> users = 
                (IReadOnlyList<Object>)result.Object;

            Assert.IsTrue(users.Count > 0);

            for (int i = 0; i < users.Count; i++)
            {
                try
                {
                    FBTestUser testuser = (FBTestUser)users[i];
                    if (string.CompareOrdinal(testuser.Id, user.Id) == 0)
                    {
                        found = true;
                    }
                }
                catch (InvalidCastException)
                {
                    Assert.IsTrue(false, "Item returned is not expected type" +
                        " (FBTestUser)");
                }
            }

            Assert.IsTrue(found);

            await deleteTestUser(user);
        }

        [TestMethod]
        public async Task testDeleteAllTestUsers()
        {
            await deleteTestUsers();
        }

        [TestMethod]
        public async Task testUploadPhoto()
        {
            string token = await getAppToken();
            PropertySet parameters = new PropertySet();

            parameters.Add("access_token", Uri.EscapeUriString(token));
            parameters.Add("permissions", FBTestPhotoUploadPermissions);

            FBTestUser user = await createTestUser(parameters);

            StorageFolder appFolder = 
                Windows.ApplicationModel.Package.Current.InstalledLocation;
            StorageFile f = await appFolder.GetFileAsync(
                FBTestImagePath);
            IRandomAccessStreamWithContentType stream = await f.OpenReadAsync();
            Assert.IsNotNull(stream);

            FBMediaStream fbStream = new FBMediaStream(FBTestImageName, 
                stream);

            // Switch to user access token to post photo.
            parameters.Remove("access_token");
            parameters.Add("access_token", user.AccessToken);

            parameters.Add("source", fbStream);

            string path = "/" + user.Id + "/photos";

            FBSingleValue sval = new FBSingleValue(path, parameters,
                new FBJsonClassFactory(FBPhoto.FromJson));

            FBResult result = await sval.Post();
            Assert.IsTrue(result.Succeeded);

            try
            {
                FBPhoto pic = (FBPhoto)result.Object;
            }
            catch (InvalidCastException)
            {
                Assert.IsFalse(true, "Object returned was not of the " +
                    "expected type (FBPhoto).");
            }
        }

        [TestMethod]
        public async Task testLikeSomething()
        {
            string token = await getAppToken();
            PropertySet parameters = new PropertySet();

            parameters.Add("access_token", Uri.EscapeUriString(token));
            parameters.Add("permissions",
                "public_profile,publish_actions,user_photos");

            FBTestUser user = await createTestUser(parameters);

            string path = user.Id + "/og.likes";

            // Because *everybody* likes these, amirite?
            string likedObject =
                Uri.EscapeUriString("http://en.wikipedia.org/wiki/Brussels_sprout");

            parameters.Add("object", likedObject);

            FBSingleValue sval = new FBSingleValue(path, parameters,
                new FBJsonClassFactory(FBObject.FromJson));

            FBResult result = await sval.Post();
            Assert.IsTrue(result.Succeeded);

            try
            {
                FBObject like = (FBObject)result.Object;
            }
            catch (InvalidCastException)
            {
                Assert.IsFalse(true, "Object returned was not of the " +
                    "expected type (FBObject).");
            }
        }

        [TestMethod]
        public async Task testPublishCustomStory()
        {
            string token = await getAppToken();
            PropertySet parameters = new PropertySet();
            FBObject customObject = null;
            FBObject customStory = null;

            parameters.Add("access_token", Uri.EscapeUriString(token));
            parameters.Add("permissions",
                "public_profile,publish_actions,user_photos");

            FBTestUser user = await createTestUser(parameters);

            FBResult result = await publishCustomUserObject(user);
            Assert.IsTrue(result.Succeeded);

            try
            {
                customObject = (FBObject)result.Object;
            }
            catch (InvalidCastException)
            {
                Assert.IsFalse(true, "Object returned was not of the " +
                    "expected type (FBObject).");
            }

            result = await publishCustomStory(user, customObject);
            Assert.IsTrue(result.Succeeded);

            try
            {
                customStory = (FBObject)result.Object;
            }
            catch (InvalidCastException)
            {
                Assert.IsFalse(true, "Object returned was not of the " +
                    "expected type (FBObject).");
            }
        }
    }
}
