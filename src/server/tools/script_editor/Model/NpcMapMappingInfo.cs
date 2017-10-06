using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;

namespace Model
{
    [XmlRoot("root")]
    public class NpcMapMappingInfo
    {
        [XmlElement("npc_map_mapping_tplt")]
        public NpcMapMapping[] npcMapMapping
        {
            get;
            set;
        }
    }
}
