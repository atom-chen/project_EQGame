namespace GetEmailAddress
{
    using MSNPSharp;
    using System;
    using System.ComponentModel;
    using System.Runtime.InteropServices;
    using System.Threading;
    using System.Web.UI;

    public class GetAddressByMsn
    {
        private GetAddressStatus _status = GetAddressStatus.Success;
        private Page _workingPage;
        private AutoResetEvent are;
        private Messenger messenger = new Messenger();
        private int tmpCount = 1;

        public GetAddressByMsn(Page workingPage)
        {
            this._workingPage = workingPage;
        }

        public ContactList GetMSNContactList(string username, string password)
        {
            if (!this.messenger.Nameserver.IsSignedIn)
            {
                this.Login(username, password);
            }
            return this.messenger.Nameserver.ContactList;
        }

        public ContactList GetMSNContactList(string username, string password, bool IsPageAsync)
        {
            if (!this.messenger.Nameserver.IsSignedIn)
            {
                this.Login(username, password, IsPageAsync);
            }
            return this.messenger.Nameserver.ContactList;
        }

        public GetAddressStatus GetMSNContactList(string username, string password, out ContactList list)
        {
            if (!this.messenger.Nameserver.IsSignedIn)
            {
                this.Login(username, password);
            }
            list = this.messenger.Nameserver.ContactList;
            return this._status;
        }

        public ContactList GetMSNContactList1(string username, string password, bool IsPageAsync)
        {
            if (!this.messenger.Nameserver.IsSignedIn)
            {
                this.Login(username, password, IsPageAsync);
            }
            return this.messenger.Nameserver.ContactList;
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
                        if (!(this.messenger.Connected && this.messenger.Nameserver.IsSignedIn))
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
            this._status = GetAddressStatus.UidOrPwdError;
        }

        private void Nameserver_ExceptionOccurred(object sender, ExceptionEventArgs e)
        {
            this.are.Set();
            if (!(e.Exception is UnauthorizedException))
            {
            }
        }

        private void Nameserver_SignedIn(object sender, EventArgs e)
        {
            this.are.Set();
            this._status = GetAddressStatus.Success;
        }

        private void NameserverProcessor_ConnectingException(object sender, ExceptionEventArgs e)
        {
            this.are.Set();
            this._status = GetAddressStatus.MailError;
        }
    }
}

