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

$OSCAILT_SCRIPT = "edittopics.php";
$textLabels = array("title" => "Manage Topics",
	            "topics" => "Topics",
	            "name" => "Name",
	            "language" => "Language",
	            "active" => "Active",
	            "protected" => "Protected",
	            "associated_stories" => "Associated Stories",
	            "edit" => "Edit",
	            "delete" => "Delete",
	            "CreateNewBtn" => "Create New Topic",
		    "name_description" => "Deactivating a topic will prevent new stories from being published using this topic. It will still be available for filtering and searching existing stories.",
	            "active_description" => "Protecting a topic will prevent stories of this topic from being visible to non-editors.",
	            "are_you_sure_text" => "Are you sure you wish to delete ",
	            "please_specify" => "Please Specify Name",
	            "topic_exists_already" => "A topic with this name already exists!",
	            "cancelBtn" => "Cancel",
	            "saveBtn" => "Save");

$textObj = new indyItemSet();

if($textObj->load($system_config->xml_store, "edittopics") === false)
{
    $textObj->setUserMessage( array( USER_ERROR, "Failed to get text strings for the Edit Topics. -Using defaults",""));
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



function writeTopicList()
{
   global $topicList, $OSCAILT_SCRIPT, $textLabels;
   global $languageList, $userLanguage;

   if(isset($_REQUEST['sort']) && $_REQUEST['sort']=="true" ) $sort_mode = "false";
   else $sort_mode = "true";

   if(isset($_REQUEST['language_id']) && $_REQUEST['language_id'] != "" ) $language_id = $_REQUEST['language_id'];
   else $language_id = $userLanguage->language_id;


   $lang_select = '<select name="language_id" onchange=submit()>'.$languageList->getLanguagesSelect($language_id).'</select>';

   ?>
   <table align=center>
   <tr class=admin>
   <th class=admin colspan=8 align=center> <?=$textLabels['topics']?> </th>
   </tr>
   <tr class=admin>
      <FORM name="select_form" action="edittopics.php" method=post>
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

   $topicList->reset();
   // The way the array is returned from getTopics ensures the keys are 1 to N even though originally the
   // keys match the topic_id is the Db Id and which may skip numbers. Use db_id to display Ids.
   $topics = $topicList->getTopics($language_id);

   // Sort by name. 
   for($i=0;$i<count($topics);$i++)
   {
      if(isset($_REQUEST['sort']) && $_REQUEST['sort']=="true" )
      {
         $topic=$topics[$i];
         $topicsArray[] = $topic->topic_name;
      }
      $array_order[] = $i;
   }
   // Sort by name. 
   if (isset($_REQUEST['sort']) && $_REQUEST['sort']=="true" )
   {
      array_multisort($topicsArray, $array_order);
   }

   for($i=0;$i<count($topics);$i++)
   {
      $sorted_index=$array_order[$i];
      $topic=$topics[$sorted_index];

      $language = $languageList->getLanguageByID($topic->language_id);
      // If Db switch set then show Db Id.
      if(isset($_REQUEST['db_id']) && $_REQUEST['db_id']=="true" ) $t_index = ($i+1)." (".$topic->topic_id.")";
      else $t_index = $i+1;

      ?>
      <tr class=admin>
         <td class=admin>&nbsp;<?=$t_index?>&nbsp;</td>
         <td class=admin>&nbsp;<?=$topic->topic_name?>&nbsp;</td>
         <td class=admin>&nbsp;<?=$language->getLanguageCodePrefix()?>&nbsp;</td>
         <td class=admin align=center><?
         if($topic->active==true) echo("<img src='graphics/active.gif'>");
         else echo("<img src='graphics/inactive.gif'>");
         ?></td>
         <td class=admin align=center><?

         if($topic->excluded==true) echo("<img src='graphics/excluded.gif'>");

         else echo("<img src='graphics/included.gif'>");

         ?></td>

         <td class=admin align=center>&nbsp;<?=$topic->getStoryCount()?>&nbsp;</td>
	 <td class=admin align=center><a href="edittopics.php?subpage=edit&topic_id=<?=$topic->topic_id?>&language_id=<?=$topic->language_id?>"><img src='graphics/edit.gif' border=0></a></td>
	 <td class=admin align=center><a href="edittopics.php?subpage=delete&topic_id=<?=$topic->topic_id?>&language_id=<?=$topic->language_id?>"><img src='graphics/delete.gif'  border=0></a></td>
      </tr>
      <?
   }

   ?>
   <tr>
      <FORM name="create_edit_form" action="edittopics.php" method=post>
        <input type=hidden name=subpage value="edit">
        <td colspan=8 align=center><input type=submit value="<?=$textLabels['CreateNewBtn']?>"></td>
      </FORM>
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
   global $languageList, $textLabels, $topicList;

   if ($reload == true ) {
       $topicList->reset();
   }

   $topic = new Topic();
   if (isset($_REQUEST['language_id']))
   { 
       $topic->language_id = $_REQUEST['language_id'];
   } else {
       $topic->language_id = $languageList->getMinLanguageId();
   }

   if (isset($_REQUEST['topic_id']))
   {
       $topic->topic_id=$_REQUEST['topic_id'];
   }
   if($topic->topic_id != null) $topic->load();

   $language = $languageList->getLanguageByID($topic->language_id);
   
   $topic_languages = $topicList->getAllTopicLanguagesByID($topic->topic_id);

   // foreach ($topic_languages as $each_topic) {
       //echo "topic ".$each_topic->topic_name." lang " .$each_topic->language_id . "<BR>";
   // }

   ?>
   <table class=admin align=center width=300 border=0>

   <form action="edittopics.php" method=post>
   <input type=hidden name=subpage value="edit">
   <?
      if($topic->topic_id != null)
      {
         ?>
         <input type=hidden name=topic_id value="<?=$topic->topic_id?>">
         <input type=hidden name=language_id value="<?=$topic->language_id?>">
         <?
      }
   ?>
   <tr class=admin>
      <th class=admin colspan=3>
      <?
      if($topic->topic_id != null) echo($textLabels['edit']." ".$topic->topic_name);
      else echo($textLabels['CreateNewBtn']);
      ?>
      </td>
   </tr>
   <?
   $topic_name_langs = array();
   foreach ( $languageList->getLanguages() as $each_lang) { 
       $lang_code = $each_lang->getLanguageCodePrefix();
       $topic_name_langs[$lang_code] = "";
       foreach ( $topic_languages as $active_lang) { 
           if ($each_lang->language_id == $active_lang->language_id) 
	       $topic_name_langs[$lang_code] = $active_lang->topic_name;
       }
   }
   // <select name="language_id">   $languageList->getLanguagesSelect(1)  </select> 
   foreach ( $languageList->getLanguages() as $each_lang) { 
     $lang_code = $each_lang->getLanguageCodePrefix();
   ?>
   <tr class=admin>
      <td class=admin>&nbsp;<B><?=$textLabels['name']?></B> (<?=$lang_code;?>)&nbsp;</td>
      <td class=admin colspan=2> <input type="text" name="topic_name_<?=$lang_code?>" value="<?=$topic_name_langs[$lang_code]?>" size="30"></td>
   </tr>
   <?
   }

   ?>
   <tr class=admin>
   <td class=admin colspan=3><?=$textLabels['name_description']?>
      <!--Deactivating a topic will prevent new stories from being published using this topic. It will still be 
          available for filtering and searching existing stories.-->
      </td>
   </tr>
   <tr class=admin>
   <td class=admin><B><?=$textLabels['active']?></B></td>
      <td class=admin colspan=2 align=right><input type=checkbox name=active <? if($topic->active==true) echo("checked"); ?>></td>
   </tr>

   <tr class=admin>
      <td class=admin colspan=3>
      <?=$textLabels['active_description']?>
      <!-- Protecting a topic will prevent stories of this topic from being visible to non-editors.-->
      </td>
   </tr>

   <tr class=admin>
      <td class=admin><B><?=$textLabels['protected']?></B></td> 
      <td class=admin colspan=2 align=right><input type=checkbox name=excluded <? if($topic->excluded==true) echo("checked"); ?>></td>
   </tr>
   <tr>
      <td colspan=3 align=center>
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

   $topic = new Topic();
   $topic->topic_id=$_REQUEST['topic_id'];
   $topic->language_id=$_REQUEST['language_id'];
   $topic->load();
   ?>
   <table align=center>
   <form action="edittopics.php" method=post>
   <input type=hidden name=subpage value="delete">
   <input type=hidden name=topic_id value="<?=$topic->topic_id?>">
   <input type=hidden name=language_id value="<?=$topic->language_id?>">
   <tr>
      <td colspan=2 align=center><img src="graphics/caution.gif" align=middle><BR><BR><B><?=$textLabels['are_you_sure_text']?> <?=$topic->topic_name?>?</B><BR><BR></td>
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
   writeAdminHeader("editlanguages.php","Edit Languages");



   if($editor_session->editor->allowedReadAccessTo("edittopics"))
   {
      if(isset($_REQUEST['subpage']) && isset($_REQUEST['confirm']) && $_REQUEST['subpage']=="delete" && $_REQUEST['confirm']!=null)
      {
         $topic = new Topic();
         $topic->topic_id=$_REQUEST['topic_id'];
         $topic->language_id=$_REQUEST['language_id'];
         $topic->load();
         if($topic->topic_id != null)
         {
            if($topic->getStoryCount()==0)
            {
               if($editor_session->editor->allowedWriteAccessTo("edittopics"))
               {
                  logAction("", $topic->topic_id, "topic", "delete", "(Field " .$topic->getName().")");
                  $topic->delete();
               }
               else $editor_session->writeNoWritePermissionError();
            }
            else
            {
               writeError("All stories associated with '$topic->topic_name' must be deleted or reassigned<BR>before '$topic->topic_name' can be deleted");
            }
         }
         writeTopicList();
      }
      else if(isset($_REQUEST['subpage']) && isset($_REQUEST['cancel']) && $_REQUEST['subpage']=="delete" && $_REQUEST['cancel']!=null)
      {
         writeTopicList();
      }
      else if(isset($_REQUEST['subpage']) && $_REQUEST['subpage']=="delete")
      {
         writeConfirmDeleteBox();
      }
      else if(isset($_REQUEST['subpage']) && $_REQUEST['subpage']=="edit" && isset($_REQUEST['save']) && $_REQUEST['save']!=null)
      {
         $topic= new Topic();
         if (isset($_REQUEST['topic_id'])) $topic->topic_id=$_REQUEST['topic_id'];

         if(isset($_REQUEST['active']) && ($_REQUEST["active"]=="yes" || $_REQUEST["active"]=="on"  || $_REQUEST["active"]=="true")) $topic->active = 1;
         else $topic->active = 0;
         if(isset($_REQUEST['excluded']) && ($_REQUEST["excluded"]=="yes" || $_REQUEST["excluded"]=="on"  || $_REQUEST["excluded"]=="true")) $topic->excluded = 1;

         else $topic->excluded = 0;

	 $t_show_list= false;
	 foreach ( $languageList->getLanguages() as $each_lang) 
	 {
            $lang_code = $each_lang->getLanguageCodePrefix();

            $t_name = "topic_name_".$lang_code;
            $topic->topic_name = cleanseFormFieldFiller($_REQUEST[$t_name]);
            $topic->language_id = $each_lang->language_id;

            if($topic->topic_name==null || $topic->topic_name=="")
            {
               writeError($textLabels['please_specify']." for ".$lang_code);
	       // If first is created have to set id so it is displayed in edit box
	       if ($t_show_list == true) $_REQUEST['topic_id'] = $topic->topic_id;
               writeEditBox(true);
               $t_show_list = false;
               break;
            }
            else if($topic->topic_id==null && $topicList->getTopicByName($topic->topic_name)!=null)
            {
               writeError($textLabels['topic_exists_already']);
               writeEditBox(true);
               $t_show_list = false;
               break;
            }
            else
            {
               if($editor_session->editor->allowedWriteAccessTo("edittopics"))
               {
                  if ($topic->topic_id == null) {
                     $t_action = "create";
                  } else {
                     $t_action = "update";
                  }
                  $topic->save();
                  // topic id gets updated from the insert.
                  // Fill the description of what was updated into the action_reason parameter.
                  logAction("", $topic->topic_id, "topic", $t_action, "(Field " .$topic->getName().")");
                  $t_show_list = true;
                  // writeTopicList();
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
	 if ($t_show_list== true) writeTopicList();
      }
      else if(isset($_REQUEST['subpage']) && $_REQUEST['subpage']=="edit" && !isset($_REQUEST['cancel']) )
      {
         writeEditBox();
      }
      else
      {
         writeTopicList();
      }
   }
   else $editor_session->writeNoReadPermissionError();
}
else $editor_session->writeNoSessionError();


include_once("adminfooter.inc");

?>
