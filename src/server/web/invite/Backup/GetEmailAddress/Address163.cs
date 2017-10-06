namespace GetEmailAddress
{
    using System;
    using System.Collections.Generic;
    using System.IO;
    using System.Net;
    using System.Text;
    using System.Xml;

    public class Address163
    {
        private CookieContainer cookieCon;
        protected static string cookieheader = string.Empty;
        private Entrys en;
        private const string mail_163_com = "https://reg.163.com/logins.jsp?username={0}&password={1}&type=1&url=http://entry.mail.163.com/coremail/fcg/ntesdoor2?lightweight%3D1%26verifycookie%3D1%26language%3D-1%26style%3D-1 ";
        protected static string NextUrl = string.Empty;
        private string pwd;
        private HttpWebRequest req;
        private HttpWebResponse res;
        private string uName;

        private Address163()
        {
            this.cookieCon = new CookieContainer();
        }

        public Address163(string name, string pwd, Entrys type)
        {
            this.cookieCon = new CookieContainer();
            this.uName = name;
            this.pwd = pwd;
            this.en = type;
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
            if (this.en == Entrys.mail163)
            {
                format = "https://reg.163.com/logins.jsp?username={0}&password={1}&type=1&url=http://entry.mail.163.com/coremail/fcg/ntesdoor2?lightweight%3D1%26verifycookie%3D1%26language%3D-1%26style%3D-1 ";
            }
            return string.Format(format, this.uName, this.pwd);
        }

        private string GetHtml()
        {
            string entryUrl = this.GetEntryUrl();
            return this.Process163mail(entryUrl);
        }

        private string GetRequestHtml(string URL)
        {
            return this.GetRequestHtml(URL, Encoding.Default);
        }

        private string GetRequestHtml(string URL, Encoding EnCodeing)
        {
            return this.GetRequestHtml(URL, EnCodeing, ReqMethod.POST);
        }

        private string GetRequestHtml(string URL, Encoding EnCodeing, ReqMethod RMethod)
        {
            try
            {
                this.req = (HttpWebRequest) WebRequest.Create(URL);
                this.req.AllowAutoRedirect = true;
                this.req.CookieContainer = this.cookieCon;
                this.req.Credentials = CredentialCache.DefaultCredentials;
                this.req.Method = RMethod.ToString();
                this.req.ContentType = "application/x-www-form-urlencoded";
                this.req.UserAgent = "Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 6.0; GTB5; Mozilla/4.0(Compatible Mozilla/4.0(Compatible-EmbeddedWB 14.59 http://bsalsa.com/ EmbeddedWB- 14.59  from: http://bsalsa.com/ ; SLCC1; .NET CLR 2.0.50727; Media Center PC 5.0; InfoPath.2; .NET CLR 3.5.30729; .NET CLR 3.0.30618; CIBA)";
                this.res = (HttpWebResponse) this.req.GetResponse();
                if (cookieheader.Equals(string.Empty))
                {
                    cookieheader = this.req.CookieContainer.GetCookieHeader(new Uri(URL));
                }
                else
                {
                    this.req.CookieContainer.SetCookies(new Uri(URL), cookieheader);
                }
                return new StreamReader(this.res.GetResponseStream(), EnCodeing).ReadToEnd();
            }
            catch (Exception exception)
            {
                return exception.Message;
            }
        }

        private string Process163mail(string EntryUrl)
        {
            try
            {
                string requestHtml = this.GetRequestHtml(EntryUrl, Encoding.GetEncoding("utf-8"));
                NextUrl = requestHtml.Substring(requestHtml.IndexOf("URL=") + 6);
                NextUrl = NextUrl.Substring(0, NextUrl.IndexOf("\""));
                string[] strArray = NextUrl.Split(new string[] { "&#" }, StringSplitOptions.RemoveEmptyEntries);
                string str2 = string.Empty;
                for (int i = 0; i < strArray.Length; i++)
                {
                    int num2 = int.Parse(strArray[i]);
                    str2 = str2 + Encoding.ASCII.GetString(new byte[] { (byte) num2 });
                }
                NextUrl = str2;
                NextUrl = "http://entry.mail.163.com/coremail/fcg/ntesdoor2?lightweight=1&verifycookie=1&language=-1&style=35&username=" + this.uName.Replace("@163.com", "");
                requestHtml = this.GetRequestHtml(NextUrl, Encoding.GetEncoding("gb2312"));
                string str3 = "<?xml version=\"1.0\"?><object><array name=\"items\"><object><string name=\"func\">pab:searchContacts</string><object name=\"var\"><array name=\"order\"><object><string name=\"field\">FN</string><boolean name=\"ignoreCase\">true</boolean></object></array></object></object><object><string name=\"func\">user:getSignatures</string></object><object><string name=\"func\">pab:getAllGroups</string></object></array></object>";
                byte[] bytes = Encoding.ASCII.GetBytes(str3.ToString());
                NextUrl = this.res.ResponseUri.AbsoluteUri;
                string str4 = (NextUrl + "&func=global:sequential").Replace("js3", "a");
                this.req = (HttpWebRequest) WebRequest.Create(new Uri(str4.Replace("main.jsp", "s")));
                this.req.Method = "POST";
                this.req.ContentType = "application/xml";
                this.req.ContentLength = bytes.Length;
                this.req.UserAgent = "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.2; SV1; GTB6; .NET CLR 1.1.4322; .NET CLR 2.0.50727; .NET CLR 3.0.04506.648; .NET CLR 3.5.21022)";
                this.req.CookieContainer = this.cookieCon;
                using (Stream stream = this.req.GetRequestStream())
                {
                    stream.Write(bytes, 0, bytes.Length);
                }
                using (HttpWebResponse response = (HttpWebResponse) this.req.GetResponse())
                {
                    StreamReader reader = new StreamReader(response.GetResponseStream(), Encoding.UTF8);
                    requestHtml = reader.ReadToEnd();
                    reader.Close();
                }
                return requestHtml;
            }
            catch (Exception exception)
            {
                return (exception.ToString() + "登录失败，请检查用户名称和密码");
            }
        }

        public enum Entrys
        {
            mail163,
            mail126,
            mailQQ,
            mailYeah
        }

        private enum ReqMethod
        {
            POST,
            GET
        }
    }
}

