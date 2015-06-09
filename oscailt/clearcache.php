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

$OSCAILT_SCRIPT = "clearcache.php";
require_once("oscailt_init.inc");
require_once("objects/statistics.inc");
require_once("objects/indyobjects/indyobject.inc");
require_once("objects/indyobjects/indyitemset.inc");

// Preparation for enabling admin labels to be translatable.

//$textLabels = getAdminLabels("clearcache.php");
$textLabels = array("title" => "Clear Cache and Cache Checks",
                    "Summary" => "The buttons below allow you to clear the various data caches used by oscailt.
      While this usally shouldn't be necessary, you may wish to clear caches if errors become cached
      or cache files get corrupted.
      <BR><BR>
      There is also an addition option to check the database cache directory exists and if it does
      not to create it. For the new database cache directory structure in Oscailt 3.3 onwards, there
      are 5 associated subdirectories that will be checked and created also if that cache mode is in use.",

	            "ClearSQLBtn" => "Clear SQL Cache &gt;&gt;",
	            "ClearRSSBtn" => "Clear RSS Cache &gt;&gt;",
	            "ClearOMLBtn" => "Clear OML Cache &gt;&gt;",
	            "ClearIMGBtn" => "Clear Image Cache &gt;&gt;",
	            "ClearHTMLBtn" => "Clear HTML Cache &gt;&gt;",
	            "RebuildCachesBtn" => "Rebuild Object Caches &gt;&gt;",
	            "RebuildTypesBtn" => "Rebuild Type Details Caches &gt;&gt;",
	            "CheckSQLDirBtn" => "Check SQL Cache Directories Exists and Create If Required &gt;&gt;",
	            "AreYouSureText" => "Are you sure you want to clear the",
	            "cache_word" => "Cache",
	            "and_rebuild_object" => "and Rebuild Object",
	            "editorial_cache_text" => "Editorial Cache",
	            "stories_cache_text" => "Stories &amp; Comments Cache",
	            "ed_locks_cache_text" => "Editorial Locks Cache",
	            "object_cache_text" => "Objects Cache",
	            "stats_cache_text" => "Statistics Cache",
	            "cancel_btn" => "Cancel");

$textObj = new indyItemSet();

if($textObj->load($system_config->xml_store, "clearcache") === false)
{
    $textObj->setUserMessage( array( USER_ERROR, "Failed to get text strings for the Clear Cache. -Using defaults",""));
    $textObj->writeUserMessageBox();
} else {
    foreach (array_keys($textLabels) as $str_key ) {
        if (trim($textObj->getString($str_key)) != "" )
            $textLabels[$str_key] = $textObj->getString($str_key);
	else
        {
            if ($system_config->user_error_reporting == 8) $textLabels[$str_key] .= " using default ";
	}
    }
}

addToPageTitle($textLabels["title"]);
// addToPageTitle("Clear Cache and Cache Checks");



function writeClearForm()
{
   global $query_cache, $textLabels;

   if ($query_cache->new_cache == true ) $cache_type_in_use = "New Oscailt 3.3 ";
   else $cache_type_in_use = "Standard (Oscailt 3.1) Type ";

   ?>
   <table align=center class=admin>
   <TR class=admin>
      <th class=admin align=center>Clear Cache</th>
   </tr>
   <tr class=admin>
   <?
      if (!isset($textLabels['Summary']) ) {
         ?>
	 <td class=admin><blockquote> The buttons below allow you to clear the various data caches used by oscailt.
         While this usally shouldn't be necessary, you may wish to clear caches if errors become cached
         or cache files get corrupted.
         <BR><BR>
         There is also an addition option to check the database cache directory exists and if it does
         not to create it. For the new database cache directory structure in Oscailt 3.3 onwards, there
         are 5 associated subdirectories that will be checked and created also if that cache mode is in use.
         </blockquote>
         <BR>
         </td>
         <?
      } else {
          ?>
         <td class=admin><blockquote><?=$textLabels['Summary']?> <blockquote></td>
         <?
      }
      ?>
   </tr>


   <tr class=admin>
      <td class=admin align=center> <BR>
         <form action="clearcache.php" method=post>
         <input type=hidden name="cachetype" value="SQL">
	 <input type=submit value="<?=$textLabels['ClearSQLBtn']?>">
         </form>
         <form action="clearcache.php" method=post>
         <input type=hidden name="cachetype" value="RSS">
         <input type=submit value="<?=$textLabels['ClearRSSBtn']?>">
         </form>
         <form action="clearcache.php" method=post>
         <input type=hidden name="cachetype" value="OML">
         <input type=submit value="<?=$textLabels['ClearOMLBtn']?>">
         </form>
         <form action="clearcache.php" method=post>
         <input type=hidden name="cachetype" value="HTML">
	 <input type=submit value="<?=$textLabels['ClearHTMLBtn']?>">
         </form>
         <form action="clearcache.php" method=post>
         <input type=hidden name="cachetype" value="Image">
         <input type=submit value="<?=$textLabels['ClearIMGBtn']?>">
         </form>
         <form action="clearcache.php" method=post>
         <input type=hidden name="cachetype" value="Object">
         <input type=submit value="<?=$textLabels['RebuildCachesBtn']?>">
         </form>
         <form action="clearcache.php" method=post>
         <input type=hidden name="cachetype" value="Type">
         <input type=submit value="<?=$textLabels['RebuildTypesBtn']?>">
         </form>
	 <br>
	 <br>
         <form action="clearcache.php" method=post>
         <input type=hidden name="checkdbcache" value="Type">
         <input type=submit value="<?=$textLabels['CheckSQLDirBtn']?>">
         </form>
	 <p>
	 Note: <?=$cache_type_in_use?> SQL Cache Structure in use.
	 </p>
      </td>
   </tr>

   </table>
   <?
}

