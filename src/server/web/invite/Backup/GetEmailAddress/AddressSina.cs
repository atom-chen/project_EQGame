namespace GetEmailAddress
{
    using System;
    using System.Net;
    using System.Runtime.InteropServices;
    using System.Text;

    internal class AddressSina : AddressBase
    {
        private string refererurl;

        public override bool GetAddressPage(out string page, Encoding encoding)
        {
            page = null;
            string str = this.refererurl.Substring(7);
            str = str.Substring(0, str.IndexOf("/"));
            string url = "http://" + str + "/classic/addr_member.php";
            string s = "act=list&sort_item=letter&sort_type=desc";
            byte[] bytes = Encoding.UTF8.GetBytes(s);
            try
            {
                HttpWebResponse response = base.SendRequest("POST", url, this.refererurl, bytes);
                page = base.GetPageByResponse(response, encoding);
                string str4 = "http://" + str + "/classic/logout.php?from=mail";
                HttpWebResponse response2 = base.SendRequest("GET", str4);
            }
            catch
            {
                return false;
            }
            return true;
        }

        public override GetAddressStatus Login()
        {
            try
            {
                string url = "http://mail.sina.com.cn/cgi-bin/login.cgi";
                string s = "logintype=uid&u=" + base.Credential.UserName.Substring(0, base.Credential.UserName.IndexOf("@")) + "&psw=" + base.Credential.Password + "&btnloginfree=%B5%C7+%C2%BC";
                byte[] bytes = Encoding.UTF8.GetBytes(s);
                HttpWebResponse response = base.SendRequest("POST", url, "http://mail.sina.com.cn/index.html", bytes);
                this.refererurl = response.ResponseUri.ToString();
                if (this.refererurl == url)
                {
                    return GetAddressStatus.UidOrPwdError;
                }
            }
            catch
            {
                return GetAddressStatus.NetError;
            }
            return GetAddressStatus.Success;
        }
    }
}

