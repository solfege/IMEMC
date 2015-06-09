<?php
/************************************************************************/
/* Oscailt                                                              */
/* Independent Media Centre Content Management System                    */
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
   <meta name="description" content="<?= $pageDescription ?>">
   <meta name="keywords" content="<?= $pageKeywords ?>">
<meta name="sitelock-site-verification" content="3750" />

   <TITLE>
   <?= $pageTitle ?>
   </TITLE>
<link rel="image_src" href="http://imemc.org/graphics/imemc_logo.jpg">

   <?
   if(isset($_REQUEST['fontsizeinc']) && $_REQUEST['fontsizeinc'] > 0)
   {
      $clean_file = substr($site_file_store, strlen($pseudo_directory_offset), strlen($site_file_store) - strlen($pseudo_directory_offset));
      ?>
      <link href="<?=$pseudo_directory_offset?>stylefilter.php?fontsizeinc=<?=$_REQUEST['fontsizeinc']?>&style=<?=$clean_file?><?=getFilteredFile($site_file_store,$style_sheet,false)?>" type="text/css" rel="stylesheet">
      <link href="<?=$pseudo_directory_offset?>stylefilter.php?fontsizeinc=<?=$_REQUEST['fontsizeinc']?>&style=<?=$clean_file?><?=getFilteredFile($site_file_store,'common.css',false)?>" type="text/css" rel="stylesheet">
      <link href="<?=$pseudo_directory_offset?>stylefilter.php?fontsizeinc=<?=$_REQUEST['fontsizeinc']?>&style=<?=$clean_file?><?=getFilteredFile($site_file_store,'contentstyle.css',false)?>" type="text/css" rel="stylesheet">
      <?
   }
   else
   {
      ?>
      <link href="<?=$site_file_store?><?=getFilteredFile($site_file_store,$style_sheet,false)?>" type="text/css" rel="stylesheet">
      <link href="<?=$site_file_store?><?=getFilteredFile($site_file_store,'common.css',false)?>" type="text/css" rel="stylesheet">
      <link href="<?=$site_file_store?><?=getFilteredFile($site_file_store,'contentstyle.css',false)?>" type="text/css" rel="stylesheet">
   <?
   }
   ?>
   <link href="http://<?=$system_config->site_url?>/rss.xml" rel="alternate" type="application/rss+xml" title="<?=$system_config->site_short_name?> RSS Feed">
   <link href="http://<?=$system_config->site_url?>/atom.xml" rel="alternate" type="application/atom+xml" title="<?=$system_config->site_short_name?> Atom Feed">
   <link href="http://<?=$system_config->site_url?>/podcast.xml" rel="alternate" type="application/rss+xml" title="<?=$system_config->site_short_name?> Podcast">

<script>var noskim = 'true';</script>

</HEAD>
<BODY>
