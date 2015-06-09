<?php
/*
Plugin Name: Wordpress Video Plugin
Plugin URI: http://www.daburna.de/blog/2006/12/13/wordpress-video-plugin/
Description: A filter for WordPress that displays videos from myvideo.de, youtube, google video, Stage6, Brightcove, Aniboom, 123video.nl, MyspaceTV, Yahoo! Video, Veoh, Wandeo, Glumbert, LiveLeak, sevenload.de, clipfish.de, break.com, Metacafe, dailymotion.com, Vimeo, videotube, blip.tv, revver, IFILM, Tu.tv, grouper, Cellfish.com, Sumo.tv, Last.fm, Vsocial, teachertube, SlideShare...others coming soon. Modified by <a href="http://www.daburna.de">Oliver Wunder</a>
Version: 0.62
Author: Oliver Wunder 
Author URI: http://www.daburna.de/

*/
// Politube.org
/*
define("POLITUBE_WIDTH", 450);
define("POLITUBE_HEIGHT", 400);
define("POLITUBE_REGEXP", "/\[politube ([[:print:]]+)\]/");
define("POLITUBE_TARGET", "<object classid=\"clsid:d27cdb6e-ae6d-11cf-96b8-444553540000a\" codebase=\"http://fpdownload.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=8,0,0,0\" type=\"application/x-shockwave-flash\" width=\"".POLITUBE_WIDTH."\" height=\"".POLITUBE_HEIGHT."\"> <param name=\"movie\" value=\"http://politube.org/flvplayer.swf\" /> <param id=\"param_###VID_ID###\" name=\"flashvars\" value=\"file=http://politube.org/flv/media_###VID_ID###.flv&image=http://politube.org/thumb/###VID_ID###&largecontrols=true&showdigits=true&callback=http://politube.org/flv_callback/###VID_ID###&autostart=false&allowfullscreen=true\"/> <embed  id=\"embed_###VID_ID###\" src=\"http://politube.org/flvplayer.swf\" type=\"application/x-shockwave-flash\" pluginspage=\"http://www.macromedia.com/go/getflashplayer\" width=\"".POLITUBE_WIDTH."\" height=\"".POLITUBE_HEIGHT."\" flashvars=\"file=http://politube.org/flv/media_###VID_ID###.flv&image=http://politube.org/thumb/###VID_ID###&largecontrols=true&showdigits=true&callback=http://politube.org/flv_callback/###VID_ID###&autostart=false&allowfullscreen=true\" /> </object>");
*/

// This object is defined merely so that there is a global object that will hold the target string and can then
// be referenced inside the replacement callback.

Class videoObjectString
{
    var $width =null;
    var $height=null;
    var $target_video_string =null;
 
    function videoObjectString() { }
    function setVideoObjectString($target_str)
    {
        $this->target_video_string = $target_str;
    }
    function setVideoObjectSize($width, $height)
    {
        $this->width = $width;
        $this->height = $height;
    }
}

$videoObject = new videoObjectString();


function politube_plugin_callback($match) {

	global $videoObject;
	$output = $videoObject->target_video_string;
	$output = str_replace("###VID_ID###", $match[1], $output);
	return ($output);
}

function politube_plugin($content, $width=null, $height=null) {

if (isset($width) && $width > 0 ) $POLITUBE_WIDTH= $width;
else $POLITUBE_WIDTH= 450;

if (isset($height) && $height > 0 ) $POLITUBE_HEIGHT= $height;
else $POLITUBE_HEIGHT= 400;

$POLITUBE_REGEXP= "/\[politube ([[:print:]]+)\]/";
$POLITUBE_TARGET= "<object classid=\"clsid:d27cdb6e-ae6d-11cf-96b8-444553540000a\" codebase=\"http://fpdownload.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=8,0,0,0\" type=\"application/x-shockwave-flash\" width=\"".$POLITUBE_WIDTH."\" height=\"".$POLITUBE_HEIGHT."\"> <param name=\"movie\" value=\"http://politube.org/flvplayer.swf\" /> <param id=\"param_###VID_ID###\" name=\"flashvars\" value=\"file=http://politube.org/flv/media_###VID_ID###.flv&image=http://politube.org/thumb/###VID_ID###&largecontrols=true&showdigits=true&callback=http://politube.org/flv_callback/###VID_ID###&autostart=false&allowfullscreen=true\"/> <embed  id=\"embed_###VID_ID###\" src=\"http://politube.org/flvplayer.swf\" type=\"application/x-shockwave-flash\" pluginspage=\"http://www.macromedia.com/go/getflashplayer\" width=\"".$POLITUBE_WIDTH."\" height=\"".$POLITUBE_HEIGHT."\" flashvars=\"file=http://politube.org/flv/media_###VID_ID###.flv&image=http://politube.org/thumb/###VID_ID###&largecontrols=true&showdigits=true&callback=http://politube.org/flv_callback/###VID_ID###&autostart=false&allowfullscreen=true\" /> </object>";

        global $videoObject;
        $videoObject->setVideoObjectString($POLITUBE_TARGET);
	return preg_replace_callback($POLITUBE_REGEXP, 'politube_plugin_callback', $content);
}

// Reason.tv

define("REASON_REGEXP", "/\[reason ([[:print:]]+)\]/");
define("REASON_TARGET", "<script type=\"text/javascript\" src=\"http://www.reason.tv/embed/video.php?id=###ID###\"></script>");

function reason_plugin_callback($match) {
	$output = REASON_TARGET;
	$output = str_replace("###ID###", $match[1], $output);
	return ($output);
}

function reason_plugin($content) {
	return preg_replace_callback(REASON_REGEXP, 'reason_plugin_callback', $content);
}


// SlideShare Slides

define("SS_WIDTH", 425);
define("SS_HEIGHT", 348);
define("SS_REGEXP", "/\[slideshare ([[:print:]]+) ([[:print:]]+)\]/");

define("SS_TARGET", "<object type=\"application/x-shockwave-flash\" data=\"https://s3.amazonaws.com:443/slideshare/ssplayer.swf?id=###id###&doc=###doc###\"
width=\"".SS_WIDTH."\" height=\"".SS_HEIGHT."\"><param name=\"movie\" value=\"https://s3.amazonaws.com:443/slideshare/ssplayer.swf?id=###id###&doc=###doc###\" /></object>");

function ss_plugin_callback($match){
        $output = SS_TARGET;
	$output = str_replace("###id###", $match[1], $output);
	$output = str_replace("###doc###", $match[2], $output);
	return ($output);
}

function ss_plugin($content){
	return (preg_replace_callback(SS_REGEXP, 'ss_plugin_callback', $content));
}



// Teachertube.com code

