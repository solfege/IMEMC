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

class EditorPermission
{
    var $permission_id;
    var $role_id;
    var $page;
    var $readonly;

    function save()
    {
        global $prefix, $dbconn;
        if($this->permission_id == NULL)
        {
            if($this->readonly==false) $this->readonly="NULL";
            checkForError(sql_query("INSERT INTO ".$prefix."_role_permissions values (NULL, $this->role_id, '$this->page',$this->readonly)", $dbconn, 1));
            $this->permission_id = mysql_insert_id($dbconn);
            if($this->readonly=="NULL") $this->readonly=false;
        }
        else
        {
            checkForError(sql_query("UPDATE ".$prefix."_role_permissions SET role_id = $this->role_id, page = '$this->page', readonly = $this->readonly WHERE permission_id = $this->permission_id", $dbconn, 1));
        }
    }

    function load()
    {
        if($this->permission_id != NULL)
        {
            global $prefix, $dbconn;
            $result = sql_query("SELECT role_id, page, readonly from ".$prefix."_role_permissions WHERE role_id = $this->role_id", $dbconn, 1);
            checkForError($result);
            if(sql_num_rows( $result ) > 0)
            {
                list($this->role_id, $this->page,$this->readonly) = sql_fetch_row($result, $dbconn);
            }
            else
            {
                reportError("No Permission with ID $this->permission_id exists");
            }
        }
        else
        {
            reportError("The topic id is null");
        }
    }

    function delete()
    {
        if($this->permission_id != NULL)
        {
            global $prefix, $dbconn;
            $result = sql_query("DELETE FROM ".$prefix."_role_permissions WHERE permission_id = $this->permission_id", $dbconn, 1);
            checkForError($result);
        }
        else
        {
            reportError("The permission id is null");
        }
    }
}





class EditorRole
{
   var $editor_role_id;
   var $editor_id;
   var $role_id;
   var $role_site_id;

    function save()
    {
        global $prefix, $dbconn;

        if($this->editor_role_id == NULL)
        {
            checkForError(sql_query("INSERT INTO ".$prefix."_editor_roles values (NULL, $this->editor_id, $this->role_id, '$this->role_site_id')", $dbconn, 1));
            $this->editor_role_id = mysql_insert_id($dbconn);
        }

        else
        {
            checkForError(sql_query("UPDATE ".$prefix."_editor_roles SET editor_id = '$this->editor_id', role_id = '$this->role_id', role_site_id = '$this->role_site_id' WHERE editor_role_id = $this->editor_role_id", $dbconn, 1));
        }
    }



    function load()
    {
        if($this->editor_role_id != NULL)
        {
            global $prefix, $dbconn;
            $result = sql_query("SELECT editor_id, role_id, role_site_id from ".$prefix."_editor_roles WHERE editor_role_id = $this->editor_role_id", $dbconn, 1);
            checkForError($result);
            if(sql_num_rows( $result ) > 0)
            {
                list($this->editor_id, $this->role_id) = sql_fetch_row($result, $dbconn);
            }
            else
            {
                reportError("No Editor Role with ID $this->editor_role_id exists");
            }
        }
        else
        {
            reportError("The Role id is null");
        }
    }

    function delete()
    {
        if($this->editor_role_id != NULL)
        {
            global $prefix, $dbconn;
            $result = sql_query("DELETE FROM ".$prefix."_editor_roles WHERE editor_role_id = $this->editor_role_id", $dbconn, 1);
            checkForError($result);
        }
        else
        {
            reportError("The role id is null");
        }
    }

}
?>
