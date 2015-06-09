<?php
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

$PAGE_CONTENTS = ob_get_contents();
ob_end_clean();
$got_page = false;

if(!isset($_SESSION['rescue_mode']))
{
   $page_obj = $system_config->adminpage_obj_id;
   $cachefile = getObjectCacheIndexFile($page_obj);
   if(file_exists($cachefile))
   {
      $OSCAILT_SCRIPT = 'index.php';
      $st_store = $system_config->site_file_store_base.'default/';
      $site_file_store = $pseudo_directory_offset.$st_store;
      $oscailt_page_setting = array();
      $oscailt_story_query = new StoryQuery();
      require_once("header.inc");
      include_once($cachefile);
      $got_page = true;
   }
}
if(!$got_page)
{
   require_once("rescueheader.inc");
   require_once("rescuepage.inc");
}
$file_size = strlen($PAGE_CONTENTS);
require_once("oscailt_destroy.inc");
require_once("footer.inc");

?>