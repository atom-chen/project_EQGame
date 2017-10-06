using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using Model;
using Controls;

namespace ScriptEditor
{
    public partial class FunctionContainer : UserControl
    {
        private int x = 10;
        private int y = 10;
        private List<FunctionInfo> FunctionList;
        public delegate void FunctionContainerEvent(FunctionInfo functionInfo);
        public event FunctionContainerEvent ClickButton;

        public FunctionContainer(List<FunctionInfo> functionList)
        {
            InitializeComponent();

            this.FunctionList = functionList;
        }

        public void InitContainer()
        {
            foreach (FunctionInfo functionInfo in FunctionList)
            {
                CustomerButton customerBtn = new CustomerButton();
                customerBtn.Width = 90;
                customerBtn.Tag = functionInfo.Remark;
                customerBtn.Text = functionInfo.Remark;
                
                customerBtn.functionInfo = functionInfo;
                customerBtn.Click += new EventHandler(customerBtn_Click);
                this.Controls.Add(customerBtn);
                if (x + customerBtn.Width + 10 > this.Parent.Width)
                {
                    y += customerBtn.Height + 10;
                    x = 10;
                    customerBtn.Location = new Point(x, y);
                }
                else
                {
                    if (y == 10)
                    {
                        customerBtn.Location = new Point(x, y);
                        x += customerBtn.Width + 10;
                    }
                    else
                    {
                        x += customerBtn.Width + 10;
                        customerBtn.Location = new Point(x, y);
                    }
                }
            }
        }

        private void customerBtn_Click(object sender, EventArgs e)
        {
            CustomerButton customerBtn = sender as CustomerButton;
            if (customerBtn != null)
            {
                ClickButton(customerBtn.functionInfo);
            }
        }
    }
}
