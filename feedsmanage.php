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
/* Description:                                                         */
/* This file displays basic information about the objects like their    */
/* ids, type, oscailt class, name, cache filename. It can sort by and   */
/* filter by various fields.                                            */
/************************************************************************/
require_once("oscailt_init.inc");
require_once("objects/indyobjects/indydataobjects.inc");
require_once("objects/feed_utilities.inc");
$OSCAILT_SCRIPT = "feedsmanage.php";
addToPageTitle("Imported Feeds Manager and Status Screen ");


function writeLocalAdminHeader()
{
   global $OSCAILT_SCRIPT;
   ?>
     <TABLE class='admin'>
        <TR class='admin'><TD class='admin'><a href="<?=$OSCAILT_SCRIPT?>?sort=none">Raw from Db</a> | <a href="<?=$OSCAILT_SCRIPT?>?sort=type-site">Sort By Site Id</a>
        | <a href="<?=$OSCAILT_SCRIPT?>?sort=id-type-site">Sort By Obj Id &amp Site Id</a> </TD></TR>
     </TABLE>
   <?
}

function getDropdownList( $ObjectArray, $FilteredSelection)
{
   $js_string = "<option value=\"No Filter\">No Filter</option>";
   $form_string ="<option value=\"";
   foreach ( $ObjectArray as $each_type)
   {
       if ($each_type == $FilteredSelection)
       {
            $js_string .= "<option selected value=\"" . $each_type ."\">" . $each_type . "</option>";
       }
       else
       {
            $js_string .= $form_string . $each_type ."\">" . $each_type . "</option>";
       }
   }

   return $js_string;
}
function getHeadings( $HeadingsList)
{
   $table_str = "";
   foreach ( $HeadingsList as $each_heading)
   {
       $table_str .= "<th class=admin>&nbsp;".ucfirst($each_heading)."&nbsp;</th>";
   }

   return $table_str;
}
// The input is an array of the meta data tags to use
function getDataFields($ObjectPtr, $DataList)
{
   $table_str = "";
   foreach ( $DataList as $each_field)
   {
       if (trim($each_field) == "protected") {
           $each_value = $ObjectPtr->protected ? "<b>yes</b>" : "no";
       } else {
           $each_value = $ObjectPtr->getMeta($each_field);
       }
       $table_str .= "<td class=admin>&nbsp;".$each_value."&nbsp;</td>";
   }

   return $table_str;
}
/******************************************************************************/
/* This function displays a list of all the object ids created and their type */
/* It can sort them by type, site and object id                               */
/* It can also filter by object type, site id and oscailt class. Any or all   */
/* filters can be switched on or off                                          */
/******************************************************************************/
function writeFeedsObjectList( $sort_type='none')
{
   global $system_config, $userLanguage, $obj_man, $OSCAILT_SCRIPT;

   //$available_objects = $obj_man->obj_set->getAllObjectIDs();
   $available_objects = $obj_man->obj_set->getObjectStubs();

   // Change because sites are mixed in here.
   //$total_objs = count($available_objects);

   // Need to get the count done here, even though we go through the loop below again.
   $total_objs = 0;
   foreach($available_objects as $obj_stub)
   {
      if ($obj_stub->obj_type == "SITE") continue;
      $total_objs++;
   }


   $full_display_on = false;
   if (isset($_REQUEST['full_display'])) $full_display_on = true;

   $filter_object_on = false;
   $filter_site_on = false;
   $def_filter_object="No Filter";
   $def_filter_site  ="No Filter";
   if ( isset($_REQUEST['filter_object'])) { $def_filter_object = $_REQUEST['filter_object'];}
   if ( isset($_REQUEST['filter_site'])) { $def_filter_site = $_REQUEST['filter_site'];}

   # The value No Filter can still be set in which case it is really off.
   if ( $def_filter_object != "No Filter") { $filter_object_on = true; }
   if ( $def_filter_site != "No Filter") { $filter_site_on = true; }

   # If we filter on the object name then we will display the object name.
   $col_span_size = 9;
   $obj_name_info = $obj_man->obj_set->getObjectInfoByTypename("feedimport", "en" );
   if ( $full_display_on == true ) 
   {
       $col_span_size = 10;
   }
   ?>
   <table align=center>
   <tr class=admin>
      <th class=admin colspan=<?=$col_span_size?>>Feed Import Object Total = <?=$total_objs?> </td>
   </tr>
   <?
   if ($sort_type != 'none')
   {
       if ($sort_type == 'type-site') $sort_text = "Site";
       else if ($sort_type == 'id-type-site') $sort_text = "Site and Object Id";
       ?>
       <tr class=admin>
         <th class=admin colspan=<?=$col_span_size?>>Sorted by <?=$sort_text?> </td>
       </tr>
       <?
   }
   if ( $filter_object_on == true ) 
   {
       $t_object_url = "<a href='admin.php?action=list&site_id=1&obj_type=".$def_filter_object."&obj_language=en'>".  $def_filter_object."</a>";
       // Previously was just $def_filter_object
       ?>
       <tr class=admin>
         <td class=admin colspan=<?=$col_span_size?>>Filtering by Object Type: <b><?=$t_object_url?></b> </td>
       </tr>
       <?
   }

   # Generate the form selections ... would it be more efficient to print each selection or
   # as here generate fulls string and then print? Probably sub-millisecond difference anyhow.
   ?>
   <FORM name="importfeeds_objsform" action="<?=$OSCAILT_SCRIPT?>" method="post">
   <tr class=admin>
      <td align=left colspan=<?=$col_span_size?>>
   <?
   if ( $full_display_on == false ) {
      ?>
      <input type=submit name=full_display value="Show URL &gt;&gt;"> &nbsp;
      <?
   } else {
      ?>
      <input type=submit name=full_display_off value="Hide URL &gt;&gt;"> &nbsp;
      <?
   }
   ?>  
      <input type=submit name=filter_tag value="Filter"> Site 
      <select name='filter_site' > 
      <?  
      $js_string = getDropdownList($obj_man->obj_set->sites, $def_filter_site);
      echo $js_string; 
      ?>
      </select> 
      </td>
   </tr>
   </form>
   <tr class=admin>
      <th class=admin>&nbsp;#&nbsp;</th>
      <th class=admin>&nbsp;Object ID&nbsp;</th>
      <th class=admin>&nbsp;Site&nbsp;</th>
      <th class=admin>&nbsp;Feed Name&nbsp;</th>
      <th class=admin>&nbsp;Feed Type&nbsp;</th>
   <?
   if ($full_display_on == true) { 
      ?> <th class=admin>&nbsp;Imported Feed URL&nbsp;</th> <?
   }
   ?>
      <th class=admin>&nbsp;Status&nbsp;</th>
      <th class=admin>&nbsp;# Retries&nbsp;</th>
      <th class=admin>&nbsp;Last Retry&nbsp;</th>
      <th class=admin>&nbsp;Last Error&nbsp;</th>
   </tr>
   <?

   // Grab the data and store in arrays as needed per sort. The PHP sort will rejig all
   // the data in all the arrays passed to it.

   // Get the actual list. Then get the list from the db and match the two.
   $ImportStatusList = getImportFeedsStatusList();

   $counter = 0;
   foreach($available_objects as $obj_stub)
   {
      if ($obj_stub->obj_type == "SITE") continue;
      $arr_order[] = $counter++;
      $arr_obj_stub[] = $obj_stub;
      $arr_obj_site[] = $obj_stub->site_id;
      if ($sort_type == 'id-type-site')
      {
          $arr_obj_id[] = $obj_stub->obj_id;
      }
   }

   # Best reading up the PHP array_multisort to figure this out..
   if ($sort_type == 'type-site')
   {
      array_multisort($arr_obj_site, $arr_order);
   }
   else if ($sort_type == 'id-type-site')
   {
      array_multisort($arr_obj_site, $arr_obj_id, $arr_order);
   }

   $iRow = 0;
   // Build up array of objects for fast searching. Actually object_id should be unique.
   $LookupArray = array();
   foreach ($ImportStatusList as $dbFeedStatusObj) {
       array_push($LookupArray, $dbFeedStatusObj->object_id);
   }

   $http_mode = isset($_SERVER['HTTPS']) ? "https" : "http";
   $url_base = '<a href="'.$http_mode.'://'.$system_config->site_url.'/';

   $ok_cnt = 0;
   $unknown_cnt = 0;
   $error_cnt = 0;

   for($aIndex=0;$aIndex < $counter ;$aIndex++)
   {
      $sortIndex = $arr_order[$aIndex];
      if ($sort_type == 'id-type-site')
      {
          $ob_id = $arr_obj_id[$aIndex];
      }
      else
      {
          $ob_id = $arr_obj_stub[$sortIndex]->obj_id;
      }
      $ob_site = $arr_obj_site[$aIndex];
      $ob_site_url= $url_base. 'admin.php?action=list&site_id='.$ob_site.'&obj_type=feedimport">'.$ob_site.'</a>';

      # This filtering could be done before the sorting. It would be a lot more efficient
      # but it was done here, because it allowed for easier debug during development.
      if ( $filter_object_on == true && $obj_typename != $def_filter_object) { continue; }
      if ( $filter_site_on == true && $ob_site != $def_filter_site) { continue; }

      $iRow++;

      $new_obj = $obj_man->obj_set->fetchObject($ob_id, "en");

      $feed_name = $new_obj->name();
      $feed_type = "unknown";
      $feed_url  = $new_obj->getMeta('url') ;
      $feed_status = "<font color='orange'>unknown</font>";
      $number_retries = " N/A";
      $last_retry_time = "N/A";
      $last_error_text = "";

      $tPtr = array_search($ob_id,$LookupArray);
      if ($tPtr === FALSE) {
          $unknown_cnt++;
      } else {
          $dbFeedStatusObj = $ImportStatusList[$tPtr];

          if ($dbFeedStatusObj->site_id == $ob_site && $dbFeedStatusObj->object_id == $ob_id)
	  {
              $feed_type = $dbFeedStatusObj->feed_type;
	      if ($dbFeedStatusObj->feed_status == "Okay") {
                  $feed_status = "<font color='green'>Okay</font>";
                  $ok_cnt++;
	      } else {
                  $feed_status = "<font color='red'>".$dbFeedStatusObj->feed_status."</font>";
                  $error_cnt++;
	      }

              $number_retries = $dbFeedStatusObj->retries;
              $last_retry_time = strftime("%a %d %b %H:%M:%S",$dbFeedStatusObj->last_retry+$system_config->timezone_offset);
              $last_error_text = $dbFeedStatusObj->last_error;
	  }
      } 

      ?>
      <tr class=admin>
         <td class=admin align=center><?=($iRow)?></td>
         <td class=admin align=center><?=$ob_id?></td>
      <?
      // Check for isProtected and display coloured ball -red or green
      ?>
         <td class=admin align=center><?=$ob_site_url?></td>
         <td class=admin align=left><?=$feed_name?></td>
         <td class=admin align=center><?=$feed_type?></td> 
      <?
      if ($full_display_on == true) { 
         ?>
         <td class=admin align=left><?=$feed_url?></td>
         <?
      }
      ?>
         <td class=admin align=left><?=$feed_status?></td>
         <td class=admin align=center><?=$number_retries?></td>
         <td class=admin align=center><?=$last_retry_time?></td>
         <td class=admin align=left><?=$last_error_text?></td>
      </tr>
      <?
   }

   $ok_percent =floor($ok_cnt / $total_objs * 100 + .5);
   $unknown_percent =floor($unknown_cnt / $total_objs * 100 + .5);
   $error_percent =floor($error_cnt / $total_objs * 100 + .5);
   $total_percent = $ok_percent + $unknown_percent + $error_percent;
   
   $t_percent_str = " ".$ok_cnt. " (".$ok_percent."%) <BR> ";
   $t_percent_str .= $error_cnt. " (".$error_percent."%) <BR> ";
   $t_percent_str .= $unknown_cnt. " (".$unknown_percent."%) <BR> ";
   $t_percent_str .= $total_objs. " (".$total_percent."%)";

   ?>
   <tr class=admin>
      <td class=admin align=left colspan=3> Okay State <BR> Error State <BR> Unknown State <BR> Totals </TD>
      <td class=admin align=left colspan=<?=($col_span_size-3)?>> <?=$t_percent_str?> </td>
   </tr> 
   <tr class=admin>
   <FORM name="importfeeds_reset" action="<?=$OSCAILT_SCRIPT?>" method="post">
   <tr class=admin>
      <td align=center colspan=<?=$col_span_size?>> <BR>
      <input type=submit name=reset_retries value="Reset Retry Count to Zero &gt;&gt;"> <BR> </td>
   </tr>
   </form>
   </table>
   <?

   // writeJSFunctions();
}
// Function to return the actual list of status entries for imported feeds. This may be less than the full 
// configured list because some of them may never have been active.
function getImportFeedsStatusList()
{
    global $dbconn, $prefix;

    $sql_str1 = "SELECT site_id, object_id, retries, feed_type, feed_status, UNIX_TIMESTAMP(last_retry), feed_url, last_error FROM ".$prefix."_importfeeds_status";
    $result_rss = sql_query($sql_str1, $dbconn, 2);
    checkForError($result_rss);

    $ImportFeedsList = array();
    if(sql_num_rows( $result_rss ) > 0)
    {
        $total_rows = sql_num_rows( $result_rss );
        for ($iRow=0; $iRow < $total_rows; $iRow++)
        {
             list($t_site, $t_obj, $t_retries, $t_feedtype, $t_feedstatus, $t_last_retry, $t_feed_url, $t_last_error) = sql_fetch_row($result_rss, $dbconn);
             $t_feed_obj = new ImportFeedStatus();
             $t_feed_obj->site_id = $t_site;
             $t_feed_obj->object_id = $t_obj;
             $t_feed_obj->retries = $t_retries;
             $t_feed_obj->feed_type = $t_feedtype;
             $t_feed_obj->feed_status = $t_feedstatus;
             $t_feed_obj->last_retry = $t_last_retry;
             $t_feed_obj->feed_url = $t_feed_url;
             $t_feed_obj->last_error = $t_last_error;

	     // echo("i ".$iRow. " o ".$t_obj."<BR>");
             $ImportFeedsList[$iRow] = $t_feed_obj;
        }
    }

    return $ImportFeedsList; 
}
// Function to reset the retry counts to zero which allows the RSS and Atom imported feeds to be retried.
// 
function resetImportFeedsRetryCount()
{
    global $dbconn, $prefix;

    $sql_str1 = "UPDATE ".$prefix."_importfeeds_status SET retries=0 WHERE retries > 0 ";
    $result_rss = sql_query($sql_str1, $dbconn, 2);
    checkForError($result_rss);
}

