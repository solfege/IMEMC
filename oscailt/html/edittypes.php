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

$OSCAILT_SCRIPT = "edittypes.php";
$textLabels = array("title" => "Manage Article Types",
	            "types" => "Types",
	            "name" => "Name",
	            "language" => "Language",
	            "active" => "Active",
	            "protected" => "Protected",
	            "associated_stories" => "Associated Stories",
	            "edit" => "Edit",
	            "delete" => "Delete",
	            "CreateNewType" => "Create New Type",
	            "name_description" => "Deactivating a type will prevent new stories from being published using this type. It will still be available for filtering and searching existing stories.",
	            "active_description" => "Protecting a type will make stories published with this type invisible to everybody who doesn't have the correct permissions.",
	            "cancelBtn" => "Cancel",
	            "saveBtn" => "Save",
	            "are_you_sure" => "Are you sure you wish to delete",
	            "feature_delete_error" => "The 'Feature' type is integral to the working of the site and cannot be deleted!",
	            "feature_modify_error" => "The 'Feature' type is integral to the working of the site and cannot be modified!",
	            "delete_msg_1" => "All stories associated with ",
	            "delete_msg_2" => " must be deleted or reassigned<BR>before ",
	            "delete_msg_3" => " can be deleted",
	            "specify_name_error" => "Please Specify Name",
	            "type_exists_error" => "A type with this name already exists!");

$textObj = new indyItemSet();

