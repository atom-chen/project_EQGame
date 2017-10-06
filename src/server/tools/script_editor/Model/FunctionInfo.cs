using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;

namespace Model
{
    public class FunctionInfo
    {
        [XmlElement("id")]
        public int Id
        {
            get;
            set;
        }

        [XmlElement("name")]
        public string Name
        {
            get;
            set;
        }

        [XmlElement("remark")]
        public string Remark
        {
            get;
            set;
        }

        [XmlElement("type")]
        public string Type
        {
            get;
            set;
        }
    }
}
