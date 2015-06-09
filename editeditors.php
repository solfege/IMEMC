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
require_once "objects/editor.inc";
require_once "objects/indyobjects/indydataobjects.inc";
$OSCAILT_SCRIPT = "editeditors.php";
$editorList = new EditorList();

addToPageTitle("Edit The Editors of the Site");


function writeEditorList()
{
    global $editor_session, $editorList, $system_config, $OSCAILT_SCRIPT;
    if(isset($_REQUEST['editself']))
    {
        writeEditBox();
        return;
    }
    $req_page = 1;
    if(isset($_REQUEST['req_page'])) {
       $req_page = $_REQUEST['req_page'];
       if ($req_page < 1) $req_page = 1;
    }

    $editorList->reset();

    $sort_mode = "true";
    $pg_sort = "";
    if(isset($_REQUEST['sort']) && $_REQUEST['sort'] == 'true') {
       $editorList->setSort();
       $sort_mode = "false";
       $pg_sort = "&sort=true";
    }

    $editors = $editorList->getEditors($req_page);
    //$total_editors = count($editors);
    $total_editors = $editorList->getEditorGrandTotal();
    $show_pagelinks = false;

    $title_counts_msg = "Total of ". $total_editors ." editors ";
    if ($total_editors > $editorList->getPageSize() ) {
       $max_pages = ceil($total_editors/$editorList->getPageSize());

       $title_counts_msg .= " &nbsp;  - Page ". $req_page ." of " . $max_pages;
       $show_pagelinks = true;

       $next_page = $req_page+1;
       $prev_page = $req_page-1;
       // No link if on first page
       if ($prev_page < 1 ) $prev_page_link = '';
       else $prev_page_link = '<a href="'.$OSCAILT_SCRIPT . '?req_page='.$prev_page.$pg_sort.'">&lt;&lt; Prev</a> ';

       // No link if on last page
       if ($next_page > $max_pages ) $next_page_link = '';
       else $next_page_link = '<a href="'.$OSCAILT_SCRIPT . '?req_page='.$next_page.$pg_sort.'">Next &gt;&gt</a>';
    }

    if(isset($_REQUEST['sort']) && $_REQUEST['sort'] == 'true') {
       $title_counts_msg .= " &nbsp; &nbsp; <small>sorted by name</small>";
    }

    ?>
    <table align=center>
    <?
    if ($show_pagelinks == true) {
       ?>
       <tr class=admin>
          <td class=admin align="left" colspan=3><small>&nbsp; <?=$prev_page_link?>&nbsp;</small></td>
          <td class=admin align="right" colspan=3><small>&nbsp; <?=$next_page_link?>&nbsp;<small></td>
       </tr>
       <?
    }

    ?>
    <tr class=admin>
       <th class=admin colspan=6> <?=$title_counts_msg?> </th>
    </tr>
    <tr class=admin>
        <th class=admin>&nbsp;#&nbsp;</th>
        <th class=admin>&nbsp;<a class='editor-option' href="<?=$OSCAILT_SCRIPT?>?sort=<?=$sort_mode?>">Name</a>&nbsp;</th>
        <th class=admin>&nbsp;Email&nbsp;</th>
        <th class=admin>&nbsp;Last Login&nbsp;</th>
        <th class=admin>&nbsp;Edit&nbsp;</th>
        <th class=admin>&nbsp;Delete&nbsp;</th>
    </tr>
    <?

    // We should only display the amount we retrieved, not the grand total or page size because 
    // on the last page, we might only get say 10.
    for($i=0; $i < $editorList->getEditorTotal();$i++)
    {
        $i_row = $i + (($req_page -1) * $editorList->getPageSize()) + 1;

        $editor=$editors[$i];
	if ($editor->editor_lastlogin == NULL || $editor->editor_lastlogin ==0) $txt_editor_lastlogin = "";
	else $txt_editor_lastlogin = strftime($system_config->default_strftime_format, $editor->editor_lastlogin + $system_config->timezone_offset);

        ?>
        <tr class=admin>
            <td class=admin>&nbsp;<?=$i_row?>&nbsp;</td>
            <td class=admin>&nbsp;<?=$editor->editor_name?>&nbsp;</td>
            <td class=admin>&nbsp;<a href="mailto:<?=$editor->editor_email?>"><?=$editor->editor_email?></a>&nbsp;</td>
            <td class=admin align="right">&nbsp;<?=$txt_editor_lastlogin?>&nbsp;</td>
            <td class=admin align=center><a href="editeditors.php?subpage=edit&editor_id=<?=$editor->editor_id?>"><img src='graphics/edit.gif' border=0></a></td>
            <td class=admin align=center><a href="editeditors.php?subpage=delete&editor_id=<?=$editor->editor_id?>"><img src='graphics/delete.gif' border=0></a></td>
        </tr>
        <?
    }

    if ($show_pagelinks == true) {
       ?>
       <tr class=admin>
          <td class=admin align="left" colspan=3><small>&nbsp; <?=$prev_page_link?>&nbsp;</small></td>
          <td class=admin align="right" colspan=3><small>&nbsp; <?=$next_page_link?>&nbsp;<small></td>
       </tr>
       <?
    }

    ?>
    <tr>
        <form name="create_editor" action="editeditors.php" method=post>
        <input type=hidden name=subpage value="edit">
        <td colspan=6 align=center><input type=submit value="Create New Editor"></td>
        </form>
    </tr>
    </table>
    <?
}


