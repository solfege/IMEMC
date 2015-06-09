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
$OSCAILT_SCRIPT = "edittypes.php";
addToPageTitle("Manage Article Types");


function writeTypeList()
{
   global $typeList;
   ?>
   <table align=center>
   <tr class=admin>
      <th class=admin colspan=7>Types</td>

   </tr>
   <tr class=admin>
      <th class=admin>&nbsp;Db Id&nbsp;</th>
      <th class=admin>&nbsp;Name&nbsp;</th>
      <th class=admin>&nbsp;Active&nbsp;</th>
      <th class=admin>&nbsp;Protected&nbsp;</th>

      <th class=admin>&nbsp;Associated Stories&nbsp;</th>
      <th class=admin>&nbsp;Edit&nbsp;</th>
      <th class=admin>&nbsp;Delete&nbsp;</th>
   </tr>
   <?
   $typeList->reset();
   $types = $typeList->getTypes();
   for($i=0;$i<count($types);$i++)
   {
      $type=$types[$i];
      ?>
      <tr class=admin>
         <td class=admin>&nbsp;<?=$type->type_id?>&nbsp;</td>
         <td class=admin>&nbsp;<?=$type->type_name?>&nbsp;</td>
         <td class=admin align=center><?
         if($type->active==true) echo("<img src='graphics/active.gif'>");
         else echo("<img src='graphics/inactive.gif'>");
         ?></td>
         <td class=admin align=center><?

         if($type->excluded==true) echo("<img src='graphics/excluded.gif'>");

         else echo("<img src='graphics/included.gif'>");

         ?></td>

         <td class=admin align=center>&nbsp;<?=$type->getStoryCount()?>&nbsp;</td>
         <td class=admin align=center><a href="edittypes.php?subpage=edit&type_id=<?=$type->type_id?>"><img src='graphics/edit.gif' border=0></a></td>
         <td class=admin align=center><a href="edittypes.php?subpage=delete&type_id=<?=$type->type_id?>"><img src='graphics/delete.gif'  border=0></a></td>
      </tr>
      <?
   }
   ?>
   <tr>
      <form action="edittypes.php" method=post>
      <input type=hidden name=subpage value="edit">
      <td colspan=7 align=center><input type=submit value="Create New Type"></td>

      </form>
   </tr>
   </table>
   <?
}


function writeError($error)
{
   ?><BR><BR><font class=error><B><?=$error?></B></font><BR><BR><?
}


