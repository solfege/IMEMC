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
$OSCAILT_SCRIPT = "editregions.php";
addToPageTitle("Manage Regions");

function writeRegionList()
{
   global $regionList, $OSCAILT_SCRIPT;

   if(isset($_REQUEST['sort']) && $_REQUEST['sort']=="true" ) $sort_mode = "false";
   else $sort_mode = "true";

   ?>
   <table align=center>
   <tr class=admin>
      <th class=admin colspan=7 align=center>Regions</td>

   </tr>
   <tr class=admin>
      <th class=admin>&nbsp;#&nbsp;</th>
      <th class=admin>&nbsp;<a class='editor-option' href="<?=$OSCAILT_SCRIPT?>?sort=<?=$sort_mode?>">Name</a>&nbsp;</th>
      <th class=admin>&nbsp;Active&nbsp;</th>
      <th class=admin>&nbsp;Protected&nbsp;</th>

      <th class=admin>&nbsp;Associated Stories&nbsp;</th>
      <th class=admin>&nbsp;Edit&nbsp;</th>
      <th class=admin>&nbsp;Delete&nbsp;</th>
   </tr>
   <?

   $regionList->reset();
   $regions = $regionList->getRegions();

   for($i=0;$i<count($regions);$i++)
   {
      if(isset($_REQUEST['sort']) && $_REQUEST['sort']=="true" )
      {
         $region=$regions[$i];
         $regionsArray[] = $region->region_name;
      }
      $array_order[] = $i;
   }
   // Sort by name
   if (isset($_REQUEST['sort']) && $_REQUEST['sort']=="true" )
   {
      array_multisort($regionsArray, $array_order);
   }

   for($i=0;$i<count($regions);$i++)
   {
      $sorted_index=$array_order[$i];
      $region=$regions[$sorted_index];
      ?>
      <tr class=admin>
         <td class=admin>&nbsp;<?=($i+1)?>&nbsp;</td>
         <td class=admin>&nbsp;<?=$region->region_name?>&nbsp;</td>
         <td class=admin align=center><?
         if($region->active==true) echo("<img src='graphics/active.gif'>");
         else echo("<img src='graphics/inactive.gif'>");


         ?></td>
         <td class=admin align=center><?

         if($region->excluded==true) echo("<img src='graphics/excluded.gif'>");

         else echo("<img src='graphics/included.gif'>");

         ?></td>

         <td class=admin align=center>&nbsp;<?=$region->getStoryCount()?>&nbsp;</td>

         <td class=admin align=center><a href="editregions.php?subpage=edit&region_id=<?=$region->region_id?>"><img src='graphics/edit.gif' border=0></a></td>

         <td class=admin align=center><a href="editregions.php?subpage=delete&region_id=<?=$region->region_id?>"><img src='graphics/delete.gif'  border=0></a></td>


      </tr>
      <?
   }
   ?>
   <tr>
      <form action="editregions.php" method=post>
      <input type=hidden name=subpage value="edit">
      <td colspan=7 align=center><input type=submit value="Create New Region"></td>

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
   $region = new Region();
   if (isset($_REQUEST['region_id']) )
       $region->region_id=$_REQUEST['region_id'];

   if($region->region_id != null) $region->load();
   ?>
   <table class=admin align=center width=300>

   <form action="editregions.php" method=post>
   <input type=hidden name=subpage value="edit">
   <?
      if($region->region_id != null)
      {
         ?><input type=hidden name=region_id value="<?=$region->region_id?>"><?
      }
   ?>
   <tr class=admin>
      <th class=admin colspan=4>
      <?
      if($region->region_id != null) echo("Edit ".$region->region_name);
      else echo("Create New Region");
      ?>
      </th>
   </tr>
   <tr class=admin>
      <td class=admin>&nbsp;<B>Name</B>&nbsp;</td>
      <td class=admin colspan=3><input name=region_name value="<?=$region->region_name?>"></td>
   </tr>
   <tr class=admin>
      <td class=admin colspan=4>
      Deactivating a region will prevent new stories from being 
      published using this region except by editors. It will still be available for 
      filtering and searching existing stories.  
      </td>
   </tr>
   <tr class=admin>
      <td class=admin><B>Active</B></td>
      <td class=admin colspan=3 align=right><input type=checkbox name=active <? if($region->active==true) echo("checked"); ?>></td>

   </tr>

   <tr class=admin> 
      <td class=admin colspan=4> 
      Making a region protected will make all stories published with this region become invisible to anybody who doesn't have the right permissions 
      </td> 
   </tr>

   <tr class=admin> 
      <td class=admin><B>Protected</B></td>

      <td class=admin colspan=3 align=right><input type=checkbox name=excluded <? if($region->excluded==true) echo("checked"); ?>></td>

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
   $region = new Region();
   $region->region_id=$_REQUEST['region_id'];
   $region->load();
   ?>
   <table align=center>
   <form action="editregions.php" method=post>
   <input type=hidden name=subpage value="delete">
   <input type=hidden name=region_id value="<?=$region->region_id?>"><?
   ?>
   <tr>
      <td colspan=2 align=center><img src="graphics/caution.gif" align=middle><BR><BR><B>Are you sure you wish to delete <?=$region->region_name?>?</B><BR><BR></td>
   </tr>
   <tr>
      <td align=right><input type=submit name=cancel value="&lt;&lt; Cancel"></td>
      <td ><input type=submit name=confirm value="Delete &gt;&gt;"></td>
   </tr>
   </form>
   </table>
   <?
}


