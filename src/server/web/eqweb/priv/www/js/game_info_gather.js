$(document).ready(function() {
    $("#button_start_gather").click(
        function() 
        { 
	        start_gather();
        });
});

function start_gather()
{
    var ret = confirm("该操作比较耗时，是否继续？");
    if(ret == true)
    {
	var hostname = $.query.get('hostname'); 
        $.ajax(
	    {
            url: "game_info_gather?hostname="+hostname,
            cache: false,
            success: 
            function(msg)
            {
		document.getElementById("memo1").value = msg;
            }
        });
    }
}
