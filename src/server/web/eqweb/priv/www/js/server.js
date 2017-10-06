$(document).ready(function () {
    server_list();
    $("#submit").click(
        function() 
        { 
	        select();
        });
});

function get_status(hostname, index)
{
    $.ajax(
	{
        url: "server_status/"+hostname,
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
        };
    }
}

function server_list()
{
    $.ajax(
	{
        url: "server_list",
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
                        $("#server_row").remove();
                    }
                    $("#server_table").append(msg);
                    monitor_status();
                break;
            }
        }
    });
}

function close_add_server()
{
    window.parent.cancelPopup();
    window.parent.server_list();
}

function add_server()
{
    var ip = $("#ip").val();
    var hostname = $("#hostname").val();
    var servername = $("#servername").val();
    $.ajax(
	{
        url: "server_add?ip="+ip+"&hostname="+hostname+"&servername="+servername,
        cache: false,
        success: 
        function(msg)
        {
            switch(msg)
            {
                case "ok":
                    alert("更新成功！");
                    window.parent.cancelPopup();
                    window.parent.server_list();
                break;
                default:
                    alert(msg);
                break;
            }
        }
    });
}

function delete_server(hostname)
{
    var ret = confirm('确定将此记录删除?');
    if(ret == true)
    {
        $.ajax(
	    {
            url: "server_delete?hostname="+hostname,
            cache: false,
            success: 
            function(msg)
            {
                switch(msg)
                {
                    case "ok":
                        alert("删除成功！");
                        server_list();
                    break;
                    default:
                        alert(msg);
                    break;
                }
            }
        });
    }
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

function server_submit(hostname, index)
{
    selectType = $("input[name='server_"+index+"']:checked").val();
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

function start(hostname, index)
{
    $.ajax(
	{
        url: "start_server/"+hostname,
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
        url: "close_server/"+hostname,
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
        url: "restart_server/"+hostname,
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