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


function getBanLengthSelect()
{
   $str = "<select name='ban_period'>";
   $opts = array(1 => "1 hour", 2 => "2 hours", 6 => "6 hours", 12 => "12 hours", 24 => "24 hours", 48 => "2 days", 72 => "3 days", 120 => "5 days", 168 => "1 week", 336 => "2 weeks", 720 => "1 month", 1440 => "2 months", 2160 => "3 months", 2880 => "4 months", 4320 => "6 months", 5760 => "8 months", 8640 => "1 year", 0 => "ever and ever");
   foreach(array_keys($opts) as $k)
   {
      $str .= "<option value='$k'>".$opts[$k]."</option>\n";
   }
   $str .= "</select>";
   return $str;
}

function writeError($error)
{
   ?><BR><BR><font class=error><B><?=$error?></B></font><BR><BR><?
}

function writeConfirmBanBox( $ban_reason, $ban_author=false)
{

   if ($ban_author == true) {
     global $system_config, $monitorList, $editor_session, $OSCAILT_SCRIPT;
     $target_author = trim($_REQUEST['target_author']);
     if($target_author == "")
     {
        writeError("You must enter a valid author name to block!");
        writeMonitorPage();
        return;
     }
   }
   else
   {
      global $system_config, $ipBanList, $monitorList, $editor_session, $OSCAILT_SCRIPT;
      $target_ip = cleanseIP($_REQUEST['target_ip']);
      if($target_ip === false)
      {
         writeError("You must enter a valid IP address to block!");
         writeMonitorPage();
         return;
      }
   }

   if($_REQUEST['subpage'] == 'unban')
   {
      $action = 'unban';
      $extra_str = "";
      $intro = "Are you sure that you wish to remove the ban on ";

      if ($ban_author == true)
          $intro .= "( " .$target_author ." ) publishing on this site?";
      else
          $intro .= $target_ip ."(".gethostbyaddr($target_ip).") publishing on this site?";
   }
   else
   {
      $action = 'ban';
      $intro = "Are you sure that you wish to ban ";
      if ($ban_author == true)
          $intro .= $target_author ." from publishing on this site?";
      else
          $intro .= $target_ip . "(".gethostbyaddr($target_ip).") from publishing on this site?";

      $extra_str = "<P>Select the length of the ban: ". getBanLengthSelect()."</P>";
      $extra_str .= "<P align='center'>Enter Reason for ban: <br /><TEXTAREA rows=5 cols=30 name='ban_reason'>" . $ban_reason . "</TEXTAREA></P>";
   }
   ?>
   <table align=center>
   <form name="banned_data" action="<?=$OSCAILT_SCRIPT?>" method="post">
   <input type="hidden" name="subpage" value="<?=$action?>">
   <?

   if ($ban_author == true) {
       ?><input type="hidden" name="target_author" value="<?=$target_author?>"><?
   } else {
       ?><input type="hidden" name="target_ip" value="<?=$target_ip?>"><?
   }

   ?>
   <tr>
      <td colspan=2 align=center><img src="graphics/caution.gif" align=middle><BR><BR><B><?=htmlspecialchars($intro, ENT_QUOTES)?></B><BR><BR>
      <?=$extra_str?></td>
   </tr>
   <tr>
      <td align=right><input type=submit name=cancel value="&lt;&lt; Cancel"></td>
      <td><input type=submit name=confirm value="Confirm <?=$action?> &gt;&gt;"></td>
   </tr>
   </form>
   </table>
   <?
}

?>
