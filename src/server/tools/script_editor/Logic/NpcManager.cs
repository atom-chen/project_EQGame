using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Model;
using Common;

namespace Logic
{
    public class NpcManager
    {
        private XmlManager xmlManager = new XmlManager();

        public List<NpcMapMapping> Get(int sceneId)
        {
            try
            {
                string fileName = string.Format(@"Template\npc_map_mapping_{0}_tplt.xml", sceneId);
                NpcMapMappingInfo npcMapMappingInfo = xmlManager.Deserialize<NpcMapMappingInfo>(DelegateUtils.GetVirtualPath() + fileName);
                return npcMapMappingInfo.npcMapMapping.ToList<NpcMapMapping>();
            }
            catch
            {
                return null;
            }
        }

        public string Get(List<NpcMapMapping> npcMapMappings, int npcId)
        {
            foreach (NpcMapMapping npcMapMapping in npcMapMappings)
            {
                if (npcId == npcMapMapping.npc_id)
                    return npcMapMapping.id.ToString();
            }

            return null;
        }
    }
}