define("TT_WIDTH", 425);
define("TT_HEIGHT", 350);
define("TT_REGEXP", "/\[teachertube ([[:print:]]+)\]/");
define("TT_TARGET", "<embed src=\"http://www.teachertube.com/skin-p/flvplayer.swf\" allowfullscreen=\"true\" flashvars=\"&file=http://www.teachertube.com/flvideo/###tt_mu###.flv&image=http://www.teachertube.com/thumb/###tt_mu###.jpg&location=http://www.teachertube.com/skin-p/flvplayer.swf&logo=http://www.teachertube.com/images/greylogo.swf&frontcolor=0xffffff&backcolor=0x000000&lightcolor=0xFF0000&autostart=false&volume=80&overstretch=fit\" quality=\"high\" bgcolor=\"#000000\" wmode=\"transparent\" width=\"".TT_WIDTH."\" height=\"".TT_HEIGHT."\" loop=\"false\" align=\"middle\" type=\"application/x-shockwave-flash\" pluginspage=\"http://www.macromedia.com/go/getflashplayer\"> </embed>");

function tt_plugin_callback_mu($match)
{
//      $match[1] = str_replace("&w=425", "", $match[1]);
	$output = TT_TARGET;
	$output = str_replace("###tt_mu###", $match[1], $output);
            $output = str_replace('"','',$output);
	return ($output);
}

function tt_plugin_mu($content)
{
	return (preg_replace_callback(TT_REGEXP, 'tt_plugin_callback_mu', $content));
}


// vsocial Code

define("VSOCIAL_WIDTH", 400);
define("VSOCIAL_HEIGHT", 410);
define("VSOCIAL_REGEXP", "/\[vsocial ([[:print:]]+)\]/");
define("VSOCIAL_TARGET", "<object width=\"".VSOCIAL_WIDTH."\" height=\"".VSOCIAL_HEIGHT."\"><embed src=\"http://static.vsocial.com/flash/ups.swf?d=###URL###&a=0\" type=\"application/x-shockwave-flash\" width=\"".VSOCIAL_WIDTH."\" height=\"".VSOCIAL_HEIGHT."\"></embed></object>");

function vsocial_plugin_callback($match) {
	$output = VSOCIAL_TARGET;
	$output = str_replace("###URL###", $match[1], $output);
	return ($output);
}

function vsocial_plugin($content) {
	return preg_replace_callback(VSOCIAL_REGEXP, 'vsocial_plugin_callback', $content);
}



// last.fm Code

define("LASTFM_WIDTH", 340);
define("LASTFM_HEIGHT", 289);
define("LASTFM_REGEXP", "/\[lastfm ([[:print:]]+)\]/");
define("LASTFM_TARGET", "<object width=\"".LASTFM_WIDTH."\" height=\"".LASTFM_HEIGHT."\"><embed src=\"http://cdn.last.fm/videoplayer/33/VideoPlayer.swf?=###URL###\" type=\"application/x-shockwave-flash\" width=\"".LASTFM_WIDTH."\" height=\"".LASTFM_HEIGHT."\" wmode=\"transparent\"></embed></object>");

function lastfm_plugin_callback($match) {
	$output = LASTFM_TARGET;
	$output = str_replace("###URL###", $match[1], $output);
	return ($output);
}

function lastfm_plugin($content) {
	return preg_replace_callback(LASTFM_REGEXP, 'lastfm_plugin_callback', $content);
}


// Sumo.tv Code

/*
define("SUMOTV_WIDTH", 400);
define("SUMOTV_HEIGHT", 329);
define("SUMOTV_REGEXP", "/\[sumotv ([[:print:]]+)\]/");
define("SUMOTV_TARGET", "<object width=\"".SUMOTV_WIDTH."\" height=\"".SUMOTV_HEIGHT."\"><embed src=\"http://www.sumo.tv/embed.swf?file=###URL###.flv&autostart=false\" type=\"application/x-shockwave-flash\" width=\"".SUMOTV_WIDTH."\" height=\"".SUMOTV_HEIGHT."\" wmode=\"transparent\"></embed></object>");
*/
function sumotv_plugin_callback($match) {

	global $videoObject;
	$output = $videoObject->target_video_string;
	$output = str_replace("###URL###", $match[1], $output);
	return ($output);
}

function sumotv_plugin($content, $width=null, $height=null) {

if (isset($width) && $width > 0 ) $SUMOTV_WIDTH= $width;
else $SUMOTV_WIDTH= 400;

if (isset($height) && $height > 0 ) $SUMOTV_HEIGHT= $height;
else $SUMOTV_HEIGHT= 329;

$SUMOTV_REGEXP= "/\[sumotv ([[:print:]]+)\]/";
$SUMOTV_TARGET = "<object width=\"".$SUMOTV_WIDTH."\" height=\"".$SUMOTV_HEIGHT."\"><embed src=\"http://www.sumo.tv/embed.swf?file=###URL###.flv&autostart=false\" type=\"application/x-shockwave-flash\" width=\"".$SUMOTV_WIDTH."\" height=\"".$SUMOTV_HEIGHT."\" wmode=\"transparent\"></embed></object>";

        global $videoObject;
        $videoObject->setVideoObjectString($SUMOTV_TARGET);
	return preg_replace_callback($SUMOTV_REGEXP, 'sumotv_plugin_callback', $content);
}



// 123video.nl code

define("VIDEONL_WIDTH", 420);
define("VIDEONL_HEIGHT", 339);
define("VIDEONL_REGEXP", "/\[123videonl ([[:print:]]+)\]/");
define("VIDEONL_TARGET", "<object width=\"".VIDEONL_WIDTH."\" height=\"".VIDEONL_HEIGHT."\"><embed src=\"http://www.123video.nl/123video_share.swf?mediaSrc=###URL###\" type=\"application/x-shockwave-flash\" quality=\"high\" width=\"".VIDEONL_WIDTH."\" height=\"".VIDEONL_HEIGHT."\"></embed></object>");

function videonl_plugin_callback($match)
{
        $output = VIDEONL_TARGET;
        $output = str_replace("###URL###", $match[1], $output);
        return ($output);
}

function videonl_plugin($content)
{
        return (preg_replace_callback(VIDEONL_REGEXP, 'videonl_plugin_callback', $content));
}


// Brightcove code

