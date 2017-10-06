using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Logic
{
    public class UniqueIdManager
    {
        private static int UniqueId = 0;

        public static int Get()
        {
            return UniqueId;
        }

        public static int Create()
        {
            UniqueId += 1;
            return UniqueId;
        }

        public static int ReCreate()
        {
            UniqueId = 0;
            return UniqueId;
        }
    }
}
