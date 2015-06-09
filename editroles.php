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

require_once("oscailt_init.inc");
require_once "objects/indyobjects/indydataobjects.inc";

$OSCAILT_SCRIPT = "editroles.php";

addToPageTitle("Manage user roles and their permissions");

// Function to return a list of editor names (in html) who have the target role
// It shows in brackets the site id 
function getEditorsWithRole($target_role_id)
{
    global $dbconn, $prefix;
    $html_str = "";

    // $sql_str1 = "SELECT e.editor_name, r.role_site_id, r.editor_id,r.role_site_id FROM ".$prefix."_editor_roles as r LEFT JOIN ".$prefix."_editors as e ON (e.editor_id = r.editor_id) WHERE r.role_id='$target_role_id' ORDER_BY r.editor_id, r.role_site_id";
    $sql_str1 = "SELECT e.editor_name, r.role_site_id, r.editor_id,r.role_site_id FROM ".$prefix."_editor_roles as r LEFT JOIN ".$prefix."_editors as e ON (e.editor_id = r.editor_id) WHERE r.role_id='$target_role_id' ";
    $result_role = sql_query($sql_str1, $dbconn, 1);
    checkForError($result_role);

    if(sql_num_rows( $result_role ) > 0)
    {
        $total_rows = sql_num_rows( $result_role );
        for ($iRow=0; $iRow < $total_rows; $iRow++)
        {
             list($editor_name, $site_id, $dummy1,$dummy2) = sql_fetch_row($result_role, $dbconn);
	     // There seems to be a bug with either MySql or PHP when two DISTINCT queries are done 
             $html_str .= $editor_name ." (".$site_id.")";
             if ($iRow < ($total_rows-1) ) $html_str .= "<BR>";
        }
    } else {
        return "";
    }
    return $html_str;
 
}

