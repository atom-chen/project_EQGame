namespace GetEmailAddress
{
    using System;
    using System.Net;
    using System.Text;

    internal class AddressTom : AddressBase
    {
        public override GetAddressStatus Login()
        {
            string url = "http://login.mail.tom.com/cgi/login";
            string s = "type=0&style=10&user=" + base.Credential.UserName.Substring(0, base.Credential.UserName.IndexOf("@")) + "&pass=" + base.Credential.Password + "&enter=%B5%C7%A1%A1%C2%BC";
            byte[] bytes = Encoding.GetEncoding("gb2312").GetBytes(s);
            try
            {
                HttpWebResponse response = base.SendRequest("POST", url, "http://mail.tom.com/", bytes);
                if (response.ResponseUri.ToString() == url)
                {
                    return GetAddressStatus.UidOrPwdError;
                }
                string str3 = response.ResponseUri.ToString();
                str3 = str3.Substring(7, str3.Length - 7);
                string str4 = str3.Substring(0, str3.IndexOf("/"));
                string str5 = str3.Substring(str3.IndexOf("sid=") + 4);
                base.AddressUrl = "http://" + str4 + "/cgi/ldvcapp?funcid=address&sid=" + str5 + "&tempname=address%2Faddress.htm&showlist=all&ifirstv=all&listnum=0&x=25&y=5";
            }
            catch
            {
                return GetAddressStatus.NetError;
            }
            return GetAddressStatus.Success;
        }
    }
}

