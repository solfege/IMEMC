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

class EditLock
{
   var $lock_id;
   var $target_type;
   var $target_id;
   var $lock_owner;
   var $icon;
   var $lock_time_granted;
   var $lock_expiry_time;
   var $result_message;

   function EditLock()
   {
      global $graphics_store;
      $this->lock_id = 0;
      $this->icon = "<img src='$graphics_store"."lock.gif'> ";
   }

   function copyValues($other)
   {
      $this->lock_id = $other->lock_id;
      $this->target_type = $other->target_type;

      $this->target_id = $other->target_id;
      $this->lock_owner = $other->lock_owner;
      $this->icon = $other->icon;
      $this->lock_time_granted = $other->lock_time_granted;
      $this->lock_expiry_time = $other->lock_expiry_time;
      $this->result_message = $other->result_message;
   }

   function lockItem($type, $id)
   {
      global $editor_session, $system_config;
      $this->target_type = $type;
      $this->target_id = $id;
      // If article is being edited by public then use 9900
      if ($editor_session->isSessionOpen()) $this->lock_owner = $editor_session->editor->editor_id;
      else $this->lock_owner = 9900;
      $this->lock_time_granted = time();
      $this->lock_expiry_time = $this->lock_time_granted + $system_config->edit_locking_time;
      if($this->lockExists($type, $id))
      {
         $existing_lock = new EditLock();
         $existing_lock->load($type, $id);
         if($existing_lock->lock_owner == $this->lock_owner)
         {

            if($existing_lock->isValid())
            {
               $this->copyValues($existing_lock);
               return true;
            }
            elseif($system_config->forbid_consecutive_locks_without_save)
            {
               $this->copyValues($existing_lock);
               return false;
            }
            else
            {
               $existing_lock->delete();
            }
         }
         elseif(!$existing_lock->isValid())
         {
            $existing_lock->delete();
         }
         else
         {
            $this->result_message = $existing_lock->getAsFailureMessage();
            return false;
         }
      }
      return $this->save();
   }

   function canUpdate($type, $id)
   {
      global $editor_session, $system_config;
      $this->target_type = $type;
      $this->target_id = $id;
      // If article is being edited by public then use 9900
      if ($editor_session->isSessionOpen()) $this->lock_owner = $editor_session->editor->editor_id;
      else $this->lock_owner = 9900;

      $this->lock_time_granted = time();
      $this->lock_expiry_time = $this->lock_time_granted + $system_config->edit_locking_time;
      if($this->isUsable())
      {
         unset($this->result_message);
         return true;
      }
      return false;
   }

   function isValid()
   {
      if($this->lock_expiry_time > time() && $this->lock_id > 0) return true;
      return false;
   }

   function isUsable()
   {
      if($this->isValid()) return true;
      if(!$this->lockExists($this->target_type, $this->target_id))
      {
         $this->result_message = $this->icon."Your edit lock for $this->target_type $this->target_id has expired.  Since the time that it has expired somebody else may have updated it.  You should hit the back button, save any of your changes in a text editor and start editing from the beginning and merge your changes with any other new edits";
         return false;
      }
      $existing_lock = new EditLock();
      $existing_lock->load($this->target_type, $this->target_id);
      if($existing_lock->lock_owner != $this->lock_owner)
      {
         $this->result_message = "Your lock expired! ". $existing_lock->getAsFailureMessage();
         return false;
      }
      $this->result_message = $this->icon."Your edit lock for $this->target_type $this->target_id has expired.  You should save your changes immediately, before anybody else can lock this $this->target_type and prevent you from saving your work.";
      return true;
   }

   function getAsFailureMessage()
   {
      global $system_config, $editor_session;
      require_once "objects/editor.inc";
      $editorList = new editorList();
      $editorList->load();
      // If article is being edited by public then use 9900
      if ($this->lock_owner == 9900) {
          $lock_by_name = "Joe Public";
      } else {
          $ed = $editorList->getEditorByID($this->lock_owner);
          $lock_by_name = $ed->editor_name;
      }
      

      $str = $this->icon."$this->target_type $this->target_id is locked by <b>$lock_by_name</b>. The lock was granted at ".strftime("%H:%M:%S", $this->lock_time_granted)." and will remain active until ".strftime("%H:%M:%S", $this->lock_expiry_time). " or until $lock_by_name saves their work, whichever comes first.</p>";
      if ($editor_session->isSessionOpen() ) {
          $str .= "<p> Click <a href='http://".$system_config->site_url."/editorstatus.php'>here</a> to communicate with that editor.";
      }
      return $str;
   }