function writeJSFunctions()
{
    global $system_config, $obj_man;

    # This function is no longer used. It could be updated to re-submit on selection above.
    ?>
    <script type="text/javascript" language="Javascript">
         function checkFilterItemChange()
         {
            if(document.viewobjsform.filter_object.value=="tag_1")
            {
               // Do the submit
            }
         }
    </script>
    <?
}

function writeError($error)
{
   ?><BR><BR><font class=error><B><?=$error?></B></font><BR><BR><?
}

function loadManagedObjectSet()
{
   global $system_config, $OSCAILT_SCRIPT, $obj_man;
   # This ultimately loads the object stubs (i.e. basic info) for all known objects.
   $obj_man = new indyObjectManager($system_config->xmltypedef_dir, $OSCAILT_SCRIPT);
   $obj_man->obj_set = new indyObjectSet($obj_man->type_dir, $obj_man->storage);
   $sites = array("*");
   $types = array("feedimport");
   if(!$obj_man->obj_set->load($sites, $types, $obj_man->action_req))
   {
      $obj_man->writeUserMessageBox();
      writeError("Programme Error: Failed to Load Set of Managed FeedImport Objects.");
      return false;
   }
   return true;
}

ob_start();
$obj_man = null;
$admin_table_width = "100%";
$max_rss_retries = 3;
// loadManagedObjectSet();

if($editor_session->isSessionOpen())
{
   writeAdminHeader("viewsitelog.php?log_type=rssfeeds","View Bad RSS File");


   if($editor_session->editor->allowedReadAccessTo("editdataobjects"))
   {
      writeLocalAdminHeader();
      if( loadManagedObjectSet() == true ) {
         $sort_mode = 'none';
         if ( isset($_REQUEST['sort']))
         {
             $sort_mode = $_REQUEST['sort'];
         }
	 // If resetting the counts do it now, then call the display and read the data from the db.
         if ( isset($_REQUEST['reset_retries'])) 
	 {
             resetImportFeedsRetryCount();
	 }
         writeFeedsObjectList($sort_mode);
      }
   }
   else $editor_session->writeNoReadPermissionError();
}
else $editor_session->writeNoSessionError();

include_once("adminfooter.inc");
?>