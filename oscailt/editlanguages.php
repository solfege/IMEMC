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
require_once("objects/indyobjects/indyobject.inc");
require_once("objects/indyobjects/indyitemset.inc");
$OSCAILT_SCRIPT = "editlanguages.php";
$textLabels = array("title" => "Manage Oscailt's Languages",
	            "languages" => "Languages",
	            "name" => "Name",
	            "code" => "Code",
	            "active" => "Active",
	            "protected" => "Protected",
	            "associated_stories" => "Associated Stories",
	            "edit" => "Edit",
	            "delete" => "Delete",
	            "CreateNewBtn" => "Create New Language",
	            "name_description" => "A language code can be be a two letter code, or four letter code separated by a hyphen. eg \"en\" for english or \"en-US\" for US english specifically.",
	            "code_description" => "Deactivating a language will prevent new stories from being published using this language. It will still be available for filtering and searching existing stories.",
	            "active_description" => "Protecting a language will mean that stories published with this language will only be visible to editors with the right permissions.",
	            "cancelBtn" => "Cancel",
	            "saveBtn" => "Save",
	            "are_you_sure_msg" => "Are you sure you wish to delete ",
                    "delete_msg_1" => "All stories associated with ",
	            "delete_msg_2" => " must be deleted or reassigned<BR>before ",
	            "delete_msg_3" => " can be deleted",
	            "specify_name_error" => "Please Specify Name",
	            "specify_code_error" => "Please Specify Code",
	            "language_name_error" => "A language with this name already exists!",
	            "language_code_error" => "A language with this code already exists!");


$textObj = new indyItemSet();

if($textObj->load($system_config->xml_store, "editlanguages") === false)
{
    $textObj->setUserMessage( array( USER_ERROR, "Failed to get text strings for the Edit Languages. -Using defaults",""));
    $textObj->writeUserMessageBox();
} else {
    foreach (array_keys($textLabels) as $str_key ) {
	if (trim($textObj->getString($str_key)) != "" )
            $textLabels[$str_key] = $textObj->getString($str_key);
	else
        {
            if ($system_config->user_error_reporting == 8) $textLabels[$str_key] .= " using default ";
	} 
    }
}

addToPageTitle($textLabels["title"]);


function writeLanguageList()
{
   global $languageList, $textLabels;
   ?>
   <table class=admin align=center>
   <tr class=admin>
   <th colspan=7 class=admin><?=$textLabels['languages']?></th>

   </tr>
   <tr class=admin>
      <th class=admin>&nbsp;<?=$textLabels['name']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['code']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['active']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['protected']?>&nbsp;</th>

      <th class=admin>&nbsp;<?=$textLabels['associated_stories']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['edit']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['delete']?>&nbsp;</th>
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
      <td colspan=7 align=center><input type=submit value="<?=$textLabels['CreateNewBtn']?>"></td>

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
   global $textLabels;

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
      if($language->language_id != null) echo($textLabels['edit']." ".$language->language_name);
      else echo($textLabels['CreateNewBtn']);
      ?>
      </th>
   </tr>
   <tr class=admin>
   <td class=admin>&nbsp;<B><?=$textLabels['name']?></B>&nbsp;</td>
      <td class=admin colspan=3><input name=language_name value="<?=$language->language_name?>"></td>
   </tr>
   <tr class=admin>
      <td class=admin colspan=4> <?=$textLabels['name_description']?>
      </td>
   </tr>
   <tr class=admin>
      <td class=admin>&nbsp;<B><?=$textLabels['code']?></B>&nbsp;</td>
      <td class=admin colspan=3><input name=language_code value="<?=$language->language_code?>"></td>
   </tr>
   <tr class=admin>
      <td class=admin colspan=4> <?=$textLabels['active_description']?>
      </td>
   </tr>
   <tr class=admin>
      <td class=admin><B><?=$textLabels['active']?></B></td>
      <td  class=admin colspan=3 align=right><input type=checkbox name=active <? if($language->active==true) echo("checked"); ?>></td>

   </tr>

   <tr class=admin>
      <td class=admin colspan=4> <?=$textLabels['active_description']?>
      </td>
   </tr>

   <tr class=admin>
      <td class=admin><B><?=$textLabels['protected']?></B></td>
      <td class=admin colspan=3 align=right><input type=checkbox name=excluded <? if($language->excluded==true) echo("checked"); ?>></td>
   </tr>
   <tr>
      <td colspan=4 align=center>
      <input type=submit name=cancel value="&lt;&lt; <?=$textLabels['cancelBtn']?>">
      <input type=submit name=save value="<?=$textLabels['saveBtn']?> &gt;&gt;">
      </td>
   </tr>
   </form>
   </table>
   <?
}


function writeConfirmDeleteBox()
{
   global $textLabels;

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
      <td colspan=2 align=center><img src="graphics/caution.gif" align=middle><BR><BR><B> <?=$textLabels['are_you_sure_msg']?> <?=$language->language_name?>?</B><BR><BR></td>
   </tr>
   <tr>
      <td align=right><input type=submit name=cancel value="&lt;&lt; <?=$textLabels['cancelBtn']?>"></td>
      <td><input type=submit name=confirm value="<?=$textLabels['delete']?> &gt;&gt;"></td>
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
               // writeError("All stories associated with '$language->language_name' must be deleted or reassigned<BR>before '$language->language_name' can be deleted");
               writeError($textLabels['delete_msg_1'].$language->language_name. $textLabels['delete_msg_2']. $language->language_name. $textLabels['delete_msg_3']);
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

         if (isset($_REQUEST['language_id']) ) $language->language_id=$_REQUEST['language_id'];


         if(isset($_REQUEST["active"]) && ($_REQUEST["active"]=="yes" || $_REQUEST["active"]=="on"  || $_REQUEST["active"]=="true")) $language->active = 1;


         else $language->active = 0;
         if(isset($_REQUEST["excluded"]) && ($_REQUEST["excluded"]=="yes" || $_REQUEST["excluded"]=="on"  || $_REQUEST["excluded"]=="true")) $language->excluded = 1;

         else $language->excluded = 0;

         $language->language_name=cleanseTitleField($_REQUEST['language_name']);


         $language->language_code=cleanseTitleField($_REQUEST['language_code']);
         if($language->language_name==null || $language->language_name=="")
         {
            writeError($textLabels['specify_name_error']);
            writeEditBox();
         }
         else if($language->language_code==null || $language->language_code=="")
         {
            writeError($textLabels['specify_code_error']);
            writeEditBox();
         }
         else if($language->language_id==null && $languageList->getLanguageByName($language->language_name)!=null)
         {
            writeError($textLabels['language_name_error']);
            writeEditBox();
         }
         else if($language->language_id==null && $languageList->getLanguageByCode($language->language_code)!=null)
         {
            writeError($textLabels['language_code_error']);
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
