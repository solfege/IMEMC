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

// This should be called after the BODY is printed and before anything else
// because it will print a message to say a message is pending if there is one.
function handleSchTask($schTaskList)
{
    // Work through the list and if any scheduled tasks are pending then handle them and
    // then delete them.
    $cleanup_due = false;
    foreach ($schTaskList->recent_schTasks as $schTask) {
        if ($schTask->task_object == "feature") {
            if ($schTask->trigger_time < time() )
            {
                $cleanup_due = true;
		if ($schTask->task_object_id != null && $schTask->task_action == "unhide") {

                    require_once("objects/publishstate.inc");
                    $editorStateMsgList = new PublishState();
                    $editorStateMsgList->load();

                    $t_story = new Story();
                    $t_story->story_id = $schTask->task_object_id;
                    // Make sure it hasn't already been unhidden and merged
                    if ($t_story->load() == true) {
        	        if ($t_story->hidden != null && $t_story->hidden > 0) {
        		    // echo "unhiding ".$t_story->story_id."<BR>";
        		    $t_opt=explode('&', $schTask->task_options);
        		    $s_options = array();
        		    foreach ($t_opt as $element) {
        		        list($t_name, $t_value) = explode('=', $element);
        	       	        $s_options[$t_name] = $t_value;
        		    }
        		    if (isset($s_options['merge_feature']) && $s_options['merge_feature'] == true) 
                            {
        		        if (isset($s_options['reset_feature_date']) && $s_options['reset_feature_date'] == true) 
                                    $t_story->time_posted = time();
        				
                                $t_story->mergeTemporaryFeatureIntoOriginal();
                            }
        		    else if (isset($s_options['reset_feature_date']) && $s_options['reset_feature_date'] == true) 
                            {
                                $t_story->time_posted = time();
                                $t_story->save();
                            }
                            //$t_story->addslashes(); 
                            $t_story->unhide();
    		        }
                        $editorStateMsgList->add("system", "post", time(),"Scheduled unhide of feature id ".$schTask->task_object_id);
    		    } else {
                        $editorStateMsgList->add("system", "post", time(),"Scheduled unhide of feature id ".$schTask->task_object_id." failed. Feature possibly already merged and unhidden");
		    }
                    $editorStateMsgList->save();
		    unset($t_story);
		}
            }
	}
    } 

    if ($cleanup_due == true) {
        $schTaskList->discardOldRecords();
        $schTaskList->save();
    }
}

function checkForOscailtMsg($return_msg = false)
{
    global $editor_session, $system_config;

    // New check for pending scheduled feature unhides. Do the check every 50th hit
 
    if (rand(0,50) == 50 ) {
        require_once("objects/reminderlist.inc");
        $schTaskList = new SchTaskList();
        $schTaskList->load();
        if (count($schTaskList->recent_schTasks) > 0 ) handleSchTask($schTaskList);
    }

    // If it is an ordinary user, then they won't be logged in so return false.
    // Should add  ordinary user, then they won't be logged in so return false.

    if ($editor_session->isSessionOpen()) {

        // If this editor does not have permissions for this then return now.
        if ($editor_session->editor->allowedReadAccessTo("viewstatus") != true) {
            if($return_msg == true) return "";
            return "";
        }

        require_once("objects/reminderlist.inc");
	$reminderMsgList = new ReminderList();
        $reminderMsgList->load();
        $totalCount = count($reminderMsgList->recent_reminders);

        require_once("objects/publishstate.inc");
	$editorStateMsgList = new PublishState();
        $editorStateMsgList->load();

        $msgs_due = false;
	// Work through the list and if any reminders are pending post them to the message system and 
	// then delete them.
        for($i_list=0; $i_list < $totalCount;$i_list++)
        {
            $reminder = $reminderMsgList->recent_reminders[$i_list];
	    // Timeoffset is not really used -only in the display so we do not need it here.
            if ($reminder->remind_time < time() )
            {
                $msgs_due = true;
                $editorStateMsgList->add("system", "post", time(),"Reminder: ".$reminder->sys_message);
                $editorStateMsgList->save();
            }
        }
        if ($msgs_due == true) {
            $reminderMsgList->discardOldRecords();
            $reminderMsgList->save();
        }

 
        // Need to read the list of messages and see if any were posted since this user last viewed 
        // them or since they logged in.
 
        $totalCount = count($editorStateMsgList->recent_publishes);

        $msg_pending = false;
        $found_post = false;
        $found_view = false;
        $found_logout = false;
	// Work from the bottom up.
        for($i_list=0; $i_list < $totalCount;$i_list++)
        {
            $list_index = ($totalCount - $i_list -1);
            $rs = $editorStateMsgList->recent_publishes[$list_index];
	    // echo("checking ilist ".$i_list . " state " .$rs->user_state . " usr " . $rs->username . "<BR>");
            if (($rs->user_state == "post") && ($editor_session->editor->editor_name != $rs->username)) {
                $found_post = true;
                if ($found_view == false) {
                    $msg_pending = true;
                    break;
                }
            }
            if (($rs->user_state == "post") && ($editor_session->editor->editor_name == $rs->username) && ($found_post == false)) {
                // Break because no posts have been found except their own and they would have read their own.
                break;
            }

	    // If you find a view by this editor, then there is no msg pending for them unless it was before
	    // the last post.
            if (($rs->user_state == "view") && ($editor_session->editor->editor_name == $rs->username))
	    {
                if ($found_post == true) {
                    $msg_pending = true;
                }
                break;
            }
        }
        if ($msg_pending == true ) {
	    if($return_msg == true) 
	    {
                return '1 new Oscailt message pending. Click <a href="http://'.$system_config->site_url.'/editorstatus.php">here</a> to read';
	    }

            ?><p class="user-message">1 new Oscailt message pending. 
	       Click <a href="http://<?=$system_config->site_url?>/editorstatus.php">here</a> to read </p> 
	    <?
        }
    }
    if($return_msg == true) return "";
}
?>
