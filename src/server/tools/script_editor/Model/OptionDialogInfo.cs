using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Model
{
    public class OptionDialogInfo
    {
        public string TemplateId
        {
            get;
            set;
        }

        public string Rules
        {
            get;
            set;
        }

        /// <summary>
        /// 选项列表
        /// </summary>
        public List<OptionInfo> _OptionList;
        public List<OptionInfo> OptionList
        {
            get
            {
                if (_OptionList == null)
                    _OptionList = new List<OptionInfo>();

                return _OptionList;
            }
            set
            {
                _OptionList = value;
            }
        }
    }
}
