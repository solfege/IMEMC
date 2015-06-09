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


class ReminderEntry
{
   var $remind_time;
   var $sys_message;
   var $save_it;

   function ReminderEntry($remind_time, $reminder_msg="")
   {
      $this->remind_time = $remind_time;
      $this->sys_message = $reminder_msg;
      $this->save_it = true;
   }

   function getSerialisedPHP()
   {
      $str = '$recent_reminders[] = new ReminderEntry('.$this->remind_time.', "'.addslashes($this->sys_message).'"'.")\n";
      return $str;
   }

}

class ReminderList
{
   var $recent_reminders = array();
   var $reminder_size_limit;

   function Reminder()
   {
      global $system_config;
      // This should eventually have its own limit defined.
      //$this->reminder_size_limit = $system_config->reminder_size_limit;
   }

   function load($force=false)
   {
      global $system_config;
      if ($force == true) $this->recent_reminders = null;
      if(!$force && count($this->recent_reminders) > 0) return;

      $file_path = $system_config->reminder_cache_store;
      if(file_exists($file_path))
      {
         $fcontents = implode("", file($file_path));
         $recent_reminders = array();
         eval(' ?>'.$fcontents.'<?');
         if(count($recent_reminders) > 0)
         {
            for($i = 0; $i < count($recent_reminders); $i++)
            {
               $recent_reminders[$i]->sys_message = stripslashes($recent_reminders[$i]->sys_message);
            }
            $this->recent_reminders = $recent_reminders;
         }
      } else {
        if ($system_config->user_error_reporting >= 2)
            echo("<p>Cannot find reminder cache file:$system_config->reminder_cache_store</p>");
      }
   }

   function discardOldRecords()
   {
      global $system_config;

      for($j=0; $j < count($this->recent_reminders); $j++)
      {
         $ptr = $this->recent_reminders[$j];
         // Time offset not really used
         if ($ptr->remind_time < time()) 
         {
             // Beware for other arrays, to modify, you have to change it like this. Other ways don't work!
             $this->recent_reminders[$j]->save_it = false;
         }
      }
   }

   function add($remind_time, $message="")
   {
      $new_entry = new ReminderEntry($remind_time, $message);
      $this->recent_reminders[] = $new_entry;
   }

   function save()
   {
      global $system_config;
     
      $file = $system_config->reminder_cache_store;
      $fp = fopen($file,"w");
      if (flock($fp, LOCK_EX))
      {
         fputs($fp, '<?php '."\n");
         foreach($this->recent_reminders as $one_publish)
         {
            // Only if it is true should we save it.
            if ($one_publish->save_it == true) fputs($fp, $one_publish->getSerialisedPHP().";\n");
         }
         // unlocking file
         fputs($fp, '?>'."\n");
         flock($fp, LOCK_UN);
         fclose($fp);
      }
      else echo "<P>failed to get lock on reminder cache file $system_config->reminder_cache_store</p>";
   }
}

?>
