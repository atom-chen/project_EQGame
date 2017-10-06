using System;
using System.Collections;
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
using GetEmailAddress;
using System.Collections.Generic;
using System.Text;
using System.Net.Mail;

namespace web
{
    public partial class Default : System.Web.UI.Page
    {
        IList<Person> list;
        protected void Page_Load(object sender, EventArgs e)
        {
            if (!IsPostBack)
            {
                this.imgSendMail.Attributes["onclick"] = "return DelAllItem('single_Chk'); return SendMailMessage()";
                this.imgFindEmail.Attributes["onclick"] = "return MailSelect()";
            }
        }
        #region//获取MSN还有联系人
        protected void imgbtnFindByMsn_Click(object sender, ImageClickEventArgs e)
        {

            GetContactPerson contactPerson = new GetContactPerson();

            GetAddressStatus status = GetContactPerson.GetAddressByEmail(txtMsnId.Text.Trim(), txtMsnPd.Text.Trim(), out list);

            switch (status)
            {
                case GetAddressStatus.Success:
                    //  listResult.Items.Clear();
                    //oreach (Person tmp in list)
                    //{ 
                    //string text="Email:" + tmp.Email + " Name:" + tmp.Name;
                    // this.listResult.Items.Add(new ListItem(text, tmp.Email));    
                    //}
                    //listResult.Visible = true;
                    this.listResult.DataSource = list;
                    this.listResult.DataBind();
                    divShow.Visible = true;
                    lbtError.Text = "";
                    break;
                case GetAddressStatus.UidOrPwdError:
                    lbtError.Text = "MSN用户名或密码错误！";
                    break;
                case GetAddressStatus.NetError:
                    lbtError.Text = "网络错误！";
                    break;
                case GetAddressStatus.NoAddress:
                    lbtError.Text = "MSN地址不存在！";
                    break;
                case GetAddressStatus.MailError:
                    lbtError.Text = "邮箱地址格式不正确！";
                    break;
                default:
                    break;
            }
        }
        #endregion

        #region//发送邮件
        protected void imgSendMail_Click(object sender, ImageClickEventArgs e)
        {
            for (int i = 0; i < listResult.Items.Count; i++)
            {
                string mailid = ((HiddenField)listResult.Items[i].FindControl("sendid")).Value;
                senEmail("smtp.sina.com", 25, "uniex@sina.cn", "85016653", mailid, "您的好友，邀请你加入网站。", this.txtConent.Value, "");
                ClientScript.RegisterStartupScript(this.GetType(), "", "<script>alert('邀请好友Email已发送,敬请期待好友的来访.');window.location.href='Default1.aspx';</script>");
            }
        }
        #endregion

        #region //发送邮件类库
        /// <param name="SMPTURL">发送人的SMP服务器地址</param>		
        /// <param name="SMPTPort">发送人的SMP服务器端口</param>		
        /// <param name="myEmail">发送email的地址</param>		
        /// <param name="myPas">发送email的密码</param>		
        /// <param name="reciever">邮件人email地址</param>		
        /// <param name="mailtitle">邮件标题</param>		
        /// <param name="mailtext">邮件内容</param>		
        /// <param name="urlfile">邮件附件</param>		
        /// <returns></returns>		
        public static bool senEmail(string SMPTURL, int SMPTPort, string myEmail, string myPas, string reciever, string mailtitle, string mailtext, string urlfile)
        {
            StringBuilder strSql = new StringBuilder();
            MailMessage message = new MailMessage();
            message.From = new MailAddress(myEmail, myEmail.Split('@')[0]);//用来发送邮件的外部邮箱的用户名，昵称		
            message.To.Add(new MailAddress(reciever)); // the email you want to send email to 		
            message.Subject = mailtitle;
            message.IsBodyHtml = true;
            message.BodyEncoding = System.Text.Encoding.UTF8;
            message.Body = mailtext;
            if (urlfile.Length > 0)
            {
                Attachment item = new Attachment(urlfile);
                message.Attachments.Add(item);
            }
            message.Priority = MailPriority.High;
            SmtpClient client = new SmtpClient(SMPTURL, SMPTPort); // smtp服务器及端口		
            client.Credentials = new System.Net.NetworkCredential(myEmail, myPas); ////用来发送邮件的外部邮箱的用户名，密码		
            //client.EnableSsl = true; //经过ssl加密 		
            object userState = message;
            try
            {
                client.Send(message);
                return true;
            }
            catch
            {
                return false;
            }
        }
        #endregion

        #region//查找各个邮箱的联系人
        protected void imgFindEmail_Click(object sender, ImageClickEventArgs e)
        {
            StringBuilder build = new StringBuilder();

            GetAddressStatus status = GetContactPerson.GetAddressByEmail(txtName.Text.Trim() + selMailType.Value.Trim(), txtPassword.Text.Trim(), null, out list);

            switch (status)
            {
                case GetAddressStatus.Success:
                    this.listResult.DataSource = list;
                    this.listResult.DataBind();
                    divShow.Visible = true;
                    lbtError.Text = "";
                    break;
                case GetAddressStatus.UidOrPwdError:
                    lbtError.Text = "邮箱用户名或密码错误！";
                    break;
                case GetAddressStatus.NetError:
                    lbtError.Text = "网络错误！";
                    break;
                case GetAddressStatus.NoAddress:
                    lbtError.Text = "邮箱地址不存在！";
                    break;
                case GetAddressStatus.MailError:
                    lbtError.Text = "邮箱地址格式不正确！";
                    break;
                default:

                    break;
            }
        }
        #endregion

    }
}
