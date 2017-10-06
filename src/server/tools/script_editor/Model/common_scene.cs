using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;

namespace Model
{
    [Serializable]
    public class common_scene
    {
        [XmlElement("id")]
        public int id
        {
            get;
            set;
        }

        [XmlElement("name")]
        public string name
        {
            get;
            set;
        }

        [XmlElement("file_res")]
        public string file_res
        {
            get;
            set;
        }
    }
}
