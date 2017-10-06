using System;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Web;
using System.Web.Security;
using System.Web.UI;
using System.Web.UI.HtmlControls;
using System.Web.UI.WebControls;
using System.Web.UI.WebControls.WebParts;
using System.Xml.Linq;
using System.IO;

public partial class _Default : System.Web.UI.Page 
{
    private string[] splitstr = new string[] { "__" };

    protected void Page_Load(object sender, EventArgs e)
    {
        if (Request.Files.Count > 0)
        {
            try
            {
                HttpPostedFile file = Request.Files[0];
                string account = GetAccount(file.FileName);
                string time = GetTime(file.FileName);
                string fileName = GetFileName(file.FileName);
                string filePath = CreateDirectory(account, time, fileName);
                file.SaveAs(filePath);
                Response.Write("Success\r\n");
            }
            catch
            {
                Response.Write("Error\r\n");
            }
        }
    }

    private string GetAccount(string data)
    {
        return data.Split(splitstr,  StringSplitOptions.RemoveEmptyEntries)[0];
    }

    private string GetTime(string data)
    {
        string time = data.Split(splitstr, StringSplitOptions.RemoveEmptyEntries)[1];
        DateTime datetime = Convert.ToDateTime(time);
        return datetime.Year.ToString() + datetime.Month.ToString() + datetime.Day.ToString() + datetime.Hour.ToString() + datetime.Minute.ToString() + datetime.Second.ToString();
    }

    private string GetFileName(string data)
    {
        return data.Split(splitstr, StringSplitOptions.RemoveEmptyEntries)[2];
    }

    private string CreateDirectory(string account, string time, string fileName)
    {
        string filePath = this.MapPath("UploadDocument") + "\\" + account + "\\" + time + "\\";
        if (Directory.Exists(filePath) == false)
        {
            Directory.CreateDirectory(filePath);
        }

        return filePath + fileName;
    }
}