function writeRoleList()
{
    global $roleList, $OSCAILT_SCRIPT;

    if(isset($_REQUEST['sort_by_name']) && $_REQUEST['sort_by_name']=="true" ) {
        $sort_by_name_mode = "false";
        $sort_by_name = true;
    } else {
        $sort_by_name_mode = "true"; 
        $sort_by_name = false;
    }

    if(isset($_REQUEST['sort_by_type']) && $_REQUEST['sort_by_type']=="true" ) {
        $sort_by_type_mode = "false";
        $sort_by_type = true;
    } else {
        $sort_by_type_mode = "true"; 
        $sort_by_type = false;
    }

    $who_has = false;
    $max_col = 6;

    if ( isset($_REQUEST['show_who_has']) && $_REQUEST['show_who_has'] == 'true') {
       $swap_mode ="Hide Who Has Roles";
       $switch_mode ="show_who_has=false";
       $who_has = true;
       $max_col = 7;
    } else  {
       $swap_mode ="Show Who Has Roles";
       $switch_mode ="show_who_has=true";
    }


    ?>
    <TABLE align=center>
    <tr class=admin>
	<TD class='admin' colspan=<?=$max_col?>><a href="<?=$OSCAILT_SCRIPT?>?<?=$switch_mode?>"><?=$swap_mode?></a></TD>
    </TR>

    <tr class=admin>
        <th class=admin colspan=<?=$max_col?>>Roles</th>
    </tr>
    <tr class=admin>
        <th class=admin>&nbsp;#&nbsp;</th>
        <th class=admin>&nbsp;<a class='editor-option' href="<?=$OSCAILT_SCRIPT?>?sort_by_name=<?=$sort_by_name_mode?>">Name</a>&nbsp;</th> 
        <th class=admin>&nbsp;<a class='editor-option' href="<?=$OSCAILT_SCRIPT?>?sort_by_type=<?=$sort_by_type_mode?>">Type</a>&nbsp;</th> 
        <th class=admin>&nbsp;Description&nbsp;</th>
    <?
    if($who_has == true) {
        ?>
        <th class=admin>&nbsp;Who (Site)&nbsp;</th>
        <?
    }
    ?>
        <th class=admin>&nbsp;Edit&nbsp;</th>
        <th class=admin>&nbsp;Delete&nbsp;</th>
    </tr>
    <?
    $roleList->reset();
    $roles = $roleList->getRoles();

    for($i=0;$i<count($roles);$i++)
    {
      if($sort_by_name == true )
      {
         $role=$roles[$i];
         $rolesArray[] = $role->role_name;
      }
      elseif($sort_by_type == true )
      {
         $role=$roles[$i];
         $rolesArray[] = $role->role_type;
      }

      $array_order[] = $i;
    }
    // Sort by Type
    if ($sort_by_name == true || $sort_by_type == true )
    {
      array_multisort($rolesArray, $array_order);
    }

    for($i=0;$i<count($roles);$i++)
    {
        $sorted_index=$array_order[$i];
        $role=$roles[$sorted_index];
        ?>
        <tr class=admin>
            <td class=admin>&nbsp;<?=($i+1)?>&nbsp;</td>
            <td class=admin>&nbsp;<?=$role->role_name?>&nbsp;</td>
            <td class=admin align=center><?=$role->role_type?></td>
            <td class=admin align=center><?=$role->role_description?></td>
        <?
        if ($who_has == true) {
            $who_has_roles_list = getEditorsWithRole($role->role_id);
            ?>
            <td class=admin align=right><?=$who_has_roles_list?></td>
	    <?
        }
        ?>
            <td class=admin align=center><a href="editroles.php?subpage=edit&role_id=<?=$role->role_id?>"><img src='graphics/edit.gif' border=0></a></td>
            <td class=admin align=center><a href="editroles.php?subpage=delete&role_id=<?=$role->role_id?>"><img src='graphics/delete.gif' border=0></a></td>
        </tr>
        <?
    }
    ?>
    <tr class='admin'>
        <td colspan=<?=$max_col?> align=center>
           <form name="editrole_form" action="editroles.php" method=post>
           <br />
           <input type=hidden name="subpage" value="edit">
           <input type=submit name='create_role' value="Create New Role">
           <input type=hidden name='new_role' value="true">

           <select name='role_type'>
             <option value='editorial'>Editorial Role</option>
             <option value='admin'>Administrative Role</option>
             <option value='site'>Site Builder Role</option>
           </select>
           </form>
        </td>
    </tr>
    </table>
    <?
}

function writeError($error)
{
    ?><BR><BR><font class=error><B><?=$error?></B></font><BR><BR><?
}

function loadSitePermissionOptions()
{
   global $permissionsList, $path_prefix, $system_config;
   $typeSet = new indyDataTypeSet( $system_config->xmltypedef_dir );
   if( !$typeSet->load() || !$typeSet->loadAllTypes())
   {
      $error_messages = $typeSet->getUserMessages();
      $error_messages[] = array( INDY_ERROR,
         "Failed to load the set of data types $system_config->xmltypedef_dir","AAs" );
      echo "<P class='error'>";
      foreach($error_messages as $msg_arr)
      {
         echo $msg_arr[1]."<br />";
      }
      echo "</P>";
      //$this->setUserMessage ( array( INDY_ERROR,
         //"Failed to load the set of data types $this->typeDirectory","AAs" ) );
      return false;
   }
   foreach($typeSet->dataTypes as $type)
   {
      if($type->type == 'site') continue;
      $permissionsList['site'][] = array("createobj".$type->type, "<b>Create</b> New objects of type <b>$type->type</b> (".$type->getMeta("description").")");
      $permissionsList['site'][] = array("deleteobj".$type->type, "<b>Delete</b> Objects of type <b>$type->type</b>");
      $permissionsList['site'][] = array("editobj".$type->type, "<b>Manage</b> Existing Objects of type <b>$type->type</b>");
   }
}

