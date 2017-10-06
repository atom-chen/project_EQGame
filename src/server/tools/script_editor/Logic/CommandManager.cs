using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Model;
using Logic.Command;

namespace Logic
{
    public class CommandManager
    {
        public string Render(OperateRecordManager operateRecordManager)
        {
            string templateContent = TemplateManager.Load("npc", @"Template\npc.tplt");
            
            if (!string.IsNullOrEmpty(templateContent))
            {
                templateContent = ReplaceModule(operateRecordManager, templateContent);
                templateContent = ReplaceContent(operateRecordManager, templateContent);

                return templateContent;
            }

            return null;
        }

        private string ReplaceModule(OperateRecordManager operateRecordManager, string templateContent)
        {
            return templateContent.Replace("{0}", operateRecordManager.NpcKey);
        }

        private string ReplaceContent(OperateRecordManager operateRecordManager, string templateContent)
        {
            StringBuilder stringBuilder = new StringBuilder();
            stringBuilder.Append(GetBeforeCommandContent(operateRecordManager) + "\r\n");
            stringBuilder.Append(GetCommandContent(operateRecordManager) + "\r\n");
            return templateContent.Replace("{1}", stringBuilder.ToString());
        }

        private string GetBeforeCommandContent(OperateRecordManager operateRecordManager)
        {
            StringBuilder stringBuilder = new StringBuilder();
            for (int i = 0; i < operateRecordManager.OperateList.Count; i++)
            {
                ICommandType commandType = operateRecordManager.OperateList[i];
                if (i < operateRecordManager.OperateList.Count - 1)
                    stringBuilder.Append(commandType.GetBeforeCommand() + ";\r\n");
                else
                    stringBuilder.Append(commandType.GetBeforeCommand() + ".\r\n");
            }

            return stringBuilder.ToString();
        }

        private string GetCommandContent(OperateRecordManager operateRecordManager)
        {
            StringBuilder stringBuilder = new StringBuilder();
            for (int i = 0; i < operateRecordManager.OperateList.Count; i++)
            {
                ICommandType commandType = operateRecordManager.OperateList[i];
                if (i < operateRecordManager.OperateList.Count - 1)
                    stringBuilder.Append(commandType.GetCommand() + ";\r\n");
                else
                    stringBuilder.Append(commandType.GetCommand() + ".\r\n");
            }

            return stringBuilder.ToString();
        }

        private string GetAfterCommandContent(OperateRecordManager operateRecordManager)
        {
            StringBuilder stringBuilder = new StringBuilder();
            for (int i = 0; i < operateRecordManager.OperateList.Count; i++)
            {
                ICommandType commandType = operateRecordManager.OperateList[i];
                if (i < operateRecordManager.OperateList.Count - 1)
                    stringBuilder.Append(commandType.GetAfterCommand() + ";\r\n");
                else
                    stringBuilder.Append(commandType.GetAfterCommand() + ".\r\n");
            }

            return stringBuilder.ToString();
        }
    }
}
