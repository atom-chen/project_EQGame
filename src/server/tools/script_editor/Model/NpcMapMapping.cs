using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;

namespace Model
{
    public class NpcMapMapping
    {
        [XmlElement("id")]
        public int id
        {
            get;
            set;
        }

        [XmlElement("npc_id")]
        public int npc_id
        {
            get;
            set;
        }

        [XmlElement("npc_name")]
        public string npc_name
        {
            get;
            set;
        }

        [XmlElement("x")]
        public decimal x
        {
            get;
            set;
        }

        [XmlElement("y")]
        public decimal y
        {
            get;
            set;
        }

        [XmlElement("z")]
        public decimal z
        {
            get;
            set;
        }

        [XmlElement("script_id")]
        public int script_id
        {
            get;
            set;
        }

        [XmlElement("action")]
        public int action
        {
            get;
            set;
        }

        [XmlElement("key")]
        public string key
        {
            get;
            set;
        }
    }
}