define("BRIGHTCOVE_WIDTH", 486);
define("BRIGHTCOVE_HEIGHT", 412);
define("BRIGHTCOVE_REGEXP", "/\[brightcove ([[:print:]]+)\]/");
define("BRIGHTCOVE_TARGET", "<object width=\"".BRIGHTCOVE_WIDTH."\" height=\"".BRIGHTCOVE_HEIGHT."\"><embed src=\"http://www.brightcove.com/playerswf?allowFullScreen=true&initVideoId=###URL###&servicesURL=http://www.brightcove.com&viewerSecureGatewayURL=https://www.brightcove.com&cdnURL=http://admin.brightcove.com&autoStart=false\" type=\"application/x-shockwave-flash\" quality=\"high\" width=\"".BRIGHTCOVE_WIDTH."\" height=\"".BRIGHTCOVE_HEIGHT."\"></embed></object>");

function brightcove_plugin_callback($match)
{
        $output = BRIGHTCOVE_TARGET;
        $output = str_replace("###URL###", $match[1], $output);
        return ($output);
}

function brightcove_plugin($content)
{
        return (preg_replace_callback(BRIGHTCOVE_REGEXP, 'brightcove_plugin_callback', $content));
}

 

// Aniboom code

define("ANIBOOM_WIDTH", 448);
define("ANIBOOM_HEIGHT", 372);
define("ANIBOOM_REGEXP", "/\[aniboom ([[:print:]]+)\]/");
define("ANIBOOM_TARGET", "<object width=\"".ANIBOOM_WIDTH."\" height=\"".ANIBOOM_HEIGHT."\"><param name=\"movie\" value=\"window\"></param><embed src=\"http://api.aniboom.com/embedded.swf?videoar=###URL###\" type=\"application/x-shockwave-flash\" quality=\"high\" width=\"".ANIBOOM_WIDTH."\" height=\"".ANIBOOM_HEIGHT."\"></embed></object>");

function aniboom_plugin_callback($match)
{
        $output = ANIBOOM_TARGET;
        $output = str_replace("###URL###", $match[1], $output);
        return ($output);
}

function aniboom_plugin($content)
{
        return (preg_replace_callback(ANIBOOM_REGEXP, 'aniboom_plugin_callback', $content));
}

 

// Cellfish.com code

define("CELLFISH_WIDTH", 420);
define("CELLFISH_HEIGHT", 315);
define("CELLFISH_REGEXP", "/\[cellfish ([[:print:]]+)\]/");
define("CELLFISH_TARGET", "<object width=\"".CELLFISH_WIDTH."\" height=\"".CELLFISH_HEIGHT."\"><param name=\"vmode\" value=\"window\"></param><embed src=\"http://cellfish.com/static/swf/player8.swf?Id=###URL###\" type=\"application/x-shockwave-flash\" wmode=\"window\" width=\"".CELLFISH_WIDTH."\" height=\"".CELLFISH_HEIGHT."\"></embed></object>");

function cellfish_plugin_callback($match)
{
        $output = CELLFISH_TARGET;
        $output = str_replace("###URL###", $match[1], $output);
        return ($output);
}

function cellfish_plugin($content)
{
        return (preg_replace_callback(CELLFISH_REGEXP, 'cellfish_plugin_callback', $content));
}

 

// Stage6 code

define("STAGE6_WIDTH", 640);
define("STAGE6_HEIGHT", 480);
define("STAGE6_REGEXP", "/\[stage6 ([[:print:]]+)\]/");
define("STAGE6_TARGET", "<object codebase=\"http://download.divx.com/webplayer/stage6/windows/AutoDLDivXWebPlayerInstaller.cab\"
 width=\"".STAGE6_WIDTH."\" height=\"".STAGE6_HEIGHT."\" classid=\"clsid:67DABFBF-D0AB-41fa-9C46-CC0F21721616\">
<param name=\"autoplay\" value=\"false\"></param>
<param name=\"src\" value=\"http://video.stage6.com/###URL###/.divx\"></param>
<param name=\"custommode\" value=\"Stage6\" /><param name=\"showpostplaybackad\" value=\"false\" />
<embed type=\"video/divx\" src=\"http://video.stage6.com/###URL###/.divx\" showpostplaybackad=\"false\" custommode=\"Stage6\" 
autoplay=\"false\" width=\"".STAGE6_WIDTH."\" height=\"".STAGE6_HEIGHT."\"></embed></object>");

function stage6_plugin_callback($match)
{
        $output = STAGE6_TARGET;
        $output = str_replace("###URL###", $match[1], $output);
        return ($output);
}

function stage6_plugin($content)
{
        return (preg_replace_callback(STAGE6_REGEXP, 'stage6_plugin_callback', $content));
}

 

// Tu.tv code

define("TUTV_WIDTH", 425);
define("TUTV_HEIGHT", 350);
define("TUTV_REGEXP", "/\[tutv ([[:print:]]+)\]/");
define("TUTV_TARGET", "<object width=\"".TUTV_WIDTH."\" height=\"".TUTV_HEIGHT."\"><param name=\"vmode\" value=\"transparent\"></param><embed src=\"http://www.tu.tv/tutvweb.swf?kpt=###URL###\" type=\"application/x-shockwave-flash\" wmode=\"transparent\" width=\"".TUTV_WIDTH."\" height=\"".TUTV_HEIGHT."\"></embed></object>");

function tutv_plugin_callback($match)
{
        $output = TUTV_TARGET;
        $output = str_replace("###URL###", $match[1], $output);
        return ($output);
}

function tutv_plugin($content)
{
        return (preg_replace_callback(TUTV_REGEXP, 'tutv_plugin_callback', $content));
}

 

// Yahoo! Video code

define("YAHOO_WIDTH", 425);
define("YAHOO_HEIGHT", 350);
define("YAHOO_REGEXP", "/\[yahoo ([[:print:]]+)\]/");
define("YAHOO_TARGET", "<object width=\"".YAHOO_WIDTH."\" height=\"".YAHOO_HEIGHT."\"><param name=\"vmode\" value=\"transparent\"></param><embed src=\"http://us.i1.yimg.com/cosmos.bcst.yahoo.com/player/media/swf/FLVVideoSolo.swf?id=###URL###\" type=\"application/x-shockwave-flash\" wmode=\"transparent\" width=\"".YAHOO_WIDTH."\" height=\"".YAHOO_HEIGHT."\"></embed></object>");

function yahoo_plugin_callback($match)
{
	echo("match  ".$match[1]."]<BR>");
        $output = YAHOO_TARGET;
        $output = str_replace("###URL###", $match[1], $output);
        return ($output);
}

function yahoo_plugin($content)
{
	echo("Yahoo Content ".$content."]<BR>");
	echo("Yahoo REGEXP ".YAHOO_REGEXP."]<BR>");
        return (preg_replace_callback(YAHOO_REGEXP, 'yahoo_plugin_callback', $content));
}


// MyspaceTV code