function writeError($error)
{
   ?><BR><BR><font class=error><B><?=$error?></B></font><BR><BR><?
}


function writeConfirmClearBox()
{
   global $system_config, $textLabels;
   // No idea what this variable is so commented it out.
   //$time_posted_upper_limit = strtotime($_REQUEST['delete'])+$system_config->timezone_offset;
   // if ($_REQUEST['cachetype'] == "Object") $requestedAction = "and Rebuild Object ";
   if ($_REQUEST['cachetype'] == "Object") $requestedAction = $textLabels['and_rebuild_object'];
   else $requestedAction = $_REQUEST['cachetype'];
   ?>
   <table align=center border=0>
   <form action="clearcache.php" method=post>
   <input type=hidden name="cachetype" value="<?=$_REQUEST['cachetype']?>">
   <tr>
      <td colspan=2 align=center><img src="graphics/caution.gif" align=middle><BR><BR><B> <?=$textLabels['AreYouSureText']?> <?=$_REQUEST['cachetype']?> <?=$textLabels['cache_word']?></B><BR><BR></td>
   </tr>

   <?
   if ($_REQUEST['cachetype'] == "SQL") {

       if (isset($_REQUEST['old_cache']) && $_REQUEST['old_cache'] == 'true') {
            ?> <input type=hidden name="old_cache" value="true"> <?
       } else {
       }
       ?>
       <tr>
       <td align=right><?=$textLabels['editorial_cache_text']?> 1</td><td align=center><input type=checkbox name="sql_cache_1" checked></td>
       </tr>
       <tr>
       <td align=right><?=$textLabels['stories_cache_text']?> 2</td><td align=center><input type=checkbox name="sql_cache_2" checked></td>
       </tr>
       <tr>
       <td align=right><?=$textLabels['ed_locks_cache_text']?> 3</td><td align=center><input type=checkbox name="sql_cache_3" checked></td>
       </tr>
       <tr>
       <td align=right><?=$textLabels['object_cache_text']?> 4</td><td align=center><input type=checkbox name="sql_cache_4" checked></td>
       </tr>
       <tr>
       <td align=right><?=$textLabels['stats_cache_text']?> 5</td><td align=center><input type=checkbox name="sql_cache_5" checked></td>
       </tr>
       <tr> <td colspan=2> &nbsp; </td> </tr>
       <?

       if (isset($_REQUEST['old_cache']) && $_REQUEST['old_cache'] == 'true') {
            ?> <input type=hidden name="old_cache" value="true"> <?
       } else {
            global $query_cache;
	    if ($query_cache->new_cache == true) {
               ?>
               <tr><td align=right><small>Delete pre Oscailt 3.3 cache directory only</small></td><td align=center><input type=checkbox name='old_cache' unchecked> </td> </tr>
	       <tr> <td colspan=2> &nbsp; </td> </tr>
               <?
	    }
       }
   }
   if ($_REQUEST['cachetype'] == "Image") {

       if (isset($_REQUEST['video_cache']) && $_REQUEST['video_cache'] == 'true') {
            ?> <input type=hidden name="video_cache" value="true"> <?
       } else {
       }
       ?>
       <tr>
       <td align=right>Main Image Caches</td><td align=center><input type=checkbox name="main_image_cache" checked></td>
       </tr>
       <tr>
       <td align=right>Embedded Video Cover Images Cache</td><td align=center><input type=checkbox name="video_image_cache" unchecked></td>
       </tr>
       <tr> <td colspan=2> &nbsp; </td> </tr>
       <?

       if (isset($_REQUEST['video_cache']) && $_REQUEST['video_cache'] == 'true') {
            ?> <input type=hidden name="video_cache" value="true"> <?
       } else {
	    global $oscailt_basic_config;
	    if ($oscailt_basic_config['software_version'] >= 3.3 && !file_exists($system_config->image_cache."/video_images/")) {
               ?>
               <tr><td align=right><small>Create embedded video cover image directory if it does not?</small></td><td align=center><input type=checkbox name='video_cache' unchecked> </td> </tr>
	       <tr> <td colspan=2> &nbsp; </td> </tr>
               <?
	    }
       }
   }
   ?>
   <tr>
      <td align=right><input type=submit name=cancel value="&lt;&lt; <?=$textLabels['cancel_btn']?>"></td>
      <td><input type=submit name=confirm value="Clear <?=$requestedAction?> <?=$textLabels['cache_word']?> &gt;&gt;"> </td>
   </tr>
   </form>
   </table>
   <?
}

