using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Model;

namespace Logic.Command
{
    public class OptionCommand : CommandBase
    {
        public OptionCommand()
            : base()
        {
            base.Remark = " %% 打开对话框(带选项)";
        }

        public OptionDialogInfo optionDialogInfo
        {
            get;
            set;
        }

        #region ICommandType Members
        public override string GetBeforeCommand()
        {
            string rules = string.IsNullOrEmpty(optionDialogInfo.Rules) ? "true" : optionDialogInfo.Rules;
            return BeforeCommandTempate.Replace("{0}", PreUniqueId.ToString()).Replace("{1}", PageIndex.ToString()).Replace("{2}", CommandIndex.ToString()).Replace("{3}", rules);
        }

        public override string GetCommand()
        {
            string commandList = GetCommandList();
            string commandContent = "{open_dialog_by_option, " + optionDialogInfo.TemplateId + ", [" + commandList + "]}";
            return CommandTemplate.Replace("{0}", PreUniqueId.ToString()).Replace("{1}", PageIndex.ToString()).Replace("{2}", CommandIndex.ToString()).Replace("{3}", string.IsNullOrEmpty(PrevCommand) ? "" : "%% 执行:\"" + PrevCommand + "\"的操作").Replace("{4}", commandContent);
        }

        public override string GetAfterCommand()
        {
            return string.Format(AfterCommandTemplate, PageIndex, CommandIndex, "true");
        }
        #endregion

        private string GetCommandList()
        {
            StringBuilder stringBuilder = new StringBuilder();
            for (int i = 0; i < optionDialogInfo.OptionList.Count; i++)
            {
                stringBuilder.Append("{" + optionDialogInfo.OptionList[i].TemplateId + ", " + this.UniqueId + ", " + (PageIndex + 1).ToString() + ", " + (i + 1).ToString() + "}");
                if (i < optionDialogInfo.OptionList.Count - 1)
                    stringBuilder.AppendFormat(",");
            }

            return stringBuilder.ToString();
        }
    }
}
