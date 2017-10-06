namespace GetEmailAddress
{
    using System;
    using System.Net;
    using System.Text;
    using System.Text.RegularExpressions;

    internal class AddressYahoo : AddressBase
    {
        private const string _addressBookUrl = "http://address.yahoo.com/yab/us/index.php?VPC=contact_list&.rand=74651234";
        private const string _authUrl = "https://login.yahoo.com/config/login?";
        private const string _loginPage = "https://login.yahoo.com/config/login";

        private string GetPostData(string page)
        {
            string str5;
            string str = "";
            Regex regex = new Regex("type=\"hidden\" name=\"(.*?)\" value=\"(.*?)\"", RegexOptions.IgnoreCase);
            for (Match match = regex.Match(page); match.Success; match = regex.Match(page, (int) (match.Index + match.Length)))
            {
                if (match.Groups[0].Value.Length > 0)
                {
                    string str2 = match.Groups[1].Value;
                    string str3 = match.Groups[2].Value;
                    if (str != "")
                    {
                        str5 = str;
                        str = str5 + "&" + str2 + "=" + str3;
                    }
                    else
                    {
                        str = str + str2 + "=" + str3;
                    }
                }
            }
            str5 = str;
            return (str5 + "&.save=Sign In&.persistent=y&login=" + base.Credential.UserName + "&passwd=" + base.Credential.Password);
        }

        public override GetAddressStatus Login()
        {
            try
            {
                HttpWebResponse response = base.SendRequest("GET", "https://login.yahoo.com/config/login");
                string pageByResponse = base.GetPageByResponse(response, Encoding.UTF8);
                string postData = this.GetPostData(pageByResponse);
                if (base.SendRequest("POST", "https://login.yahoo.com/config/login?", "https://login.yahoo.com/config/login", Encoding.ASCII.GetBytes(postData)).Cookies.Count <= 0)
                {
                    return GetAddressStatus.UidOrPwdError;
                }
                base.AddressUrl = "http://address.yahoo.com/yab/us/index.php?VPC=contact_list&.rand=74651234";
            }
            catch
            {
                return GetAddressStatus.NetError;
            }
            return GetAddressStatus.Success;
        }
    }
}

