using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using Model;
using Common;
using Logic;

namespace ScriptEditor
{
    public partial class DialogType : Form
    {
        #region 实体
        private OperateRecordManager operateRecordManager
        {
            get;
            set;
        }

        /// <summary>
        /// 页面索引值
        /// </summary>
        private int PageIndex
        {
            get;
            set;
        }

        private int CommandIndex
        {
            get;
            set;
        }

        private string prevCommand
        {
            get;
            set;
        }

        public int PreUniqueId
        {
            get;
            set;
        }
        #endregion

        public DialogType(OperateRecordManager operateRecordManager, int pageIndex, int commandIndex, string prevCommand, int preUniqueId)
        {
            InitializeComponent();
            this.operateRecordManager = operateRecordManager;
            this.PageIndex = pageIndex;
            this.CommandIndex = commandIndex;
            this.prevCommand = prevCommand;
            this.PreUniqueId = preUniqueId;

            BindDialogDropDownList();
        }

        #region 方法
        private List<DialogTypeInfo> InitDialogData()
        {
            List<DialogTypeInfo> dialogTypes = new List<DialogTypeInfo>();
            dialogTypes.Add(new DialogTypeInfo() { id = 1, name = "信息（不带选项）" });
            dialogTypes.Add(new DialogTypeInfo() { id = 2, name = "信息（带选项）" });
            dialogTypes.Add(new DialogTypeInfo() { id = 3, name = "信息（带物品）"});
            dialogTypes.Add(new DialogTypeInfo() { id = 4, name = "关闭窗口" });

            return dialogTypes;
        }

        private void BindDialogDropDownList()
        {
            DialogDropDownList.DataSource = InitDialogData();
            DialogDropDownList.DisplayMember = "name";
            DialogDropDownList.ValueMember = "id";
        }
        #endregion

        #region 事件
        private void Next_Click(object sender, EventArgs e)
        {
            int typeId = Utils.TryToInt(DialogDropDownList.SelectedValue, 0);
            if(typeId > 0)
            {
                this.Hide();
                Form dialog = DialogFactory.Create(operateRecordManager, typeId, PageIndex, CommandIndex, prevCommand, PreUniqueId);
                if (dialog != null)
                    dialog.ShowDialog();
                    
            }
        }
        #endregion
    }
}