define("MYSPACETV_WIDTH", 385);
define("MYSPACETV_HEIGHT", 346);
define("MYSPACETV_REGEXP", "/\[myspacetv ([[:print:]]+)\]/");
define("MYSPACETV_TARGET", "<object width=\"".MYSPACETV_WIDTH."\" height=\"".MYSPACETV_HEIGHT."\"><param name=\"vmode\" value=\"transparent\"></param><embed src=\"http://lads.myspace.com/videos/vplayer.swf?m=###URL###&type=video\" type=\"application/x-shockwave-flash\" wmode=\"transparent\" width=\"".MYSPACETV_WIDTH."\" height=\"".MYSPACETV_HEIGHT."\"></embed></object>");

function myspacetv_plugin_callback($match)
{
        $output = MYSPACETV_TARGET;
        $output = str_replace("###URL###", $match[1], $output);
        return ($output);
}

function myspacetv_plugin($content)
{
        return (preg_replace_callback(MYSPACETV_REGEXP, 'myspacetv_plugin_callback', $content));
}

        

// Veoh code
/*
define("VEOH_WIDTH", 385);
define("VEOH_HEIGHT", 312);
define("VEOH_REGEXP", "/\[veoh ([[:print:]]+)\]/");
define("VEOH_TARGET", "<object width=\"".VEOH_WIDTH."\" height=\"".VEOH_HEIGHT."\"><param name=\"vmode\" value=\"transparent\"></param><embed src=\"http://www.veoh.com/videodetails.swf?permalinkId=###URL###&id=anonymous&player=videodetailsembedded&videoAutoPlay=0\" type=\"application/x-shockwave-flash\" wmode=\"transparent\" width=\"".VEOH_WIDTH."\" height=\"".VEOH_HEIGHT."\"></embed></object>");
*/
function veoh_plugin_callback($match)
{
	global $videoObject;
	$output = $videoObject->target_video_string;
        $output = str_replace("###URL###", $match[1], $output);
        return ($output);
}

function veoh_plugin($content, $width=null, $height=null) {

if (isset($width) && $width > 0 ) $VEOH_WIDTH= $width;
else $VEOH_WIDTH= 385;

if (isset($height) && $height > 0 ) $VEOH_HEIGHT= $height;
else $VEOH_HEIGHT= 312;

$VEOH_REGEXP = "/\[veoh ([[:print:]]+)\]/";
$VEOH_TARGET = "<object width=\"".$VEOH_WIDTH."\" height=\"".$VEOH_HEIGHT."\"><param name=\"vmode\" value=\"transparent\"></param><embed src=\"http://www.veoh.com/videodetails.swf?permalinkId=###URL###&id=anonymous&player=videodetailsembedded&videoAutoPlay=0\" type=\"application/x-shockwave-flash\" wmode=\"transparent\" width=\"".$VEOH_WIDTH."\" height=\"".$VEOH_HEIGHT."\"></embed></object>";

        global $videoObject;
        $videoObject->setVideoObjectString($VEOH_TARGET);
        return (preg_replace_callback($VEOH_REGEXP, 'veoh_plugin_callback', $content));
}


// Wandeo Video Code

define("WANDEO_WIDTH", 352);
define("WANDEO_HEIGHT", 308);
define("WANDEO_REGEXP", "/\[wandeo ([[:print:]]+)\]/");
define("WANDEO_TARGET", "<object width=\"".WANDEO_WIDTH."\" height=\"".WANDEO_HEIGHT."\"><param name=\"vmode\" value=\"transparent\"></param><embed src=\"http://www.wandeo.com/v/###URL###\" type=\"application/x-shockwave-flash\" wmode=\"transparent\" width=\"".WANDEO_WIDTH."\" height=\"".WANDEO_HEIGHT."\"></embed></object>");

function wandeo_plugin_callback($match) {
       $output = WANDEO_TARGET;
       $output = str_replace("###URL###", $match[1], $output);
       return ($output);
}

function wandeo_plugin($content) {
      return preg_replace_callback(WANDEO_REGEXP, 'wandeo_plugin_callback', $content);
}



// glumbert code

define("GLUMBERT_WIDTH", 448);
define("GLUMBERT_HEIGHT", 336);
define("GLUMBERT_REGEXP", "/\[glumbert ([[:print:]]+)\]/");
define("GLUMBERT_TARGET", "<object width=\"".GLUMBERT_WIDTH."\" height=\"".GLUMBERT_HEIGHT."\"><param name=\"vmode\" value=\"transparent\"></param><embed src=\"http://www.glumbert.com/embed/###URL###\" type=\"application/x-shockwave-flash\" wmode=\"transparent\" width=\"".GLUMBERT_WIDTH."\" height=\"".GLUMBERT_HEIGHT."\"></embed></object>");

function glumbert_plugin_callback($match)
{
        $output = GLUMBERT_TARGET;
        $output = str_replace("###URL###", $match[1], $output);
        return ($output);
}

function glumbert_plugin($content)
{
        return (preg_replace_callback(GLUMBERT_REGEXP,
'glumbert_plugin_callback', $content));
}


// IFILM Code

define("IFILM_WIDTH", 448);
define("IFILM_HEIGHT", 365);
define("IFILM_REGEXP", "/\[ifilm ([[:print:]]+)\]/");
define("IFILM_TARGET", "<object type=\"application/x-shockwave-flash\" data=\"http://www.ifilm.com/efp?flvbaseclip=###URL###\" width=\"".IFILM_WIDTH."\" height=\"".IFILM_HEIGHT."\" wmode=\"transparent\"><param name=\"movie\" value=\"http://www.ifilm.com/efp?flvbaseclip=###URL###\" /></object>");

function ifilm_plugin_callback($match) {
	$output = IFILM_TARGET;
	$output = str_replace("###URL###", $match[1], $output);
	return ($output);
}

function ifilm_plugin($content) {
	return preg_replace_callback(IFILM_REGEXP, 'ifilm_plugin_callback', $content);
}


//LiveLeak Code

define("LIVELEAK_WIDTH", 457);
define("LIVELEAK_HEIGHT", 361);
define("LIVELEAK_REGEXP", "/\[liveleak ([[:print:]]+)\]/");
define("LIVELEAK_TARGET", "<object type=\"application/x-shockwave-flash\" width=\"".LIVELEAK_WIDTH."\" height=\"".LIVELEAK_HEIGHT."\" wmode=\"transparent\" data=\"http://www.liveleak.com/player.swf?autostart=false&token=###URL###\"><param name=\"movie\" value=\"http://www.liveleak.com/player.swf?autostart=false&token=###URL###\"><param name=\"wmode\" value=\"transparent\"><param name=\"quality\" value=\"high\"></object>");

