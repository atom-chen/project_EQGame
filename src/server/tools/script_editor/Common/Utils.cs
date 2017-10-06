using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Common
{
    public class Utils
    {
        /// <summary>
        /// 判断对象是否为Int32类型的数字
        /// </summary>
        /// <param name="Expression"></param>
        /// <returns></returns>
        public static bool IsNumeric(object Expression)
        {
            return TypeParse.IsNumeric(Expression);
        }

        public static int TryToInt(object Expression, int errorNumber)
        {
            if (IsNumeric(Expression))
                return Convert.ToInt32(Expression);
            else
                return errorNumber;
        }

        public static bool ValidateIsSelected(object value)
        {
            if (value != null && !string.IsNullOrEmpty(value.ToString()))
                return true;
            else
                return false;
        }
    }
}
