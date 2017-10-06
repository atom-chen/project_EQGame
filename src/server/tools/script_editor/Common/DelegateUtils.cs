using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Common
{
    public delegate string VirtualPathHandle();

    public class DelegateUtils
    {
        public static event VirtualPathHandle VirtualPath;

        public static string GetVirtualPath()
        {
            return VirtualPath() + @"\";
        }
    }
}
