using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using Model;
using Logic;
using Logic.Command;

namespace ScriptEditor
{
    public partial class ItemDialog : Form
    {
        private OperateRecordManager operateRecordManager
        {
            get;
            set;
        }

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

        private int PreUniqueId
        {
            get;
            set;
        }

        private int UniqueId
        {
            get;
            set;
        }

        private ItemCommand ItemCommand;

        public ItemDialog(OperateRecordManager operateRecordManager, int pageIndex, int commandIndex, string prevCommand, int preUniqueId)
        {
            InitializeComponent();
            this.operateRecordManager = operateRecordManager;
            this.PageIndex = pageIndex;
            this.CommandIndex = commandIndex;
            this.prevCommand = prevCommand;
            this.PreUniqueId = preUniqueId;
            this.UniqueId = UniqueIdManager.Create();

            ItemCommand = operateRecordManager.AddItemCommand(this.PageIndex, this.CommandIndex, this.Rule.Text, this.Comment.Text, prevCommand, preUniqueId, UniqueId);
        }

        #region 事件
        private void Next_Click(object sender, EventArgs e)
        {
            if (!string.IsNullOrEmpty(this.Comment.Text))
            {
                operateRecordManager.UpdateItemCommand(ItemCommand, this.PageIndex, this.CommandIndex, this.Rule.Text, this.Comment.Text);
                this.Close();
            }
            else
            {
                MessageBox.Show("请录入内容！");
                this.Comment.Focus();
            }
        }

        private void AddOption_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            ItemOption option = new ItemOption(OptionDataGrid, ItemCommand);
            option.ShowDialog();
        }
        #endregion

        private void DeleteOption_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            int count = OptionDataGrid.SelectedRows.Count;
            int rowIndex = OptionDataGrid.SelectedRows[count - 1].Index;
            if (rowIndex >= 0)
            {
                ItemCommand.optionDialogInfo.OptionList.RemoveAt(rowIndex);
                OptionDataGrid.DataSource = new BindingList<OptionInfo>(ItemCommand.optionDialogInfo.OptionList);
            }
        }

        private void OptionDataGrid_CellDoubleClick(object sender, DataGridViewCellEventArgs e)
        {
            string commandName = OptionDataGrid.Rows[e.RowIndex].Cells[0].Value.ToString();
            DialogType dialogType = new DialogType(operateRecordManager, this.PageIndex + 1, e.RowIndex + 1, commandName, UniqueId);
            dialogType.ShowDialog();
        }
    }
}
