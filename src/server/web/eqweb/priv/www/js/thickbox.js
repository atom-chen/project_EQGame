/*
 * Thickbox 3.1 - One Box To Rule Them All.
 * By Cody Lindley (http://www.codylindley.com)
 * Copyright (c) 2007 cody lindley
 * Licensed under the MIT License: http://www.opensource.org/licenses/mit-license.php
*/
		  
var tb_pathToImage = "/images/loadingAnimation.gif";
var message;
var zIndex = 100;
var keyArray = new Array;
var callbackArray = new Array;
var closeFunctionArray = new Array;
var confirmCallback = null;
var callbackControlId = null;
/*!!!!!!!!!!!!!!!!! edit below this line at your own risk !!!!!!!!!!!!!!!!!!!!!!!*/

//on page load call tb_init
$(document).ready(function(){   
	imgLoader = new Image();// preload image
	imgLoader.src = tb_pathToImage;
});

function closePopup(data)
{
    if(keyArray.length > 0)
    {
        var key = keyArray.pop();
        removeByKey(key);
        var callbackFunction = callbackArray[key];
        if(callbackFunction != null)
        {
            if(data != null)
                callbackFunction(data);
            else
                callbackFunction();
        }
    }
}

function cancelPopup()
{
    if(keyArray.length > 0)
    {
        var key = keyArray.pop();
        removeByKey(key);
    }
}

function openPopup(caption, url, width, height, callback, isManuallyClose)
{
    var MergeUrl = "width="+width+"&height="+height+"&TB_iframe=true";
    if(url.indexOf("?")>-1 || url.indexOf("&") > -1)
    {
        url = url + "&" + MergeUrl;
    }
    else
    {
        url = url + "?" + MergeUrl;
    }
    var key = Math.round(Math.random()*100000);
    registerCallback(key, callback);
    registerCloseFunction(key, isManuallyClose);
    keyArray.push(key);
    tb_show(caption, url, null, key);
}

function registerCallback(key, callback)
{
    if(callback != null)
    {
        callbackArray.push(key);
        callbackArray[key] = callback;
    }
}

function registerCloseFunction(key, isManuallyClose)
{
    if(isManuallyClose != null)
    {
        closeFunctionArray.push(key);
        closeFunctionArray[key] = isManuallyClose;
    }
}

function showConfirm(caption, content, width, height, callback, controlId)
{
    message = content;
    var mergeUrl = "width="+width+"&height="+height+"&modal=true&Confirm=true";
    var url = "index.html";
    if(url.indexOf("?")>-1 || url.indexOf("&") > -1)
    {
        url = url + "&" + mergeUrl;
    }
    else
    {
        url = url + "?" + mergeUrl;
    }
    
    confirmCallback = callback;
    callbackControlId = controlId;
    
    tb_show(caption, url, null, Math.round(Math.random()*100000));
}

function closeConfirm(key, data)
{
    removeByKey(key);
    
    if(confirmCallback != null && callbackControlId != null)
        confirmCallback(callbackControlId, data);
}

function showMessage(caption, content, width, height, callback, controlId, focusType)
{
    confirmCallback = callback;
    callbackControlId = controlId;
    message = content;
    var mergeUrl = "width="+width+"&height="+height+"&modal=true&Message=true";
    var url = "index.html";
    if(url.indexOf("?")>-1 || url.indexOf("&") > -1)
    {
        url = url + "&" + mergeUrl;
    }
    else
    {
        url = url + "?" + mergeUrl;
    }
    
    tb_show(caption, url, null, Math.round(Math.random()*100000), focusType);
}

function closeMessage(key, focusType)
{    
    removeByKey(key);
    if(confirmCallback != null && callbackControlId != null)
        confirmCallback(callbackControlId, focusType);
}