   function delete()
   {
      global $prefix, $dbconn;
       $result = sql_query("DELETE FROM ".$prefix.   "_edit_locks WHERE target_id =$this->target_id and target_type='$this->target_type'", $dbconn, 3);
      if(!$result)
      {
         $this->result_message = "Failed to delete old lock from database : ".mysql_errno()." - ".mysql_error();
         return false;
      }
      return true;
   }

   function cleanUp()
   {
      global $prefix, $dbconn;
      $cutoff = time() - 60*60*3;//delete all older than 3 hours to tidy up
       $result = sql_query("DELETE FROM ".$prefix.   "_edit_locks WHERE UNIX_TIMESTAMP(lock_expiry_time) < $cutoff", $dbconn, 3);
      if(!$result)
      {
         $this->result_message = "Failed to delete old locks from database : ".mysql_errno()." - ".mysql_error();
         return false;
      }
      return true;
   }

   function save()
   {
      global $prefix, $dbconn;
      $result = sql_query("INSERT INTO ".$prefix.   "_edit_locks values (null, $this->target_id, '$this->target_type', $this->lock_owner, FROM_UNIXTIME($this->lock_time_granted), FROM_UNIXTIME($this->lock_expiry_time))", $dbconn, 3);
      checkForError($result);
      if(!$result)
      {
         $this->result_message = "Failed to insert lock into database : ".mysql_errno()." - ".mysql_error();
         return false;
      }
      $this->lock_id = mysql_insert_id($dbconn);
      return true;
   }

   function load($type, $id=0)
   {
      global $prefix, $dbconn;

      // If it is zero we will want to pick up all the locked objects of that type.
      if ($id == 0 )
          $result = sql_query("SELECT lock_id, target_id, target_type, lock_owner, UNIX_TIMESTAMP(lock_time_granted), UNIX_TIMESTAMP(lock_expiry_time) FROM ".$prefix.   "_edit_locks WHERE target_type='$type'", $dbconn, 3);
      else
          $result = sql_query("SELECT lock_id, target_id, target_type, lock_owner, UNIX_TIMESTAMP(lock_time_granted), UNIX_TIMESTAMP(lock_expiry_time) FROM ".$prefix.   "_edit_locks WHERE target_type='$type' and target_id=$id", $dbconn, 3);
      if(!$result)
      {
         $this->result_message = "Failed to load lock from database for $target_type $id: ".mysql_errno()." - ".mysql_error();
         return false;
      }
      if(sql_num_rows( $result ) > 0)
      {
         // Orignal hack when Id was specified was basically to get the last, but code updated now to get list.
         if ($id > 0 ) {
            list($this->lock_id, $this->target_id, $this->target_type, $this->lock_owner, $this->lock_time_granted, $this->lock_expiry_time) = sql_fetch_row($result, $dbconn);
            return true;

         } else {
            $locksList = array();
            for ($i=0; $i<sql_num_rows( $result ); $i++)
            {
                $locksList[$i] = new EditLock();
                list($locksList[$i]->lock_id, $locksList[$i]->target_id, $locksList[$i]->target_type, $locksList[$i]->lock_owner, $locksList[$i]->lock_time_granted, $locksList[$i]->lock_expiry_time) = sql_fetch_row($result, $dbconn);
            }
            return $locksList;
         }
      }
   }

   function lockExists($type, $id)
   {
      global $prefix, $dbconn;
      $result = sql_query("SELECT lock_id FROM ".$prefix.   "_edit_locks WHERE target_type='$type' and target_id=$id", $dbconn, 3);
      if(!$result)
      {
         $this->result_message = "Failed in check for lock from database for $this->target_type $id: ".mysql_errno()." - ".mysql_error();
         return false;
      }
      if(sql_num_rows( $result ) > 0)
      {
         return true;
      }
      return false;
   }


}
