namespace GetEmailAddress
{
    using System;

    public enum GetAddressStatus
    {
        Success,
        UidOrPwdError,
        NoAddress,
        NetError,
        MailError,
        NotAuspice
    }
}

