using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Model;
using Common;

namespace Logic
{
    public class MapManager
    {
        private XmlManager xmlManager = new XmlManager();

        public List<common_scene> Get()
        {
            SceneInfo sceneInfo = xmlManager.Deserialize<SceneInfo>(DelegateUtils.GetVirtualPath() + @"Template\common_scene.xml");
            return sceneInfo.common_scene.ToList<common_scene>();
        }
    }
}
