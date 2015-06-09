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

class BannedAuthor
{
   var $author_name;
   var $time_limit;
   var $begin_ban;
   var $reason;

   function BannedAuthor($author_name, $time_limit, $reason, $begin_ban)
   {
      $this->author_name = trim($author_name);
      $this->time_limit = $time_limit;
      $this->reason = $reason;
      $this->begin_ban = $begin_ban;
   }
}

class BannedAuthorList
{
   var $banned_list = array();
   var $loaded = false;

   function isBanned($author_name)
   {
      global $prefix, $dbconn;
      if(!$this->loaded)
      {
         $this->load();
      }
      if(in_array($author_name, array_keys($this->banned_list)))
      {
         return true;
      }
      return false;

   }
   function isExpired($author_name)
   {
      global $system_config;

      $banned_author_name = $this->banned_list[$author_name];

      # Now check if it has expired. If there is more time to go then it is not expired.
      if (time() < ($banned_author_name->time_limit + $system_config->timezone_offset)) return false;

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
      $sql = "DELETE from ".$prefix."_banned_authors WHERE UNIX_TIMESTAMP(time_limit) < $cutoff";
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
      $sql = "select author_name, UNIX_TIMESTAMP(time_limit), reason, UNIX_TIMESTAMP(begin_ban) from ".$prefix."_banned_authors where 1 ORDER BY begin_ban";
      $result = sql_query($sql, $dbconn, 1);
      checkForError($result);
      $tmp_startdate = 0;
      if(sql_num_rows( $result ) > 0)
      {
         for ($i=0; $i<sql_num_rows( $result ); $i++)
         {
            list($author_name, $time_limit, $reason, $begin_ban) = sql_fetch_row($result, $dbconn);
            $new_ban = new BannedAuthor($author_name, $time_limit, $reason, $begin_ban);
            $author_name = trim($author_name);
            $this->banned_list[$author_name] = $new_ban;
         }
      }
      $this->loaded = true;
   }

   function banAuthor($author_name, $time_limit, $reason)
   {
      global $prefix, $dbconn, $system_config;
      $r = addslashes($reason);
      $author_name = addslashes(trim($author_name));
      $begin_ban = time() + $system_config->timezone_offset;

      // Update the ban by actually deleting the old one and inserting a new one. Changed to do update.
      if($this->isBanned($author_name)) {
          $do_update = true;
          $sql = "UPDATE ".$prefix."_banned_authors SET time_limit = FROM_UNIXTIME($time_limit), reason='$r' WHERE author_name='$author_name'";
      } else {
          $do_update = false;
          $sql = "INSERT INTO ".$prefix."_banned_authors VALUES('$author_name', FROM_UNIXTIME($time_limit), '$r', FROM_UNIXTIME($begin_ban))";
      }

      checkForError(sql_query($sql, $dbconn, 1));
      if($this->loaded) {
          if($do_update == true) {
	      $this->banned_list[$author_name]->time_limit = $time_limit;
	      $this->banned_list[$author_name]->reason = $reason;
          } else {
	      $this->banned_list[$author_name] = new BannedAuthor($author_name, $time_limit, $reason, $begin_ban);
          }
      }
   }

   function unbanAuthor($author_name)
   {
      global $prefix, $dbconn;
      $author_name = addslashes($author_name);
      $sql = "delete from ".$prefix."_banned_authors where author_name = '".addslashes($author_name)."'";
      checkForError(sql_query($sql, $dbconn, 1));
      if($this->loaded) unset($this->banned_list[$author_name]);
   }

   function getBannedMessage($author_name)
   {
      global $system_config;

      $msg = "The computer that you are posting is banned.  You can still view the site but you can not publish here or use the site contact forms.  This publishing ban will last until ". strftime("%H:%M, %a %b %d, %Y", $this->banned_list[$author_name]->time_limit + $system_config->timezone_offset). ". It is a very bad idea to attract repeated bans. If you feel that this ban is inappropriate, contact the site editors, by email at: <b>". htmlspecialchars($system_config->contact_email_address)."</b>";

      return $msg;
   }
}
?>
