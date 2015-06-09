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
require_once("objects/indyobjects/indydataobjects.inc");
$OSCAILT_SCRIPT = "editbannedips.php";
addToPageTitle("Manage Blocked IP addresses");


function writeBannedIPList()
{
   global $ipBanList, $system_config;
   $timeMsg = strftime($system_config->default_strftime_format, time() + $system_config->timezone_offset);
   ?>
   <table align=center>
   <tr class=admin>
      <th class=admin colspan="6">Banned IPs (publish and contact form bans) &nbsp; Total = <?=count($ipBanList->banned_list)?>
      <br>&nbsp; <?=$timeMsg?>
      </td>
   </tr>
   <tr class=admin>
      <th class=admin>&nbsp;IP Address&nbsp;</th>
      <th class=admin>&nbsp;Ban Began&nbsp;</th>
      <th class=admin>&nbsp;Ban Expires&nbsp;</th>
      <th class=admin>&nbsp;Ban Reason&nbsp;</th>
      <th class=admin>&nbsp;Lift Ban&nbsp;</th>
      <th class=admin>&nbsp;Change Ban Length&nbsp;</th>
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
             $expires = "<b>Expired: </b>" . $expires;
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
         <form action="editbannedips.php" method=post>
         <input type="hidden" name="subpage" value="ban">
         <input type="submit" name="go" value="Ban IP">
         <input type="text" name="target_ip" value="">
         </form>
      </td>
      </tr>
   </table>
   <?
}


ob_start();

if($editor_session->isSessionOpen())
{
   writeAdminHeader("editbannedauthors.php","Ban Authors");
   if($editor_session->editor->allowedReadAccessTo("editbannedips"))
   {
      $ipBanList = new BannedIPList();
      $ipBanList->load();
      if(isset($_REQUEST['subpage']) && isset($_REQUEST['confirm']))
      {
         $target_ip = cleanseIP($_REQUEST['target_ip']);
         if($target_ip === false)
         {
            writeError("No valid IP Address was input!");
            writeBannedIPList();
            return;
         }
         if($editor_session->editor->allowedWriteAccessTo("editbannedips"))
         {
            if($_REQUEST['subpage']=="unban")
            {
               $ipBanList->unbanIP($target_ip);
               // echo "<P>unban</p>";
	       $logMsg = $editor_session->editor->editor_name.":\tunban:\tIP:\t" . $target_ip . ":\tban lifted.";
	       logAction(null, $target_ip, "IP", "unban", "ban lifted");
	       // logMessage($logMsg,"actionlog.txt");
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
	       // logMessage($logMsg, "actionlog.txt");
               $ipBanList->notifyByEmail($target_ip, $time_limit, $r);
            }
         }
         else
         {
            $editor_session->writeNoWritePermissionError();
         }
         writeBannedIPList();
      }
      else if(isset($_REQUEST['subpage']) && isset($_REQUEST['cancel']))
      {
         writeBannedIPList();
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
      else
      {
         writeBannedIPList();
      }
   }
   else $editor_session->writeNoReadPermissionError();
}
else $editor_session->writeNoSessionError();

include_once("adminfooter.inc");
?>