function writeError($error)
{
    ?><BR><BR><font class=error><B><?=$error?></B></font><BR><BR><?
}

function writeLocalAdminHeader($view_role)
{
   global $OSCAILT_SCRIPT;
   if($view_role == 'true') {
       $v_lnk = "?editself=true";
       $v_txt = "Edit Self";
   } else {
       $v_lnk = "?viewrole=true&editself=true";
       $v_txt = "View Role";
   }
   $u_lnk = "editorstatus.php?viewprofile=true";
   $u_txt = "View Editor Profiles"
       
   ?>
     <TABLE class='admin'>
        <TR class='admin'><TD class='admin'><a href="<?=$OSCAILT_SCRIPT?><?=$v_lnk?>"><?=$v_txt?></a> &nbsp;<a href="<?=$u_lnk?>"><?=$u_txt?></a> </TD></TR>
     </TABLE>
   <?
}


function writeEditBox()
{
    global $roleList, $editorList, $editor_session;
    $editor = new Editor();
    if(isset($_REQUEST['editor_id']))
    {
       $editor->editor_id=cleanseNumericalQueryField($_REQUEST['editor_id']);
       $editor->load();

    }
    else $editor->editor_id=null;

    $view_ownrole = false;
    if(isset($_REQUEST['viewrole']) && $_REQUEST['viewrole'] == 'true') {
        if(isset($_REQUEST['editself']) && $_REQUEST['editself'] == 'true') {
             // And you have to be the editor otherwise the functionality does not work because what it is
             // testing is that the load permissions as actually loaded work or have the values they have.
             if($editor->editor_id != NULL) {
                 if($editor->editor_id == $editor_session->editor->editor_id ) {
                     $view_ownrole = true;
                 }
             }
        }
    }
    // And no point showing the local header URL options if we are a super editor looking at another
    // editor details. We must be that editor.
    if($editor->editor_id != NULL) {
        if($editor->editor_id == $editor_session->editor->editor_id ) {
            writeLocalAdminHeader($view_ownrole);
        }
    }

    ?>
    <form name="editbox_editors" action="editeditors.php" method=post>

    <input type=hidden name=subpage value="edit">
    <table align=center class=admin>
    <?
        if($editor->editor_id != null)
        {
            ?><input type=hidden name=editor_id value="<?=$editor->editor_id?>"><?
        }
        if(isset($_REQUEST['editself']))
        {
            ?><input type=hidden name=editself value=true><?
        }

        if($view_ownrole == true) {
           writeRoleBox();
        } else {
        
        ?>
        <tr class=admin>
            <th class=admin colspan=4>
            <?
            $required_str="";
            if($editor->editor_id != null) echo("Edit ".$editor->editor_name);
            else 
            {
                echo("Create New Editor");
                $required_str= " <small class='error'>(required)</small>";
            }
            ?>
            </th>
        </tr>
        <tr class=admin>
            <td class=admin>&nbsp;<B>Name</B><?=$required_str?>&nbsp;</td>
            <td class=admin colspan=3><input name=editor_name value="<?=$editor->editor_name?>"></td>
        </tr>
        <tr class=admin>
            <td class=admin>&nbsp;<B>Email</B><?=$required_str?>&nbsp;</td>
            <td class=admin colspan=3><input name=editor_email value="<?=$editor->editor_email?>"></td>
        </tr>
        <tr class=admin>
            <td class=admin>&nbsp;<B>Details</B></td>
            <td class=admin colspan=3><input name=editor_details size=80 maxlength=254 value="<?=$editor->editor_details?>"></td>
        </tr>
        <tr class=admin>
            <td class=admin colspan=4>&nbsp;
            <?
            if($editor->editor_id != null) echo("Leave the password fields blank if you do not wish to change them");
            else echo("Please type the password twice to ensure it is correct");
            ?>&nbsp;
            </td>
        </tr>
        <tr class=admin>
            <td class=admin>&nbsp;<B>Password</B><?=$required_str?>&nbsp;</td>
            <td class=admin colspan=3><input type=password name=editor_password></td>
        </tr>
        <tr class=admin>
            <td class=admin>&nbsp;<B>Retype Password</B><?=$required_str?>&nbsp;</td>
            <td class=admin colspan=3><input type=password name=editor_password2></td>
        </tr>
        <?
    
        if(!isset($_REQUEST['editself']))
        {
          if($editor->editor_id != null)
          {
             $editor->loadRoles(true);
             $name = $editor->editor_name;
          }
          else
          {
             $name = "New Editor";
          }
    
          writeRoleSectionFooter();
          writeRoleSectionFooter();
    
          $header_txt = "$editor->editor_name Administrative Roles";
          writeRoleSectionHeader($header_txt);
    
          $roles = $roleList->getRolesByType('admin');
    
          foreach($roles as $role)
          {
             writeRole($editor, $role, 'admin');
          }
    
          writeRoleSectionFooter();
    
          $all_sites = array();
          $all_sites = loadSiteObjects($all_sites);
    
          foreach($all_sites as $site)
          {
             $site_header_text = $name." Roles for <b>".$site->name()."</b> site (id: ".$site->objref.")";
    
             writeRoleSectionHeader($site_header_text);
             $roles = $roleList->getRolesByType('site');
             $header_txt = "<small>Site Builder Roles</small>";
             writeRoleHeader($header_txt);
    
             foreach($roles as $role)
             {
                writeRole($editor, $role, $site->id());
             }
    
             writeRoleHeader("<small>Editorial Roles</small>");
             $roles = $roleList->getRolesByType('editorial');
    
             foreach($roles as $role)
             {
                writeRole($editor, $role, $site->id());
             }
             writeRoleSectionFooter();
          }
        }
    }
    // It has to be outside the if statement
    echo "</table>";
    
    // Do not draw save and cancel if only viewing role.
    if($view_ownrole == true) return;

    ?>
    <div class='editorconfirmbuttons'>

        <input type=submit name=cancel value="&lt;&lt; Cancel">
        <input type=submit name=save value="Save &gt;&gt;">
    </div>
    </form>
    <?
}