ob_start();

if($editor_session->isSessionOpen())
{
   writeAdminHeader("viewsitelog.php?log_type=action","View Logs - Editorial");

   if($editor_session->editor->allowedReadAccessTo("clearcache"))
   {
      if(isset($_REQUEST['cachetype']) && $_REQUEST['cachetype']!=null && isset($_REQUEST['confirm']) )
      {
         //perform delete
         if($editor_session->editor->allowedWriteAccessTo("clearcache"))
         {
            global $query_cache;
            $cleared=true;
            if($_REQUEST['cachetype']=="SQL")  {
		$cache_1 = "";
		$cache_2 = "";
		$cache_3 = "";
		$cache_4 = "";
		$cache_5 = "";
		if (isset($_REQUEST['sql_cache_1'])) $cache_1 = $_REQUEST['sql_cache_1'];
		if (isset($_REQUEST['sql_cache_2'])) $cache_2 = $_REQUEST['sql_cache_2'];
		if (isset($_REQUEST['sql_cache_3'])) $cache_3 = $_REQUEST['sql_cache_3'];
		if (isset($_REQUEST['sql_cache_4'])) $cache_4 = $_REQUEST['sql_cache_4'];
		if (isset($_REQUEST['sql_cache_5'])) $cache_5 = $_REQUEST['sql_cache_5'];

		if (isset($_REQUEST['old_cache']) && ($_REQUEST['old_cache']=='true' || $_REQUEST['old_cache']=='on')) {
                    echo("<p align=center><font class=editornotice><BR><B>Deleting pre Oscailt 3.3 ".$_REQUEST['cachetype']." Cache.</B></font></p>");

		    $saved_state = $query_cache->new_cache;
		    $query_cache->new_cache=false;
                }

		if ($cache_1 == 'on' && $cache_2 == 'on' && $cache_3 == 'on' && $cache_4 == 'on' && $cache_5 == 'on')
                    $query_cache->clearCache("sql", 0);
                else 
                {
                    if ($cache_1 == 'on' ) $query_cache->clearCache("sql", 1);
                    if ($cache_2 == 'on' ) $query_cache->clearCache("sql", 2);
                    if ($cache_3 == 'on' ) $query_cache->clearCache("sql", 3);
                    if ($cache_4 == 'on' ) $query_cache->clearCache("sql", 4);
                    if ($cache_5 == 'on' ) $query_cache->clearCache("sql", 5);
                }
		// Undocumented code to delete the old cache
		if (isset($_REQUEST['old_cache']) && ($_REQUEST['old_cache']=='true' || $_REQUEST['old_cache']=='on')) {
		    $query_cache->new_cache = $saved_state;
                }

            }
            else if($_REQUEST['cachetype']=="RSS") $query_cache->clearCache("rss");
            else if($_REQUEST['cachetype']=="OML") $query_cache->clearCache("oml",0);
            else if($_REQUEST['cachetype']=="HTML") clearHtmlCache(true);
            else if($_REQUEST['cachetype']=="Image")
            {
		$clr_main_image = false;
		$clr_video_image = false;
		if (isset($_REQUEST['main_image_cache'])) $clr_main_image = $_REQUEST['main_image_cache'];
		if (isset($_REQUEST['video_image_cache'])) $clr_video_image = $_REQUEST['video_image_cache'];

		if (isset($_REQUEST['video_cache']) && ($_REQUEST['video_cache']=='true' || $_REQUEST['video_cache']=='on')) {
		    $t_path = $system_config->image_cache."/video_images/";
                    echo("<p align=center><font class=editornotice><BR><B>Creating video image cache ".$t_path."</B></font></p>");
		    if(!mkdir($t_path, $system_config->default_writable_directory_permissions))
                        echo("<p align=center><font class=editornotice><BR><B>Unable to create directory ".$t_path."</B></font></p>");
                }

                $imagetool = new ImageTool();
                $imagetool->clearImageCache($clr_main_image, $clr_video_image);

		$extra_text = "";
		if ($clr_main_image == true) $extra_text = " main ";
		if ($clr_main_image == true && $clr_video_image == true) $extra_text .= " and ";
		if ($clr_video_image == true) $extra_text .= " video images ";
		if ($extra_text != "") $extra_text = " auto note: clear " . $extra_text. " cache";
                logAction("", "all-images", "image", "cache clear", $extra_text);
            }
            elseif($_REQUEST['cachetype']=="Object")
            {
               include_once("objects/indyobjects/indydataobjects.inc");
               global $system_config, $redirectList;
               $redirectList->load();
               $obj_set = new indyObjectSet($system_config->xmltypedef_dir, $system_config->object_index_storage);
               $x = new indyObjectActionRequest();
               $obj_set->load(array("*"),array("*"), $x);
               $obj_set->rebuildCaches(array("*"));
               $obj_set->writeUserMessageBox();
               logAction("", "all-objects", "object", "site-recache");

            }
            elseif($_REQUEST['cachetype']=="Type")
            {
               include_once("objects/indyobjects/indydataobjects.inc");
               global $system_config;
               $obj_set = new indyObjectSet($system_config->xmltypedef_dir, $system_config->object_index_storage);
               $x = new indyObjectActionRequest();
               $obj_set->load(array("*"),array("*"), $x);
               $obj_set->supportedTypes->saveTypeDetailsCache();
               $obj_set->writeUserMessageBox();
               logAction("", "all-types", "type object", "type-recache");
            }
            else
            {
               $cleared=false;
               echo("<p align=center><font class=editornotice><BR><B>".$_REQUEST['cachetype']." Cache Type Is Unknown!</B></font></p>");
            }
            if($cleared) echo("<p align=center><font class=editornotice><BR><B>".$_REQUEST['cachetype']." Cache Cleared!</B></font></p>");
         }
         else $editor_session->writeNoWritePermissionError();
         writeClearForm();
      }
      else if(isset($_REQUEST['cachetype']) && $_REQUEST['cachetype']!=null && isset($_REQUEST['cancel']) )
      {
         writeClearForm();
      }
      else if(isset($_REQUEST['cachetype']) && $_REQUEST['cachetype']!=null)
      {
         writeConfirmClearBox();
      }
      else if(isset($_REQUEST['checkdbcache']) && $_REQUEST['checkdbcache'] !=null)
      {
         // Check it exists 
         if($editor_session->editor->allowedWriteAccessTo("clearcache"))
         {
            global $query_cache;

            $error_array = $query_cache->checkCacheDirs(true, true);
	    if (count($error_array) == 0 ) {
                echo("<p align=center><font class=editornotice><BR> All SQL Cache Directories exist.</font></p>");
            } else {
                echo("<p align=center><font class=editornotice><BR>");
                foreach ($error_array as $each_msg) {
                    echo $each_msg."<BR>";
                }
                echo("</font></p>");
            }
         }
         else $editor_session->writeNoWritePermissionError();
         writeClearForm();
      }
      else
      {
         writeClearForm();
      }
   }
   else $editor_session->writeNoReadPermissionError();
}
else $editor_session->writeNoSessionError();


require_once("adminfooter.inc");
?>
