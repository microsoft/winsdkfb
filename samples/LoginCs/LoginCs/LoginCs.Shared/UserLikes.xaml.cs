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
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

using winsdkfb;
using winsdkfb.Graph;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace LoginCs
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class UserLikes : Page
    {
        public ObservableCollection<MyFBPage> Items { get; private set; }
        private FBPaginatedArray _likes;

        public UserLikes()
        {
            this.InitializeComponent();
            Items = new ObservableCollection<MyFBPage>();
            this.DataContext = this;
            GetUserLikes();
        }

        private void DoneGetUserLikes()
        {
            ;
        }

        private async void AddLikes(
            IReadOnlyList<object> pages 
            )
        {
            int count = 0;
            foreach (object page in pages)
            {
                Items.Add((MyFBPage)page);
                if (count == 0)
                {
                    count++;
                    PageGetter pg = new PageGetter();
                    pg.GetPageWithId(((MyFBPage)page).Id);
                }
            }

            if (_likes.HasNext)
            {
                FBResult result = await _likes.NextAsync();
                if (result.Succeeded)
                {
                    IReadOnlyList<object> nextPages = 
                        (IReadOnlyList<object>)result.Object;
                    AddLikes(nextPages);
                }
            }
            else
            {
                DoneGetUserLikes();
            }
        }

        public async void GetUserLikes()
        {
            if (FBSession.ActiveSession.LoggedIn)
            {
                string graphPath = FBSession.ActiveSession.User.Id + "/likes";
                
                FBJsonClassFactory fact = new FBJsonClassFactory(
                    (JsonText) => MyFBPage.FromJson(JsonText));

                _likes = new FBPaginatedArray(graphPath, null, fact);
                FBResult result = await _likes.FirstAsync();
                if (result.Succeeded)
                {
                    BadResultsTextBlock.Visibility = Windows.UI.Xaml.Visibility.Collapsed;
                    LikesListView.Visibility = Windows.UI.Xaml.Visibility.Visible;
                    if (_likes.Current.Count > 0)
                    {
                        AddLikes(_likes.Current);
                    }
                    else
                    {
                        LikesListView.Visibility = Windows.UI.Xaml.Visibility.Collapsed;
                        BadResultsTextBlock.Visibility = Windows.UI.Xaml.Visibility.Visible;
                        BadResultsTextBlock.Text = "No User likes found";
                    }

                }
                else
                {
                    LikesListView.Visibility = Windows.UI.Xaml.Visibility.Collapsed;
                    BadResultsTextBlock.Visibility = Windows.UI.Xaml.Visibility.Visible;
                    BadResultsTextBlock.Text = result.ErrorInfo.Message;
                }
            }
        }


        private void BackButton_Click(object sender, RoutedEventArgs e)
        {
            Frame.GoBack();
        }

        void ListView_SelectionChanged(Object sender, SelectionChangedEventArgs e)
        {
            //FBPageBindable ^ selected = static_cast < FBPageBindable ^> (e->AddedItems->GetAt(0));
            MyFBPage selected = (MyFBPage) e.AddedItems.First();
            ItemDescription.Text = "hello";
        }

    }

}