function tb_show(caption, url, imageGroup, key, focusType) {//function called when the user clicks on a thickbox link

	try {
		if (typeof document.body.style.maxHeight === "undefined") {//if IE 6
			$("body","html").css({height: "100%", width: "100%"});
			$("html").css("overflow","hidden");
			if (document.getElementById("TB_HideSelect"+key) === null) {//iframe to hide select elements in ie6
				$("body").append("<iframe id='TB_HideSelect"+key+"' class='TB_HideSelect TB_HideSelect_IE6'></iframe><div id='TB_overlay"+key+"' class='TB_overlay_IE6' style='z-index:"+ ++zIndex +"'></div><div id='TB_window"+key+"' onmousedown=\"startDrag(this)\" onmouseup=\"stopDrag(this)\" onmousemove=\"drag(this)\"  class='TB_window_IE6' style='z-index:"+ ++zIndex +"'></div>");
			}
		}else{//all others
			if(document.getElementById("TB_overlay"+key) === null){
				$("body").append("<div id='TB_overlay"+key+"' class='TB_overlay' style='z-index:"+ ++zIndex +"'></div><div id='TB_window"+key+"' onmousedown=\"startDrag(this)\" onmouseup=\"stopDrag(this)\" onmousemove=\"drag(this)\" class='TB_window' style='z-index:"+ ++zIndex +"'></div>");
			}
		}
		
		if(tb_detectMacXFF()){
			$("#TB_overlay"+key).addClass("TB_overlayMacFFBGHack");//use png overlay so hide flash
		}else{
			$("#TB_overlay"+key).addClass("TB_overlayBG");//use background and opacity
		}
		
		if(caption===null){caption="";}
		$("body").append("<div id='TB_load"+key+"' class='TB_load'><img src='"+imgLoader.src+"' /></div>");//add loader to the page
		$("#TB_load"+key).show();//show loader
		
		var baseURL;
	   if(url.indexOf("?")!==-1){ //ff there is a query string involved
			baseURL = url.substr(0, url.indexOf("?"));
	   }else{ 
	   		baseURL = url;
	   }
	   
		var queryString = url.replace(/^[^\?]+\??/,'');
		var params = tb_parseQuery( queryString );

		TB_WIDTH = (params['width']*1) + 30 || 630; //defaults to 630 if no paramaters were added to URL
		TB_HEIGHT = (params['height']*1) + 40 || 440; //defaults to 440 if no paramaters were added to URL
		ajaxContentW = TB_WIDTH - 30;
		ajaxContentH = TB_HEIGHT - 45;
		
		if(url.indexOf('TB_iframe') != -1){// either iframe or ajax window		
				urlNoQuery = url.split('TB_');
				$("#TB_iframeContent"+key).remove();
				if(params['modal'] != "true"){//iframe no modal
					$("#TB_window"+key).append("<div id=\"TB_title"+key+"\" class='TB_title'><div id='TB_ajaxWindowTitle"+key+"' class='TB_windowTitle TB_ajaxWindowTitle'>"+caption+"</div><div id='TB_closeAjaxWindow"+key+"' class='TB_closeAjaxWindow'><a href='#' id='TB_closeWindowButton"+key+"' class='TB_closeWindowButton' title='Close'></a></div></div><iframe frameborder='0' hspace='0' src='"+urlNoQuery[0]+"' id='TB_iframeContent"+key+"' class='TB_iframeContent' name='TB_iframeContent"+Math.round(Math.random()*1000)+"' style='width:"+(ajaxContentW)+"px;height:"+(ajaxContentH + 17)+"px;'> </iframe><div id='TB_footer"+key+"' class='TB_footer'></div>");
					tb_showIframe(key);
				}else{//iframe modal
				$("#TB_overlay").unbind();
				
				$("#TB_window").append("<iframe frameborder='0' hspace='0' src='"+urlNoQuery[0]+"' id='TB_iframeContent' name='TB_iframeContent"+Math.round(Math.random()*1000)+"' onload='tb_showIframe()' style='width:"+(ajaxContentW)+"px;height:"+(ajaxContentH + 17)+"px;'> </iframe>");
				}
		}else{// not an iframe, ajax
				if($("#TB_window"+key).css("display") != "block"){
					    if(params['modal'] != "true"){//ajax no modal
					    $("#TB_window"+key).append("<div id='TB_title'><div id='TB_ajaxWindowTitle' class='TB_windowTitle'>"+caption+"</div><div id='TB_closeAjaxWindow'><a href='#' id='TB_closeWindowButton'></a></div></div><div id='TB_ajaxContent' style='width:"+ajaxContentW+"px;height:"+ajaxContentH+"px'></div><div id='TB_footer'></div>");
					    }else{//ajax modal
					    $("#TB_overlay"+key).unbind();
    					
					    if(params['Confirm'] == "true"){
					        $("#TB_window"+key).append("<div id='TB_title' class='TB_title'><div id='TB_ajaxWindowTitle' class='TB_ajaxWindowTitle TB_messageTitle'>"+caption+"</div><div class='right'><a href='javascript:void(0);closeConfirm("+key+", 'N'); id='TB_closeWindowButton'></a></div></div><div id='TB_ajaxContent' class='TB_ajaxContent TB_modal' style='width:"+ajaxContentW+"px;height:"+ajaxContentH+"px;'><div ID='TB_MessageContent' class='TB_MessageContent'>"+message+"</div></div><div id='TB_Button' class='TB_Button'><input id='message' type='button' onclick=\"closeConfirm("+key+", 'Y');\" value=' 确定 ' class='greenBtn' />&nbsp;&nbsp;&nbsp;&nbsp;<input id='Button1' type='button' onclick=\"closeConfirm("+key+", 'N');\" value=' 取消 ' class='greenBtn' /></div><div id='TB_footer' class='TB_footer'></div>");	
					    }
    					
					    if(params['Message'] =="true")
				        {
				            $("#TB_window"+key).append("<div id='TB_title' class='TB_title'><div id='TB_ajaxWindowTitle' class='TB_ajaxWindowTitle TB_messageTitle'>"+caption+"</div><div class='right'><a href=\"javascript:void(0);closeMessage("+key+", '"+focusType+"');\" id='TB_closeWindowButton'></a></div></div><div id='TB_ajaxContent' class='TB_ajaxContent TB_modal' style='width:"+ajaxContentW+"px;height:"+ajaxContentH+"px;'><div ID='TB_MessageContent' class='TB_MessageContent'>"+message+"</div></div><div id='TB_Button' class='TB_Button'><input id='message' type='button' onclick=\"closeMessage("+key+", '"+focusType+"');\" value=' 确定 ' class='greenBtn' /></div><div id='TB_footer' class='TB_footer'></div>");	
				            
				        }
					}
				}else{//this means the window is already up, we are just loading new content via ajax
					$("#TB_ajaxContent"+key)[0].style.width = ajaxContentW +"px";
					$("#TB_ajaxContent"+key)[0].style.height = ajaxContentH +"px";
					$("#TB_ajaxContent"+key)[0].scrollTop = 0;
					$("#TB_ajaxWindowTitle"+key).html(caption);
				}
		}
		
		$("#TB_closeWindowButton"+key).click(closeWindow);
		
			if(url.indexOf('TB_inline') != -1){	
				$("#TB_ajaxContent").append($('#' + params['inlineId']).children());
				$("#TB_window").unload(function () {
					$('#' + params['inlineId']).append( $("#TB_ajaxContent").children() ); // move elements back when you're finished
				});
				tb_position();
				$("#TB_load").remove();
				$("#TB_window").css({display:"block"}); 
			}else if(url.indexOf('TB_iframe') != -1){
				tb_position(key);
				if($.browser.safari){//safari needs help because it will not fire iframe onload
					$("#TB_load"+key).remove();
					$("#TB_window"+key).css({display:"block"});
				}
			}else{
			    tb_position(key);
				$("#TB_load"+key).remove();
				$("#TB_window"+key).css({display:"block"});
			}

		if(!params['modal']){
			document.onkeyup = function(e){ 	
				if (e == null) { // ie
					keycode = event.keyCode;
				} else { // mozilla
					keycode = e.which;
				}
				if(keycode == 27){ // close
					tb_remove();
				}	
			};
		}
		if(params['Message'] =="true" || params['Confirm'] == "true")
		    setFocus();
	} catch(e) {
		//nothing here
	}
}

