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
require_once("objects/magpie/rss_utils.inc");
require_once("objects/indyobjects/indyobject.inc");
require_once("objects/indyobjects/indyitemset.inc");

$OSCAILT_SCRIPT = "feedsmanage.php";
$textLabels = array("title" => "Imported Feeds Manager and Status Screen ",
	            "link_text1" => "Raw from Db",
	            "link_text2" => "Sort by Site Id",
	            "link_text3" => "Sort by Obj Id &amp; Site Id",
	            "link_text4" => "Sort by Feed Name",
	            "dropdown_text" => "No Filter",
	            "no_filter" => "No Filter",
	            "feed_import_object_subtitle" => "Feed Import Object Total = ",
	            "Feed_import_object_text" => "If you reset the retry counts, you may need to also clear the RSS cache to resolve problems with feeds",
	            "site_word" => "Site",
	            "site_object" => "Site and Object Id",
	            "feed_name_word" => "Feed Name",
	            "sorted_by" => "Sorted By",
		    "filter_by_obj_type" => "Filtering by Object Type:",
	            "show_url_btn" => "Show URL",
	            "hide_url_btn" => "Hide URL",
	            "filter_by_site_btn" => "Filter By Site",
	            "filter_by_status_btn" => "Filter by Status",
	            "okay_choice" => "Okay",
	            "unknown_choice" => "unknown",
	            "fetch_failure_choice" => "Fetch failure",
	            "parse_failure_choice" => "Parse failure",
	            "object_id_label" => "Object Id",
	            "site_label" => "Site",
	            "feedname_label" => "Feed Name", 
		    "feedtype_label" => "Feed Type",
		    "imported_feed_url_label" => "Imported Feed URL",
		    "status_label" => "Status",
		    "retries_label" => "# Retries",
		    "reset_cnt_label" => "Reset Cnt",
		    "preparse_label" => "PreParse",
		    "last_retry_label" => "Last Retry",
		    "last_error_label" => "Last Error / Cache File",
		    "reset_cnt_label" => "check_spamlinks",
		    "non_applicable_label" => "N/A",
		    "add_label" => "Add",
		    "edit_label" => "Edit", 
		    "cache_file_label" => "Cache File",
		    "all_stats_totals_labels" => "Okay State &lt;BR&gt; Error State &lt;BR&g; Unknown State &lt;BR&gt; Totals",
		    "reset_all_retry_to_zero_btn" => "Reset all Retry Counts to Zero",
		    "reset_all_retry_to_zero_subtext" => "You may need to clear the RSS cache too",
		    "unknown_preparse_rule_mode_text" => "Unknown Pre-Parse Fixup Rule editing mode",
		    "no_site_key_passed_text" => "No Site - Object Key passed Pre-Parse Fixup Rule editing mode",
	            "preparse_title_part1" => "Pre-Parse Fixup Rules for Imported Feed:",
	            "preparse_title_part2" => "Object Id:",
	            "preparse_title_part4" => "For 'Apply to Lines', enter line numbers separated by spaces. Search and Replace option only applicable to Replace mode",
	            "rule_label" => "Rule",
	            "apply_to_line_label" => "Apply to Lines",
	            "search_string_label" => "Search String",
	            "replacement_str_label" => "Replacement String",
	            "add_rule_btn_label" => "Add Rule",
	            "cancel_btn_label" => "Cancel",
	            "save_rules_btn_label" => "Save Rules",
	            "remove_all_btn_label" => "Remove All Rules",
	            "rule_btns_subtext" => "To delete a rule just blank out the 'Apply to Lines' field and then Save-Rules.",
	            "remove_lines_label" => "Remove Lines",
	            "replace_text_label" => "Replace Text on Line(s)",
	            "configuration_saved_msg" => "Configuration Saved!");

$textObj = new indyItemSet();

