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

class BannedIP
{
   var $ip;
   var $time_limit;
   var $begin_ban;
   var $reason;

   function BannedIP($ip, $time_limit, $reason, $begin_ban)
   {
      $this->ip = $ip;
      $this->time_limit = $time_limit;
      $this->reason = $reason;
      $this->begin_ban = $begin_ban;
   }
}
class BannedSubHost
{
   var $sub_hostname;
   var $browser_type;
   var $ban_type;
   var $time_limit;
   var $begin_ban;
   var $reason;

   function BannedSubHost($sub_hostname, $browser_type, $bantype, $time_limit, $reason, $begin_ban)
   {
      $this->sub_hostname = $sub_hostname;
      $this->browser_type = $browser_type;
      $this->ban_type = $bantype;
      $this->time_limit = $time_limit;
      $this->reason = $reason;
      $this->begin_ban = $begin_ban;
   }
}


class BannedIPList
{
   var $banned_list = array();
   var $loaded = false;

   function isBanned($ip)
   {
      global $prefix, $dbconn;
      if(!$this->loaded)
      {
         $this->load();
      }
      if(in_array($ip, array_keys($this->banned_list)))
      {
         return true;
      }
      return false;

   }
   function isExpired($ip)
   {
      global $system_config;

      $banned_ip = $this->banned_list[$ip];

      # Now check if it has expired. If there is more time to go then it is not expired.
      if (time() < ($banned_ip->time_limit + $system_config->timezone_offset)) return false;

      return true;
   }

   function clearOldBans()
   {
      global $prefix, $dbconn;
      $cutoff = time();
      if($this->loaded)
      {
         $needs_delete = false;
         foreach($this->banned_list as $b)
         {
            if($b->time_limit < $cutoff)
            {
               $needs_delete = true;
               break;
            }
         }
         if($needs_delete == false) return;
      }
      $sql = "DELETE from ".$prefix."_banned_ips WHERE UNIX_TIMESTAMP(time_limit) < $cutoff";
      checkForError(sql_query($sql, $dbconn, 1));
      if($this->loaded)
      {
         $this->loaded = false;
         $this->banned_list = array();
         $this->load();
      }
   }

   function load()
   {
      global $prefix, $dbconn;
      $sql = "select ip, UNIX_TIMESTAMP(time_limit), reason, UNIX_TIMESTAMP(begin_ban) from ".$prefix."_banned_ips where 1 ORDER BY BEGIN_BAN";
      $result = sql_query($sql, $dbconn, 1);
      checkForError($result);
      $tmp_startdate = 0;
      if(sql_num_rows( $result ) > 0)
      {
         for ($i=0; $i<sql_num_rows( $result ); $i++)
         {
            list($ip, $time_limit, $reason, $begin_ban) = sql_fetch_row($result, $dbconn);
            $new_ban = new BannedIP($ip, $time_limit, $reason, $begin_ban);
            $this->banned_list[$ip] = $new_ban;
         }
      }
      $this->loaded = true;
   }

   function banIP($ip, $time_limit, $reason)
   {
      global $prefix, $dbconn, $system_config;
      $r = addslashes($reason);
      $ip = addslashes($ip);
      $begin_ban = time() + $system_config->timezone_offset;

      // Update the ban by actually deleting the old one and inserting a new one. Changed to do update.
      if($this->isBanned($ip)) {
          $do_update = true;
          $sql = "UPDATE ".$prefix."_banned_ips SET time_limit = FROM_UNIXTIME($time_limit), REASON='$r' WHERE IP='$ip'";
      } else {
          $do_update = false;
          $sql = "INSERT INTO ".$prefix."_banned_ips VALUES('$ip', FROM_UNIXTIME($time_limit), '$r', FROM_UNIXTIME($begin_ban))";
      }

      checkForError(sql_query($sql, $dbconn, 1));
      // if($this->loaded) {
          if($do_update == true) {
	      $this->banned_list[$ip]->time_limit = $time_limit;
	      $this->banned_list[$ip]->reason = $reason;
          } else {
	      $this->banned_list[$ip] = new BannedIP($ip, $time_limit, $reason, $begin_ban);
          }
      // }
   }

   function unbanIP($ip)
   {
      global $prefix, $dbconn;
      $ip = addslashes($ip);
      $sql = "delete from ".$prefix."_banned_ips where ip = '".addslashes($ip)."'";
      checkForError(sql_query($sql, $dbconn, 1));
      if($this->loaded) unset($this->banned_list[$ip]);
   }

