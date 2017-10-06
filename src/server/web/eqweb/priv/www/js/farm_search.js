$(document).ready(function() {
    $("#search").click(
        function() 
        { 
	        search_farm_player();
        });
});

function search_farm_player()
{
    var account = $("#txt_account").val();
    var hostname = $.query.get('hostname'); 
    $.ajax(
	{
        url: "search_farm_player?account="+account+"&hostname="+hostname,
        cache: false,
        success: 
        function(msg)
        {
            switch(msg)
            {
                case "fail":
                    $("#farm_row").remove();
                    alert("没有查询到数据！");
                break;
                default:
                    $("#farm_row").remove();
                    $("#farm_table").append(msg);
                break;
            }
        }
    });
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
                    window.parent.search_farm_player();
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

function format(value)
{
    if(value == "")
        return 0;
    else
        return value;
}