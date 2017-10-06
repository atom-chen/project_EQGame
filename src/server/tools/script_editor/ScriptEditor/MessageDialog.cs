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
    public partial class MessageDialog : Form
    {
        private CommandManager commandManager = new CommandManager();

        #region 实体
        public OperateRecordManager operateRecordManager
        {
            get;
            set;
        }

        public int PageIndex
        {
            get;
            set;
        }

        public int CommandIndex
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

        private MessageCommand MessageCommand;
        #endregion

        #region 构造函数
        public MessageDialog(OperateRecordManager operateRecordManager, int pageIndex, int commandIndex, string prevCommand, int preUniqueId)
        {
            InitializeComponent();
            this.operateRecordManager = operateRecordManager;
            this.PageIndex = pageIndex;
            this.CommandIndex = commandIndex;
            this.prevCommand = prevCommand;
            this.PreUniqueId = preUniqueId;
            this.UniqueId = UniqueIdManager.Create();

            MessageCommand = this.operateRecordManager.AddMessageCommand(this.PageIndex, this.CommandIndex, this.Rule.Text, this.Comment.Text, prevCommand, preUniqueId, UniqueId);
        }
        #endregion

        #region 事件
        private void Next_Click(object sender, EventArgs e)
        {
            if (!string.IsNullOrEmpty(this.Comment.Text))
            {
                this.operateRecordManager.UpdateMessageCommand(MessageCommand, this.PageIndex, this.CommandIndex, this.Rule.Text, this.Comment.Text);

                this.Hide();
                DialogType dialogType = new DialogType(operateRecordManager, PageIndex + 1, CommandIndex, "",  this.UniqueId);
                dialogType.ShowDialog();
            }
            else
            {
                MessageBox.Show("请录入内容！");
                this.Comment.Focus();
            }
        }
        #endregion
    }
}
