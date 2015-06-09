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

require_once("oscailt_init.inc");
require_once("objects/publishmonitor.inc");
require_once("objects/bannedip.inc");
require_once("objects/indyobjects/indydataobjects.inc");
require_once("objects/adminutilities.inc");
define ('NO_SORT', 0);
define ('SORT_IP', 1);
define ('SORT_BR', 2);

$OSCAILT_SCRIPT = "editmonitor.php";

$textLabels = array("title" => "Monitor Publishing IP addresses",
	            "hour_word" => "hour",
	            "hours_word" => "hours",
	            "days_word" => "days",
	            "are_you_sure_stop_monitoring_msg" => "Are you sure that you want to stop IP address monitoring of users publications?  This will mean that you will not be able to block any attackers from publishing",
	            "are_you_sure_start_monitoring_msg" => "Are you sure that you want to start IP address monitoring of users publications?  This will mean that you will have some data which may be potentially interesting to the authorities",
	            "select_duration_msg" => "Select the duration for monitoring:",
	            "enter_reason_msg" => "Enter Reason for monitoring:",
	            "cancel_btn" => "&lt;&lt; Cancel",
	            "confirm_btn" => "Confirm &gt;&gt;",
	            "ip_turned_off_text" => "IP Publishing Monitor Turned Off and cache emptied!",
	            "ip_turned_on_text" => "IP Publishing Monitor Turned On!",
	            "log_turn_off_text" => "Turned off",
	            "log_turn_on_text" => "Turned on",
	            "ip_turned_on_by_text" => "IP Monitor turned on by user:",
	            "ip_turned_off_by_text" => "IP Monitor turned off by user:",
	            "period_extend_text" => "Period extended for",
	            "period_is_text" => "Period is for",
	            "period_extend_msg" => "IP Publishing Monitor Period Extended for",
		    "turned_on_msg" => "IP Publishing Monitor Turned On!",
	            "disable_btn" => "Disable Monitor and Purge Data",
	            "enable_btn" => "Enable IP Monitor",
	            "turned_on_at_text" => "Turned on at:",
	            "remaining_word" => "Remaining:",
	            "turned_prev_text" => "Previously turned on at:",
	            "switch_off_info_1" => "This should have been switch off",
	            "switch_off_info_2" => "This will automatically switch off on next access.",
	            "pub_monitor_heading" => "Publishing Monitor",
	            "change_monitor_length_heading" => "Change Monitor Length",
	            "ip_address_heading" => "IP Address",
	            "browser_heading" => "Browser Info",
		    "publish_url_heading" => "Publish URL",
		    "publish_time_heading" => "Publish Time",
		    "allowed_publish_heading" => "Allowed Publish",
		    "change_ban_heading" => "Change Ban Status",
		    "no_valid_addr_input_msg" => "No valid IP Address was input!",
	            );

$textObj = new indyItemSet();