   function getBannedReason($ip)
   {
      $banned_ip = $this->banned_list[$ip];
      return $banned_ip->reason;
   }

   function getBannedMessage($ip)
   {
      global $system_config;
      if($system_config->disallow_ipless_publication && ($ip === false or !isset($this->banned_list[$ip])))
      {
         $msg = "The computer that you are posting from is either transmitting no IP address to oscailt or it is sending an incorrectly formatted one.  It is not possible to publish from such a computer";
      }
      else
      {
         $msg = "The computer that you are posting from has a banned IP address.  You can still view the site but you can not publish here or use the site contact forms.  This publishing ban will last until ". strftime("%H:%M, %a %b %d, %Y", $this->banned_list[$ip]->time_limit + $system_config->timezone_offset). ". It is a very bad idea to attract repeated bans. If you feel that this ban is inappropriate, contact the site editors, by email at: <b>". htmlspecialchars($system_config->contact_email_address)."</b>";
      }
      return $msg;
   }

   
   // This code is so like lots of other code, it could be made common.
   function notifyByEmail($target_ip, $ban_length, $ban_reason)
   {
      global $system_config,$editor_session;

      // Do not show in email
      $target_ip = "";
      $subject = "Notification of ban of IP " .$target_ip ." performed by ".$editor_session->editor->editor_name;
      $message = $system_config->software_name." ".$system_config->software_version." Automatic Notification"."\r\n";
      $message = $message."Date   : ".date("l, M j Y, g:ia",time()+$system_config->timezone_offset)."\r\n";
      $message = $message."Action   : Ban of IP address ".$target_ip." performed by ".$editor_session->editor->editor_name."\r\n";

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

}

// Class for Banned Host and Browser list.  

class BannedHostList
{
   var $banned_list = array();
   var $loaded = false;

   function findHostMatches($t_hostname)
   {
      global $prefix, $dbconn;
      if(!$this->loaded)
      {
         $this->load();
      }

      // It is assumed there are more than zero in the list.
      $t_return = "";
      foreach ($this->banned_list as $each_ban)
      {
	 // if(strpos($t_hostname, $each_ban->sub_hostname) === false) continue;
	 //$t_return = true;     
         if (preg_match("/".$each_ban->sub_hostname."/", $t_hostname) == 0 ) continue;
	 $t_return = $each_ban->sub_hostname;
	 break;     
      }
      // Maybe return the fragment found as that should be the match
      return $t_return;
   }

   function isBanned($t_hostname, $t_browser)
   {
      global $prefix, $dbconn;
      if(!$this->loaded)
      {
         $this->load();
      }
      $pkey = $t_hostname."+".$t_browser;
      // Need to add in check to do like compare.
      if(in_array($pkey, array_keys($this->banned_list)))
      {
         return true;
      }
      return false;
   }

   function getBannedReason($t_hostname, $t_browser)
   {
      $pkey = $t_hostname."+".$t_browser;
      $banned_host = $this->banned_list[$pkey];
      return $banned_host->reason;
   }


   function getBanType($t_hostname, $t_browser)
   {
      global $prefix, $dbconn;
      if(!$this->loaded)
      {
         $this->load();
      }
      $pkey = $t_hostname."+".$t_browser;
      // Need to add in check to do like compare.
      if(in_array($pkey, array_keys($this->banned_list)))
      {
         return $this->banned_list[$pkey]->ban_type;
      }
      return null;
   }

   function isExpired($t_hostname)
   {
      global $system_config;

      $banned_host = $this->banned_list[$t_hostname];

      # Now check if it has expired. If there is more time to go then it is not expired.
      if (time() < ($banned_host->time_limit + $system_config->timezone_offset)) return false;

      return true;
   }

   function clearOldBans()
   {
      global $prefix, $dbconn;
      $cutoff = time();
      if($this->loaded)
      {
         $needs_delete = false;
         foreach($this->banned_list as $b)
         {
            if($b->time_limit < $cutoff)
            {
               $needs_delete = true;
               break;
            }
         }
         if($needs_delete == false) return;
      }
      $sql = "DELETE from ".$prefix."_banned_host_browser WHERE UNIX_TIMESTAMP(time_limit) < $cutoff";
      checkForError(sql_query($sql, $dbconn, 1));
      if($this->loaded)
      {
         $this->loaded = false;
         $this->banned_list = array();
         $this->load();
      }
   }