function loadSiteObjects(&$all_sites)
{
   global $system_config, $userLanguage, $OSCAILT_SCRIPT;

   $obj_man = new indyObjectManager($system_config->xmltypedef_dir, $OSCAILT_SCRIPT);
   return $obj_man->getAllManagedSiteObjects($all_sites, $userLanguage->getLanguageCodePrefix());
}

function writeRoleSectionFooter()
{
   echo "<TR><TD colspan=4>&nbsp;</TD></TR>";
}

function writeRoleSectionHeader($header_txt)
{
   echo "<TR><TH align='center' colspan=4>$header_txt</TH></TR>";
   writeRoleColumnHeaders();
}


function writeRoleGroupHeader($header_txt)
{
   writeRoleHeader($header_txt);
   writeRoleColumnHeaders();
}

function writeRoleHeader($header_txt)
{
?>
      <tr class="admin">
         <td class="admin" align="center" colspan="4"><b><?=$header_txt?></b></td>
      </tr>
<?
}

function writeRoleColumnHeaders()
{
?>
      <tr class=admin>
         <th class=admin align=center>Role</th>
         <th class=admin align=center>Description</th>
         <th class=admin align=center>No</th>
         <th class=admin align=center>Yes</th>
      </tr>
<?
}

function writeRole(&$editor, &$role, $site_id)
{
    ?>
    <tr class=admin>
        <td class=admin align=left><strong>&nbsp;<?=$role->role_name?></strong></td>
        <td class=admin align=left><i>&nbsp;<?=$role->role_description?></i></td>
      <?
      $site_role_id = "site_".$site_id."_role_".$role->role_id;

      if($editor->editor_id != null && $editor->possessesRole($role->role_id, $site_id))
      {
         ?>
         <td class=admin align=center><input type=radio name="<?=$site_role_id?>" value="no"></td>
         <td class=admin align=center><input type=radio name="<?=$site_role_id?>" value="yes" checked></td>
         <?
      }
      else
      {
         ?>
         <td class=admin align=center><input type=radio name="<?=$site_role_id?>" value="no" checked></td>
         <td class=admin align=center><input type=radio name="<?=$site_role_id?>" value="yes" ></td>
         <?
      }
      ?>
    </tr>
    <?
}


