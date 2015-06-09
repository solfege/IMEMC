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
$OSCAILT_SCRIPT = "editlanguages.php";
addToPageTitle("Manage Oscailt's Languages");


function writeLanguageList()
{
   global $languageList;
   ?>
   <table class=admin align=center>
   <tr class=admin>
      <th colspan=7 class=admin>Languages</th>

   </tr>
   <tr class=admin>
      <th class=admin>&nbsp;Name&nbsp;</th>
      <th class=admin>&nbsp;Code&nbsp;</th>
      <th class=admin>&nbsp;Active&nbsp;</th>
      <th class=admin>&nbsp;Protected&nbsp;</th>

      <th class=admin>&nbsp;Associated Stories&nbsp;</th>
      <th class=admin>&nbsp;Edit&nbsp;</th>
      <th class=admin>&nbsp;Delete&nbsp;</th>
   </tr>
   <?
   $languageList->reset();
   $languages = $languageList->getLanguages();
   for($i=0;$i<count($languages);$i++)
   {
      $language=$languages[$i];
      ?>
      <tr class=admin>
         <td class=admin>&nbsp;<?=$language->language_name?>&nbsp;</td>
         <td class=admin>&nbsp;<?=$language->language_code?>&nbsp;</td>
         <td class=admin align=center><?
         if($language->active==true) echo("<img src='graphics/active.gif'>");
         else echo("<img src='graphics/inactive.gif'>");
         ?></td>
         <td class=admin align=center><?

         if($language->excluded==true) echo("<img src='graphics/excluded.gif'>");

         else echo("<img src='graphics/included.gif'>");

         ?></td>

         <td class=admin align=center>&nbsp;<?=$language->getStoryCount()?>&nbsp;</td>
         <td class=admin align=center><a href="editlanguages.php?subpage=edit&language_id=<?=$language->language_id?>"><img src='graphics/edit.gif' border=0></a></td>
         <td class=admin align=center><a href="editlanguages.php?subpage=delete&language_id=<?=$language->language_id?>"><img src='graphics/delete.gif'  border=0></a></td>
      </tr>
      <?
   }
   ?>
   <tr>
      <form action="editlanguages.php" method=post>
      <input type=hidden name=subpage value="edit">
      <td colspan=7 align=center><input type=submit value="Create New Language"></td>

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
   $language = new Language();
   if (isset($_REQUEST['language_id']))
       $language->language_id=$_REQUEST['language_id'];

   if($language->language_id != null) $language->load();
   ?>
   <table class=admin align=center width=300>

   <form action="editlanguages.php" method=post>
   <input type=hidden name=subpage value="edit">
   <?
      if($language->language_id != null)
      {
         ?><input type=hidden name=language_id value="<?=$language->language_id?>"><?
      }
   ?>
   <tr class=admin>
      <th class=admin colspan=4>
      <?
      if($language->language_id != null) echo("Edit ".$language->language_name);
      else echo("Create New Language");
      ?>
      </th>
   </tr>
   <tr class=admin>
      <td class=admin>&nbsp;<B>Name</B>&nbsp;</td>
      <td class=admin colspan=3><input name=language_name value="<?=$language->language_name?>"></td>
   </tr>
   <tr class=admin>
      <td class=admin colspan=4>
      A language code can be be a two letter code,
      or four letter code separated by a hyphen.
      eg "en" for english or "en-US" for US english specifically.
      </td>
   </tr>
   <tr class=admin>
      <td class=admin>&nbsp;<B>Code</B>&nbsp;</td>
      <td class=admin colspan=3><input name=language_code value="<?=$language->language_code?>"></td>
   </tr>
   <tr class=admin>
      <td class=admin colspan=4>
      Deactivating a language will prevent new stories from being
      published using this language. It will still be available for
      filtering and searching existing stories.
      </td>
   </tr>
   <tr class=admin>
      <td class=admin><B>Active</B></td>
      <td  class=admin colspan=3 align=right><input type=checkbox name=active <? if($language->active==true) echo("checked"); ?>></td>

   </tr>

   <tr class=admin>
      <td class=admin colspan=4>
      Protecting a language will mean that stories published with this language will only be visible to editors with the right permissions.
      </td>
   </tr>

   <tr class=admin>
      <td class=admin><B>Protected</B></td>
      <td class=admin colspan=3 align=right><input type=checkbox name=excluded <? if($language->excluded==true) echo("checked"); ?>></td>
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
   $language = new Language();
   $language->language_id=$_REQUEST['language_id'];
   $language->load();
   ?>
   <table align=center>
   <form action="editlanguages.php" method=post>
   <input type=hidden name=subpage value="delete">
   <input type=hidden name=language_id value="<?=$language->language_id?>"><?
   ?>
   <tr>
      <td colspan=2 align=center><img src="graphics/caution.gif" align=middle><BR><BR><B>Are you sure you wish to delete <?=$language->language_name?>?</B><BR><BR></td>
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

   if($editor_session->editor->allowedReadAccessTo("editlanguages"))
   {
      if(isset($_REQUEST['subpage']) && $_REQUEST['subpage']=="delete" && isset($_REQUEST['confirm']) )
      {
         $language = new Language();
         $language->language_id=$_REQUEST['language_id'];
         $language->load();
         if($language->language_id != null)
         {
            if($language->getStoryCount()==0)
            {
               if($editor_session->editor->allowedWriteAccessTo("editlanguages"))
               {
                  logAction("", $language->language_id, "language", "delete", "(Field ".$language->getName().")"); 
                  $language->delete();
               }
               else $editor_session->writeNoWritePermissionError();
            }
            else
            {
               writeError("All stories associated with '$language->language_name' must be deleted or reassigned<BR>before '$language->language_name' can be deleted");
            }
         }
         writeLanguageList();
      }
      else if(isset($_REQUEST['subpage']) && $_REQUEST['subpage']=="delete" && isset($_REQUEST['cancel']) )
      {
         writeLanguageList();
      }
      else if(isset($_REQUEST['subpage']) && $_REQUEST['subpage']=="delete")
      {
         writeConfirmDeleteBox();
      }
      else if(isset($_REQUEST['subpage']) && $_REQUEST['subpage']=="edit" && isset($_REQUEST['save']) )
      {
         $language= new Language();

         $language->language_id=$_REQUEST['language_id'];


         if($_REQUEST["active"]=="yes" || $_REQUEST["active"]=="on"  || $_REQUEST["active"]=="true") $language->active = true;


         else $language->active = false;
         if($_REQUEST["excluded"]=="yes" || $_REQUEST["excluded"]=="on"  || $_REQUEST["excluded"]=="true") $language->excluded = true;

         else $language->excluded = false;

         $language->language_name=cleanseTitleField($_REQUEST['language_name']);


         $language->language_code=cleanseTitleField($_REQUEST['language_code']);
         if($language->language_name==null || $language->language_name=="")
         {
            writeError("Please Specify Name");
            writeEditBox();
         }
         else if($language->language_code==null || $language->language_code=="")
         {
            writeError("Please Specify Code");
            writeEditBox();
         }
         else if($language->language_id==null && $languageList->getLanguageByName($language->language_name)!=null)
         {
            writeError("A language with this name already exists!");
            writeEditBox();
         }
         else if($language->language_id==null && $languageList->getLanguageByCode($language->language_code)!=null)
         {
            writeError("A language with this code already exists!");
            writeEditBox();
         }
         else
         {
            if($editor_session->editor->allowedWriteAccessTo("editlanguages"))
            {
               if ($language->language_id == null) {
                  $t_action = "create";
               } else {
                  $t_action = "update";
               }
               $language->save();
               logAction("", $language->language_id, "language", $t_action, "(Field ".$language->getName().")");
               writeLanguageList();
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
         writeLanguageList();
      }
   }
   else $editor_session->writeNoReadPermissionError();
}
else $editor_session->writeNoSessionError();


include_once("adminfooter.inc");

?>