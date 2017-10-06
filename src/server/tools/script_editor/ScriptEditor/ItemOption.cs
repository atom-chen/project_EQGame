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
    public partial class ItemOption : Form
    {
        private DataGridView OptionDataGrid;
        private ItemCommand ItemCommand;

        public ItemOption(DataGridView optionDataGrid, ItemCommand itemCommand)
        {
            InitializeComponent();
            this.OptionDataGrid = optionDataGrid;
            this.ItemCommand = itemCommand;
        }

        private void Confirm_Click(object sender, EventArgs e)
        {
            if (!string.IsNullOrEmpty(this.Title.Text))
            {
                OptionInfo optionInfo = new OptionInfo();
                optionInfo.TemplateId = Title.Text;
                ItemCommand.optionDialogInfo.OptionList.Add(optionInfo);

                OptionDataGrid.DataSource = new BindingList<OptionInfo>(ItemCommand.optionDialogInfo.OptionList);
                Title.Text = "";
                Title.Focus();
            }
            else
            {
                MessageBox.Show("请录入选项名称！");
                this.Title.Focus();
            }
        }

        private void Close_Click(object sender, EventArgs e)
        {
            this.Close();
        }
    }
}
