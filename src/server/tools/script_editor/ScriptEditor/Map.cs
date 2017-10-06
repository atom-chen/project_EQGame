using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using Logic;
using Model;
using Common;

namespace ScriptEditor
{
    public partial class Map : Form
    {
        private MapManager mapManager = new MapManager();
        private NpcManager npcManager = new NpcManager();
        private OperateRecordManager OperateRecordManager;

        public Map(OperateRecordManager operateRecordManager)
        {
            InitializeComponent();
            this.OperateRecordManager = operateRecordManager;
            BindMapDropDownList();
        }

        #region 方法
        private void BindMapDropDownList()
        {
            List<common_scene> CommonScenes = mapManager.Get();
            CommonScenes.Insert(0, new common_scene());
            if (CommonScenes != null)
            {
                MapDropDownList.DataSource = CommonScenes;
                MapDropDownList.DisplayMember = "name";
                MapDropDownList.ValueMember = "id";
            }
            else
            {
                MapDropDownList.DataSource = null;
            }
        }

        private void BindNpcDropDownlist(int sceneId)
        {
            List<NpcMapMapping> NpcMapMappings = OperateRecordManager.NpcMapMappings = npcManager.Get(sceneId);
            if (NpcMapMappings != null)
            {
                NpcDropDownList.DataSource = NpcMapMappings;
                NpcDropDownList.DisplayMember = "npc_name";
                NpcDropDownList.ValueMember = "npc_id";
            }
            else
            {
                NpcDropDownList.DataSource = null;
            }
        }
        #endregion

        #region 事件
        private void Next_Click(object sender, EventArgs e)
        {
            if (Utils.ValidateIsSelected(MapDropDownList.SelectedValue) && Utils.ValidateIsSelected(NpcDropDownList.SelectedValue))
            {
                OperateRecordManager.SetOperateRecordInfo(
                    Utils.TryToInt(MapDropDownList.SelectedValue, 0),
                    Utils.TryToInt(NpcDropDownList.SelectedValue, 0)
                    );
                this.Hide();
                DialogType dialogType = new DialogType(OperateRecordManager, 0, 0, "", UniqueIdManager.ReCreate());
                dialogType.ShowDialog();
            }
            else
            {
                MessageBox.Show("请选择地图以及NPC!");
            }
        }

        private void MapDropDownList_SelectedIndexChanged(object sender, EventArgs e)
        {
            int sceneId = Utils.TryToInt(((ComboBox)sender).SelectedValue, 0);
            BindNpcDropDownlist(sceneId);
        }
        #endregion
    }
}