if($textObj->load($system_config->xml_store, "editmonitor") === false)
{
    $textObj->setUserMessage( array( USER_ERROR, "Failed to get text strings for the Edit Monitor. -Using defaults",""));
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

addToPageTitle($textLabels['title']);


function getMonLengthSelect()
{
   $str = "<select name='mon_period'>";
   $opts = array(1 => "1 hour", 2 => "2 hours", 3 => "3 hours", 4 => "4 hours", 5 => "5 hours", 6 => "6 hours", 8 => "8 hours", 12 => "12 hours", 15 => "15 hours", 18 => "18 hours", 21 => "21 hours", 24 => "24 hours", 36 => "36 hours", 48 => "2 days", 60 => "2.5 days", 72 => "3 days");
   foreach(array_keys($opts) as $k)
   {
      $str .= "<option value='$k'>".$opts[$k]."</option>\n";
   }
   $str .= "</select>";
   return $str;
}


function writeConfirmToggleBox($extend_mode=false)
{
   global $system_config, $OSCAILT_SCRIPT, $textLabels;

   if($system_config->publish_monitor_enabled && $extend_mode == false)
   {
      // $txt = "Are you sure that you want to stop IP address monitoring of users publications?  This will mean that you will not be able to block any attackers from publishing";
      $txt = $textLabels['are_you_sure_stop_monitoring_msg'];
      $extra_form_str="";
   }
   else
   {
      // $txt = "Are you sure that you want to start IP address monitoring of users publications?  This will mean that you will have some data which may be potentially interesting to the authorities";
      $txt = $textLabels['are_you_sure_start_monitoring_msg'];
      $extra_form_str = "<P>".$textLabels['select_duration_msg']." ". getMonLengthSelect()."</P>";
      $extra_form_str .= "<P align='center'>".$textLabels['enter_reason_msg']. " <br /><TEXTAREA rows=5 cols=30 name='mon_reason'></TEXTAREA></P>";

   }
   ?>
   <table align=center>
   <form action="<?=$OSCAILT_SCRIPT?>" method="post">
   <input type="hidden" name="subpage" value="toggle_monitor_status">
   <?

   if($extend_mode == true) {
      ?>
      <input type="hidden" name="extend" value="true">
      <?
   }

   ?>
   <tr>
      <td colspan=2 align=center><img src="graphics/caution.gif" align=middle><BR><BR><B><?=htmlspecialchars($txt, ENT_QUOTES)?></B><BR><BR>
      <?=$extra_form_str?></td>
   </tr>
   <tr>
   <td align=right><input type=submit name=cancel value="<?=$textLabels['cancel_btn']?>"></td>
      <td><input type=submit name=confirm value="<?=$textLabels['confirm_btn']?>"></td>
   </tr>
   </form>
   </table>
   <?

}

function writeLocalAdminHeader()
{
   global $OSCAILT_SCRIPT;

   if ( isset($_REQUEST['sort']) && $_REQUEST['sort'] == 'true')
   {
      $sort_url_mode = "sort=false";
      $sort_cur_mode = "sort=true";
      $sort_url_msg = "Sort By Time Posted";
   } else {
      $sort_url_mode = "sort=true";
      $sort_cur_mode = "sort=false";
      $sort_url_msg = "Sort By IP";
   }

   if ( isset($_REQUEST['absolute']) && $_REQUEST['absolute'] == 'true')
   {
      $time_url_mode = $sort_cur_mode . "&absolute=false";
      $time_url_msg = "Relative Time";
   } else {
      $time_url_mode = $sort_cur_mode. "&absolute=true";
      $time_url_msg = "Display Publish Timestamp";
   }
/*
   if ($sort_url_mode == "true") $time_url_mode .= "&sort=false";
   else $time_url_mode .= "&sort=true";
*/


   if ( isset($_REQUEST['showhost']) && $_REQUEST['showhost'] == 'true') {
       $swap_mode ="Hide Hostname";
       $switch_mode ="&showhost=false";
       $show_host_name ="&showhost=true";
   } else  {
       $swap_mode ="Show Hostname";
       $switch_mode ="&showhost=true";
       $show_host_name ="&showhost=false";
   }

   if ( isset($_REQUEST['showbrowser']) && $_REQUEST['showbrowser'] == 'true') {
       $br_swap_mode ="Hide Browser Info";
       $br_switch_mode ="&showbrowser=false";
       $show_browser ="&showbrowser=true";
   } else  {
       $br_swap_mode ="Show Browser Info";
       $br_switch_mode ="&showbrowser=true";
       $show_browser ="&showbrowser=false";
   }
   $br_switch_mode .= $show_host_name;
   $switch_mode .= $show_browser;


   ?>
     <TABLE class='admin'>
	<TR class='admin'><TD class='admin'>
        <a href="<?=$OSCAILT_SCRIPT?>?<?=$sort_url_mode?><?=$show_host_name?><?=$show_browser?>"><?=$sort_url_msg?></a> |
        <a href="<?=$OSCAILT_SCRIPT?>?<?=$time_url_mode?><?=$show_host_name?><?=$show_browser?>"><?=$time_url_msg?></a> |
        <a href="<?=$OSCAILT_SCRIPT?>?<?=$time_url_mode?><?=$switch_mode?>"><?=$swap_mode?></a> |
        <a href="<?=$OSCAILT_SCRIPT?>?<?=$time_url_mode?><?=$br_switch_mode?>"><?=$br_swap_mode?></a> </TD></TR>
     </TABLE>
   <?
}

function toggleMonitorStatus($extend_mode=false)
{
   global $system_config, $monitorList, $editor_session, $textLabels;
   if($system_config->publish_monitor_enabled && $extend_mode == false)
   {
      $monitorList->recent_publishes = array();
      $monitorList->save();
      $system_config->publish_monitor_enabled = false;
      $system_config->save();

      logAction("", "N/A", "IP-Monitor", "Turned off");
      //writeError("IP Publishing Monitor Turned Off and cache emptied!");
      writeError($textLabels['ip_turned_off_text']);

      // Also report it to the Oscailt messaging system.
      $editorStatusList = new PublishState();
      $editorStatusList->load();
      // $editorStatusList->add($editor_session->editor->editor_name, "post", time(),"IP Monitor turned off");
      $editorStatusList->add("system", "post", time(),"IP Monitor turned off by user: ".$editor_session->editor->editor_name);
      $editorStatusList->save();
   }
   else
   {
      $time_limit = cleanseNumericalQueryField($_REQUEST['mon_period']);
      if($time_limit <= 0) $time_limit = 1;
      $system_config->publish_monitor_due_off = $time_limit * 3600;

      if ($_REQUEST['mon_reason'] == "") $_REQUEST['action_reason'] = "<not specified>";
      else $_REQUEST['action_reason'] = $_REQUEST['mon_reason'];

      $monitor_period = getTimeAgoStr($system_config->publish_monitor_due_off, false);
      if ($extend_mode == true)
         $_REQUEST['action_reason'] = "Period extended for " . $monitor_period . ". " . $_REQUEST['action_reason'];
      else
         $_REQUEST['action_reason'] = "Period is for " . $monitor_period . ". " . $_REQUEST['action_reason'];

      $system_config->publish_monitor_enabled = true;
      $system_config->publish_monitor_began = time();
      $system_config->save();

      logAction("", "N/A", "IP-Monitor", "Turned on");

      if ($extend_mode == true) {
         // IP Publishing Monitor Period Extended for ". $monitor_period
         writeError($textLabels['period_extend_msg']." ". $monitor_period);
      } else {
         // writeError("IP Publishing Monitor Turned On!");
         writeError($textLabels['ip_turned_on_text']);
      }

      // Also report it to the Oscailt messaging system.
      $editorStatusList = new PublishState();
      $editorStatusList->load();
      // $editorStatusList->add($editor_session->editor->editor_name, "post", time(),"IP Monitor turned off");
      $editorStatusList->add("system", "post", time(),"IP Monitor turned on by user: ".$editor_session->editor->editor_name." " .$_REQUEST['action_reason']);
      $editorStatusList->save();
   }
}

// This is the exact same as the function in EditorStatus.php Both need to be
// moved to utilities.inc
// This will return a string for an input time difference and format it as
// x hrs y min z secs ago
function getTimeAgoStr($time_diff, $add_ago=true)
{

   if ($time_diff < 60 )
   {
       $t_str = $time_diff . " secs";
   }
   elseif ($time_diff < 3600 )
   {
       // Less than one hour ago.
       $t_min = (int) $time_diff / 60;
       $t_min = floor($t_min);
       $t_sec = $time_diff - ( (int) (60 * $t_min));
       $t_str = $t_min . " mins";
       if ($t_sec > 0 ) $t_str .= " " . $t_sec . " secs";
   }
   elseif ($time_diff < 86400 )
   {
       // Less than one day ago.
       $t_hr = (int) $time_diff / 3600;
       $t_hr = floor($t_hr);
       $time_diff_left = $time_diff - ($t_hr * 3600);

       $t_min = (int) $time_diff_left / 60;
       $t_min = floor($t_min);
       $t_sec = $time_diff_left - (60 * $t_min);
       $t_str = $t_hr . " hrs" ;
       // $t_str = $t_hr . " hrs " . $t_min . " mins " . $t_sec . " secs";
       if ($t_min > 0 ) $t_str .= " " . $t_min . " mins";
       if ($t_sec > 0 ) $t_str .= " " . $t_sec . " secs";
   }
   elseif ($time_diff == 86400 )
   {
       $t_str = "24 hrs";
   }
   else
   {
       $t_day = (int) $time_diff / 86400;
       // For less than 5 days show the fraction
       if ($t_day < 5 ) 
         $t_day = floor(10 * $t_day)/10;
       else
         $t_day = floor($t_day);

       $t_str = $t_day . " days";
   }
   if ($add_ago == true) $t_str .= " ago";

   return $t_str;
}

function writeMonitorList( $sort_type=NO_SORT, $relative_time=true, $show_host=false)
{
   global $monitorList, $ipBanList, $hostBanList, $system_config, $OSCAILT_SCRIPT;
   global $textLabels;
   $current_time = time();
   $show_warning_note = false;
   $warning_note = "Do not copy any of this information";
   $warning_repeat = 3;

   $num_cols = 5;
   $show_browser_info = false;
   if ( isset($_REQUEST['showbrowser']) && $_REQUEST['showbrowser'] == 'true') {
       $show_browser_info = true;
   }
   if ($show_browser_info == true) $num_cols = 6;

   //if ($show_host == true) $num_cols = 6;

   if($system_config->publish_monitor_enabled)
   {
      $button = "<input type='submit' name='action' value='".$textLabels['disable_btn']." >>'>";
      $monitor_state = "On";
   }
   else
   {
      $button = "<input type='submit' name='action' value='".$textLabels['enable_btn']." >>'>";
      $monitor_state = "Off";
   }
   $begin_txt =  strftime("%a %d %b %H:%M:%S",$system_config->publish_monitor_began+$system_config->timezone_offset);
   $timelen_txt = getTimeAgoStr($system_config->publish_monitor_due_off, false);
   $time_diff = (($system_config->publish_monitor_began + $system_config->publish_monitor_due_off) - $current_time);

   if ( isset($_REQUEST['showme']) && $_REQUEST['showme'] == 'true') {
     echo "<b>My IP Addr:</b> ".$_SERVER['REMOTE_ADDR'] . "<BR>";
     echo "<b>My IP Host:</b> ".gethostbyaddr($_SERVER['REMOTE_ADDR']) . "<BR>";
     echo "<b>My Browser Info:</b> ".$_SERVER['HTTP_USER_AGENT'] . "<BR>";
   }

   ?>
   <table align=center>
   <tr class=admin>
      <th class=admin colspan="<?=($num_cols-1)?>"><?=$textLabels['pub_monitor_heading']?> <?=$monitor_state?></th>
      <th class=admin width="15%"><?=$textLabels['change_monitor_length_heading']?></th>
   </tr>
   <tr class=admin>
   <?
   if($system_config->publish_monitor_enabled)
   {
      // Check the time is not overdue and if so give warning
      if ($time_diff < 0 ) {
          $time_diff = -$time_diff;
          $remain_txt = getTimeAgoStr($time_diff, true);
          ?>
	  <td class=admin colspan="<?=($num_cols-1)?>"><?=$textLabels['turned_on_at_text']?> <b><?=$begin_txt?></b> for <b><?=$timelen_txt?></b> </td>
          <td class=admin align=center width="15%"> <a href='editmonitor.php?subpage=toggle_monitor_status&amp;extend=true'><img src='graphics/unhide.gif' /></a></td>
          </tr>
          <tr class=admin>
          <td class=error colspan="<?=$num_cols?>"><?=$textLabels['switch_off_info_1']?> <b><?=$remain_txt?></b><BR><?=$textLabels['switch_off_info_2']?></td>
          <?
      } else {
          $remain_txt = getTimeAgoStr($time_diff, false);
          ?>
	  <td class=admin colspan="<?=($num_cols-1)?>"><?=$textLabels['turned_on_at_text']?> <b><?=$begin_txt?></b> for <b><?=$timelen_txt?></b> <?=$textLabels['remaining_word']?> <b><?=$remain_txt?></b></td>
          <td class=admin align=center width="15%"> <a href='editmonitor.php?subpage=toggle_monitor_status&amp;extend=true'><img src='graphics/unhide.gif' /></a></td>
          <?
      }
   }
   else
   {
      ?>
      <td class=admin colspan="<?=$num_cols?>"><?=$textLabels['turned_prev_text']?> <b><?=$begin_txt?></b> for <b><?=$timelen_txt?></b></td>
      <?
   }
   ?>
   </tr>
   <tr class=admin>
      <th class=admin>&nbsp;<?=$textLabels['ip_address_heading']?>&nbsp;</th>
      <?
      if($show_browser_info == true )
      {
          ?> <th class=admin>&nbsp;<?=$textLabels['browser_heading']?>&nbsp;</th> <?
      }
      ?>
      <th class=admin>&nbsp;<?=$textLabels['publish_url_heading']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['publish_time_heading']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['allowed_publish_heading']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['change_ban_heading']?>&nbsp;</th>
   </tr>
   <?
   // Store the IP and time in array for sorting. It will then be discarded as we
   // can just use the arr_order to track the changes.
   for($i=0; $i < count($monitorList->recent_publishes);$i++)
   {
      if ($sort_type == SORT_IP)
      {
         $r=$monitorList->recent_publishes[$i];
         $arr_ip[] = $r->ip;
         $arr_time[] = $r->time_posted;
      } 
      else if ($sort_type == SORT_BR)
      {
         $r=$monitorList->recent_publishes[$i];
         $arr_ip[] = $r->browser_info;
         $arr_time[] = $r->time_posted;
      } 
      $arr_order[] = $i;
   }
   // Sort by IP and posted time. And track the index reshuffle. For details on sort
   // lookup the PHP multi-array sorts.
   if (($sort_type == SORT_IP || $sort_type == SORT_BR) && count($monitorList->recent_publishes) > 0)
   {
      array_multisort($arr_ip, $arr_time, $arr_order);
   }

   $class_types = array("admin","admin_fade");
   $admin_cls=$class_types[0];
   $last_ip ="0.0.0.0";
   $cls_ptr = 0;

   for($i=0;$i<count($monitorList->recent_publishes);$i++)
   {
      $sorted_index=$arr_order[$i];
      $r=$monitorList->recent_publishes[$sorted_index];
      $ip = $r->ip;
      $ip_text = "&nbsp;" .$ip."&nbsp;";
      if ($show_host == true) $ip_text .= "<br>". $r->host_of_ip;

      if ($sort_type == SORT_IP || $sort_type == SORT_BR)
      {
         if ($last_ip != $ip) 
	 {
            $admin_cls= $class_types[$cls_ptr];
	    $cls_ptr++;
	    if ($cls_ptr > 1 ) $cls_ptr =  0;
         }
         $last_ip = $ip;
      }

      if ($relative_time == true ) 
      {
         $t_str = getTimeAgoStr(($current_time - $r->time_posted));
      } else {
         $t_str = strftime("%H:%M:%S, %a %d %b", ($r->time_posted+$system_config->timezone_offset));
      }

      if($ipBanList->isBanned($ip))
      {
         $al = "<img src='graphics/inactive.gif'>";
         $ban = "<a href='$OSCAILT_SCRIPT"."?subpage=unban&amp;target_ip=".$ip."' title='unban'> <img src='graphics/delete.gif' border=0></a>";
      }
      else if($hostBanList->isBanned($r->host_of_ip, $r->browser_info))
      {
         if($hostBanList->getBanType($r->host_of_ip, $r->browser_info) == 1) $al = "<img src='graphics/inactive.gif'>";
	 else $al = "<img src='graphics/hide.gif'>";

         $ban = "<a href='$OSCAILT_SCRIPT"."?hostsubpage=unban&amp;target_host=".$r->host_of_ip."&amp;target_browser=".$r->browser_info."' title='unban'> <img src='graphics/delete.gif' border=0></a>";
      }
      else
      {
         $al = "<img src='graphics/active.gif'>";
         $ban = "<a href='$OSCAILT_SCRIPT"."?subpage=ban&amp;target_ip="."$ip' title='ban'> <img src='graphics/hide.gif' border=0></a>";
      }
      if ($i % $warning_repeat == 0 && $show_warning_note == true)
      {
          ?>
	  <tr class=admin> <td class=admin align=center colspan=<?=$num_cols?>><b><?=$warning_note?></b></td>
          </tr>
          <?
      }
      ?>
      <tr class=admin>
         <td class=<?=$admin_cls?> align=center><?=$ip_text?></td>
         <?
         if($show_browser_info == true )
         {
            ?> <td class=admin><?=$r->browser_info?></td> <?
         }
         ?>
         <td class=admin align=left  >&nbsp;<?=$r->url?>&nbsp;</td>
         <td class=admin align=center><?=$t_str?></td>
         <td class=admin align=center><?=$al?></td>
         <td class=admin align=center><?=$ban?></td>
        </tr>
        <?
   }
   ?>
    <tr class=admin>
      <td class=admin align=center colspan="<?=$num_cols?>">
      <form action="<?=$OSCAILT_SCRIPT?>" method="POST">
      <input type='hidden' name='subpage' value='toggle_monitor_status'>
      <?=$button?>
      </form>
      </td>
    </tr>
   </table>
   <?
}


ob_start();
// $admin_table_width = "90%";


if($editor_session->isSessionOpen())
{
   if ( isset($_REQUEST['showhost']) && $_REQUEST['showhost'] == 'true') $admin_table_width = "90%";
   if ( isset($_REQUEST['showme']) && $_REQUEST['showme'] == 'true') $admin_table_width = "90%";

   // Add a link to the block IP screen
   writeAdminHeader("editbannedips.php","Block IP Addresses and Sub Hosts", array("editbannedauthors.php" => "Block Authors"));
   writeLocalAdminHeader();
   $sort_mode = NO_SORT;
   $relative_time = true;
   if ( isset($_REQUEST['absolute']) && $_REQUEST['absolute'] == 'true')
   {
      $relative_time = false;
   }
   if ( isset($_REQUEST['sort']) && $_REQUEST['sort'] == 'true')
   {
      $sort_mode = SORT_IP;
   } 
   else if ( isset($_REQUEST['sort']) && $_REQUEST['sort'] == 'brow')
   {
      $sort_mode = SORT_BR;
   } 

   // The code here has to handle a number of states which are both displaying and then confirming on
   // the confirm box for IP ban length changes, turning on the IP monitor and extended the monitor period
   if($editor_session->editor->allowedReadAccessTo("editmonitor"))
   {
      $monitorList = new PublishMonitor();
      $ipBanList = new BannedIPList();
      $hostBanList = new BannedHostList();
      $monitorList->load();
      $ipBanList->load();
      $hostBanList->load();
      if(isset($_REQUEST['subpage']) && $_REQUEST['subpage'] == 'toggle_monitor_status')
      {
         if(isset($_REQUEST['confirm']))
         {
            if($editor_session->editor->allowedWriteAccessTo("editmonitor"))
            {
               if(isset($_REQUEST['extend']) && $_REQUEST['extend'] == 'true')
                  toggleMonitorStatus(true);
               else
                  toggleMonitorStatus();

               writeMonitorList($sort_mode, $relative_time);
            }
            else
            {
               $editor_session->writeNoWritePermissionError();
            }
         }
         else
         {
            if(isset($_REQUEST['extend']) && $_REQUEST['extend'] == 'true')
               writeConfirmToggleBox(true);
            else
               writeConfirmToggleBox();
         }
      }
      elseif(isset($_REQUEST['subpage']) && isset($_REQUEST['confirm']))
      {
         $target_ip = cleanseIP($_REQUEST['target_ip']);
         if($target_ip === false)
         {
            // writeError("No valid IP Address was input!");
            writeError($textLabels['no_valid_addr_input_msg']);
            writeMonitorList($sort_mode, $relative_time);
            return;
         }
         if($editor_session->editor->allowedWriteAccessTo("editbannedips"))
         {
            if($_REQUEST['subpage']=="unban")
            {
               $ipBanList->unbanIP($target_ip);
       	       // $logMsg = $editor_session->editor->editor_name.":\tunban:\tIP:\t" . $target_ip . ":\tban lifted.";
               logAction(null, $target_ip, "IP", "unban", "ban lifted"); 
	    } 
            else
            {
               $time_limit = cleanseNumericalQueryField($_REQUEST['ban_period']);
               if($time_limit <= 0)
               {
                  $time_limit = 10000 * 24;
               }
               $ban_end = time() + ($time_limit * 60 * 60);
               $r = trim($_REQUEST['ban_reason']);

               if($r == "") $r = "<not specified>";

       	       # If Ban begins not found then it is probably a new ban..
	       if (strstr($r, "Ban begins") != true )
               {
                  $r = $editor_session->editor->editor_name.": ". "(Ban begins: " . strftime($system_config->default_strftime_format, time() + $system_config->timezone_offset) .") " . $r;
               }
	       else
               {
                  $r = $editor_session->editor->editor_name.": ". $r;
               }

               $ipBanList->banIP($target_ip, $ban_end, $r);
	       //$logMsg = $editor_session->editor->editor_name.":\tban:\tIP:\t" . $target_ip . ":\t " .$r;
               logAction(null, $target_ip, "IP", "ban", $r); 
               $ipBanList->notifyByEmail($target_ip, $time_limit, $r);
            }
         }
         else
         {
            $editor_session->writeNoWritePermissionError();
         }
         writeMonitorList($sort_mode, $relative_time);
      }
      else if(isset($_REQUEST['subpage']) && isset($_REQUEST['cancel']))
      {
         writeMonitorList($sort_mode, $relative_time);
      }
      else if(isset($_REQUEST['subpage']))
      {
	 if (isset($_REQUEST['reason']) ) writeConfirmBanBox($_REQUEST['reason']);
	 else writeConfirmBanBox("");
      }
      elseif(isset($_REQUEST['hostsubpage']) && isset($_REQUEST['confirm']))
      {
         if($editor_session->editor->allowedWriteAccessTo("editbannedips"))
         {
      	    if($_REQUEST['hostsubpage']=="unban")
            {
               if (isset($_REQUEST['target_host']) && isset($_REQUEST['target_browser']) && strlen($_REQUEST['target_host']) > 0 && strlen($_REQUEST['target_browser']) > 0 )
               {
               	   $hostBanList->unbanHost($_REQUEST['target_host'], $_REQUEST['target_browser']);
	           logAction(null, $_REQUEST['target_host'], "Hostname", "unban", "ban lifted");
	       }
            }
         }
         else
         {
            $editor_session->writeNoWritePermissionError();
         }
         writeMonitorList($sort_mode, $relative_time);
      }
      else if(isset($_REQUEST['hostsubpage']))
      {
         writeConfirmBanHostBox();
      }
      else
      {
         if(isset($_REQUEST['showhost']) && $_REQUEST['showhost']=='true' )
             writeMonitorList($sort_mode, $relative_time, true);
         else
             writeMonitorList($sort_mode, $relative_time);
      }
   }
   else $editor_session->writeNoReadPermissionError();
}
else $editor_session->writeNoSessionError();

include_once("adminfooter.inc");
?>