function writeEditBox($is_from_form=false)
{
    global $permissionsList;
    $role = new Role();
    if(isset($_REQUEST['role_id']))
    {
       $role->role_id=cleanseNumericalQueryField($_REQUEST['role_id']);
       $role->load();
    }
    else
    {
       $role->role_type = cleanseTitleField($_REQUEST['role_type']);
    }
    if($role->role_type == 'site')
    {
       loadSitePermissionOptions();
    }
    ?>
    <table align=center class=admin>
    <form action="editroles.php" method=post>
    <input type="hidden" name="subpage" value="edit">
    <input type="hidden" name="role_type" value="<?=$role->role_type?>">
    <?
        if($role->role_id != null)
        {
            ?><input type=hidden name=role_id value="<?=$role->role_id?>"><?
        }
    ?>
    <tr class=admin>
        <th class=admin colspan=4>
        <?
        if($role->role_id != null) echo("Edit $role->role_name $role->role_type"."-role.");
        else echo("Create New $role->role_type Role");
        ?>
        </th>
    </tr>
    <tr class=admin>
        <td class=admin>&nbsp;<B>Name</B>&nbsp;</td>
        <td class=admin colspan=3><input name=role_name value="<?=$role->role_name?>"></td>
    </tr>
    <tr class=admin>
            <td class=admin>&nbsp;<B>Description</B>&nbsp;</td>
            <td class=admin colspan=3><input name=role_description value="<?=$role->role_description?>"></td>
    </tr>
    <tr class=admin>
        <th class=admin align=center>&nbsp;Permissions&nbsp;</th>
        <th class=admin align=center>&nbsp;No Access&nbsp;</th>
        <th class=admin align=center>&nbsp;Read Only&nbsp;</th>
        <th class=admin align=center>&nbsp;Read/Write&nbsp;</th>
    </tr>

    <?
    foreach($permissionsList[$role->role_type] as $perm){
        writePermission($role,$perm[0],$perm[1], $is_from_form);
    }

    if(isset($_REQUEST['new_role']) && $_REQUEST['new_role'] == 'true')
    {
       // Hidden variable so that at the end we know whether it was an create or update.
       ?> <input type=hidden name='new_role' value="true"> <?
    }

    ?>
    <tr>
        <td colspan=4 align=center>
        <input type=submit name=cancel value="&lt;&lt; Cancel">
        <input type=submit name=save value="Save &gt;&gt;">
        </td>
    </tr>
    </form>
    </table>
    <?
}

function writeConfirmDeleteBox()
{
    $role = new Role();
    $role->role_id=cleanseNumericalQueryField($_REQUEST['role_id']);
    $role->load();
    ?>

    <table align=center>
    <form action="editroles.php" method=post>
    <input type=hidden name=subpage value="delete">
    <input type=hidden name=role_id value="<?=$role->role_id?>"><?
    ?>
    <tr>
        <td colspan=2 align=center><img src="graphics/caution.gif" align=middle><BR><BR><B>Are you sure you wish to delete <?=$role->role_name?>?</B><BR><BR></td>
    </tr>
    <tr>
        <td align=right><input type=submit name=cancel value="&lt;&lt; Cancel"></td>
        <td><input type=submit name=confirm value="Delete &gt;&gt;"></td>
    </tr>
    </form>
    </table>
    <?

}



function writePermission($role, $page, $display_name, $is_from_form)
{
    ?>
    <tr class=admin>
        <td class=admin>&nbsp;<?=$display_name?>&nbsp;</td>
        <?
            if(($role->role_id != null && $role->allowedWriteAccessTo($page)) or ($is_from_form && isset($_REQUEST[$page."permission"]) && $_REQUEST[$page."permission"] == "readwrite"))
            {
                ?>
                <td class=admin align=center><input type=radio name=<?=$page?>permission value="noaccess"></td>
                <td class=admin align=center><input type=radio name=<?=$page?>permission value="readonly"></td>
                <td class=admin align=center><input type=radio name=<?=$page?>permission value="readwrite" checked></td>
                <?
            }
            elseif(($role->role_id != null && $role->allowedReadAccessTo($page)) or ($is_from_form && isset($_REQUEST[$page."permission"]) && $_REQUEST[$page."permission"] == "readonly"))
            {
                ?>
                <td class=admin align=center><input type=radio name=<?=$page?>permission value="noaccess"></td>
                <td class=admin align=center><input type=radio name=<?=$page?>permission value="readonly" checked></td>
                <td class=admin align=center><input type=radio name=<?=$page?>permission value="readwrite"></td>
                <?
            }
            else
            {
                ?>
                <td class=admin align=center><input type=radio name=<?=$page?>permission value="noaccess" checked></td>
                <td class=admin align=center><input type=radio name=<?=$page?>permission value="readonly"></td>
                <td class=admin align=center><input type=radio name=<?=$page?>permission value="readwrite"></td>
                <?
            }
        ?>
    </tr>
    <?
}