ob_start();



if($editor_session->isSessionOpen())
{
   writeAdminHeader();
   if($editor_session->editor->allowedReadAccessTo("editregions"))
   {
      if(isset($_REQUEST['subpage']) && $_REQUEST['subpage']=="delete" && isset($_REQUEST['confirm']) )
      {
         $region = new Region();
         $region->region_id=$_REQUEST['region_id'];
         $region->load();
         if($region->region_id != null)
         {
            if($region->getStoryCount()==0)
            {
               if($editor_session->editor->allowedWriteAccessTo("editregions"))
               {
                  logAction("", $region->region_id, "region", "delete", "(Field " .$region->getName().")");
                  $region->delete();
               }
               else $editor_session->writeNoWritePermissionError();
            }
            else
            {
               writeError("All stories associated with '$region->region_name' must be deleted or reassigned<BR>before '$region->region_name' can be deleted");
            }
         }
         writeRegionList();
      }
      else if(isset($_REQUEST['subpage']) && $_REQUEST['subpage']=="delete" && isset($_REQUEST['cancel']) )
      {
         writeRegionList();
      }
      else if(isset($_REQUEST['subpage']) && $_REQUEST['subpage']=="delete")
      {
         writeConfirmDeleteBox();
      }
      else if(isset($_REQUEST['subpage']) && $_REQUEST['subpage']=="edit" && isset($_REQUEST['save']) )
      {
         $region= new Region();

         if(isset($_REQUEST["region_id"]) && $_REQUEST["region"] !="") $region->region_id=$_REQUEST['region_id'];
	 else $region->region_id=null;

         if($_REQUEST["active"]=="yes" || $_REQUEST["active"]=="on"  || $_REQUEST["active"]=="true") $region->active = true;
         else $region->active = false;
         if(isset($_REQUEST["excluded"]) && ($_REQUEST["excluded"]=="yes" || $_REQUEST["excluded"]=="on"  || $_REQUEST["excluded"]=="true")) $region->excluded = true;

         else $region->excluded = false;

         $region->region_name=cleanseTitleField($_REQUEST['region_name']);
         if($region->region_name==null || $region->region_name=="")
         {
            writeError("Please Specify Name");
            writeEditBox();
         }
         else if($region->region_id==null && $regionList->getRegionByName($region->region_name)!=null)
         {
            writeError("A region with this name already exists!");
            writeEditBox();
         }
         else
         {
            if($editor_session->editor->allowedWriteAccessTo("editregions"))
            {
               if ($region->region_id == null) {
                  $t_action = "create";
               } else {
                  $t_action = "update";
               }
               $region->save();
               logAction("", $region->region_id, "region", $t_action, "(Field " .$region->getName().")");
               writeRegionList();
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
         writeRegionList();
      }
   }
   else $editor_session->writeNoReadPermissionError();
}
else $editor_session->writeNoSessionError();


include_once("adminfooter.inc");

?>