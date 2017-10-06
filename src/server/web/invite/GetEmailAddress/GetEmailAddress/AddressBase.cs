namespace GetEmailAddress
{
    using System;
    using System.Collections.Generic;
    using System.Collections.Specialized;
    using System.IO;
    using System.Net;
    using System.Runtime.InteropServices;
    using System.Text;
    using System.Text.RegularExpressions;

    internal class AddressBase
    {
        private string _addressurl;
        private CookieContainer _container = new CookieContainer();
        private NetworkCredential _credential;

        public GetAddressStatus GetAddressList(string username, string password, Encoding encoding, out IList<Person> emails)
        {
            this._credential = new NetworkCredential(username, password);
            emails = null;
            switch (this.Login())
            {
                case GetAddressStatus.Success:
                {
                    string page = null;
                    if (!this.GetAddressPage(out page, encoding))
                    {
                        return GetAddressStatus.NetError;
                    }
                    if (!this.GetEmails(page, out emails))
                    {
                        return GetAddressStatus.NoAddress;
                    }
                    return GetAddressStatus.Success;
                }
                case GetAddressStatus.UidOrPwdError:
                    return GetAddressStatus.UidOrPwdError;

                case GetAddressStatus.NetError:
                    return GetAddressStatus.NetError;
            }
            return GetAddressStatus.Success;
        }

        public virtual bool GetAddressPage(out string page, Encoding encoding)
        {
            page = null;
            try
            {
                HttpWebResponse response = this.SendRequest("GET", this._addressurl);
                page = this.GetPageByResponse(response, encoding);
            }
            catch
            {
                return false;
            }
            return true;
        }

        public bool GetEmails(string page, out IList<Person> emails)
        {
            StringCollection strings = new StringCollection();
            emails = new List<Person>();
            string pattern = @"([\w-\.]+)@((\[[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.)|(([\w-]+\.)+))([a-zA-Z]{2,4}|[0-9]{1,3})(\]?)";
            for (Match match = Regex.Match(page, pattern); match.Success; match = match.NextMatch())
            {
                string str2 = match.ToString();
                if (!strings.Contains(str2) && (str2 != this._credential.UserName))
                {
                    Person item = new Person();
                    item.Email = str2;
                    if (!string.IsNullOrEmpty(match.Groups["txtName"].Value.ToString()))
                    {
                        item.Name = match.Groups["txtName"].Value.ToString();
                    }
                    else
                    {
                        item.Name = "暂无名称";
                    }
                    emails.Add(item);
                }
            }
            if (emails.Count <= 0)
            {
                return false;
            }
            return true;
        }

        public string GetPageByResponse(HttpWebResponse response, Encoding encoding)
        {
            string str = null;
            using (Stream stream = response.GetResponseStream())
            {
                using (StreamReader reader = new StreamReader(stream, encoding))
                {
                    str = reader.ReadToEnd();
                }
            }
            return str;
        }

        public virtual GetAddressStatus Login()
        {
            return GetAddressStatus.Success;
        }

        public HttpWebResponse SendRequest(string method, string url)
        {
            return this.SendRequest(method, url, null);
        }

        public HttpWebResponse SendRequest(string method, string url, string referer)
        {
            return this.SendRequest(method, url, referer, null);
        }

        public HttpWebResponse SendRequest(string method, string url, string referer, byte[] buffer)
        {
            HttpWebRequest request = WebRequest.Create(new Uri(url)) as HttpWebRequest;
            request.Method = method;
            request.Timeout = 0x2710;
            request.CookieContainer = this._container;
            request.ContentType = "application/x-www-form-urlencoded";
            request.UserAgent = "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 1.1.4322; CIBA; .NET CLR 2.0.50727)";
            if (method.ToUpper() == "POST")
            {
                Stream requestStream = request.GetRequestStream();
                requestStream.Write(buffer, 0, buffer.Length);
                requestStream.Close();
            }
            return (request.GetResponse() as HttpWebResponse);
        }

        public string AddressUrl
        {
            get
            {
                return this._addressurl;
            }
            set
            {
                this._addressurl = value;
            }
        }

        public CookieContainer Container
        {
            get
            {
                return this._container;
            }
            set
            {
                this._container = value;
            }
        }

        public NetworkCredential Credential
        {
            get
            {
                return this._credential;
            }
            set
            {
                this._credential = value;
            }
        }
    }
}