ob_start();

if($editor_session->isSessionOpen())
{
    if($editor_session->editor->allowedWriteAccessTo("editeditors"))
        writeAdminHeader("","", array("editeditors.php" =>"EditEditors") ); 
    else
        writeAdminHeader();

    if($editor_session->editor->allowedReadAccessTo("editroles"))
    {
        if(isset($_REQUEST['subpage']) && $_REQUEST['subpage']=="delete" && isset($_REQUEST['confirm']))
        {
            $role = new Role();
            $role->role_id=cleanseNumericalQueryField($_REQUEST['role_id']);
            if($role->role_id != null)
            {
                if($editor_session->editor->allowedWriteAccessTo("editroles"))
                {
                    $t_role_id = $role->role_id;
                    $role->load();
                    $role->deletePermissions();
                    $role->delete();
		    logAction(null, $t_role_id, "oscailt role", "delete role");
                }
                else $editor_session->writeNoWritePermissionError();
            }
            else $editor_session->writeNoWritePermissionError();
            writeRoleList();
        }
        else if(isset($_REQUEST['subpage']) && $_REQUEST['subpage'] =="delete" && isset($_REQUEST['cancel']))
        {
            writeRoleList();
        }
        else if(isset($_REQUEST['subpage']) && $_REQUEST['subpage']=="delete")
        {
            writeConfirmDeleteBox();
        }
        else if(isset($_REQUEST['subpage']) && $_REQUEST['subpage']=="edit" && isset($_REQUEST['save']))
        {
            $role= new Role();
            if(isset($_REQUEST['role_id'])) $role->role_id=cleanseNumericalQueryField($_REQUEST['role_id']);
            else $role->role_id = null;

            $role->role_name=cleanseTitleField($_REQUEST['role_name']);
            $role->role_description=trim(addslashes($_REQUEST['role_description']));
            if($role->role_name==null || $role->role_name=="")
            {
                writeError("Please Specify Name");
                writeEditBox(true);
            }

            else if($role->role_id==null && $roleList->getRoleByName($role->role_name)!=null)
            {
                writeError("A role with this name already exists!");
                writeEditBox(true);
            }
            elseif($editor_session->editor->allowedWriteAccessTo("editroles"))
            {
                if(!isset($_REQUEST['role_type']))
                {
                   writeError("No Role Type Specified!");
                   writeRoleList();
                }
                $role->role_type = cleanseTitleField($_REQUEST['role_type']);
                if($role->role_type == 'site')
                {
                   loadSitePermissionOptions();
                }
                $role->save(null);
                $role->deletePermissions();
                $role->loadPermissionsFromForm();
                $role->savePermissions();

		if (isset($_REQUEST['new_role']) && $_REQUEST['new_role'] == 'true')
		    logAction(null, $role->role_id, "oscailt role", "create role");
                else
		    logAction(null, $role->role_id, "oscailt role", "update role");

                //could do something here about updating current user, but nah, just let them log out..
                writeRoleList();
            }
            else
            {
                $editor_session->writeNoWritePermissionError();
                writeEditBox();
            }
        }
        else if(isset($_REQUEST['subpage']) && $_REQUEST['subpage']=="edit" && !isset($_REQUEST['cancel']))
        {
            writeEditBox();
        }
        else
        {
            writeRoleList();
        }
    }
    else $editor_session->writeNoReadPermissionError();
}
else $editor_session->writeNoSessionError();
include_once("adminfooter.inc");

?>