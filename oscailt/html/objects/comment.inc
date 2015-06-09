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
   class Comment
   {
      var $comment_id = NULL;
      var $story_id;
      var $time_posted;
      var $related_link;
      var $author_name;
      var $author_organisation;
      var $author_email;
      var $author_phone;
      var $author_address;
      var $comment_title;
      var $comment_content;
      var $comment_content_html;
      var $hidden;
      var $attachments;
      var $attachments_loaded = false;
      var $image_attachments;
      var $video_attachments;
      var $audio_attachments;
      var $miscellaneous_attachments;
      // Only used by the publish form
      // This really needs to become an array
      var $youtube_id_1;
      var $youtube_id_2;
      var $youtube_id_3;
      var $youtube_id_4;
      var $youtube_id_5;
      var $audio_id_1;
      var $audio_id_2;
      var $audio_id_3;
      var $audio_id_4;
      var $audio_id_5;
      var $embed_video_attachments;
      var $embed_audio_attachments;
      var $vote;


      function Comment()
      {
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

         $result = sql_query("SELECT 1 FROM ".$prefix."_votes WHERE item_id = '$this->comment_id'", $dbconn, 2);
         checkForError($result);
         if(sql_num_rows( $result ) > 0)
         {
             if ($check_voted == true) {
                 // Now check to see if this editor has voted already. Match the story and editor id.
                 // If or when public voting is introduce, then may have to match on IP.
                 $result = sql_query("SELECT 1 FROM ".$prefix."_voted WHERE vote_id = '$this->comment_id' AND editor_id=".$editor_session->editor->editor_id, $dbconn, 0);
                 checkForError($result);
                 if(sql_num_rows( $result ) > 0) return false;
             }

             if ($vote_direction > 0) {
                 checkForError(sql_query("UPDATE ".$prefix."_votes SET vote_value =IFNULL(vote_value,0)+1 WHERE item_id = $this->comment_id", $dbconn, 2));
             } else {
                 checkForError(sql_query("UPDATE ".$prefix."_votes SET vote_value =IFNULL(vote_value,0)-1 WHERE item_id = $this->comment_id", $dbconn, 2));
             }
         } elseif ($vote_direction > 0) {
             checkForError(sql_query("INSERT ".$prefix."_votes VALUES ('$this->comment_id',".$editor_session->editor->editor_id.",1,'comment',$current_time)", $dbconn, 2));
             if ($check_voted == true) {
                 // $t_ip = "127.0.0.1";
                 $t_ip = $_SERVER['REMOTE_ADDR'];
                 checkForError(sql_query("INSERT ".$prefix."_voted VALUES ('$this->comment_id',".$editor_session->editor->editor_id.",'$t_ip',$current_time)", $dbconn, 2));
             }
         }
	 return true;
      }

      function clear()
      {
         $this->comment_id = NULL;
         $this->story_id = NULL;
         $this->time_posted = NULL;
         $this->related_link = NULL;
         $this->author_name = NULL;
         $this->author_organisation = NULL;
         $this->author_email = NULL;
         $this->author_phone = NULL;
         $this->author_address = NULL;
         $this->comment_title = NULL;
         $this->comment_content = NULL;
         $this->comment_content_html = NULL;
         $this->hidden = NULL;
         $this->attachments = NULL;
         $this->attachments_loaded = false;
         $this->image_attachments = NULL;
         $this->video_attachments = NULL;
         $this->audio_attachments = NULL;
         $this->miscellaneous_attachments = NULL;

      }

      function addslashes()
      {
         $this->author_name = addslashes($this->author_name);
         $this->author_address = addslashes($this->author_address);
         $this->author_phone = addslashes($this->author_phone);
         $this->author_email = addslashes($this->author_email);
         $this->author_organisation = addslashes($this->author_organisation);
         $this->related_link = addslashes($this->related_link);
         $this->comment_title = addslashes($this->comment_title);
         $this->comment_content = addslashes($this->comment_content);
      }

      function stripslashes()
      {
         $this->author_name = stripslashes($this->author_name);
         $this->author_address = stripslashes($this->author_address);
         $this->author_phone = stripslashes($this->author_phone);
         $this->author_email = stripslashes($this->author_email);
         $this->author_organisation = stripslashes($this->author_organisation);
         $this->related_link = stripslashes($this->related_link);
         $this->comment_title = stripslashes($this->comment_title);
         $this->comment_content = stripslashes($this->comment_content);
      }

      function save()
      {
         global $prefix, $dbconn;
         if($this->comment_id == NULL)
         {
            $this->story_id = cleanseNumericalQueryField($this->story_id);
            if($this->time_posted==null) $time_posted="NULL";
            else $time_posted="FROM_UNIXTIME(".$this->time_posted.")";
            checkForError(sql_query("INSERT INTO ".$prefix."_comments values ('$this->comment_id', '$this->story_id', $time_posted, '$this->related_link', '$this->author_name', '$this->author_organisation', '$this->author_email', '$this->author_phone', '$this->author_address', '$this->comment_title', '$this->comment_content', '$this->hidden')", $dbconn, 2));
            $this->comment_id = mysql_insert_id($dbconn);
         }
         else
         {
            $this->story_id = cleanseNumericalQueryField($this->story_id);
            $this->comment_id = cleanseNumericalQueryField($this->comment_id);
            checkForError(sql_query("UPDATE ".$prefix."_comments SET story_id = '$this->story_id', time_posted = FROM_UNIXTIME($this->time_posted), related_link = '$this->related_link', author_name = '$this->author_name', author_organisation = '$this->author_organisation', author_email = '$this->author_email', author_phone = '$this->author_phone', author_address = '$this->author_address', comment_title = '$this->comment_title', comment_content = '$this->comment_content', hidden = '$this->hidden' WHERE comment_id = $this->comment_id", $dbconn, 2));
         }
      }

      function pending()
      {
         global $prefix, $dbconn;
         $this->hidden=2;
         $this->comment_id = cleanseNumericalQueryField($this->comment_id);
         checkForError(sql_query("UPDATE ".$prefix."_comments SET hidden = '$this->hidden', time_posted = FROM_UNIXTIME($this->time_posted) WHERE comment_id = $this->comment_id", $dbconn, 2));
      }
      function hide()
      {
         global $prefix, $dbconn;
         $this->hidden=1;
         $this->comment_id = cleanseNumericalQueryField($this->comment_id);
         checkForError(sql_query("UPDATE ".$prefix."_comments SET hidden = '$this->hidden', time_posted = FROM_UNIXTIME($this->time_posted) WHERE comment_id = $this->comment_id", $dbconn, 2));
      }

      function unhide()
      {
         global $prefix, $dbconn;

         $this->hidden=false;
         $this->comment_id = cleanseNumericalQueryField($this->comment_id);
         checkForError(sql_query("UPDATE ".$prefix."_comments SET hidden = '$this->hidden', time_posted = FROM_UNIXTIME($this->time_posted) WHERE comment_id = $this->comment_id", $dbconn, 2));
      }

      function loadVote()
      {
         global $prefix, $dbconn;
         $result = sql_query("SELECT VOTE_VALUE FROM ".$prefix."_votes WHERE item_id = '$this->comment_id' AND item_type='comment'", $dbconn, 2);
         checkForError($result);
         if(sql_num_rows( $result ) > 0)
         {
             list($this->vote) = sql_fetch_row($result, $dbconn);
         }
      }


      function load()
      {
         if($this->comment_id != NULL)
         {
            global $prefix, $dbconn;
            $this->comment_id = cleanseNumericalQueryField($this->comment_id);
            $result = sql_query("SELECT story_id, UNIX_TIMESTAMP(time_posted), related_link, author_name, author_organisation, author_email, author_phone, author_address, comment_title, comment_content, hidden from ".$prefix."_comments where comment_id=$this->comment_id", $dbconn, 2);
            checkForError($result);
            if(sql_num_rows( $result ) > 0)
            {
               list($this->story_id, $this->time_posted, $this->related_link, $this->author_name, $this->author_organisation, $this->author_email, $this->author_phone, $this->author_address, $this->comment_title, $this->comment_content, $this->hidden) = sql_fetch_row($result, $dbconn);
               $this->comment_content_html = 'on';
	       $this->loadVote(); 
               return true;
            }
            else
            {
               reportError("No Comment with ID $this->comment_id exists");
               return false;
            }
         }
         else
         {
            reportError("The comment id is null");
            return false;
         }
      }

      function delete()
      {
         if($this->comment_id != NULL)
         {
            global $prefix, $dbconn;
            $this->comment_id = cleanseNumericalQueryField($this->comment_id);
            $result = sql_query("DELETE FROM ".$prefix."_comments where comment_id=$this->comment_id", $dbconn, 2);
            checkForError($result);
            global $editor_session;
            if($editor_session->isSessionOpen()) $editor_session->clipboard->unclip($this);
         }
         else
         {
            reportError("The comment id is null");
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

      function setNoAttachments()
      {
	 # This is called when we know the comment has no attachments
	 # and want to avoid queries later that will return nothing anyhow.
         if($this->attachments_loaded===false)
         {
            $this->attachments = array();
            $this->image_attachments = array();
            $this->video_attachments = array();
            $this->audio_attachments = array();
            $this->miscellaneous_attachments = array();
            $this->attachments_loaded=true;
         }
      }
      function loadAttachments()
      {
         $this->comment_id = cleanseNumericalQueryField($this->comment_id);
         if($this->attachments_loaded===false)
         {
            if ($this->comment_id == null || $this->comment_id == 0) {
                logMessage("Error trying to load attachments for comment id = 0. Parent story id: ".$this->story_id,"sitelog.txt");
                return;
            }


            global $prefix, $dbconn, $editor_session;
            if($editor_session->isSessionOpen())
            {
               $result = sql_query("SELECT attachment_id, story_id, comment_id, UNIX_TIMESTAMP(time_posted), attachment_file, description, image, video, audio, hidden, featurized FROM ".$prefix."_attachments WHERE comment_id=$this->comment_id ORDER BY COALESCE(image,audio,video,0) ASC", $dbconn, 2);
            }
            else $result = sql_query("SELECT attachment_id, story_id, comment_id, UNIX_TIMESTAMP(time_posted), attachment_file, description, image, video, audio, hidden, featurized FROM ".$prefix."_attachments WHERE comment_id=$this->comment_id AND hidden='' ORDER BY COALESCE(image,audio,video,0) ASC", $dbconn, 2);
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
            $this->attachments_loaded=true;
         }
      }

      function loadFormValues()
      {
         global $editor_session,$system_config, $oscailt_page_setting;
         $site_id = $oscailt_page_setting['site_id'];
         $loaded = false;
         if($editor_session->isSessionOpen())
         {
            if(isset($_REQUEST["comment_id"]))
            {
               $this->comment_id = cleanseNumericalQueryField($_REQUEST["comment_id"]);
               $loaded = true;
            }
            elseif(isset($_REQUEST["editcomment"]))
            {
               $this->comment_id = cleanseNumericalQueryField($_REQUEST["editcomment"]);
               $loaded = true;
            }
            if($loaded)
            {
               $this->load();
               if ($editor_session->editor->allowedReadAccessTo("editcomment", $site_id) == false and (strcasecmp($this->author_name, $editor_session->editor->editor_name) != 0 or $editor_session->editor->allowedReadAccessTo("editowncontent", $site_id) != true))
               {
                  $this->clear();
                  $loaded = false;
               }
            }
            if(!$loaded && $system_config->prepopulate_editor_details)
            {
               $this->author_name = $editor_session->editor->editor_name;
               $this->author_email = $editor_session->editor->editor_email;
               $this->author_organisation = "1 of ".$system_config->site_short_name." Editorial Group";
            }
         }
         if(isset($_REQUEST["story_id"])) $this->story_id = cleanseNumericalQueryField($_REQUEST["story_id"]);
         if(isset($_REQUEST["related_link"])) $this->related_link = $_REQUEST["related_link"];
         if(isset($_REQUEST["time_posted"]) && strlen(trim($_REQUEST["time_posted"]))>0)
         {
            $this->time_posted = strtotime($_REQUEST["time_posted"])-$system_config->timezone_offset;
         }
         if(isset($_REQUEST["author_name"])) $this->author_name = $_REQUEST["author_name"];
         if(isset($_REQUEST["author_organisation"])) $this->author_organisation = $_REQUEST["author_organisation"];
         if(isset($_REQUEST["author_email"])) $this->author_email = $_REQUEST["author_email"];
         if(isset($_REQUEST["author_phone"])) $this->author_phone = $_REQUEST["author_phone"];
         if(isset($_REQUEST["author_address"])) $this->author_address = $_REQUEST["author_address"];
         if(isset($_REQUEST["comment_title"])) $this->comment_title = $_REQUEST["comment_title"];
         if(isset($_REQUEST["comment_content"])) $this->comment_content = $_REQUEST["comment_content"];
         if(isset($_REQUEST["comment_content_html"])) $this->comment_content_html = $_REQUEST["comment_content_html"];
         // Only set and used in the publish form.
         if(isset($_POST["youtube_id_1"])) $this->youtube_id_1 = $_POST["youtube_id_1"];
         if(isset($_POST["youtube_id_2"])) $this->youtube_id_2 = $_POST["youtube_id_2"];
         if(isset($_POST["youtube_id_3"])) $this->youtube_id_3 = $_POST["youtube_id_3"];
         if(isset($_POST["youtube_id_4"])) $this->youtube_id_4 = $_POST["youtube_id_4"];
         if(isset($_POST["youtube_id_5"])) $this->youtube_id_5 = $_POST["youtube_id_5"];
         // Same for audio
         if(isset($_POST["audio_id_1"])) $this->audio_id_1 = $_POST["audio_id_1"];
         if(isset($_POST["audio_id_2"])) $this->audio_id_2 = $_POST["audio_id_2"];
         if(isset($_POST["audio_id_3"])) $this->audio_id_3 = $_POST["audio_id_3"];
         if(isset($_POST["audio_id_4"])) $this->audio_id_4 = $_POST["audio_id_4"];
         if(isset($_POST["audio_id_5"])) $this->audio_id_5 = $_POST["audio_id_5"];

      }

      function listValidityProblems()
      {
         $problems = array();
         $missing_required_field_list=array();
         if($this->author_name==NULL) array_push($missing_required_field_list,"author name");
         if($this->story_id==NULL) array_push($missing_required_field_list,"story id");
         if($this->comment_title==NULL) array_push($missing_required_field_list,"title");
         if($this->comment_content==NULL) array_push($missing_required_field_list,"comments");
         $problems['required'] = $missing_required_field_list;
         $content_problems = array();
         $content_problems['Comment Contents'] = findContentProblems($this->comment_content, ($this->comment_content_html == 'on'));
         $problems['content'] = $content_problems;

         return $problems;
      }

      function format()
      {
         // strip html
         global $editor_session, $system_config;
         $this->author_name = strip_tags(trim($this->author_name));
         $this->author_address = strip_tags(trim($this->author_address));
         $this->author_phone = strip_tags(trim($this->author_phone));
         $this->author_email = strip_tags(trim($this->author_email));
         $this->related_link = strip_tags(trim($this->related_link));
         $this->comment_title = strip_tags(trim($this->comment_title));
         $this->comment_content = processRichContentField($this->comment_content, ($this->comment_content_html == 'on'));
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
         if($this->comment_content!=null)
         {
            $this->comment_content = unprocessRichContentField($this->comment_content, ($this->comment_content_html == 'on'));
         }
      }

      function getParentStory()
      {
         $story = new Story();
         $story->story_id=$this->story_id;
         $story->load();
         return $story;
      }

      function upgradeToStory()
      {
         $parentStory=$this->getParentStory();
         $story = new Story();
         $story->topic_id = $parentStory->topic_id;
         $story->region_id = $parentStory->region_id;
         $story->type_id = $parentStory->type_id;
         if($story->type_id==1) $story->type_id=2;
         $story->language_id = $parentStory->language_id;
         $story->related_link = $this->related_link;
         $story->time_posted = $this->time_posted;
         $story->author_name = $this->author_name;
         $story->author_organisation = $this->author_organisation;
         $story->author_email = $this->author_email;
         $story->author_phone = $this->author_phone;
         $story->author_address = $this->author_address;
         $story->story_title = $this->comment_title;
         if(strlen($this->comment_content)<=1000)
         {
            $story->story_summary = $this->comment_content;
         }
         else
         {
            $story->story_summary = substr($this->comment_content,0,1000);
            $story->story_content = substr($this->comment_content,1000);
         }
         $story->hidden = $this->hidden;
         $story->addslashes();
         $story->save();
         //move attachments to new story
         $attachments=$this->getAttachments();
         for($i=0;$i<count($attachments);$i++)
         {
            $attachments[$i]->moveToStory($story->story_id);
         }
         $this->delete();
         return $story;
      }

      function applyOscailtMacros()
      {
         //pre-populating the scan stuff...
         $this->comment_content = substituteImageMacro($this->comment_content, true);
         // $this->externalImages = $this->scanForExternalImages();
         // $this->hiddenAttachments = $this->scanForLinksToHiddenAttachments();
         //this must be post-scan...
         $this->comment_content = substituteObjectMacro($this->comment_content);
      }

      function moveToStory($story_id)
      {
         $attachments_to_move = $this->getAttachments();
         $this->story_id=$story_id;
         $this->addslashes();
         $this->save();
         // move any attachments from original comments
         for($i=0;$i<count($attachments_to_move);$i++)
         {
            $attachments_to_move[$i]->moveToComment($this->comment_id);
         }
      }

      function copyToStory($story_id)
      {
         $attachments_to_copy = $this->getAttachments();
         $this->comment_id=null;
         $this->story_id=$story_id;
         $this->addslashes();
         $this->save();
         // copy over any attachments from original comments
         for($i=0;$i<count($attachments_to_copy);$i++)
         {
            $attachments_to_copy[$i]->copyToComment($this->comment_id);
         }
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
         $file = $export_dir."/".$this->comment_id.".dump";
         $fp = fopen($file,"w");
         if (flock($fp, LOCK_EX))
         {
            if($update_links)
            {
               $this->comment_content = makeLinksRelative($this->comment_content);
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

      function toString($show_parent_link, $strip_html=true)
      {
         global $system_config;
         if($strip_html)
         {
            $this->unformat();
         }
         $buffer = "--- Comment ".$this->comment_id." ---"."\r\n";
	 if($show_parent_link==true) {
             $parentStory=$this->getParentStory();
             $buffer = $buffer."Parent Story : "."http://".$system_config->site_url."/newswire.php?story_id=".$this->story_id."\r\n";
             $buffer = $buffer."Parent Story Title : ".$parentStory->story_title."\r\n";
         }

         $buffer = $buffer."Comment Title   : ".$this->comment_title."\r\n";
         $buffer = $buffer."Author   : ".$this->author_name."\r\n";
         $buffer = $buffer."Organisation   : ".$this->author_organisation."\r\n";

	 if (substr($this->author_email,0,5) != "hide:") 
             $buffer = $buffer."Email   : ".$this->author_email."\r\n";

         $buffer = $buffer."Phone   : ".$this->author_phone."\r\n";
         $buffer = $buffer."Address   : ".$this->author_address."\r\n";
         $buffer = $buffer."Related Link   : ".$this->related_link."\r\n";
         $buffer = $buffer."Time Posted   : ".date("l, M j Y, g:ia",$this->time_posted+$system_config->timezone_offset)."\r\n";
         $buffer = $buffer."--- Comment Content ---"."\r\n";
         $buffer = $buffer.$this->comment_content."\r\n";
         $attachments = $this->getAttachments();
         for($i=0;$i<count($attachments);$i++)
         {
            if($i==0) $buffer = $buffer."--- Comment Attachments ---"."\r\n";
            $buffer = $buffer.$attachments[$i]->toString(false);
         }
         return $buffer;
      }
   }
?>
