$(document).ready(function() {
    $("#search").click(
        function() 
        { 
	        search_sys_broadcast();
        });
        
    $("#add_broadcast").click(
        function() 
        { 
            var hostname = $.query.get('hostname');     
	        show_add_sys_broadcast(hostname);
        });
});

function player_broadcast(hostname)
{
    openPopup("公告管理", "sys_broadcast.html?hostname="+hostname, 800, 600, null, null);
}

function load_sys_broadcast()
{
    var type = $.query.get('type');
    var content = $.query.get('content');
    var datetime = $.query.get('datetime');
    var count = $.query.get('count');
    var priority = $.query.get('priority');
    
    $("#type").val(type);
    $("#content").val(content);
    $("#datetime").val(datetime);
    $("#count").val(count);
    $("#priority").val(priority);
}

function search_sys_broadcast()
{
    var type = $("#type").val();
    var hostname = $.query.get('hostname'); 
    $.ajax(
	{
        url: "search_sys_broadcast?type="+type+"&hostname="+hostname,
        cache: false,
        success: 
        function(msg)
        {
            switch(msg)
            {
                case "fail":
                    var tab = document.getElementById("table") ;
                    var rows = tab.rows.length ;
                    for(var i = 0; i < rows; i++)
                    {
                        $("#row").remove();
                    }; 
                break;
                default:
                    var tab = document.getElementById("table") ;
                    var rows = tab.rows.length ;
                    for(var i = 0; i < rows; i++)
                    {
                        $("#row").remove();
                    }; 
                    $("#table").append(msg);
                break;
            }
        }
    });
}

function show_add_sys_broadcast(hostname)
{
    openPopup("增加系统公告", "player_add_sys_broadcast.html?hostname="+hostname, 600, 400, null, null);
}

function show_modify_sys_broadcast(instid, type, content, datetime, count, priority)
{
    var hostname = $.query.get('hostname');
    openPopup("修改系统公告", "player_modify_sys_broadcast.html?hostname="+hostname+"&instid="+instid+"&type="+type+"&content="+content+"&datetime="+datetime+"&count="+count+"&priority="+priority, 600, 400, null, null);
}

function delete_sys_broadcast(instid, type)
{
    var hostname = $.query.get('hostname');
    $.ajax(
	{
        url: "delete_sys_broadcast?hostname="+hostname+"&instid="+instid+"&type="+type,
        cache: false,
        success: 
        function(msg)
        {
            switch(msg)
            {
                case "ok":
                    alert("删除系统公告成功！");
                    search_sys_broadcast();
                break;
                default:
                    alert("删除系统公告失败！");
                break;
            }
        }
    });
}

function modify_sys_broadcast()
{
    var hostname = $.query.get('hostname'); 
    var instid = $.query.get('instid');
    var type = $("#type").val();
    var content = $("#content").val();
    var dt = $("#datetime").val();
    var count = $("#count").val();
    var priority = $("#priority").val();
    var nDateTime = new Date(dt);
    var y = nDateTime.getFullYear();
    var m = nDateTime.getMonth() + 1;
    var d = nDateTime.getDate();
    var h = nDateTime.getHours();
    var mi = nDateTime.getMinutes();
    var s = nDateTime.getSeconds();
    var is_broadcast = $("#is_broadcast").attr("checked");
    $.ajax(
	{
        url: "player_modify_sys_broadcast?hostname="+hostname+"&instid="+instid+"&type="+type+"&content="+content+"&y="+y+"&m="+m+"&d="+d+"&h="+h+"&mi="+mi+"&s="+s+"&count="+count+"&priority="+priority+"&is_broadcast="+is_broadcast,
        cache: false,
        success: 
        function(msg)
        {
            switch(msg)
            {
                case "ok":
                    alert("修改系统公告成功！");
                    window.parent.cancelPopup();
                    window.parent.search_sys_broadcast();
                break;
                default:
                    alert("修改系统公告失败！");
                break;
            }
        }
    });
}


function add_sys_broadcast()
{
    var hostname = $.query.get('hostname'); 
    var type = $("#type").val();
    var content = $("#content").val();
    var dt = $("#datetime").val();
    var count = $("#count").val();
    var priority = $("#priority").val();
    var nDateTime = new Date(dt);
    var y = nDateTime.getFullYear();
    var m = nDateTime.getMonth() + 1;
    var d = nDateTime.getDate();
    var h = nDateTime.getHours();
    var mi = nDateTime.getMinutes();
    var s = nDateTime.getSeconds();
    var is_broadcast = $("#is_broadcast").attr("checked");
    $.ajax(
	{
        url: "player_add_sys_broadcast?hostname="+hostname+"&type="+type+"&content="+content+"&y="+y+"&m="+m+"&d="+d+"&h="+h+"&mi="+mi+"&s="+s+"&count="+count+"&priority="+priority+"&is_broadcast="+is_broadcast,
        cache: false,
        success: 
        function(msg)
        {
            switch(msg)
            {
                case "ok":
                    alert("增加系统公告成功！");
                    window.parent.cancelPopup();
                    window.parent.search_sys_broadcast();
                break;
                default:
                    alert("增加系统公告失败！");
                break;
            }
        }
    });
}

function close_add_sys_broadcast()
{
    window.parent.cancelPopup();
}