function setFocus()
{
    document.getElementById("message").focus();
}

function closeWindow()
{
    if(keyArray.length > 0)
    {
        var key = keyArray[keyArray.length - 1];
        var closeFunction = closeFunctionArray[key];
        if(closeFunctionArray.length > 0 && closeFunction != null && closeFunction != "")
        {
            window.frames["TB_iframeContent"+key+""].eval(""+ closeFunction +"()");
        }
        else
            tb_remove();   
    }
}

//helper functions below
function tb_showIframe(key){
	$("#TB_load"+key).remove();
	$("#TB_window"+key).css({display:"block"});
}

function tb_remove() 
{
    var currentKey = keyArray.pop();
 	removeByKey(currentKey);
}

function removeByKey(currentKey) 
{
    $("#TB_imageOff"+currentKey).unbind("click");
	$("#TB_closeWindowButton"+currentKey).unbind("click");
	$("#TB_window"+currentKey).fadeOut("fast",function(){$("#TB_window"+currentKey+",#TB_overlay"+currentKey+",#TB_HideSelect"+currentKey+"").trigger("unload").unbind().remove();});
	$("#TB_load"+currentKey).remove();
	if (typeof document.body.style.maxHeight == "undefined") {//if IE 6
		$("body","html").css({height: "auto", width: "auto"});
		$("html").css("overflow","");
	}
	document.onkeydown = "";
	document.onkeyup = "";
	return false;
}