if($textObj->load($system_config->xml_store, "feedsmanage") === false)
{
    $textObj->setUserMessage( array( USER_ERROR, "Failed to get text strings for the Feeds Manage. -Using defaults",""));
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
//addToPageTitle("Imported Feeds Manager and Status Screen ");


function writeLocalAdminHeader()
{
   global $OSCAILT_SCRIPT, $textLabels;
   ?>
     <TABLE class='admin'>
	<TR class='admin'><TD class='admin'><a href="<?=$OSCAILT_SCRIPT?>?sort=none"><?=$textLabels['link_text1']?></a> | <a href="<?=$OSCAILT_SCRIPT?>?sort=type-site"><?=$textLabels['link_text2']?></a>
	| <a href="<?=$OSCAILT_SCRIPT?>?sort=id-type-site"><?=$textLabels['link_text3']?></a> 
        | <a href="<?=$OSCAILT_SCRIPT?>?sort=name"><?=$textLabels['link_text4']?></a> </TD></TR>
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
   global $textLabels;

   // Call for magpie settings.
   init();
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
   $show_url = false;
   if (isset($_REQUEST['full_display'])) {
       $full_display_on = true;
       $show_url = true;
   } elseif (isset($_REQUEST['full_display_off'])) {
       $show_url = false;
   } else {
       if (isset($_REQUEST['show_url']) && ($_REQUEST['show_url'] == 'true' || $_REQUEST['show_url'] == true || $_REQUEST['show_url'] == '1')) {
           $show_url = true;
           $full_display_on = true;
       }
   }

   $filter_object_on = false;
   $filter_site_on = false;
   $def_filter_object="No Filter";
   $def_filter_site  ="No Filter";
   if ( isset($_REQUEST['filter_object'])) { $def_filter_object = $_REQUEST['filter_object'];}
   if ( isset($_REQUEST['filter_site'])) { $def_filter_site = $_REQUEST['filter_site'];}
   $filter_by_status = false;
   if ( isset($_REQUEST['filter_status_tag']) && isset($_REQUEST['filter_status']) && $_REQUEST['filter_status'] != "No Filter") {
       $filter_by_status = true;
       $filter_status = $_REQUEST['filter_status'];
   }

   # The value No Filter can still be set in which case it is really off.
   if ( $def_filter_object != "No Filter") { $filter_object_on = true; }
   if ( $def_filter_site != "No Filter") { $filter_site_on = true; }

   # If we filter on the object name then we will display the object name.
   $col_span_size = 11;
   $obj_name_info = $obj_man->obj_set->getObjectInfoByTypename("feedimport", "en" );
   if ( $full_display_on == true ) 
   {
       $col_span_size = 12;
   }
   ?>
   <table align=center>
   <tr class=admin>
      <th class=admin colspan=<?=$col_span_size?>><?=$textLabels['feed_import_object_subtitle']?>  <?=$total_objs?> <br>
      <small><?=$textLabels['feed_import_object_text']?> </small>
      </th>
   </tr>
   <?
   if ($sort_type != 'none')
   {
       if ($sort_type == 'type-site') $sort_text = $textLabels['site_word'];
       else if ($sort_type == 'id-type-site') $sort_text = $textLabels['site_object'];
       else if ($sort_type == 'name') $sort_text = $textLabels['feed_name_word'];
       ?>
       <tr class=admin>
	 <th class=admin colspan=<?=$col_span_size?>><?=$textLabels['sorted_by']?> <?=$sort_text?> </td>
       </tr>
       <?
   }
   if ( $filter_object_on == true ) 
   {
       $t_object_url = "<a href='admin.php?action=list&site_id=1&obj_type=".$def_filter_object."&obj_language=en'>".  $def_filter_object."</a>";
       // Previously was just $def_filter_object
       // Filtering by Object Type: 
       ?>
       <tr class=admin>
	 <td class=admin colspan=<?=$col_span_size?>><?=$textLabels['filter_by_obj_type']?> <b><?=$t_object_url?></b> </td>
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
      <input type=submit name=full_display value="<?=$textLabels['show_url_btn']?> &gt;&gt;"> &nbsp;
      <?
   } else {
      ?>
      <input type=submit name=full_display_off value="<?=$textLabels['hide_url_btn']?> &gt;&gt;"> &nbsp;
      <?
   }
   ?>  
      <input type=hidden name=show_url value="<?=$show_url?>">
      <input type=submit name=filter_tag value="<?=$textLabels['filter_by_site_btn']?>">
      <select name='filter_site' > 
      <?  
      $selected_choice = "No Filter";
      if (isset($_REQUEST['filter_site'])) $selected_choice = $_REQUEST['filter_site'];
      $js_string = getDropdownList($obj_man->obj_set->sites, $selected_choice);
      echo $js_string; 
      ?>
      </select> &nbsp;
      <input type=submit name=filter_status_tag value="<?=$textLabels['filter_by_status_btn']?>">
      <select name='filter_status' > 
      <?  
      $selected_choice = "No Filter";
      if (isset($_REQUEST['filter_status'])) $selected_choice = $_REQUEST['filter_status'];
      $js_string = getDropdownList(array("Okay", "unknown", "Fetch failure", "Parse failure"), $selected_choice);
      echo $js_string; 
      ?>
      </select> 

      </td>
   </tr>
   </form>
   <tr class=admin>
      <th class=admin>&nbsp;#&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['object_id_label']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['site_label']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['feedname_label']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['feedtype_label']?>&nbsp;</th>
   <?
   if ($show_url == true) { 
      ?> <th class=admin>&nbsp;<?=$textLabels['object_id_label']?>&nbsp;</th> <?
   }
   ?>
      <th class=admin>&nbsp;<?=$textLabels['status_label']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['retries_label']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['reset_cnt_label']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['preparse_label']?>&nbsp;<br>fix</th>
      <th class=admin>&nbsp;<?=$textLabels['last_retry_label']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['last_error_label']?>&nbsp;</th>
   </tr>
   <?

   // Grab the data and store in arrays as needed per sort. The PHP sort will rejig all
   // the data in all the arrays passed to it.

   // Get the actual list. Then get the list from the db and match the two.
   $ImportStatusList = getImportFeedsStatusList();
   $rulesSet = getFixupRules();
   $ruleKeys = array_keys($rulesSet);

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
      else if ($sort_type == "name") 
      {
          $arr_obj_id[] = $obj_stub->obj_id;
          $new_obj = $obj_man->obj_set->fetchObject($obj_stub->obj_id, "en");
          $feed_name = $new_obj->name();
          $arr_feed_url[] = $new_obj->getMeta('url') ;
          $arr_obj_name[] = $feed_name;
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
   else if ($sort_type == 'name')
   {
      array_multisort($arr_obj_name, $arr_obj_site, $arr_obj_id, $arr_order);
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
      if ($sort_type == 'id-type-site' OR $sort_type == 'name')
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

      if ($sort_type == 'name') {
          $feed_name = $arr_obj_name[$aIndex] ;
          $feed_type = "unknown";
          $feed_url  = $arr_feed_url[$aIndex] ;
      } else {
          $new_obj = $obj_man->obj_set->fetchObject($ob_id, "en");
          $feed_name = $new_obj->name();
          $feed_type = "unknown";
          $feed_url  = $new_obj->getMeta('url') ;
      }

      if (strlen($feed_name) > 23) $feed_name = "<small>".$feed_name."</small>";
      $feed_status = "<font color='orange'>unknown</font>";
      $number_retries = " N/A";
      $last_retry_time = "N/A";
      $last_error_text = "";
      $reset_form = "";

      $tPtr = array_search($ob_id,$LookupArray);
      if ($tPtr === FALSE) {
          $unknown_cnt++;
          $rule_str = "N/A";
          if ( $filter_by_status == true && $filter_status != "unknown") { continue; }
      } else {
          $dbFeedStatusObj = $ImportStatusList[$tPtr];

          //if ( $filter_by_status == true )echo ">".$dbFeedStatusObj->feed_status." and ".$filter_status."]<BR>";
          if ( $filter_by_status == true && $dbFeedStatusObj->feed_status != $filter_status) { continue; }

          $t_ruleKey = $dbFeedStatusObj->site_id."-".$dbFeedStatusObj->object_id;

          if (in_array($t_ruleKey, $ruleKeys) == true) {
	      // URL for Edit rule
              $rule_str = "<a href='".$OSCAILT_SCRIPT."?fixup_rule=edit&key=".$t_ruleKey."&feedname=".urlencode($feed_name)."'>".$textLabels['edit_label']."</a>";
          } else {
	      // URL for Add rule
              $rule_str = "<a href='".$OSCAILT_SCRIPT."?fixup_rule=add&key=".$t_ruleKey."&feedname=".urlencode($feed_name)."'>".$textLabels['add_label']."</a>";
          }


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
              $t_cache_file = md5(($feed_url . MAGPIE_OUTPUT_ENCODING));
	      if (strlen($last_error_text) > 0 && ($dbFeedStatusObj->feed_status != "Fetch failure")) {
              	//$last_error_text .= "<BR>Cache file ";
              	$last_error_text .= "<BR>".$textLabels['cache_file_label'];
                if (file_exists($system_config->rss_cache."/".$t_cache_file)) $last_error_text .= "exists: ";
                else $last_error_text .= " does <b>not</b> exist: ";
              	$last_error_text .= $t_cache_file;
              	// $last_error_text .= "<BR>". ($feed_url . MAGPIE_OUTPUT_ENCODING);
	      } else if (strlen($last_error_text) == 0 && ($dbFeedStatusObj->feed_status == "Okay")) {
              	$last_error_text .= "Cache file ";
                if (file_exists($system_config->rss_cache."/".$t_cache_file)) $last_error_text .= "exists: ";
                else $last_error_text .= " does <b>not</b> exist: ";
              	$last_error_text .= $t_cache_file;
              	//$last_error_text .= "Cache file: ". md5(($feed_url . MAGPIE_OUTPUT_ENCODING));
	      }
              $reset_form = '<form name="rss_reset_'.$iRow.'" action="'.$OSCAILT_SCRIPT.'" method="post">';
              $reset_form .= '<input type=submit name=single_reset value="Reset">';
              $reset_form .= '<input type=hidden name=reset_site  value="'.$ob_site.'">';
              $reset_form .= '<input type=hidden name=reset_obj   value="'.$ob_id.'">';
              $reset_form .= '<input type=hidden name=reset_cachefile value="'.$t_cache_file.'">';
	      if (isset($_REQUEST['filter_status'])) {
                  $reset_form .= '<input type=hidden name=filter_status value="'.$_REQUEST['filter_status'].'">';
                  $reset_form .= '<input type=hidden name=filter_status_tag value="Filter by Status">';
	      }
              $reset_form .= '</form>';
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
      if ($show_url == true) { 
         ?>
		 <td class=admin align=left><a href="<?=$feed_url?>" target="new_"><?=$feed_url?></a></td>
         <?
      }
      ?>
         <td class=admin align=left><?=$feed_status?></td>
         <td class=admin align=center><?=$number_retries?></td>
         <td class=admin align=center><?=$reset_form?></td>
         <td class=admin align=center><?=$rule_str?></td>
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
      <input type=submit name=reset_retries value="<?=$textLabels['reset_all_retry_to_zero_btn']?> &gt;&gt;"> <BR> 
      <!-- You may need to clear the RSS cache too -->
      <small><?=$textLabels['reset_all_retry_to_zero_subtext']?> </small>
      </td>
   </tr>
   </form>
   </table>
   <?

   // writeJSFunctions();
}
// Function to display the screen to handled adding or editing simple rules to be applied to fixup RSS or ATOM
// feeds before they are parsed.
function editFeedRules($rule_request)
{
   global $OSCAILT_SCRIPT, $textLabels;

   // echo "INPUT [".$rule_request."]<BR>";
   if (substr($rule_request,0,3) == "add") $rule_mode = "Add";
   else if (substr($rule_request,0,4) == "edit") $rule_mode = "Edit";
   else 
   {
	   //writeError("Unknown Pre-Parse Fixup Rule editing mode: [".$rule_request."]");
	   writeError($textLabels['unknown_preparse_rule_mode_text']. ": [".$rule_request."]");
	   return;
   }
   if (!isset($_REQUEST['key']))
   {
	   // writeError("No Site - Object Key passed Pre-Parse Fixup Rule editing mode.");
	   writeError($textLabels['no_site_key_passed_text']);
	   return;
   }

   $key_parts = explode("-",trim($_REQUEST['key']));
   $rule_site_id = $key_parts[0];
   $rule_obj_id = $key_parts[1];
   // $rule_key = $_REQUEST['key'];
   $rule_key = $rule_site_id."-".$rule_obj_id;

   if ( isset($_REQUEST['remove_rules'])) {
       deleteFixupRules($rule_site_id, $rule_obj_id);
   }

   if ( isset($_REQUEST['save_rules'])) {
   	if ( isset($_REQUEST['rule_count'])) {
	    $save_rules = array();
	    // echo "Rule mode = ". $rule_mode . " ";
	    // echo "rule cnt = ". $_REQUEST['rule_count']. "<BR>";
	    for ($iRule = 1; $iRule <= $_REQUEST['rule_count']; $iRule++) {
		$save_this_rule = true;
		$rule_info = array();
		$filter_field = "filter_rule_".$iRule;
		if (isset($_REQUEST[$filter_field]))
                    $rule_info[] = $_REQUEST[$filter_field];

		// echo "<br>iRule: ".$iRule." ";
		// echo "filter field ".$filter_field." Val=[".$_REQUEST[$filter_field]."]<BR>";
		$lines_field = "rule_lines_".$iRule;
                $rule_info[] = $_REQUEST[$lines_field];
		// echo " lines field ".$lines_field .  "  val=[".$_REQUEST[$lines_field]."]";
                if (trim($_REQUEST[$lines_field]) == "") $save_this_rule = false;

                if ($_REQUEST[$filter_field] == "replace") {
                    // if (isset($_REQUEST['rule_search_str_'.$iRule])) echo "Search=".$_REQUEST['rule_search_str_'.$iRule];
                    // if (isset($_REQUEST['rule_replace_str_'.$iRule])) echo " Rep=".$_REQUEST['rule_replace_str_'.$iRule];
                    if (isset($_REQUEST['rule_search_str_'.$iRule])) $rule_info[] = $_REQUEST['rule_search_str_'.$iRule];
                    if (isset($_REQUEST['rule_replace_str_'.$iRule])) $rule_info[] =$_REQUEST['rule_replace_str_'.$iRule];
                }
                if ($save_this_rule == true) $save_rules[] = implode(":",$rule_info);
	    }
	    if (count($save_rules) > 0) {
                $rule_str = implode(";",$save_rules);
	    }
            if ($rule_mode == "Add" && $_REQUEST['rule_count']==1) updateFixupRules($rule_site_id,$rule_obj_id,$rule_str, true);
	    else updateFixupRules($rule_site_id,$rule_obj_id,$rule_str, false);
	}
   }


   ?>
   <script type="text/javascript" language="Javascript">
         function setSaveMode(save_mode)
         {
            if(save_mode == 1) 
            {
               document.importfeeds_ruleform.fixup_rule.value = "add";
               document.importfeeds_ruleform.submit();
               // document.publishform.event_time_day.disabled=false;
               // document.publishform.event_time_day.style.display="";
             }
         }

         function updateFields(filter_value)
         {
            if(document.importfeeds_ruleform.filter_rule_1.value == "remove")
            {
               document.importfeeds_ruleform.rule_search_str_1.disabled=true;
               document.importfeeds_ruleform.rule_search_str_1.style.display="none";
               document.importfeeds_ruleform.rule_replace_str_1.disabled=true;
	       document.importfeeds_ruleform.rule_replace_str_1.style.display="none";
	    } else {
               document.importfeeds_ruleform.rule_search_str_1.disabled=false;
               document.importfeeds_ruleform.rule_search_str_1.style.display="";
               document.importfeeds_ruleform.rule_replace_str_1.disabled=false;
	       document.importfeeds_ruleform.rule_replace_str_1.style.display="";
            }
         }
   </script>
   
   <FORM name="importfeeds_ruleform" action="<?=$OSCAILT_SCRIPT?>" method="post">
   <input type=hidden name=fixup_rule value="<?=$_REQUEST['fixup_rule']?>">
   <input type=hidden name=key value="<?=$_REQUEST['key']?>">
   <br>
   <table align=center width="90%">
   <tr class=admin>
   <th class=admin colspan=5><br><?=$textLabels['preparse_title_part1']?> <u><?=$_REQUEST['feedname']?></u> <?=$textLabels['preparse_title_part2']?> <?=$rule_obj_id?> Site <?=$rule_site_id?><br><br><small> <?=$textLabels['preparse_title_part4']?> </small><br> &nbsp;
      </th>
   </tr>
   <?
   # Above text is <br>Pre-Parse Fixup Rules for Imported Feed: <u>_feedname_ </u> Object Id: Site <br><br><small>For 'Apply to Lines', enter line numbers separated by spaces. Search and Replace option only applicable to Replace mode</small><br> 
   # Generate the form selections ... would it be more efficient to print each selection or
   # as here generate fulls string and then print? Probably sub-millisecond difference anyhow.
   ?>
   <tr class=admin>
      <th class=admin>&nbsp;#&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['rule_label']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['apply_to_line_label']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['search_string_label']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['replacement_str_label']?>&nbsp;</th>
   </tr>
   <?

   // Get the rules and the one to be edited.
   $rulesSet = getFixupRules();
   $ruleKeys = array_keys($rulesSet);
   /*
   } else {
       $rulesSet = array($rule_key => "");
       $ruleKeys = array_keys($rulesSet);
   }
    */

   $counter = 1;
   //$rule_action = "replace";
   // echo "<br>Writing rules <br>";
   // Since $rulesSet is the set of all rules for all feeds, we just want the ones for this feed.
   if (array_key_exists($rule_key, $rulesSet)) 
   {
       $this_rulesSet = $rulesSet[$rule_key];
       foreach($this_rulesSet as $subRuleSet)
       {
              foreach($subRuleSet as $rule_action => $each_target)
              {
              // echo "<BR>Processing ".$rule_action. " on ".$each_target."<BR>";
              $rule_search_str = $rule_replace_str = "";

              if (is_array($each_target)) {
        	  // Use reset if you print the contents of the array here.
        	  // foreach($each_rule as $kk => $dd) { echo "key ".$kk. " val ".$dd."<BR>";}
                      $rule_lines = $each_target[0];
                      $rule_search_str = $each_target[1];
                      $rule_replace_str = $each_target[2];
              } else {
                  $rule_lines = $each_target;
              }
        
              $rule_select = getRulesList($rule_action,$counter);
              displayRule($counter, $rule_select, $rule_lines, $rule_search_str, $rule_replace_str);
              $counter++;
              }
       } 
   } 
   if ($rule_mode == "Add") {
      $rule_select = getRulesList("remove",$counter);
      displayRule($counter, $rule_select, "", "", "");
      $counter++;
   }
   ?>
      <tr class=admin>
      <td class=admin>&nbsp;<?=$counter?>&nbsp;</td>
      <td class=admin colspan=4><input type=submit name=add_rule_<?=$counter?> value="<?=$textLabels['add_rule_btn_label']?> &gt;&gt;" onClick="setSaveMode(1)"></td>
      </tr>
      <tr class=admin>
      <td class=admin colspan=5 align=center><BR> 
      <input type=hidden name=rule_count value="<?=($counter-1)?>"> 
	  <input type=submit name=cancel_rules value="<?=$textLabels['cancel_btn_label']?>"> &nbsp;
	  <input type=submit name=save_rules   value="<?=$textLabels['save_rules_btn_label']?>">
	  <input type=submit name=remove_rules value="<?=$textLabels['remove_all_btn_label']?>">
	  <br><small><?=$textLabels['rule_btns_subtext']?> </small>
      </td>
      </tr>
   </table>
   </FORM>
   <br><br>
   <?

}
function displayRule($counter, $rule_select, $rule_lines, $rule_search_str, $rule_replace_str)
{
      ?>
      <tr class=admin>
      <td class=admin>&nbsp;<?=$counter?>&nbsp;</td>
      <td class=admin><?=$rule_select?></td>
      <td class=admin><input type=text name=rule_lines_<?=$counter?> value="<?=$rule_lines?>" ></td>
      <td class=admin><input type=text name=rule_search_str_<?=$counter?> size=25 value="<?=$rule_search_str?>" ></td>
      <td class=admin><input type=text name=rule_replace_str_<?=$counter?> size=25 value="<?=$rule_replace_str?>" ></td>
      </tr>
      <?
}
function getRulesList($RuleSelection, $RowId)
{
   $list_string ="<select name='filter_rule_".$RowId."' onChange='updateFields()'> ";
   $rulesList = array("remove" => "Remove Lines", "replace" => "Replace Text on Line(s)");
   foreach ( $rulesList as $t_value => $t_string)
   {
       if ($t_value == $RuleSelection)
       {
            $list_string .= "<option selected value=\"" . $t_value ."\">" . $t_string . "</option>";
       }
       else
       {
            $list_string .= "<option value=\"" . $t_value ."\">" . $t_string . "</option>";
       }
   }
   $list_string .= "</select>";

   return $list_string;
}
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
function resetImportFeedsRetryCount($site_id=0, $obj_id=0, $cache_filename=null)
{
    global $dbconn, $prefix, $system_config, $path_prefix;

    $sql_str1 = "UPDATE ".$prefix."_importfeeds_status SET retries=0 WHERE retries > 0 ";

    if ($site_id > 0 && $obj_id > 0) {
    	$sql_str1 .= "AND site_id=".$site_id. " AND object_id=".$obj_id;
    }
    $result_rss = sql_query($sql_str1, $dbconn, 2);
    checkForError($result_rss);
    if ($cache_filename != null) {
	    if (file_exists($path_prefix.$system_config->rss_cache."/".$cache_filename))
	        unlink($path_prefix.$system_config->rss_cache."/".$cache_filename);
    }
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

// loadManagedObjectSet();

if($editor_session->isSessionOpen())
{
   writeAdminHeader("viewsitelog.php?log_type=rssfeeds","View Bad RSS File",
   array("clearcache.php" => "Clear Cache", "editredirects.php" => "Edit Friendly URLs"));


   if($editor_session->editor->allowedReadAccessTo("editdataobjects"))
   {
      if (!isset($_REQUEST['fixup_rule'])) writeLocalAdminHeader();

      if( loadManagedObjectSet() == true ) {
         $sort_mode = 'none';
         if ( isset($_REQUEST['sort']))
         {
             $sort_mode = $_REQUEST['sort'];
         }
	 if ( isset($_REQUEST['fixup_rule']) && !isset($_REQUEST['cancel_rules'])) {
            // Display the Add / Edit screen for the fixup rules
            editFeedRules($_REQUEST['fixup_rule']);
	 } else {
   	 // If resetting the counts do it now, then call the display and read the data from the db.
            if ( isset($_REQUEST['reset_retries'])) 
   	    {
                resetImportFeedsRetryCount();
   	    }
   	    else if ( isset($_REQUEST['single_reset'])) 
   	    {
                if ( isset($_REQUEST['reset_site']) && isset($_REQUEST['reset_obj'])) {
                    if ( isset($_REQUEST['reset_cachefile']) )
                        resetImportFeedsRetryCount($_REQUEST['reset_site'], $_REQUEST['reset_obj'], $_REQUEST['reset_cachefile']);
		    else
                        resetImportFeedsRetryCount($_REQUEST['reset_site'], $_REQUEST['reset_obj']);
   	        }
   	    }
            writeFeedsObjectList($sort_mode);
	 }
      }
   }
   else $editor_session->writeNoReadPermissionError();
}
else $editor_session->writeNoSessionError();

include_once("adminfooter.inc");
?>
