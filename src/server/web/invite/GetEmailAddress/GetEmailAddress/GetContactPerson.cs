namespace GetEmailAddress
{
    using MSNPSharp;
    using System;
    using System.Collections.Generic;
    using System.Runtime.InteropServices;
    using System.Text;
    using System.Web.UI;

    public class GetContactPerson
    {
        public static GetAddressStatus GetAddressByEmail(string mailaddress, string password, out IList<Person> emaillist)
        {
            ContactList list2;
            emaillist = null;
            List<Person> list = new List<Person>();
            GetAddressStatus status = new GetAddressByMsn(null).GetMSNContactList(mailaddress, password, out list2);
            foreach (KeyValuePair<int, Contact> pair in list2)
            {
                Person item = new Person();
                item.Name = string.IsNullOrEmpty(pair.Value.NickName) ? pair.Value.Name : pair.Value.NickName;
                item.Email = pair.Value.Mail;
                list.Add(item);
            }
            emaillist = list;
            return status;
        }

        public static GetAddressStatus GetAddressByEmail(string mailaddress, string password, Page workingPage, out IList<Person> emaillist)
        {
            GetAddressStatus success;
            Address163 address;
            AddressHotMail mail;
            emaillist = null;
            string[] strArray = mailaddress.Split(new char[] { '@' });
            if ((strArray == null) || (strArray.Length != 2))
            {
                return GetAddressStatus.MailError;
            }
            if (strArray[1].IndexOf("yahoo") >= 0)
            {
                AddressYahoo yahoo = new AddressYahoo();
                success = yahoo.GetAddressList(mailaddress, password, Encoding.UTF8, out emaillist);
                yahoo = null;
                return success;
            }
            switch (strArray[1].ToLower().Trim())
            {
                case "163.com":
                    address = new Address163(mailaddress, password, Address163.Entrys.mail163);
                    success = GetAddressStatus.Success;
                    emaillist = address.getContact();
                    if (emaillist.Count > 0)
                    {
                        success = GetAddressStatus.Success;
                        break;
                    }
                    success = GetAddressStatus.NoAddress;
                    break;

                case "126.com":
                {
                    Address126 address2 = new Address126(mailaddress, password, Address163.Entrys.mail126);
                    success = GetAddressStatus.Success;
                    emaillist = address2.getContact();
                    if (emaillist.Count > 0)
                    {
                        success = GetAddressStatus.Success;
                    }
                    else
                    {
                        success = GetAddressStatus.NoAddress;
                    }
                    address2 = null;
                    return success;
                }
                case "yeah.net":
                {
                    AddressYeah yeah = new AddressYeah(mailaddress, password, Address163.Entrys.mailYeah);
                    success = GetAddressStatus.Success;
                    emaillist = yeah.getContact();
                    if (emaillist.Count > 0)
                    {
                        success = GetAddressStatus.Success;
                    }
                    else
                    {
                        success = GetAddressStatus.NoAddress;
                    }
                    yeah = null;
                    return success;
                }
                case "sina.com":
                    success = new AddressSina().GetAddressList(mailaddress, password, Encoding.UTF8, out emaillist);
                    return success;

                case "sohu.com":
                {
                    AddressSohu sohu = new AddressSohu(mailaddress, password, AddressSohu.Entrys.mailSohu);
                    success = GetAddressStatus.Success;
                    emaillist = sohu.getContact();
                    if (emaillist.Count > 0)
                    {
                        success = GetAddressStatus.Success;
                    }
                    else
                    {
                        success = GetAddressStatus.NoAddress;
                    }
                    sohu = null;
                    return success;
                }
                case "tom.com":
                    success = new AddressTom().GetAddressList(mailaddress, password, Encoding.GetEncoding("gb2312"), out emaillist);
                    return success;

                case "hotmail.com":
                    mail = new AddressHotMail(workingPage);
                    success = mail.GetAddressList(mailaddress, password);
                    emaillist = mail.GetContactList();
                    mail = null;
                    return success;

                case "live.com":
                {
                    AddressHotMail mail2 = new AddressHotMail(workingPage);
                    success = mail2.GetAddressList(mailaddress, password);
                    emaillist = mail2.GetContactList();
                    mail = null;
                    return success;
                }
                case "live.cn":
                {
                    AddressHotMail mail3 = new AddressHotMail(workingPage);
                    success = mail3.GetAddressList(mailaddress, password);
                    emaillist = mail3.GetContactList();
                    mail = null;
                    return success;
                }
                case "gmail.com":
                {
                    AddressGmail gmail = new AddressGmail(mailaddress, password, AddressGmail.Entrys.mailgmail);
                    success = GetAddressStatus.Success;
                    emaillist = gmail.getContact();
                    if (emaillist.Count > 0)
                    {
                        success = GetAddressStatus.Success;
                    }
                    else
                    {
                        success = GetAddressStatus.NoAddress;
                    }
                    gmail = null;
                    return success;
                }
                default:
                    return GetAddressStatus.NotAuspice;
            }
            address = null;
            return success;
        }
    }
}