function liveleak_plugin_callback($match)
{
   $output = LIVELEAK_TARGET;
   $output = str_replace("###URL###", $match[1], $output);
   return ($output);
}

function liveleak_plugin($content)
{
   return (preg_replace_callback(LIVELEAK_REGEXP, 'liveleak_plugin_callback', $content));
}


// Grouper Code

define("GROUPER_WIDTH", 400);
define("GROUPER_HEIGHT", 325);
define("GROUPER_REGEXP", "/\[grouper ([[:print:]]+)\]/");
define("GROUPER_TARGET", "<object type=\"application/x-shockwave-flash\" data=\"http://grouper.com/mtg/mtgPlayer.swf?v=1.7ap=0&rf=-1&vfver=8&extid=-1&extsite=-1&id=###URL###\" width=\"".GROUPER_WIDTH."\" height=\"".GROUPER_HEIGHT."\" wmode=\"transparent\"><param name=\"movie\" value=\"http://grouper.com/mtg/mtgPlayer.swf?v=1.7ap=0&rf=-1&vfver=8&extid=-1&extsite=-1&id=###URL###\" /></object>");

function grouper_plugin_callback($match)
{
	$output = GROUPER_TARGET;
	$output = str_replace("###URL###", $match[1], $output);
	return ($output);
}

function grouper_plugin($content)
{
	return (preg_replace_callback(GROUPER_REGEXP, 'grouper_plugin_callback', $content));
}



// UnCut Code

define("UNCUT_WIDTH", 425);
define("UNCUT_HEIGHT", 350);
define("UNCUT_REGEXP", "/\[uncut ([[:print:]]+)\]/");
define("UNCUT_TARGET", "<object type=\"application/x-shockwave-flash\" data=\"http://uncutvideo.aol.com/en-US/uc_videoplayer.swf?aID=1###URL###\" width=\"".UNCUT_WIDTH."\" height=\"".UNCUT_HEIGHT."\" wmode=\"transparent\"><param name=\"movie\" value=\"http://uncutvideo.aol.com/en-US/uc_videoplayer.swf?aID=1###URL###\" /></object>");

function uncut_plugin_callback($match)
{
	$output = UNCUT_TARGET;
	$output = str_replace("###URL###", $match[1], $output);
	return ($output);
}

function uncut_plugin($content)
{
	return (preg_replace_callback(UNCUT_REGEXP, 'uncut_plugin_callback', $content));
}



// Revver Code

define("REVVER_WIDTH", 480);
define("REVVER_HEIGHT", 392);
define("REVVER_REGEXP", "/\[revver ([[:print:]]+)\]/");
define("REVVER_TARGET", "<object type=\"application/x-shockwave-flash\" data=\"http://flash.revver.com/player/1.0/player.swf?mediaId=###URL###\" width=\"".REVVER_WIDTH."\" height=\"".REVVER_HEIGHT."\" wmode=\"transparent\"><param name=\"movie\" value=\"http://flash.revver.com/player/1.0/player.swf?mediaId=###URL###\" /></object>");

function revver_plugin_callback($match)
{
	$output = REVVER_TARGET;
	$output = str_replace("###URL###", $match[1], $output);
	return ($output);
}

function revver_plugin($content)
{
	return (preg_replace_callback(REVVER_REGEXP, 'revver_plugin_callback', $content));
}


// blip.tv Code
define("BLIPTV_WIDTH", 400);
define("BLIPTV_HEIGHT", 294);
define("BLIPTV_REGEXP", "/\[bliptv ([[:print:]]+)\]/");
// define("BLIPTV_TARGET", "<object type=\"application/x-shockwave-flash\" data=\"http://blip.tv/scripts/flash/blipplayer.swf?autoStart=false&file=http://blip.tv/file/get/###URL###.flv%3Fsource%3D3\" width=\"".BLIPTV_WIDTH."\" height=\"".BLIPTV_HEIGHT."\" wmode=\"transparent\"><param name=\"movie\" value=\"http://blip.tv/scripts/flash/blipplayer.swf?autoStart=false&file=http://blip.tv/file/get/###URL###.flv%3Fsource%3D3\" /></object>");
define("BLIPTV_TARGET", "<embed src=\"http://blip.tv/play/###URL###\" type=\"application/x-shockwave-flash\" width=\"".BLIPTV_WIDTH."\" height=\"".BLIPTV_HEIGHT."\" allowscriptaccess=\"always\" allowfullscreen=\"true\"></embed>");

function bliptv_plugin_callback($match) {
	$output = BLIPTV_TARGET;
	$output = str_replace("###URL###", $match[1], $output);
	return ($output);
}

function bliptv_plugin($content) {
	return preg_replace_callback(BLIPTV_REGEXP, 'bliptv_plugin_callback', $content);
}


// Videotube Code

define("VIDEOTUBE_WIDTH", 480);
define("VIDEOTUBE_HEIGHT", 400);
define("VIDEOTUBE_REGEXP", "/\[videotube ([[:print:]]+)\]/");
define("VIDEOTUBE_TARGET", "<object type=\"application/x-shockwave-flash\" data=\"http://www.videotube.de/ci/flash/videotube_player_4.swf?videoId=###URL###&svsf=0&lang=german&host=www.videotube.de\" width=\"".VIDEOTUBE_WIDTH."\" height=\"".VIDEOTUBE_HEIGHT."\" wmode=\"transparent\"><param name=\"movie\" value=\"http://www.videotube.de/ci/flash/videotube_player_4.swf?videoId=###URL###&svsf=0&lang=german&host=www.videotube.de\" /></object>");

function videotube_plugin_callback($match) {
	$output = VIDEOTUBE_TARGET;
	$output = str_replace("###URL###", $match[1], $output);
	return ($output);
}

function videotube_plugin($content) {
	return preg_replace_callback(VIDEOTUBE_REGEXP, 'videotube_plugin_callback', $content);
}



// Vimeo Code
/*
define("VIMEO_WIDTH", 400);
define("VIMEO_HEIGHT", 294);
define("VIMEO_REGEXP", "/\[vimeo ([[:print:]]+)\]/");
define("VIMEO_TARGET", "<object type=\"application/x-shockwave-flash\" data=\"http://www.vimeo.com/moogaloop.swf?clip_id=###URL###\" width=\"".VIMEO_WIDTH."\" height=\"".VIMEO_HEIGHT."\" wmode=\"transparent\"><param name=\"movie\" value=\"http://www.vimeo.com/moogaloop.swf?clip_id=###URL###\" /></object>");
*/
function vimeo_plugin_callback($match) {

	global $videoObject;
	$output = $videoObject->target_video_string;
	$output = str_replace("###URL###", $match[1], $output);
	return ($output);
}

