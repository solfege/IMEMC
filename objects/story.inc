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
require_once "objects/comment.inc";

require_once('objects/contentcleansing.inc');


   class Story
   {
      var $story_id = NULL;
      var $topic_id;
      var $region_id;
      var $type_id;
      var $language_id;
      var $time_posted;
      var $related_link;
      var $author_name;
      var $author_organisation;
      var $author_email;
      var $author_phone;
      var $author_address;
      var $story_title;
      var $story_subtitle;
      var $story_summary;
      var $story_content;
      var $story_summary_html;
      var $story_content_html;
      var $hidden;
      var $locked;
      var $event_time;
      var $sticky_time;
      var $time_last_comment_posted;
      var $comments_amount;
      var $comments;
      var $attachments_amount;
      var $image_amount;
      var $video_amount;
      var $audio_amount;
      var $miscellaneous_amount;
      var $attachments;
      var $attachments_loaded = false;
      var $image_attachments;
      var $video_attachments;
      var $audio_attachments;
      var $miscellaneous_attachments;
      // feature fields not used by ordinary stories
      var $story_content_extra;
      var $story_content_extra_html;
      var $hide_summary_on_feature_page;
      var $hide_summary_on_story_page;
      var $hide_attachments_on_feature_page;
      // used when a temporary feature is created to edit an existing newswire story
      var $temporary_feature_original_story_id;

      var $hiddenAttachments = null;
      var $externalImages = null;
      // Only used by the publish form
      var $youtube_id_1;
      var $youtube_id_2;
      var $youtube_id_3;
      var $youtube_id_4;
      var $youtube_id_5;
      // Same for audio
      var $audio_id_1;
      var $audio_id_2;
      var $audio_id_3;
      var $audio_id_4;
      var $audio_id_5;
      var $embed_video_attachments;
      var $embed_audio_attachments;
      var $vote;

      function Story()
      {}

      function addslashes()
      {
         $this->author_name = addslashes($this->author_name);

         $this->author_address = addslashes($this->author_address);
         $this->author_phone = addslashes($this->author_phone);
         $this->author_email = addslashes($this->author_email);
         $this->author_organisation = addslashes($this->author_organisation);
         $this->related_link = addslashes($this->related_link);
         $this->story_title = addslashes($this->story_title);
         $this->story_subtitle = addslashes($this->story_subtitle);
         $this->story_summary = addslashes($this->story_summary);
         $this->story_content = addslashes($this->story_content);
         $this->story_content_extra = addslashes($this->story_content_extra);
      }

      function stripslashes()
      {
         $this->author_name = stripslashes($this->author_name);
         $this->author_address = stripslashes($this->author_address);
         $this->author_phone = stripslashes($this->author_phone);
         $this->author_email = stripslashes($this->author_email);
         $this->author_organisation = stripslashes($this->author_organisation);
         $this->related_link = stripslashes($this->related_link);
         $this->story_title = stripslashes($this->story_title);
         $this->story_subtitle = stripslashes($this->story_subtitle);
         $this->story_summary = stripslashes($this->story_summary);
         $this->story_content = stripslashes($this->story_content);
         $this->story_content_extra = stripslashes($this->story_content_extra);
      }

      function save()
      {
         global $prefix, $dbconn;
         // we set timeposted to null as it will be set by database
         if($this->time_posted==null) $time_posted="NULL";
         else $time_posted="FROM_UNIXTIME(".$this->time_posted.")";
         // if event time or sticky time is null we set to 0 as we don't want DB to autofill them
         if($this->event_time==null) $event_time="FROM_UNIXTIME(0)";
         else $event_time="FROM_UNIXTIME(".$this->event_time.")";
         if($this->sticky_time==null) $sticky_time="FROM_UNIXTIME(0)";
         else $sticky_time="FROM_UNIXTIME(".$this->sticky_time.")";
         if($this->story_id == NULL)
         {
            checkForError(sql_query("INSERT INTO ".$prefix."_stories values ('$this->story_id', '$this->topic_id', '$this->region_id', '$this->type_id', '$this->language_id', $time_posted, '$this->related_link', '$this->author_name', '$this->author_organisation', '$this->author_email', '$this->author_phone', '$this->author_address', '$this->story_title', '$this->story_subtitle', '$this->story_summary', '$this->story_content', '$this->hidden', '$this->locked', $event_time, $sticky_time)", $dbconn, 2));
	    // Not ready yet to add the VOTE option.
            //checkForError(sql_query("INSERT INTO ".$prefix."_stories values ('$this->story_id', '$this->topic_id', '$this->region_id', '$this->type_id', '$this->language_id', $time_posted, '$this->related_link', '$this->author_name', '$this->author_organisation', '$this->author_email', '$this->author_phone', '$this->author_address', '$this->story_title', '$this->story_subtitle', '$this->story_summary', '$this->story_content', '$this->hidden', '$this->locked', $event_time, $sticky_time,'$this->vote')", $dbconn, 2));
            $this->story_id = mysql_insert_id($dbconn);
         }
         else
         {
            $this->story_id = cleanseNumericalQueryField($this->story_id);
            checkForError(sql_query("UPDATE ".$prefix."_stories SET topic_id = '$this->topic_id', region_id = '$this->region_id', type_id = '$this->type_id', language_id = '$this->language_id', time_posted = $time_posted, related_link = '$this->related_link', author_name = '$this->author_name', author_organisation = '$this->author_organisation', author_email = '$this->author_email', author_phone = '$this->author_phone', author_address = '$this->author_address', story_title = '$this->story_title', story_subtitle = '$this->story_subtitle', story_summary = '$this->story_summary', story_content = '$this->story_content', hidden = '$this->hidden', locked = '$this->locked', event_time = $event_time, sticky_time = $sticky_time WHERE story_id = $this->story_id", $dbconn, 2));
         }
      }

      // to be used by migration scripts from version 1.x sites only
      // really shouldn't be too many of them around now
      function forceInsertSave()
      {
         global $prefix, $dbconn;
         // we set timeposted to null as it will be set by database
         if($this->time_posted==null) $time_posted="NULL";
         else $time_posted="FROM_UNIXTIME(".$this->time_posted.")";
         // if event time or sticky time is null we set to 0 as we don't want DB to autofill them
         if($this->event_time==null) $event_time="FROM_UNIXTIME(0)";
         else $event_time="FROM_UNIXTIME(".$this->event_time.")";
         if($this->sticky_time==null) $sticky_time="FROM_UNIXTIME(0)";
         else $sticky_time="FROM_UNIXTIME(".$this->sticky_time.")";

	 // Not ready yet to add the VOTE option.
         // checkForError(sql_query("INSERT INTO ".$prefix."_stories values ('$this->story_id', '$this->topic_id', '$this->region_id', '$this->type_id', '$this->language_id', $time_posted, '$this->related_link', '$this->author_name', '$this->author_organisation', '$this->author_email', '$this->author_phone', '$this->author_address', '$this->story_title', '$this->story_subtitle', '$this->story_summary', '$this->story_content', '$this->hidden', '$this->locked',$event_time, $sticky_time,'$this->vote')", $dbconn, 2));
         checkForError(sql_query("INSERT INTO ".$prefix."_stories values ('$this->story_id', '$this->topic_id', '$this->region_id', '$this->type_id', '$this->language_id', $time_posted, '$this->related_link', '$this->author_name', '$this->author_organisation', '$this->author_email', '$this->author_phone', '$this->author_address', '$this->story_title', '$this->story_subtitle', '$this->story_summary', '$this->story_content', '$this->hidden', '$this->locked',$event_time, $sticky_time)", $dbconn, 2));
         $this->story_id = mysql_insert_id($dbconn);
      }

      function hide()
      {
         global $prefix, $dbconn;
         $this->hidden=1;
         $this->story_id = cleanseNumericalQueryField($this->story_id);
         checkForError(sql_query("UPDATE ".$prefix."_stories SET hidden = '$this->hidden', time_posted = time_posted, event_time = event_time, sticky_time = sticky_time WHERE story_id = $this->story_id", $dbconn, 2));
      }

      function unhide()
      {
         global $prefix, $dbconn;
         $this->hidden=false;
         $this->story_id = cleanseNumericalQueryField($this->story_id);
         checkForError(sql_query("UPDATE ".$prefix."_stories SET hidden = '$this->hidden', time_posted = time_posted, event_time = event_time, sticky_time = sticky_time WHERE story_id = $this->story_id", $dbconn, 2));
      }

      function lock()
      {
         global $prefix, $dbconn;
         $this->locked=1;
         $this->story_id = cleanseNumericalQueryField($this->story_id);
         checkForError(sql_query("UPDATE ".$prefix."_stories SET locked = '$this->locked', time_posted = time_posted, event_time = event_time, sticky_time = sticky_time WHERE story_id = $this->story_id", $dbconn, 2));
      }

      function unlock()
      {
         global $prefix, $dbconn;
         $this->locked=0;
         $this->story_id = cleanseNumericalQueryField($this->story_id);
         checkForError(sql_query("UPDATE ".$prefix."_stories SET locked = '$this->locked', time_posted = time_posted, event_time = event_time, sticky_time = sticky_time WHERE story_id = $this->story_id", $dbconn, 2));

      }

      function stick($sticky_time)
      {
         global $prefix, $dbconn;
         $this->sticky_time=$sticky_time;
         checkForError(sql_query("UPDATE ".$prefix."_stories SET time_posted = time_posted, event_time = event_time, sticky_time = FROM_UNIXTIME($sticky_time) WHERE story_id = $this->story_id", $dbconn, 2));
      }

      function unstick()
      {
         global $prefix, $dbconn;
         $this->sticky_time=0;
         checkForError(sql_query("UPDATE ".$prefix."_stories SET time_posted = time_posted, event_time = event_time, sticky_time = FROM_UNIXTIME(0) WHERE story_id = $this->story_id", $dbconn, 2));
      }

      // If the field is null, then set it to one, otherwise increment but if demote and null then just return
      function promote($vote_direction =1)
      {
         global $prefix, $dbconn, $editor_session;
         // Probably should make this configurable at some point.
         $check_voted = false;
         //checkForError(sql_query("UPDATE ".$prefix."_stories SET time_posted = time_posted, vote=IFNULL(vote,0)+1 WHERE story_id = $this->story_id", $dbconn, 2));
         // Assume if vote_value is zero, then there is no record yet in the table.
         $current_time = "FROM_UNIXTIME(".time().")";

         $result = sql_query("SELECT 1 FROM ".$prefix."_votes WHERE item_id = '$this->story_id'", $dbconn, 2);
         checkForError($result);
         if(sql_num_rows( $result ) > 0)
         {
             if ($check_voted == true) {
                 // Now check to see if this editor has voted already. Match the story and editor id.
                 // If or when public voting is introduce, then may have to match on IP.
                 $result = sql_query("SELECT 1 FROM ".$prefix."_voted WHERE vote_id = '$this->story_id' AND editor_id=".$editor_session->editor->editor_id, $dbconn, 0);
                 checkForError($result);
                 if(sql_num_rows( $result ) > 0) return false;
             }

             if ($vote_direction > 0) {
                 checkForError(sql_query("UPDATE ".$prefix."_votes SET vote_value =IFNULL(vote_value,0)+1 WHERE item_id = $this->story_id", $dbconn, 2));
             } else {
                 checkForError(sql_query("UPDATE ".$prefix."_votes SET vote_value =IFNULL(vote_value,0)-1 WHERE item_id = $this->story_id", $dbconn, 2));
             }
         } elseif ($vote_direction > 0) {
             checkForError(sql_query("INSERT ".$prefix."_votes VALUES ('$this->story_id',".$editor_session->editor->editor_id.",1,'story',$current_time)", $dbconn, 2));
             if ($check_voted == true) {
                 // $t_ip = "127.0.0.1";
                 $t_ip = $_SERVER['REMOTE_ADDR'];
                 checkForError(sql_query("INSERT ".$prefix."_voted VALUES ('$this->story_id',".$editor_session->editor->editor_id.",'$t_ip',$current_time)", $dbconn, 2));
             }
         }
	 return true;
      }

      function load()
      {
        if($this->story_id != NULL)
        {
           $this->story_id = cleanseNumericalQueryField($this->story_id);
           global $prefix, $dbconn;
           // Not ready yet for the VOTE option
           // $result = sql_query("SELECT topic_id, region_id, type_id, language_id, UNIX_TIMESTAMP(time_posted), related_link, author_name, author_organisation, author_email, author_phone, author_address, story_title, story_subtitle, story_summary, story_content, hidden, locked, UNIX_TIMESTAMP(event_time), UNIX_TIMESTAMP(sticky_time), vote from ".$prefix."_stories where story_id=$this->story_id", $dbconn, 2);
           $result = sql_query("SELECT topic_id, region_id, type_id, language_id, UNIX_TIMESTAMP(time_posted), related_link, author_name, author_organisation, author_email, author_phone, author_address, story_title, story_subtitle, story_summary, story_content, hidden, locked, UNIX_TIMESTAMP(event_time), UNIX_TIMESTAMP(sticky_time) from ".$prefix."_stories where story_id=$this->story_id", $dbconn, 2);
           checkForError($result);
           if(sql_num_rows( $result ) > 0)
           {
              // Not ready yet for the VOTE option
              // list($this->topic_id, $this->region_id, $this->type_id, $this->language_id, $this->time_posted, $this->related_link, $this->author_name, $this->author_organisation, $this->author_email, $this->author_phone, $this->author_address, $this->story_title, $this->story_subtitle, $this->story_summary, $this->story_content, $this->hidden, $this->locked, $this->event_time, $this->sticky_time, $this->vote) = sql_fetch_row($result, $dbconn);
              list($this->topic_id, $this->region_id, $this->type_id, $this->language_id, $this->time_posted, $this->related_link, $this->author_name, $this->author_organisation, $this->author_email, $this->author_phone, $this->author_address, $this->story_title, $this->story_subtitle, $this->story_summary, $this->story_content, $this->hidden, $this->locked, $this->event_time, $this->sticky_time) = sql_fetch_row($result, $dbconn);
              $this->story_summary_html = "on";
              $this->story_content_html = "on";
              $this->story_content_extra_html = "on";
              return true;
            }
            else
            {
               reportError("No Story with ID $this->story_id exists");
            }
         }
         else
         {
            reportError("The story id is null");
         }
         return false;
      }

      function delete()
      {
         $this->story_id = cleanseNumericalQueryField($this->story_id);
         if($this->story_id != NULL)
         {
            global $prefix, $dbconn;
            $result = sql_query("DELETE FROM ".$prefix."_stories where story_id=$this->story_id", $dbconn, 2);
            checkForError($result);
            global $editor_session;
            if($editor_session->isSessionOpen()) $editor_session->clipboard->unclip($this);
         }
         else
         {
            reportError("The story id is null");
         }
      }

      function getTopic()
      {
         global $topicList;
         return $topicList->getTopicByID($this->topic_id);
      }

      function setTopic($topic)
      {
         $this->topic_id = $topic->topic_id;
      }

      function getRegion()
      {
        global $regionList;
        return $regionList->getRegionByID($this->region_id);
      }

      function setRegion($region)
      {
         $this->region_id = $region->region_id;
      }

      function getType()
      {
         global $typeList;
         return $typeList->getTypeByID($this->type_id);
      }

      function setType($type)
      {
         $this->type_id = $type->type_id;
      }

      function getLanguage()
      {
         global $languageList;
         return $languageList->getLanguageByID($this->language_id);
      }

      function setLanguage($type)
      {
         $this->language_id = $type->language_id;
      }

      function isExcluded()
      {
         $type=$this->getType();
         $topic=$this->getTopic();
         $region=$this->getRegion();
         $language=$this->getLanguage();
         if($type->excluded==true) return true;
         else if($topic->excluded==true) return true;
         else if($region->excluded==true) return true;
         else if($language->excluded==true) return true;
         else return false;
      }

      function getComments()
      {
         $this->loadComments();
         $this->comments_amount = count($this->comments);
         return $this->comments;
      }

      function getNumberOfComments()
      {
         return $this->comments_amount;
      }

      function getNumberHiddenComments()
      {
         // We are making the assumption that the comments have to be already loaded. Porbably should save
	 // the value too.
         $number_hidden= 0;
         for($i=0;$i<count($this->comments);$i++)
         {
            if ($this->comments[$i]->hidden) $number_hidden++;
         }

         return $number_hidden;
      }

      function deleteComments()
      {
         $this->loadComments();
         for($i=0;$i<count($this->comments);$i++)
         {
            $this->comments[$i]->deleteAttachments();
            $this->comments[$i]->delete();
         }
      }

      function loadComments()
      {
         $this->story_id = cleanseNumericalQueryField($this->story_id);
         if($this->comments==null)
         {
            global $prefix, $dbconn, $editor_session, $system_config;
            if($editor_session->isSessionOpen())
            {
               $result = sql_query("SELECT comment_id, story_id, UNIX_TIMESTAMP(time_posted), related_link, author_name, author_organisation, author_email, author_phone, author_address, comment_title, comment_content, hidden FROM ".$prefix."_comments WHERE story_id=$this->story_id ORDER BY time_posted ASC", $dbconn, 2);
            }
            else 
	    {
                // If there is a comment publish delay (in mins), then do a separate query.
                if($system_config->publish_comment_delay > 0) {
                    // $allowed_time = date("YmdHi", time())."00";
                    $allowed_time = date("YmdHi", (time()-60*$system_config->publish_comment_delay)) ."00";
		    $result = sql_query("SELECT comment_id, story_id, UNIX_TIMESTAMP(time_posted), related_link, author_name, author_organisation, author_email, author_phone, author_address, comment_title, comment_content, hidden FROM ".$prefix."_comments WHERE story_id=$this->story_id AND hidden='' AND time_posted < ". $allowed_time ." ORDER BY time_posted ASC", $dbconn, 0);
                }
	        else
		    $result = sql_query("SELECT comment_id, story_id, UNIX_TIMESTAMP(time_posted), related_link, author_name, author_organisation, author_email, author_phone, author_address, comment_title, comment_content, hidden FROM ".$prefix."_comments WHERE story_id=$this->story_id AND hidden='' ORDER BY time_posted ASC", $dbconn, 2);
            }
            checkForError($result);
            $this->comments = array();
            if(sql_num_rows( $result ) > 0)
            {
               for ($i=0; $i<sql_num_rows( $result ); $i++)
               {
                  $comment = new Comment();
                  list($comment->comment_id, $comment->story_id, $comment->time_posted, $comment->related_link, $comment->author_name, $comment->author_organisation, $comment->author_email, $comment->author_phone, $comment->author_address, $comment->comment_title, $comment->comment_content, $comment->hidden) = sql_fetch_row($result, $dbconn);
                  array_push($this->comments,$comment);
               }
	       # Now see if these comments have any attachments...
               $this->countCommentAttachments();
            }
         }
      }

      # The aim here is to find out if the comments to the stories have any attachments.
      # Most of the time they want and in that case we can save ourselves a lot of queries.
      # All of this is based on the assumption that vast majority of comments have no attachments.
      function countCommentAttachments()
      {
         $this->story_id = cleanseNumericalQueryField($this->story_id);
         global $prefix, $dbconn, $editor_session;
         if($editor_session->isSessionOpen())
         {
             $result = sql_query("SELECT comment_id FROM ".$prefix."_attachments WHERE story_id=$this->story_id AND comment_id > 0 ", $dbconn, 2);
         }
         else $result = sql_query("SELECT comment_id FROM ".$prefix."_attachments WHERE story_id=$this->story_id AND comment_id > 0 AND hidden='' ", $dbconn, 2);
         checkForError($result);

	 # If there were no comments now update the status of all comments internally.
	 $attachment_count=0;

         # Next change would be to mark those comments that have no attachments and leave
	 # the rest unmarked to be loaded as normal.
	 # So would need to trawl through result and match into comment id
	 $attachment_count=sql_num_rows( $result );
	 // echo("Number of rows returned: $attachment_count <BR>");
         // if(sql_num_rows( $result ) == 0)
         if($attachment_count == 0)
         {
            for ($i=0;$i<count($this->comments);$i++)
            {
                $this->comments[$i]->setNoAttachments();
            }
         }
	 else
	 {
	    # Create an array with the keys as the comment ids for use below and another
	    # element to indicate if has or hasn't attachments because we are utlimately
	    # selecting those we did NOT find.
	    $cmt_ids = array();
            for ($i=0;$i<count($this->comments);$i++)
            {
               $cmt_ids[$this->comments[$i]->comment_id] = array(0 => $i, 0);
            }
            for ($i=0; $i< $attachment_count; $i++)
            {
               list($comment_id_with_attachment) = sql_fetch_row($result, $dbconn);
	       # Indicate it has attachments
	       $cmt_ids[$comment_id_with_attachment] [1] = 1;
            }
	    # Now going back over the array we can easily find those that were not returned
	    # and it is okay to call the setNoAttachments for.
            foreach ($cmt_ids as $key => $value) 
            {
               if ($cmt_ids[$key] [1] == 0 )
               {
		  $comment_index = $cmt_ids[$key] [0];
                  $this->comments[$comment_index]->setNoAttachments();
               }
            }
         }
      }

      function getAttachments()
      {
         $this->loadAttachments();
         return $this->attachments;
      }

      function deleteAttachments()
      {
         $this->loadAttachments();
         for($i=0;$i<count($this->attachments);$i++)
         {
            $this->attachments[$i]->deleteFileIfSingleReference();
            $this->attachments[$i]->delete();
         }
      }

      function getImageAttachments()
      {
         $this->loadAttachments();
         return $this->image_attachments;
      }

      function getVideoAttachments()
      {
         $this->loadAttachments();
         return $this->video_attachments;
      }

      function getAudioAttachments()
      {
         $this->loadAttachments();
         return $this->audio_attachments;
      }

      function getEmbeddedVideoAttachments()
      {
         $this->loadAttachments();
         return $this->embed_video_attachments;
      }

      function getEmbeddedAudioAttachments()
      {
         $this->loadAttachments();
         return $this->embed_audio_attachments;
      }


      function getMiscellaneousAttachments()
      {
         $this->loadAttachments();
         return $this->miscellaneous_attachments;
      }

      function getIcons()
      {
         $icons=array();
         if($this->attachments_amount==0) array_push($icons,"texticon.gif");
         else
         {
            if($this->image_amount>0) array_push($icons,"imageicon.gif");
            if($this->video_amount>0) array_push($icons,"videoicon.gif");
            if($this->audio_amount>0) array_push($icons,"audioicon.gif");
            if($this->miscellaneous_amount>0)
            {
               foreach($this->getMiscellaneousAttachments() as $attachment)
               {
                  $icon=$attachment->getIcon();
                  if(!in_array ($icon, $icons)) array_push($icons,$attachment->getIcon());
               }
            }
         }
         return $icons;
      }

      function getIconTag($type)
      {
         global $graphics_store;
         if($type == "comment")
         {
            return "<IMG alt=\"text\"  class='icon' src=\"".$graphics_store."texticon.gif\" border=0>";
         }
         elseif($type == "image")
         {
            return "<IMG alt=\"image\"  class='icon' src=\"".$graphics_store."imageicon.gif\" border=0>";
         }
         elseif($type == "video")
         {
            return "<IMG alt=\"video\" class='icon' src=\"".$graphics_store."videoicon.gif\" border=0>";
         }
         elseif($type == "audio")
         {
            return "<IMG alt=\"audio\" class='icon' src=\"".$graphics_store."audioicon.gif\" border=0>";
         }
         /*else
         {
            $icons=array();

            if($this->miscellaneous_amount>0)
            {
               foreach($this->getMiscellaneousAttachments() as $attachment)
               {
                  $icon=$attachment->getIconTag();

                  if(!in_array ($icon, $icons)) array_push($icons,$attachment->getIconTag());

               }
            }
            return implode("", $icons);

         }*/
      }

      function getIconTags()
      {
         global $graphics_store;
         $icons=array();
         if($this->attachments_amount==0)
            array_push($icons,"<IMG  class='icon' alt=\"text\" src=\"".$graphics_store."texticon.gif\" border=0>");
         else
         {
            if($this->image_amount>0) array_push($icons,"<IMG class='icon' alt=\"image\" src=\"".$graphics_store."imageicon.gif\" border=0>");
            if($this->video_amount>0) array_push($icons,"<IMG class='icon' alt=\"video\" src=\"".$graphics_store."videoicon.gif\" border=0>");
            if($this->audio_amount>0) array_push($icons,"<IMG class='icon' alt=\"audio\" src=\"".$graphics_store."audioicon.gif\" border=0>");
            if($this->miscellaneous_amount>0)
            {
               $miscs = $this->getMiscellaneousAttachments();
               if(is_array($miscs))
               {
                  foreach($miscs as $attachment)
                  {
                     $icon=$attachment->getIconTag();
                     if(!in_array ($icon, $icons)) array_push($icons,$attachment->getIconTag());
                  }
               }
            }
         }
         return $icons;
      }

      function loadAttachments()
      {
         $this->story_id = cleanseNumericalQueryField($this->story_id);
         if($this->attachments_loaded===false)
         {
            global $prefix, $dbconn, $editor_session;
            if($editor_session->isSessionOpen())
            {
               //$result = sql_query("SELECT attachment_id, story_id, comment_id, UNIX_TIMESTAMP(time_posted), attachment_file, description, image, video, audio, hidden, featurized FROM ".$prefix."_attachments WHERE story_id=$this->story_id AND comment_id=0 ORDER BY attachment_id ASC", $dbconn, 2);
               $result = sql_query("SELECT attachment_id, story_id, comment_id, UNIX_TIMESTAMP(time_posted), attachment_file, description, image, video, audio, hidden, featurized FROM ".$prefix."_attachments WHERE story_id=$this->story_id AND comment_id=0 ORDER BY COALESCE(image,audio,video,0) ASC", $dbconn, 2);
            }
            else
            {
               $result = sql_query("SELECT attachment_id, story_id, comment_id, UNIX_TIMESTAMP(time_posted), attachment_file, description, image, video, audio, hidden, featurized FROM ".$prefix."_attachments WHERE story_id=$this->story_id AND comment_id=0 AND hidden='' ORDER BY COALESCE(image,audio,video,0) ASC", $dbconn, 2);
            }
            checkForError($result);
            $this->attachments = array();
            $this->image_attachments = array();
            $this->video_attachments = array();
            $this->audio_attachments = array();
            $this->embed_video_attachments = array();
            $this->embed_audio_attachments = array();
            $this->miscellaneous_attachments = array();
            if(sql_num_rows( $result ) > 0)
            {
               for ($i=0; $i<sql_num_rows( $result ); $i++)
               {
                  $attachment = new Attachment();
                  list($attachment->attachment_id, $attachment->story_id, $attachment->comment_id, $attachment->time_posted,$attachment->attachment_file, $attachment->description, $attachment->image, $attachment->video, $attachment->audio, $attachment->hidden, $attachment->featurized) = sql_fetch_row($result, $dbconn);

                  array_push($this->attachments,$attachment);
                  if($attachment->isImage()) array_push($this->image_attachments,$attachment);
                  else if($attachment->isVideo()) array_push($this->video_attachments,$attachment);
                  else if($attachment->isAudio()) array_push($this->audio_attachments,$attachment);
                  else if($attachment->isEmbeddedVideo()) array_push($this->embed_video_attachments,$attachment);
                  else if($attachment->isEmbeddedAudio()) array_push($this->embed_audio_attachments,$attachment);
                  else array_push($this->miscellaneous_attachments,$attachment);
               }
            }
         }
         $this->attachments_loaded = true;
      }

      function listValidityProblems()
      {
         global $system_config;
         $problems = array();
         $missing_required_field_list=array();
         if($this->author_name==NULL) array_push($missing_required_field_list,"author name");
         if($this->region_id==NULL) array_push($missing_required_field_list,"region");
         if($this->topic_id==NULL) array_push($missing_required_field_list,"topic");
         if($this->type_id==NULL)  array_push($missing_required_field_list,"type");
         if($this->language_id==NULL)  array_push($missing_required_field_list,"language");
         if($this->story_title==NULL) array_push($missing_required_field_list,"story title");
         if($this->story_summary==NULL) array_push($missing_required_field_list,"story summary");
         if($this->story_content==NULL) array_push($missing_required_field_list,"story content");
         if($this->type_id==$system_config->event_type_id && $this->event_time==NULL) array_push($missing_required_field_list,"event time");
         $problems['required'] = $missing_required_field_list;
         $content_problems = array();
         $content_problems['Story Summary'] = findContentProblems($this->story_summary, ($this->story_summary_html == 'on'));
         $content_problems['Story Content'] = findContentProblems($this->story_content, ($this->story_content_html == 'on'));
         $content_problems['Story Content Extra'] = findContentProblems($this->story_content_extra, ($this->story_content_extra_html == 'on'));
         $problems['content'] = $content_problems;
         return $problems;
      }

      function format()
      {
        global $editor_session, $system_config;
         // strip html
         $this->author_name = strip_tags(trim($this->author_name));
         $this->author_address = strip_tags(trim($this->author_address));
         $this->author_phone = strip_tags(trim($this->author_phone));
         $this->author_organisation = strip_tags(trim($this->author_organisation));
         $this->author_email = strip_tags(trim($this->author_email));
         $this->related_link = strip_tags(trim($this->related_link));
         $this->story_title = strip_tags(trim($this->story_title));
         $this->story_subtitle = strip_tags(trim($this->story_subtitle));
         $this->story_summary = processRichContentField($this->story_summary, ($this->story_summary_html == 'on'));
         $this->story_content = processRichContentField($this->story_content, ($this->story_content_html == 'on'));
         $this->story_content_extra = processRichContentField($this->story_content_extra, ($this->story_content_extra_html == 'on'));
	 if ($this->youtube_id_1 != null) $this->youtube_id_1 = strip_tags(trim($this->youtube_id_1));
	 if ($this->youtube_id_2 != null) $this->youtube_id_2 = strip_tags(trim($this->youtube_id_2));
	 if ($this->youtube_id_3 != null) $this->youtube_id_3 = strip_tags(trim($this->youtube_id_3));
	 if ($this->youtube_id_4 != null) $this->youtube_id_4 = strip_tags(trim($this->youtube_id_4));
	 if ($this->youtube_id_5 != null) $this->youtube_id_5 = strip_tags(trim($this->youtube_id_5));

	 if ($this->audio_id_1 != null) $this->audio_id_1 = strip_tags(trim($this->audio_id_1));
	 if ($this->audio_id_2 != null) $this->audio_id_2 = strip_tags(trim($this->audio_id_2));
	 if ($this->audio_id_3 != null) $this->audio_id_3 = strip_tags(trim($this->audio_id_3));
	 if ($this->audio_id_4 != null) $this->audio_id_4 = strip_tags(trim($this->audio_id_4));
	 if ($this->audio_id_5 != null) $this->audio_id_5 = strip_tags(trim($this->audio_id_5));
      }

      function unformat()
      {
         if($this->story_summary!=null)
         {
            $this->story_summary = unprocessRichContentField($this->story_summary, ($this->story_summary_html == 'on'));
         }
         if($this->story_content!=null)
         {
            $this->story_content = unprocessRichContentField($this->story_content, ($this->story_content_html == 'on'));
         }
         if($this->story_content_extra!=null)
         {
            $this->story_content_extra = unprocessRichContentField($this->story_content_extra, ($this->story_content_extra_html == 'on'));
         }
      }

      function loadFormValues($is_new, $override_edit_permission = false)
      {
         global $editor_session,$system_config, $oscailt_page_setting;
         $site_id = $oscailt_page_setting['site_id'];
         if($editor_session->isSessionOpen() || $override_edit_permission == true)
         {
            $is_edit = false;
            if(isset($_REQUEST["story_id"]))
            {
               $this->story_id = cleanseNumericalQueryField($_REQUEST["story_id"]);
               if($this->story_id != null) $is_edit = true;
            }
            if($is_edit)
            {
               $this->load();
               $this->splitFeatureContent();
               if($override_edit_permission == true)  ;

               else if(($this->type_id==1 and $editor_session->editor->allowedReadAccessTo("editfeature", $site_id)==false) or
                ($this->type_id!=1 and
                 ($editor_session->editor->allowedReadAccessTo("editstory", $site_id)==false) and
                  ((strcasecmp($this->author_name, $editor_session->editor->editor_name) != 0) or
                  ($editor_session->editor->allowedReadAccessTo("editowncontent", $site_id) == false)
                 )
                )
               )
               {
                  $this->story_id = null;
                  $this->topic_id = null;
                  $this->region_id = null;
                  $this->type_id = null;
                  $this->language_id = null;
                  $this->related_link = null;
                  $this->time_posted = null;
                  $this->author_name = null;
                  $this->author_organisation = null;
                  $this->author_email = null;
                  $this->author_phone = null;
                  $this->author_address = null;
                  $this->story_title = null;
                  $this->story_subtitle = null;
                  $this->story_summary = null;
                  $this->story_content = null;
                  $this->hidden=null;
                  $this->locked=null;
                  $this->event_time = null;
                  $this->sticky_time = null;
                  $this->story_content_extra=null;
                  $this->hide_summary_on_feature_page=null;
                  $this->hide_summary_on_story_page=null;
                  $this->hide_attachments_on_feature_page=null;
               }
            }
            elseif($system_config->prepopulate_editor_details)
            {
               $this->author_organisation = "1 of ".$system_config->site_short_name." Editorial Group";
               $this->author_name = $editor_session->editor->editor_name;
               $this->author_email = $editor_session->editor->editor_email;
            }
         }
         if(isset($_REQUEST["topic_id"])) $this->topic_id = cleanseNumericalQueryField($_REQUEST["topic_id"]);
         if(isset($_REQUEST["region_id"])) $this->region_id = cleanseNumericalQueryField($_REQUEST["region_id"]);
         if(isset($_REQUEST["type_id"])) $this->type_id = cleanseNumericalQueryField($_REQUEST["type_id"]);
         if(isset($_REQUEST["language_id"])) $this->language_id = cleanseNumericalQueryField($_REQUEST["language_id"]);
         if(isset($_POST["related_link"])) $this->related_link = $_POST["related_link"];
         if(isset($_POST["time_posted"]) && strlen(trim($_POST["time_posted"]))>0)
         {
            $this->time_posted = strtotime($_REQUEST["time_posted"])-$system_config->timezone_offset;
         }
         if(isset($_REQUEST["author_name"])) $this->author_name = $_REQUEST["author_name"];
         if(isset($_POST["author_organisation"])) $this->author_organisation = $_POST["author_organisation"];
         if(isset($_POST["author_email"])) $this->author_email = $_POST["author_email"];
         if(isset($_POST["author_phone"])) $this->author_phone = $_POST["author_phone"];
         if(isset($_POST["author_address"])) $this->author_address = $_POST["author_address"];
         if(isset($_POST["story_title"])) $this->story_title = $_POST["story_title"];
         if(isset($_POST["story_subtitle"])) $this->story_subtitle = $_POST["story_subtitle"];
         if(isset($_POST["story_summary"])) $this->story_summary = $_POST["story_summary"];
         if(isset($_POST["story_content"])) $this->story_content = $_POST["story_content"];
         if(isset($_POST["story_content_extra"])) $this->story_content_extra = $_POST["story_content_extra"];
         if(isset($_POST["event_time"]) && strlen(trim($_POST["event_time"]))>0 && $this->type_id==$system_config->event_type_id)
         {
            $this->event_time = $_POST["event_time"]-$system_config->timezone_offset;
         }
         else if(isset($_POST["event_time_day"]) && isset($_POST["event_time_month"]) && isset($_POST["event_time_year"]) && $this->type_id==$system_config->event_type_id)
         {
            if(isset($_POST["event_time_hr"]) && isset($_POST["event_time_min"]) && ($_POST["event_time_hr"] > 0 || $_POST["event_time_min"] > 0) ) {
                // Bugfix. -1 caused when "Mins" is left as selected entry.
                if ($_POST["event_time_min"] == -1 ) $_POST["event_time_min"] = 0;
                $this->event_time = mktime($_POST["event_time_hr"],$_POST["event_time_min"],0,$_POST["event_time_month"],$_POST["event_time_day"],$_POST["event_time_year"])-$system_config->timezone_offset;
	 } else
                $this->event_time = mktime(0,0,0,$_POST["event_time_month"],$_POST["event_time_day"],$_POST["event_time_year"])-$system_config->timezone_offset;
         }
         if(isset($_POST["sticky_time"]) && strlen(trim($_POST["sticky_time"]))>0)
         {
            $this->sticky_time = strtotime($_POST["sticky_time"])-$system_config->timezone_offset;
         }
         if(isset($_POST["hide_summary_on_feature_page"]) and $_POST["hide_summary_on_feature_page"]=="on")
         {
            $this->hide_summary_on_feature_page = true;
         }
         elseif(!$is_new) $this->hide_summary_on_feature_page = false;
         if(isset($_POST["hide_summary_on_story_page"]) && $_POST["hide_summary_on_story_page"]=="on")         {
            $this->hide_summary_on_story_page = true;
         }
         elseif(!$is_new) $this->hide_summary_on_story_page = false;
         if(isset($_POST["hide_attachments_on_feature_page"]) && $_POST["hide_attachments_on_feature_page"]=="on")
         {
            $this->hide_attachments_on_feature_page = true;
         }
         elseif(!$is_new) $this->hide_attachments_on_feature_page = false;
         if(isset($_POST["story_content_html"])) $this->story_content_html = $_POST["story_content_html"];
         if(isset($_POST["story_summary_html"])) $this->story_summary_html = $_POST["story_summary_html"];
         if(isset($_POST["story_content_extra_html"])) $this->story_content_extra_html = $_POST["story_content_extra_html"];
	 // Only set and used in the publish form.
         if(isset($_POST["youtube_id_1"])) $this->youtube_id_1 = $_POST["youtube_id_1"];
         if(isset($_POST["youtube_id_2"])) $this->youtube_id_2 = $_POST["youtube_id_2"];
         if(isset($_POST["youtube_id_3"])) $this->youtube_id_3 = $_POST["youtube_id_3"];
         if(isset($_POST["youtube_id_4"])) $this->youtube_id_4 = $_POST["youtube_id_4"];
         if(isset($_POST["youtube_id_5"])) $this->youtube_id_5 = $_POST["youtube_id_5"];
	 // Audio
         if(isset($_POST["audio_id_1"])) $this->audio_id_1 = $_POST["audio_id_1"];
         if(isset($_POST["audio_id_2"])) $this->audio_id_2 = $_POST["audio_id_2"];
         if(isset($_POST["audio_id_3"])) $this->audio_id_3 = $_POST["audio_id_3"];
         if(isset($_POST["audio_id_4"])) $this->audio_id_4 = $_POST["audio_id_4"];
         if(isset($_POST["audio_id_5"])) $this->audio_id_5 = $_POST["audio_id_5"];
      }

      function splitFeatureContent()
      {
         $content = split("<!--OFB-->",$this->story_content);
         if(!isset($content[1]))
         {
            $this->story_context_extra = "";
         }
         else $this->story_content_extra=$content[1];
         $this->story_content=$content[0];
         if(strstr($this->story_content,"<!--OHSOFP-->"))
         {
            $this->hide_summary_on_feature_page=true;
            $this->story_content = str_replace("<!--OHSOFP-->","",$this->story_content);
         }
         if(strstr($this->story_content,"<!--OHSOSP-->"))
         {
            $this->hide_summary_on_story_page=true;
            $this->story_content = str_replace("<!--OHSOSP-->","",$this->story_content);
         }
         if(strstr($this->story_content,"<!--OHAOFP-->"))
         {
            $this->hide_attachments_on_feature_page=true;
            $this->story_content = str_replace("<!--OHAOFP-->","",$this->story_content);
         }

         if(strstr($this->story_content,"<!--OTFOSI-->"))
         {
            preg_match_all("|<!--OTFOSI-->.*?<!--OTFOSI-->|",$this->story_content,$matchbuffer);
            $this->temporary_feature_original_story_id = str_replace("<!--OTFOSI-->","",$matchbuffer[0][0]);
            $this->story_content = str_replace($matchbuffer[0][0],"",$this->story_content);
         }
      }

      function mergeFeatureContent()
      {
         if($this->story_content_extra!=null)
         {
            $this->story_content=$this->story_content."<!--OFB-->".$this->story_content_extra;
            $this->story_content_extra=null;
         }
         if($this->hide_summary_on_feature_page)
         {
            $this->story_content="<!--OHSOFP-->".$this->story_content;
         }
         if($this->hide_summary_on_story_page)
         {
            $this->story_content="<!--OHSOSP-->".$this->story_content;
         }
         if($this->hide_attachments_on_feature_page)
         {
            $this->story_content="<!--OHAOFP-->".$this->story_content;
         }
         if($this->temporary_feature_original_story_id!=null)
         {
            $this->story_content="<!--OTFOSI-->".$this->temporary_feature_original_story_id."<!--OTFOSI-->".$this->story_content;
         }
      }

      function downgradeToComment($parent_story_id)
      {
         $parent_story_id = cleanseNumericalQueryField($parent_story_id);
         $comment = new Comment();
         $comment->story_id=$parent_story_id;
         $comment->related_link = $this->related_link;
         $comment->time_posted = $this->time_posted;
         $comment->author_name = $this->author_name;
         $comment->author_organisation = $this->author_organisation;
         $comment->author_email = $this->author_email;
         $comment->author_phone = $this->author_phone;
         $comment->author_address = $this->author_address;
         $comment->comment_title = $this->story_title;
         $content = "";
         if(strlen($this->story_subtitle)>0) $content .= $this->story_subtitle."<br />\n";
         $content .= $this->story_summary;
         if(strlen($this->story_content)>0) $content = $content."<br />\n".$this->story_content;
         $comment->comment_content = $content;
         $comment->hidden = $this->hidden;
         $comment->addslashes();
         $comment->save();
         // move attachments to new comment
         $attachments=$this->getAttachments();
         for($i=0;$i<count($attachments);$i++)
         {
            $attachments[$i]->moveToComment($comment->comment_id);
         }
         //move comments to be comments of the new parent story
         $comments=$this->getComments();
         for($i=0;$i<count($comments);$i++)
         {
            $comments[$i]->moveToStory($parent_story_id);
         }
         $this->delete();
         return $comment;
      }

      function populateFeatureContentWithSkeleton()
      {
         global $pseudo_directory_offset, $graphics_store, $system_config;
         $fname = "config/featureskeleton.html";
         $contents = implode("", file($fname));
         $markers = array("%SUMMARY%", "%IMAGE%");
         $fillers = array($this->story_summary);
         $imgs = $this->getImageAttachments();
         if(count($imgs) > 0)
         {
            $a_id = $system_config->attachment_store.$imgs[0]->attachment_file;
            //$img_info = $imgs[0]->getTransformedImageURL($system_config->default_feature_image);
            //$disp = $imgs[0]->getDisplayName();
         }
         else
         {
            $a_id = "DUMMY";
            //$url = $graphics_store."feature-dummy.jpg";
            //$tool = new ImageTool();
            //$disp = "Dummy picture for feature";
            //$img_info = $tool->getTransformedImageURL($url,$system_config->default_feature_image);
         }
         //$fillers[1] = "<img $img_info[2] class='feature' src='$img_info[0]' alt='$disp' title='$disp'>"
         $fillers[1] = "OSCAILTIMAGE($a_id 300 0)";
         $this->story_content = str_replace($markers, $fillers, $contents);
      }

      function makeTemporaryFeature()
      {
         $attachments_to_copy = $this->getAttachments();
         // set type to feature
         $this->type_id = 1;
         // set time posted to now so it will go to top of front page
         $this->time_posted = time();
         // temporarily hide it to allow editor to edit it
         $this->hidden = true;
         $this->locked = false;

         $this->splitFeatureContent();
         // copy the summary and main text to the extra text area
         $this->story_content_extra="<p>\n".$this->story_content."\n<p>\n".$this->story_content_extra;
         // clear the main text to allow editor to place feature page content here
         $this->populateFeatureContentWithSkeleton();
         // setup summary so it only appears on summary page
         $this->hide_summary_on_feature_page=true;
         $this->hide_summary_on_story_page=true;
         // set any existing attachments not to show on front page
         $this->hide_attachments_on_feature_page=true;
         $this->temporary_feature_original_story_id=$this->story_id;
         $this->mergeFeatureContent();
         //blank story_id so new story is created
         $this->story_id=null;
         $this->addslashes();
         $this->save();

         // new temporary story created copy over any attachments from original story
         for($i=0;$i<count($attachments_to_copy);$i++)
         {
            $attachments_to_copy[$i]->copyToStory($this->story_id);
         }
      }

      function mergeTemporaryFeatureIntoOriginal()
      {
         $this->story_id = cleanseNumericalQueryField($this->story_id);
         global $system_config;
         $this->splitFeatureContent();
         // copy new feature backmover original
         if($this->temporary_feature_original_story_id!=null)
         {
            $attachments_to_move = $this->getAttachments();
            $comments_to_move = $this->getComments();
            $this->delete();
            $this->story_id=$this->temporary_feature_original_story_id;
            $this->temporary_feature_original_story_id=null;
            $this->mergeFeatureContent();
            $this->addslashes();
            $this->save();
            // make a note of feature image( if any) that original story contains
            if($system_config->featured_image!=null)
            {
               $featured_attachment = new Attachment();
               $featured_attachment->attachment_id=$system_config->featured_image;
               $featured_attachment->load();
               if($featured_attachment->story_id==$this->story_id)
               {
                  $featured_attachment_file=$featured_attachment->attachment_file;
               }
            }
            // delete any attachments from original story
            $this->attachments = null;
            $story_copy = new Story();
            $story_copy->story_id = $this->story_id;
            $story_copy->load();
            $existing_attachments = $story_copy->getAttachments();
            $a_files = array();
            foreach($existing_attachments as $a)
            {
            	$a_files[] = $a->attachment_file;
            }
            // move over any attachments from temporary story
            for($i=0;$i<count($attachments_to_move);$i++)
            {
               if(!in_array($attachments_to_move[$i]->attachment_file, $a_files))
               {
                  $attachments_to_move[$i]->moveToStory($this->story_id);
                  if(isset($featured_attachment_file) && $featured_attachment_file!=null && $featured_attachment_file==$attachments_to_move[$i]->attachment_file)
                  {
                     //moving attachment that was featureized in original so refeaturize it
                     $system_config->featured_image=$attachments_to_move[$i]->attachment_id;
                     $system_config->save();
                  }
               }
               else
               {
               	  $attachments_to_move[$i]->delete();
               }
            }
            // move over any comments from temporary story (although there shouldn't really be any, but hey who knows!)
            for($i=0;$i<count($comments_to_move);$i++)
            {
               $comments_to_move[$i]->moveToStory($this->story_id);
            }
         }
         else
         {
            $this->mergeFeatureContent();

         }
      }

      function scanForExternalImages()
      {
         global $pseudo_directory_offset, $system_config;
         if($this->externalImages !== null) return $this->externalImages;
         $html=$this->story_summary.$this->story_content.$this->story_content_extra;
         $attachments=$this->getImageAttachments();
         if (count($attachments) == 0) return null;
         $tags = array ("/<img[^>]+src\s*=\s*[\"']?([^\"' ]+)[\"']?[>|\s.*>]/i");
         $this->externalImages = array();
         foreach($tags as $tag)
         {
            preg_match_all($tag, $html, $matches);
            foreach($matches[1] as $match)
            {
               $found=false;
               foreach($attachments as $attachment)
               {
                  if(strpos($match,$attachment->attachment_file)!==false)
                  {
                     $found=true;
                     break;
                  }
                  else
                  {
                     //look for a cached version of the file...
                     $pathinfo = pathinfo($attachment->attachment_file);
                     $pathinfo['basename_we'] = substr($pathinfo['basename'], 0, -(strlen($pathinfo['extension']) + ($pathinfo['extension'] == '' ? 0 : 1)));
                     if(strpos($match, $system_config->image_cache) !== false and strpos($match, $system_config->attachment_store.$pathinfo['dirname']) !== false and strpos($match, $pathinfo['basename_we']) !== false)
                     {
                        $found=true;
                        break;
                     }
                  }
               }
               if(!$found) array_push($this->externalImages,$match);
            }
         }
         return $this->externalImages;
      }

      function scanForLinksToHiddenAttachments()
      {
         global $pseudo_directory_offset;
         if($this->hiddenAttachments !== null) return $this->hiddenAttachments;
         $html=$this->story_summary.$this->story_content.$this->story_content_extra;
         $attachments=$this->getAttachments();
         if (count($attachments) == 0) return null;
         $tags = array ("/<img[^>]+src\s*=\s*[\"']?([^\"' ]+)[\"']?[>|\s.*>]/i",
                   "/<a[^>]+href=[\"']?([^\"' ]+)[\"']?[>|\s.*>]/i");
         $this->hiddenAttachments = array();
         foreach($tags as $tag)
         {
            preg_match_all($tag, $html, $matches);
            foreach($matches[1] as $match)
            {
               foreach($attachments as $attachment)
               {
                  if($attachment->hidden==true && strpos($match,$attachment->attachment_file)!=false)
                  {
                     array_push($this->hiddenAttachments,$attachment);
                     break;
                  }
               }
            }
         }
         return $this->hiddenAttachments;
      }

      function correctLinkDirectoryOffset($proc_summary = true, $proc_extra = true)
      {
         global $pseudo_directory_offset, $system_config, $graphics_store;
         if($pseudo_directory_offset != "")
         {
            $tags = array ("!<img[^>]+src\s*=\s*[\"']?([^\"' ]+)[\"']?[>|\s.*>]!i",
                   "/<a[^>]+href\s*=\s*[\"']?([^\"' ]+)[\"']?[>|\s.*>]/i");
            foreach($tags as $tag)
            {
               preg_match_all($tag, $this->story_content, $matches);
               foreach($matches[1] as $match)
               {
                  $x = $match{0};
                  if(!strpos($match, "://") && ($x != '/') && ($x != '\\'))
                  {
                     $this->story_content = str_replace($match, $pseudo_directory_offset.$match, $this->story_content);
                  }
               }
               if($proc_summary)
               {
                  preg_match_all($tag, $this->story_summary, $matches);
                  foreach($matches[1] as $match)
                  {
                     $x = $match{0};
                     if(!strpos($match, "://") && ($x != '/') && ($x != '\\'))
                     {
                        $this->story_content = str_replace($match, $pseudo_directory_offset.$match, $this->story_content);
                     }
                  }
               }
               if($proc_extra)
               {
                  preg_match_all($tag, $this->story_content_extra, $matches);

                  foreach($matches[1] as $match)
                  {
                     $x = $match{0};

                     if(!strpos($match, "://") && ($x != '/') && ($x != '\\'))
                     {
                        $this->story_content_extra = str_replace($match, $pseudo_directory_offset.$match, $this->story_content_extra);
                     }
                  }
               }
            }
         }

      }


      function applyOscailtMacros()
      {
         //pre-populating the scan stuff...
         $this->story_content = substituteImageMacro($this->story_content, true);
         $this->story_content_extra = substituteImageMacro($this->story_content_extra, true);
         $this->externalImages = $this->scanForExternalImages();
         $this->hiddenAttachments = $this->scanForLinksToHiddenAttachments();
         //this must be post-scan...
         $this->story_content = substituteObjectMacro($this->story_content);
         //$this->story_content = stripOscailtMacros($this->story_content);
      }

      function toString($strip_html = true)
      {
         global $system_config;
         $topic = $this->getTopic();
         $region = $this->getRegion();
         $type = $this->getType();
         $language = $this->getLanguage();
         if($strip_html)
         {
            $this->unformat();
         }
         $buffer = "--- Story ".$this->story_id." ---"."\r\n";
         $buffer = $buffer."Title   : ".$this->story_title."\r\n";
         $buffer = $buffer."Subtitle   : ".$this->story_subtitle."\r\n";
         $buffer = $buffer."Topic   : ".$topic->topic_name."\r\n";
         $buffer = $buffer."Region   : ".$region->region_name."\r\n";
         $buffer = $buffer."Type      : ".$type->type_name."\r\n";
         if($this->type_id==$system_config->event_type_id)
         {
            $buffer = $buffer."Event Time   : ".date("l, M j Y",$this->event_time+$system_config->timezone_offset)."\r\n";
         }
         $buffer = $buffer."Language   : ".$language->language_name." (".$language->language_code.")\r\n";
         $buffer = $buffer."Author   : ".$this->author_name."\r\n";
         $buffer = $buffer."Organisation   : ".$this->author_organisation."\r\n";
         $buffer = $buffer."Email   : ".$this->author_email."\r\n";
         $buffer = $buffer."Phone   : ".$this->author_phone."\r\n";
         $buffer = $buffer."Address   : ".$this->author_address."\r\n";
         $buffer = $buffer."Related Link   : ".$this->related_link."\r\n";
         $buffer = $buffer."Time Posted   : ".date("l, M j Y, g:ia",$this->time_posted+$system_config->timezone_offset)."\r\n";
         if($this->sticky_time>0)         {            $buffer .= "Sticky Until   : ".date("l, M j Y, g:ia",$this->sticky_time+$system_config->timezone_offset)."\r\n";
         }         $buffer = $buffer."--- Summary ---"."\r\n";
         $buffer = $buffer.$this->story_summary."\r\n";
         $buffer = $buffer."--- Content ---"."\r\n";
         $buffer = $buffer.$this->story_content."\r\n";
         $attachments = $this->getAttachments();
         for($i=0;$i<count($attachments);$i++)
         {
            if($i==0) $buffer = $buffer."--- Story Attachments ---"."\r\n";
            $buffer = $buffer.$attachments[$i]->toString(false);
         }
         $comments = $this->getComments();
         for($i=0;$i<count($comments);$i++)
         {
            if($i==0) $buffer = $buffer."--- Story Comments ---"."\r\n";
            $buffer = $buffer."\r\n".$comments[$i]->toString(false);
         }
         return $buffer;
      }

      function dump($export_dir, $update_links)
      {
         global $system_config;
         if(!is_dir($export_dir))
         {
             if(!mkdir($export_dir, $system_config->default_writable_directory_permissions))
             {
                 $this->setUserMessage( array ( INDY_ERROR, "Failed to Make Directory $export_dir for export of story $this->id", "sf5"));
                 return false;
             }
         }
         $file = $export_dir."/".$this->story_id.".dump";
         $fp = fopen($file,"w");
         if (flock($fp, LOCK_EX))
         {
            if($update_links)
            {
               $this->story_content = makeLinksRelative($this->story_content);
               $this->story_summary = makeLinksRelative($this->story_summary);
            }
            fputs($fp, serialize($this));
            flock($fp, LOCK_UN);
            fclose($fp);
         }
         else
         {
            reportError("<P>failed to get lock on export dump file $file</p>");
            return false;
         }
         return true;
      }
   }
?>
