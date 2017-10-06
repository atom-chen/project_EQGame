<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="Default.aspx.cs" Inherits="web.Default" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head id="Head1" runat="server">
    <title></title>

    <script type="text/javascript">
     function SendMailMessage() {
            if (document.getElementById('txtConent').value == "") {

                alert('请输入要发送邮件的正文。'); document.getElementById('txtConent').focus(); return false;
            }
            
        }

        function MailSelect() 
        {
            if (document.getElementById("selMailType").options[document.getElementById("selMailType").selectedIndex].text == '请选择')
            { alert('请选择要查询的邮箱。'); return false }
            if (document.getElementById('txtPassword').value == "") {

                alert('请输入邮箱登陆密码。'); document.getElementById('txtPassword').focus(); return false;
            }
        }
    </script>

    <link href="css/browse1.css" rel="Stylesheet" type="text/css" />
</head>
<body>
    <form id="form1" runat="server">
    <div class="realelecleft" align="center">
        <div class="jietop">
            <span class="sttop"></span>
            <div class="kok">
                <div class="boio0">
                    <div class="titop">
                        <ul>
                            <li class="nav_current">
                                <div class="divpaleft">
                                    <div class="divheight">
                                    </div>
                                    <div class="nav_link">
                                        获取联系人发送邮件</div>
                                </div>
                            </li>
                        </ul>
                    </div>
                    <div class="nrong">
                        <div class="divshow1" style="color: Red; font-weight: bold">
                            完整版本，整合了包括读取邮箱通讯录、MSN好友列表的的功能，目前读取邮箱通讯录支持如下邮箱：gmail(Y)、hotmail(Y)、live(Y)、tom(Y)、yahoo(Y)(有点慢)、sina(Y)、163(Y)、126(Y)、yeah(Y)、sohu(Y)<br />
                            读取后可以发送邮件(完善版本)</div>
                        <div class="divshow1">
                            <ul>
                                <li class="li1">邮箱用户名：</li>
                                <li class="li2">
                                    <asp:TextBox ID="txtName" runat="server"></asp:TextBox>
                                    <select name="postoffice" id="selMailType" runat="server">
                                        <option value="163.com">请选择</option>
                                        <option value="@163.com">@163.com</option>
                                        <option value="@126.com">@126.com</option>
                                        <option value="@yeah.net">@yeah.net</option>
                                        <option value="@sina.com">@sina.com</option>
                                        <option value="@hotmail.com">@hotmail.com</option>
                                        <option value="@gmail.com">@gmail.com</option>
                                        <option value="@live.cn">@live.cn</option>
                                        <option value="@yahoo.cn">@yahoo.cn</option>
                                        <option value="@yahoo.com">@yahoo.com.cn</option>
                                        <option value="@sohu.com">@sohu.com</option>
                                        <option value="@tom.com">@tom.com</option>
                                    </select></li>
                            </ul>
                            <ul>
                                <li class="li1">邮箱密码：</li>
                                <li class="li2">
                                    <asp:TextBox ID="txtPassword" runat="server" TextMode="Password"></asp:TextBox></li>
                            </ul>
                            <ul>
                                <li class="li1"></li>
                                <li class="li2" style="float: right; padding-top: 8px; padding-bottom: 8px;">
                                    <asp:ImageButton ID="imgFindEmail" runat="server" CssClass="find" ImageUrl="img/chz.jpg"
                                        OnClick="imgFindEmail_Click" /></li>
                            </ul>
                        </div>
                        <div class="divshow1">
                            <ul>
                                <li class="li1">MSN帐号：</li>
                                <li class="li2">
                                    <asp:TextBox ID="txtMsnId" runat="server" CssClass="findbox"></asp:TextBox>
                                    &nbsp;<asp:RequiredFieldValidator ID="RequiredFieldValidator1" runat="server" ControlToValidate="txtMsnId"
                                        ErrorMessage="帐号不能为空~" ForeColor="#009933" ValidationGroup="msn"></asp:RequiredFieldValidator>
                                </li>
                            </ul>
                            <ul>
                                <li class="li1">MSN密码：</li>
                                <li class="li2">
                                    <asp:TextBox ID="txtMsnPd" runat="server" CssClass="findbox" TextMode="Password"></asp:TextBox>
                                    &nbsp;<asp:RequiredFieldValidator ID="RequiredFieldValidator2" runat="server" ControlToValidate="txtMsnPd"
                                        ErrorMessage="密码不能为空~" ForeColor="#009933" ValidationGroup="msn"></asp:RequiredFieldValidator>
                                </li>
                            </ul>
                            <ul>
                                <li class="li1"></li>
                                <li class="li2" style="float: right; padding-top: 8px; padding-bottom: 8px;">
                                    <asp:ImageButton ID="imgbtnFindByMsn" runat="server" CssClass="find" ImageUrl="img/chz.jpg"
                                        ValidationGroup="msn" OnClick="imgbtnFindByMsn_Click" /></li>
                            </ul>
                        </div>
                        <div class="divshow1">
                            <ul>
                                <li class="li1">选择：</li>
                                <li class="li2">
                                    <div class="divShowMail" runat="server" visible="false" id="divShow">
                                        <asp:Repeater ID="listResult" runat="server">
                                            <ItemTemplate>
                                                <ul>
                                                    <li>
                                                        <asp:CheckBox ID="single_Chk" name="selectAll" runat="server" />
                                                        <asp:HiddenField ID="sendid" Value='<%#Eval("Email") %>' runat="server" />
                                                        &nbsp; Email:<%#Eval("Email")%>&nbsp; Name:<%#Eval("Name")%>
                                                    </li>
                                                </ul>
                                            </ItemTemplate>
                                        </asp:Repeater>
                                    </div>
                                    <asp:Label ID="lbtError" runat="server" Text="暂无联系人,请选择邮箱进行导入."></asp:Label>
                                </li>
                            </ul>
                            <ul>
                                <li class="li1">内容：</li>
                                <li class="li2" style="float: right; padding-top: 8px; padding-bottom: 8px;">
                                    <input value="我已经在艺在线注册了。赶紧来和我交朋友。参加比赛，展现你的才艺。艺在线：www.yizaixian.com" onfocus="if(value=='我已经在艺在线注册了。赶紧来和我交朋友。参加比赛，展现你的才艺。艺在线：www.yizaixian.com'){value=''}"
                                        onblur="if (value ==''){value='我已经在艺在线注册了。赶紧来和我交朋友。参加比赛，展现你的才艺。艺在线：www.yizaixian.com'}"
                                        class="sanbaocontent" runat="server" id="txtConent" />
                                </li>
                            </ul>
                            <ul>
                                <li class="li1"></li>
                                <li class="li2" style="float: right; padding-top: 8px; padding-bottom: 8px;">
                                    <asp:ImageButton ImageUrl="img/D_fasong.jpg" ID="imgSendMail" runat="server" OnClick="imgSendMail_Click" /></li>
                            </ul>
                        </div>
                    </div>
                    <span class="hbottom"></span>
                </div>
            </div>
            <span class="dttop"></span>
        </div>
    </div>
    </form>
</body>
</html>