function vimeo_plugin($content, $width=null, $height=null) {

if (isset($width) && $width > 0 ) $VIMEO_WIDTH= $width;
else $VIMEO_WIDTH= 450;

if (isset($height) && $height > 0 ) $VIMEO_HEIGHT= $height;
else $VIMEO_HEIGHT= 400;

$VIMEO_REGEXP = "/\[vimeo ([[:print:]]+)\]/";
$VIMEO_TARGET = "<object type=\"application/x-shockwave-flash\" data=\"http://www.vimeo.com/moogaloop.swf?clip_id=###URL###&fullscreen=1\" width=\"".$VIMEO_WIDTH."\" height=\"".$VIMEO_HEIGHT."\" wmode=\"transparent\"><param name=\"allowfullscreen\" value=\"true\" /><param name=\"movie\" value=\"http://www.vimeo.com/moogaloop.swf?clip_id=###URL###&fullscreen=1\" /></object>";

        global $videoObject;
        $videoObject->setVideoObjectString($VIMEO_TARGET);
	return preg_replace_callback($VIMEO_REGEXP, 'vimeo_plugin_callback', $content);
}



// Metacafe Code
define("METACAFE_WIDTH", 400);
define("METACAFE_HEIGHT", 345);
define("METACAFE_REGEXP", "/\[metacafe ([[:print:]]+)\]/");
define("METACAFE_TARGET", "<object type=\"application/x-shockwave-flash\" data=\"http://www.metacafe.com/fplayer/###URL###/.swf\" width=\"".METACAFE_WIDTH."\" height=\"".METACAFE_HEIGHT."\" wmode=\"transparent\"><param name=\"movie\" value=\"http://www.metacafe.com/fplayer/###URL###/.swf\" /></object>");

function metacafe_plugin_callback($match) {
	$output = METACAFE_TARGET;
	$output = str_replace("###URL###", $match[1], $output);
	return ($output);
}

function metacafe_plugin($content) {
	return preg_replace_callback(METACAFE_REGEXP, 'metacafe_plugin_callback', $content);
}



// Break.com Codes

define("BREAK_WIDTH", 425);
define("BREAK_HEIGHT", 350);
define("BREAK_REGEXP", "/\[break ([[:print:]]+)\]/");
define("BREAK_TARGET", "<object type=\"application/x-shockwave-flash\" data=\"http://embed.break.com/###URL###\" width=\"".BREAK_WIDTH."\" height=\"".BREAK_HEIGHT."\" wmode=\"transparent\"><param name=\"movie\" value=\"http://embed.break.com/###URL###\" /></object>");

function break_plugin_callback($match) {
	$output = BREAK_TARGET;
	$output = str_replace("###URL###", $match[1], $output);
	return ($output);
}

function break_plugin($content) {
	return preg_replace_callback(BREAK_REGEXP, 'break_plugin_callback', $content);
}



// MyVideo Code

define("MYVIDEO_WIDTH", 425);
define("MYVIDEO_HEIGHT", 367);
define("MYVIDEO_REGEXP", "/\[myvideo ([[:print:]]+)\]/");
define("MYVIDEO_TARGET", "<object type=\"application/x-shockwave-flash\" data=\"http://www.myvideo.de/movie/###URL###\" width=\"".MYVIDEO_WIDTH."\" height=\"".MYVIDEO_HEIGHT."\" wmode=\"transparent\"><param name=\"movie\" value=\"http://www.myvideo.de/movie/###URL###\" /></object>");

function myvideo_plugin_callback($match) {
	$output = MYVIDEO_TARGET;
	$output = str_replace("###URL###", $match[1], $output);
	return ($output);
}

function myvideo_plugin($content) {
	return preg_replace_callback(MYVIDEO_REGEXP, 'myvideo_plugin_callback', $content);
}


// Dailymotion Code

define("DAILYMOTION_WIDTH", 425);
define("DAILYMOTION_HEIGHT", 335);
define("DAILYMOTION_REGEXP", "/\[dailymotion ([[:print:]]+)\]/");
define("DAILYMOTION_TARGET", "<object type=\"application/x-shockwave-flash\" data=\"http://www.dailymotion.com/swf/###URL###\" width=\"".DAILYMOTION_WIDTH."\" height=\"".DAILYMOTION_HEIGHT."\" wmode=\"transparent\"><param name=\"movie\" value=\"http://www.dailymotion.com/swf/###URL###\" /></object>");

function dailymotion_plugin_callback($match) {
	$output = DAILYMOTION_TARGET;
	$output = str_replace("###URL###", $match[1], $output);
	return ($output);
}

function dailymotion_plugin($content) {
	return preg_replace_callback(DAILYMOTION_REGEXP, 'dailymotion_plugin_callback', $content);
}



// Sevenload Code

define("SEVENLOAD_WIDTH", 380);
define("SEVENLOAD_HEIGHT", 313);
define("SEVENLOAD_REGEXP", "/\[sevenload ([[:print:]]+)\]/");
define("SEVENLOAD_TARGET", "<object type=\"application/x-shockwave-flash\" data=\"http://sevenload.de/pl/###URL###/380x313/swf\" width=\"".SEVENLOAD_WIDTH."\" height=\"".SEVENLOAD_HEIGHT."\" wmode=\"transparent\"><param name=\"movie\" value=\"http://sevenload.de/pl/###URL###/380x313/swf\" /></object>");

function sevenload_plugin_callback($match) {
	$output = SEVENLOAD_TARGET;
	$output = str_replace("###URL###", $match[1], $output);
	return ($output);
}

function sevenload_plugin($content) {
	return preg_replace_callback(SEVENLOAD_REGEXP, 'sevenload_plugin_callback', $content);
}


// Clipfish Code

define("CLIPFISH_WIDTH", 464);
define("CLIPFISH_HEIGHT", 380);
define("CLIPFISH_REGEXP", "/\[clipfish ([[:print:]]+)\]/");
define("CLIPFISH_TARGET", "<object type=\"application/x-shockwave-flash\" data=\"http://www.clipfish.de/videoplayer.swf?as=0&videoid=###URL###&r=1\" width=\"".CLIPFISH_WIDTH."\" height=\"".CLIPFISH_HEIGHT."\" wmode=\"transparent\"><param name=\"movie\" value=\"http://www.clipfish.de/videoplayer.swf?as=0&videoid=###URL###&r=1\" /></object>");

function clipfish_plugin_callback($match)
{
	$output = CLIPFISH_TARGET;
	$output = str_replace("###URL###", $match[1], $output);
	return ($output);
}

function clipfish_plugin($content)
{
	return (preg_replace_callback(CLIPFISH_REGEXP, 'clipfish_plugin_callback', $content));
}



