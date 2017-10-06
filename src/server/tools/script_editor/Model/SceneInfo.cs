using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;

namespace Model
{
    [XmlRoot("root")]
    public class SceneInfo
    {
        [XmlElement("common_scene")]
        public common_scene[] common_scene
        {
            get;
            set;
        }
    }
}
