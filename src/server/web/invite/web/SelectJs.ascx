<%@ Control Language="C#" AutoEventWireup="true" CodeFile="SelectJs.ascx.cs" Inherits="SelectJs" %>
<span>选择：</span> <span id="select1" style="cursor: pointer" onclick="SettingChecked('selectAll');BackColor();this.style['color']='#33AA3F'">
    全部</span> - <span id="select2" style="cursor: pointer" onclick="SettingChecked();BackColor();this.style['color']='#33AA3F'">
        &nbsp;反选 &nbsp;</span>- <span id="select3" style="cursor: pointer" onclick="NoCheckSelect();BackColor();this.style['color']='#33AA3F'">
            &nbsp;不选</span>

<script language="javascript" type="text/javascript">

     //设置 CheckBox 的选择状态js反选 全选 多选框 兼容ie和firefox版本
     function SettingChecked(isCked) {
         var ck = document.getElementsByTagName("input"); //意思是找到页面上所有 <input > 节点
         for (var i = 0; i < ck.length; i++) {
             if (ck[i].type == "checkbox") {
                 ck[i].checked = (isCked == "selectAll") ? true : (!ck[i].checked);
             }
         }
     }
     //判断选中提示。
     function DelAllItem(id, evt) {
         if (CkeckedTolNum(id) == 0) {
             alert('请选择要发送的好友邮箱。');
             return false;
         }
         else
         { return window.confirm('确认要给好友发送Email吗?'); }
     }
     function CkeckedTolNum(cid) {
         var num = 0;
         var cks = document.getElementsByTagName("input");
         for (var i = 0; i < cks.length; i++) {
             //cks[i].id.indexOf(cid) = 0 时，说明 是以 cid 开头的 ID
             if (cks[i].type == "checkbox" && cks[i].checked && cks[i].id.indexOf(cid) >= 0)
             { num++; }
         }
         return num;
     }
     function NoCheckSelect()//不选
     {
         var cks = document.getElementsByTagName("input");
         for (var i = 0; i < cks.length; i++) {
             if (cks[i].type.toLowerCase() == "checkbox" && cks[i].checked) {
                 //如果选择框选中则清空
                 if (cks[i].checked.toString() == "true") { cks[i].checked = false; }
             }
         }
     }
     function BackColor()//点击字体颜色变化
     {
         var q = 5; for (i = 1; i < 5; i++)
         { if (document.getElementById("select" + i) != null) { document.getElementById("select" + i).style.color = ''; } }
     }
</script>