// GoogleVideo Code
/*
define("GOOGLE_WIDTH", 425);
define("GOOGLE_HEIGHT", 350);
define("GOOGLE_REGEXP", "/\[google ([[:print:]]+)\]/");
define("GOOGLE_TARGET", "<object type=\"application/x-shockwave-flash\" data=\"http://video.google.com/googleplayer.swf?docId=###URL###\" width=\"".GOOGLE_WIDTH."\" height=\"".GOOGLE_HEIGHT."\" wmode=\"transparent\"><param name=\"movie\" value=\"http://video.google.com/googleplayer.swf?docId=###URL###\" /></object>");
*/
function google_plugin_callback($match) {

	global $videoObject;
	$output = $videoObject->target_video_string;
	$output = str_replace("###URL###", $match[1], $output);
	return ($output);
}

function google_plugin($content, $width=null, $height=null) {

if (isset($width) && $width > 0 ) $GOOGLE_WIDTH= $width;
else $GOOGLE_WIDTH= 425;

if (isset($height) && $height > 0 ) $GOOGLE_HEIGHT= $height;
else $GOOGLE_HEIGHT= 350;

$GOOGLE_REGEXP = "/\[google ([[:print:]]+)\]/";
$GOOGLE_TARGET = "<object type=\"application/x-shockwave-flash\" data=\"http://video.google.com/googleplayer.swf?docId=###URL###\" width=\"".$GOOGLE_WIDTH."\" height=\"".$GOOGLE_HEIGHT."\" wmode=\"transparent\"><param name=\"movie\" value=\"http://video.google.com/googleplayer.swf?docId=###URL###\" /></object>";

        global $videoObject;
        $videoObject->setVideoObjectString($GOOGLE_TARGET);
	return preg_replace_callback($GOOGLE_REGEXP, 'google_plugin_callback', $content);
}



// Youtube Code
/*
define("YOUTUBE_WIDTH", 425); // default width
define("YOUTUBE_HEIGHT", 355); // default height
define("YOUTUBE_REGEXP", "/\[youtube ([[:print:]]+)\]/");
define("YOUTUBE_TARGET", "<object type=\"application/x-shockwave-flash\" data=\"http://www.youtube.com/v/###URL###\" width=\"###WIDTH###\" height=\"###HEIGHT###\" wmode=\"transparent\"><param name=\"movie\" value=\"http://www.youtube.com/v/###URL###\" /></object>");
*/
function youtube_plugin_callback($match)
{
	global $videoObject; 
	$output = $videoObject->target_video_string;
	$T_YOUTUBE_WIDTH  = $videoObject->width;
	$T_YOUTUBE_HEIGHT = $videoObject->height;

	$tag_parts = explode(" ", rtrim($match[0], "]"));
	$output = str_replace("###URL###", $tag_parts[1], $output);
	if (count($tag_parts) > 2) {
		if ($tag_parts[2] == 0) {
			$output = str_replace("###WIDTH###", $T_YOUTUBE_WIDTH, $output);
		} else {
			$output = str_replace("###WIDTH###", $tag_parts[2], $output);
		}
		if ($tag_parts[3] == 0) {
			$output = str_replace("###HEIGHT###", $T_YOUTUBE_HEIGHT, $output);
		} else {
			$output = str_replace("###HEIGHT###", $tag_parts[3], $output);
		}
	} else {
		$output = str_replace("###WIDTH###", $T_YOUTUBE_WIDTH, $output);
		$output = str_replace("###HEIGHT###", $T_YOUTUBE_HEIGHT, $output);	
	}
	return ($output);
}
function youtube_plugin($content, $width=null, $height=null) {

if (isset($width) && $width > 0 ) $YOUTUBE_WIDTH= $width;
else $YOUTUBE_WIDTH= 425;

if (isset($height) && $height > 0 ) $YOUTUBE_HEIGHT= $height;
else $YOUTUBE_HEIGHT= 355;

$YOUTUBE_REGEXP = "/\[youtube ([[:print:]]+)\]/";
$YOUTUBE_TARGET = "<object type=\"application/x-shockwave-flash\" data=\"http://www.youtube.com/v/###URL###\" width=\"###WIDTH###\" height=\"###HEIGHT###\" wmode=\"transparent\"><param name=\"movie\" value=\"http://www.youtube.com/v/###URL###\" /></object>";

        global $videoObject;
        $videoObject->setVideoObjectString($YOUTUBE_TARGET);
        $videoObject->setVideoObjectSize($YOUTUBE_WIDTH, $YOUTUBE_HEIGHT);
	return (preg_replace_callback($YOUTUBE_REGEXP, 'youtube_plugin_callback', $content));
}

// DO NOT CHANGE THE INDEXES FOR THE LOOKUPS IN THE SWITCH STATEMENT BECAUSE THESE INDEXES
// ARE STORED AGAINST THE EMBEDDED VIDEO IN THE DATABASE. CHANGING THESE WILL BREAK THIS.
// Note: use_small_size option not implemented yet.

function getEmbeddedVideoHTML($videoIdTypeIndex, $videoId, $t_width=null, $t_height=null)
{
    switch($videoIdTypeIndex) {
        case 0:
            return "";
	    break;
        case 1:
            return(youtube_plugin("[youtube ".$videoId."]", $t_width, $t_height));
	    break;

        case 2:
            return(google_plugin("[google ".$videoId."]", $t_width, $t_height));
	    break;

        case 3:
            return(clipfish_plugin("[clipfish ".$videoId."]"));
	    break;

        case 4:
            return(stage6_plugin("[stage6 ".$videoId."]"));
	    break;

        case 5:
            return(sevenload_plugin("[sevenload ".$videoId."]"));
	    break;

        case 6:
            return(dailymotion_plugin("[dailymotion ".$videoId."]"));
	    break;

        case 7:
            return(myvideo_plugin("[myvideo ".$videoId."]"));
	    break;

        case 8:
            return(break_plugin("[break ".$videoId."]"));
	    break;

        case 9:
            return(metacafe_plugin("[metacafe ".$videoId."]"));
	    break;

        case 10:
            return(vimeo_plugin("[vimeo ".$videoId."]", $t_width, $t_height));
	    break;

        case 11:
            return(videotube_plugin("[videotube ".$videoId."]"));
	    break;

        case 12:
            return(bliptv_plugin("[bliptv ".$videoId."]"));
	    break;

        case 13:
            return(revver_plugin("[revver ".$videoId."]"));
	    break;

        case 14:
            return(uncut_plugin("[uncut ".$videoId."]"));
	    break;

        case 15:
            return(liveleak_plugin("[liveleak ".$videoId."]"));
	    break;

        case 16:
            return(yahoo_plugin("[yahoo ".$videoId."]"));
	    break;

        case 17:
            return(myspacetv_plugin("[myspacetv ".$videoId."]"));
	    break;

        case 18:
            return(ifilm_plugin("[ifilm ".$videoId."]"));
	    break;

        case 19:
            return(veoh_plugin("[veoh ".$videoId."]", $t_width, $t_height));
	    break;

        case 20:
            return(vsocial_plugin("[vsocial ".$videoId."]"));
	    break;

        case 21:
            return(lastfm_plugin("[lastfm ".$videoId."]"));
	    break;

        case 22:
            return(glumbert_plugin("[glumbert ".$videoId."]"));
	    break;

        case 23:
            return(wandeo_plugin("[wandeo ".$videoId."]"));
	    break;

        case 24:
            return(tutv_plugin("[tutv ".$videoId."]"));
	    break;

        case 25:
            return(cellfish_plugin("[cellfish ".$videoId."]"));
	    break;

        case 26:
            return(grouper_plugin("[grouper ".$videoId."]"));
	    break;

        case 27:
            return(politube_plugin("[politube ".$videoId."]", $t_width, $t_height));
	    break;

        default:
            return "";
    }
}

