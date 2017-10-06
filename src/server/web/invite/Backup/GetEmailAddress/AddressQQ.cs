namespace GetEmailAddress
{
    using System;
    using System.Collections.Generic;
    using System.IO;
    using System.Net;
    using System.Text;
    using System.Text.RegularExpressions;
    using System.Xml;

    public class AddressQQ
    {
        private byte[] b;
        private CookieContainer cookieCon;
        protected static string cookieheader = string.Empty;
        private Address163.Entrys en;
        private Match m;
        private const string mailqq = "https://mail.qq.com/cgi-bin/loginpage";
        protected static string NextUrl = string.Empty;
        private string pwd;
        private Regex reg;
        private string uName;

        private AddressQQ()
        {
            this.cookieCon = new CookieContainer();
        }

        public AddressQQ(string name, string pwd, Address163.Entrys type)
        {
            this.cookieCon = new CookieContainer();
            this.uName = name;
            this.pwd = pwd;
            this.en = type;
            StringBuilder builder = new StringBuilder();
            builder.Append("&sid=zXDT6jyz,2,zh_CN");
            builder.Append("&firstlogin=false");
            builder.Append("&f=html");
            builder.Append("&ept=0");
            builder.Append("&uin=" + name);
            builder.Append("&p=" + pwd);
            builder.Append("&chg=0");
            builder.Append("&checkisWebLogin=9");
            builder.Append("&aliastype=@qq.com");
            builder.Append("&pp=00000000000");
            builder.Append("&verifycode=");
            this.b = Encoding.ASCII.GetBytes(builder.ToString());
        }

        public List<Person> getContact()
        {
            List<Person> list = new List<Person>();
            string xml = Encoding.UTF8.GetString(Encoding.Convert(Encoding.UTF8, Encoding.UTF8, Encoding.UTF8.GetBytes(this.GetHtml())));
            XmlDocument document = new XmlDocument();
            document.LoadXml(xml);
            XmlNodeList list2 = document.SelectNodes("/result/array/object");
            if ((list2 != null) && (list2.Count > 0))
            {
                XmlNodeList list3 = list2[0].SelectNodes("array/object");
                foreach (XmlNode node in list3)
                {
                    Person item = new Person();
                    foreach (XmlNode node2 in node.ChildNodes)
                    {
                        if (node2.Attributes["name"].Value == "EMAIL;PREF")
                        {
                            item.Email = node2.InnerText;
                        }
                        if (node2.Attributes["name"].Value == "FN")
                        {
                            if (!string.IsNullOrEmpty(node2.InnerText))
                            {
                                item.Name = node2.InnerText;
                            }
                            else
                            {
                                item.Name = "暂无名称";
                            }
                        }
                    }
                    list.Add(item);
                }
            }
            return list;
        }

        private string GetEntryUrl()
        {
            string format = string.Empty;
            if (this.en == Address163.Entrys.mailQQ)
            {
                format = "https://mail.qq.com/cgi-bin/loginpage";
            }
            return string.Format(format, this.uName, this.pwd);
        }

        private string GetHtml()
        {
            string entryUrl = this.GetEntryUrl();
            return this.Process126mail(entryUrl);
        }

        private string Process126mail(string EntryUrl)
        {
            try
            {
                Stream stream;
                HttpWebResponse response;
                StreamReader reader;
                HttpWebRequest request = (HttpWebRequest) WebRequest.Create(new Uri(EntryUrl));
                request.Method = "POST";
                request.KeepAlive = false;
                request.ContentType = "application/x-www-form-urlencoded";
                request.ContentLength = this.b.Length;
                request.UserAgent = "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.2; SV1; GTB6; .NET CLR 1.1.4322; .NET CLR 2.0.50727; .NET CLR 3.0.04506.648; .NET CLR 3.5.21022)";
                request.CookieContainer = this.cookieCon;
                using (stream = request.GetRequestStream())
                {
                    stream.Write(this.b, 0, this.b.Length);
                }
                string str = string.Empty;
                using (response = (HttpWebResponse) request.GetResponse())
                {
                    reader = new StreamReader(response.GetResponseStream(), Encoding.UTF8);
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
                    cookie.Domain = ".126.com";
                    cookie.Expires = DateTime.Now.AddHours(1.0);
                }
                this.cookieCon.Add(this.cookieCon.GetCookies(uri3));
                string responseHeader = string.Empty;
                request = (HttpWebRequest) WebRequest.Create(new Uri("http://entry.mail.126.com/cgi/ntesdoor?hid=10010102&lightweight=1&verifycookie=1&language=0&style=-1&username=" + this.uName));
                request.UserAgent = "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.2; SV1; GTB6; .NET CLR 1.1.4322; .NET CLR 2.0.50727; .NET CLR 3.0.04506.648; .NET CLR 3.5.21022)";
                request.CookieContainer = this.cookieCon;
                request.Method = "HEAD";
                request.AllowAutoRedirect = false;
                using (response = (HttpWebResponse) request.GetResponse())
                {
                    responseHeader = response.GetResponseHeader("Location");
                }
                string str4 = "<?xml version=\"1.0\"?><object><array name=\"items\"><object><string name=\"func\">pab:searchContacts</string><object name=\"var\"><array name=\"order\"><object><string name=\"field\">FN</string><boolean name=\"ignoreCase\">true</boolean></object></array></object></object><object><string name=\"func\">user:getSignatures</string></object><object><string name=\"func\">pab:getAllGroups</string></object></array></object>";
                byte[] bytes = Encoding.ASCII.GetBytes(str4.ToString());
                request = (HttpWebRequest) WebRequest.Create(new Uri((responseHeader + "&func=global:sequential").Replace("js3", "a").Replace("main.jsp", "s")));
                request.Method = "POST";
                request.ContentType = "application/xml";
                request.ContentLength = bytes.Length;
                request.UserAgent = "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.2; SV1; GTB6; .NET CLR 1.1.4322; .NET CLR 2.0.50727; .NET CLR 3.0.04506.648; .NET CLR 3.5.21022)";
                request.CookieContainer = this.cookieCon;
                using (stream = request.GetRequestStream())
                {
                    stream.Write(bytes, 0, bytes.Length);
                }
                using (response = (HttpWebResponse) request.GetResponse())
                {
                    reader = new StreamReader(response.GetResponseStream(), Encoding.UTF8);
                    str = reader.ReadToEnd();
                    reader.Close();
                }
                return str;
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

