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

class Editor
{
    var $editor_id;
    var $editor_name;
    var $editor_email;
    var $editor_lastlogin;
    var $editor_details=NULL;
    var $permissions;
    var $parameterised_permissions;
    var $roles;
    var $editor_state = array();

    function validatePassword($password)
    {
       global $prefix, $dbconn;
       $password=mysql_escape_string($password);
       $result = sql_query("SELECT editor_id FROM ".$prefix."_editors WHERE editor_name = '$this->editor_name' and editor_password = MD5('$password')", $dbconn, 1);
       checkForError($result);
       if(sql_num_rows( $result ) > 0)
       {
          list($this->editor_id) = sql_fetch_row($result, $dbconn);
          return true;
       }
       else return false;
    }

    function validateUsername($username)
    {
       global $prefix, $dbconn;
       $username=mysql_escape_string($username);
       $result = sql_query("SELECT editor_id FROM ".$prefix."_editors WHERE editor_name = '$username'", $dbconn, 1);
       checkForError($result);
       if(sql_num_rows( $result ) > 0)
       {
          list($this->editor_id) = sql_fetch_row($result, $dbconn);
          return true;
       }
       else return false;
    }

    function deleteRoles()
    {
       global $dbconn, $prefix;
       $result = sql_query("DELETE FROM ".$prefix."_editor_roles WHERE editor_id = ".$this->editor_id, $dbconn, 1);
       checkForError($result);
       $this->roles = null;
       return true;
       /*$roles = $this->getRoles();
        for($i=0;$i<count($roles);$i++)        {            $editor_role = new EditorRole();            $result = sql_query("SELECT editor_role_id FROM ".$prefix."_editor_roles WHERE editor_id = ".$this->editor_id." and role_id =". $roles[$i]->role_id, $dbconn, 1);
            checkForError($result);
            if(sql_num_rows( $result ) > 0)
            {
                list($editor_role->editor_role_id) = sql_fetch_row($result, $dbconn);
                $editor_role->load();
                $editor_role->delete();
                return true;
            }
       }*/
    }

    function saveRoles()
    {
       foreach(array_keys($this->roles) as $role_site_id)
       {
          foreach($this->roles[$role_site_id] as $role)
          {
             $editor_role = new EditorRole();
             $editor_role->role_id = $role->role_id;
             $editor_role->editor_id = $this->editor_id;
             $editor_role->role_site_id = $role_site_id;
             $editor_role->save();
          }
       }
    }

    function loadRoles($force=false)
    {
       if($this->roles==null or $force)
       {
           global $prefix, $dbconn;
           $this->roles = array();
           $sql_str = "SELECT DISTINCT(role_site_id) from ".$prefix."_editor_roles WHERE editor_id = $this->editor_id";
           $result = sql_query($sql_str, $dbconn, 1);
           checkForError($result);
           if(sql_num_rows( $result ) > 0)
           {
              for ($i=0; $i<sql_num_rows( $result ); $i++)
              {
                 list($site_id) = sql_fetch_row($result, $dbconn);
                 $this->roles[$site_id] = array();
              }
           }
           foreach(array_keys($this->roles) as $site_id)
           {
              $sql_str2 = "SELECT DISTINCT(role_id) from ".$prefix."_editor_roles WHERE editor_id = $this->editor_id and role_site_id='$site_id'";
              $result2 = sql_query($sql_str2, $dbconn, 1);
              checkForError($result2);

              if(sql_num_rows( $result2 ) > 0)
              {
                 for ($i=0; $i<sql_num_rows( $result2 ); $i++)
                 {
                    list($role_id) = sql_fetch_row($result2, $dbconn);
		    // There seems to be a bug with either MySql or PHP when two DISTINCT queries are done 
                    $rl = new Role;
                    $rl->role_id = $role_id;
                    // If you do this load which is another query inside the SELECT DISTINCT cursor it does not work
                    // $rl->load();
                    if(!isset($this->roles[$site_id])) $this->roles[$site_id] = array();
                    $this->roles[$site_id][] = $rl;
                 }
                 // sql_free_result($result2);
		 // Do the loads outside the cursor
                 foreach ($this->roles[$site_id] as $t_role)
                 {
                     $t_role->load();
                 }
              }
           }
       }
       return true;
    }