function writeConfirmDeleteBox()
{
    $editor = new Editor();
    $editor->editor_id=cleanseNumericalQueryField($_REQUEST['editor_id']);
    $editor->load();
    ?>
    <table align=center>
    <form name="confirm_delete" action="editeditors.php" method=post>
    <input type=hidden name=subpage value="delete">
    <input type=hidden name=editor_id value="<?=$editor->editor_id?>"><?
    ?>
    <tr>
        <td colspan=2 align=center><img src="graphics/caution.gif" align=middle><BR><BR><B>Are you sure you wish to delete <?=$editor->editor_name?>?</B><BR><BR></td>
    </tr>
    <tr>
        <td align=right><input type=submit name=cancel value="&lt;&lt; Cancel"></td>
        <td><input type=submit name=confirm value="Delete &gt;&gt;"></td>
    </tr>
    </form>
    </table>
    <?
}

function writeRoleBox()
{
    global $permissionsList;
    global $editor_session;
    loadSitePermissionOptions();
    ?>
    <table align=center class=admin>
    <tr class=admin>
        <th class=admin colspan=4 align=center>Permissions for user <?=$editor_session->editor->editor_name?></th>
    </tr>
    <?

    foreach(array_keys($editor_session->editor->permissions) as $each_permission)
    {
        $site_area = $each_permission;
    ?>
    <tr class=admin>
        <th class=admin align=center rowspan=2>&nbsp;Permissions for site: <?=$site_area?>&nbsp;</th>
        <th class=admin align=center>&nbsp;No Access&nbsp;</th>
        <th class=admin align=center>&nbsp;Read Only&nbsp;</th>
        <th class=admin align=center>&nbsp;Read/Write&nbsp;</th>
    </tr>
    <?
        if ($site_area == "admin" ) {
            ?> <tr class=admin> <th class=admin align=center colspan=3>Administration Permissions</th> </tr><?
	    $permissionsListSubset = $permissionsList[$site_area];
        } else {
            ?> <tr class=admin> <th class=admin align=center colspan=3>Site Builder Permissions</th> </tr> <?
	    $permissionsListSubset = $permissionsList["site"];
            foreach($permissionsListSubset as $permListSubArray)
            {
               writePermission($editor_session->editor, $permListSubArray[0], $permListSubArray[1], $site_area);
            }

    ?>
    <tr class=admin>
        <th class=admin align=center rowspan=2>&nbsp;Permissions for site: <?=$site_area?>&nbsp;</th>
        <th class=admin align=center>&nbsp;No Access&nbsp;</th>
        <th class=admin align=center>&nbsp;Read Only&nbsp;</th>
        <th class=admin align=center>&nbsp;Read/Write&nbsp;</th>
    </tr>
    <?
            ?> <tr class=admin> <th class=admin align=center colspan=3>Editorial Permissions</th> </tr> <?
	    $permissionsListSubset = $permissionsList["editorial"];
        }
        foreach($permissionsListSubset as $permListSubArray)
        {
            // echo("Pass: " . $permListSubArray[0] ." and " . $permListSubArray[1] . "<BR>");
            writePermission($editor_session->editor, $permListSubArray[0], $permListSubArray[1], $site_area);
        }
    }
    ?>
    <tr>
    </table>
    <?
}

