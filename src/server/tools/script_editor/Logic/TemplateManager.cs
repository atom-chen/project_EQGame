using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using Common;

namespace Logic
{
    public class TemplateManager
    {
        private static Dictionary<string, string> Templates = new Dictionary<string, string>();

        public static string Load(string key, string path)
        {
            if (Templates != null && Templates.ContainsKey(key))
            {
                return Templates[key];
            }
            else
            {
                string template = Load(path);
                Templates.Add(key, template);
                return template;
            }
        }

        private static string Load(string path)
        {
            StreamReader streamReader = new StreamReader(DelegateUtils.GetVirtualPath() + path, System.Text.Encoding.Default);
            string result = streamReader.ReadToEnd();
            streamReader.Close();

            return result;
        }
    }
}
