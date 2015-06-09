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


class EditEntry
{
   var $story_id;
   var $init_time;
   var $granted_by;
   var $password;
   var $user_session;
   var $password_enabled;
   var $save_it;

   function EditEntry($story_id, $init_time, $granted_by, $password, $web_session, $pass_enable=0) 
   {
      $this->story_id = $story_id;
      $this->init_time = $init_time;
      $this->granted_by = $granted_by;
      $this->password = $password;
      $this->user_session = $web_session;
      $this->password_enabled = $pass_enable;
      $this->save_it = true;
   }

   function dump()
   {
      echo "<br>story id ".  $this->story_id ;
      echo "<br>init_time ".  $this->init_time ;
      echo "<br>grant    ".  $this->granted_by ;
      echo "<br>password ".  $this->password ;
      echo "<br>pass enabled ".  $this->password_enabled ;
      echo "<br>session  ".  $this->user_session ;
   }

   function getSerialisedPHP()
   {
      $str = '$recent_edit_entries[] = new EditEntry('.$this->story_id.', '.$this->init_time.', "'.addslashes($this->granted_by).'", "'.addslashes($this->password).'", "'.addslashes($this->user_session).'", '.$this->password_enabled.")\n";
      return $str;
   }

}

class EditEntriesList
{
   var $use_password;
   var $expire_days;
   var $edit_hours;
   var $recent_edit_entries = array();
   var $base_path = "cache/public_edit_list.inc";
   var $loaded;

   function EditEntriesList()
   {
       //$this->use_password = false;
       $this->use_password = true;
       $this->loaded = false;
       // Allow 5 days for it to edited continously
       $this->expire_days = 2 * 24 * 3600;

       // Allow X hours before user can no longer edit
       $this->edit_hours = 1 * 3600;
   }

   function kount()
   {
      if ($this->loaded == false) $this->load();

      if ($this->recent_edit_entries == null) return 0;

      return count($this->recent_edit_entries);
   }

   function load($force=false)
   {
      global $system_config;
      if ($force == true) $this->recent_edit_entries = null;
      if(!$force && count($this->recent_edit_entries) > 0) return;

      $file_path = $this->base_path;
      if(file_exists($file_path))
      {
         $fcontents = implode("", file($file_path));
         $recent_edit_entries = array();
         eval('?>'.$fcontents.'<?');
         if(count($recent_edit_entries) > 0)
         {
            $this->recent_edit_entries = $recent_edit_entries;
         }
         $this->loaded = true;
      } else {
        if ($system_config->user_error_reporting >= 2)
            echo("<p>Cannot find publicedit cache file:$this->base_path</p>");
      }
   }
   # ' 

   function markStoryForDelete($t_story_id)
   {
      if ($this->loaded == false) $this->load();

      for($j=0; $j < count($this->recent_edit_entries); $j++)
      {
         $ptr = $this->recent_edit_entries[$j];
         if ( $ptr->story_id == $t_story_id) 
         {
             $this->recent_edit_entries[$j]->save_it = false;
             return true;
         }
      }
      return false;
   }
   function isStoryInList($t_story_id)
   {
      if ($this->loaded == false) $this->load();

      for($j=0; $j < count($this->recent_edit_entries); $j++)
      {
         $ptr = $this->recent_edit_entries[$j];
         if ( $ptr->story_id == $t_story_id) 
         {
             return true;
         }
      }
      return false;
   }

   // Only return an entry if password is set and it hasn't expired
   function getEntryForStory($t_story_id)
   {
      $time_now = time();
      for($j=0; $j < count($this->recent_edit_entries); $j++)
      {
         $ptr = $this->recent_edit_entries[$j];
         if ( $ptr->story_id == $t_story_id) 
         {
             if ( ($ptr->init_time - $time_now + $this->edit_hours) <= 0) continue;
             {
                 return $ptr;
	     }
         }
      }
      return null;
   }

   function getEntryForSession($t_session, $t_story_id)
   {
      global $system_config;

      $time_now = time() - $system_config->timezone_offset;
      for($j=0; $j < count($this->recent_edit_entries); $j++)
      {
         $ptr = $this->recent_edit_entries[$j];
         // Expire days already converted to secs.
	 // echo "j=".$j." Match ".  $ptr->story_id ." to ". $t_story_id. "<BR>";
         if ( $ptr->story_id == $t_story_id) 
         {
             if ( ($ptr->init_time - $time_now + $this->edit_hours) <= 0) continue;
		 
             if ( $ptr->user_session == $t_session) 
             {
                 // Found ....
                 return $ptr;
	     }
         }
      }
      // Not found
      return null;
   }

   // Check the expiry time too.
   function canEditThisStory($t_story_id)
   {
      if ($t_story_id == null) return;

      for($j=0; $j < count($this->recent_edit_entries); $j++)
      {
         $ptr = $this->recent_edit_entries[$j];
         if ( $ptr->story_id == $t_story_id) 
         {
	     echo "id=".$ptr->story_id." checking init_time + expire ".($ptr->init_time + $this->expire_days)." is less than ". time()."<BR>";
             if ( ($ptr->init_time + $this->expire_days) > time() )
             {
                 return true;
	     }
         }
      }

      return false;
   }


   function discardOldRecords()
   {
      // No point calling the time() in every loop
      $time_now = time();

      for($j=0; $j < count($this->recent_edit_entries); $j++)
      {
         $ptr = $this->recent_edit_entries[$j];
         // Expire days already converted to secs.
         if ( ($ptr->init_time + $this->expire_days) < $time_now) 
         {
             // Beware for other arrays, to modify, you have to change it like this. Other ways don't work!
             $this->recent_edit_entries[$j]->save_it = false;
         }
      }
   }

   function updateTime($story_id, $init_time, $password, $w_session) 
   {
      if ($this->use_password == true )
          $t_obj = $this->getEntryForStory($story_id);
      else
          $t_obj = $this->getEntryForSession($w_session, $story_id);

      if ($t_obj != null) {
          $t_obj->init_time = $init_time;
          $t_obj->save_it = true;
          return true;
      }
      return false;
   }

   function add($story_id, $init_time, $granted_by, $password, $w_session, $pass_enable=0) 
   {
      if ($this->updateTime($story_id, $init_time, $password, $w_session) == true) return;

      $new_entry = new EditEntry($story_id, $init_time, $granted_by, $password, $w_session, $pass_enable);
      $this->recent_edit_entries[] = $new_entry;
   }

   function save()
   {
      $file = $this->base_path;
      $fp = fopen($file,"w");
      if (flock($fp, LOCK_EX))
      {
         fputs($fp, '<?php '."\n");
         foreach($this->recent_edit_entries as $each_entry)
         {
            // Only if it is true should we save it.
            // if ($each_entry->save_it == true) echo "Saving id ".$each_entry->story_id."<BR>";
            if ($each_entry->save_it == true) fputs($fp, $each_entry->getSerialisedPHP().";\n");
         }
         // unlocking file
         fputs($fp, '?>'."\n");
         flock($fp, LOCK_UN);
         fclose($fp);
      }
      else echo "<P>failed to get lock on reminder cache file $this->base_path</p>";
   }
}

?>
