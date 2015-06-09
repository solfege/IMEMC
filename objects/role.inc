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

class Role
{
    var $role_id;
    var $role_name;
    var $role_type;
    var $role_description;
    var $permissions;

    function getRoleIDList()
    {
        global $prefix, $dbconn;

        $roles = array();
        $result = sql_query("SELECT role_id from ".$prefix."_roles", $dbconn, 1);
        checkForError($result);
        if(sql_num_rows( $result ) > 0)
        {
           for ($i=0; $i<sql_num_rows( $result ); $i++)
           {

              $roles[$i] = sql_fetch_row($result, $dbconn);
           }
        }
        return $roles;
    }

    function getRoleNameList()
    {
        global $prefix, $dbconn;

        $roles = array();
        $result = sql_query("SELECT role_name from ".$prefix."_roles", $dbconn, 1);
        checkForError($result);
        if(sql_num_rows( $result ) > 0)
        {
           for ($i=0; $i<sql_num_rows( $result ); $i++)
           {

              $roles[$i] = sql_fetch_row($result, $dbconn);
           }
        }
        return $roles;
    }


    function deletePermissions()
    {
        $permissions = $this->getPermissions();
        for($i=0;$i<count($permissions);$i++)
        {
            $permissions[$i]->delete();
        }
    }

     function savePermissions()
     {
        $permissions = $this->getPermissions();
        for($i=0;$i<count($permissions);$i++)
        {
            $permissions[$i]->save();
        }
    }

    function getPermissions()
    {
        if($this->permissions==null)
        {
            global $prefix, $dbconn;
            $result = sql_query("SELECT permission_id, role_id, page, readonly from ".$prefix."_role_permissions WHERE role_id = $this->role_id", $dbconn, 1);
            checkForError($result);
            $permissions = array();
            if(sql_num_rows( $result ) > 0)
            {
                for ($i=0; $i<sql_num_rows( $result ); $i++)
                {
                    $permissions[$i] = new EditorPermission();
                    list($permissions[$i]->permission_id, $permissions[$i]->role_id, $permissions[$i]->page,$permissions[$i]->readonly) = sql_fetch_row($result, $dbconn);
                }
            }
            $this->permissions = $permissions;
        }
        return $this->permissions;
    }

    function loadPermissionsFromForm()
    {
        global $permissionsList;
        $this->permissions=array();
        foreach($permissionsList[$this->role_type] as $perm)
        {
            $this->loadPermissionFromForm($perm[0]);
        }
    }

    function loadPermissionFromForm($page)
    {
        $permission= new EditorPermission();
        $permission->role_id=$this->role_id;
        $permission->page=$page;
        if($_REQUEST[$page.'permission']=="readwrite")
        {
            $permission->readonly=false;
            array_push($this->permissions,$permission);
        }
        else if($_REQUEST[$page.'permission']=="readonly")
        {
            $permission->readonly=true;
            array_push($this->permissions,$permission);
        }
    }


    function save()
    {
        global $prefix, $dbconn;
        if($this->role_id == NULL)
        {
            checkForError(sql_query("INSERT INTO ".$prefix."_roles values (NULL, '$this->role_name', '$this->role_type', '$this->role_description')", $dbconn, 1));
            $this->role_id = mysql_insert_id($dbconn);
        }
        else
        {
            checkForError(sql_query("UPDATE ".$prefix."_roles SET role_name = '$this->role_name' WHERE role_id = $this->role_id", $dbconn, 1));
            checkForError(sql_query("UPDATE ".$prefix."_roles SET role_description = '$this->role_description' WHERE role_id = $this->role_id", $dbconn, 1));
            checkForError(sql_query("UPDATE ".$prefix."_roles SET role_type = '$this->role_type' WHERE role_id = $this->role_id", $dbconn, 1));
        }
    }


