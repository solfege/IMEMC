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


class PublishStateEntry
{
   var $username;
   var $user_state;
   var $time_posted;
   var $sys_message;
   var $save_it;

   function PublishStateEntry($username, $user_status, $time_posted, $system_msg="")
   {
      $this->username = $username;
      $this->user_state = $user_status;

      $this->time_posted = $time_posted;
      $this->sys_message = $system_msg;
      $this->save_it = true;
   }

   function getSerialisedPHP()
   {
      $str = '$recent_publishes[] = new PublishStateEntry("'.addslashes($this->username).'", "'.addslashes($this->user_state).'",'.$this->time_posted.', "'.addslashes($this->sys_message).'"'.")\n";
      return $str;
   }

}

class PublishState
{
   var $recent_publishes = array();
   var $status_monitor_size_limit;

   function PublishState()
   {
      global $system_config;
      // This should eventually have its own limit defined.
      $this->status_monitor_size_limit = $system_config->status_monitor_size_limit;
   }

   function load($force=false)
   {
      global $system_config;
      if(!$force && count($this->recent_publishes) > 0) return;
      $file_path = $system_config->status_monitor_cache_store;
      if(file_exists($file_path))
      {
         $fcontents = implode("", file($file_path));
         $recent_publishes = array();
         eval(' ?>'.$fcontents.'<?');
         if(count($recent_publishes) > 0)
         {
            for($i = 0; $i < count($recent_publishes); $i++)
            {
               $recent_publishes[$i]->sys_message = stripslashes($recent_publishes[$i]->sys_message);
            }
            $this->recent_publishes = $recent_publishes;
         }
         $this->discardOldRecords();
      }
   }

   // This function is used for clearing all old 'view' records matching input.
   function discardViewsByUsername($username)
   {
      for($j=0; $j < count($this->recent_publishes); $j++)
      {
         $ptr = $this->recent_publishes[$j];
         if ($ptr->username == $username)
         {
             // Beware for other arrays, to modify, you have to change it like this. Other ways don't work!
             if ($ptr->user_state == "view") { $this->recent_publishes[$j]->save_it = 0;}
         }
      }
   }

   function discardOldRecords()
   {
      while(count($this->recent_publishes) > $this->status_monitor_size_limit)
      {
         array_shift($this->recent_publishes);
      }
   }

   function add($username, $user_state, $time_posted, $message="")
   {
      $new_entry = new PublishStateEntry($username, $user_state, $time_posted, $message);
      $this->recent_publishes[] = $new_entry;
      $this->discardOldRecords();
   }

   function save()
   {
      global $system_config;
      $file = $system_config->status_monitor_cache_store;
      $fp = fopen($file,"w");
      if (flock($fp, LOCK_EX))
      {
         fputs($fp, '<?php '."\n");
         foreach($this->recent_publishes as $one_publish)
         {
            // Only if it is true should we save it.
            if ($one_publish->save_it == true) fputs($fp, $one_publish->getSerialisedPHP().";\n");
         }
         // unlocking file
         fputs($fp, '?>'."\n");
         flock($fp, LOCK_UN);
         fclose($fp);
      }
      else echo "<P>failed to get lock on publish cache file $system_config->status_monitor_cache_store</p>";
   }

}

?>
