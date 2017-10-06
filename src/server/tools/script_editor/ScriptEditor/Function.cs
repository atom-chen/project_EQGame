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
    public partial class Function : Form
    {
        private FunctionManager FunctionMgr = new FunctionManager();
        public  FunctionInfo FunctionInfo = new FunctionInfo();

        public Function()
        {
            InitializeComponent();
        }

        private void Function_Load(object sender, EventArgs e)
        {
            BindMapDropDownList();
        }

        private void BindMapDropDownList()
        {
            List<FunctionInfo> FunctionList = FunctionMgr.Get();
            if (FunctionList != null)
            {
                FunctionDropDownList.DataSource = FunctionList;
                FunctionDropDownList.DisplayMember = "remark";
                FunctionDropDownList.ValueMember = "id";
            }
            else
            {
                FunctionDropDownList.DataSource = null;
            }
        }

        private void Confirm_Click(object sender, EventArgs e)
        {
            int functionId = Utils.TryToInt(FunctionDropDownList.SelectedValue, 0);
            this.FunctionInfo = FunctionMgr.Get(functionId);
            this.Close();
        }
    }
}
