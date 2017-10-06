using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Common;
using Model;

namespace Logic
{
    public class FunctionManager
    {
        private XmlManager xmlManager = new XmlManager();
        private List<FunctionInfo> FunctionList;
        private List<string> keyWordList = new List<string>();

        public List<FunctionInfo> Get()
        {
            if (FunctionList == null)
            {
                FunctionListInfo functionListInfo = xmlManager.Deserialize<FunctionListInfo>(DelegateUtils.GetVirtualPath() + @"Template\Function_list.xml");
                FunctionList = functionListInfo.FunctionList.ToList<FunctionInfo>();
            }

            return FunctionList;
        }

        public FunctionInfo Get(int id)
        {
            foreach (FunctionInfo functionInfo in FunctionList)
            {
                if (functionInfo.Id == id)
                    return functionInfo;
            }

            return null;
        }

        public List<string> GetKeyWord()
        {
            if (keyWordList != null && keyWordList.Count <= 0)
            {
                List<FunctionInfo> FunctionList = Get();
                foreach (FunctionInfo functionInfo in FunctionList)
                {
                    string name = functionInfo.Name;
                    int startIndex = name.IndexOf("(") + 1;
                    int endIndex = name.IndexOf(")");
                    string[] args = name.Substring(startIndex, endIndex - startIndex).Split(new char[] { ',' });
                    foreach (string arg in args)
                    {
                        if (!keyWordList.Contains(arg) && !string.IsNullOrEmpty(arg))
                            keyWordList.Add(arg.Trim());
                    }
                }
            }

            return keyWordList;
        }

        public Dictionary<string, List<FunctionInfo>> GetFunctionType()
        {
            Dictionary<string, List<FunctionInfo>> typeDict = new Dictionary<string,List<FunctionInfo>>();
            List<FunctionInfo> FunctionList = Get();
            foreach (FunctionInfo functionInfo in FunctionList)
            {
                if (!typeDict.ContainsKey(functionInfo.Type))
                {
                    List<FunctionInfo> newFunctionInfoList = new List<FunctionInfo>();
                    newFunctionInfoList.Add(functionInfo);
                    typeDict.Add(functionInfo.Type, newFunctionInfoList);
                }
                else
                    typeDict[functionInfo.Type].Add(functionInfo);
            }

            return typeDict;
        }
    }
}