    function load()
    {
        if($this->role_id != NULL)
        {
            global $prefix, $dbconn;
            $result = sql_query("SELECT role_id, role_name, role_type, role_description from ".$prefix."_roles WHERE role_id = $this->role_id", $dbconn, 1);
            checkForError($result);
            if(sql_num_rows( $result ) > 0)
            {
                list($this->role_id, $this->role_name, $this->role_type, $this->role_description ) = sql_fetch_row($result, $dbconn);
                $this->role_description = stripslashes($this->role_description);
            }
            else
            {
                reportError("No role with ID $this->role_id exists");
            }
        }
        else
        {
            reportError("The role id is null");
        }
    }



    function delete()
    {
        if($this->role_id != NULL)
        {
            global $prefix, $dbconn;
            $result = sql_query("DELETE FROM ".$prefix."_roles WHERE role_id = $this->role_id", $dbconn, 1);
            checkForError($result);
        }
        else
        {
            reportError("The role id is null");
        }
    }

    function allowedReadAccessTo($page)
    {
        $permissions = $this->getPermissions();
        for($i=0;$i<count($permissions);$i++)
        {
            if($permissions[$i]->page==$page) return true;
        }
        return false;
    }

    function allowedWriteAccessTo($page)
    {
        $permissions = $this->getPermissions();
        for($i=0;$i<count($permissions);$i++)
        {
            if($permissions[$i]->page==$page && $permissions[$i]->readonly==false) return true;
        }
        return false;
    }

    function getStrippedName()
    {
       return strtolower(str_replace(array("\\","/"," ","-","_"),"",$this->role_name));
    }
}

Class RoleList
{
   var $roles;
   function load()
   {
      global $prefix, $dbconn;
      $result = sql_query("SELECT role_id, role_name, role_type, role_description from ".$prefix."_roles", $dbconn, 1);

      checkForError($result);
      if(sql_num_rows( $result ) > 0)
      {
         $this->roles = array();
         for ($i=0; $i<sql_num_rows( $result ); $i++)
         {
             $this->roles[$i] = new Role();
             list($this->roles[$i]->role_id, $this->roles[$i]->role_name, $this->roles[$i]->role_type, $this->roles[$i]->role_description) = sql_fetch_row($result, $dbconn);
             $this->roles[$i]->role_description = stripslashes($this->roles[$i]->role_description);
         }
      }
      else
      {
         reportError("No Roles present in database");
      }
   }

   function getRoles()
   {
      if($this->roles==NULL) $this->load();
      return $this->roles;
   }

   function getRoleByName($name)
   {
      if($this->roles==NULL) $this->load();
      for ($i=0; $i<count($this->roles); $i++)
      {
         if (!strcasecmp($this->roles[$i]->role_name,$name)) return $this->roles[$i];
      }
      return NULL;
   }


   function getroleByID($role_id)
   {
      if($this->roles==NULL) $this->load();
      for ($i=0; $i<=count($this->roles); $i++)
      {
         if($this->roles[$i]->role_id == $role_id) return $this->roles[$i];
      }
      return NULL;
   }

   function getRolesByType($role_type)
   {
      if($this->roles==NULL) $this->load();
      $relevant_roles = array();
      foreach($this->roles as $role)
      {
         if($role->role_type == $role_type)
            $relevant_roles[] = $role;
      }
      return $relevant_roles;
   }

   function reset()
   {
      $this->roles = NULL;
   }

   function dump($dir, $ids = null)
   {
      foreach($this->roles as $r)
      {
         $r->getPermissions();
         if($ids === null or in_array($r->role_id, $ids))
         {
            $file = $dir."/".$r->getStrippedName().".dump";
            $fp = fopen($file,"w");
            if (flock($fp, LOCK_EX))
            {
               fputs($fp, serialize($r));
               flock($fp, LOCK_UN);
               fclose($fp);
            }
            else
            {
               reportError("<P>failed to get lock on export dump file $file</p>");
               return false;
            }
         }
      }
      return true;
   }
}

$roleList= new RoleList();

?>
