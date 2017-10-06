using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using Model;
using Logic;

namespace ScriptEditor
{
    public partial class TabContainer : UserControl
    {
        private FunctionManager functionMgr = new FunctionManager();
        public delegate void TabContainerEventHandle(FunctionInfo functionInfo);
        public event TabContainerEventHandle ClickButton;

        public TabContainer()
        {
            InitializeComponent();
            
        }

        private void InitContainer()
        {
            Dictionary<string, List<FunctionInfo>> typeList = functionMgr.GetFunctionType();
            foreach (string key in typeList.Keys)
            {
                TabPage tabPage = new TabPage(key);
                FunctionContainer functionContainer = new FunctionContainer(typeList[key]);
                functionContainer.Dock = DockStyle.Fill;
                functionContainer.ClickButton += new FunctionContainer.FunctionContainerEvent(functionContainer_ClickButton);
                tabPage.Controls.Add(functionContainer);
                FunctionTabContainer.TabPages.Add(tabPage);
                functionContainer.InitContainer();
            }
        }

        private void functionContainer_ClickButton(FunctionInfo functionInfo)
        {
            ClickButton(functionInfo);
        }

        private void TabContainer_Load(object sender, EventArgs e)
        {
            if (!DesignMode)
            {
                InitContainer();
            }
        }
    }
}
