namespace GetEmailAddress
{
    using System;
    using System.Collections.Generic;
    using System.IO;
    using System.Net;
    using System.Text;
    using System.Web.Security;

    public class AddressSohu
    {
        private CookieContainer cookieCon;
        protected static string cookieheader = string.Empty;
        private Entrys en;
        private string mailSohu;
        public string rb;
        private HttpWebRequest req;
        private HttpWebResponse res;

        private AddressSohu()
        {
            this.mailSohu = string.Empty;
            this.cookieCon = new CookieContainer();
            this.rb = string.Empty;
        }

        public AddressSohu(string name, string pwd, Entrys type)
        {
            this.mailSohu = string.Empty;
            this.cookieCon = new CookieContainer();
            this.rb = string.Empty;
            string str = name.Substring(0, name.IndexOf("@"));
            string str2 = FormsAuthentication.HashPasswordForStoringInConfigFile(pwd, "MD5");
            this.en = type;
            this.mailSohu = "http://passport.sohu.com/sso/login.jsp?userid=" + str + "%40sohu.com&password=" + str2.ToLower() + "&appid=1000&persistentcookie=0&isSLogin=1&s=1275999259296&b=5&w=1024&pwdtype=1&v=26";
        }

        public List<Person> getContact()
        {
            List<Person> list = new List<Person>();
            foreach (string str2 in this.GetHtml().Split(new string[] { "\r\n\">" }, StringSplitOptions.RemoveEmptyEntries))
            {
                if (str2.Length > 0)
                {
                    string[] strArray2 = str2.Split(new char[] { ',' });
                    if ((strArray2[12] != null) && (strArray2[13].Length > 0))
                    {
                        Person item = new Person();
                        item.Name = strArray2[12].Remove(0, 2).Trim();
                        item.Email = strArray2[13];
                        list.Add(item);
                    }
                }
            }
            return list;
        }

        private string GetHtml()
        {
            return this.ProcessSoHuMail(this.mailSohu);
        }

        private string ProcessSoHuMail(string EntryUrl)
        {
            try
            {
                string str = string.Empty;
                this.req = (HttpWebRequest) WebRequest.Create(EntryUrl);
                this.req.AllowAutoRedirect = true;
                this.req.CookieContainer = this.cookieCon;
                this.req.Credentials = CredentialCache.DefaultCredentials;
                this.req.Method = "GET";
                this.req.ContentType = "application/x-www-form-urlencoded";
                this.req.UserAgent = "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.2; SV1; GTB6; .NET CLR 1.1.4322; .NET CLR 2.0.50727; .NET CLR 3.0.04506.648; .NET CLR 3.5.21022)";
                this.res = (HttpWebResponse) this.req.GetResponse();
                if (cookieheader.Equals(string.Empty))
                {
                    cookieheader = this.req.CookieContainer.GetCookieHeader(new Uri(EntryUrl));
                }
                else
                {
                    this.req.CookieContainer.SetCookies(new Uri(EntryUrl), cookieheader);
                }
                str = new StreamReader(this.res.GetResponseStream(), Encoding.GetEncoding(this.res.CharacterSet)).ReadToEnd();
                if (str.Substring(str.IndexOf("'") + 1, (str.LastIndexOf("'") - str.IndexOf("'")) - 1) == "success")
                {
                    Uri requestUri = new Uri(string.Format("http://mail.sohu.com/address/export", new object[0]));
                    this.req = (HttpWebRequest) WebRequest.Create(requestUri);
                    this.req.Method = "GET";
                    this.req.CookieContainer = this.cookieCon;
                    this.req.Accept = "text/javascript";
                    HttpWebResponse response = (HttpWebResponse) this.req.GetResponse();
                    this.rb = new StreamReader(response.GetResponseStream(), Encoding.GetEncoding(response.CharacterSet)).ReadToEnd();
                }
                return this.rb;
            }
            catch (Exception exception)
            {
                return (exception.ToString() + "登录失败，请检查用户名称和密码");
            }
        }

        public enum Entrys
        {
            mailSohu
        }
    }
}