function writeEditBox()
{
   $type = new Type();

   if (isset($_REQUEST['type_id']) ) 
       $type->type_id=$_REQUEST['type_id'];

   if($type->type_id != null) $type->load();
   ?>
   <table class=admin align=center width=300>

   <form action="edittypes.php" method=post>
   <input type=hidden name=subpage value="edit">
   <?
      if($type->type_id != null)
      {
         ?><input type=hidden name=type_id value="<?=$type->type_id?>"><?
      }
   ?>
   <tr class=admin>
      <th class=admin colspan=4>
      <?
      if($type->type_id != null) echo("Edit ".$type->type_name);
      else echo("Create New Type");
      ?>
      </th>
   </tr>
   <tr class=admin>
      <td class=admin>&nbsp;<B>Name</B>&nbsp;</td>
      <td class=admin colspan=3><input name=type_name value="<?=$type->type_name?>"></td>
   </tr>
   <tr class=admin>
      <td class=admin colspan=4>
      Deactivating a type will prevent new stories from being
      published using this type. It will still be available for
      filtering and searching existing stories.
      </td>
   </tr>
   <tr class=admin>
      <td class=admin><B>Active</B></td>
      <td class=admin colspan=3 align=right><input type=checkbox name=active <? if($type->active==true) echo("checked"); ?>></td>

   </tr>

   <tr class=admin>

      <td class=admin colspan=4> 
      Protecting a type will make stories published with this type invisible to everybody who doesn't have the correct permissions.  
      </td> 
   </tr>

   <tr class=admin>

      <td class=admin><B>Protected</B></td>

      <td class=admin colspan=3 align=right><input type=checkbox name=excluded <? if($type->excluded==true) echo("checked"); ?>></td>

   </tr>
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
   $type = new Type();
   $type->type_id=$_REQUEST['type_id'];
   $type->load();
   ?>
   <table align=center>
   <form action="edittypes.php" method=post>
   <input type=hidden name=subpage value="delete">
   <input type=hidden name=type_id value="<?=$type->type_id?>"><?
   ?>
   <tr>
      <td colspan=2 align=center><img src="graphics/caution.gif" align=middle><BR><BR><B>Are you sure you wish to delete <?=$type->type_name?>?</B><BR><BR></td>
   </tr>
   <tr>
      <td align=right><input type=submit name=cancel value="&lt;&lt; Cancel"></td>
      <td><input type=submit name=confirm value="Delete &gt;&gt;"></td>
   </tr>
   </form>
   </table>
   <?
}
ob_start();
if($editor_session->isSessionOpen())
{
   writeAdminHeader();

   if($editor_session->editor->allowedReadAccessTo("edittypes"))
   {
      if(isset($_REQUEST['subpage']) && $_REQUEST['subpage']=="delete" && isset($_REQUEST['confirm']) )
      {
         $type = new Type();
         $type->type_id=$_REQUEST['type_id'];
         $type->load();
         if($type->type_id != null)
         {
            $feature_type=$typeList->getTypeByName('Feature');
            if($feature_type->type_id==$type->type_id)
            {
               writeError("The 'Feature' type is integral to the working of the site and cannot be deleted!");
            }
            else if($type->getStoryCount()==0)
            {
               if($editor_session->editor->allowedWriteAccessTo("edittypes"))
               {
                  logAction("", $type->type_id, "type", "delete","(Field ".$type->getName().")");
                  $type->delete();
               }
               else $editor_session->writeNoWritePermissionError();
            }
            else
            {
               writeError("All stories associated with '$type->type_name' must be deleted or reassigned<BR>before '$type->type_name' can be deleted");
            }
         }
         writeTypeList();
      }
      else if(isset($_REQUEST['subpage']) && $_REQUEST['subpage']=="delete" && isset($_REQUEST['cancel']) )
      {
         writeTypeList();
      }
      else if(isset($_REQUEST['subpage']) && $_REQUEST['subpage']=="delete")
      {
         writeConfirmDeleteBox();
      }
      else if(isset($_REQUEST['subpage']) && $_REQUEST['subpage']=="edit" && isset($_REQUEST['save']) )
      {
         $type= new Type();
         $type->type_id=$_REQUEST['type_id'];
         if($_REQUEST["active"]=="yes" || $_REQUEST["active"]=="on"  || $_REQUEST["active"]=="true") $type->active = true;
         else $type->active = false;
         if($_REQUEST["excluded"]=="yes" || $_REQUEST["excluded"]=="on"  || $_REQUEST["excluded"]=="true") $type->excluded = true;

         else $type->excluded = false;

         $type->type_name=cleanseTitleField($_REQUEST['type_name']);



         $feature_type=$typeList->getTypeByName('Feature');
         if($feature_type->type_id==$type->type_id)
         {
            writeError("The 'Feature' type is integral to the working of the site and cannot be modified!");
            writeEditBox();
         }
         else if($type->type_name==null || $type->type_name=="")
         {
            writeError("Please Specify Name");
            writeEditBox();
         }
         else if($type->type_id==null && $typeList->getTypeByName($type->type_name)!=null)
         {
            writeError("A type with this name already exists!");
            writeEditBox();
         }
         else
         {
            if($editor_session->editor->allowedWriteAccessTo("edittypes"))
            {
               if ($type->type_id == null) {
                   $t_action = "create";
               } else {
                   $t_action = "update";
               }
               // Save returns the type_id fromt the insert
               $type->save();
               logAction("", $type->type_id, "type", $t_action, "(Field " .$type->getName().")");
               writeTypeList();
            }
            else
            {
               $editor_session->writeNoWritePermissionError();
               writeEditBox();
            }
         }
      }
      else if(isset($_REQUEST['subpage']) && $_REQUEST['subpage']=="edit" && !isset($_REQUEST['cancel']) )
      {
         writeEditBox();
      }
      else
      {
         writeTypeList();
      }
   }
   else $editor_session->writeNoReadPermissionError();
}
else $editor_session->writeNoSessionError();


include_once("adminfooter.inc");

?>