using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;
using Common;

namespace ScriptEditor
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            DelegateUtils.VirtualPath += new VirtualPathHandle(DelegateUtils_VirtualPath);
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new Editor());
        }

        private static string DelegateUtils_VirtualPath()
        {
            return Application.StartupPath;
        }
    }
}
