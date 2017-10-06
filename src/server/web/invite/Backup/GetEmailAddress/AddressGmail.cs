namespace GetEmailAddress
{
    using Google.Contacts;
    using Google.GData.Client;
    using Google.GData.Extensions;
    using System;
    using System.Collections.Generic;

    public class AddressGmail
    {
        private Entrys en;
        private string pwd;
        private string uName;

        private AddressGmail()
        {
        }

        public AddressGmail(string name, string pwd, Entrys type)
        {
            this.uName = name;
            this.pwd = pwd;
            this.en = type;
        }

        public List<Person> getContact()
        {
            List<Person> list = new List<Person>();
            RequestSettings settings = new RequestSettings("", this.uName, this.pwd);
            settings.AutoPaging = true;
            Feed<Contact> contacts = new ContactsRequest(settings).GetContacts();
            foreach (Contact contact in contacts.Entries)
            {
                Person item = null;
                foreach (EMail mail in contact.Emails)
                {
                    item = new Person();
                    if (!string.IsNullOrEmpty(contact.Title))
                    {
                        item.Name = contact.Title;
                    }
                    else
                    {
                        item.Name = "暂无名称";
                    }
                    item.Email = mail.Address;
                    list.Add(item);
                }
            }
            return list;
        }

        public enum Entrys
        {
            mailgmail
        }
    }
}

