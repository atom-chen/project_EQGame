using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Logic.Command
{
    public abstract class CommandBase : ICommandType
    {
        protected string BeforeCommandTempate;
        protected string CommandTemplate;
        protected string AfterCommandTemplate;

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

        public string PrevCommand
        {
            get;
            set;
        }

        public string path
        {
            get;
            set;
        }

        public int UniqueId
        {
            get;
            set;
        }

        public int PreUniqueId
        {
            get;
            set;
        }

        public string Remark
        {
            get;
            set;
        }

        public CommandBase()
        {
            BeforeCommandTempate = TemplateManager.Load("BeforeCommandType", @"Template\npc_before_command.tplt");
            CommandTemplate = TemplateManager.Load("CommandType", @"Template\npc_command.tplt");
        }

        #region ICommandType Members

        public virtual string GetBeforeCommand()
        {
            return null;
        }

        public virtual string GetCommand()
        {
            return null;
        }

        public virtual string GetAfterCommand()
        {
            return null;
        }

        #endregion
    }
}
