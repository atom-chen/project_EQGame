using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;

namespace Model
{
    [XmlRoot("root")]
    public class FunctionListInfo
    {
        [XmlElement("function")]
        public FunctionInfo[] FunctionList
        {
            get;
            set;
        }
    }
}
