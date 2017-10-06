namespace GetEmailAddress
{
    using MSNPSharp;
    using System;
    using System.Collections.Generic;
    using System.ComponentModel;
    using System.Threading;
    using System.Web.UI;

    public class AddressHotMail
    {
        private GetAddressStatus _addressStatus = GetAddressStatus.UidOrPwdError;
        private Page _workingPage;
        private AutoResetEvent are;
        private Messenger messenger = new Messenger();
        private int tmpCount = 1;

        public AddressHotMail(Page workingPage)
        {
            this._workingPage = workingPage;
        }

        public GetAddressStatus GetAddressList(string username, string password)
        {
            this.Login(username, password);
            return this._addressStatus;
        }

        public GetAddressStatus GetAddressList(string username, string password, bool IsPageAsync)
        {
            this.Login(username, password, IsPageAsync);
            return (this.messenger.Nameserver.IsSignedIn ? GetAddressStatus.Success : GetAddressStatus.NetError);
        }

        public List<Person> GetContactList()
        {
            List<Person> list = new List<Person>();
            Dictionary<int, Contact> contactList = this.messenger.Nameserver.ContactList;
            foreach (KeyValuePair<int, Contact> pair in contactList)
            {
                string str = string.IsNullOrEmpty(pair.Value.NickName) ? pair.Value.Name : pair.Value.NickName;
                Person item = new Person();
                if (!string.IsNullOrEmpty(str))
                {
                    item.Name = str;
                }
                else
                {
                    item.Name = "暂无名称";
                }
                item.Email = pair.Value.Mail;
                list.Add(item);
            }
            return list;
        }

        private void Login(string username, string password)
        {
            this.are = new AutoResetEvent(false);
            SynchronizationContext synchronizationContext = AsyncOperationManager.SynchronizationContext;
            try
            {
                AsyncOperationManager.SynchronizationContext = new SynchronizationContext();
                this.messenger.Nameserver.SignedIn += new EventHandler<EventArgs>(this.Nameserver_SignedIn);
                this.messenger.NameserverProcessor.ConnectingException += new EventHandler<ExceptionEventArgs>(this.NameserverProcessor_ConnectingException);
                this.messenger.Nameserver.ExceptionOccurred += new EventHandler<ExceptionEventArgs>(this.Nameserver_ExceptionOccurred);
                this.messenger.Nameserver.AuthenticationError += new EventHandler<ExceptionEventArgs>(this.Nameserver_AuthenticationError);
                if (this.messenger.Connected)
                {
                    this.messenger.Disconnect();
                }
                this.messenger.Credentials = new Credentials(username, password, (MsnProtocol) Enum.Parse(typeof(MsnProtocol), MsnProtocol.MSNP18.ToString()));
                this.messenger.Connect();
                this.are.WaitOne();
            }
            finally
            {
                AsyncOperationManager.SynchronizationContext = synchronizationContext;
            }
        }

        private void Login(string username, string password, bool IsPageAsync)
        {
            if (IsPageAsync)
            {
                SynchronizationContext synchronizationContext = AsyncOperationManager.SynchronizationContext;
                try
                {
                    AsyncOperationManager.SynchronizationContext = new SynchronizationContext();
                    this.messenger.Credentials = new Credentials(username, password, (MsnProtocol) Enum.Parse(typeof(MsnProtocol), MsnProtocol.MSNP18.ToString()));
                    this.messenger.Connect();
                    Thread.Sleep(0xbb8);
                    while (this.tmpCount < 6)
                    {
                        if (!(this.messenger.Connected && !this.messenger.Nameserver.IsSignedIn))
                        {
                            this.tmpCount++;
                            this.Login(username, password, IsPageAsync);
                        }
                    }
                }
                finally
                {
                    AsyncOperationManager.SynchronizationContext = synchronizationContext;
                }
            }
        }

        private void Nameserver_AuthenticationError(object sender, ExceptionEventArgs e)
        {
            this.are.Set();
            this._addressStatus = GetAddressStatus.UidOrPwdError;
        }

        private void Nameserver_ExceptionOccurred(object sender, ExceptionEventArgs e)
        {
            this.are.Set();
            if (!(e.Exception is UnauthorizedException))
            {
                this._addressStatus = GetAddressStatus.NotAuspice;
            }
        }

        private void Nameserver_SignedIn(object sender, EventArgs e)
        {
            this.are.Set();
            this._addressStatus = GetAddressStatus.Success;
        }

        private void NameserverProcessor_ConnectingException(object sender, ExceptionEventArgs e)
        {
            this.are.Set();
            this._addressStatus = GetAddressStatus.MailError;
        }
    }
}

