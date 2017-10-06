$(document).ready(function () {
    scene_monitor();
});

$(document).everyTime(3000, 'timer', function() {
    scene_monitor();
    total_user();
});

function total_user()
{
    var hostname = $.query.get('hostname'); 
    $.ajax(
	{
        url: "total_user/"+hostname,
        cache: false,
        success: 
        function(msg)
        {
            $("#TotalUser").text(msg);
        }
    });
}

function scene_monitor()
{
    var hostname = $.query.get('hostname'); 
    $.ajax(
	{
        url: "scene_monitor/"+hostname,
        cache: false,
        success: 
        function(msg)
        {
            var datas = msg.split("|");
            $.each(datas, function(i, n) {
                $("#scene_row").remove();
            });
            $.each(datas, function(i, n) {
                var data = n.split(",");
                $("#scene_table").append("<tr id=\"scene_row\" style=\"background-color:#E7E8EB\"><td id=\"scene_id\">"+data[0]+"</td><td id=\"scene_name\">"+data[1]+"</td><td id=\"scene_players_count\">"+data[2]+"</td></tr>");
            });
        }
    });
}