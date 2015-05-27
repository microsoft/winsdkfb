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

#include "pch.h"
#include "ColorLuminosityConverter.h"
#include "HlsColor.h"
#include <string>

using namespace Facebook;
using namespace Platform;
using namespace std;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;

Object^ ColorLuminosityConverter::Convert(
    Object^ value,
    TypeName targetType,
    Object^ parameter,
    String^ language
    )
{
    String^ paramString = safe_cast<String^>(parameter);
    double factor = stod(wstring(paramString->Data()));
    Object^ result = nullptr;
    HlsColor^ hlsColor = ref new HlsColor();

    SolidColorBrush^ scb = dynamic_cast<SolidColorBrush^>(value);
    if (scb != nullptr)
    {
        Color brushColor = scb->Color;
        hlsColor->RgbValue = brushColor;
        hlsColor->Luminosity *= factor;
        Color newColor = hlsColor->RgbValue;
        result = ref new SolidColorBrush(newColor);
    }
    else 
    {
        LinearGradientBrush^ lgb = dynamic_cast<LinearGradientBrush^>(value);
        if (lgb != nullptr)
        {
            GradientStopCollection^ gradientStops =
                ref new GradientStopCollection();

            IIterator<GradientStop^>^ iter = nullptr;
            for (iter = lgb->GradientStops->First(); iter->HasCurrent;
                iter->MoveNext())
            {
                GradientStop^ stop = iter->Current;
                GradientStop^ newStop = ref new GradientStop();

                hlsColor->RgbValue = stop->Color;
                hlsColor->Luminosity *= factor;
                newStop->Color = hlsColor->RgbValue;
                newStop->Offset = stop->Offset;
                gradientStops->Append(newStop);
            }

            result = ref new LinearGradientBrush(gradientStops, 0.0);
        }
    }

    return result;
}
