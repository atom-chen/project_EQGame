using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Model;

namespace Logic.Command
{
    public class MessageCommand : CommandBase
    {
        public MessageCommand()
            : base()
        {
            base.Remark = " %% 打开对话框(不带选项)";
        }

        private MessageDialogInfo _MessageDialogInfo;
        public MessageDialogInfo messageDialogInfo
        {
            get
            {
                if (_MessageDialogInfo == null)
                    _MessageDialogInfo = new MessageDialogInfo();
                return _MessageDialogInfo;
            }
            set
            {
                _MessageDialogInfo = value;
            }
        }

        #region ICommandType Members
        public override string GetBeforeCommand()
        {
            string rules = string.IsNullOrEmpty(messageDialogInfo.Rules) ? "true" : messageDialogInfo.Rules;
            return BeforeCommandTempate.Replace("{0}", PreUniqueId.ToString()).Replace("{1}", PageIndex.ToString()).Replace("{2}", CommandIndex.ToString()).Replace("{3}", rules);
        }

        public override string GetCommand()
        {
            string commandContent = string.Format("{{open_dialog_by_message, {0}, {1}, {2}, {3}}}", messageDialogInfo.Comment, UniqueId, PageIndex + 1, CommandIndex);
            return CommandTemplate.Replace("{0}", PreUniqueId.ToString()).Replace("{1}", PageIndex.ToString()).Replace("{2}", CommandIndex.ToString()).Replace("{3}", string.IsNullOrEmpty(PrevCommand) ? "" : "%% 执行:\"" + PrevCommand + "\"的操作").Replace("{4}", commandContent);
        }

        public override string GetAfterCommand()
        {
            return string.Format(AfterCommandTemplate, PageIndex, CommandIndex, "true");
        }
        #endregion
    }
}
