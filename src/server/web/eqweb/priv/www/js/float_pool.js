$(document).ready(function() {
	$("#button_add_bottle").click(
	function()
	{
		add_gm_bottle();
	});

	$("#button_get_gm_bottle_count").click(
	function()
	{
		get_gm_bottle_count();
	});

	$("#button_clear_all_gm_bottle").click(
	function()
	{
		clear_all_gm_bottle();
	});

	$("#button_reset").click(
	function()
	{
		do_reset();
	});
});


function do_reset()
{
  document.getElementById("reward_id").value = "0";
  document.getElementById("bottle_count").value = "1";
  document.getElementById("gm_name").value = "";
  document.getElementById("content").value = "";
}


function input_is_valid(content, gm_name, reward_id, count)
{
	var regx=/^\d{1,4}$/;
	if(!regx.test(count))
	{
		alert("允许输入投入的数量为（1-1000）");
		return false;
	}
	
	if (count > 1000)
	{
		alert("允许输入投入的数量为（1-1000）");
		return false;
	}
		
	var regID=/^\d{1,5}$/;
	if(!regID.test(reward_id))
	{
		alert("奖励ID必需为数字！ ");
		return false;
	}
	
	if (content.length == 0)
	{
		alert("没输入内容是无法投入的 ");
		return false;
	}
	
	if (content.length > 140)
	{
		alert("允许输入的字数为1-140字 ");
		return false;
	}
	
	return true;
}

function add_gm_bottle()
{
	//var ret = confirm('Are you sure to add records?');
	var hostname = $.query.get('hostname');

	var content = $("#content").val();
	var gm_name = $("#gm_name").val();
	var reward_id = $("#reward_id").val();
	var count = $("#bottle_count").val();
	if (!input_is_valid(content, gm_name, reward_id, count))
		return false;
		
	$.ajax(
	{
		url: "add_gm_bottle?hostname="+hostname+"&msg="+escape(content)+"&bottle_count="+count+"&gm_name="+escape(gm_name)+"&reward_id="+reward_id,
		cache: false,
		success:
		function(msg)
		{
			switch(msg)
			{
				case "ok":
				alert("添加成功");
				var iOld = parseInt(document.getElementById("label_gm_count").value);
				document.getElementById("label_gm_count").value = iOld + parseInt(count);	
				break;
				default:
				alert("添加失败");
				break;
			}
		}
	});
}


function get_gm_bottle_count()
{
	//var ret = confirm('Are you sure to get records?');

	var hostname = $.query.get('hostname');
	$.ajax(
	{
		url: "get_gm_bottle_count?hostname="+hostname,
		cache: false,
		success:
		function(msg)
		{
			var len = msg.length;
			var idx = msg.indexOf(",");
			if (0 <= idx && idx < len)
			{
				document.getElementById("label_gm_count").value = msg.substring(0, idx);
				document.getElementById("label_player_count").value = msg.substring(idx+1, len);
			}
			else
			{
					alert("请求失败 ");				
			}
		}
	});

}


function clear_all_gm_bottle()
{
	var ret = confirm('你确定要删除该服务器的所有官方瓶子信息吗？');
	if(ret == true)
	{
		var hostname = $.query.get('hostname');
		$.ajax(
		{
			url: "clear_all_gm_bottle?hostname="+hostname,
			cache: false,
			success:
			function(msg)
			{
				switch(msg)
				{
					case "ok":
					alert("删除成功 ");
					document.getElementById("label_gm_count").value = "0";
					break;
					default:
					alert("删除失败 ");
					break;
				}
			}
		});
	}
}