if($textObj->load($system_config->xml_store, "edittypes") === false)
{
    $textObj->setUserMessage( array( USER_ERROR, "Failed to get text strings for the Edit Types. -Using defaults",""));
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


function writeTypeList()
{
   global $typeList, $textLabels;
   global $languageList, $userLanguage;

   if(isset($_REQUEST['language_id']) && $_REQUEST['language_id'] != "" ) $language_id = $_REQUEST['language_id'];
   else $language_id = $userLanguage->language_id;

   $lang_select = '<select name="language_id" onchange=submit()>'.$languageList->getLanguagesSelect($language_id).'</select>';

   ?>
   <table align=center>
   <tr class=admin>
   <th class=admin colspan=8> <?=$textLabels['types']?></td>

   </tr>
   <tr class=admin>
      <FORM name="select_form" action="edittypes.php" method=post>
      <th class=admin>&nbsp;Db Id&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['name']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['language']?><br><?=$lang_select?></th>
      <th class=admin>&nbsp;<?=$textLabels['active']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['protected']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['associated_stories']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['edit']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['delete']?>&nbsp;</th>
      </FORM>
   </tr>
   <?
   $typeList->reset();
   $types = $typeList->getTypes($language_id);
   for($i=0;$i<count($types);$i++)
   {
      $type=$types[$i];
      $language = $languageList->getLanguageByID($type->language_id);
      ?>
      <tr class=admin>
         <td class=admin>&nbsp;<?=$type->type_id?>&nbsp;</td>
         <td class=admin>&nbsp;<?=$type->type_name?>&nbsp;</td>
         <td class=admin>&nbsp;<?=$language->getLanguageCodePrefix()?>&nbsp;</td>
         <td class=admin align=center><?
         if($type->active==true) echo("<img src='graphics/active.gif'>");
         else echo("<img src='graphics/inactive.gif'>");
         ?></td>
         <td class=admin align=center><?

         if($type->excluded==true) echo("<img src='graphics/excluded.gif'>");

         else echo("<img src='graphics/included.gif'>");

         ?></td>

         <td class=admin align=center>&nbsp;<?=$type->getStoryCount()?>&nbsp;</td>
         <td class=admin align=center><a href="edittypes.php?subpage=edit&type_id=<?=$type->type_id?>&language_id=<?=$type->language_id?>"><img src='graphics/edit.gif' border=0></a></td>
         <td class=admin align=center><a href="edittypes.php?subpage=delete&type_id=<?=$type->type_id?>&language_id=<?=$type->language_id?>"><img src='graphics/delete.gif'  border=0></a></td>
      </tr>
      <?
   }
   ?>
   <tr>
      <form action="edittypes.php" method=post>
      <input type=hidden name=subpage value="edit">
      <td colspan=7 align=center><input type=submit value="<?=$textLabels['CreateNewType']?>"></td>

      </form>
   </tr>
   </table>
   <?
}


function writeError($error)
{
   ?><p><font class=error><B><?=$error?></B></font></p><?
}


function writeEditBox($reload=false)
{
   global $languageList, $textLabels, $typeList;

   if ($reload == true ) {
       $typeList->reset();
   }

   $type = new Type();
   if (isset($_REQUEST['language_id']))
   { 
       $type->language_id = $_REQUEST['language_id'];
   } else {
       $type->language_id = $languageList->getMinLanguageId();
   }

   if (isset($_REQUEST['type_id']) ) 
       $type->type_id=$_REQUEST['type_id'];

   if($type->type_id != null) $type->load();

   $language = $languageList->getLanguageByID($type->language_id);
   
   $type_languages = $typeList->getAllTypeLanguagesByID($type->type_id);

   ?>
   <table class=admin align=center width=300>

   <form name="create_edit_form" action="edittypes.php" method=post>
   <input type=hidden name=subpage value="edit">
   <?
      if($type->type_id != null)
      {
         ?>
           <input type=hidden name=type_id value="<?=$type->type_id?>">
           <input type=hidden name=language_id value="<?=$type->language_id?>">
         <?
      }
   ?>
   <tr class=admin>
      <th class=admin colspan=4>
      <?
      if($type->type_id != null) echo($textLabels['edit'] ." ".$type->type_name);
      else echo($textLabels['CreateNewType']);
      ?>
      </th>
   </tr>
   <?
   $type_name_langs = array();
   foreach ( $languageList->getLanguages() as $each_lang) { 
       $lang_code = $each_lang->getLanguageCodePrefix();
       $type_name_langs[$lang_code] = "";
       foreach ( $type_languages as $active_lang) { 
           if ($each_lang->language_id == $active_lang->language_id) 
	       $type_name_langs[$lang_code] = $active_lang->type_name;
       }
   }
   // <select name="language_id">   $languageList->getLanguagesSelect(1)  </select> 
   foreach ( $languageList->getLanguages() as $each_lang) { 
     $lang_code = $each_lang->getLanguageCodePrefix();
   ?>
   <tr class=admin>
      <td class=admin>&nbsp;<B><?=$textLabels['name']?></B> (<?=$lang_code;?>)&nbsp;</td>
      <td class=admin colspan=3><input type="text" name="type_name_<?=$lang_code?>" value="<?=$type_name_langs[$lang_code]?>" size="25"></td>
   </tr>
   <?
   }

   ?>
   <tr class=admin>
      <td class=admin colspan=4> <?=$textLabels['name_description']?> </td>
   </tr>
   <tr class=admin>
      <td class=admin><B><?=$textLabels['active']?></B></td>
      <td class=admin colspan=3 align=right><input type=checkbox name=active <? if($type->active==true) echo("checked"); ?>></td>
   </tr>

   <tr class=admin>
      <td class=admin colspan=4> <?=$textLabels['active_description']?>
      </td> 
   </tr>

   <tr class=admin> 
      <td class=admin><B><?=$textLabels['protected']?></B></td> 
      <td class=admin colspan=3 align=right><input type=checkbox name=excluded <? if($type->excluded==true) echo("checked"); ?>></td>

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
   $type = new Type();
   $type->type_id=$_REQUEST['type_id'];
   $type->language_id=$_REQUEST['language_id'];
   $type->load();
   ?>
   <table align=center>
   <form action="edittypes.php" method=post>
   <input type=hidden name=subpage value="delete">
   <input type=hidden name=type_id value="<?=$type->type_id?>">
   <input type=hidden name=language_id value="<?=$type->language_id?>">

   <tr>
      <td colspan=2 align=center><img src="graphics/caution.gif" align=middle><BR><BR><B> <?=$textLabels['are_you_sure']?> <?=$type->type_name?>?</B><BR><BR></td>
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

   if($editor_session->editor->allowedReadAccessTo("edittypes"))
   {
      if(isset($_REQUEST['subpage']) && $_REQUEST['subpage']=="delete" && isset($_REQUEST['confirm']) )
      {
         $type = new Type();
         $type->type_id=$_REQUEST['type_id'];
         $type->language_id=$_REQUEST['language_id'];
         $type->load();
         if($type->type_id != null)
         {
            $feature_type=$typeList->getTypeByName('Feature', $type->language_id);
            if($feature_type->type_id==$type->type_id)
            {
               // The 'Feature' type is integral to the working of the site and cannot be deleted!
               writeError($textLabels['feature_delete_error']);
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
               writeError($textLabels['delete_msg_1'].$type->type_name .$textLabels['delete_msg_2'].$type->type_name. $textLabels['delete_msg_3']);
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
         if(isset($_REQUEST["type_id"])) $type->type_id=$_REQUEST['type_id'];
	 else $type->type_id=null;

         if(isset($_REQUEST["active"]) && ($_REQUEST["active"]=="yes" || $_REQUEST["active"]=="on" || $_REQUEST["active"]=="true")) $type->active = 1;
         else $type->active = 0;
         if(isset($_REQUEST["excluded"]) && ($_REQUEST["excluded"]=="yes" || $_REQUEST["excluded"]=="on"  || $_REQUEST["excluded"]=="true")) $type->excluded = 1;

	 else $type->excluded = 0;

	 $t_show_list= false;
	 foreach ( $languageList->getLanguages() as $each_lang) 
	 {
            $lang_code = $each_lang->getLanguageCodePrefix();

            $t_name = "type_name_".$lang_code;
            $type->type_name = cleanseFormFieldFiller($_REQUEST[$t_name]);
            $type->language_id = $each_lang->language_id;

            $feature_type=$typeList->getTypeByName('Feature');
	    // Only apply this to the first language so get the min language id.
            if($feature_type->type_id==$type->type_id && $type->language_id == $languageList->getMinLanguageId() ) 
            {
               // ("The 'Feature' type is integral to the working of the site and cannot be modified!");
               writeError($textLabels['feature_modify_error']);
               //writeEditBox();
               $t_show_list= true;
            }
            else if($type->type_name==null || $type->type_name=="")
            {
               writeError($textLabels['specify_name_error']." for ".$lang_code);
       	       // If first is created have to set id so it is displayed in edit box
               if ($t_show_list == true) $_REQUEST['type_id'] = $type->type_id; 
               writeEditBox(true);
               $t_show_list= false;
               break;
            }
            else if($type->type_id==null && $typeList->getTypeByName($type->type_name)!=null)
            {
               writeError($textLabels['type_exists_error']);
               writeEditBox(true);
               $t_show_list= false;
               break;
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
                  $t_show_list= true;
                  logAction("", $type->type_id, "type", $t_action, "(Field " .$type->getName().")");
               }
               else
               {
                  $editor_session->writeNoWritePermissionError();
                  writeEditBox();
                  $t_show_list= false;
                  break;
               }
            }
         }
         if ($t_show_list== true) writeTypeList();
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
