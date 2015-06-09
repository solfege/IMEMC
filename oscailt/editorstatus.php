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
require_once("objects/publishstate.inc");
require_once("objects/reminderlist.inc");
require_once("objects/indyobjects/indydataobjects.inc");
require_once("objects/indyobjects/indyobject.inc");
require_once("objects/indyobjects/indyitemset.inc");
require_once("objects/adminutilities.inc");
require_once("objects/languagedates.inc");
require_once("objects/publiclist.inc");

$textLabels = array("title" => "Editor Status and System Messaging Display",
	            "editorial_locks_link" => "Editorial Locks",
	            "editor_profile_link" => "View Editor Profiles",
	            "reminders_link" => "View & Add Reminders",
	            "you_are_user_text" => "You are user",
	            "user_heading" => "User",
	            "time_heading" => "Time",
		    "status_heading" => "Status",
	            "msg_heading" => "System Message",
	            "last_viewed_msg" => "Last viewed",
	            "user_status_msg" => "User Status",
	            "note_text" => "Logged in users may have timed out.",
	            "user_maybe_timeout_text" => "User probably timed-out.",
	            "form_post_msg" => "Post a message &nbsp (max 180 chars) &nbsp;",
	            "editorial_locks_text" => "Total Editorial Locks on Stories and Comments:",
	            "editorial_locks_note_text" => "Editorial locks are put in place when an editor is editing either a story or comment. e.g while preparing a feature.",
	            "el_object_id_heading" => "Obj Id",
	            "el_type_heading" => "Type",
	            "el_locked_by_heading" => "Locked By",
	            "el_locked_at_heading" => "Locked At",
	            "el_locked_until_heading" => "Locked Until",
	            "el_state_heading" => "State",
	            "mins_word" => "mins",
	            "seconds_left" => "secs left",
	            "no_locks_text" => "There are no editorial locks for stories or comments in place.",
	            "total_schtasks_text" => "Total Scheduled Tasks Pending",
	            "st_scheduled_for_text" => "Scheduled For ",
	            "st_type_text" => "Type",
	            "st_object_text" => "Obj Id",
	            "st_action_text" => "Action",
	            "st_sch_task_msg_text" => "Scheduled Task Message",
	            "st_delete_text" => "Delete",
	            "sched_task_info_text" => "Scheduled tasks can be setup at the point when a feature is being unhidden so that it can be timed to automatically unhide later.",
	            "total_reminders_text" => "Total Reminders Pending",
	            "r_scheduled_for_text" => "Scheduled For ",
	            "r_reminder_msg_text" => "Reminder Message",
	            "r_delete_text" => "Delete",
	            "setup_reminder_btn_text" => "Setup a reminder message &nbsp; (max 180 chars)",
	            "reminder_label_text" => "Reminder",
	            "set_reminder_label_text" => "Set Reminder For",
	            "public_edits_disabled_text" => "Public Editing is disabled. See configuration page to enable it",
	            "public_edits_total_text" => "Total Public Edits Allowed :",
	            "public_edits_total_subtext" => "You can delete entries, enable use of passwords and edit password for any story",
	            "pe_story_id_text" => "Story Id",
	            "pe_init_time_text" => "Init Time",
	            "pe_expires_text" => "Expires",
	            "pe_edit_expires_text" => "Edit Expires",
	            "pe_granted_text" => "Granted By",
	            "pe_enable_pass_text" => "Enable Pass",
	            "pe_password_text" => "Password",
	            "pe_user_session_text" => "User Session",
                    "pe_delete_text" => "Delete",
	            "pe_save_changes_btn_text" => "Save Changes",
	            "pe_edit_password_btn_text" => "Edit Password",
	            "scratch_pad_title" => "Oscailt Scratch Pad",
	            "scratch_pad_info" => "Use this screen for writing notes or ideas or drafts of letters and emails.",
	            "scratch_pad_note" => "Beware that another user is not trying to save the text at the same time.",
	            "save_scratch_pad_btn_text" => "Save ScratchPad",
	            "scratch_pad_saved_text" => "Scratch pad file saved successfully!",
	            "editor_profiles_title" => "Editor Profiles for all Oscailt users",
	            "ep_name_label" => "Name",
	            "ep_email_label" => "Email",
	            "ep_rx_story_label" => "Receive Story Notifications",
	            "ep_rx_comment_label" => "Receive Comment Notifications",
	            "ep_details_label" => "Additional Details",
	            "ep_login_label" => "Last Login"
	            );
	    
$OSCAILT_SCRIPT = "editorstatus.php";
$textObj = new indyItemSet();

