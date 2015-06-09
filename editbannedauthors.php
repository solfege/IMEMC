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
require_once("objects/bannedauthor.inc");
require_once("objects/adminutilities.inc");
require_once("objects/indyobjects/indydataobjects.inc");
$OSCAILT_SCRIPT = "editbannedauthors.php";
addToPageTitle("Manage Blocked Author Names");


function writeBannedAuthorList()
{
   global $authorBanList, $system_config;
   $timeMsg = strftime($system_config->default_strftime_format, time() + $system_config->timezone_offset);
   ?>
   <table align=center>
   <tr class=admin>
      <th class=admin colspan="6">Banned Author Names (publish bans) &nbsp;  Time now: <?=$timeMsg?></td>
   </tr>
   <tr class=admin>
      <th class=admin>&nbsp;Author Name&nbsp;</th>
      <th class=admin>&nbsp;Ban Began&nbsp;</th>
      <th class=admin>&nbsp;Ban Expires&nbsp;</th>
      <th class=admin>&nbsp;Ban Reason&nbsp;</th>
      <th class=admin>&nbsp;Lift Ban&nbsp;</th>
      <th class=admin>&nbsp;Change Ban Length&nbsp;</th>
   </tr>
   <?
      foreach($authorBanList->banned_list as $one_ban)
      {
         $author_name = $one_ban->author_name;
         $reason = htmlspecialchars($one_ban->reason);
	 // The system offset time has already been added to the ban begin time.
         $ban_began = strftime($system_config->default_strftime_format, $one_ban->begin_ban);
         $expires = strftime($system_config->default_strftime_format, $one_ban->time_limit + $system_config->timezone_offset);
         $lift_str = "<a href='editbannedauthors.php?subpage=unban&amp;target_author=$author_name'><img src='graphics/delete.gif' /></a>";
         $extend_str = "<a href='editbannedauthors.php?subpage=ban&amp;target_author=$author_name'><img src='graphics/unhide.gif' /></a>";
	 if (time() > ($one_ban->time_limit + $system_config->timezone_offset))
         {
             $expires = "<b>Expired: </b>" . $expires;
         }
         ?>
         <tr class=admin>
            <td class=admin align=center>&nbsp;<?=$author_name?>&nbsp;</td>
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
         <form action="editbannedauthors.php" method=post>
         <input type="hidden" name="subpage" value="ban">
         <input type="submit" name="go" value="Ban Author">
         <input type="text" name="target_author" value="">
         </form>
      </td>
      </tr>
   </table>
   <?
}

// This code is so like lots of other code, it could be made common.
function notifyByEmail($target_author, $ban_length, $ban_reason)
{
      global $system_config,$editor_session;

      $subject = "Notification of ban of Author Name " .$target_author ." performed by ".$editor_session->editor->editor_name;
      $message = $system_config->software_name." ".$system_config->software_version." Automatic Notification"."\r\n";
      $message = $message."Date   : ".date("l, M j Y, g:ia",time()+$system_config->timezone_offset)."\r\n";
      $message = $message."Action   : Ban of Author Name ".$target_author." performed by ".$editor_session->editor->editor_name."\r\n";

      $ban_units = "hours";
      if ($ban_length >= 8640 ) {
         $ban_units = "years";
         $ban_length = round($ban_length / 8640);
      } elseif ($ban_length >= 720 ) {
         $ban_units = "months";
         $ban_length = $ban_length / 720;
      } elseif ($ban_length >= 168 ) {
         $ban_units = "weeks";
         $ban_length = $ban_length / 168;
      } elseif ($ban_length > 48 ) {
         $ban_units = "days";
         $ban_length = $ban_length / 24;
      }

      $message = $message."For      : " . $ban_length . " ".$ban_units ."\r\n";
      if($ban_reason==null || strlen($ban_reason)==0) $ban_reason = "<not specified>";
      $message = $message."Reason   : ".$ban_reason."\r\n";
      $message = $message."\r\n";

      // echo($message);
      mail($system_config->notification_to_email_address, $subject, $message, "From: ".$system_config->notification_from_email_address."\r\n"."Reply-To: ".$editor_session->editor->editor_email.",".$system_config->notification_replyto_email_address."\r\n"."X-Mailer: ".$system_config->software_name."/".$system_config->software_version." using PHP/".phpversion());
}





ob_start();

if($editor_session->isSessionOpen())
{
   writeAdminHeader("editbannedips.php","Ban IPs");
   if($editor_session->editor->allowedReadAccessTo("editbannedips"))
   {
      $authorBanList = new BannedAuthorList();
      $authorBanList->load();
      if(isset($_REQUEST['subpage']) && isset($_REQUEST['confirm']))
      {
         $target_author = trim($_REQUEST['target_author']);
         if($target_author === false)
         {
            writeError("No valid Author Name was input!");
            writeBannedAutorList();
            return;
         }
         if($editor_session->editor->allowedWriteAccessTo("editbannedips"))
         {
            if($_REQUEST['subpage']=="unban")
            {
               $authorBanList->unbanAuthor($target_author);
	       $logMsg = $editor_session->editor->editor_name.":\tunban:\tauthor name:\t".$target_author.":\tban lifted.";
	       logAction(null, $target_author, "IP", "name unban", "ban lifted");
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

               $authorBanList->banAuthor($target_author, $ban_end, $r);
	       $logMsg = $editor_session->editor->editor_name.":\tban:\tauthor name:\t" . $target_author . ":\t " .$r;
	       logAction(null, $target_author, "IP", "name ban", $r);
               notifyByEmail($target_author, $time_limit, $r);
            }
         }
         else
         {
            $editor_session->writeNoWritePermissionError();
         }
         writeBannedAuthorList();
      }
      else if(isset($_REQUEST['subpage']) && isset($_REQUEST['cancel']))
      {
         writeBannedAuthorList();
      }
      else if(isset($_REQUEST['subpage']))
      {
         # Go through the list to get the reason.
         $reason = "";
         if (isset($_REQUEST['target_author']))
         {
             foreach($authorBanList->banned_list as $one_ban)
	     {
                 if ($_REQUEST['target_author'] == $one_ban->author_name )
                 {
                     $reason = htmlspecialchars($one_ban->reason);
                     break;
                 }
             }
         }
	 // 2nd parameter means ban authors.
         writeConfirmBanBox($reason, true);
      }
      else
      {
         writeBannedAuthorList();
      }
   }
   else $editor_session->writeNoReadPermissionError();
}
else $editor_session->writeNoSessionError();

include_once("adminfooter.inc");
?>