// ***** IMPORTANT NOTE ***** 
// This functon is used for returning the list of embedded video choices. DO NOT CHANGE the indexes because
// these correspond to the embedded video type and now get saved in the database in the attachment_file field
// in a special preformatted way. If you change that, you will break things.
// However the position in the array can be changed as long as the index + name are moved together.

function getEmbeddedVideoTypes($videoIndex, $getAll = false)
{
    // When $getAll is true, this array is used for the display of the video type selection and the position
    // in the array controls where it appears in the drop-down list.

    $allTypes = array( 1 => "Youtube Video", 2 => "Google video", 27 => "Politube", 3 => "Clipfish", 
                       5 => "Sevenload", 
                         7 => "MyVideo", 8 => "Break", 9 => "Metacafe", 10 => "Vimeo", 
			11 => "Videotube", 12 => "Blip.tv", 13 => "Revver", 14 => "UnCut", 15 => "LiveLeak",
			16 => "Yahoo! Video", 17 => "MyspaceTV", 19 => "Veoh", 
			20 => "Vsocial", 21 => "Last.fm", 22 => "Glumbert", 23 => "Wandeo", 
			24 => "Tu.tv", 25 => "Cellfish.com", 26 => "Grouper");

    if ($getAll == true)
       return($allTypes);
    else
       return($allTypes[$videoIndex]);
}

// Embedded Audio section.

function getEmbeddedAudioHTML($is_embedded, $audioFile, $audioDescription ="", $descripList = null)
{
    // Ignore the type because there is only one for the moment.
    // So build the HTML for displaying the audio player.
    // If the is_embedded flag is true, then we assume it is an external file and must build the song_url
    // a bit differently. We will also display the player in blue to indicate something is different.

    global $system_config;
    $player_width = 400;
    $player_height= 18;

    // Adding playlists requires creating an XML file and pointing to it with playlist_url=http://....
    // The main problem is when to create the file and the name to give. Story Id perhaps?
    $generate_playlist = false;
    if ($descripList != null) $generate_playlist = true;

    $song_title = $audioDescription;
    // Probably should check the last character of attachment_store is a slash because we expect it.
    $site_base_lnk= "http://" . $system_config->site_url . "/" .$system_config->attachment_store;

    if ($is_embedded == true) 
    {
        $player_title = "Indymedia Embedded External Audio";
        // In this case the audio file is the full URL except we must extract the leading 'embedaudio:01' tag.
        // $player_song_url = substr($audioFile, 14);
	if (substr($audioFile,0 ,7) != "http://") $player_song_url = "http://" .$audioFile;
	else $player_song_url = $audioFile;
    } 
    else 
    {
        $player_title = "Indymedia Embedded Audio";
        // For ordinary audio, the audio file is stored as an attachment and its path and filename relative to
        // the attachments directory is stored in the attachments table and is tbe value here.
        $player_song_url = $site_base_lnk . "/". $audioFile;
    }

    // Two players. Original Xspf Player (1) and enchanced version from boutell. (2)
    $player_type = 1;

    if ($player_type == 1) {
        //$player_color = "008040";
        $player_color = "97B697";

        if ($is_embedded == true) $player_color = "7195FF";

        $site_base_lnk .= "xspf_player";
    } else {
        // If the audio is a real file on the site, then show player in Green otherwise Blue to indicate external
        $player_height= 30;
        //$player_color = "304F20";
        $player_color = "002000";
        if ($is_embedded == true) $player_color = "000020";

        $site_base_lnk .= "xspf/Slim";
    }

    // For the moment just pass the song URL.
    if ($player_type == 1) {
        if ($generate_playlist == true) {
            $playlist_file = $site_base_lnk ."/68.xspf";
            $embed_audio_str = '<object type="application/x-shockwave-flash" width="'.$player_width.'" height="'.$player_height.'" data="'.$site_base_lnk.'/xspf_player_slim.swf?playlist_url='.$playlist_file.'&player_title='.$player_title.'&timedisplay=1"> <param name="movie" value="'.$site_base_lnk.'/xspf_player_slim.swf?&playlist_url='.$playlist_file.'&player_title='.$player_title.'&timedisplay=1"/>';
         } else {
            $embed_audio_str = '<object type="application/x-shockwave-flash" width="'.$player_width.'" height="'.$player_height.'" data="'.$site_base_lnk.'/xspf_player_slim.swf?song_url='.$player_song_url.'&song_title='.$song_title.'&player_title='.$player_title.'"> <param name="movie" value="'.$site_base_lnk.'/xspf_player_slim.swf?song_url='.$player_song_url.'&song_title='.$song_title.'&player_title='.$player_title.'"/>';
        }
        $embed_audio_str .= '<param name="bgcolor" value="#'.$player_color.'"/>';
    } else {
        // Alpha is the percent of color applied. Set to 100.
        $embed_audio_str = '<object type="application/x-shockwave-flash" width="'.$player_width.'" height="'.$player_height.'" data="'.$site_base_lnk.'/xspf_player_slim.swf?song_url='.$player_song_url.'&alpha=100&color='.$player_color.'&player_title='.$player_title.'&song_title='.$song_title.'"> <param name="movie" value="'.$site_base_lnk.'/xspf_player_slim.swf?song_url='.$player_song_url.'&alpha=100&color='.$player_color.'&player_title='.$player_title.'&song_title='.$song_title.'"/>';
    }
    $embed_audio_str .= '</object>';

    return $embed_audio_str;
}
?>