function writePermission($editor, $page, $display_name, $site_area)
{
    ?>
    <tr class=admin>
        <td class=admin>&nbsp;<?=$display_name?>&nbsp;</td>
        <?
            if($editor != null && $editor->allowedWriteAccessTo($page, $site_area))
            {
                ?>
                <td class=admin align=center> <img src="graphics/inactive.gif"> </td>
                <td class=admin align=center> <img src="graphics/inactive.gif"> </td>
                <td class=admin align=center> <img src="graphics/active.gif"> </td>
                <?
            }
            elseif($editor != null && $editor->allowedReadAccessTo($page, $site_area) )
            {
                ?>
                <td class=admin align=center> <img src="graphics/inactive.gif"> </td>
                <td class=admin align=center> <img src="graphics/active.gif"> </td>
                <td class=admin align=center> <img src="graphics/inactive.gif"> </td>
                <?
            }
            else
            {
                ?>
                <td class=admin align=center> <img src="graphics/active.gif"> </td>
                <td class=admin align=center> <img src="graphics/inactive.gif"> </td>
                <td class=admin align=center> <img src="graphics/inactive.gif"> </td>
                <?
            }
        ?>
    </tr>
    <?
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
         echo $msg_arr[1]." End of Error Msg<br />";
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


ob_start();


if($editor_session->isSessionOpen())
{
    if($editor_session->editor->allowedWriteAccessTo("editroles")) 
        writeAdminHeader($OSCAILT_SCRIPT."?editself=true","EditSelf", array("editroles.php" =>"EditRoles") );
    else
        writeAdminHeader($OSCAILT_SCRIPT."?editself=true","EditSelf");


    if((!isset($_REQUEST['editself']) && $editor_session->editor->allowedReadAccessTo("editeditors")) || (isset($_REQUEST['editself']) && $editor_session->editor->allowedReadAccessTo("editself")))
    {
        if(isset($_REQUEST['editself'])) $_REQUEST['editor_id']=$editor_session->editor->editor_id;
        if(isset($_REQUEST['subpage']) && $_REQUEST['subpage']=="delete" && isset($_REQUEST['confirm']))
        {
            $editor = new Editor();
            $editor->editor_id=cleanseNumericalQueryField($_REQUEST['editor_id']);
            if($editor->editor_id != null)
            {
                if($editor_session->editor->allowedWriteAccessTo("editeditors"))
                {
                    if($editor->editor_id==$editor_session->editor->editor_id)
                    {
                        writeError("You cannot delete yourself!");
                    }
                    else
                    {
                        $editor->load();
                        $editor->deleteRoles();
                        $editor->delete();
                        logAction(null, $editor->editor_name, "oscailt user", "delete editor");
                    }
                }
                else $editor_session->writeNoWritePermissionError();
            }
            else $editor_session->writeNoWritePermissionError();
            writeEditorList();
        }
        else if(isset($_REQUEST['subpage'])&&$_REQUEST['subpage']=="delete" && isset($_REQUEST['cancel']))
        {
            writeEditorList();
        }
        else if(isset($_REQUEST['subpage']) && $_REQUEST['subpage']=="delete")
        {
            writeConfirmDeleteBox();
        }
        else if(isset($_REQUEST['subpage']) && $_REQUEST['subpage']=="edit" && isset($_REQUEST['save']))
        {
            $editor= new Editor();
            $editor->editor_id=cleanseNumericalQueryField($_REQUEST['editor_id']);
            $editor->editor_name=trim(cleanseTitleField($_REQUEST['editor_name']));
            $editor->editor_email=trim($_REQUEST['editor_email']);
	    // This is an optional field.
            $editor->editor_details=trim($_REQUEST['editor_details']);

            if($editor->editor_name==null || $editor->editor_name=="")
            {
                writeError("Please Specify Name");
                writeEditBox();
            }

            else if($editor->editor_id==null && $editorList->getEditorByName($editor->editor_name)!=null)
            {
                writeError("An editor with this name already exists!");
                writeEditBox();
            }
            else if($editor->editor_email==null || $editor->editor_email=="" || !isValidEmailAddress($editor->editor_email))
            {
                writeError("Please Specify a Valid Email Address");
                writeEditBox();
            }
            else if($_REQUEST['editor_password']==null && $_REQUEST['editor_password2']==null)
            {
                if($editor->editor_id==null)
                {
                    writeError("Please Specify Password");
                    writeEditBox();
                }
                else
                {
                    if(!isset($_REQUEST['editself']) && $editor_session->editor->allowedWriteAccessTo("editeditors"))
                    {
                        // You are here if you an editor is updating another editors profile. Log it.
                        $editor->save(null);
                        $editor->deleteRoles();
                        $all_sites = array();

                        $all_sites = loadSiteObjects($all_sites);
                        $editor->loadRolesFromForm('admin', 'admin');

                        foreach($all_sites as $site)
                        {
                           $editor->loadRolesFromForm('site', $site->id());
                           $editor->loadRolesFromForm('editorial', $site->id());
                        }
                        $editor->saveRoles();
                        if($editor->editor_id==$editor_session->editor->editor_id)
                        {
                           $editor->loadPermissions();

                           $_SESSION['current_editor'] = $editor;
                           $editor_session->editor=&$_SESSION['current_editor'];

                           //print_r($editor_session->editor);
                        }

                        logAction(null, $editor->editor_name, "oscailt user", "update editor");
                        writeError("Successfully Saved!");
                        writeEditorList();
                    }
                    else if(isset($_REQUEST['editself']) && $editor_session->editor->allowedWriteAccessTo("editself"))
                    {
                        // You are here if you are editing your own profile. No need to log these.
                        $editor->save(null);
                        $_SESSION['current_editor'] = $editor;

                        $editor_session->editor=&$_SESSION['current_editor'];

                        writeError("Successfully Saved!");
                        writeEditBox();
                    }
                    else
                    {
                        $editor_session->writeNoWritePermissionError();
                        writeEditBox();
                    }
                }
            }
            else if($_REQUEST['editor_password']==$_REQUEST['editor_password2'])
            {
                if(strlen(trim($_REQUEST['editor_password']))<5)
                {
                    writeError("Your password must contain at least 5 characters");
                    writeEditBox();
                }
                else
                {
                    if(!isset($_REQUEST['editself']) && $editor_session->editor->allowedWriteAccessTo("editeditors"))
                    {
                        // Save the editor info to the database.
                        // Figure out if this is a create or pw update by another editor, then log it below.
			$create_action = false;
                        if ($editor->editor_id == null ) $create_action = true;

                        $editor->save(trim(cleanseTitleField($_REQUEST['editor_password'])));

                        $editor->deleteRoles();
                        $all_sites = array();
                        $all_sites = loadSiteObjects($all_sites);
                        $editor->loadRolesFromForm('admin', 'admin');

                        foreach($all_sites as $site)
                        {
                            $editor->loadRolesFromForm('site', $site->id());
                            $editor->loadRolesFromForm('editorial', $site->id());
                        }

                        $editor->saveRoles();
                        if($editor->editor_id==$editor_session->editor->editor_id)
                        {
                            $_SESSION['current_editor'] = $editor;
                            $editor_session->editor=&$_SESSION['current_editor'];
                        }

                        // Log the action. Note during create, the save assigns the editor_id.
			if ($editor->editor_id != $editor_session->editor->editor_id) {
                            if ($create_action == true )
                                logAction(null, $editor->editor_name, "oscailt user", "create editor");
			    else
                                logAction(null, $editor->editor_name, "oscailt user", "update pw editor");
                        }

                        writeError("Successfully Saved!");
                        writeEditorList();
                    }
                    else if(isset($_REQUEST['editself']) && $editor_session->editor->allowedWriteAccessTo("editself"))
                    {
                        $editor->save(trim(cleanseTitleField($_REQUEST['editor_password'])));
                        if($editor->editor_id==$editor_session->editor->editor_id)
                        {
                            $editor->permissions = $editor_session->editor->permissions;
                            $editor->roles = $editor_session->editor->roles;
                            $_SESSION['current_editor'] = $editor;
                            $editor_session->editor=&$_SESSION['current_editor'];
                        }
                        writeError("Successfully Saved!");
                        writeEditBox();
                    }
                    else
                    {
                        $editor_session->writeNoWritePermissionError();
                        writeEditBox();
                    }
                }
            }
            else
            {
                writeError("Passwords Do Not Match!");
                writeEditBox();
            }
        }
        else if(isset($_REQUEST['subpage']) && $_REQUEST['subpage']=="edit" && !isset($_REQUEST['cancel']))
        {
            writeEditBox();
        }
        else
        {
            writeEditorList();
        }
    }
    else $editor_session->writeNoReadPermissionError();
}
else $editor_session->writeNoSessionError();

include_once("adminfooter.inc");
?>