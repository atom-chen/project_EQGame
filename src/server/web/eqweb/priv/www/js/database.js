$(document).ready(function () {
    database_list();
});

function database_list()
{
    $.ajax(
	{
        url: "database_list",
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
                    var total_database = $("#total_database").val();
                    for(var i = 1; i <= total_database; i++)
                    {
                        $("#database_row").remove();
                    }
                    $("#database_table").append(msg);
                    database_status(); 
                break;
            }
        }
    });
}

function show_node_list()
{
    var total_database = $("#total_database").val();
    for(var i = 1; i <= total_database; i++)
    {
        hostname = $("#hostname_"+i).html();
        node_list(hostname);
    }
}

function node_list(hostname)
{
    $.ajax(
	{
        url: "database_node_list/"+hostname,
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
                    var total_database = $("#total_database").val();
                    for(var i = 1; i <= total_database; i++)
                    {
                        $("#node_row").remove();
                    }
                    $("#node_table").append(msg);
                    $("#node_table").fadeIn("normal");

                break;
            }
        }
    });
}

function add_activity(hostname, tablename)
{
    var ret = confirm('确定要增加分片表?');
    if(ret == true)
    {
        $.ajax(
	    {
            url: "database_add_activity?hostname="+hostname+"&tablename="+tablename,
            cache: false,
            success: 
            function(msg)
            {
                switch(msg)
                {
                    case "ok":
                        alert("增加分片表成功！");
                        show_node_list();
                    break;
                    default:
                        alert("增加分片表失败！");
                    break;
                }
            }
        });
    }
}

function show_add_node(hostname)
{
    openPopup("增加数据库节点", "node_add.html?hostname="+hostname, 250, 150, null, null);
}

function close_add_node()
{
    window.parent.cancelPopup();
}

function add_node()
{
    var nhostname = $("#nhostname").val();
    var servername = $("#servername").val();
    var ip = $("#ip").val();
    var hostname = $.query.get('hostname'); 
    $.ajax(
	{
        url: "database_add_node?hostname="+hostname+"&nhostname="+nhostname+"&servername="+servername+"&ip="+ip,
        cache: false,
        success: 
        function(msg)
        {
            switch(msg)
            {
                case "ok":
                    alert("增加节点成功！");
                    window.parent.cancelPopup();
                    window.parent.show_node_list();
                break;
                default:
                    alert("增加节点失败！");
                break;
            }
        }
    });
}

function database_status()
{
    var total_database = $("#total_database").val();
    if(total_database != undefined)
    {
        for(var i = 1; i <= total_database; i++)
        {
            $("#status_"+i).attr("src","images/loading.gif"); 
            hostname = $("#hostname_"+i).html();
            get_status(hostname, i);
        };
    }
}

function get_status(hostname, index)
{
    $.ajax(
	{
        url: "database_status/"+hostname,
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


function start(hostname, index)
{
    $.ajax(
	{
        url: "start_database/"+hostname,
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
        url: "close_database/"+hostname,
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
        url: "restart_database/"+hostname,
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

function database_submit(hostname, index)
{
    selectType = $("input[name='database_"+index+"']:checked").val();
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