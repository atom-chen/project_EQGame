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
    public partial class OptionDialog : Form
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

        private OptionCommand OptionCommand;

        public OptionDialog(OperateRecordManager operateRecordManager, int pageIndex, int commandIndex, string prevCommand, int preUniqueId)
        {
            InitializeComponent();
            this.operateRecordManager = operateRecordManager;
            this.PageIndex = pageIndex;
            this.CommandIndex = commandIndex;
            this.prevCommand = prevCommand;
            this.PreUniqueId = preUniqueId;
            this.UniqueId = UniqueIdManager.Create();

            OptionCommand = operateRecordManager.AddOptionCommand(this.PageIndex, this.CommandIndex, this.Rule.Text, this.Comment.Text, prevCommand, preUniqueId, UniqueId);
        }

        #region 事件
        private void Next_Click(object sender, EventArgs e)
        {
            if (!string.IsNullOrEmpty(this.Comment.Text))
            {
                operateRecordManager.UpdateOptionCommand(OptionCommand, this.PageIndex, this.CommandIndex, this.Rule.Text, this.Comment.Text);
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
            Option option = new Option(OptionDataGrid, OptionCommand);
            option.ShowDialog();
        }
        #endregion

        private void DeleteOption_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            int count = OptionDataGrid.SelectedRows.Count;
            int rowIndex = OptionDataGrid.SelectedRows[count - 1].Index;
            if (rowIndex >= 0)
            {
                OptionCommand.optionDialogInfo.OptionList.RemoveAt(rowIndex);
                OptionDataGrid.DataSource = new BindingList<OptionInfo>(OptionCommand.optionDialogInfo.OptionList);
            }
        }

        private void OptionDataGrid_CellDoubleClick(object sender, DataGridViewCellEventArgs e)
        {
            string commandName = OptionDataGrid.Rows[e.RowIndex].Cells[0].Value.ToString();
            DialogType dialogType = new DialogType(operateRecordManager, this.PageIndex + 1, e.RowIndex + 1, commandName, this.UniqueId);
            dialogType.ShowDialog();
        }
    }
}
