using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using Model;
using Logic;

namespace ScriptEditor
{
    public class DialogFactory
    {
        public static Form Create(OperateRecordManager operateRecordManager, int typeId, int pageIndex, int commandIndex, string prevCommand, int preUniqueId)
        {
            switch (typeId)
            { 
                case 1:
                    return new MessageDialog(operateRecordManager, pageIndex, commandIndex, prevCommand, preUniqueId);
                case 2:
                    return new OptionDialog(operateRecordManager, pageIndex, commandIndex, prevCommand, preUniqueId);
                case 3:
                    return new ItemDialog(operateRecordManager, pageIndex, commandIndex, prevCommand, preUniqueId);
                case 4:
                    operateRecordManager.AddCloseCommand(pageIndex, commandIndex, prevCommand, preUniqueId);
                    return null;
                default:
                    return new MessageDialog(operateRecordManager, pageIndex, commandIndex, prevCommand, preUniqueId);
            }
        }
    }
}
