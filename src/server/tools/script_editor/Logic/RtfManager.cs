using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Logic
{
    public class RtfManager
    {
        private string textColor = @"\red0\green0\blue0;";//文字颜色-->黑 
        private string BgWhite = @"\red255\green255\blue255;";//背景颜色-->白 
        private string BgGray = @"\red180\green180\blue180;";//背景颜色-->灰 

        //插入文字 
        public string getTagRtf(string text, string textColor, string textBgColor)
        {
            StringBuilder rtfText = new StringBuilder();
            rtfText.Append(@"{\rtf1\ansi\ansicpg936\deff0\deflang1033\deflangfe2052");//rtf头 
            rtfText.Append(@"{\fonttbl{\f0\fnil\fcharset134\''cb\''ce\''cc\''e5;}}");//字体 
            rtfText.Append(@"{\colortbl;");//定义颜色 
            rtfText.Append(textColor);//文字颜色 
            rtfText.Append(textBgColor);//文字背景颜色 
            rtfText.Append(@"}");
            rtfText.Append(@"{\*\generator Msftedit 5.41.21.2500;}\viewkind4\uc1\pard\highlight2\b\lang2052\f0\fs18");//文本体 
            rtfText.Append(text);//要显示的文本。 
            rtfText.Append(@"}");//rtf结尾//\par 
            string rtfcode = rtfText.ToString();
            return rtfcode;
        }
    }
}
