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
require_once("objects/bannedip.inc");
require_once("objects/adminutilities.inc");
require_once("objects/indyobjects/indyobject.inc");
require_once("objects/indyobjects/indyitemset.inc");

$OSCAILT_SCRIPT = "editbannedips.php";

define ('BANTYPE_BAN', 1);
define ('BANTYPE_MODERATION', 2);

$textLabels = array("title" => "Manage Blocked IP addresses",
	            "banned_ip_subtitle" => "Banned IPs (publish and contact form bans) &amp;nbsp;  Total ",
	            "ip_addr_label" => "IP Address",
	            "ban_began_label" => "Ban Began",
	            "ban_expires_label" => "Ban Expires",
	            "ban_reason_label" => "Ban Reason",
	            "lift_ban_label" => "Lift Ban",
	            "change_ban_length_label" => "Change Ban Length",
	            "ban_ip_btn_label" => "Ban IP",
	            "ban_subhost_browser_subtitle" => "Banned Sub-Hosts and Browser Types Combinations (publish form bans) &amp;nbsp; Total = ",
	            "sub_hostname_browser_label" => "Sub Hostname &amp; &amp;nbsp; Browser",
	            "ban_type_label" => "Ban Type",
	            "expired_word" => "Expired",
	            "ban_sub_host_browser_type_btn_label" => "Ban Sub Hostname + Browser Type",
	            "ban_sub_host_browser_type_btn_subtext" => "See Monitor Publishers for hostnames and browsers",
	            "sub_hostname_word" => "Sub Hostname",
	            "put_on_moderation_label" => "Put on Moderation",
	            "ban_posts_label" => "Ban Posts",
	            "browser_type_label" => "Browser Type",
	            "note_word" => "Note:",
	            "target_browser_subtext" => "You should use part of a hostname and the full browser description. These bans may block legitimate users so use short ban periods and try to select moderation instead.",
	            "no_valid_ip_addr_input_text" => "No valid IP Address was input!",
	            "no_valid_sub_hostname_input_text" => "No valid Sub Hostname Address was input!",
	            "no_valid_browser_type_input_text" => "No valid browser type was input!",
	            ); 

