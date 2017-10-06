using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Logic.Command
{
    public class CloseCommand : CommandBase
    {
        public CloseCommand()
            : base()
        {
            base.Remark = " %% 关闭对话框";
        }

        #region ICommandType Members

        public override string GetBeforeCommand()
        {
            return BeforeCommandTempate.Replace("{0}", PreUniqueId.ToString()).Replace("{1}", PageIndex.ToString()).Replace("{2}", CommandIndex.ToString()).Replace("{3}", "true");
        }

        public override string GetCommand()
        {
            string commandContent = "{close_dialog}";
            return CommandTemplate.Replace("{0}", PreUniqueId.ToString()).Replace("{1}", PageIndex.ToString()).Replace("{2}", CommandIndex.ToString()).Replace("{3}", string.IsNullOrEmpty(PrevCommand) ? "" : "%% 执行:\"" + PrevCommand + "\"的操作").Replace("{4}", commandContent);
        }

        public override string GetAfterCommand()
        {
            return string.Format(AfterCommandTemplate, PageIndex, CommandIndex, "true");
        }

        #endregion
    }
}