    function loadPermissions($force = false)
    {
       if($this->permissions!=null and !$force)
       {
          return true;
       }
       global $prefix, $dbconn;
       //1. get all role ids
       $this->permissions = array();
       $this->permissions['admin'] = array();
       $this->loadRoles($force);
       if(count($this->roles)==0)
       {
          return;
       }
       $all_role_ids = array();
       $site_role_map = array();
       foreach(array_keys($this->roles) as $role_site_id)
       {
          foreach($this->roles[$role_site_id] as $role)
          {
             if(!isset($site_role_map[$role_site_id])) $site_role_map[$role_site_id] = array();
             $site_role_map[$role_site_id][] = $role->role_id;
             if(!in_array($role->role_id, $all_role_ids))
             {
                $all_role_ids[] = $role->role_id;
             }
          }
       }

       //2. get all permissions associated with the various role ids
       $where_clause = "role_id = ".implode(" or role_id =", $all_role_ids);
       $result = sql_query("SELECT permission_id, role_id, page, readonly from ".$prefix."_role_permissions WHERE " . $where_clause, $dbconn, 1); 
       checkForError($result);
       if(sql_num_rows( $result ) > 0)
       {
          for ($i=0; $i<sql_num_rows( $result ); $i++)
          {
             $new_perm = new EditorPermission();
             list($new_perm->permission_id, $new_perm->role_id, $new_perm->page, $new_perm->readonly) = sql_fetch_row($result, $dbconn);
             //3. put the permission into the relevant site array part of the permissions array..
             foreach(array_keys($site_role_map) as $role_site_id)
             {
                if(in_array($new_perm->role_id, $site_role_map[$role_site_id]))
                {
                   if(!isset($this->permissions[$role_site_id]))
                   {
                      $this->permissions[$role_site_id] = array();
                   }
                   if(!isset($this->permissions[$role_site_id][$new_perm->page]))
                   {
                      $this->permissions[$role_site_id][$new_perm->page] = $new_perm;
                   }
                   elseif($this->permissions[$role_site_id][$new_perm->page]->readonly == true && $new_perm->readonly==false)
                   {
                      $this->permissions[$role_site_id][$new_perm->page]->readonly = false;
                      $this->permissions[$role_site_id][$new_perm->role_id] = $new_perm->role_id;
                   }
                }
             }
          }
       }
       return true;
    }

    function loadRolesFromForm($role_type, $site_id)
    {
        global $roleList;
        $roles = $roleList->getRolesByType($role_type);
        foreach($roles as $role)
        {
            $this->loadRoleFromForm($role, $site_id);
        }
    }

    function loadRoleFromForm(&$role, $site_id)
    {
       $form_entry = "site_".$site_id."_role_".$role->role_id;
       if($_REQUEST[$form_entry]=="yes")
       {
          if(!isset($this->roles[$site_id])) $this->roles[$site_id] = array();
          $this->roles[$site_id][] = $role;
       }
    }

    function allowedReadAccessTo($page, $site_id='admin', $disp=0)
    {
       $this->loadPermissions();
       if(!isset($this->permissions[$site_id]))
       {
           return false;
       }
       //speed up for read permissions (nice)
       if(is_array($this->permissions[$site_id]) && in_array($page, array_keys($this->permissions[$site_id]))) return true;
       return false;
    }

   function allowedWriteAccessTo($page, $site_id ='admin')
   {
      $this->loadPermissions();
      if(!isset($this->permissions[$site_id]) or !(is_array($this->permissions[$site_id])))
      {
         return false;
      }
      foreach($this->permissions[$site_id] as $perm)
      {
         if($perm->page==$page and !$perm->readonly) return true;
      }
      return false;
   }

   function containsRoleForSite($site_id)
   {
      if(isset($this->roles[$site_id])) return true;
      return false;
   }

   function possessesRole($role_id, $site_id)
   {
      $this->loadRoles();
      if(!isset($this->roles[$site_id]))
      {
         return false;
      }
      foreach ($this->roles[$site_id] as $r)
      {
         if($r->role_id == $role_id)
         {
            return true;
         }
      }
      return false;
   }

   function save($password)
   {
      global $prefix, $dbconn;
      if($this->editor_id == NULL)
      {
         checkForError(sql_query("INSERT INTO ".$prefix."_editors values (NULL, '$this->editor_name', MD5('$password'), '$this->editor_email',NULL,'$this->editor_details')", $dbconn, 1));
         $this->editor_id = mysql_insert_id($dbconn);
      }
      else
      {
         if($password==null)
         {
            checkForError(sql_query("UPDATE ".$prefix.  "_editors SET editor_name = '$this->editor_name', editor_email = '$this->editor_email', editor_details='$this->editor_details', editor_lastlogin = editor_lastlogin WHERE editor_id = $this->editor_id", $dbconn, 1));
         }
         else
         {
            checkForError(sql_query("UPDATE ".$prefix."_editors SET editor_name = '$this->editor_name', editor_password = MD5('$password'), editor_email = '$this->editor_email', editor_details='$this->editor_details', editor_lastlogin = editor_lastlogin WHERE editor_id = $this->editor_id", $dbconn, 1));
         }
      }
   }

   function updateLoginTime($current_time=0)
   {
      global $prefix, $dbconn;

      if($this->editor_id != NULL)
      {
         if($current_time == 0) $login_time="FROM_UNIXTIME(".time().")";
	 else $login_time="FROM_UNIXTIME(". $current_time .")";

	 // Note the 2nd quotes rather than single quote is important, otherwise the login time is quoted
	 // Note also the cache group IS ZERO. During invalidation of the cache, tbe zero will be caught and
	 // no invalidation will take place. This is about the only SQL statement where this is okay to do.
         checkForError(sql_query("UPDATE ".$prefix.  "_editors SET editor_lastlogin = ".$login_time." WHERE editor_id = $this->editor_id", $dbconn, 0));
      }
   }