if($textObj->load($system_config->xml_store, "editorstatus") === false)
{
    $textObj->setUserMessage( array( USER_ERROR, "Failed to get text strings for the Editor Status. -Using defaults",""));
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

addToPageTitle($textLabels['title']);

function writeLocalAdminHeader()
{
   global $OSCAILT_SCRIPT, $system_config;

   if ( isset($_REQUEST['relative']) && $_REQUEST['relative'] == 'true')
   {
      $time_url_mode = "false";
   } else {
      $time_url_mode = "true";
   }


   if ( isset($_REQUEST['reverse']) && $_REQUEST['reverse'] == 'true')
   {
      $reverse_url_mode = "false";
      $reverse_url_msg = "Reverse Order";
   } else {
      $reverse_url_mode = "true";
      $reverse_url_msg = "Reverse Order";
   }

   $show_reverse = false;
   if ( !isset($_REQUEST['viewprofile']) && !isset($_REQUEST['locks']) && !isset($_REQUEST['reminder']) && !isset($_REQUEST['schedule']) && !isset($_REQUEST['publicedits']) && !isset($_REQUEST['scratchpad']) ) $show_reverse = true;

   ?> <TABLE border=0 class='admin'><TR class='admin'> <TD class='admin' width="90%">
        <a href="<?=$OSCAILT_SCRIPT?>?relative=<?=$time_url_mode?>">User Status &amp; Msgs</a>
	| <a href="<?=$OSCAILT_SCRIPT?>?viewprofile=true&relative=<?=$time_url_mode?>">View Editor Profiles</a>
	| <a href="<?=$OSCAILT_SCRIPT?>?locks=true">Editorial Locks</a>
	| <a href="<?=$OSCAILT_SCRIPT?>?reminder=true">View & Add Reminders</a>
	| <a href="<?=$OSCAILT_SCRIPT?>?schedule=true">View Scheduled Tasks</a>
	| <a href="<?=$OSCAILT_SCRIPT?>?publicedits=true">Public Edits</a>
	| <a href="<?=$OSCAILT_SCRIPT?>?scratchpad=true">Scratch Pad</a>
      </TD> <TD class='admin' align="right">
   <?
   if ($show_reverse == true) {
   ?>
        <a href="<?=$OSCAILT_SCRIPT?>?relative=true&reverse=<?=$reverse_url_mode?>"><?=$reverse_url_msg?></a> 
   <?
   }

   // Get the base path of the site and use that to subtract off that from the HTTP_REFERER
   if (isset($_SERVER['HTTP_REFERER'])) 
   {
       $http_mode = isset($_SERVER['HTTPS']) ? "https" : "http";
       $dom_info = parse_url($http_mode."://".$system_config->site_url);
       $sitepath = isset($dom_info['path']) ? $dom_info['path']."/" : "/";
    
       $dom_info = parse_url($_SERVER['HTTP_REFERER']);
       if ($sitepath == "/" || $sitepath == "" )
           $return_url = $_SERVER['HTTP_REFERER'];
       else
       {
           $refer_script = str_replace($sitepath,"",$dom_info['path']);
           $return_url = $http_mode."://" .$system_config->site_url . "/" . $refer_script;
       }
    
       if ($refer_script != $OSCAILT_SCRIPT && $refer_script != "admin.php")
       {
          ?> </TD></TR>
          <TR class='admin'><TD class='admin' colspan=2> Back to <a href="<?=$return_url?>">Site</a> 
          <?
       }
   }
   ?>
      </TD></TR>
   </TABLE>
   <?
}


function writeStateAndSystemMsgsList( $relative_time=true,$reverse_order=true)
{
   global $editorStateMsgList, $system_config, $OSCAILT_SCRIPT;
   global $editor_session, $textLabels;
   $current_time = time();

   $timeMsg = strftime($system_config->default_strftime_format, $current_time + $system_config->timezone_offset);
   $editorname = $editor_session->editor->editor_name;

   // I had added code to show a counter but we do not display entries for 'views' so it messes things up
   // $show_counter = 0;
   // if ( isset($_REQUEST['number']) && $_REQUEST['number'] == 'true') $show_counter = 1;
   if ( isset($_REQUEST['relative']) && $_REQUEST['relative'] == 'true')
   {
      $time_url_mode = "false";
      $time_url_msg = "Display Timestamp";
   } else {
      $time_url_mode = "true";
      $time_url_msg = "Relative Timestamp";
   }


   ?>
   <table align=center width=100%>
   <tr class=admin>
      <th class=admin colspan="2">&nbsp;<?=$timeMsg?>&nbsp;</th>
      <th class=admin colspan="2"><?=$textLabels['you_are_user_text']?>: <u><?=$editorname?></u> </th>
   </tr>
   <?

   if ( $reverse_order == true ) displayStatusMessageButton();

   $totalCount = count($editorStateMsgList->recent_publishes);

   if ( $reverse_order == true ) displayUserAnalysisList($totalCount, $editorname, $current_time, $relative_time);

   ?>
   <tr class=admin>
      <th class=admin>&nbsp;<?=$textLabels['user_heading']?>&nbsp;</th> 
      <th class=admin>&nbsp;<?=$textLabels['time_heading']?> <small>(<a class='editor-option' href="<?=$OSCAILT_SCRIPT?>?relative=<?=$time_url_mode?>"><?=$time_url_msg?></a>)</small> &nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['status_heading']?>&nbsp;</th> 
      <th class=admin>&nbsp;<?=$textLabels['msg_heading']?>&nbsp;</th>
   </tr>
   <?
   // 

   for($i=0; $i < $totalCount;$i++)
   {
      if ( $reverse_order == true ) $list_index = ($totalCount - $i -1);
      else $list_index = $i;

      $r=$editorStateMsgList->recent_publishes[$list_index];

      if ($r->user_state == "view" ) continue;

      $username = $r->username;
      if ($editorname == $username ) $username = "<b>". $username . "</b>";

      $state_class = "admin";
      if ($r->user_state == "Login" ) $state_class = "admin_highlight";
      if ($r->user_state == "Logged out" ) $state_class = "admin_fade";

      if ($relative_time == true ) 
      {
          $t_diff = $current_time - $r->time_posted;
          $t_str = getTimeAgoString($t_diff);
      }
      else
      {
          $t_str = strftime("%a %H:%M:%S", ($r->time_posted + $system_config->timezone_offset));
      }

      $sysmsg = $r->sys_message;
      if (strpos($sysmsg, "<not specified>") === false) {
          // Do nothing
      } else {
          $sysmsg = str_replace(">", "&gt;", $sysmsg);
          $sysmsg = str_replace("<", "&lt;", $sysmsg);
      }


      ?>
      <tr class=admin>
         <td class=<?=$state_class?> align=center>&nbsp;<?=$username?>&nbsp;</td>
         <td class=<?=$state_class?> align=left><?=$t_str?></td>
         <td class=<?=$state_class?> align=center><?=$r->user_state?></td>
         <td class=admin align=left><?=$sysmsg?></td>
      </tr>
      <?
   }

   if ( $reverse_order == false ) displayStatusMessageButton();

   if ( $reverse_order == false ) displayUserAnalysisList($totalCount, $editorname, $current_time, $relative_time);

   ?>
   </table>
   <?
}

// This function goes through the list to find all the unique users and determines their state
// and filters out those who viewed the display and shows the time that they did.
function displayUserAnalysisList( $totalCount, $this_editor, $current_time, $relative_time)
{
   global $editorStateMsgList, $system_config, $OSCAILT_SCRIPT, $textLabels;

   // Display logged out users status
   $show_logged = true;
   // Display logged in users status but who have not viewed this screen
   $show_logged_in = false;
   $show_logged_in = true;
   $state_class = "admin";

   // Create a list of unique users found. Then go back through list and work out
   // when they last refreshed this page.
   $username_stack = array($this_editor );
   $username_status= array("");
   $username_statustime = array(0);
   $username_viewtime = array(0);

   $prev_user = $editorStateMsgList->recent_publishes[($totalCount-1)]->username;
   if ($prev_user == "") $prev_user = "Unknown";

   // Get the unique list of users (editors)
   for($i=0; $i < $totalCount;$i++)
   {
      $list_index = ($totalCount - $i -1);

      $rs = $editorStateMsgList->recent_publishes[$list_index];
      $username = $rs->username;
      if ($username == "") $username = "Unknown";

      // If user same as previous then no point searching twice into array
      if ($username == $prev_user ) continue;

      if ( ! in_array($username, $username_stack) )
      {
          $username_stack[] = $username;
          $username_status[]= "-";
          $username_statustime[] = 0;
          $username_viewtime[] = 0;
      }
      $prev_user = $username;
   }

   // Now determine their states by loop for each user, but must travel in reverse up list.
   $totalActiveUsers = count($username_stack);
   for($i_user=0; $i_user < $totalActiveUsers; $i_user++)
   {
       $last_ref = -1;
       $found_view = false;
       $found_logout = false;
       for($i_list=0; $i_list < $totalCount;$i_list++)
       {
          $list_index = ($totalCount - $i_list -1);
          // $list_index = $i_list ;
          $rs = $editorStateMsgList->recent_publishes[$list_index];
          if ($username_stack[$i_user] == $rs->username )
          {
              if ($rs->user_state == "Login" )
	      {
                  // If we already found at an earlier time that they had logged out, then thats the state, not this
                  if ($found_logout == false && $last_ref < 0) {
                      $username_status[$i_user] = "Login";
                      $username_statustime[$i_user] = $rs->time_posted;
                  }
                  $last_ref = $i_user;
	      }
	      elseif ($rs->user_state == "Logged out" )
	      {
		  // In other words if we already detected login then ignore earlier logout but quit loop.
	          if ($last_ref >= 0 )
                  {
	              if ($username_status[$last_ref] == "Login" )
                      {
                          // And mark them as login. Logout really says stop searching.
                          $username_status[$i_user] = "Login";
                          $username_statustime[$i_user] = $username_statustime[$last_ref];
                          if ($found_view == false) break;
                          break;
                      }
                  }

                  $username_status[$i_user] = "logout";
                  if ($found_logout == false) $username_statustime[$i_user] = $rs->time_posted;
                  // If we have not found an instance of view, but we have of logout we still want to know
                  // if and when this user last viewewd this screen, so we press on.
                  if ($found_view == true) break;
                  $found_logout = true;
	      }
	      elseif ($rs->user_state == "view" )
	      {
                  // Because of the way views are recored there can be 2 in the list, so we
                  // do not want the later time, we just want the earliest or most recent view time.
                  if ($found_view == false) $username_viewtime[$i_user] = $rs->time_posted;
                  if ($found_logout == true) break;
                  $found_view = true;
	      }
	      elseif ($rs->user_state == "post")
	      {
                  // A post and a view are the same really, hence comment above applies.
                  if ($found_view == false) $username_viewtime[$i_user] = $rs->time_posted;
                  if ($found_logout == true) break;
                  $found_view  = true;
	      }
          }
       }
   }

   // Now display the unique list and the status of the users.
   // Text was: Last time this display viewed
   ?>
   <tr class=admin>
      <th class=admin>&nbsp;<?=$textLabels['user_heading']?>&nbsp;</th>
      <th class=admin colspan="2">&nbsp;<?=$textLabels['last_viewed_msg']?>&nbsp;</th>
      <th class=admin >&nbsp;<?=$textLabels['user_status_msg']?>&nbsp;<BR><small>&nbsp; Note:<i> <?=$textLabels['note_text']?></i></small></th>
   </tr>
   <?
   // 
   $totalActiveUsers = count($username_stack);
   for($i_user=0; $i_user < $totalActiveUsers;$i_user++)
   {
      $username = $username_stack[$i_user];
      // System message will be written by non-existant user 'system', so we ignore status of this.
      if ($username == "system") continue;

      $view_state = "";
      if ($username_viewtime[$i_user] > 0 ) 
      {
          // $view_state = "Last viewed this display ";
          $view_state = "";
          if ($relative_time == true ) {
              $time_diff = ($current_time - $username_viewtime[$i_user] );
              // 2nd parameter means if more than 1hr, do not add secs ago to string
	      $t_str = getTimeAgoString($time_diff, true);
	  }
	  else 
	  {
              // $view_state .= "at ";
	      $t_str = strftime("%H:%M:%S %a", $username_viewtime[$i_user] + $system_config->timezone_offset);
	  }

          $view_state .= "<b>" . $t_str . "</b>";
      }

      $user_state = "Default: " . $username_status[$i_user];
      if ($username_status[$i_user] == "logout")
      {
          if ($show_logged == false) continue;

          $user_state = "<font color='#FF0000'> Logged out </font>";
          if ($relative_time == true ) {
              $time_diff = ($current_time - $username_statustime[$i_user] );
	      $t_str = getTimeAgoString($time_diff);
	  }
	  else 
	  {
              $user_state .= "at ";
	      $t_str = strftime("%H:%M:%S %a", ($username_statustime[$i_user] + $system_config->timezone_offset));
          }

          $user_state .= "<b>" . $t_str . "</b>";
      }
      elseif ($username_status[$i_user] == "Login")
      {
          if ($show_logged_in == false) continue;

          $user_state = "<font color='#49594C'> Login </font>";
          if ($relative_time == true ) {
              $time_diff = ($current_time - $username_statustime[$i_user] );
	      $t_str = getTimeAgoString($time_diff);
	  }
	  else 
	  {
              $user_state .= "at ";
	      $t_str = strftime("%H:%M:%S %a", ($username_statustime[$i_user] + $system_config->timezone_offset));
          }

          $user_state .= "<b>" . $t_str . "</b>";
          // If logged in over 18 hrs ago, assume they are timed out.
          if ($relative_time == true && $time_diff > 64800) {
              $user_state .= " <font color='red'><i>".$textLabels['user_maybe_timeout_text']."</i></font>";
          }
      }

      ?>
      <tr class=admin>
         <td class=<?=$state_class?> align=center>&nbsp;<?=$username?>&nbsp;</td>
         <td class=<?=$state_class?> align=center colspan="2" >&nbsp;<?=$view_state?>&nbsp;</td>
         <td class=<?=$state_class?> align=left >&nbsp;<?=$user_state?>&nbsp;</td>
      </tr>
      <?
   }
   /*
   ?>
   <tr class=admin>
     <td class=admin colspan="4"></td>
   </tr>
   <?
   */

}

function displayStatusMessageButton()
{
   global $OSCAILT_SCRIPT, $textLabels;
   $post_msg_fld = "<INPUT maxLength=180 size=130 type='text' name='system_msg' value='' oninput='javaScript:onTypeChange()'>";
   $post_msg_btn = "<INPUT type='submit' name='action' value='".$textLabels['form_post_msg']." >>'>";
   $post_msg_cnt = "<INPUT type='button' name='remaining' value='180 chars'>";

   ?>
    <SCRIPT LANGUAGE="JavaScript">
    function onTypeChange() {
	var inputMsg = document.editormessage.system_msg.value;
	if (inputMsg.length >= 0 )
	{
	    var nLeft = 180 - inputMsg.length;
            document.editormessage.remaining.value = nLeft + " chars";
	} 
    }
    function submitHandler() {
	var inputMsg = document.editormessage.system_msg.value;
	if (inputMsg.length > 0 )
	{
            document.editormessage.subpage.value="update_sys_message";
	} 
    }
    </SCRIPT>
    <tr class=admin>
      <td class=admin align=center colspan="4">
      <FORM name="editormessage" action="<?=$OSCAILT_SCRIPT?>" onSubmit="submitHandler()" method="POST">
      <input type='hidden' name='subpage' value=''>
      Message &gt;&gt; <?=$post_msg_fld?>
      <br> <br> <?=$post_msg_btn?>
      <?=$post_msg_cnt?>
      </FORM>
      </td>
    </tr>
   <?

   // Note: Table is closed above. So be carefull where and how this is called.
}
function displayLockedStoriesScreen()
{
   global $editor_session, $system_config, $OSCAILT_SCRIPT, $textLabels;
   $show_indy_locks = true;
   $num_lock_groups = 2;

   $baseEditLock = new EditLock();
   $story_editLockList = $baseEditLock->load("story");
   $comment_editLockList = $baseEditLock->load("comment");

   if ($show_indy_locks == true)
   {
      $num_lock_groups = 3;
      $indy_editLockList = $baseEditLock->load("indyobject");
   }

   // Get the total count.
   $lockCount = count($story_editLockList) + count($comment_editLockList);
   if ($show_indy_locks == true)
   {
      $lockCount = $lockCount + count($indy_editLockList);
      $obj_man = new indyObjectManager($system_config->xmltypedef_dir, $OSCAILT_SCRIPT);
      $obj_man->obj_set = new indyObjectSet($obj_man->type_dir, $obj_man->storage);
      $sites = array("*");
      $types = array("*");
      if(!$obj_man->obj_set->load($sites, $types, $obj_man->action_req))
      {
          $obj_loaded = false;
      } else {
          $obj_loaded = true;
          $available_objects = $obj_man->obj_set->getObjectStubs();
      }
   }

   $timeMsg = strftime($system_config->default_strftime_format, time() + $system_config->timezone_offset);
   
   $show_indy_locks = true;
   ?>
   <table align=center width=100%>
   <tr class=admin>
      <th class=admin colspan=7> <?=$timeMsg?> </th>
   </tr>
   <tr class=admin>
      <th class=admin colspan=7><?=$textLabels['editorial_locks_text']?> <b><?=$lockCount?></b> 
      <br><small><?=$textLabels['editorial_locks_note_text']?></small>
      </th>
   </tr>
   <tr class=admin>
      <th class=admin>&nbsp;#&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['el_object_id_heading']?>&nbsp;</th> 
      <th class=admin>&nbsp;<?=$textLabels['el_type_heading']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['el_locked_by_heading']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['el_locked_at_heading']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['el_locked_until_heading']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['el_state_heading']?>&nbsp;</th>
   </tr>
   <?
   if ($lockCount > 0 ) 
   {
      // No point loadings refs to eds, if no locks.
      $editorList = new editorList();
      $editorList->load();
      $http_mode = isset($_SERVER['HTTPS']) ? "https" : "http";
      $url_base = '<a href="'.$http_mode.'://'.$system_config->site_url.'/';

      $lock_index = 0;
      for($itype=0; $itype < $num_lock_groups;$itype++)
      {
         if ($itype == 0) $array_ptr = $story_editLockList;
         else if ($itype == 1) $array_ptr = $comment_editLockList;
         else if ($itype == 2) $array_ptr = $indy_editLockList;

         for($i=0; $i < count($array_ptr);$i++)
         {
             $lock_index++;
             $lockObj = $array_ptr[$i];
             // Windows doesn't support day of the month item: %e
             $lock_at = strftime("%d %a %B at %H:%M:%S", ($lockObj->lock_time_granted + $system_config->timezone_offset));
             $lock_until=strftime("%d %a %B at %H:%M:%S", ($lockObj->lock_expiry_time + $system_config->timezone_offset));
	     if ($lockObj->lock_owner == 9900) { 
		$locked_by_ed = "Joe Public";
	     } else {
		$locked_by_edObj = $editorList->getEditorByID($lockObj->lock_owner);
		$locked_by_ed = $locked_by_edObj->editor_name;
	     }

             // Generate a URL for stories.
             if ($itype == 0) {
   	         $lock_target = $url_base . 'article/' . $lockObj->target_id. '">' .$lockObj->target_id.'</a>';
             } else {
   	         $lock_target = $lockObj->target_id;
             }
   	     $lock_type = $lockObj->target_type;

	     if (time() > $lockObj->lock_expiry_time) {
   	         $lock_state = "<b>expired</b>";
   	         $lookup_type = false;
             } else {
   	         $r_mins = floor(($lockObj->lock_expiry_time -time())/60);
   	         $r_secs = ( ($lockObj->lock_expiry_time -time())- (60 * $r_mins));
   	         $lock_state = $r_mins . " mins " . $r_secs . " secs left";
   	         $lookup_type = true;
             }
     	     // For indytype, see what type it is.
             if ($lookup_type == true && $itype == 2 && $obj_loaded == true) {
                 // Lookup the id and see what type it is...
                 foreach($available_objects as $obj_stub)
                 {
                     if( $lock_target == $obj_stub->obj_id) {
                         $lock_type = "indyobject<BR>(".ucfirst(strtolower($obj_stub->obj_type)).")";
                         break;
                     }
                 }
             }

         ?>
         <tr class=admin>
            <td class=admin align="center"><?=$lock_index?></td>
   	    <td class=admin align="center"><?=$lock_target?></td>
   	    <td class=admin align="center"><?=$lock_type?></td>
   	    <td class=admin align="center"><?=$locked_by_ed?></td>
   	    <td class=admin align="center"><?=$lock_at?></td>
   	    <td class=admin align="center"><?=$lock_until?></td>
   	    <td class=admin align="center"><?=$lock_state?></td>
         </tr>
         <?
         }
      }
   } else {
      ?>
      <tr class=admin>
          <td class=admin align="center" colspan=7> <?=$textLabels['no_locks_text']?>
   	  </td>
      </tr>
      <?
   }

   ?>
   </table>
   <?
}
function displayScheduledTasksScreen()
{
   global $editor_session, $system_config, $OSCAILT_SCRIPT, $redirectList, $textLabels;

   $scheduleTaskList = new SchTaskList();
   $scheduleTaskList->load();

   // Check for any deletes pending
   if (isset($_REQUEST['subpage_r_d']) && $_REQUEST['subpage_r_d'] == 'true')
   {
       $recs_to_delete = false;
       $totalCount = count($scheduleTaskList->recent_schTasks);
       for($idel=0; $idel < $totalCount;$idel++)
       {
           $reminder = $scheduleTaskList->recent_schTasks[$idel];
           $uri_tag = "remind_del_" . ($idel+1);
           if(isset($_POST[$uri_tag]) && ($_POST[$uri_tag] == 'true' || $_POST[$uri_tag] == 'on'))
           {
              $recs_to_delete = true;
              $scheduleTaskList->recent_schTasks[$idel]->save_it = false;
           }
       }
       if ($recs_to_delete == true) {
           $scheduleTaskList->save();
           $scheduleTaskList->load(true);
       }
   }


   // Get the unique list of users (editors)
   $totalCount = count($scheduleTaskList->recent_schTasks);
   $timeMsg = strftime($system_config->default_strftime_format, time() + $system_config->timezone_offset);
   
   ?>
   <table align=center width=100%>
   <tr class=admin>
      <th class=admin colspan=7>&nbsp;<?=$timeMsg?> &nbsp;</th>
   </tr>
   <tr class=admin>
      <th class=admin colspan=7>&nbsp;<?=$textLabels['total_schtasks_text']?>: <?=$totalCount?>&nbsp;</th>
   </tr>
   <tr class=admin>
      <th class=admin>&nbsp;#&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['st_scheduled_for_text']?>&nbsp;</th> 
      <th class=admin>&nbsp;<?=$textLabels['st_type_text']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['st_object_text']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['st_action_text']?>&nbsp;</th> 
      <th class=admin>&nbsp;<?=$textLabels['st_sch_task_msg_text']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['st_delete_text']?>&nbsp;</th>
      <FORM name="remindermsg_del" action="<?=$OSCAILT_SCRIPT?>" method="POST">
      <input type='hidden' name='subpage_r' value='true'> 
      <input type='hidden' name='subpage_r_d' value='true'> 
   </tr>
   <?

   $http_mode = isset($_SERVER['HTTPS']) ? "https" : "http";

   // Get any article redirector.
   $article_primary = "article";
   $redirectList->load();
   foreach($redirectList->redirects as $redirect)
   {
       if ($redirect->type  == "ArticleRedirector") {
           $article_primary = $redirect->getPrimaryRedirect();
	   break;
       }
   }
   

   for($i=0; $i < $totalCount;$i++)
   {
      $task = $scheduleTaskList->recent_schTasks[$i];
      // Windows doesn't support day of the month item: %e
      $t_msg = strftime("%d %a %B %Y at %H:%M:%S", ($task->trigger_time + $system_config->timezone_offset));
      // task_object_id
      $t_obj_id_lnk = '<a href="'.$http_mode.'://'.$system_config->site_url.'/'.$article_primary.'/'.$task->task_object_id.'">'.$task->task_object_id.'</a>'; 

      ?>
      <tr class=admin>
         <td class=admin><?=($i+1)?></td><td class=admin><?=$t_msg?> </td> <td class=admin><?=$task->task_object?></td>
         <td class=admin><?=$t_obj_id_lnk?> </td> <td class=admin><?=$task->task_action?></td>
         <td class=admin><?=$task->sys_message?></td>
         <td class=admin align=center><INPUT type=checkbox name='remind_del_<?=($i+1)?>'></td>
      </tr>
      <?
   }

   ?>
    <tr class=admin>
         <td class=admin colspan=6> &nbsp;</td>
         <td class=admin align="center">
         <input type='hidden' name='schedule' value='true'> 
   <?

   // If there are no reminders to delete then don't show the delete btn
   if ($totalCount > 0 )
   {
         ?> <INPUT type='submit' name='action_del' value='<?=$textLabels['st_delete_text']?> >>'> <?
   }

   ?>
      </td></FORM>
    </tr>
    <tr class=admin>
	 <td class=admin colspan=7><?=$textLabels['sched_task_info_text']?>
         </td>
    </tr>
   </table>
   <?
}

function displayReminderEntryScreen()
{
   global $editor_session, $system_config, $OSCAILT_SCRIPT, $textLabels;

   $reminderMsgList = new ReminderList();
   $reminderMsgList->load();

   // Check for any deletes pending
   if (isset($_REQUEST['subpage_r_d']) && $_REQUEST['subpage_r_d'] == 'true')
   {
       $recs_to_delete = false;
       $totalCount = count($reminderMsgList->recent_reminders);
       for($idel=0; $idel < $totalCount;$idel++)
       {
           $uri_tag = "remind_del_" . ($idel+1);
           if(isset($_POST[$uri_tag]) && ($_POST[$uri_tag] == 'true' || $_POST[$uri_tag] == 'on'))
           {
              $recs_to_delete = true;
              $reminderMsgList->recent_reminders[$idel]->save_it = false;
           }
       }
       if ($recs_to_delete == true) {
           $reminderMsgList->save();
           $reminderMsgList->load(true);
       }
   }


   if (isset($_REQUEST['remind_msg'] ))
   {
       if(isset($_POST["event_time_day"]) && isset($_POST["event_time_month"]) && isset($_POST["event_time_year"]) )
       {
            if(isset($_POST["event_time_hr"]) && isset($_POST["event_time_min"]) && ($_POST["event_time_hr"] > 0 || $_POST["event_time_min"] > 0) )
                $reminder_time = mktime($_POST["event_time_hr"],$_POST["event_time_min"],0,$_POST["event_time_month"],$_POST["event_time_day"],$_POST["event_time_year"])-$system_config->timezone_offset;
	    else
                $reminder_time = mktime(0,0,0,$_POST["event_time_month"],$_POST["event_time_day"],$_POST["event_time_year"])-$system_config->timezone_offset;

            $reminderMsgList->add($reminder_time, $_REQUEST['remind_msg']);
            $reminderMsgList->save();
       }
   }

   // Get the unique list of users (editors)
   $totalCount = count($reminderMsgList->recent_reminders);
   $timeMsg = strftime($system_config->default_strftime_format, time() + $system_config->timezone_offset);
   
   ?>
   <table align=center width=100%>
   <tr class=admin>
      <th class=admin colspan=4>&nbsp;<?=$timeMsg?> &nbsp;</th>
   </tr>
   <tr class=admin>
      <th class=admin colspan=4>&nbsp;<?=$textLabels['total_reminders_text']?>: <?=$totalCount?>&nbsp;</th>
   </tr>
   <tr class=admin>
      <th class=admin>&nbsp;#&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['r_scheduled_for_text']?>&nbsp;</th> 
      <th class=admin>&nbsp;<?=$textLabels['r_reminder_msg_text']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['r_delete_text']?>&nbsp;</th>
      <FORM name="remindermsg_del" action="<?=$OSCAILT_SCRIPT?>" method="POST">
      <input type='hidden' name='subpage_r' value='true'> 
      <input type='hidden' name='subpage_r_d' value='true'> 
   </tr>
   <?

   for($i=0; $i < $totalCount;$i++)
   {
      $reminder = $reminderMsgList->recent_reminders[$i];
      // Windows doesn't support day of the month item: %e
      $t_msg = strftime("%d %a %B %Y at %H:%M:%S", ($reminder->remind_time + $system_config->timezone_offset));

      ?>
      <tr class=admin>
         <td class=admin><?=($i+1)?></td><td class=admin><?=$t_msg?> </td> <td class=admin><?=$reminder->sys_message?></td>
         <td class=admin align=center><INPUT type=checkbox name='remind_del_<?=($i+1)?>'></td>
      </tr>
      <?
   }

   ?>
    <tr class=admin>
         <td class=admin colspan=3> &nbsp;</td>
         <td class=admin>
         <input type='hidden' name='reminder' value='true'> 
   <?

   // If there are no reminders to delete then don't show the delete btn
   if ($totalCount > 0 )
   {
         ?> <INPUT type='submit' name='action_del' value='Delete >>'></td> <?
   }

   ?>
      </FORM>
    </tr>

    <tr class=admin>
      <td class=admin align=center colspan="4"><BR>
      <FORM name="remindermsg" action="<?=$OSCAILT_SCRIPT?>" method="POST">
      <input type='hidden' name='subpage_r' value=''> 
      <input type='hidden' name='reminder' value='true'> 
      <b><?=$textLabels['reminder_label_text']?></b> &gt;&gt; <INPUT maxLength=180 size=120 type='text' name='remind_msg' value=''>
      <br> <br> <INPUT type='submit' name='action' value='<?=$textLabels['setup_reminder_btn_text']?> >>'><br><br>
      <?=$textLabels['set_reminder_label_text']?>: <?=getLocalReminderDateSelect();?> <br> 
      </FORM>
      </td>
    </tr>
   </table>
   <?

}

function displayPublicEditsScreen()
{
   global $editor_session, $system_config, $OSCAILT_SCRIPT;
   global $redirectList, $textLabels;

   if ($system_config->enable_public_editing != true) {
       ?>
       <table align=center width=100% border=0> 
       <tr class=admin>
         <th class=admin> &nbsp; </th>
       </tr>
       <tr class=admin>
         <td class=admin>
         <p><?=$textLabels['public_edits_disabled_text']?></p>
         </td>
       </tr></table>
       <?
       return;
   }
   $publicEditList = new EditEntriesList();
   $publicEditList->load();

   // Check for any deletes pending
   if (isset($_REQUEST['subpage_p_d']) && $_REQUEST['subpage_p_d'] == 'true')
   {
       $recs_to_change = false;
       $totalCount = $publicEditList->kount();
       for($idel=0; $idel < $totalCount;$idel++)
       {
           $uri_tag = "publicedit_del_" . ($idel+1);
           if(isset($_POST[$uri_tag]) && ($_POST[$uri_tag] == 'true' || $_POST[$uri_tag] == 'on'))
           {
              $recs_to_change = true;
	      // echo "delete entry for story id ".  $publicEditList->recent_edit_entries[$idel]->story_id."<BR>";
              $publicEditList->recent_edit_entries[$idel]->save_it = false;
           }
           $uri_tag = "publicpass_en_" . ($idel+1);
           if(isset($_POST[$uri_tag]) && ($_POST[$uri_tag] == 'true' || $_POST[$uri_tag] == 'on'))
           {
              $recs_to_change = true;
	      // echo "delete entry for story id ".  $publicEditList->recent_edit_entries[$idel]->story_id."<BR>";
              $publicEditList->recent_edit_entries[$idel]->password_enabled= true;
           }
	   else if(!isset($_POST[$uri_tag]) OR (isset($_POST[$uri_tag]) && ($_POST[$uri_tag] != 'true' && $_POST[$uri_tag] != 'on')))
           {
              $recs_to_change = true;
              $publicEditList->recent_edit_entries[$idel]->password_enabled= 0;
           }

       }
       if ($recs_to_change == true) {
           $publicEditList->save();
           $publicEditList->load(true);
       }
   }

   $t_last_edit_id = 0;
   if (isset($_REQUEST['edit_password'] ))
   {
       if(isset($_POST["new_password"]) && strlen($_POST["new_password"]) > 0 )
       {
            if(isset($_POST["edit_entry"])) {
                $totalCount = $publicEditList->kount();
                for($idel=0; $idel < $totalCount;$idel++)
                {
                    if ($publicEditList->recent_edit_entries[$idel]->story_id == $_POST["edit_entry"] )
                    {
                       $t_last_edit_id = $_POST["edit_entry"];

                       $publicEditList->recent_edit_entries[$idel]->password = $_POST['new_password'];
                       $publicEditList->save();
                       $publicEditList->load(true);
                       break;
                    }
                }
            }
       }
   }

   // Get the unique list of users (editors)
   $totalCount = $publicEditList->kount();
   $timeMsg = strftime($system_config->default_strftime_format, time() + $system_config->timezone_offset);
   
   ?>
   <table align=center width=100% border=0>
   <tr class=admin>
      <th class=admin colspan=10>&nbsp;<?=$timeMsg?> &nbsp;</th>
   </tr>
   <tr class=admin>
      <th class=admin colspan=10>&nbsp;<?=$textLabels['public_edits_total_text']?> <?=$totalCount?>&nbsp; <BR> 
                <?=$textLabels['public_edits_total_subtext']?> </th>
   </tr>
   <tr class=admin>
      <th class=admin>&nbsp;#&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['pe_story_id_text']?>&nbsp;</th> 
      <th class=admin>&nbsp;<?=$textLabels['pe_init_time_text']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['pe_expires_text']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['pe_edit_expires_text']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['pe_granted_text']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['pe_enable_pass_text']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['pe_password_text']?>&nbsp;</th>
      <th class=admin>&nbsp;<?=$textLabels['pe_user_session_text']?>&nbsp;</th>
      <th class=admin> <?=$textLabels['pe_delete_text']?></th>
      <FORM name="publiclist_del" action="<?=$OSCAILT_SCRIPT?>" method="POST">
      <input type='hidden' name='subpage_p' value='true'> 
      <input type='hidden' name='subpage_p_d' value='true'> 
   </tr>
   <?

   $http_mode = isset($_SERVER['HTTPS']) ? "https" : "http";

   // Get any article redirector.
   $article_primary = "article";
   $redirectList->load();

   foreach($redirectList->redirects as $redirect)
   {
       if ($redirect->type  == "ArticleRedirector") {
           $article_primary = $redirect->getPrimaryRedirect();
	   break;
       }
   }

   $t_now = time();

   for($i=0; $i < $totalCount;$i++)
   {
      $t_entry = $publicEditList->recent_edit_entries[$i];
      // Windows doesn't support day of the month item: %e
      $t_msg = strftime("%d %a %B %Y at %H:%M", ($t_entry->init_time + $system_config->timezone_offset));
      $t_expire = strftime("%d %a %B at %H:%M", ($t_entry->init_time + $publicEditList->expire_days + $system_config->timezone_offset));
      $t_edit_expire = strftime("%d %a %B at %H:%M", ($t_entry->init_time + $publicEditList->edit_hours + $system_config->timezone_offset));

      if (($t_entry->init_time + $publicEditList->expire_days + $system_config->timezone_offset) < $t_now ) 
          $t_expire = "<font color='orange'>".$t_expire."</font>";

      if (($t_entry->init_time + $publicEditList->edit_hours + $system_config->timezone_offset) < $t_now ) 
          $t_edit_expire = "<font color='orange'>".$t_edit_expire."</font>";

      $t_password = $t_entry->password;

      if ($t_last_edit_id > 0) {
          if ($t_entry->story_id == $t_last_edit_id ) $t_password = "<b>".$t_password."</b>";
      }
      $story_lnk = '<a href="'.$http_mode.'://'.$system_config->site_url.'/'.$article_primary.'/'.$t_entry->story_id.'">'.$t_entry->story_id.'</a>'; 

      $t_checked = "";
      if ($t_entry->password_enabled) $t_checked = "checked";

      ?>
      <tr class=admin>
	 <td class=admin><?=($i+1)?></td>
         <td class=admin><?=$story_lnk?></td>
         <td class=admin><?=$t_msg?></td>
         <td class=admin><?=$t_expire?></td>
         <td class=admin><?=$t_edit_expire?></td>
         <td class=admin><?=$t_entry->granted_by?></td>
	 <td class=admin align=center><INPUT type=checkbox name='publicpass_en_<?=($i+1)?>' <?=$t_checked?>></td>
         <td class=admin><?=$t_password?></td>
         <td class=admin><?=$t_entry->user_session?></td>
         <td class=admin align=center><INPUT type=checkbox name='publicedit_del_<?=($i+1)?>'></td>
      </tr>
      <?
   }

   $edit_select = '<select name="edit_entry" >';
   for($i=0; $i < $totalCount;$i++)
   {
      $t_entry = $publicEditList->recent_edit_entries[$i];
      $edit_select .= '<option value="'.$t_entry->story_id.'">'.($i+1).'</option>';
   }
   $edit_select .= '</select>';

   ?>
    <tr class=admin>
         <td class=admin colspan=9 align='center'> &nbsp;
         <INPUT type='submit' name='p_action_del' value='<?=$textLabels['pe_save_changes_btn_text']?> >>'>
         </td>
         <td class=admin>
         <input type='hidden' name='publicedits' value='true'> 
   <?

   // Allow entries to be deleted.
   if ($totalCount > 0 )
   {
         ?> <INPUT type='submit' name='p_action_del' value='<?=$textLabels['pe_delete_text']?> >>'></td> <?
   }

   ?>
      </FORM>
    </tr>

    <tr class=admin>
      <td class=admin align=left colspan="9">
      <FORM name="publicedit_list_msg" action="<?=$OSCAILT_SCRIPT?>" method="POST">
      <input type='hidden' name='subpage_r' value=''> 
      <input type='hidden' name='publicedits' value='true'> 
      <input type='submit' name='edit_password' value='<?=$textLabels['pe_edit_password_btn_text']?>'> 
      &nbsp; for # <?=$edit_select?> &nbsp;
      <input type='text' name='new_password' value=''> 
      </FORM>
      </td>
      <td class=admin align=center colspan="1"> &nbsp;</td>
    </tr>
   </table>
   <?

}


function getLocalReminderDateSelect()
{
      global $system_config, $editor_session, $pageType;

      $default_date = time()+$system_config->timezone_offset;

      $selected_min = date("i",$default_date);
      $selected_hr  = date("H",$default_date);
      $selected_day = date("j",$default_date);
      $selected_month = date("n",$default_date);
      $selected_year = date("Y",$default_date);
      $current_year = date("Y",time()+$system_config->timezone_offset);

      $str = '<select name="event_time_day"';
      $str .= ">\n";

      for ($i=1; $i<=31; $i++)
      {
         $str .= '<option';
         if($i==$selected_day) $str .= " selected";
         $str.= ' value="'.$i.'">'.$i.'</option>';
      }
      $str .= "</select>\n";
      $str .= '<select name="event_time_month" ';
      $str .= ">\n";

      // Make it day 2 in mktime, to get over any complications with timezone etc
      for ($i=1; $i<=12; $i++)
      {
         $str .= "<option ";
         if($i==$selected_month) $str .= "selected ";
         $str .= "value='$i'>". strftime("%B", mktime (0,0,0,$i,2,$selected_year))."</option>\n";
      }
      $str .= "</select>\n";

      $str .= '<select name="event_time_year">';
      $str .= "\n";

      for ($i=$current_year; $i<=$current_year+1; $i++)
      {
          $str .= "<option ";
          if($i==$selected_year) $str .= "selected ";
          $str.= ' value="'.$i.'">'.$i.'</option>';
      }
      $str .= "</select>\n";
      $str .= "&nbsp;";

      // Add in the start time: hour and minutes. Make 00:00 the default and if not changed then ignore
      $start_time = '<select name="event_time_hr" ';

      $start_time .= ">\n";
      if ($selected_hr == 0 ) $start_time .= "<option selected value='-1'>Hour</option>";
      for ($i=0; $i <= 23; $i++)
      {
            $start_time .= "<option ";
	    $ihr = $i;
            if ($selected_hr > 0 ) {
                if($i==$selected_hr) $start_time .= "selected ";
            }
	    if ($ihr < 10 ) $ihr = "0" . $ihr;
            $start_time.= ' value="'.$i.'">'.$ihr.'</option>';
      }
      $start_time .= "</select>\n";

      $start_time .= '<select name="event_time_min" ';
      $start_time .= ">\n";
      if ($selected_min == 0 ) $start_time .= "<option selected value='-1'>Mins</option>";
      for ($i=0; $i < 12; $i++)
      {
            $start_time .= "<option ";
	    $imin = $i*5;
            if ($selected_min > 0 ) {
                if($selected_min == ($i*5)) $start_time .= "selected ";
            }
	    if ($imin < 10 ) $imin = "0" . $imin;
            $start_time.= ' value="'.$imin.'">'.$imin.'</option>';
      }
      $start_time .= "</select>\n";
      $str .= $start_time;

      return $str;
}
// This function displays the entry screen for Oscailt scratch pad 
function displayScratchPadScreen()
{
    global $system_config, $editor_session, $textLabels;
    $scratch_filename = "scratchpad.txt";

    $scratch_filepath = $system_config->private_cache.$scratch_filename;
    $update_msg = "";
    if (file_exists($scratch_filepath)) {
        $file_modt = filemtime($scratch_filepath);
        $update_msg = "<BR>Last update: ".strftime("%H:%M:%S, %a %d %b", $file_modt);
    }

   ?>
   <table align=center class=admin width="65%">
   <tr class=admin>
   <th class=admin colspan=2><font size=+1><?=$textLabels['scratch_pad_title']?> </font> <?=$update_msg?></th>
   </tr>
   <tr class=admin>
      <td class=admin colspan=2> <?=$textLabels['scratch_pad_info']?>
        <br>
        <br>
        <?=$textLabels['scratch_pad_note']?> <br>
      </td>
   </tr>

   <script language="JavaScript" type="text/javascript">
    function clearText( form_name ) {

       if (form_name == "scratchText_block") {
           document.scratchpadform.scratchText_block.value="";
       } 
    } 

    function convertTextCase( form_name, convert_mode) {

       if (form_name == "scratchText_block") {
           var formPtr = document.scratchpadform.scratchText_block;
           var form_text = formPtr.value;
	   if (convert_mode == 1 ) {
               formPtr.value=form_text.toLowerCase();
	   } else {
               formPtr.value=form_text.toUpperCase();
	   }
       } 
    } 
    function searchAndReplace( form_name) {

       if (form_name == "scratchText_block") {
           var formPtr = document.scratchpadform.scratchText_block;
           var form_text = formPtr.value;
    
           var target_text = document.scratchpadform.to_be_replaced.value;
           var new_text = document.scratchpadform.to_replace.value;
           if (target_text.length == 0) {
               alert("Enter the text to be replaced in "+form_name);
               return;
           } else if (new_text.length == 0) {
               alert("Enter the replacement text for "+form_name);
               return;
           }
        
           var pattern_to_match = new RegExp(target_text,"g");
           var text_matches = form_text.match(pattern_to_match);
           if (text_matches == null ) alert("No matches found for "+pattern_to_match);
           var t_num = text_matches.length;
    
           var j = 0;
           while( j < t_num ) {
               form_text = form_text.replace(target_text, new_text);
               j++;
           }
           formPtr.value=form_text;
       } 
    }
    function removeLineBrks( form_name) {
       if (form_name == "scratchText_block") {
           var formPtr = document.scratchpadform.scratchText_block;
           var form_text = formPtr.value;
    
           var target_text = "<br />";
           var new_text = "";
        
           var pattern_to_match = new RegExp(target_text,"gi");
           var text_matches = form_text.match(pattern_to_match);
           if (text_matches == null ) alert("No matches found for "+pattern_to_match);
           var t_num = text_matches.length;
    
           var j = 0;
           while( j < t_num ) {
               form_text = form_text.replace(target_text, new_text);
               j++;
           }
           formPtr.value=form_text;
       } 
    }
    </script>
    
       <form name="scratchpadform" action="editorstatus.php" method=post>
   <?

    $scratchlist = "";
    if (file_exists($scratch_filepath)) {
        $scratchlist = implode("",file($scratch_filepath));
    }
 
    ?>
       <tr><td class=admin align=center> 
           <textarea rows=16 cols=85 name='scratchText_block'><?=$scratchlist?> </textarea>
           <br>
           <br>
           <input type='button' name='lwr_btn' value='Convert to Lowercase' onclick='javascript:convertTextCase("scratchText_block",1);'/>
           &nbsp;
           <input type='button' name='upr_btn' value='Convert to Uppercase' onclick='javascript:convertTextCase("scratchText_block",0);'/>
           &nbsp; &nbsp; &nbsp; 
           <input type='button' name='replace' value='Replace' onclick='javascript:searchAndReplace("scratchText_block");'/>

           <input type="text" name="to_be_replaced" value="" /> with 
           <input type="text" name="to_replace" value="" /><br />
           <br>
           <input type='button' name='clr_btn' value='Clear' onclick='javascript:clearText("scratchText_block");'/>
           &nbsp;
           <input type='button' name='linebrk_btn' value='Remove HTML Line Breaks' onclick='javascript:removeLineBrks("scratchText_block",0);'/>
           </td>
       </tr>
       <tr class=admin>
         <td colspan=2 align=center>
       <?
       if($editor_session->editor->allowedWriteAccessTo("viewstatus") ) {
         ?>
         <input type=submit name=scratchpad_btn value="<?=$textLabels['save_scratch_pad_btn_text']?>">
         <input type=hidden name=save_scratchpad value=true>
         <?
       }
       ?>
         <input type=hidden name=scratchpad value=true>
	 <input type=hidden name=scratchpadfile value="<?=$scratch_filename?>">
         </td>
       </tr>
    </form>
    </table>
    <?
}


function displayMeetingsEntryScreen()
{
   global $editor_session, $system_config, $OSCAILT_SCRIPT;

   $current_time = time() + $system_config->timezone_offset;
   $editorList = new EditorList();
   $editorList->reset();
   $editors = $editorList->getEditors(1);
   $meet_day = 1;
   $meet_mon = date("n",$current_time);

   // Proposed meeting ...
   $timeMsg = strftime($system_config->default_strftime_format, $current_time);
   $reminderftime = "None defined yet ";
   if (isset($_REQUEST['remind_msg'] ))
   {
       if(isset($_POST["event_time_day"]) && isset($_POST["event_time_month"]) && isset($_POST["event_time_year"]) )
       {
            $meet_day = $_POST["event_time_day"];
            $meet_mon = $_POST["event_time_month"];
            if(isset($_POST["event_time_hr"]) && isset($_POST["event_time_min"]) && ($_POST["event_time_hr"] > 0 || $_POST["event_time_min"] > 0) )
                $reminder_time = mktime($_POST["event_time_hr"],$_POST["event_time_min"],0,$_POST["event_time_month"],$_POST["event_time_day"],$_POST["event_time_year"])-$system_config->timezone_offset;
	    else
                $reminder_time = mktime(0,0,0,$_POST["event_time_month"],$_POST["event_time_day"],$_POST["event_time_year"])-$system_config->timezone_offset;

            $reminderftime = strftime($system_config->default_strftime_format, $reminder_time + $system_config->timezone_offset);
       }
   }
   $days_text = array("Sun", "Mon", "Tues", "Wed", "Thur", "Fri", "Sat");
   $language_dates = new languageDates();
   $day_limit = 31;
   if ($meet_mon == 4 OR $meet_mon == 6 OR $meet_mon == 9 OR $meet_mon == 11 ) $day_limit = 30;
   if ($meet_mon == 2 ) $day_limit = 28;
   
   ?>
   <FORM name="meetings_form" action="<?=$OSCAILT_SCRIPT?>" method="POST">
   <table align=center width=100%>
   <tr class=admin>
      <th class=admin colspan=8>&nbsp;<?=$timeMsg?> &nbsp; <br> Tick the days that you can attend a meeting and times.</th>
   </tr>
   <tr class=admin>
      <th class=admin colspan=8>&nbsp;<br>Proposed Meeting on: <?=$reminderftime?> <br>&nbsp; </th>
   </tr>
   <tr class=admin>
      <th class=admin> Editor </th>
      <?
      $day_number = $meet_day;
      for($i=0; $i < 7;$i++)
      {
	      if ($day_number > $day_limit) $day_number = 1;
	      $th_text = $language_dates->getNumber_Th_Text($day_number);
	      echo("<th class=admin> ".$days_text[$i] ." ".$day_number.$th_text. "</th>");
	      $day_number++;
      }
      ?>
   </tr>
   <?

   for($i=0; $i < count($editors);$i++)
   {
      $editor=$editors[$i];
      $form_style = "";
      if ($editor->editor_name != $editor_session->editor->editor_name) $form_style = "DISABLED";
      ?>
      <tr class=admin>
         <td class=admin>&nbsp;<?=$editor->editor_name?>&nbsp;</td>
	 <td class=admin align=center> <input type='checkbox' name='sun_mt_<?=$i?>' value='false' <?=$form_style?> > </td>
         <td class=admin align=center> <input type='checkbox' name='mon_mt_<?=$i?>' value='false' <?=$form_style?>> </td>
         <td class=admin align=center> <input type='checkbox' name='tue_mt_<?=$i?>' value='false' <?=$form_style?>> </td>
         <td class=admin align=center> <input type='checkbox' name='wed_mt_<?=$i?>' value='false' <?=$form_style?>> </td>
         <td class=admin align=center> <input type='checkbox' name='thu_mt_<?=$i?>' value='false' <?=$form_style?>> </td>
         <td class=admin align=center> <input type='checkbox' name='fri_mt_<?=$i?>' value='false' <?=$form_style?>> </td>
         <td class=admin align=center> <input type='checkbox' name='sat_mt_<?=$i?>' value='false' <?=$form_style?>> </td>
      </tr>
      <?
   }

   $post_btn_txt = "Setup a proposed meeting time &nbsp; >>";
   ?>
    <tr class=admin>
         <td class=admin colspan=8> &nbsp;
         <input type='hidden' name='meetings' value='true'> 
         </td>
    </tr>
    </FORM>
   <?

   ?>
    <tr class=admin>
      <td class=admin align=center colspan="8"><BR>
      <FORM name="meetingdate" action="<?=$OSCAILT_SCRIPT?>" method="POST">
      <input type='hidden' name='subpage_r' value=''> 
      <input type='hidden' name='meetings' value='true'> 
      <INPUT type='submit' name='action' value='<?=$post_btn_txt?>'><br><br>
      Set Proposed Meeting For: <?=getLocalReminderDateSelect();?> <br> 
      </FORM>
      </td>
    </tr>
   </table>
   </FORM>
   <?

}


function writeEditorsProfiles($relative_time)
{
    global $editor_session, $system_config, $OSCAILT_SCRIPT, $textLabels;

    $req_page = 1;
    $show_pagelinks = false;
    if(isset($_REQUEST['req_page'])) {
       $req_page = $_REQUEST['req_page'];
       if ($req_page < 1) $req_page = 1;
    }

    $editorList = new EditorList();
    $editorList->reset();

    $sort_by_logintime = 'true';
    $sort_mode_now = 'false';
    if(isset($_REQUEST['login_sort']) && $_REQUEST['login_sort'] == 'true') {
       $sort_by_logintime = 'false';
       $sort_mode_now = 'true';
       $editorList->setLoginSort();
    }

    $editors = $editorList->getEditors($req_page);
    $current_time = time();

    $totalEditors = $editorList->getEditorGrandTotal();
    $title_counts_msg = "Total of ". $totalEditors ." Editors ";
    if ($totalEditors > $editorList->getPageSize() )
    {
       $max_pages = ceil($totalEditors/$editorList->getPageSize());

       $title_counts_msg .= " - Page ". $req_page ." of " . $max_pages;
       $show_pagelinks = true;

       $next_page = $req_page+1;
       $prev_page = $req_page-1;
       // No link if on first page
       if ($prev_page < 1 ) $prev_page_link = '';
       else $prev_page_link = '<a href="'.$OSCAILT_SCRIPT . '?viewprofile=true&req_page='.$prev_page.'&login_sort='.$sort_mode_now.'">&lt;&lt; Prev</a> ';

       // No link if on last page
       if ($next_page > $max_pages ) $next_page_link = '';
       else $next_page_link = '<a href="'.$OSCAILT_SCRIPT . '?viewprofile=true&req_page='.$next_page .'&login_sort='.$sort_mode_now.'">Next &gt;&gt</a>';
    }
    $lastlogin_lnk = '<a class="editor-option" href="'.$OSCAILT_SCRIPT .'?viewprofile=true&req_page='.$req_page .'&login_sort='.$sort_by_logintime.'">'.$textLabels['ep_login_label'].'</a>';

    if ( isset($_REQUEST['relative']) && $_REQUEST['relative'] == 'true')
    {
      $time_url_mode = "false";
      $time_url_msg = "Display Timestamp";
    } else {
      $time_url_mode = "true";
      $time_url_msg = "Relative Timestamp";
    }

    ?>
    <table align=center width="90%">
    <tr class=admin>
        <th class=admin colspan=7> &nbsp; <?=$textLabels['editor_profiles_title']?> ( <?=$title_counts_msg?> ) &nbsp;</th>
    </tr>
    <?
    if ($show_pagelinks == true) {
       ?>
       <tr class=admin>
          <td class=admin align="left" colspan=2><small>&nbsp; <?=$prev_page_link?>&nbsp;</small></td>
          <td class=admin align="left" colspan=3>&nbsp;</td>
          <td class=admin align="right" colspan=2><small>&nbsp; <?=$next_page_link?>&nbsp;<small></td>
       </tr>
       <?
    }

    ?>
    <tr class=admin>
        <th class=admin>&nbsp;#&nbsp;</th>
        <th class=admin>&nbsp;<?=$textLabels['ep_name_label']?>&nbsp;</th>
        <th class=admin>&nbsp;<?=$textLabels['ep_email_label']?>&nbsp;</th>
        <th class=admin><?=$textLabels['ep_rx_story_label']?></th>
        <th class=admin><?=$textLabels['ep_rx_comment_label']?></th>
        <th class=admin>&nbsp;<?=$textLabels['ep_details_label']?>&nbsp;</th>
        <th class=admin>&nbsp;<?=$lastlogin_lnk?><br><small>(<a class='editor-option' href="<?=$OSCAILT_SCRIPT?>?viewprofile=true&relative=<?=$time_url_mode?>"><?=$time_url_msg?></a>)</small></th>
    </tr>
    <?

    for($i=0;$i<count($editors);$i++)
    {
        $i_row = $i + (($req_page -1) * $editorList->getPageSize()) + 1;

        $editor = $editors[$i];
	if ($editor->isEmailStoryOn()) $t_s_notify = "Yes";
	else $t_s_notify = "No";

	if ($editor->isEmailCommentOn()) $t_c_notify = "Yes";
	else $t_c_notify = "No";

	if ($editor->editor_lastlogin == NULL || $editor->editor_lastlogin ==0) $txt_editor_lastlogin = "";
	else {
            if ($relative_time == true ) {
                 $time_diff = ($current_time - $editor->editor_lastlogin);
	         $txt_editor_lastlogin = getTimeAgoString($time_diff);
	    } else {
	         $txt_editor_lastlogin = strftime($system_config->default_strftime_format, $editor->editor_lastlogin + $system_config->timezone_offset);
            }
        }
	//$txt_editor_lastlogin = $editor->editor_lastlogin ;

        $txt_ed_name = $editor->editor_name;
	if ($editor_session->editor->editor_id == $editor->editor_id) {
            $http_mode = isset($_SERVER['HTTPS']) ? "https" : "http";
	    $txt_ed_name = '<a href="'.$http_mode.'://'.$system_config->site_url.'/editeditors.php?editself=true">';
	    $txt_ed_name .= "<b>" . $editor->editor_name . "</b>";
	    $txt_ed_name .= "</a>";
	}

        ?>
        <tr class=admin>
            <td class=admin>&nbsp;<?=$i_row?>&nbsp;</td>
            <td class=admin>&nbsp;<?=$txt_ed_name?>&nbsp;</td>
            <td class=admin>&nbsp;<a href="mailto:<?=$editor->editor_email?>"><?=$editor->editor_email?></a>&nbsp;</td>
            <td class=admin align=center>&nbsp;<?=$t_s_notify?>&nbsp;</td>
            <td class=admin align=center>&nbsp;<?=$t_c_notify?>&nbsp;</td>
            <td class=admin>&nbsp;<?=$editor->editor_details?>&nbsp;</td>
            <td class=admin align="right">&nbsp;<?=$txt_editor_lastlogin?></td>
        </tr>
        <?
    }

    if ($show_pagelinks == true) {
       ?>
       <tr class=admin>
          <td class=admin align="left" colspan=2><small>&nbsp; <?=$prev_page_link?>&nbsp;</small></td>
          <td class=admin align="left" colspan=3>&nbsp;</td>
          <td class=admin align="right" colspan=2><small>&nbsp; <?=$next_page_link?>&nbsp;<small></td>
       </tr>
       <?
    }

    ?>
    </table>
    <p> &nbsp; </p>
    <?
}

function displayStatusSystemMsgScreen( $relative_time=true,$reverse_order=true)
{
    writeLocalAdminHeader();
    writeStateAndSystemMsgsList( $relative_time,$reverse_order);
    writeLocalAdminHeader();
}

ob_start();
$admin_table_width = "95%";

if($editor_session->isSessionOpen())
{
   if($editor_session->editor->allowedReadAccessTo("viewstatus") ) 
   {
      writeAdminHeader("viewsitelog.php?log_type=action_db", "View Site Log");
   
   
      $relative_time = true;
      $reverse_order = true;
      if ( isset($_REQUEST['relative']) && $_REQUEST['relative'] == 'false') $relative_time = false;;
      if ( isset($_REQUEST['reverse']) && $_REQUEST['reverse'] == 'false') $reverse_order = false;
   
      if (isset($_REQUEST['publicedits']) && $_REQUEST['publicedits'] == 'true') {
          writeLocalAdminHeader();
          displayPublicEditsScreen();

      } else if (isset($_REQUEST['reminder']) && $_REQUEST['reminder'] == 'true') {
          writeLocalAdminHeader();
          displayReminderEntryScreen();

      } else if (isset($_REQUEST['schedule']) && $_REQUEST['schedule'] == 'true') {
          writeLocalAdminHeader();
          displayScheduledTasksScreen();

      } else if (isset($_REQUEST['scratchpad']) && $_REQUEST['scratchpad'] == 'true') {
          writeLocalAdminHeader();
	  //if($editor_session->editor->allowedWriteAccessTo("viewstatus")) {
          if(isset($_REQUEST['save_scratchpad']) && $_REQUEST['save_scratchpad'] =="true") {
              if($editor_session->editor->allowedWriteAccessTo("viewstatus") ) {
                 if(isset($_REQUEST['scratchText_block']) && $_REQUEST['scratchText_block'] !=null) {
                    $scratch_filepath = $system_config->private_cache."scratchpad.txt";

		    $fh = fopen($scratch_filepath,"w");
		    if ($fh != null) {
                        fwrite($fh,$_REQUEST['scratchText_block']);
                        fclose($fh);
                        echo "<P class='error'>".$textLabels['scratch_pad_saved_text']."</P>";
	            }
	        }
	      }
	  }
	  //}
          displayScratchPadScreen();

      } else if (isset($_REQUEST['meetings']) && $_REQUEST['meetings'] == 'true') {
          writeLocalAdminHeader();
          displayMeetingsEntryScreen();

      } else if (isset($_REQUEST['locks']) && $_REQUEST['locks'] == 'true') {
          require_once("objects/editlock.inc");
          writeLocalAdminHeader();
          displayLockedStoriesScreen();

      } else if ( isset($_REQUEST['viewprofile']) && $_REQUEST['viewprofile'] == 'true') {
          writeLocalAdminHeader();
          writeEditorsProfiles($relative_time);

      } else {
         $editorStateMsgList = new PublishState();
         $editorStateMsgList->load();
   
         if(isset($_REQUEST['subpage']) && $_REQUEST['subpage'] == 'update_sys_message')
         {
            $editorStateMsgList->add($editor_session->editor->editor_name, "post", time(), $_REQUEST['system_msg']);
            $editorStateMsgList->save();
   
            $_REQUEST['subpage'] = '';
            displayStatusSystemMsgScreen($relative_time,$reverse_order);
         }
         else 
         {
            $editorStateMsgList->discardViewsByUsername($editor_session->editor->editor_name);
            $editorStateMsgList->add($editor_session->editor->editor_name, "view", time(), "");
            $editorStateMsgList->save();
            displayStatusSystemMsgScreen($relative_time,$reverse_order);
         }
      }
   }
   else $editor_session->writeNoReadPermissionError();
}
else $editor_session->writeNoSessionError();

include_once("adminfooter.inc");
?> 
