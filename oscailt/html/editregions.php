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
$OSCAILT_SCRIPT = "editregions.php";

$textLabels = array("title" => "Manage Regions",
	            "regions" => "Regions",
	            "name" => "Name",
	            "language" => "Language",
	            "active" => "Active",
	            "protected" => "Protected",
	            "associated_stories" => "Associated Stories",
	            "edit" => "Edit",
	            "delete" => "Delete",
	            "CreateNewBtn" => "Create New Region",
		    "name_description" => "Deactivating a region will prevent new stories from being published using this region except by editors. It will still be available for filtering and searching existing stories.",
	            "active_description" => "Making a region protected will make all stories published with this region become invisible to anybody who doesn't have the right permissions",
	            "are_you_sure" => "Are you sure you wish to delete",
	            "delete_msg_1" => "All stories associated with ",
	            "delete_msg_2" => " must be deleted or reassigned<BR>before ",
	            "delete_msg_3" => " can be deleted",
	            "specify_name_error" => "Please Specify Name",
	            "region_error" => "A region with this name already exists!",
	            "cancelBtn" => "Cancel",
	            "saveBtn" => "Save");

$textObj = new indyItemSet();

if($textObj->load($system_config->xml_store, "editregions") === false)
{
    $textObj->setUserMessage( array( USER_ERROR, "Failed to get text strings for the Edit Regions. -Using defaults",""));
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

function writeRegionList()
{
   global $regionList, $OSCAILT_SCRIPT, $textLabels;
   global $languageList, $userLanguage;

   if(isset($_REQUEST['sort']) && $_REQUEST['sort']=="true" ) $sort_mode = "false";
   else $sort_mode = "true";

   if(isset($_REQUEST['language_id']) && $_REQUEST['language_id'] != "" ) $language_id = $_REQUEST['language_id'];
   else $language_id = $userLanguage->language_id;

   $lang_select = '<select name="language_id" onchange=submit()>'.$languageList->getLanguagesSelect($language_id).'</select>';

   ?>
   <table align=center>
   <tr class=admin>
   <th class=admin colspan=8 align=center><?=$textLabels['regions']?></td>
   </tr>
   <tr class=admin>
      <FORM name="select_form" action="editregions.php" method=post>
      <th class=admin>&nbsp;#&nbsp;</th>
      <th class=admin>&nbsp;<a class='editor-option' href="<?=$OSCAILT_SCRIPT?>?sort=<?=$sort_mode?>"><?=$textLabels['name']?></a>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['language']?><br><?=$lang_select?></th>
      <th class=admin>&nbsp;<?=$textLabels['active']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['protected']?>&nbsp;</th>

      <th class=admin>&nbsp;<?=$textLabels['associated_stories']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['edit']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['delete']?>&nbsp;</th>
      </FORM>
   </tr>
   <?

   $regionList->reset();
   $regions = $regionList->getRegions($language_id);

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

      $language = $languageList->getLanguageByID($region->language_id);
      // If Db switch set then show Db Id.
      if(isset($_REQUEST['db_id']) && $_REQUEST['db_id']=="true" ) $t_index = ($i+1)." (".$region->region_id.")";
      else $t_index = $i+1;

      ?>
      <tr class=admin>
         <td class=admin>&nbsp;<?=$t_index?>&nbsp;</td>
         <td class=admin>&nbsp;<?=$region->region_name?>&nbsp;</td>
         <td class=admin>&nbsp;<?=$language->getLanguageCodePrefix()?>&nbsp;</td>
         <td class=admin align=center><?
         if($region->active==true) echo("<img src='graphics/active.gif'>");
         else echo("<img src='graphics/inactive.gif'>");
         ?></td>
         <td class=admin align=center><?

         if($region->excluded==true) echo("<img src='graphics/excluded.gif'>");

         else echo("<img src='graphics/included.gif'>");

         ?></td>

         <td class=admin align=center>&nbsp;<?=$region->getStoryCount()?>&nbsp;</td>

         <td class=admin align=center><a href="editregions.php?subpage=edit&region_id=<?=$region->region_id?>&language_id=<?=$region->language_id?>"><img src='graphics/edit.gif' border=0></a></td>

         <td class=admin align=center><a href="editregions.php?subpage=delete&region_id=<?=$region->region_id?>&language_id=<?=$region->language_id?>"><img src='graphics/delete.gif'  border=0></a></td>


      </tr>
      <?
   }
   ?>
   <tr>
      <form name="create_edit_form" action="editregions.php" method=post>
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


function writeEditBox($reload=false)
{
   global $languageList, $textLabels, $regionList;

   if ($reload == true ) {
       $regionList->reset();
   }

   $region = new Region();
   if (isset($_REQUEST['language_id']))
   { 
       $region->language_id = $_REQUEST['language_id'];
   } else {
       $region->language_id = $languageList->getMinLanguageId();
   }

   if (isset($_REQUEST['region_id']) )
       $region->region_id=$_REQUEST['region_id'];

   if($region->region_id != null) $region->load();

   $language = $languageList->getLanguageByID($region->language_id);
   
   $region_languages = $regionList->getAllRegionLanguagesByID($region->region_id);
 
   ?>
   <table class=admin align=center width=300>

   <form action="editregions.php" method=post>
   <input type=hidden name=subpage value="edit">
   <?
      if($region->region_id != null)
      {
	 ?>
         <input type=hidden name=region_id value="<?=$region->region_id?>">
         <input type=hidden name=language_id value="<?=$region->language_id?>">
         <?
      }
   ?>
   <tr class=admin>
      <th class=admin colspan=4>
      <?
      if($region->region_id != null) echo($textLabels["edit"]." ".$region->region_name);
      else echo($textLabels["CreateNewBtn"]);
      ?>
      </th>
   </tr>
   <?
   $region_name_langs = array();
   foreach ( $languageList->getLanguages() as $each_lang) { 
       $lang_code = $each_lang->getLanguageCodePrefix();
       $region_name_langs[$lang_code] = "";
       foreach ( $region_languages as $active_lang) { 
           if ($each_lang->language_id == $active_lang->language_id) 
	       $region_name_langs[$lang_code] = $active_lang->region_name;
       }
   }
   // <select name="language_id">   $languageList->getLanguagesSelect(1)  </select> 
   foreach ( $languageList->getLanguages() as $each_lang) { 
     $lang_code = $each_lang->getLanguageCodePrefix();
   ?>
   <tr class=admin>
   <td class=admin>&nbsp;<B><?=$textLabels['name']?></B> (<?=$lang_code;?>)&nbsp;</td>
      <td class=admin colspan=3><input type="text" name="region_name_<?=$lang_code?>" value="<?=$region_name_langs[$lang_code]?>" size="25"></td>
   </tr>
   <?
   }

   ?>
   <tr class=admin>
   <td class=admin colspan=4> <?=$textLabels['name_description']?> </td>
   </tr>
   <tr class=admin>
      <td class=admin><B><?=$textLabels['active']?></B></td>
      <td class=admin colspan=3 align=right><input type=checkbox name=active <? if($region->active==true) echo("checked"); ?>></td>

   </tr>

   <tr class=admin> 
   <td class=admin colspan=4> <?=$textLabels['active_description']?>
      </td> 
   </tr>

   <tr class=admin> 
   <td class=admin><B><?=$textLabels['protected']?></B></td>

      <td class=admin colspan=3 align=right><input type=checkbox name=excluded <? if($region->excluded==true) echo("checked"); ?>></td>

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
   $region = new Region();
   $region->region_id=$_REQUEST['region_id'];
   $region->language_id=$_REQUEST['language_id'];
   $region->load();
   ?>
   <table align=center>
   <form action="editregions.php" method=post>
   <input type=hidden name=subpage value="delete">
   <input type=hidden name=region_id value="<?=$region->region_id?>">
   <input type=hidden name=language_id value="<?=$region->language_id?>">
   <tr>
      <td colspan=2 align=center><img src="graphics/caution.gif" align=middle><BR><BR><B><?=$textLabels['are_you_sure']?> <?=$region->region_name?>?</B><BR><BR></td>
   </tr>
   <tr>
      <td align=right><input type=submit name=cancel value="&lt;&lt; <?=$textLabels['cancelBtn']?>"></td>
      <td ><input type=submit name=confirm value="<?=$textLabels['delete']?> &gt;&gt;"></td>
   </tr>
   </form>
   </table>
   <?
}


ob_start();

$textObj->writeUserMessageBox();

if($editor_session->isSessionOpen())
{
   writeAdminHeader("editlanguages.php","Edit Languages");

   if($editor_session->editor->allowedReadAccessTo("editregions"))
   {
      if(isset($_REQUEST['subpage']) && $_REQUEST['subpage']=="delete" && isset($_REQUEST['confirm']) )
      {
         $region = new Region();
         $region->region_id=$_REQUEST['region_id'];
         $region->language_id=$_REQUEST['language_id'];
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
               // writeError("All stories associated with '$region->region_name' must be deleted or reassigned<BR>before '$region->region_name' can be deleted");
               writeError($textLabels['delete_msg_1'].$region->region_name.$textLabels['delete_msg_2'].$region->region_name. $textLabels['delete_msg_3']);
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

         if(isset($_REQUEST["region_id"]) && $_REQUEST["region_id"] !="") $region->region_id=$_REQUEST['region_id'];
	 else $region->region_id=null;

         if(isset($_REQUEST["active"]) && ($_REQUEST["active"]=="yes" || $_REQUEST["active"]=="on"  || $_REQUEST["active"]=="true")) $region->active = 1;
         else $region->active = 0;
         if(isset($_REQUEST["excluded"]) && ($_REQUEST["excluded"]=="yes" || $_REQUEST["excluded"]=="on"  || $_REQUEST["excluded"]=="true")) $region->excluded = 1;

         else $region->excluded = 0;
 
         $t_show_list= false;
         foreach ( $languageList->getLanguages() as $each_lang) 
         {
            $lang_code = $each_lang->getLanguageCodePrefix();

            $t_name = "region_name_".$lang_code;
            $region->region_name = cleanseFormFieldFiller($_REQUEST[$t_name]);
            $region->language_id = $each_lang->language_id;

            if($region->region_name==null || $region->region_name=="")
            {
               writeError($textLabels['specify_name_error']." for ".$lang_code);
       	       // If first is created have to set id so it is displayed in edit box
               if ($t_show_list == true) $_REQUEST['region_id'] = $region->region_id;
               writeEditBox(true);
               $t_show_list = false;
               break;
            }
            else if($region->region_id==null && $regionList->getRegionByName($region->region_name, $region->language_id)!=null)
            {
               writeError($textLabels['region_error']);
               writeEditBox(true);
               $t_show_list = false;
               break;
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
                  $t_show_list = true;
                  // writeRegionList();
               }
               else
               {
                  $editor_session->writeNoWritePermissionError();
                  writeEditBox();
                  $t_show_list = false;
                  break;
               }
            }
         }
	 if ($t_show_list== true) writeRegionList();
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
