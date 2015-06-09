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
$OSCAILT_SCRIPT = "edittopics.php";
addToPageTitle("Manage Topics");


function writeTopicList()
{
   global $topicList, $OSCAILT_SCRIPT;

   if(isset($_REQUEST['sort']) && $_REQUEST['sort']=="true" ) $sort_mode = "false";
   else $sort_mode = "true";

   ?>
   <table align=center>
   <tr class=admin>
      <th class=admin colspan=7 align=center> Topics </th>
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

   $topicList->reset();
   $topics = $topicList->getTopics();

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
      ?>
      <tr class=admin>
         <td class=admin>&nbsp;<?=($i+1)?>&nbsp;</td>
         <td class=admin>&nbsp;<?=$topic->topic_name?>&nbsp;</td>
         <td class=admin align=center><?
         if($topic->active==true) echo("<img src='graphics/active.gif'>");
         else echo("<img src='graphics/inactive.gif'>");
         ?></td>
         <td class=admin align=center><?

         if($topic->excluded==true) echo("<img src='graphics/excluded.gif'>");

         else echo("<img src='graphics/included.gif'>");

         ?></td>

         <td class=admin align=center>&nbsp;<?=$topic->getStoryCount()?>&nbsp;</td>
         <td class=admin align=center><a href="edittopics.php?subpage=edit&topic_id=<?=$topic->topic_id?>"><img src='graphics/edit.gif' border=0></a></td>
         <td class=admin align=center><a href="edittopics.php?subpage=delete&topic_id=<?=$topic->topic_id?>"><img src='graphics/delete.gif'  border=0></a></td>
      </tr>
      <?
   }

   ?>
   <tr>
      <form action="edittopics.php" method=post>
      <input type=hidden name=subpage value="edit">
      <td colspan=7 align=center><input type=submit value="Create New Topic"></td>

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
   $topic = new Topic();
   if (isset($_REQUEST['topic_id']))
   {
       $topic->topic_id=$_REQUEST['topic_id'];
   }
   if($topic->topic_id != null) $topic->load();
   ?>
   <table class=admin align=center width=300>

   <form action="edittopics.php" method=post>
   <input type=hidden name=subpage value="edit">
   <?
      if($topic->topic_id != null)
      {
         ?><input type=hidden name=topic_id value="<?=$topic->topic_id?>"><?
      }
   ?>
   <tr class=admin>
      <th class=admin colspan=4>
      <?
      if($topic->topic_id != null) echo("Edit ".$topic->topic_name);
      else echo("Create New Topic");
      ?>
      </td>
   </tr>
   <tr class=admin>
      <td class=admin>&nbsp;<B>Name</B>&nbsp;</td>
      <td class=admin colspan=3><input type="text" name="topic_name" value="<?=$topic->topic_name?>"></td>
   </tr>
   <tr class=admin>
      <td class=admin colspan=4>
      Deactivating a topic will prevent new stories from being
      published using this topic. It will still be available for
      filtering and searching existing stories.
      </td>
   </tr>
   <tr class=admin>
      <td class=admin><B>Active</B></td>
      <td class=admin colspan=3 align=right><input type=checkbox name=active <? if($topic->active==true) echo("checked"); ?>></td>
   </tr>

   <tr class=admin>
      <td class=admin colspan=4>
      Protecting a topic will prevent stories of this topic from being visible to non-editors.
      </td>
   </tr>

   <tr class=admin>
      <td class=admin><B>Protected</B></td>

      <td class=admin colspan=3 align=right><input type=checkbox name=excluded <? if($topic->excluded==true) echo("checked"); ?>></td>
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
   $topic = new Topic();
   $topic->topic_id=$_REQUEST['topic_id'];
   $topic->load();
   ?>
   <table align=center>
   <form action="edittopics.php" method=post>
   <input type=hidden name=subpage value="delete">
   <input type=hidden name=topic_id value="<?=$topic->topic_id?>"><?
   ?>
   <tr>
      <td colspan=2 align=center><img src="graphics/caution.gif" align=middle><BR><BR><B>Are you sure you wish to delete <?=$topic->topic_name?>?</B><BR><BR></td>
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



   if($editor_session->editor->allowedReadAccessTo("edittopics"))
   {
      if(isset($_REQUEST['subpage']) && $_REQUEST['subpage']=="delete" && $_REQUEST['confirm']!=null)
      {
         $topic = new Topic();
         $topic->topic_id=$_REQUEST['topic_id'];
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
      else if(isset($_REQUEST['subpage']) && $_REQUEST['subpage']=="delete" && $_REQUEST['cancel']!=null)
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

         if(isset($_REQUEST['active']) && ($_REQUEST["active"]=="yes" || $_REQUEST["active"]=="on"  || $_REQUEST["active"]=="true")) $topic->active = true;
         else $topic->active = false;
         if(isset($_REQUEST['excluded']) && ($_REQUEST["excluded"]=="yes" || $_REQUEST["excluded"]=="on"  || $_REQUEST["excluded"]=="true")) $topic->excluded = true;

         else $topic->excluded = false;

         $topic->topic_name=cleanseTitleField($_REQUEST['topic_name']);
         if($topic->topic_name==null || $topic->topic_name=="")
         {
            writeError("Please Specify Name");
            writeEditBox();
         }
         else if($topic->topic_id==null && $topicList->getTopicByName($topic->topic_name)!=null)
         {
            writeError("A topic with this name already exists!");
            writeEditBox();
         }
         else
         {
            if(true ||$editor_session->editor->allowedWriteAccessTo("edittopics"))
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
               writeTopicList();
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
         writeTopicList();
      }
   }
   else $editor_session->writeNoReadPermissionError();
}
else $editor_session->writeNoSessionError();


include_once("adminfooter.inc");

?>