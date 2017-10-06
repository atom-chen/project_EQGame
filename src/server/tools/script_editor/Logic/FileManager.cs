using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using Common;

namespace Logic
{
    public class FileManager
    {
        public void Save(string path, string content)
        { 
            FileStream fileStream = new FileStream(DelegateUtils.GetVirtualPath() + path, FileMode.Create);
            StreamWriter streamWriter = new StreamWriter(fileStream);
            streamWriter.Write(content);
            streamWriter.Close();
        }

        public void Save1(string path, string content)
        {
            FileStream fileStream = new FileStream(path, FileMode.Create);
            StreamWriter streamWriter = new StreamWriter(fileStream);
            streamWriter.Write(content);
            streamWriter.Close();
        }

        public string Load(string path)
        {
            StreamReader streamReader = new StreamReader(path);
            string result = streamReader.ReadToEnd();
            streamReader.Close();
            return result;
        }
    }
}
