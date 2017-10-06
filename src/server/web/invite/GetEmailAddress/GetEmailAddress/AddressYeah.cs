namespace GetEmailAddress
{
    using System;
    using System.Collections.Generic;
    using System.IO;
    using System.Net;
    using System.Text;
    using System.Text.RegularExpressions;

    public class AddressYeah
    {
        private byte[] b;
        private CookieContainer cookieCon;
        protected static string cookieheader = string.Empty;
        private Address163.Entrys en;
        private Match m;
        private const string mailYeah = "https://reg.163.com/logins.jsp?type=1&product=mailyeah&url=http://entry.mail.yeah.net/cgi/ntesdoor?lightweight%3D1%26verifycookie%3D1%26style%3D-1";
        protected static string NextUrl = string.Empty;
        private string pwd;
        private Regex reg;
        private HttpWebRequest req;
        private HttpWebResponse res;
        private string uName;

        private AddressYeah()
        {
            this.cookieCon = new CookieContainer();
        }

        public AddressYeah(string name, string pwd, Address163.Entrys type)
        {
            this.cookieCon = new CookieContainer();
            this.uName = name;
            this.pwd = pwd;
            this.en = type;
            StringBuilder builder = new StringBuilder();
            builder.Append("&domain=yeah.net");
            builder.Append("&language=0");
            builder.Append("&bCookie=");
            builder.Append("&username=" + name + "@yeah.net");
            builder.Append("&savelogin=");
            builder.Append("&user=" + name);
            builder.Append("&password=" + pwd);
            builder.Append("&style=-1");
            builder.Append("&secure=");
            builder.Append("&enter.x=登　录");
            this.b = Encoding.ASCII.GetBytes(builder.ToString());
        }

        public List<Person> getContact()
        {
            List<Person> list = new List<Person>();
            foreach (string str2 in this.GetHtml().Split(new string[] { "\r\n\">" }, StringSplitOptions.RemoveEmptyEntries))
            {
                if (str2.Length > 0)
                {
                    string[] strArray2 = str2.Split(new char[] { ',' });
                    if ((strArray2[0x16] != null) && (strArray2[0x17].Length > 0))
                    {
                        Person item = new Person();
                        if (!string.IsNullOrEmpty(strArray2[0x16]))
                        {
                            item.Name = strArray2[0x16];
                        }
                        else
                        {
                            item.Name = "暂无名称";
                        }
                        item.Email = strArray2[0x17];
                        list.Add(item);
                    }
                }
            }
            return list;
        }

        private string GetEntryUrl()
        {
            string format = string.Empty;
            if (this.en == Address163.Entrys.mailYeah)
            {
                format = "https://reg.163.com/logins.jsp?type=1&product=mailyeah&url=http://entry.mail.yeah.net/cgi/ntesdoor?lightweight%3D1%26verifycookie%3D1%26style%3D-1";
            }
            return string.Format(format, this.uName, this.pwd);
        }

        private string GetHtml()
        {
            string entryUrl = this.GetEntryUrl();
            return this.ProcessYeahmail(entryUrl);
        }

        private string ProcessYeahmail(string EntryUrl)
        {
            try
            {
                HttpWebResponse response;
                HttpWebRequest request = (HttpWebRequest) WebRequest.Create(new Uri(EntryUrl));
                request.Method = "POST";
                request.KeepAlive = false;
                request.ContentType = "application/x-www-form-urlencoded";
                request.ContentLength = this.b.Length;
                request.UserAgent = "Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 6.1; Trident/4.0; SLCC2; .NET CLR 2.0.50727; .NET CLR 3.5.30729; .NET CLR 3.0.30729; Media Center PC 6.0)";
                request.CookieContainer = this.cookieCon;
                using (Stream stream = request.GetRequestStream())
                {
                    stream.Write(this.b, 0, this.b.Length);
                }
                string str = string.Empty;
                using (response = (HttpWebResponse) request.GetResponse())
                {
                    StreamReader reader = new StreamReader(response.GetResponseStream(), Encoding.UTF8);
                    str = reader.ReadToEnd();
                    reader.Close();
                }
                this.reg = new Regex("HTTP-EQUIV=REFRESH CONTENT=\"[\\d];URL=(.+?)\">");
                this.m = this.reg.Match(str.ToUpper());
                if (this.m.Success)
                {
                    EntryUrl = this.m.Groups[1].Value.ToLower();
                }
                EntryUrl = TransCode(EntryUrl);
                Uri uri = new Uri("http://www.163.com");
                Uri uri2 = new Uri("http://reg.163.com");
                Uri uri3 = new Uri(EntryUrl);
                foreach (Cookie cookie in this.cookieCon.GetCookies(uri))
                {
                    cookie.Domain = uri3.Host;
                }
                this.cookieCon.Add(this.cookieCon.GetCookies(uri));
                foreach (Cookie cookie in this.cookieCon.GetCookies(uri2))
                {
                    cookie.Domain = uri3.Host;
                }
                this.cookieCon.Add(this.cookieCon.GetCookies(uri2));
                foreach (Cookie cookie in this.cookieCon.GetCookies(uri3))
                {
                    cookie.Domain = ".yeah.net";
                    cookie.Expires = DateTime.Now.AddHours(1.0);
                }
                this.cookieCon.Add(this.cookieCon.GetCookies(uri3));
                string responseHeader = string.Empty;
                request = (HttpWebRequest) WebRequest.Create(new Uri("http://entry.mail.yeah.net/cgi/ntesdoor?lightweight=1&verifycookie=1&style=-1&username=" + this.uName));
                request.UserAgent = "Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 6.1; Trident/4.0; SLCC2; .NET CLR 2.0.50727; .NET CLR 3.5.30729; .NET CLR 3.0.30729; Media Center PC 6.0)";
                request.CookieContainer = this.cookieCon;
                request.Method = "HEAD";
                request.AllowAutoRedirect = false;
                using (response = (HttpWebResponse) request.GetResponse())
                {
                    responseHeader = response.GetResponseHeader("Location");
                }
                Uri requestUri = new Uri(string.Format("http://webmail.mail.yeah.net/js3/s?sid=" + responseHeader.Remove(0, 0x2e) + "&func=pab%3AexportContacts&outformat=8", new object[0]));
                request = (HttpWebRequest) WebRequest.Create(requestUri);
                request.Method = "GET";
                request.CookieContainer = this.cookieCon;
                request.Accept = "text/javascript";
                HttpWebResponse response2 = (HttpWebResponse) request.GetResponse();
                return new StreamReader(response2.GetResponseStream(), Encoding.GetEncoding(response2.CharacterSet)).ReadToEnd();
            }
            catch (Exception exception)
            {
                return (exception.ToString() + "登录失败，请检查用户名称和密码");
            }
        }

        private static string TransCode(string str)
        {
            Regex regex = new Regex(@"&#([\d]{1,5})", RegexOptions.None);
            StringBuilder builder = new StringBuilder();
            foreach (Match match in regex.Matches(str))
            {
                builder.Append(((char) int.Parse(match.Groups[1].Value)).ToString());
            }
            return builder.ToString();
        }

        private enum ReqMethod
        {
            POST,
            GET
        }
    }
}

