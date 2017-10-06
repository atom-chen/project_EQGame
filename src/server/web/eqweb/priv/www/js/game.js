﻿$(document).ready(function () {
    game_svr_list();
});


function scene_monitor(hostname)
{
    openPopup("场景管理", "scene_monitor.html?hostname="+hostname, 800, 600, null, null);
}

function player_manager(hostname)
{
    openPopup("玩家管理", "player.html?hostname="+hostname, 800, 600, null, null);
}

function game_info_gather(hostname)
{
    openPopup("游戏信息收集", "game_info_gather.html?hostname="+hostname, 800, 600, null, null);
}

function float_bottle(hostname)
{
    openPopup("飘流瓶", "float_pool.html?hostname="+hostname, 600, 600, null, null);
}

function close_add_game_svr()
{
    window.parent.cancelPopup();
    window.parent.game_svr_list();
}

function add_game_svr()
{
    var ip = $("#ip").val();
    var hostname = $("#hostname").val();
    var servername = $("#servername").val();
    $.ajax(
	{
        url: "game_svr_add?ip="+ip+"&hostname="+hostname+"&servername="+servername,
        cache: false,
        success: 
        function(msg)
        {
            switch(msg)
            {
                case "ok":
                    alert("更新成功！");
                    close_add_game_svr();
                break;
                default:
                    alert(msg);
                break;
            }
        }
    });
}

function delete_game_svr(hostname)
{
    var ret = confirm('确定将此记录删除?');
    if(ret == true)
    {
        $.ajax(
	    {
            url: "game_svr_delete?hostname="+hostname,
            cache: false,
            success: 
            function(msg)
            {
                switch(msg)
                {
                    case "ok":
                        alert("删除成功！");
                        game_svr_list();
                    break;
                    default:
                        alert(msg);
                    break;
                }
            }
        });
    }
}

function get_status(hostname, index)
{
    $.ajax(
	{
        url: "game_svr_status/"+hostname,
        cache: false,
        success: 
        function(msg)
        {
            switch(msg)
            {
                case "Started":
                    $("#status_"+index).attr("src","images/started.png"); 
                break;
                case "Closed":
                    $("#status_"+index).attr("src","images/closed.png");  
                break;
                default:
                    $("#status_"+index).attr("src","images/closed.png");  
                break;
            }
        }
    });
}

function get_time(hostname, index)
{
    $.ajax(
	{
        url: "farm_svr_time/"+hostname,
        cache: false,
        success: 
        function(msg)
        {
            switch(msg)
            {
                case "Error":
                    $("#time_"+index).text("0-0-0 0:0:0");
                default:
                    $("#time_"+index).text(msg);
            }
        }
    });
}

function monitor_status()
{
    var total_server = $("#total_server").val();
    if(total_server != undefined)
    {
        for(var i = 1; i <= total_server; i++)
        {
            $("#status_"+i).attr("src","images/loading.gif"); 
            hostname = $("#hostname_"+i).html();
            get_status(hostname, i);
            get_time(hostname, i);
        };
    }
}

function game_svr_list()
{
    $.ajax(
	{
        url: "game_svr_list",
        cache: false,
        success: 
        function(msg)
        {
            switch(msg)
            {
                case "fail":
                    alert("服务器异常!");
                break;
                default:
                    var total_server = $("#total_server").val();
                    for(var i = 1; i <= total_server; i++)
                    {
                        $("#game_svr_row").remove();
                    }
                    $("#game_svr_table").append(msg);
                    monitor_status();
                break;
            }
        }
    });
}

function start(hostname, index)
{
    $.ajax(
	{
        url: "start_game_svr/"+hostname,
        cache: false,
        success: 
        function(msg)
        {
            switch(msg)
            {
                case "ok":
                    $("#status_"+index).attr("src","images/started.png"); 
                break;
                case "fail":
                    $("#status_"+index).attr("src","images/fail.png"); 
                break;
                default:
                    alert(msg);
                break;
            }
        }
    });
}

function close(hostname, index)
{
    $.ajax(
	{
        url: "close_game_svr/"+hostname,
        cache: false,
        success: 
        function(msg)
        {
            switch(msg)
            {
                case "ok":
                    $("#status_"+index).attr("src","images/closed.png");  
                break;
                default:
                    alert(msg);
                break;
            }
        }
    });
}

function restart(hostname, index)
{
    $.ajax(
	{
        url: "restart_game_svr/"+hostname,
        cache: false,
        success: 
        function(msg)
        {
            switch(msg)
            {
                case "ok":
                    $("#status_"+index).attr("src","images/started.png"); 
                break;
                case "fail":
                    $("#status_"+index).attr("src","images/fail.png");
                break;
                default:
                    alert(msg);
                break;
            }
        }
    });
}

function template_update(hostname)
{
    $.ajax(
	{
        url: "template_update/"+hostname,
        cache: false,
        success: 
        function(msg)
        {
            switch(msg)
            {
                case "ok":
                    alert("更新成功！");
                break;
                default:
                    alert(msg);
                break;
            }
        }
    });
}

function select()
{
    selectType = $("input[name='server']:checked").val();
    switch(selectType)
    {
        case "start":
            start();
        break;
        case "close":
            close();
        break;
        case "restart":
            restart();
        break;
        default:
            alert("请选择操作类型！");
        break;
    }
}

function game_svr_submit(hostname, index)
{
    selectType = $("input[name='game_svr_"+index+"']:checked").val();
    switch(selectType)
    {
        case "start":
            start(hostname, index);
        break;
        case "close":
            close(hostname, index);
        break;
        case "restart":
            restart(hostname, index);
        break;
        default:
            alert("请选择操作类型！");
        break;
    }
}