//function calculateLocation(x, y)
//{
//    divLeft = (document.body.clientWidth - TB_WIDTH) / 2 + moveX + x;
//    divTop = (document.body.clientHeight - TB_HEIGHT) / 2 + moveY + y;
//    this.moveX = x;
//    this.moveY = y;
//}

function getDivX()
{
    return parseInt($("#TB_window"+keyArray[keyArray.length-1])[0].offsetLeft,10); 
}

function getDivY()
{
    return parseInt($("#TB_window"+keyArray[keyArray.length-1])[0].offsetTop,10) + 32; 
}

function tb_position(key) {
$("#TB_window"+key).css({marginLeft: '-' + parseInt((TB_WIDTH / 2),10) + 'px', width: TB_WIDTH + 'px'});
	if ( !(jQuery.browser.msie && jQuery.browser.version < 7)) { // take away IE6
		$("#TB_window"+key).css({marginTop: '-' + parseInt((TB_HEIGHT / 2),10) + 'px'});
	}
}

function tb_parseQuery ( query ) {
   var Params = {};
   if ( ! query ) {return Params;}// return empty object
   var Pairs = query.split(/[;&]/);
   for ( var i = 0; i < Pairs.length; i++ ) {
      var KeyVal = Pairs[i].split('=');
      if ( ! KeyVal || KeyVal.length != 2 ) {continue;}
      var key = unescape( KeyVal[0] );
      var val = unescape( KeyVal[1] );
      val = val.replace(/\+/g, ' ');
      Params[key] = val;
   }
   return Params;
}

function tb_getPageSize(){
	var de = document.documentElement;
	var w = window.innerWidth || self.innerWidth || (de&&de.clientWidth) || document.body.clientWidth;
	var h = window.innerHeight || self.innerHeight || (de&&de.clientHeight) || document.body.clientHeight;
	arrayPageSize = [w,h];
	return arrayPageSize;
}

function tb_detectMacXFF() {
  var userAgent = navigator.userAgent.toLowerCase();
  if (userAgent.indexOf('mac') != -1 && userAgent.indexOf('firefox')!=-1) {
    return true;
  }
}

var mouseX = 0, mouseY = 0, divX = 0, divY = 0;   
var moveable=false;   
//开始拖动;   
function startDrag(obj)   
{   
    if(event.button == 1)   
    {
        //锁定标题栏;   
        obj.setCapture();   
        obj.style.cursor = "move";
        //记录鼠标和层位置;   
        mouseX = event.clientX;   
        mouseY = event.clientY;   
        divX = parseInt(obj.style.marginLeft,10);   
        divY = parseInt(obj.style.marginTop,10);   
        moveable = true;   
    }   
}   
//拖动;   
function drag(obj)   
{   
    if(moveable)   
    {
        obj.style.marginLeft = divX + event.clientX - mouseX;
        obj.style.marginTop = divY + event.clientY - mouseY;
//        calculateLocation(event.clientX - mouseX, event.clientY - mouseY);
    }   
}   

//停止拖动;   
function stopDrag(obj)   
{   
    if(moveable)   
    {   
        obj.style.cursor = "auto";
        obj.releaseCapture();   
        moveable = false;   
    }   
}   