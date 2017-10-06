$(document).ready(function() {
    $("#search").click(
        function() 
        { 
	        search_player();
        });
    
    $("#player_row").dblclick( function () { alert("Hello World!"); }); 
});

function search_player()
{
    var account = $("#txt_account").val();
    var hostname = $.query.get('hostname'); 
    $.ajax(
	{
        url: "search_player?account="+account+"&hostname="+hostname,
        cache: false,
        success: 
        function(msg)
        {
            switch(msg)
            {
                case "fail":
                    $("#player_row").remove();
                    alert("没有查询到数据！");
                break;
                default:
                    $("#player_row").remove();
                    $("#player_table").append(msg);
                break;
            }
        }
    });
}

function show_add_item(hostname, account)
{
    openPopup("增加物品", "player_add_item.html?account="+account+"&hostname="+hostname, 250, 50, null, null);
}

function add_item()
{
    var hostname = $.query.get('hostname'); 
    var account = $.query.get('account');
    var item_id = $("#item_id").val();
    $.ajax(
	{
        url: "player_add_item?hostname="+hostname+"&account="+account+"&item_id="+item_id,
        cache: false,
        success: 
        function(msg)
        {
            switch(msg)
            {
                case "ok":
                    alert("增加物品成功！");
                    window.parent.cancelPopup();
                    window.parent.search_player();
                break;
                default:
                    alert("增加物品失败！");
                break;
            }
        }
    });
}

function close_add_item()
{
    window.parent.cancelPopup();
}

function show_add_coin(hostname, account)
{
    openPopup("增加金币", "player_add_coin.html?account="+account+"&hostname="+hostname, 250, 50, null, null);
}

function add_coin()
{
    var hostname = $.query.get('hostname'); 
    var account = $.query.get('account');
    var coin = $("#coin").val();
    $.ajax(
	{
        url: "player_add_coin?hostname="+hostname+"&account="+account+"&coin="+coin,
        cache: false,
        success: 
        function(msg)
        {
            switch(msg)
            {
                case "ok":
                    alert("增加金币成功！");
                    window.parent.cancelPopup();
                    window.parent.search_player();
                break;
                default:
                    alert("增加金币失败！");
                break;
            }
        }
    });
}

function close_add_coin()
{
    window.parent.cancelPopup();
}

function show_add_farm_exp(hostname, account)
{
    openPopup("增加农场经验", "player_add_farm_exp.html?account="+account+"&hostname="+hostname, 250, 50, null, null);
}

function add_farm_exp()
{
    var hostname = $.query.get('hostname'); 
    var account = $.query.get('account');
    var farm_exp = $("#farm_exp").val();
    $.ajax(
	{
        url: "player_add_farm_exp?hostname="+hostname+"&account="+account+"&farm_exp="+farm_exp,
        cache: false,
        success: 
        function(msg)
        {
            switch(msg)
            {
                case "ok":
                    alert("增加农场经验成功！");
                    window.parent.cancelPopup();
                    window.parent.search_player();
                break;
                default:
                    alert("增加农场经验失败！");
                break;
            }
        }
    });
}

function close_add_farm_exp()
{
    window.parent.cancelPopup();
}



function show_add_property(hostname, account)
{
    openPopup("增加属性", "player_add_property.html?account="+account+"&hostname="+hostname, 250, 150, null, null);
}

function add_property(hostname, account)
{
    var hostname = $.query.get('hostname'); 
    var account = $.query.get('account');
    var player_clean = format($("#player_clean").val());
    var player_health = format($("#player_health").val());
    var player_charm = format($("#player_charm").val());
    var active_value = format($("#active_value").val());
    var house_clean = format($("#house_clean").val());
    $.ajax(
	{
        url: "player_add_property?hostname="+hostname+"&account="+account+"&player_clean="+player_clean+"&player_health="+player_health+"&player_charm="+player_charm+"&active_value="+active_value+"&house_clean="+house_clean,
        cache: false,
        success: 
        function(msg)
        {
            switch(msg)
            {
                case "ok":
                    alert("增加属性成功！");
                    window.parent.cancelPopup();
                    window.parent.search_player();
                break;
                default:
                    alert("增加属性失败！");
                break;
            }
        }
    });
}

function close_add_property()
{
    window.parent.cancelPopup();
}

function show_edit_gm_level(hostname,account)
{
    openPopup("设置GM权限", "player_edit_gm.html?account="+account+"&hostname="+hostname, 250, 150, null, null);
}
function edit_gm_level()
{
    var hostname = $.query.get('hostname'); 
    var account = $.query.get('account');
    var gmlevel = ($("#gm_level").attr("checked"))?"enable":"disable";
    $.ajax(
	{
        url: "edit_gm_level?hostname="+hostname+"&account="+account+"&level="+gmlevel,
        cache: false,
        success: 
        function(msg)
        {
            switch(msg)
            {
                case "ok":
                    alert("修改GM权限成功！");
                    window.parent.cancelPopup();
                    window.parent.search_player();
                break;
                default:
                    alert("修改GM权限失败！");
                break;
            }
        }
    });
}


function format(value)
{
    if(value == "")
        return 0;
    else
        return value;
}