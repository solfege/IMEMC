<?
/************************************************************************/
/* Oscailt                                                              */
/* Indepenent Media Centre Content Management System                    */
/* ==================================================================== */
/* Copyright (c)2003-2005 by Independent Media Centre Ireland           */
/* http://www.indymedia.ie                                              */
/* Development List: oscailt@lists.indymedia.org                        */
/* See contributions.txt for the list of contributors                   */
/*                                                                      */
/* This program is free software. You can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation. http://www.gnu.org/copyleft/gpl.html   */
/*                                                                      */
/* This program is distributed in the hope that it will be useful,      */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of       */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        */
/* GNU General Public License for more details.                         */
/************************************************************************/
header('Content-type: text/html; charset='.$encode_charset);
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/1999/REC-html401-19991224/loose.dtd">
<HTML>
<HEAD>
   <meta http-equiv="Content-Type" content="text/html; charset=<?=$encode_charset?>">
   <meta name="keywords" content="indymedia, independent media centre, independent news, alternative news, non corporate news, non-corporate news" />
   <TITLE>
   <?=$pageTitle?>
   </TITLE>
   <link href="http://<?=$system_config->site_url?>/newsfeed.php" rel="alternate" type="application/rss+xml" title="rss" />
   <?php
   if(isset($_REQUEST['fontsizeinc']) && $_REQUEST['fontsizeinc'] > 0)
   {
      ?>
      <link href="stylefilter.php?fontsizeinc=<?=$_REQUEST['fontsizeinc']?>&style=<?=$graphics_store?>print.css" type="text/css" rel="stylesheet" />
      <?
   }
   else
   {
      ?>
      <link href="<?=$graphics_store?>print.css" type="text/css" rel="stylesheet" />
      <?
   }
   ?>
</HEAD>
<BODY>

<small><b><FONT FACE="Arial"><?=$system_config->site_long_name?> &nbsp; &nbsp; http://<?=$system_config->site_url?>
</FONT></small></b><HR>
<FONT face=arial,helvetica>