    function load()
    {
       if($this->editor_id != NULL)
       {
          global $prefix, $dbconn;
          $result = sql_query("SELECT editor_id, editor_name, editor_email, UNIX_TIMESTAMP(editor_lastlogin), editor_details from ".$prefix."_editors WHERE editor_id = $this->editor_id", $dbconn, 1);
          checkForError($result);
          if(sql_num_rows( $result ) > 0)
          {
             list($this->editor_id, $this->editor_name,$this->editor_email, $this->editor_lastlogin, $this->editor_details) = sql_fetch_row($result, $dbconn);
          }
          else
          {
             reportError("No editor with ID $this->editor_id exists");
          }
       }
       else
       {
          reportError("The editor id is null");
       }
    }

    function delete()
    {
       if($this->editor_id != NULL)
       {
          global $prefix, $dbconn;
          $result = sql_query("DELETE FROM ".$prefix."_editors WHERE editor_id = $this->editor_id", $dbconn, 1);
          checkForError($result);
       }
       else
       {
          reportError("The editor id is null");
       }
    }
}

Class EditorList
{
    var $editors;
    var $start_limit = 0;
    var $page_size =25;
    var $grand_total;
    var $sort_on = false;
    var $login_sort_on = false;

    function setLoginSort()
    {
       // Off by default;
       $this->login_sort_on = true;
    }


    function setSort()
    {
       // Off by default;
       $this->sort_on = true;
    }

    function load($load_all = false)
    {
       global $prefix, $dbconn;
       $editors_query = "SELECT editor_id, editor_name, editor_email, UNIX_TIMESTAMP(editor_lastlogin), editor_details from ".$prefix."_editors ";

       if ($this->sort_on == true) {
           $editors_query .= " ORDER BY editor_name ASC";
       } elseif ($this->login_sort_on == true) {
           $editors_query .= " ORDER BY editor_lastlogin DESC";
       }

       if ($load_all == false) {
           $editors_query .= " LIMIT ".$this->start_limit.", ".$this->page_size;
       }

       $result = sql_query($editors_query, $dbconn, 0);
       checkForError($result);
       if(sql_num_rows( $result ) > 0)
       {
          $this->editors = array();
          for ($i=0; $i<sql_num_rows( $result ); $i++)
          {
             $this->editors[$i] = new Editor();
             list($this->editors[$i]->editor_id, $this->editors[$i]->editor_name,$this->editors[$i]->editor_email,$this->editors[$i]->editor_lastlogin, $this->editors[$i]->editor_details) = sql_fetch_row($result, $dbconn);
          }
       }
       else
       {
           reportError("No Editors present in database");
       }
    }

    function getEditors($load_page = 1)
    {
       if($this->editors==NULL) {
          // LIMIT is zero based on the row
          $this->start_limit = (($load_page -1) * $this->page_size);
          $this->load(false);
       }
       return $this->editors;
    }
    
    // Get the total loaded because we can load a page of them.
    function getEditorTotal()
    {
       if($this->editors==NULL) return 0;
       return count($this->editors);
    }

    function getPageSize()
    {
       return $this->page_size;
    }


    // Get the total from the database for case when we really have a lot which would be rarely.
    function getEditorGrandTotal()
    {
       global $prefix, $dbconn;
       $result = sql_query("SELECT count(*) from ".$prefix."_editors", $dbconn, 1);
       checkForError($result);
       if(sql_num_rows( $result ) > 0)
       {
          list($this->grand_total) = sql_fetch_row($result, $dbconn);
          return $this->grand_total;
       }
       else
       {
           reportError("No Editors present in database");
       }
       return 0;
    }

    function getEditorByName($name)
    {
       // If there were more than 100s of editors, then this function should be changed to
       // do this by query rather than loading them all into memory.
       // Load list of editors without page, and force reload if only a page full loaded.
       if($this->editors==NULL) $this->load(true);
       else if($this->grand_total > $this->page_size) $this->load(true);

       for ($i=0; $i<=count($this->editors); $i++)
       {
          if (!strcasecmp($this->editors[$i]->editor_name,$name)) return $this->editors[$i];
       }
       return NULL;
    }

    function getEditorByID($editor_id)
    {
       // If there were more than 100s of editors, then this function should be changed to
       // do this by query rather than loading them all into memory.
       // Load list of editors without page, and force reload if only a page full loaded.
       if($this->editors==NULL) $this->load(true);
       else if($this->grand_total > $this->page_size) $this->load(true);

       for ($i=0; $i<=count($this->editors); $i++)
       {
          if($this->editors[$i]->editor_id == $editor_id) return $this->editors[$i];
       }
       return NULL;
    }

    function reset()
    {
       $this->editors = NULL;
       $this->start_limit = 0;
    }
}

?>
