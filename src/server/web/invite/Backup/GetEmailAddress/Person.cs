namespace GetEmailAddress
{
    using System;

    public class Person
    {
        private string _email;
        private string _Name;

        public string Email
        {
            get
            {
                return this._email;
            }
            set
            {
                this._email = value;
            }
        }

        public string Name
        {
            get
            {
                return this._Name;
            }
            set
            {
                this._Name = value;
            }
        }
    }
}