$textObj = new indyItemSet();
$system_config->user_error_reporting=8;
if($textObj->load($system_config->xml_store, "editbannedips") === false)
{
    $textObj->setUserMessage( array( USER_ERROR, "Failed to get text strings for the Edit Banned IPs. -Using defaults",""));
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

function writeBannedIPList()
{
   global $ipBanList, $system_config, $textLabels;
   $timeMsg = strftime($system_config->default_strftime_format, time() + $system_config->timezone_offset);
   ?>
   <table align=center>
   <tr class=admin>
      <th class=admin colspan="6"><?=$textLabels['banned_ip_subtitle']?> = <?=count($ipBanList->banned_list)?>
      <br>&nbsp; <?=$timeMsg?>
      </td>
   </tr>
   <tr class=admin>
      <th class=admin>&nbsp;<?=$textLabels['ip_addr_label']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['ban_began_label']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['ban_expires_label']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['ban_reason_label']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['lift_ban_label']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['change_ban_length_label']?>&nbsp;</th>
   </tr>
   <?
      foreach($ipBanList->banned_list as $one_ban)
      {
         $ip = $one_ban->ip;
         $reason = htmlspecialchars($one_ban->reason);
	 // The system offset time has already been added to the ban begin time.
         $ban_began = strftime($system_config->default_strftime_format, $one_ban->begin_ban);
         $expires = strftime($system_config->default_strftime_format, $one_ban->time_limit + $system_config->timezone_offset);
         $lift_str = "<a href='editbannedips.php?subpage=unban&amp;target_ip=$ip'><img src='graphics/delete.gif' /></a>";
         $extend_str = "<a href='editbannedips.php?subpage=ban&amp;target_ip=$ip'><img src='graphics/unhide.gif' /></a>";
	 if (time() > ($one_ban->time_limit + $system_config->timezone_offset))
         {
             $expires = "<b>".$textLabels['expired_word'].": </b>" . $expires;
             // $ip = "<i>" . $ip ."</i>";
         }
         ?>
         <tr class=admin>
            <td class=admin align=center>&nbsp;<?=$ip?>&nbsp;</td>
            <td class=admin align=right ><?=$ban_began?></td>
            <td class=admin align=right ><?=$expires?></td>
            <td class=admin align=center>&nbsp;<?=$reason?>&nbsp;</td>
            <td class=admin align=center>&nbsp;<?=$lift_str?>&nbsp;</td>
            <td class=admin align=center>&nbsp;<?=$extend_str?>&nbsp;</td>
         </tr>
         <?
      }
      ?>
      <tr>
      <td colspan="6" align="center">
         <form name="ban_ip_addr" action="editbannedips.php" method=post>
         <input type="hidden" name="subpage" value="ban">
	 <input type="submit" name="go" value="<?=$textLabels['ban_ip_btn_label']?>">
         <input type="text" name="target_ip" value=""> &nbsp; 
         </form>
      </td>
      </tr>
   </table>
   <?
   //    Put on Moderation <input type=radio name="target_ban_type" value="2" checked> &nbsp; Ban Posts <input type=radio name="target_ban_type" value="1">
}

function writeBannedSubHostList()
{
   global $hostBanList, $system_config, $textLabels;
   $timeMsg = strftime($system_config->default_strftime_format, time() + $system_config->timezone_offset);
   ?>
   <p> &nbsp; </p>
   <table align=center>
   <tr class=admin>
      <th class=admin colspan="6">Banned Sub-Hosts and Browser Types Combinations (publish form bans) &nbsp; Total = <?=count($hostBanList->banned_list)?>
      <br>&nbsp; <?=$timeMsg?>
      </td>
   </tr>
   <tr class=admin>
      <th class=admin>&nbsp;<?=$textLabels['sub_hostname_browser_label']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['ban_type_label']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['ban_began_label']?>&nbsp;&amp; &nbsp;<?=$textLabels['ban_expires_label']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['ban_reason_label']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['lift_ban_label']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['change_ban_length_label']?>&nbsp;</th>
   </tr>
   <?

      foreach($hostBanList->banned_list as $one_ban)
      {
         //$t_hostname = "ras2.galway.eircom.net";
         //$t_browser = "Mozilla/5.0 (Windows; U; Windows NT 5.1; en-GB; rv:1.9.0.11) Gecko/2009060215 Firefox/3.0.11"; 
         $t_hostname = $one_ban->sub_hostname;
         $t_browser =  $one_ban->browser_type;

	 if ($one_ban->ban_type == 1) $ban_type = "ban";
	 else $ban_type = "moderation";
         $reason = htmlspecialchars($one_ban->reason);
	 // The system offset time has already been added to the ban begin time.
         $ban_began = strftime($system_config->default_strftime_format, $one_ban->begin_ban);
         $expires = strftime($system_config->default_strftime_format, $one_ban->time_limit + $system_config->timezone_offset);
         $lift_str = "<a href='editbannedips.php?hostsubpage=unban&amp;target_host=".addslashes($t_hostname)."&amp;target_browser=".$t_browser."&amp;target_ban_type=".$ban_type."'><img src='graphics/delete.gif' /></a>";
         $extend_str = "<a href='editbannedips.php?hostsubpage=ban&amp;target_host=".addslashes($t_hostname)."&amp;target_browser=".$t_browser."&amp;target_ban_type=".$ban_type."'><img src='graphics/unhide.gif' /></a>";
	 if (time() > ($one_ban->time_limit + $system_config->timezone_offset))
         {
             $expires = "<b>".$textLabels['expired_word'].": </b>" . $expires;
         }
         ?>
         <tr class=admin>
            <td class=admin align=left><?=$t_hostname?>&nbsp;<br><br> <?=$t_browser?></td>
            <td class=admin align=center><?=$ban_type?></td>
            <td class=admin align=left><?=$ban_began?><br><br> <?=$expires?></td>
            <td class=admin align=left><?=$reason?>&nbsp;</td>
            <td class=admin align=center>&nbsp;<?=$lift_str?>&nbsp;</td>
            <td class=admin align=center>&nbsp;<?=$extend_str?>&nbsp;</td>
         </tr>
         <?
      }
      ?>
      <tr>
      <td colspan="6" align="center">
         <form name="ban_subhost" action="editbannedips.php" method=post>
         <input type="hidden" name="hostsubpage" value="ban"><br>
         <input type="submit" name="go" value="<?=$textLabels['ban_sub_host_browser_type_btn_label']?>"> <br><br>
         <small><i><?=$textLabels['ban_sub_host_browser_type_btn_subtext']?></i></small><br><br>
         <div align="left">
         <?=$textLabels['sub_hostname_word']?><input type="text" maxlength=120 size=60 name="target_host" value=""> &nbsp;
         <?=$textLabels['put_on_moderation_label']?><input type=radio name="target_ban_type" value="2" checked> &nbsp; <?=$textLabels['ban_posts_label']?><input type=radio name="target_ban_type" value="1"><br><br>
         <?=$textLabels['browser_type_label']?>&nbsp; <input type="text" maxlength=180 size=120 name="target_browser" value="">
         <br><br>
         <small><?=$textLabels['note_word']?>:<i><?=$textLabels['target_browser_subtext']?> </i></small><br>
         </div>
         </form>
      </td>
      </tr>
   </table>
   <?
}
function getBanTimeStrs()
{
	$time_limit = cleanseNumericalQueryField($_REQUEST['ban_period']);
        if($time_limit <= 0)
        {
            $time_limit = 10000 * 24;
        }
        $ban_end = time() + ($time_limit * 60 * 60);

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

        return array($time_limit, $ban_end);
}


ob_start();

if($editor_session->isSessionOpen())
{
   writeAdminHeader("editbannedauthors.php","Ban Authors", array("editmonitor.php" => "Monitor Publishers", "editorstatus.php" => "Users Logged in and Msgs"));
   if($editor_session->editor->allowedReadAccessTo("editbannedips"))
   {
      $ipBanList = new BannedIPList();
      $ipBanList->load();

      $hostBanList = new BannedHostList();
      $hostBanList->load();

      // $hostBanList = null;
      if(isset($_REQUEST['subpage']) && isset($_REQUEST['confirm']))
      {
         $target_ip = cleanseIP($_REQUEST['target_ip']);
         if($target_ip === false)
         {
            // No valid IP Address was input!
            writeError($textLabels['no_valid_ip_addr_input_text']);
            writeBannedIPList();
            writeBannedSubHostList();
            return;
         }
         if($editor_session->editor->allowedWriteAccessTo("editbannedips"))
         {
            if($_REQUEST['subpage']=="unban")
            {
               $ipBanList->unbanIP($target_ip);
	       $logMsg = $editor_session->editor->editor_name.":\tunban:\tIP:\t" . $target_ip . ":\tban lifted.";
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
	       $logMsg = $editor_session->editor->editor_name.":\tban:\tIP:\t" . $target_ip . ":\t " .$r;
	       logAction(null, $target_ip, "IP", "ban", $r);
               $ipBanList->notifyByEmail($target_ip, $time_limit, $r);
            }
         }
         else
         {
            $editor_session->writeNoWritePermissionError();
         }
         writeBannedIPList();
         writeBannedSubHostList();
      }
      else if(isset($_REQUEST['subpage']) && isset($_REQUEST['cancel']))
      {
         writeBannedIPList();
         writeBannedSubHostList();
      }
      else if(isset($_REQUEST['subpage']))
      {
         # Go through the list to get the reason.
         $reason = "";
         if (isset($_REQUEST['target_ip']))
         {
             foreach($ipBanList->banned_list as $one_ban)
	     {
                 if ($_REQUEST['target_ip'] == $one_ban->ip )
                 {
                     $reason = htmlspecialchars($one_ban->reason);
                     break;
                 }
             }
         }
         writeConfirmBanBox($reason);
      }
      else if(isset($_REQUEST['hostsubpage']) && isset($_REQUEST['confirm']))
      {
         $target_host = trim($_REQUEST['target_host']);
         if(strlen($target_host) === 0)
         {
            # No valid Sub Hostname Address was input!
            writeError($textLabels['no_valid_sub_hostname_input_text']);
            writeBannedIPList();
            writeBannedSubHostList();
            return;
         }
         $target_browser = trim($_REQUEST['target_browser']);
         $target_ban_type = $_REQUEST['target_ban_type'];

         if(strlen($target_browser) === 0)
         {
            # No valid browser type was input!
            writeError($textLabels['no_valid_browser_type_input_text']);
            writeBannedIPList();
            writeBannedSubHostList();
            return;
         }
         if($editor_session->editor->allowedWriteAccessTo("editbannedips"))
         {
            if($_REQUEST['hostsubpage']=="unban")
            {
               $hostBanList->unbanHost($target_host, $target_browser);
	       $logMsg = $editor_session->editor->editor_name.":\tunban:\tHostname:\t" . $target_host . ":\tban lifted.";
	       logAction(null, $target_host, "Hostname", "unban", "ban lifted");
            }
            else
            {
               $time_limit = cleanseNumericalQueryField($_REQUEST['ban_period']);
               if($time_limit <= 0)
               {
                  // Set forever to just 3 yrs approx
                  $time_limit = 1000 * 24;
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

               $hostBanList->banHost($target_host, $target_browser, $target_ban_type, $ban_end, $r);
	       $logMsg = $editor_session->editor->editor_name.":\tban:\tHostname:\t" . $target_host . ":\t " .$r;
               if ($target_ban_type == BANTYPE_BAN)
	           logAction(null, $target_host, "Hostname", "ban", $r);
	       else
	           logAction(null, $target_host, "Hostname", "ban moderation", $r);
               $hostBanList->notifyByEmail($target_host, $time_limit, $r);
            }
         }
         else
         {
            $editor_session->writeNoWritePermissionError();
         }
         writeBannedIPList();
         writeBannedSubHostList();
      }
      else if(isset($_REQUEST['hostsubpage']) && isset($_REQUEST['cancel']))
      {
         writeBannedIPList();
         writeBannedSubHostList();
      }
      else if(isset($_REQUEST['hostsubpage']))
      {
         # Go through the list to get the reason.
         $reason = "";
         if (isset($_REQUEST['target_host']))
         {
             foreach($hostBanList->banned_list as $one_ban)
	     {
                 if (trim($_REQUEST['target_host']) == $one_ban->sub_hostname && trim($_REQUEST['target_browser']) == $one_ban->browser_type)
                 {
                     $reason = htmlspecialchars($one_ban->reason);
                     break;
                 }
             }
         }
         writeConfirmBanHostBox($reason);
      }
      else
      {
         writeBannedIPList();
         writeBannedSubHostList();
      }
      if (count($hostBanList) > 0 ) $admin_table_width = "90%";
   }
   else $editor_session->writeNoReadPermissionError();
}
else $editor_session->writeNoSessionError();



include_once("adminfooter.inc");
?>