   function load()
   {
      global $prefix, $dbconn;
      $sql = "select sub_hostname, browser_type, ban_type, UNIX_TIMESTAMP(time_limit), reason, UNIX_TIMESTAMP(begin_ban) from ".$prefix."_banned_host_browser where 1 ORDER BY BEGIN_BAN";
      $result = sql_query($sql, $dbconn, 1);
      checkForError($result);
      $tmp_startdate = 0;
      if(sql_num_rows( $result ) > 0)
      {
         for ($i=0; $i<sql_num_rows( $result ); $i++)
         {
            list($subhost, $browser, $bantype, $time_limit, $reason, $begin_ban) = sql_fetch_row($result, $dbconn);
            $new_ban = new BannedSubHost($subhost, $browser, $bantype, $time_limit, $reason, $begin_ban);
            $pkey = $subhost."+".$browser;
            $this->banned_list[$pkey] = $new_ban;
         }
      }
      $this->loaded = true;
   }

   function banHost($t_hostname, $t_browser, $t_type, $time_limit, $reason)
   {
      global $prefix, $dbconn, $system_config;
      $r = addslashes($reason);
      // Type 1 = ban, 2 = moderate
      $t_hostname = addslashes($t_hostname);
      $t_browser = addslashes($t_browser);
      $begin_ban = time() + $system_config->timezone_offset;

      // Update the ban by actually deleting the old one and inserting a new one. Changed to do update.
      if($this->isBanned($t_hostname, $t_browser)) {
          $do_update = true;
          $sql = "UPDATE ".$prefix."_banned_host_browser SET time_limit = FROM_UNIXTIME($time_limit), REASON='$r' WHERE sub_hostname='$t_hostname' AND browser_type='$t_browser'";
      } else {
          $do_update = false;
          $sql = "INSERT INTO ".$prefix."_banned_host_browser VALUES('$t_hostname', '$t_browser', $t_type, FROM_UNIXTIME($time_limit), '$r', FROM_UNIXTIME($begin_ban))";
      }

      checkForError(sql_query($sql, $dbconn, 1));
      $pkey = $t_hostname."+".$t_browser;
      // if($this->loaded) {
          if($do_update == true) {
	      $this->banned_list[$pkey]->time_limit = $time_limit;
	      $this->banned_list[$pkey]->reason = $reason;
          } else {
	      $this->banned_list[$pkey] = new BannedSubHost($t_hostname, $t_browser, $t_type, $time_limit, $reason, $begin_ban);
          }
      // }
   }

   function unbanHost($t_hostname, $t_browser)
   {
      global $prefix, $dbconn;
      $t_hostname = trim($t_hostname);
      $t_browser = trim($t_browser);
      $pkey = $t_hostname."+".$t_browser;
      $sql = "DELETE FROM ".$prefix."_banned_host_browser WHERE sub_hostname = '".addslashes($t_hostname)."' AND browser_type = '".addslashes($t_browser)."'";
      checkForError(sql_query($sql, $dbconn, 1));
      if($this->loaded) unset($this->banned_list[$pkey]);
   }

   function getBannedMessage($t_hostname, $t_browser)
   {
      // Message will need to be carefully changed.
      global $system_config;
      $pkey = $t_hostname."+".$t_browser;
      if($system_config->disallow_ipless_publication && $ip === false )
      {
         $msg = "The computer that you are posting from is either transmitting no IP address to oscailt or it is sending an incorrectly formatted one.  It is not possible to publish from such a computer";
      }
      else
      {
         $msg = "The computer that you are posting from has a banned IP address.  You can still view the site but you can not publish here or use the site contact forms.  This publishing ban will last until ". strftime("%H:%M, %a %b %d, %Y", $this->banned_list[$pkey]->time_limit + $system_config->timezone_offset). ". It is a very bad idea to attract repeated bans. If you feel that this ban is inappropriate, contact the site editors, by email at: <b>". htmlspecialchars($system_config->contact_email_address)."</b>";
      }
      return $msg;
   }

   
   // This code is so like lots of other code, it could be made common.
   function notifyByEmail($target_ip, $ban_length, $ban_reason)
   {
      global $system_config,$editor_session;

      // Do not show in email
      $target_ip = "";
      $subject = "Notification of ban of IP " .$target_ip ." performed by ".$editor_session->editor->editor_name;
      $message = $system_config->software_name." ".$system_config->software_version." Automatic Notification"."\r\n";
      $message = $message."Date   : ".date("l, M j Y, g:ia",time()+$system_config->timezone_offset)."\r\n";
      $message = $message."Action   : Ban of IP address ".$target_ip." performed by ".$editor_session->editor->editor_name."\r\n";

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

}

?>
