using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Model;
using Logic.Command;

namespace Logic
{
    public class OperateRecordManager
    {
        private NpcManager npcManager = new NpcManager();

        /// <summary>
        /// 场景ID
        /// </summary>
        public int SceneId
        {
            get;
            set;
        }

        /// <summary>
        /// npc id
        /// </summary>
        public int NpcId
        {
            get;
            set;
        }

        public string NpcKey
        {
            get;
            set;
        }

        public List<NpcMapMapping> NpcMapMappings
        {
            get;
            set;
        }

        /// <summary>
        /// 消息列表
        /// </summary>
        private List<ICommandType> _OperateList;
        public List<ICommandType> OperateList
        {
            get
            {
                if (_OperateList == null)
                    _OperateList = new List<ICommandType>();

                return _OperateList;
            }
            set
            {
                _OperateList = value;
            }
        }

        public void SetOperateRecordInfo(int sceneId, int npcId)
        {
            this.SceneId = sceneId;
            this.NpcId = npcId;
            this.NpcKey = "npc_" + sceneId.ToString() + "_" + npcManager.Get(this.NpcMapMappings, sceneId);
            //this.NpcKey = "npc" + sceneId +  npcManager.Get(this.NpcMapMappings, sceneId);
        }

        public MessageCommand AddMessageCommand(int pageIndex, int commandIndex, string rules, string comment, string prevCommand, int preUniqueId, int uniqueId)
        {
            MessageCommand messageCommand = new MessageCommand()
                    {
                        PageIndex = pageIndex,
                        CommandIndex = commandIndex,
                        PrevCommand = prevCommand,
                        PreUniqueId = preUniqueId,
                        UniqueId = uniqueId,
                        messageDialogInfo = new MessageDialogInfo()
                        {
                            Rules = rules,
                            Comment = comment
                        }
                    };

            OperateList.Add(messageCommand);

            return messageCommand;
        }

        public void UpdateMessageCommand(MessageCommand messageCommand, int pageIndex, int commandIndex, string rules, string comment)
        {
            messageCommand.PageIndex = pageIndex;
            messageCommand.CommandIndex = commandIndex;
            messageCommand.messageDialogInfo.Rules = rules;
            messageCommand.messageDialogInfo.Comment = comment;
        }

        public OptionCommand AddOptionCommand(int pageIndex, int commandIndex, string rules, string comment, string prevCommand, int preUniqueId, int uniqueId)
        {
            OptionCommand optionCommand = new OptionCommand()
            {
                PageIndex = pageIndex,
                CommandIndex = commandIndex,
                PrevCommand = prevCommand,
                PreUniqueId = preUniqueId,
                UniqueId = uniqueId,
                optionDialogInfo = new OptionDialogInfo()
                {
                    Rules = rules,
                    TemplateId = comment
                }
            };

            OperateList.Add(optionCommand);

            return optionCommand;
        }

        public ItemCommand AddItemCommand(int pageIndex, int commandIndex, string rules, string comment, string prevCommand, int preUniqueId, int uniqueId)
        {
            ItemCommand optionCommand = new ItemCommand()
            {
                PageIndex = pageIndex,
                CommandIndex = commandIndex,
                PrevCommand = prevCommand,
                PreUniqueId = preUniqueId,
                UniqueId = uniqueId,
                optionDialogInfo = new OptionDialogInfo()
                {
                    Rules = rules,
                    TemplateId = comment
                }
            };

            OperateList.Add(optionCommand);

            return optionCommand;
        }

        public void UpdateOptionCommand(OptionCommand optionCommand, int pageIndex, int commandIndex, string rules, string comment)
        {
            optionCommand.PageIndex = pageIndex;
            optionCommand.CommandIndex = commandIndex;
            optionCommand.optionDialogInfo.Rules = rules;
            optionCommand.optionDialogInfo.TemplateId = comment;
        }

        public void UpdateItemCommand(ItemCommand itemCommand, int pageIndex, int commandIndex, string rules, string comment)
        {
            itemCommand.PageIndex = pageIndex;
            itemCommand.CommandIndex = commandIndex;
            itemCommand.optionDialogInfo.Rules = rules;
            itemCommand.optionDialogInfo.TemplateId = comment;
        }

        public void AddCloseCommand(int pageIndex, int commandIndex, string prevCommand, int preUniqueId)
        {
            OperateList.Add(
                new CloseCommand()
                {
                    PageIndex = pageIndex,
                    CommandIndex = commandIndex,
                    PrevCommand = prevCommand,
                    PreUniqueId = preUniqueId
                }
                );
        }
    }
}
