using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;
using System.IO;

namespace Common
{
    public class XmlManager
    {
        public T Deserialize<T>(string path) where T : class
        {
            try
            {
                XmlSerializer xmlSerializer = new XmlSerializer(typeof(T));
                TextReader textReader = new StreamReader(path);
                T result = (T)xmlSerializer.Deserialize(textReader);
                textReader.Close();
                
                return result;
            }
            catch(Exception ex)
            {
                throw new Exception(ex.Message);
            }
        }
    }
}
