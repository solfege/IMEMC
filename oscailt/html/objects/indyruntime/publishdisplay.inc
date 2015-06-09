<?
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

// Moved these up here because load_editor_wysiwyg needs to be in scope as
// markupconfig.php will test for it and for the openwysiwyg will change
// the configuration for allowed HTML tags.

$load_wysiwyg_editor=0;
// Switch on wysiwyg_editor if configured so as to give correct allowed tags settings.
if ($oscailt_basic_config['enable_wysiwyg_editor'] == "TinyMCE" ) $load_wysiwyg_editor = 1;
else if ($oscailt_basic_config['enable_wysiwyg_editor'] == "OpenWYSIWYG" ) $load_wysiwyg_editor = 2;

// OpenWYSIWYG editor does not work with Apple Safari browser
if ($load_wysiwyg_editor == 2 && strpos($_SERVER['HTTP_USER_AGENT'], "Safari") !== false ) {
    $load_wysiwyg_editor=0;
}
if ($load_wysiwyg_editor == 2 && strpos($_SERVER['HTTP_USER_AGENT'], "Chrome") !== false ) {
    $load_wysiwyg_editor=0;
}

require_once('sitefiltereddisplay.inc');
require_once('config/attachments.php');
require_once('objects/story.inc');
require_once('objects/ticket.inc');
require_once('objects/editlock.inc');
require_once('objects/attachment.inc');
require_once('objects/bannedip.inc');
require_once('objects/bannedauthor.inc');
require_once('objects/contentcleansing.inc');
require_once('objects/publishmonitor.inc');
require_once('objects/videos.inc');
require_once('objects/numericcaptcha.inc');
require_once('objects/formvalidate.inc');
require_once('objects/adminutilities.inc');
require_once('objects/publiclist.inc');


global $oscailt_basic_config, $editor_session;
$load_wysiwyg_editor=0;
// Switch off wysiwyg_editor to stop error messages when complete page is displayed.
if(!isset($_REQUEST['preview_confirmed'])) {
    if ($oscailt_basic_config['enable_wysiwyg_editor'] == "TinyMCE" ) $load_wysiwyg_editor = 1;
    else if ($oscailt_basic_config['enable_wysiwyg_editor'] == "OpenWYSIWYG" ) $load_wysiwyg_editor = 2;

    if ($load_wysiwyg_editor == 2 && strpos($_SERVER['HTTP_USER_AGENT'], "Safari") !== false ) {
        $load_wysiwyg_editor=0;
    }
    if ($load_wysiwyg_editor == 2 && strpos($_SERVER['HTTP_USER_AGENT'], "Chrome") !== false ) {
        $load_wysiwyg_editor=0;
    }
}

// This variable controls whether normal editing or editor editing javascript file gets loaded.
// 1 is for basic JS file and 2 for both JS files.
$load_js=1;
if ($editor_session->isSessionOpen()) $load_js=2;

class publishDisplay extends siteFilteredDisplay
{
   var $story;
   var $content_type;
   var $comment;
   var $checkstory;
   var $attachments;
   var $result;
   var $user_msg;
   var $new_feature;

   var $unset_t_and_c = false;
   //limits that apply to the current user
   var $max_uploads;
   var $max_uploads_filesize;
   var $max_comment;
   var $max_contents;
   var $max_summary;
   var $lock;
   var $use_captcha ;
   var $generate_captcha =false;
   var $number_embed_video;
   var $number_embed_audio;
   var $field_validation_error = false;
   var $user_on_moderation = false;
   var $hide_spam = false;
   var $allow_public_edits = false;
   var $public_password_enabled = false;

   function publishDisplay ($obj_id)
   {
      $this->siteFilteredDisplay($obj_id);
   }

   /**********************************************************************
   //Function to do the main processing of the user's request
   //inputs: none, outputs: none
   /*********************************************************************/
   function processRequest()
   {
      global $system_config,$editor_session, $oscailt_page_setting;
      $site_id = $oscailt_page_setting['site_id'];

      
      SharedMemoryUpdate("publish");

      if (strlen($system_config->spam_post_ip_list) > 0 ) {
          if ($_SERVER['REQUEST_METHOD'] == "POST") {
              //if (in_array($_SERVER['REMOTE_ADDR'], $system_config->spam_post_ip_list)) {
    	      $iper = $_SERVER['REMOTE_ADDR'];
              if (strpos($system_config->spam_post_ip_list, $iper) !== false) {
                  SharedMemoryUpdate("publish_spammer");
		  logSpammerMessage("Spammer (".$iper.") in spam_post_ip_list blocked from POST in publish form.", "spamlog.txt");
    	          return;
              }
          }
      }

      if ($_SERVER['REQUEST_METHOD'] == "POST" && (!isset($_REQUEST['preview_confirmed']) || !isset($_REQUEST['publish_button'])) ) {
          if ( (!isset($_REQUEST['file_upload_button']) && !isset($_REQUEST['number_of_files_selected']) ) && !$editor_session->isSessionOpen()) {
              $formValidationObj = new formValidator();
    	      if($formValidationObj->validateFormData() != true)
              {
                  $formValidationObj->reportInvalidation( $this->obj_id);
                  // For level = 2 we should report some of the form data
                  SharedMemoryUpdate("publish_spammer");
                  if ($system_config->security_recording_level >= 1)
                      logSpammerMessage("Publish form missing validation data. Rejecting request as spam.", "spamlog.txt");
    
        	  // Spammer detected sending bad data. 
        	  echo("Publish request cancelled. <BR>");
    	          return;
              }
          }

      }
      parent::processRequest();
      $this->integrateFilters();

      // Have to determine it here to help setup options for captcha
      $publishing_comment = false;
      if((isset($_REQUEST['section']) && $_REQUEST['section']=='addcomment') or isset($_REQUEST['editcomment']))
      {
          $publishing_comment = true;
      }

      // Setup the captcha here not in the constructor as options only get set by cache after object create

      if($this->getDisplayOption('include_numeric_captcha') != 'off') {
          if($this->getDisplayOption('include_numeric_captcha') == 'both') $this->use_captcha =true;
	  else
          {
              if($publishing_comment == true && $this->getDisplayOption('include_numeric_captcha') == 'comments') $this->use_captcha =true;
              if($publishing_comment == false && $this->getDisplayOption('include_numeric_captcha') == 'stories') $this->use_captcha =true;
          }
      }
      else $this->use_captcha = false;

      // Turn captcha off for editors
      if($editor_session->isSessionOpen()) $this->use_captcha = false;


      $edit_chk_override = false;
      if($publishing_comment == true)
      {
         if($this->getDisplayOption("who_can_comment") == 'nobody' or (!$editor_session->isSessionOpen() && $this->getDisplayOption("who_can_comment") == 'editors'))
         {
            if($this->getDisplayOption("who_can_comment") == 'editors')
            {
               $this->user_msg = $this->getDisplayOption("only_editors_may_comment");
            }
            else
            {
               $this->user_msg = $this->getDisplayOption("this_form_not_for_comments");
            }
            $this->writeFailurePage();
            return;

         }
         $this->comment = new Comment();
         $this->comment->loadFormValues($this->isNewPublication());
         // If we have just entered the form and we are editing a story then load any embedded video or audio info
         if ($this->isNewPublication() == true && isset($_REQUEST['editcomment']))
         {
             $this->comment->getEmbeddedVideoAttachments();
             $this->comment->getEmbeddedAudioAttachments();
             $this->setFormWithEmbedVideoAudio(false);
         }
         $this->processRelatedLink($this->comment);
         $this->content_type = 'comment';
      }
      else
      {
         // if($this->getDisplayOption("who_can_publish") == 'nobody' or (!$editor_session->isSessionOpen() && $this->getDisplayOption("who_can_publish") == 'editors') or (!$editor_session->isSessionOpen() && $system_config->publish_shutdown == true))
         if($this->getDisplayOption("who_can_publish") == 'nobody' or (!$editor_session->isSessionOpen() && $this->getDisplayOption("who_can_publish") == 'editors') or (!$editor_session->isSessionOpen() && $system_config->disable_open_publish == true))
         {
            if($this->getDisplayOption("who_can_publish") == 'editors')
            {
               $this->user_msg = $this->getDisplayOption("only_editors_may_publish");
            }
            else
            {
               $this->user_msg = $this->getDisplayOption("this_form_not_for_publishing");
            }
            $this->writeFailurePage();
            return;
         }
         $this->story = new Story();

         if(!$editor_session->isSessionOpen() && ($this->getDisplayOption("allow_public_edits") == 'true' && $system_config->enable_public_editing == true )) {

             $t_editList = new EditEntriesList();
             if ($t_editList->kount() > 0 || trim($this->getDisplayOption("selected_public_edit_stories")) != '') {
		     // Change so that either the ids match the story to tighten it up.
    	         if (isset($_REQUEST["story_id"]) ) {
                     $search_tag =  cleanseNumericalQueryField($_REQUEST["story_id"]);

		     // Check the buffer list and see if this story is allowed
                     if ($this->checkStoryPermitted($t_editList, $search_tag, false) == true) {
                     //if ($t_editList->canEditThisStory($search_tag)) {
                         $this->allow_public_edits = true;
                         // $this->public_password_enabled = $t_editList->use_password;
                     }
		     else if ($this->checkStoryInAllowedList($search_tag)) $this->allow_public_edits = true;

                     // If the story is in one of the lists then ...
		     //if ($this->allow_public_edits == true && $this->public_password_enabled == true) {
		     //} else if ($this->allow_public_edits == true) {
		     if ($this->allow_public_edits == true) {

		         if (isset($_REQUEST['number_of_files_selected']) && $_REQUEST['number_of_files_selected'] > 0 ) echo "# files ". $_REQUEST['number_of_files_selected'] ."<BR>";

                         if ($this->checkStoryPermitted($t_editList, $search_tag, true) == true) {
                             $_SERVER['REQUEST_METHOD']='GET';
                             $this->story->loadFormValues($this->isNewPublication(),true);
                             $edit_chk_override = true;
		         } else if ((isset($_REQUEST['preview_confirmed']) || isset($_REQUEST['file_upload_button'])) && ((isset($_REQUEST['number_of_files_selected']) && $_REQUEST['number_of_files_selected'] > 0 ) || (isset($_REQUEST['number_of_files']) && $_REQUEST['number_of_files'] > 0 ))) {

                             $this->story->loadFormValues($this->isNewPublication(),true);
                             $edit_chk_override = true;
                         } else {
                             //if ($this->isNewPublication() == true ) {
                             if ($this->isNewPublication() == true && $this->public_password_enabled == true) {
                                 $this->writePasswordRequest($_REQUEST["story_id"]);
                                 return;
                             }
                         }
                     } else {
                         $this->user_msg = "Cannot use this form to edit this article or to publish new articles.";
                         $this->writeFailurePage();
                         return;
                     }
    	         } else {
                     // $this->user_msg = "Cannot publish. No story Id set.";
                     // $this->writeFailurePage();
                     // return;
                     $this->story->loadFormValues($this->isNewPublication());
    	         }
             } else {
                 $this->story->loadFormValues($this->isNewPublication());
             }
         } else {
             $this->story->loadFormValues($this->isNewPublication());
         }

	 // If we have just entered the form and we are editing a story then load any embedded video or audio info
         if ($this->isNewPublication() == true && (isset($_REQUEST['editstory']) || isset($_REQUEST['editfeature'])) )
         {
             $this->story->getEmbeddedVideoAttachments();
             $this->story->getEmbeddedAudioAttachments();
             $this->setFormWithEmbedVideoAudio(true);
         }
         $this->processRelatedLink($this->story);
         $this->content_type = 'story';
      }
      $this->setAttachmentLimits();

      if(!$this->checkPublishFormSecurity() )
      {
         $this->writeFailurePage();
      }
      elseif($this->isCancel())
      {
         $this->writePublishPage();
      }
      elseif($this->isNewPublication())
      {
         $this->writePublishPage();
      }
      elseif(!$this->isValidPublicationRequest($edit_chk_override))
      {
         if($this->result == "no_write_permission_error" or $this->result == "no_permission_error" or $this->result == "no_session_error")
         {
            $this->writeFailurePage();
         }
         else
         {
            $this->writePublishPage();
         }
      }
      elseif(!$this->isConfirmedPublicationRequest())
      {
         if(!$this->checkPreviewSecurity())
         {
            $this->writeFailurePage();
         }
         else
         {
            $this->writePreviewPage();
         }
      }
      elseif(!$this->checkPublishSecurity())
      {
         $this->writeFailurePage();
      }
      else
      {
         if($this->executePublish())
         {
            if($system_config->publish_monitor_enabled)
            {
               $this->addPublishEventToMonitor();
            }
            $this->writeResultsPage();
         }
         else
         {
            $this->writeFailurePage();
         }

         $ipBanList = new BannedIPList();
         $ipBanList->load();
         $ipBanList->clearOldBans();

         // Do the same for banned authors.
         $authorBanList = new BannedAuthorList();
         $authorBanList->load();
         $authorBanList->clearOldBans();
      }
   }


   /**************************************************************************/
   /* This is only used in the wiki publish mode, during the initial request */ 
   /* and is used to request the password assuming it is set.                */ 
   /**************************************************************************/
   function writePasswordRequest($req_story_id)
   {
      $this->writePublishHeader();
      // $this->writeFormHeader(false);
      echo '<TABLE class="publish"><TBODY>';

      $lnk = $this->getSimpleLink();

      $formValidationData = new formValidator();
      $hiddenData = $formValidationData->getHiddenData();
      
      ?>
      <FORM name="publishform_pass" enctype="multipart/form-data" action="<?=$lnk?>" method="post">
      <INPUT type=hidden name="story_id" value="<?=$req_story_id?>">
      <?
      echo $hiddenData;
      $this->writeSectionHeader('1', 'Password', 'To edit this story you must enter the password provided for this story');
      $this->writeFormInputField('Password', '', '<INPUT type=password name=publish_password value="">', "");
      $this->writeFormInputField('', '', '<INPUT type=submit name=wik1_pub_btn value="Edit Story">', "");
      //$this->writeFormInputField($title, $prompt, $payload, $help_text);

      $this->writeFormFooter(false);
      return false;
   }

   /*********************************************************************/
   /* For public edits, check through the list of allowed story ids     */
   /*********************************************************************/
   function checkStoryPermitted($t_editList, $search_story, $chk_password)
   {
      $search_story = cleanseNumericalQueryField($search_story);

      // First check the dynamic public edit list

      if ($t_editList->kount() > 0 ) {
          if (isset($_REQUEST['PHPSESSID']) ) {
	      // If the session matches and it is not expired then thats fine 
              $t_editEntry = $t_editList->getEntryForSession($_REQUEST['PHPSESSID'], $search_story);
	      // If yes now check the password is correct.
              if ($t_editEntry != null) {
                  return true;
              } else {
                  // Otherwise if passwords allowed and it matches then thats fine
                  $t_editEntry = $t_editList->getEntryForStory($search_story);
		  if ($t_editEntry == null ) return false;
                  if ($chk_password == false) return true;

                  if ($t_editEntry->password_enabled == true) {

                      $this->public_password_enabled = $t_editEntry->password_enabled;
                      $t_editEntry = $t_editList->getEntryForStory($search_story);

                      if ($t_editEntry != null && isset($_REQUEST['publish_password']) && trim($_REQUEST['publish_password']) != "") {
			  if ($t_editEntry->password == trim($_REQUEST['publish_password'])) 
			  {
                              return true;
                          }
                      } 
                  }
              }
          }
      }
      return false;
   }

   function checkStoryInAllowedList($t_story_id)
   {
      if ($t_story_id == null) return false;
      $t_story_id = cleanseNumericalQueryField($t_story_id);


      $allowed_ids = explode(" ",$this->getDisplayOption("selected_public_edit_stories"));
      foreach ($allowed_ids as $id_pass ) {
          $t_pos = strpos($id_pass, ":");
          if ($t_pos === false ) {
              // No password set for this id just a story id.
              if ($id_pass == $t_story_id ) {
                  $this->public_password_enabled = true;
                  return true;
              }
          } else {
              // Password must be set 
              // echo "sub=".substr($id_pass,0,$t_pos)."<BR>";
              if (substr($id_pass,0, $t_pos) == $t_story_id ) return true;
          }
      }
      return false;
   }
       
   /*********************************************************************/
   /* overloads the attachment limits in system config               */
   /*********************************************************************/
   function setAttachmentLimits()
   {
      global $system_config, $editor_session;

      if($this->getDisplayOption('image_attachment_max_size') > 0 and ($this->getDisplayOption('image_attachment_max_size') * 1024) < $system_config->image_attachment_max_size)
      {
         $system_config->image_attachment_max_size = $this->getDisplayOption('image_attachment_max_size') * 1024;
      }
      // Set this to the biggest attachment limit.
      $this->max_uploads_filesize = $system_config->image_attachment_max_size;

      if($this->getDisplayOption('image_attachment_max_width') < $system_config->image_attachment_max_width and $this->getDisplayOption('image_attachment_max_width') > 0)
      {
         $system_config->image_attachment_max_width = $this->getDisplayOption('image_attachment_max_width');
      }
      if(($this->getDisplayOption('video_attachment_max_size') * 1024) < $system_config->video_attachment_max_size and $this->getDisplayOption('video_attachment_max_size') > 0)
      {
         $system_config->video_attachment_max_size = $this->getDisplayOption('video_attachment_max_size')* 1024;
      }
      if ($system_config->video_attachment_max_size > $this->max_uploads_filesize)
          $this->max_uploads_filesize = $system_config->video_attachment_max_size;

      if(($this->getDisplayOption('audio_attachment_max_size') * 1024) < $system_config->audio_attachment_max_size and $this->getDisplayOption('audio_attachment_max_size') > 0)
      {
         $system_config->audio_attachment_max_size = $this->getDisplayOption('audio_attachment_max_size')* 1024;
      }
      if ($system_config->audio_attachment_max_size > $this->max_uploads_filesize)
          $this->max_uploads_filesize = $system_config->audio_attachment_max_size;

      if(($this->getDisplayOption('miscellaneous_attachment_max_size') * 1024) < $system_config->miscellaneous_attachment_max_size and $this->getDisplayOption('miscellaneous_attachment_max_size') > 0)
      {
         $system_config->miscellaneous_attachment_max_size = $this->getDisplayOption('miscellaneous_attachment_max_size') * 1024;
      }
      if ($system_config->miscellaneous_attachment_max_size > $this->max_uploads_filesize)
          $this->max_uploads_filesize = $system_config->miscellaneous_attachment_max_size;

      if($this->getDisplayOption('allow_story_image_attachments') != 'true')
      {
         $system_config->allow_story_image_attachments = false;
      }
      if($this->getDisplayOption('allow_story_video_attachments') != 'true')
      {
         $system_config->allow_story_video_attachments = false;
      }
      if($this->getDisplayOption('allow_story_audio_attachments') != 'true')
      {
         $system_config->allow_story_audio_attachments = false;
      }
      if($this->getDisplayOption('allow_story_miscellaneous_attachments') != 'true')
      {
         $system_config->allow_story_miscellaneous_attachments = false;
      }
      if($this->getDisplayOption('allow_comment_image_attachments') != 'true')
      {
         $system_config->allow_comment_image_attachments = false;
      }
      if($this->getDisplayOption('allow_comment_video_attachments') != 'true')
      {
         $system_config->allow_comment_video_attachments = false;
      }
      if($this->getDisplayOption('allow_comment_audio_attachments') != 'true')
      {
         $system_config->allow_comment_audio_attachments = false;
      }
      if($this->getDisplayOption('allow_comment_miscellaneous_attachments') != 'true')
      {
         $system_config->allow_comment_miscellaneous_attachments = false;
      }
      if($editor_session->isSessionOpen())
      {
         $this->max_uploads = $this->getDisplayOption('editor_max_attachments');
         if($this->max_uploads == 0 or $this->max_uploads > $system_config->editor_max_attachments)
         {
            $this->max_uploads = $system_config->editor_max_attachments;
         }
         if($this->content_type == 'comment')
         {
            $this->max_comment = $this->getDisplayOption('editor_max_comment');
            if($this->max_comment == 0 or $this->max_comment > $system_config->editor_max_comment)
            {
               $this->max_comment = $system_config->editor_max_comment;
            }
         }
         else
         {
            $this->max_summary = $this->getDisplayOption('editor_max_summary');
            if($this->max_summary == 0 or $this->max_summary > $system_config->editor_max_summary)
            {
               $this->max_summary = $system_config->editor_max_summary;
            }
            $this->max_contents = $this->getDisplayOption('editor_max_contents');
            if($this->max_contents == 0 or $this->max_contents > $system_config->editor_max_contents)
            {
               $this->max_contents = $system_config->editor_max_contents;
            }
         }
      }
      else
      {
         if($this->content_type == 'comment')
         {
            $this->max_uploads = $this->getDisplayOption('comment_max_attachments');
            if($this->max_uploads == 0 or $this->max_uploads > $system_config->comment_max_attachments)
            {
               $this->max_uploads = $system_config->comment_max_attachments;
            }
            $this->max_comment = $this->getDisplayOption('user_max_comment');
            if($this->max_comment == 0 or $this->max_comment > $system_config->user_max_comment)
            {
               $this->max_comment = $system_config->user_max_comment;
            }
         }
         else
         {
            $this->max_uploads = $this->getDisplayOption('story_max_attachments');
            if($this->max_uploads == 0 or $this->max_uploads > $system_config->story_max_attachments)
            {
               $this->max_uploads = $system_config->story_max_attachments;
            }
            $this->max_summary = $this->getDisplayOption('user_max_summary');
            if($this->max_summary == 0 or $this->max_summary > $system_config->user_max_summary)
            {
               $this->max_summary = $system_config->user_max_summary;
            }
            $this->max_contents = $this->getDisplayOption('user_max_contents');
            if($this->max_contents == 0 or $this->max_contents > $system_config->user_max_contents)
            {
               $this->max_contents = $system_config->user_max_contents;
            }
         }
      }
      if($this->max_summary == 0) $this->max_summary = 8192;//default summary max is 8K
      if($this->max_contents == 0) $max_contents = 65536;//64k default
   }

   /*********************************************************************/
   /* Information about the request. - what button was hit?         */
   /*********************************************************************/
   function isNewPublication()
   {
      if(!isset($_REQUEST['publish_button']) and !isset($_REQUEST['preview_confirmed']) and !isset($_REQUEST['cancel'])) return true;
      return false;
   }

   function isCancel()
   {
      if(isset($_REQUEST['cancel'])) return true;
      return false;
   }

   function isConfirmedPublicationRequest()
   {
      if(isset($_REQUEST['preview_confirmed'])) return true;
      return false;
   }

   function isNewItem()
   {
      return ($this->isNewStory() && $this->isNewComment());
   }

   function isNewStory()
   {
      if(isset($this->story->story_id) ) {
          if($this->story->story_id != "" and $this->story->story_id != 0) return false;
      }
      return true;
   }

   function isNewComment()
   {
      if(isset($this->comment->comment_id) and $this->comment->comment_id != "") return false;
      return true;
   }

   // This is only called for edit story or comment and is called only at the start to do the initial
   // load of the embedded audio and video so as to set the form variables.
   function setFormWithEmbedVideoAudio($is_story)
   {
      if ($is_story == true) {
          if ($this->getDisplayOption("youtube_id_allow_in_stories") == 'on' && $this->story->embed_video_attachments != null) {
   
             // Assumption is that the video type is the same for all in a given story.
             $videoType = 0;
             for ($k=0; $k < count($this->story->embed_video_attachments); $k++)
             {
                 $attachment = $this->story->embed_video_attachments[$k];
                 if ($k == 0 ) $videoType = substr($attachment->attachment_file, 11,2);
   
                 if ($k == 0 ) $this->story->youtube_id_1 = substr($attachment->attachment_file, 14);
                 if ($k == 1 ) $this->story->youtube_id_2 = substr($attachment->attachment_file, 14);
                 if ($k == 2 ) $this->story->youtube_id_3 = substr($attachment->attachment_file, 14);
                 if ($k == 3 ) $this->story->youtube_id_4 = substr($attachment->attachment_file, 14);
                 if ($k == 4 ) $this->story->youtube_id_5 = substr($attachment->attachment_file, 14);
   
		 // In edit mode of publish the attachment_id and time_posted will be available and will be
		 // needed so set them, so that they can be written as hidden vars in the form later on.
                 $tmp_name = "embed_vid_desc_".($k+1);
                 $_REQUEST[$tmp_name] = $attachment->description;
                 $tmp_name = "attachment_id_v".($k+1);
                 $_REQUEST[$tmp_name] = $attachment->attachment_id;
                 $tmp_name = "attachment_tp_v".($k+1);
                 $_REQUEST[$tmp_name] = $attachment->time_posted;
             }
	     // For embedded video we assume and only allow the same type for all embedded videos in a given
	     // story or comment.
             $_REQUEST['videoType'] = $videoType;
             $_REQUEST['number_embed_video'] = count($this->story->embed_video_attachments);
          }
      } else {
	  // Assumption is that the video type is the same for all in a given comment.
          if ($this->getDisplayOption("youtube_id_allow_in_comments") == 'on' && $this->comment->embed_video_attachments != null) {
             $videoType = 0;
             for ($k=0; $k < count($this->comment->embed_video_attachments); $k++)
             {
                 $attachment = $this->comment->embed_video_attachments[$k];
                 if ($k == 0 ) $videoType = substr($attachment->attachment_file, 12,2);
   
                 if ($k == 0 ) $this->comment->youtube_id_1 = substr($attachment->attachment_file, 14);
                 if ($k == 1 ) $this->comment->youtube_id_2 = substr($attachment->attachment_file, 14);
                 if ($k == 2 ) $this->comment->youtube_id_3 = substr($attachment->attachment_file, 14);
                 if ($k == 3 ) $this->comment->youtube_id_4 = substr($attachment->attachment_file, 14);
                 if ($k == 4 ) $this->comment->youtube_id_5 = substr($attachment->attachment_file, 14);
   
                 $tmp_name = "embed_vid_desc_".($k+1);
                 $_REQUEST[$tmp_name] = $attachment->description;
                 $tmp_name = "attachment_id_v".($k+1);
                 $_REQUEST[$tmp_name] = $attachment->attachment_id;
                 $tmp_name = "attachment_tp_v".($k+1);
                 $_REQUEST[$tmp_name] = $attachment->time_posted;
             }
             $_REQUEST['videoType'] = $videoType;
             $_REQUEST['number_embed_video'] = count($this->comment->embed_video_attachments);
          }
      }

      // Now do the audio stuff
      if ($is_story == true) {
          if ($this->getDisplayOption("audio_id_allow_in_stories") == 'on' && $this->story->embed_audio_attachments != null) {
             for ($k=0; $k < count($this->story->embed_audio_attachments); $k++)
             {
                 $attachment = $this->story->embed_audio_attachments[$k];
                 if ($k == 0 ) $this->story->audio_id_1 = substr($attachment->attachment_file, 14);
                 if ($k == 1 ) $this->story->audio_id_2 = substr($attachment->attachment_file, 14);
                 if ($k == 2 ) $this->story->audio_id_3 = substr($attachment->attachment_file, 14);
                 if ($k == 3 ) $this->story->audio_id_4 = substr($attachment->attachment_file, 14);
                 if ($k == 4 ) $this->story->audio_id_5 = substr($attachment->attachment_file, 14);
   
                 $tmp_name = "embed_audio_desc_".($k+1);
                 $_REQUEST[$tmp_name] = $attachment->description;
                 $tmp_name = "attachment_id_a".($k+1);
                 $_REQUEST[$tmp_name] = $attachment->attachment_id;
   
                 $tmp_name = "attachment_tp_a".($k+1);
                 $_REQUEST[$tmp_name] = $attachment->time_posted;
             }
             $_REQUEST['number_embed_audio'] = count($this->story->embed_audio_attachments);
          }
      } else {
          if ($this->getDisplayOption("audio_id_allow_in_comments") == 'on' && $this->comment->embed_audio_attachments != null) {
             for ($k=0; $k < count($this->comment->embed_audio_attachments); $k++)
             {
                 $attachment = $this->comment->embed_audio_attachments[$k];
                 if ($k == 0 ) $this->comment->audio_id_1 = substr($attachment->attachment_file, 14);
                 if ($k == 1 ) $this->comment->audio_id_2 = substr($attachment->attachment_file, 14);
                 if ($k == 2 ) $this->comment->audio_id_3 = substr($attachment->attachment_file, 14);
                 if ($k == 3 ) $this->comment->audio_id_4 = substr($attachment->attachment_file, 14);
                 if ($k == 4 ) $this->comment->audio_id_5 = substr($attachment->attachment_file, 14);
   
		 // In edit mode of publish the attachment_id and time_posted will be available and will be
		 // needed so set them, so that they can be written as hidden vars in the form later on.
                 $tmp_name = "embed_audio_desc_".($k+1);
                 $_REQUEST[$tmp_name] = $attachment->description;
                 $tmp_name = "attachment_id_a".($k+1);
                 $_REQUEST[$tmp_name] = $attachment->attachment_id;
                 $tmp_name = "attachment_tp_a".($k+1);
                 $_REQUEST[$tmp_name] = $attachment->time_posted;
             }
             $_REQUEST['number_embed_audio'] = count($this->comment->embed_audio_attachments);
          }
      }
   }

   /*********************************************************************/
   /* Ensure the comment is connected to a valid story         */
   /*********************************************************************/
   function checkCommentValidStory()
   {
      global $editor_session, $oscailt_page_setting;
      if($this->comment->story_id==NULL)
      {
         $this->user_msg=$this->getDisplayOption("invalid_commentnostory_text");
         $this->result = "invalid_story";
         return false;
      }
      $query = new StoryQuery();
      $query->selectTickerMode();
      if($editor_session->isSessionOpen()) $query->ignore_hidden=true;
      $query->results_limit=1;
      $query->story_id=$this->comment->story_id;
      $story_list = $query->execute();
      if(count($story_list)==0)
      {
         $this->user_msg=$this->getDisplayOption("invalid_commentstoryid_text");
         $this->result = "invalid_comment";
         return false;
      }
      $story = $story_list[0];
      if($story->locked && !($editor_session->isSessionOpen() && $editor_session->editor->allowedWriteAccessTo("commentonlocked", $oscailt_page_setting['site_id'])))
      {
         $this->user_msg=$this->getDisplayOption("invalid_commentstorylocked_text");
         $this->result = "invalid_comment";
         return false;
      }
      return true;
   }

   /**********************************************************************
   //Function processes request to see if it is a publication request
   //Also checks that all fields are valid and that all files uploaded are okay
   //inputs: none, outputs: true/false
   /*********************************************************************/
   function isValidPublicationRequest($edit_chk_override=false)
   {
      if($this->content_type == 'comment')
      {
         $valid_story = $this->checkCommentValidStory();
	 // echo("Comment: Valid flag valid_story: ".$valid_story."<BR>");
         if($valid_story === false) return false;
         $problem_list = $this->comment->listValidityProblems();
      }
      else
      {
         //checks to see if each of the required fields has been filled in and does some content checks
         $problem_list = $this->story->listValidityProblems();
      }
      if($this->getDisplayOption("include_legal_section") == 'true' && (!isset($_REQUEST['accept_terms']) or $_REQUEST['accept_terms'] != 'on'))
      {
         if(!isset($problem_list['required'])) $problem_list['required'] = array();
         $problem_list['required'][] = "accept_terms";
         $this->unset_t_and_c = true;
      }

      $valid_contents = true;
      if(count($problem_list['required'])>0)
      {
         $this->field_validation_error = true;
         $this->user_msg = "You failed to fill in some required details in the publish form.  You need to fill in the following fields:<UL>";
         foreach($problem_list['required'] as $r)
         {
            $this->user_msg .= "<LI>".$this->getRequiredFieldTitle($r)."</LI>";
         }
         $this->user_msg .= "</UL>";
         $valid_contents = false;
      }
      $content_problems = array();
      foreach(array_keys($problem_list['content']) as $field_name)
      {
         if(count($problem_list['content'][$field_name]) > 0)
         {
            $content_problems[] = "<b>$field_name</b>: <ol><li>". implode("</li><li>" ,$problem_list['content'][$field_name])."</li></ol>";
         }
      }
      if(count($content_problems) > 0)
      {
         $this->field_validation_error = true;
         $this->user_msg .= "Invalid Data was detected in your input. You need to correct the data in the following fields:<UL><LI>".implode("<LI>", $content_problems)."</UL>";
         $valid_contents = false;
      }

      if($this->content_type == 'story' && $valid_contents == true)
      {
         $invalid_ranges = $this->story->checkReferenceIdRanges();
         if (strlen($invalid_ranges) > 0 ) {
             if ($system_config->security_recording_level >= 1) 
                 logSpammerMessage("Publish story. Invalid reference id for ".$invalid_ranges." - Rejecting request as spam.", "spamlog.txt");
             $valid_contents = false;
	 }
      }

      if($this->use_captcha == true) {
         if(!isset($_REQUEST['cap_digits1']) || ($_REQUEST['cap_digits1'] == "") )
         {
            $this->field_validation_error = true;
            $this->user_msg .= "Captcha data was NOT filled out. Try again with the new captcha at the end of the form.";
            $valid_contents = false;
            $this->generate_captcha = true;
         } 
      }

      if (isset($_REQUEST['title']) && strlen($_REQUEST['title']) > 0) {
         global $system_config;
         if ($system_config->security_recording_level >= 1)
             logSpammerMessage("Publish story. Title set to ".$_REQUEST['title']." - Rejecting request as spam.", "spamlog.txt");
         $this->user_msg = "Invalid title content";
         $valid_contents = false;
      }


      if($valid_contents == false)
      {
         $this->result = "invalid_content";
         return false;
      }

      //checks to ensure that any edits are allowed....
      if($edit_chk_override == false)
      {
         if(($this->content_type == 'story' && !$this->isNewStory()) or ($this->content_type == 'comment' && !$this->isNewComment()))
         {
            $edit_okay = $this->checkEditPermissions(false);
            if(!$edit_okay)
            {
               return false;
            }
         }
      }
      //finally check the various rules about particular field sizes and attachment limits etc...
      $edit_okay = $this->checkSizeLimits();

      if(!$edit_okay) return false;
      //then upload the various files attached if there are any.
      return $this->checkFiles();
   }

   function getRequiredFieldTitle($f)
   {
      $ret = "";
      if($f == 'story title')
      {
         $ret = $this->getDisplayOption("title_title");
      }
      elseif($f == 'story summary')
      {
         $ret = $this->getDisplayOption("summary_title");
      }
      elseif($f == 'story content')
      {
         $ret = $this->getDisplayOption("content_title");
      }
      elseif($f == 'language')
      {
         $ret = $this->getDisplayOption("language_title");
      }
      elseif($f == 'topic')
      {
         $ret = $this->getDisplayOption("topic_title");
      }
      elseif($f == 'type')
      {
         $ret = $this->getDisplayOption("type_title");
      }
      elseif($f == 'region')
      {
         $ret = $this->getDisplayOption("region_title");
      }
      elseif($f == 'accept_terms')
      {
         $ret = $this->getDisplayOption("termsandconditions_title");
      }
      if($ret != "") return $ret;
      return $f;
   }


   function checkSizeLimits()
   {
      global $editor_session, $system_config;

      if($editor_session->canIgnoreLimits()) return true;
      if(isset($_REQUEST['number_of_files']) && $_REQUEST['number_of_files'] > $this->max_uploads)
      {
         $_REQUEST['number_of_files'] = $this->max_uploads;
      }
      if(isset($_REQUEST['number_of_files_selected']) && $_REQUEST['number_of_files_selected'] > $this->max_uploads)
      {
         $_REQUEST['number_of_files_selected'] = $this->max_uploads;
      }

      if($this->content_type == 'comment')
      {
         if(strlen($this->comment->comment_content)>$this->max_comment)
         {
            $this->user_msg="<span class='error'>".$this->getDisplayOption("invalid_content_text") ."</span><br>Max: (".$this->max_comment.' characters) allowed in story comment content';
            $this->result = "invalid_content";
            $this->field_validation_error = true;
            return false;
         }
      }
      else
      {
         //only editors can post events in the past...
         if($editor_session->isSessionOpen()==false && $this->story->type_id==$system_config->event_type_id && ($this->story->event_time+$system_config->timezone_offset) < strtotime("11:59pm yesterday"))
         {
            //should perhaps reset the event time here?
            $this->user_msg="<span class='error'>".$this->getDisplayOption("invalid_eventdate_text")."<span>";
            $this->result = "invalid_eventdate";
            $this->field_validation_error = true;
            return false;
         }
         if(strlen($this->story->story_summary) > $this->max_summary)
         {
            $this->user_msg="<span class='error'>".$this->getDisplayOption("invalid_summary_text")."</span><br>Max: $this->max_summary characters";
            $this->result = "invalid_summary";
            $this->field_validation_error = true;
            return false;
         }
         if(strlen($this->story->story_content) > $this->max_contents)
         {
            $this->user_msg="<span class='error'>".$this->getDisplayOption("invalid_content_text") ."</span><br>Max ".$this->max_contents.' characters allowed in story content';
            $this->result = "invalid_content";
            $this->field_validation_error = true;
            return false;
         }
      }
      return true;
   }

   /*********************************************************************/
   /* Do the uploading of the actual files                         */
   /* if the user has left the number of files at zero, then we just return and no uploading is done. */
   /* There is no preview when uploading files.                                                       */
   /* This function now handles embedded video and audio even though there is nothing to upload for them */
   /*********************************************************************/
   function checkFiles()
   {
      // check files
      $this->attachments = array();
      //here is where we do all of our transforming of the limits etc...

      if(isset($_REQUEST['number_of_files_selected']) && $_REQUEST['number_of_files_selected']>0)
      {
         $it_is_story = false;
         if ($this->content_type == 'story' ) $it_is_story = true;

         for($i=1;$i<=$_REQUEST['number_of_files_selected'];$i++)
         {
            $attachment = new Attachment();
            $attachment->description=$_REQUEST['file_'.$i.'_description'];
            $result = $attachment->createFromUpload('file_'.$i, $it_is_story);
            if($result!="OK")
            {
               $t_count = $_REQUEST['number_of_files_selected'];
               for($j=0;$j < count($this->attachments); $j++)
               {
                  $this->attachments[$j]->deleteFile();
               }
               $this->user_msg="Attachment ".($j+1).": ".$result;

	       // Append any additional error message here.
               // $this->user_msg .="<BR>Go back and fix your image or file and then please try again.";

               $this->result = "invalid_attachments";
               $this->field_validation_error = true;

   	       if ($it_is_story == true ) $t_author_name = $this->story->author_name;
	       else $t_author_name = $this->comment->author_name;

   	       logMessage("Publish (".$this->content_type." by ".$t_author_name.") ".$t_count." attachments failure. Reason: ".$result, "sitelog.txt");
               return false;
            }
            else array_push($this->attachments,$attachment);
         }
      }

      // Save embedded video and audio as an id in the attachment object. No uploading actually done.
      if(isset($_REQUEST['videoType']) && isset($_REQUEST['youtube_id_1']) && $_REQUEST['youtube_id_1'] != "")
      {
         $embedded_count = 1;
         if(isset($_REQUEST['number_embed_video'])) $embedded_count = $_REQUEST['number_embed_video'];

         for($i=1;$i<=$embedded_count;$i++)
         {
            $request_var = "youtube_id_".$i;
            if (trim($_REQUEST[$request_var]) == "") continue;
             
            $desc_field = "embed_vid_desc_".$i;

            $attachment = new Attachment();
	    // Set to attachment_id later.
            $attachment->video=true;
            if (isset($_REQUEST[$desc_field])  && $_REQUEST[$desc_field] == "") 
                $attachment->description="Embedded video " .  getEmbeddedVideoTypes($_REQUEST['videoType']);
            else
                $attachment->description=$_REQUEST[$desc_field];

	    $attach_id = "attachment_id_v".$i;
	    if (isset($_REQUEST[$attach_id])) {
                $attachment->attachment_id = $_REQUEST[$attach_id];
                $attachment->video = $attachment->attachment_id;
            }

	    $attach_id = "attachment_tp_v".$i;
	    if (isset($_REQUEST[$attach_id])) $attachment->time_posted = $_REQUEST[$attach_id];

	    if ($_REQUEST['videoType'] < 10 ) $vidType = "0". trim($_REQUEST['videoType']);
	    else $vidType = trim($_REQUEST['videoType']);

	    $tmp_vid_id = trim($_REQUEST[$request_var]);
	    // Video Id validation: This check probably should be just for type Youtube (01)
	    // Remove up to 9 characters proceeding the equals and the equals sign
	    $tmp_vid_id = preg_replace("/.{0,9}=/", "", $tmp_vid_id);
	    
            $attachment->attachment_file="embedvideo:" . $vidType . ":" .  $tmp_vid_id;

            array_push($this->attachments,$attachment);
         }
      }

      // Now handle the embedded audio if any.
      if(isset($_REQUEST['audio_id_1']) && $_REQUEST['audio_id_1'] != "")
      {
         $embedded_count = 1;
         if(isset($_REQUEST['number_embed_audio'])) $embedded_count = $_REQUEST['number_embed_audio'];

         for($i=1;$i<=$embedded_count;$i++)
         {
            $request_var = "audio_id_".$i;
            if ($_REQUEST[$request_var] == "") continue;

            $desc_field = "embed_audio_desc_".$i;
             
            $attachment = new Attachment();
	    // Set to attachment_id later.
            $attachment->audio=true;

	    $attach_id = "attachment_id_a".$i;
	    if (isset($_REQUEST[$attach_id])) {
                $attachment->attachment_id = $_REQUEST[$attach_id];
                $attachment->audio = $attachment->attachment_id;
            }

	    $attach_id = "attachment_tp_a".$i;
	    if (isset($_REQUEST[$attach_id])) $attachment->time_posted = $_REQUEST[$attach_id];

	    // Index of embedded audio type is fixed at 01 since we use an embedded audio player.
            $attachment->attachment_file="embedaudio:01:" .  $_REQUEST[$request_var];

            // Maybe we should ask the user for the description... we do now ... but default it if empty
            if (isset($_REQUEST[$desc_field])  && $_REQUEST[$desc_field] == "") 
                $attachment->description="Embedded audio: " . $_REQUEST[$request_var];
            else
                $attachment->description=$_REQUEST[$desc_field];

            if (strlen($attachment->description) > 255) 
	       $attachment->description = substr($attachment->description,0, 255);

            array_push($this->attachments,$attachment);
         }
      }
      return true;
   }

   /*********************************************************************/
   /* Create the attachment list from the embedded video and or audio   */
   /*********************************************************************/
   function getEmbedVideoAudio()
   {
      if(isset($_REQUEST['embed_video_index']) && isset($_REQUEST['youtube_id_1']) && $_REQUEST['youtube_id_1'] != "")
      {
         $embedded_count = 3;
         if(isset($_REQUEST['number_embed_video'])) $embedded_count = $_REQUEST['number_embed_video'];

         for($i=1;$i<=$embedded_count;$i++)
         {
            $request_var = "youtube_id_".$i;
            if (trim($_REQUEST[$request_var]) == "") continue;
             
            $attachment = new Attachment();
            $attachment->video=true;
            //$attachment->description=$_REQUEST['embed_video_'.$i.'_description'];
            $attachment->description="Embedded video " .  getEmbeddedVideoTypes($_REQUEST['embed_video_index']);

	    if ($_REQUEST['embed_video_index'] < 10 ) $vidType = "0". $_REQUEST['embed_video_index'];
	    else $vidType = $_REQUEST['embed_video_index'];

            $attachment->attachment_file="embedvideo:" . $vidType . ":" .  trim($_REQUEST[$request_var]);
            array_push($this->attachments,$attachment);
         }
      }

   }
   /*********************************************************************/
   /* Ensure user has sufficent permissions                      */
   /*********************************************************************/
   function checkEditPermissions($is_write)
   {
      global $editor_session, $oscailt_page_setting;
      if($editor_session->isSessionOpen()==false)
      {
         $this->user_msg=$this->getDisplayOption("no_session_text");
         $this->result = "no_session_error";
         return false;
      }
      $site_id = $oscailt_page_setting['site_id'];
      if($this->content_type == 'comment')
      {
         if($this->comment->comment_id!=null && ($editor_session->isSessionOpen()==false || (!$editor_session->editor->allowedReadAccessTo("editcomment", $site_id)) or ($is_write && !$editor_session->editor->allowedWriteAccessTo("editcomment", $site_id))))
         {
            if(!($editor_session->editor->allowedWriteAccessTo("editowncontent", $site_id) && strcasecmp($this->comment->author_name, $editor_session->editor->editor_name) == 0))
            {
               $this->user_msg=$this->getDisplayOption("no_editcomment_permission_text");
               if($is_write) $this->result = "no_write_permission_error";
               else $this->result = "no_permission_error";
               return false;
            }
        }
      }
      else
      {
         $this->checkstory = new Story();
         $this->checkstory->story_id=$this->story->story_id;
         $this->checkstory->load();
         if($this->checkstory->type_id==1 && $this->story->type_id!=1 &&
           ($editor_session->editor->allowedWriteAccessTo("editfeature", $site_id)==false ||
           $editor_session->editor->allowedWriteAccessTo("editstory", $site_id)==false))
         {
            $this->user_msg=$this->getDisplayOption("no_downgrade_permission_text");
            if($is_write) $this->result = "no_write_permission_error";
            else $this->result = "no_permission_error";
            return false;
         }
         else if($this->checkstory->type_id!=1 && $this->story->type_id==1 &&
           ($editor_session->editor->allowedWriteAccessTo("editfeature", $site_id)==false ||
           $editor_session->editor->allowedWriteAccessTo("editstory", $site_id)==false))
         {
            $this->user_msg=$this->getDisplayOption("no_upgrade_permission_text");
            if($is_write) $this->result = "no_write_permission_error";
            else $this->result = "no_permission_error";
            return false;
         }
         else if($this->story->type_id==1 && (!$editor_session->editor->allowedReadAccessTo("editfeature", $site_id) or ($is_write && !$editor_session->editor->allowedWriteAccessTo("editfeature", $site_id))))
         {
            $this->user_msg= $this->getDisplayOption("no_editfeature_permission_text");
            if($is_write) $this->result = "no_write_permission_error";
            else $this->result = "no_permission_error";
            return false;
         }
         else if($this->story->type_id != 1)
         {
            if(($editor_session->editor->allowedReadAccessTo("editowncontent", $site_id) or ($is_write and $editor_session->editor->allowedWriteAccessTo("editowncontent", $site_id))) and (strcasecmp($this->story->author_name, $editor_session->editor->editor_name) == 0))
            {
               return true;
            }
            else if(!$editor_session->editor->allowedReadAccessTo("editstory", $site_id) or ($is_write && !$editor_session->editor->allowedWriteAccessTo("editstory", $site_id)))
            {
               $this->user_msg= $this->getDisplayOption("no_editstory_permission_text");
               if($is_write) $this->result = "no_write_permission_error";
               else $this->result = "no_permission_error";
               return false;
            }
         }
      }
      return true;
   }

   /*********************************************************************/
   /* Security checks when form is loaded                         */
   /*********************************************************************/
   function checkPublishFormSecurity()
   {
      global $system_config, $editor_session;

      //first check the blocked ips from the database
      $ipBanList = new BannedIPList();
      $ipBanList->load();
      $ipBanList->clearOldBans();

      $ip = cleanseIP($_SERVER['REMOTE_ADDR']);
      if($ip === false or $ipBanList->isBanned($ip))
      {
         $this->user_msg=$ipBanList->getBannedMessage($ip);
         $this->result = "publishing_banned";

         if($this->content_type == 'story') $p_type = "story";
	 else $p_type = "comment";
   	 logMessage("Publish (".$p_type.") blocked for banned IP address. Ban reason was: ".$ipBanList->getBannedReason($ip), "sitelog.txt");
   	 logMessage("Publish (".$p_type.") blocked for banned IP address ".$ip. " Banned reason was: ".$ipBanList->getBannedReason($ip), "bannedlog.txt");
   	 sleep(2);
         return false;
      }

      $hostBanList = new BannedHostList();
      $hostBanList->load();
      $hostBanList->clearOldBans();
      // if (strlen($system_config->ban_hostname_like_list) > 0 ) 

      if (count($hostBanList->banned_list) > 0 ) 
      {
	 $t_hostname = gethostbyaddr($_SERVER['REMOTE_ADDR']);
	 // $t_hostname = "10.20.30.40.ripwave-broadband.ie";

	 $t_matched_host = $hostBanList->findHostMatches($t_hostname);

	 if ($t_matched_host != "" ) 
	 {
	     $t_browser = trim($_SERVER['HTTP_USER_AGENT']);
             if ($hostBanList->isBanned($t_matched_host, $t_browser) == true ) 
             {
                $t_bantype = $hostBanList->getBanType($t_matched_host, $t_browser);
		// Ban types are 1 ban and 2 moderate
		if ($t_bantype == 1 ) {
         		$this->user_msg="Error with publishing. Aborting request";
         		$this->user_msg=$hostBanList->getBannedMessage($t_matched_host, $t_browser);
         		$this->result = "publishing_banned";

         		if($this->content_type == 'story') $p_type = "story";
	 		else $p_type = "comment";
   	 		logMessage("Publish (".$p_type.") blocked for banned hostname like ".$t_matched_host.". IP was: ".$ip ." Host was: ".$t_hostname." Ban reason was: ".$hostBanList->getBannedReason($t_matched_host, $t_browser), "sitelog.txt");
   	 		logMessage("Publish (".$p_type.") blocked for banned hostname like ".$t_matched_host.". IP was: ".$ip ." Host was: ".$t_hostname." Ban reason was: ".$hostBanList->getBannedReason($t_matched_host, $t_browser), "bannedlog.txt");
   	 		sleep(2);
         		return false;
		} else if ($t_bantype >= 2 ) {
			$this->user_on_moderation = true;
		}
      	    }
      	}
      }

      // Now check the blocked author names (if any) from the database, but bypass if it is an editor.
      // But only if author is defined otherwise it will be caught in form validation later.
      if (isset($_REQUEST['author_name']) && $editor_session->isSessionOpen() == false) {
         $authorBanList = new BannedAuthorList();
         $authorBanList->load();
         $author_name = trim($_REQUEST['author_name']);
         if($authorBanList->isBanned($author_name))
         {
            if($this->content_type == 'story') $p_type = "story";
	    else $p_type = "comment";
            // Ban this ip for 2 hours as it has used a banned author name.
            $ban_end = time() + (2 * 60 * 60);
            $r = "Publisher (".$p_type.") used a banned author name: " . $author_name . ". IP banned for 2 hrs";
            $ipBanList->banIP($ip, $ban_end, $r);
   	    $logMsg = "system:\tban:\tIP:\t" . $ip . ":\t " .$r;
   	    logMessage($logMsg, "actionlog.txt");
   	    logMessage($logMsg, "bannedlog.txt");
            // To notify by email will require some changes to that code.
   	      
            $this->user_msg=$authorBanList->getBannedMessage($author_name);
            $this->result = "publishing_banned";
            return false;
         }
      }


      if($this->getDisplayOption('publish_block') > 0)
      {
         //next the ticket publishing stuff
         $the_id = $editor_session->token;
         if($this->content_type == 'story')
         {
            $ticket = new Ticket('n');
            $this->setPublishTicketParameters($ticket);
         }
         else
         {
            $ticket = new Ticket();
            $this->setPublishCommentTicketParameters($ticket);
         }

         //check to see if it is a new story (ie none of the
         if ($this->isNewPublication() or $ticket->wasTicketUsed($the_id) == true)
         {
            if ( $ticket->generateNewTicket() == false )
            {
               $blocked = $ticket->blockedTimeRemaining();
               if($blocked == 0)
               {
                  $this->result = "no_cookie";
                  $this->user_msg = $this->getDisplayOption("invalid_no_cookie_text");
               }
               else
               {
                  $this->user_msg=$this->getDisplayOption("invalid_token_text");
                  $mins = (int)($blocked / 60);
                  $secs = $blocked % 60;
                  $this->user_msg .= "<p><tt>$mins mins, $secs secs</tt> remaining";
                  $this->user_msg .= " ".$this->getDisplayOption("seconds_till_publish_text")." a ".$this->content_type;
                  $this->result = "invalid_token";
               }
               return false;
            }
            $the_id = $ticket->getTicket();
         }
         //where to put the anti-bot delay
      }
      
      if(!empty($this->author_names))
      {
         if($editor_session->isSessionOpen()==false)
         {
            $this->user_msg = $this->getDisplayOption("no_publish_permission_text");
	    echo($this->user_msg);
            return false;
         }
         $found_name = false;
         foreach($this->author_names as $auth_name)
         {
            $p = "";
            similar_text(strtoupper($auth_name), strtoupper($editor_session->editor->editor_name), $p);
            if($p >= 95) $found_name = true;
         }
         if(!$found_name)
         {
            $this->user_msg = "Sorry ". $editor_session->editor->editor_name." ".$this->getDisplayOption("no_publish_permission_text");
            return false;
         }
      }

      return $this->lockItem();
   }

   /*********************************************************************/
   /* Preview Screen Security Checks                            */
   /*********************************************************************/
   function checkPreviewSecurity()
   {
      global $editor_session;
      $the_id = $editor_session->token;
      if($this->getDisplayOption('publish_block') > 0)
      {
         if($this->content_type == 'story')
         {
            $ticket = new Ticket('n');
            $this->setPublishTicketParameters($ticket);
         }
         else
         {
            $ticket = new Ticket();
            $this->setPublishCommentTicketParameters($ticket);
         }
         if($ticket->wasTicketUsed($the_id) == true or $ticket->isTicketTimeGapValid() == false )
         {
            logMessage("Blocked publish of possible spam story by author: ".$this->story->author_name." with title: " .$this->story->story_title,"sitelog.txt");
            $blocked = $ticket->blockedTimeRemaining();
            if($blocked == 0)
            {
               $this->result = "no_cookie";
               $this->user_msg = $this->getDisplayOption("invalid_no_cookie_text");
            }
            else
            {
               $mins = (int)($blocked / 60);
               $secs = $blocked % 60;
               $this->user_msg=$this->getDisplayOption("invalid_token_text");
               $this->user_msg .= "<p><tt>$mins mins, $secs secs</tt> remaining";
               $this->user_msg .= " ".$this->getDisplayOption("seconds_till_publish_text")." a ".$this->content_type;
               $this->result = "invalid_token";
            }

            return false;
         }
      }
      return true;//$this->lockItem();
   }


   /*********************************************************************/
   /* Pre-publish security steps                               */
   /*********************************************************************/
   function checkPublishSecurity($invalidate = false)
   {
      global $editor_session;

      if($invalidate == false && $this->use_captcha == true)
      {
	  if (!isset($_REQUEST['cap_digits1']) || !isset($_REQUEST['cap_digits2']) ) {
              $log_msg = "Attempted spam publish. No captcha variables in form ";
              // Pretty certain it is spam if this is not provided.
              // Different ticket constructors for story and comment.
              if($this->content_type == 'story')
                  $ticket = new Ticket('n');
              else
                  $ticket = new Ticket();

              $this->setPublishTicketParameters($ticket);
              $ticket->invalidateTicket();
              logMessage($log_msg,"sitelog.txt");
              return false;
          }
	  $captcha = new NumericCaptcha();
	  if ( $captcha->validateCaptchaData($_REQUEST['cap_digits1'], $_REQUEST['cap_digits2']) == false) {
              if($this->content_type == 'story') $log_msg = "Story ";
	      else $log_msg = "Comment ";

              $log_msg .= "publish failure. Captcha data did not match in form ";
              logMessage($log_msg,"sitelog.txt");
              return false;
          }
      }

      if($invalidate == false) {
          global $system_config;
          if (!$editor_session->isSessionOpen() or true) {
          //if (!$editor_session->isSessionOpen() && $this->isNewStory() ) {
              if (($system_config->check_spamwords & 1) == true && $this->content_type == 'story') {
                  if (storySpamCheck($this->story, true, "story") == false) {
		      // If Discard spam enabled then do that otherwise auto hide
                      if (($system_config->check_spamwords & 8) == true) {
                          $this->user_msg = "There were spamwords detected in the content. Post rejected.";
                          $this->result = "spammer";
                          return false;
		      } else {
                         $this->story->hidden = true;
			 // Log action will take place later
                         $this->hide_spam = true;
		      }
                  }
              }
	      // Links check
              if ($system_config->check_spamlinks > 0 && $this->content_type == 'story') {
                  if (checkStoryLinks($this->story, $system_config->check_spamlinks, true) == false) {
		      // If Discard spam enabled then do that otherwise auto hide
                      if (($system_config->check_spamwords & 8) == true) {
                          $this->user_msg = "There were spamwords detected in the content. Post rejected.";
                          $this->result = "spammer";
                          return false;
		      } else {
                         $this->story->hidden = true;
			 // Log action will take place later
                         $this->hide_spam = true;
		      }
                  }
              }

              if (($system_config->check_spamwords & 2) == true && $this->content_type == 'story') {
                  storyBogusCheck($this->story, true, "story");
              }
              if (($system_config->check_spamwords & 4) == true && $this->content_type == 'comment') {
                  commentBogusCheck($this->comment, true, "comment");
              }
          }
      }

      if($this->getDisplayOption('publish_block') > 0)
      {
         if($this->content_type == 'story')
         {
            $ticket = new Ticket('n');
            $this->setPublishTicketParameters($ticket);
            $the_id = $editor_session->newstoken;
         }
         else
         {
            $ticket = new Ticket();
            $this->setPublishCommentTicketParameters($ticket);
            $the_id = $editor_session->token;
         }

         # Function checks for null
         if(!$invalidate && $ticket->isTicketStillValid($the_id) != true )
         {
            $blocked = $ticket->blockedTimeRemaining();
            if($blocked == 0)
            {
               $this->result = "no_cookie";
               $this->user_msg = $this->getDisplayOption("invalid_no_cookie_text");
            }
            else
            {
               $mins = (int)($blocked / 60);
               $secs = $blocked % 60;
               $this->user_msg=$this->getDisplayOption("invalid_token_text");
               $this->user_msg .= "<p><tt>$mins mins, $secs secs</tt> remaining";
               $this->user_msg .= " ".$this->getDisplayOption("seconds_till_publish_text")." a ".$this->content_type;
               $this->result = "invalid_token";
            }
            if(isset($_REQUEST['number_of_files_selected']) && $_REQUEST['number_of_files_selected'] > 0 )
	    {
                if($this->content_type == 'story') $publish_type = "story";
		else $publish_type = "comment";

                $log_msg = "Publish ".$publish_type." failure: No of files = " . $_REQUEST['number_of_files_selected'] . ". Failure reason is " . $this->result . ". Browser = ".$_SERVER['HTTP_USER_AGENT'] ;
                logMessage($log_msg,"sitelog.txt");

                for($iFile=0;$iFile < count($this->attachments); $iFile++) {
                    $log_msg = "Potential orphaned files from failure are: File ".($iFile+1).": " . $this->attachments[$iFile]->attachment_file;
                    logMessage($log_msg,"sitelog.txt");
                }
	    }
            return false;
         }
         elseif($invalidate && $ticket->isTicketStillValid($the_id) == true)
         {
            $ticket->invalidateTicket();
         }
      }

      //$result = $this->checkThrottleLimits();
      //if(!$result) return false;
      if($invalidate) return true;
      else return $this->checkLocks();
   }

   /*function checkThrottleLimits()
   {
      global $system_config, $prefix, $dbconn;
      if(!$this->isNewItem()) return true;
      $time_cut_off = (time()+$system_config->timezone_offset) - (60*60);
      //$sql = "SELECT count(".$this->content_type."_id) from $prefix".$this->content_type." WHERE UNIX_TIMESTAMP(time_posted) > $time_cut_off";
      if($num >= $system_config->maximum_stories_per_hour) return false;
      return true;
   }*/

   function setPublishCommentTicketParameters(&$ticket)
   {
      $max_block = 60*12;
      $block = $this->getDisplayOption('publishcomments_block');
      $write_time = $this->getDisplayOption('publishcomments_writingtime');
      //if($block < 1 or $block > $max_block) $block = $max_block;
      if($block > $max_block) $block = $max_block;
      $block = $block * 60;
      if($write_time < 1 or $write_time > $max_block) $write_time = $max_block;
      //if($write_time > $max_block) $write_time = $max_block;
      $write_time = $write_time * 60;
      $ticket->setTimeouts($block, $write_time);
   }

   function setPublishTicketParameters(&$ticket)
   {
      $max_block = 60*12;
      $block = $this->getDisplayOption('publish_block');
      $write_time = $this->getDisplayOption('publish_writingtime');
      //if($block < 1 or $block > $max_block) $block = $max_block;
      if($block > $max_block) $block = $max_block;
      $block = $block * 60;
      //if($write_time > $max_block) $write_time = $max_block;
      if($write_time < 1 or $write_time > $max_block) $write_time = $max_block;
      $write_time = $write_time * 60;
      $ticket->setTimeouts($block, $write_time);
   }




   /*********************************************************************/
   /* Functions to take care of edit locking                      */
   /*********************************************************************/
   function lockItem()
   {
      global $system_config;
      if(($system_config->edit_locking_time > 0) && !$this->isNewItem())
      {
         $lock_ok = false;
         $lock = new EditLock();
         if($this->content_type == 'comment')
         {
            $id = $this->comment->comment_id;
         }
         else
         {
            $id = $this->story->story_id;
         }
         if($lock->lockItem($this->content_type, $id))
         {
            $lock_ok = true;
         }
         elseif($lock->lock_id == 0)
         {
            $this->user_msg = $lock->result_message;
            $lock_ok = false;
         }
         else
         {
            $result = $lock->isUsable();
            $this->user_msg = $lock->result_message;
            $lock_ok = $result;
         }
         $this->lock = $lock;
         if(!$lock_ok) return false;
      }
      return true;
   }

   function showLock()
   {
      global $editor_session, $graphics_store;
      if(isset($this->lock) && $this->lock != null)
      {
         echo "<div class='user-message'>";
         //echo "You have locked this item";
         echo $this->lock->icon;
         echo "You have locked this item for editing.  As soon as you save it the lock will be released.";
         $x = $this->lock->lock_expiry_time-time();
         $mins = (int)($x / 60);
         $secs = (int)($x % 60);
         echo ". <b>Lock Time Left</b>: ";
         echo "<form class='timerform' name='timerform' action='?' method='get'>";
         echo "<input disabled class='timerform' size='5' name='timeleft' value='$mins:$secs'>";
         echo "</form><br>";
         $this->writeTimerJS($x);

         //echo " If you decide not to save it, you can ";
         if($this->content_type =='comment')
         {
            $story_id = $this->comment->story_id;
            $target_id = $this->comment->comment_id;
         }
         else
         {
            $story_id = $this->story->story_id;
            $target_id = $story_id;
         }
         $lnk = $this->getStoryLink($story_id);
         $lnk = addLinkParameters($lnk, array("release".$this->content_type."=".$target_id));
         echo "<a href='$lnk'><img src='".$graphics_store."unlock.gif' alt='release lock' class='icon'>release your edit lock</a> and return to the article view";
         echo "</div>";
      }

   }

   function writeTimerJS($x)
   {
      ?>
      <script type="text/javascript" language="JavaScript">
      InitializeTimer(<?=$x?>);
      </SCRIPT>
      <?
   }


   function checkLocks()
   {
      global $system_config;
      if($system_config->edit_locking_time <= 0 or $this->isNewItem())
      {
         return true;
      }
      elseif(!isset($this->lock) or $this->lock == null)
      {
         return false;
      }
      /*$lock = new EditLock(); */
      if($this->content_type == 'comment')
      {
         $id = $this->comment->comment_id;
      }
      else
      {
         $id = $this->story->story_id;
      }
      $result = $this->lock->canUpdate($this->content_type, $id);
      if(isset($this->lock->result_message))
      {
         $this->user_msg = $lock->result_message;
      }
      return $result;
   }

   function relinquishLocks()
   {
      global $editor_session;
      if($this->isNewItem() or (!$editor_session->isSessionOpen() && $this->allow_public_edits != true))
      {
         return true;
      }

      $lock = new EditLock();
      if($this->content_type == 'comment')
      {
         $id = $this->comment->comment_id;
      }
      else
      {
         $id = $this->story->story_id;
      }
      $lock->lockItem($this->content_type, $id);
      $lock->delete();
      $lock->cleanUp();
   }

   function logEmbedVideoPublish($is_story)
   {
         
      if($is_story == true)
      {
         $number_videos = $this->getDisplayOption("max_embedvideo_in_stories"); 
         // Msg is: Story published with Video Id = xxxxx Type: $_REQUEST['videoType'];

         for ($i_vid = 1;$i_vid <=$number_videos; $i_vid++) {
            $t_video_id = "";

            if ($i_vid == 1 && $this->story->youtube_id_1 != "") $t_video_id = $this->story->youtube_id_1;
            if ($i_vid == 2 && $this->story->youtube_id_2 != "") $t_video_id = $this->story->youtube_id_2;
            if ($i_vid == 3 && $this->story->youtube_id_3 != "") $t_video_id = $this->story->youtube_id_3;
            if ($i_vid == 4 && $this->story->youtube_id_4 != "") $t_video_id = $this->story->youtube_id_4;
            if ($i_vid == 5 && $this->story->youtube_id_5 != "") $t_video_id = $this->story->youtube_id_5;

            if ($t_video_id != "") {
               $tmp_msg="Story published with Video Id = " . $t_video_id . " Type: ". $_REQUEST['videoType'];
               logMessage($tmp_msg, "sitelog.txt");
            }
         }
      }
      else
      {
         $number_videos = $this->getDisplayOption("max_embedvideo_in_comments"); 

         for ($i_vid = 1;$i_vid <=$number_videos; $i_vid++) {
            $t_video_id = "";

            if ($i_vid == 1 && $this->comment->youtube_id_1 != "") $t_video_id = $this->comment->youtube_id_1 ;
            if ($i_vid == 2 && $this->comment->youtube_id_2 != "") $t_video_id = $this->comment->youtube_id_2 ;
            if ($i_vid == 3 && $this->comment->youtube_id_3 != "") $t_video_id = $this->comment->youtube_id_3 ;
            if ($i_vid == 4 && $this->comment->youtube_id_4 != "") $t_video_id = $this->comment->youtube_id_4 ;
            if ($i_vid == 5 && $this->comment->youtube_id_5 != "") $t_video_id = $this->comment->youtube_id_4 ;

            if ($t_video_id != "") {
               $tmp_msg="Comment published with Video Id = " . $t_video_id . " Type: ". $_REQUEST['videoType'];
               logMessage($tmp_msg, "sitelog.txt");
            }
         }
      }

   }
   
   function logEmbedAudioPublish($is_story)
   {
         
      if($is_story == true)
      {
         $number_audios = $this->getDisplayOption("max_embedaudio_in_stories"); 
         for ($i_aud = 1;$i_aud <=$number_audios; $i_aud++) {
            $t_audio_id = "";

            if ($i_aud == 1 && $this->story->audio_id_1 != "") $t_audio_id = $this->story->audio_id_1 ;
            if ($i_aud == 2 && $this->story->audio_id_2 != "") $t_audio_id = $this->story->audio_id_2 ;
            if ($i_aud == 3 && $this->story->audio_id_3 != "") $t_audio_id = $this->story->audio_id_3 ;
            if ($i_aud == 4 && $this->story->audio_id_4 != "") $t_audio_id = $this->story->audio_id_4 ;
            if ($i_aud == 5 && $this->story->audio_id_5 != "") $t_audio_id = $this->story->audio_id_5 ;

            if ($t_audio_id != "") {
               $tmp_msg="Story published with Audio URL = " . $t_audio_id;
               logMessage($tmp_msg, "sitelog.txt");
            }
         }
      }
      else
      {
         $number_audios = $this->getDisplayOption("max_embedaudio_in_comments"); 
         for ($i_aud = 1;$i_aud <=$number_audios; $i_aud++) {
            $t_audio_id = "";

            if ($i_aud == 1 && $this->comment->audio_id_1 != "") $t_audio_id = $this->comment->audio_id_1 ;
            if ($i_aud == 2 && $this->comment->audio_id_2 != "") $t_audio_id = $this->comment->audio_id_2 ;
            if ($i_aud == 3 && $this->comment->audio_id_3 != "") $t_audio_id = $this->comment->audio_id_3 ;
            if ($i_aud == 4 && $this->comment->audio_id_4 != "") $t_audio_id = $this->comment->audio_id_4 ;
            if ($i_aud == 5 && $this->comment->audio_id_5 != "") $t_audio_id = $this->comment->audio_id_5 ;

            if ($t_audio_id != "") {
               $tmp_msg="Comment published with Audio URL = " . $t_audio_id;
               logMessage($tmp_msg, "sitelog.txt");
            }
         }
      }

   }

   /*********************************************************************/
   /* Carries out the publish itself                            */
   /*********************************************************************/
   function executePublish()
   {
      global $editor_session, $system_config, $oscailt_basic_config, $oscailt_page_setting;

      if($this->content_type != 'comment') $this->story->mergeFeatureContent();
      $site_id = $oscailt_page_setting['site_id'];

      if(($this->content_type == 'story' && !$this->isNewStory()) or ($this->content_type == 'comment' && !$this->isNewComment()))
      {
         if($this->allow_public_edits == true) $requires_approval = false;
	 else $requires_approval = !$this->checkEditPermissions(true);
      }
      else
      {
         $requires_approval = false;
      }

      $was_new_story = false;
      $was_new_comment = false;
      $notify_action = "Edit";
      //remove html unless the editor is allowed to use it.
      if($this->content_type == 'comment') $this->comment->format();
      else $this->story->format();
      $log_action = false;
      if($this->content_type == 'story')
      {
         // If the YouTube Id was entered, then add it into the story content.
         if ($this->getDisplayOption("youtube_id_allow_in_stories") == 'on') {
             $this->logEmbedVideoPublish(true);
         }
         // If Embedded audio was allowed and entered then log the fact. 
         if ($this->getDisplayOption("audio_id_allow_in_stories") == 'on') {
             $this->logEmbedAudioPublish(true);
         }

         if($this->story->type_id == $oscailt_basic_config['feature_type_id'])
         {
            $item_type = "feature";
         }
         elseif($this->story->type_id == $system_config->event_type_id)
         {
            $item_type = "event";
         }
         else
         {
            $item_type = "article";
         }

         if($editor_session->isSessionOpen() == false) $this->new_feature = false;

         //new or upgraded features are always hidden.
         if($this->story->type_id == $oscailt_basic_config['feature_type_id'] && $this->isNewStory())
         {
            $this->story->hidden=true;
            $this->new_feature=true;
         }
         elseif($this->story->type_id == $oscailt_basic_config['feature_type_id'] && $this->checkstory!=null && $this->checkstory->type_id != $oscailt_basic_config['feature_type_id'])
         {
            $this->story->hidden=true;
            $this->new_feature=true;
         }

         // Figure out if we have to send a notification...
         $send_notification = false;

         //hide if auto_hide is on and the editor doesn't have permission to override it.
         if(($this->isNewStory() && $this->getDisplayOption('auto_hide_new_stories') == 'true') && !($editor_session->isSessionOpen() && $editor_session->editor->allowedWriteAccessTo("overrideautohide", $site_id)))
         {
            $notify_action = "New Submission";
            $this->story->hidden=true;
            if($this->getDisplayOption('force_submissions_email_notifications') == 'true')
            {
               $send_notification = true;
            }
         }
         elseif($this->isNewStory() && $this->getDisplayOption('force_publications_email_notifications') == 'true')
         {
            $notify_action = "New Publication";
            $send_notification = true;
         }

	 // If a new story and an editor is logged in and it is not a feature, then a radio btn appears which
	 // allows them to select whether to set it as visible, pending or hidden. Choice does not appear for features
         if($this->isNewStory() && $editor_session->isSessionOpen() && $this->story->type_id != $oscailt_basic_config['feature_type_id']) {
             // Now if radio btn was set then ... otherwise the default is visible anyhow.
             if (isset($_REQUEST['set_story_mode'])) {
                 if ($_REQUEST['set_story_mode'] == 'pending' ) $this->story->hidden = 2;
		 else if ($_REQUEST['set_story_mode'] == 'hide' ) $this->story->hidden = 1;
             }
         }

         if($this->isNewStory()) $was_new_story = true;

         if(!$this->isNewStory() && ($editor_session->isSessionOpen() || $this->allow_public_edits == true))
         {
            $log_action = true;
            $item_type = "story";
            if($this->story->type_id != $oscailt_basic_config['feature_type_id'])
            {
               if($this->story->hidden && $this->getDisplayOption('force_hidden_email_notifications') == 'true')
                  $send_notification = true;
               elseif(!$this->story->hidden && $this->getDisplayOption('force_email_notifications') == 'true')
                  $send_notification = true;
               elseif(isset($_REQUEST['send_mail']) && $_REQUEST['send_mail']!=null)
                  $send_notification = true;
            }
            else if($this->story->type_id == $oscailt_basic_config['feature_type_id'])
            {
               $item_type = "feature";
               if($this->story->hidden && $this->getDisplayOption('force_feature_email_notifications') == 'true')
                  $send_notification = true;
               elseif(!$this->story->hidden && $this->getDisplayOption('force_email_notifications') == 'true')
                  $send_notification = true;
               elseif(isset($_REQUEST['send_mail']) && $_REQUEST['send_mail']!=null)
                  $send_notification = true;
            }
         }
         $this->story->addslashes();

	 // If user on moderation, then auto hide the story
	 if ($this->user_on_moderation == true) $this->story->hidden=true;

         if(!$requires_approval)
         {
            $this->story->author_email = $this->encodeEmailAddress($this->story->author_email);
            $this->story->save();
         }
      }
      else
      {
         $item_type = "comment";
         $send_notification = false;

         // If the YouTube Id was entered, then add it into the comment content.
         if ($this->getDisplayOption("youtube_id_allow_in_comments") == 'on') {
             $this->logEmbedVideoPublish(false);
         }
         // Log if any embedded audio was entered.
         if ($this->getDisplayOption("audio_id_allow_in_comments") == 'on') {
             $this->logEmbedAudioPublish(false);
         }

         if($this->isNewComment()) $was_new_comment = true;

         if( $this->isNewComment() && $this->getDisplayOption('auto_hide_new_comments') == 'true' && !($editor_session->isSessionOpen() && $editor_session->editor->allowedWriteAccessTo("overrideautohide", $site_id)))
         {
            $notify_action = "New Submission";
            $this->comment->hidden=true;
            if($this->getDisplayOption('force_submissions_email_notifications') == 'true')
            {
               $send_notification = true;
            }
         }
         elseif($this->isNewComment() && $this->getDisplayOption('force_publications_email_notifications') == 'true')
         {
            $notify_action = "New Publication";
            $send_notification = true;
         }
         if( $editor_session->isSessionOpen() && !$this->isNewComment())
         {
            $log_action = true;
            if($this->comment->hidden && $this->getDisplayOption('force_hidden_email_notifications') == 'true')
               $send_notification = true;
            if(!$this->comment->hidden && $this->getDisplayOption('force_email_notifications') == 'true')
               $send_notification = true;
            else if(isset($_REQUEST['send_mail']) && $_REQUEST['send_mail']!=null)
               $send_notification = true;
         }
         $this->comment->addslashes();

	 // If user on moderation, then auto hide the comment
	 if ($this->user_on_moderation == true) $this->comment->hidden=true;

         if(!$requires_approval)
         {
            $this->comment->author_email = $this->encodeEmailAddress($this->comment->author_email);
            $this->comment->save();
         }
      }

      if (!$requires_approval && ($this->user_on_moderation == true OR $this->hide_spam == true)) {
      	// Generate a message to say why story or comment was hidden -user on moderation.
	if($this->content_type == 'story') $tmp_id = $this->story->story_id;
	else $tmp_id = $this->comment->story_id . ", ".$this->comment->comment_id;

        if ($this->hide_spam == true)
      	    logAction(null, $tmp_id, $this->content_type, "hide", "Auto hide on publish as potential spam. Review for approval");
	else
      	    logAction(null, $tmp_id, $this->content_type, "hide", "Auto hide on publish as user on moderation. Review for approval");
      }


      // Saving the attachments...
      for($i=0;$i<count($this->attachments);$i++)
      {
         if($this->content_type == 'comment')
         {
            $this->attachments[$i]->comment_id=$this->comment->comment_id;
            $this->attachments[$i]->story_id=$this->comment->story_id;
         }
         else
         {
            $this->attachments[$i]->story_id=$this->story->story_id;
         }
         if($requires_approval)
         {
            $this->attachments[$i]->hidden=true;
         }
         else
         {
            $this->attachments[$i]->hidden=false;
         }
         $this->attachments[$i]->addslashes();
         $this->attachments[$i]->save();
      }

      // Handle case to remove it during edit where the video type is set to none. Delete by parent ids.
      if(isset($_REQUEST['videoType']) && $_REQUEST['videoType'] == 0)
      {
         if(!isset($_REQUEST['youtube_id_1']) || trim($_REQUEST['youtube_id_1']) == "")
         {
   	    $delete_video = false;
   	    if($this->content_type == 'story' && !$this->isNewStory() ) $delete_video = true;
   	    if($this->content_type == 'comment' && !$this->isNewComment() ) $delete_video = true;
   
   	    if($delete_video == true && $editor_session->isSessionOpen() )
            {
                $attachment = new Attachment();
                $attachment->video=true;
                $attachment->description="";
                $attachment->video = 0;
                $attachment->attachment_file="embedvideo:00:0";
   
                if ($this->content_type == 'story' ) {
                    $attachment->story_id = $this->story->story_id;
                    $attachment->comment_id = 0;
                } else {
                    $attachment->story_id = $this->comment->story_id;
                    $attachment->comment_id = $this->comment->comment_id;
                }
   
                // Added to allow deletion of embedded video during edits.
                $attachment->deleteEmptyEmbeddedVideo();
            }
         }
      }


      //logging section..
      if($requires_approval)
      {
         $this->addToApprovalQueue($item_type);
         $this->user_msg .= "<br>".$this->getDisplayOption("requires_approval");
      }
      elseif($send_notification)
      {
         $this->sendNotification($item_type, false, $notify_action);
      }

      if ($was_new_story == true ) $this->notifyEditorList($item_type);
      if ($was_new_comment == true ) $this->notifyEditorList($item_type);

      if($log_action)
      {
         if($this->content_type == 'comment' )
         {
            logAction($this->comment,$this->comment->comment_id,"comment","edit");
         }
         else
         {
            logAction($this->story,$this->story->story_id,$item_type,"edit");
         }
      }
      //invalidate the token..

      $this->checkPublishSecurity(true);
      $this->relinquishLocks();
      return !$requires_approval;
   }

   function sendNotification($item_type, $is_proposal, $action_name = "Edit")
   {
      global $editor_session, $system_config;
      if($is_proposal) $action = "Proposed Action: ";
      else $action = "";
      $action .= $action_name .' ';
      if($item_type == 'comment' )
      {
         $action .= "Comment ".$this->comment->comment_id;
         $this->comment->stripslashes();
         $body = "Link: http://".$system_config->site_url."/".$this->getCommentLink()."\r\n";
         $body .= $this->comment->toString(false, !$is_proposal);
      }
      else
      {
         $action .= ucfirst($item_type)." ".$this->story->story_id;
         $this->story->stripslashes();
         $body = "Link: http://".$system_config->site_url."/".$this->getStoryLink()."\r\n";
         $body .= $this->story->toString(!$is_proposal);
      }
      if($editor_session->isSessionOpen())
      {
         $action .= " Performed by ".$editor_session->editor->editor_name;
      }
      elseif($item_type == 'comment')
      {
         $action .= " Performed by ".$this->comment->author_name;
      }
      else
      {
         $action .= " Performed by ".$this->story->author_name;
      }
      $this->sendMail($action, $body);
   }

   // Just send out story titles of new stories to editors that have selected the option
   function notifyEditorList($item_type)
   {
      global $system_config, $oscailt_page_setting;

      // Get list that have opted in.
      $EditorList = new EditorList();
      $notifyList = $EditorList->getNotifyList();
      if ($notifyList == null ) return;
      if (count($notifyList) == 0 ) return;

      if($item_type == 'comment' ) 
      {
          $subject = "New comment: " .$this->comment->comment_title;
          $message = "Date: ".date("l, M j Y, g:ia",time()+$system_config->timezone_offset)."\r\n";
          $message .= "Comment published. Title: " .$this->comment->comment_title ."\r\n";
          $message .= "By author: " .$this->comment->author_name ."\r\n";
          $message .= "\r\n";
          $message .= "Link: http://".$system_config->site_url."/".$this->getCommentLink()."\r\n";
      } else {
          // $this->story->stripslashes();
          $subject = "New story: " .$this->story->story_title;
          $message = "Date: ".date("l, M j Y, g:ia",time()+$system_config->timezone_offset)."\r\n";
          $message .= "Story published. Title: " .$this->story->story_title ."\r\n";
          $message .= "By author: " .$this->story->author_name ."\r\n";
          $message .= "\r\n";
          $message .= "Link: http://".$system_config->site_url."/".$this->getStoryLink()."\r\n";
      }
      $message .= "\r\n";
      $message .= "Sent by ".$system_config->software_name." ".$system_config->software_version." Automatic Notification"."\r\n";

      $mail_set = $oscailt_page_setting["site_mail_settings"];
      $from = $mail_set['from'];
      $reply = $mail_set['replyto'];

      foreach ($notifyList as $each_receipient_array)
      {
          $each_receipient = $each_receipient_array[0];
	  // Options
          $t_s0 = $each_receipient_array[1];
          $t_s1 = $each_receipient_array[2];
          $t_s2 = $each_receipient_array[3];
	  //echo "email=".$each_receipient." st=".$t_s." sum=".$t_s1." con=".$t_s2."<BR>";

	  $email_content = $message;
	  if ($item_type == 'comment') {
	      if ($each_receipient_array[3] == true) $email_content .= "\nContent: \n".$this->comment->comment_content;
	      else continue;
	  } else {
	      if ($t_s1 == true) $email_content .= "\nSubtitle: \n".$this->story->story_subtitle;
	      if ($t_s1 == true) $email_content .= "\nSummary: \n".$this->story->story_summary;
	      if ($t_s2 == true) $email_content .= "\n\nContent: \n".$this->story->story_content;
	  }

          $to = trim($each_receipient);
          mail($to, $subject, $email_content, "From: ".$from."\r\n"."Reply-To: ".$reply."\r\n"."X-Mailer: ".$system_config->software_name."/".$system_config->software_version." using PHP/".phpversion());
          // $this->xxmail($to, $subject, $message, "From: ".$from."\r\n"."Reply-To: ".$reply."\r\n"."X-Mailer: ".$system_config->software_name."/".$system_config->software_version." using PHP/".phpversion());
      }
   }

   function xxmail($t_to, $t_s, $t_m, $t_f)
   {
      echo "<BR><BR>";
      echo "<BR>".$t_to;
      echo "<BR>".$t_s;
      echo "<BR>".$t_m;
      echo "<BR>".$t_f;
      echo "<BR><BR>";
   }

   function sendMail($action_str, &$body)
   {
      global $system_config, $editor_session, $oscailt_page_setting;
      $mail_set = $oscailt_page_setting["site_mail_settings"];

      $to = $mail_set['to'];

      // Don't send if nobody to send to.
      if (trim($to) == "") return;

      if(isset($_REQUEST['action_reason']))
      {
         if(trim($_REQUEST['action_reason']) == "") $reason = "<not specified>";
	 else $reason = $_REQUEST['action_reason'];
      }
      else
      {
         $reason = "<not specified>";
      }

      $from = $mail_set['from'];
      $reply = $mail_set['replyto'];
      $sub_prefix = $mail_set['subject_prefix'];
      $subject = $sub_prefix." ".$action_str;
      $message = $system_config->software_name." ".$system_config->software_version." Automatic Notification"."\r\n";
      $message .= "Date   : ".date("l, M j Y, g:ia",time()+$system_config->timezone_offset)."\r\n";
      $message .= "Action   : ".$action_str."\r\n";
      $message .= "Reason   : ".$reason."\r\n";
      $message .= "\r\n";
      $message .= $body;
      if($editor_session->isSessionOpen()) $reply = $editor_session->editor->editor_email.",".$reply;
      if( $system_config->debug_level >= DEBUG_LEVEL_1 )
      {
         echo("<BR>Email Notifications disabled while debug_level is >= ".DEBUG_LEVEL_1."<BR>");
         echo("<BR>To: $to<BR>");
         echo("<BR>From: $from<BR>");
         echo("<BR>Replyto: $reply<BR>");
         echo("<BR>Subject: $subject<BR>");
         echo("<BR><pre>".htmlspecialchars($message)."</pre><BR>");
      }
      else
      {
         mail($to, $subject, $message, "From: ".$from."\r\n"."Reply-To: ".$reply."\r\n"."X-Mailer: ".$system_config->software_name."/".$system_config->software_version." using PHP/".phpversion());
      }

   }

   function addToApprovalQueue($item_type)
   {
      $this->sendNotification($item_type, true);
   }

   function addPublishEventToMonitor()
   {
      global $editor_session;
      if($this->content_type == 'comment')
      {
         $lnk = $this->getCommentLink();
         $tag = "<a href='$lnk'>".$this->comment->comment_title."</a>";
         $id_tag = " (".$this->comment->story_id.",".$this->comment->comment_id.")";
         $nm = $this->comment->author_name;
      }
      else
      {
         $lnk = $this->getStoryLink();
         $tag = "<a href='$lnk'>".$this->story->story_title."</a>";
         $id_tag = " (".$this->story->story_id.")";
         $nm = $this->story->author_name;
      }
      if($editor_session->isSessionOpen())
      {
         $nm = $editor_session->editor->editor_name." <small class='error'>editor</small>";
      }
      $str = "$nm published ".ucfirst($this->content_type).": $tag" . $id_tag;
      $monitorList = new PublishMonitor();
      $monitorList->load();
      $monitorList->add($str);
      $monitorList->save();
   }

   function processRelatedLink(&$item)
   {
      // if link has no URL prefix then prepend a HTTP URL prefix
      if(strlen($item->related_link)>0 && stristr($item->related_link,"://")==false && stristr($item->related_link,"mailto:")==false)
      {
         $item->related_link="http://".$item->related_link;
      }
   }


   /*********************************************************************/
   /* Functions to get links to other modules                      */
   /*********************************************************************/
   function getCommentLink()
   {
      if($this->content_type != 'comment')
      {
         return "";
      }
      $save_query = $this->query->copy();
      $this->query = new StoryQuery();
      $this->query->story_id = $this->comment->story_id;
      $lnk = $this->getFilterLink($this->site_settings['articleobject']);
      $lnk = addLinkParameters($lnk, array("comment_limit=0", "condense_comments=false"));
      $lnk .= "#comment".$this->comment->comment_id;
      $this->query = $save_query;
      return $lnk;
   }

   function getFeatureLink()
   {
      if($this->content_type != 'story')
      {
         return "";
      }
      $lnk = $this->getSimpleLink($this->site_settings['featureobject']);
      return $lnk;
   }

   function getEventCalendarLink()
   {
      if($this->content_type != 'story')
      {
         return "";
      }
      $lnk = $this->getSimpleLink($this->site_settings['eventsobject']);
      return $lnk;
   }

   function getNewswireLink()
   {
      if($this->content_type != 'story')
      {
         return "";
      }
      $lnk = $this->getSimpleLink($this->site_settings['newswireobject']);
      return $lnk;
   }

   function getStoryLink($story_id=false)
   {
      if($story_id==false && $this->content_type != 'story')
      {
         return "";
      }
      elseif($story_id == false)
      {
         return parent::getStoryLink($this->story->story_id);
      }
      else
      {
         return parent::getStoryLink($story_id);
      }
   }

   /**********************************************************************
   //Functions to do the main legwork of drawing the various pages
   /*********************************************************************/
   function writePublishPage()
   {
      global $system_config,$editor_session;
      $this->writePublishHeader();
      $this->writePageIntro();
      $this->showLock();
      $this->writePublishForm();
      if($editor_session->isSessionOpen())
      {
         $editor_session->clipboard->writeClipboard($this->query);
      }
   }

   function writePublishHeader()
   {
      $this->writeDisplayHeader();
   }

   function writePageIntro()
   {
      global $system_config;
      if($this->content_type == 'comment') return $this->writeCommentPageIntro();

      if($this->isNewStory())   $act_txt = $this->getDisplayOption("new_text");
      else $act_txt = $this->getDisplayOption("edit_text");
      if($this->story->type_id==$system_config->event_type_id) $act_txt .= " ".$this->getDisplayOption("event_text");
      else $act_txt .= " ".$this->getDisplayOption("story_text");
      if(!$this->isNewStory())
      {
         $act_txt .= " ".$this->story->story_id;
      }
      echo "<div class='publish-header'>\n";
      echo "<h1 class='publish-header'>".$act_txt."</h1>\n";

      if($this->user_msg != "")
      {
         echo "<p class='user-message'>".$this->user_msg."</p>\n";
      }
      elseif($this->isNewStory())
      {
         echo "<p class='publish-header'>".$this->getDisplayOption('new_intro')."</p>\n";
      }
      else
      {
         echo "<p class='publish-header'>".$this->getDisplayOption('edit_intro')."</p>\n";
      }
      echo "</div>\n";
      //write lock info here...
   }

   function writeCommentPageIntro()
   {
      global $system_config;
      if($this->isNewComment())   $act_txt = $this->getDisplayOption("new_text");
      else $act_txt = $this->getDisplayOption("edit_text");
      $act_txt .= " ".$this->getDisplayOption("comment_text");
      $act_txt .= " <small>(".$this->getDisplayOption("story_text")." ".$this->comment->story_id.")</small>";
      echo "<div class='publish-header'>\n";
      echo "<h1 class='publish-header'>".$act_txt."</h1>\n";

      if($this->user_msg != "")
      {
         echo "<p class='user-message'>".$this->user_msg."</p>\n";
      }
      if($this->isNewComment())
      {
         echo "<p class='publish-header'>".$this->getDisplayOption('newcomment_intro')."</p>\n";
      }
      echo "</div>\n";
   }


   function writePublishForm($is_preview=false)
   {
      global $editor_session;

      $max_embed_video_entry = 0;
      $max_embed_audio_entry = 0;

      if($this->content_type == 'story')
      {
         $this->story->unformat();
         if ($this->getDisplayOption("youtube_id_allow_in_stories") == 'on')
            $max_embed_video_entry = $this->getDisplayOption("max_embedvideo_in_stories");

         if ($this->getDisplayOption("audio_id_allow_in_stories") == 'on')
            $max_embed_audio_entry = $this->getDisplayOption("max_embedaudio_in_stories");
      }
      else
      {
         $this->comment->unformat();
         if ($this->getDisplayOption("youtube_id_allow_in_comments") == 'on')
            $max_embed_video_entry = $this->getDisplayOption("max_embedvideo_in_comments");

         if ($this->getDisplayOption("audio_id_allow_in_comments") == 'on')
            $max_embed_audio_entry = $this->getDisplayOption("max_embedaudio_in_comments");
      }

      $this->writeJSFunctions($max_embed_video_entry, $max_embed_audio_entry);

      $this->writeFormHeader($is_preview);
      $section_num = 1;
      //if($this->allow_public_edits != true)
         $section_num = $this->writeAttachmentsSection($section_num);

      $section_num = $this->writeContentSection($section_num);

      if($this->content_type == 'story' && $this->allow_public_edits != true)
         $section_num = $this->writeCategorisationSection($section_num);

      $section_num = $this->writeExtraContactDetailsSection($section_num);
      $section_num = $this->writeLegalSection($section_num);
      $section_num = $this->writeEditorReasonSection($section_num);
      $this->writeSubmitSection($section_num);
      $this->writeFormFooter($is_preview);
   }

   // Note any changes to the form names will affect the code in the javascript files and these would
   // have to be upated to match.
   function writeJSFunctions($max_video_entries, $max_audio_entries)
   {
      global $system_config;
      writeHelpJS();

      ?>
      <script type="text/javascript" language="Javascript">
      <?
         if ($max_video_entries > 0)
         {
            $disable_true_str = "";
            $disable_false_str = "";
            for ($k=1; $k <= $max_video_entries; $k++) {
               $disable_true_str .= "document.publishform.youtube_id_".$k.".disabled=true;" ."\n";
               $disable_true_str .= "document.publishform.youtube_id_".$k.".style.display='none';"."\n";

               $disable_true_str .= "document.publishform.embed_vid_desc_".$k.".disabled=true;" ."\n";
               $disable_true_str .= "document.publishform.embed_vid_desc_".$k.".style.display='none';"."\n";
               $disable_true_str .= "document.getElementById('embv_".$k."').style.display='none';"."\n";
            }
	    // Back to generating Javascript function ...
            ?>
            function checkForVideoType()
            {
               <?=$disable_true_str?>
               if(document.publishform.videoType.value > 0)
               {
                  hideEmbeddedVideoForm();
               }
            }
	    <?
         }

         // Now do the same for audio
         if ($max_audio_entries > 0)
         {
            $disable_true_str = "";
            $disable_false_str = "";
            for ($k=1; $k <= $max_audio_entries; $k++) {
               $disable_true_str .= "document.publishform.audio_id_".$k.".disabled=true;" ."\n";
               $disable_true_str .= "document.publishform.audio_id_".$k.".style.display='none';"."\n";
               $disable_true_str .= "document.publishform.embed_audio_desc_".$k.".disabled=true;" ."\n";
               $disable_true_str .= "document.publishform.embed_audio_desc_".$k.".style.display='none';"."\n";
               $disable_true_str .= "document.getElementById('emba_".$k."').style.display='none';"."\n";
            }
	    // Back to generating Javascript function ...
            ?>
            function checkForAudioCount()
            {
               <?=$disable_true_str?>
               hideEmbeddedAudioForm();
            }
	    <?
         }
         ?>
      </script>
      <?
   }

   function writeFormHeader($is_preview = false)
   {
      if($is_preview)
      {
         echo "<DIV style='clear:both; margin-top: 10px;'>&nbsp;</DIV>";
         echo '<FORM name="previewform" enctype="multipart/form-data" action="';
         $lnk = $this->getSimpleLink();
         if($this->content_type == 'comment')
         {
            $lnk = addLinkParameters($lnk, "section=addcomment");
         }
         echo $lnk. '" method="post">';

         $this->writeItemAsHidden();
         $this->writeConfirmPreviewSection();
         echo "</form>";
      }

      echo '<FORM name="publishform" enctype="multipart/form-data" action="';

      $lnk = $this->getSimpleLink();
      if($this->content_type == 'comment')
      {
         $lnk = addLinkParameters($lnk, "section=addcomment");
      }
      echo $lnk.'" method="post">';

      $formValidationData = new formValidator();
      if (( $is_preview) || ($_SERVER['REQUEST_METHOD'] == "POST" && !isset($_REQUEST['preview_confirmed']) && !isset($_REQUEST['publish_button'])) || $this->field_validation_error == true ) {

          $hiddenData = $formValidationData->rewriteHiddenData();
      } else {
          $hiddenData = $formValidationData->getHiddenData();
      }
      echo $hiddenData;


      if($this->content_type == 'comment')
      {
         echo '<INPUT type=hidden name=story_id value="'.$this->comment->story_id.'">'."\n";
         if($this->comment->comment_id!=null && $this->comment->comment_id != 0)
         {
            echo '<INPUT type=hidden name="comment_id" value="'.$this->comment->comment_id.'">'."\n";
         }
      }
      else
      {
         if($this->story->story_id !=null && $this->story->story_id !=0)
         {
            echo '<INPUT type=hidden name="story_id" value="'.$this->story->story_id.'">'."\n";
         }
      }
      echo '<TABLE class="publish" border=0><TBODY>';
   }

   /*********************************************************************/
   /* General purpose form section drawing functions                */
   /*********************************************************************/
   function writeFormSection($num, $title, $type)
   {
      echo "<div class='publish-section'>\n";
      echo "<div class='publish-section-header'>\n";
      $this->writeFormSectionHeader($num, $title);
   }

   function writeFormFooter($is_preview)
   {
      echo "</TBODY>\n</TABLE>\n</FORM>";
      if($is_preview) echo "</DIV>";
   }

   function writeSectionHeader($num, $str, $intro="")
   {
      echo "<tr class='publish'><th class='publish' colspan='2'>Step $num: $str</th></tr>\n";
      if($intro != "")
      echo "<tr class='publish'><td class='publish-intro' colspan='2'>$intro</td></tr>\n";
   }

   function writeSectionFooter($str)
   {
      echo "<tr class='publish'><td class='publish' colspan='2'>$str</td></tr>\n";
   }

   function writeSectionEntry($f_prefix, $payload, $is_req, $special_options=false, $title_suffix = "")
   {
      $title = $this->getDisplayOption($f_prefix.'_title').$title_suffix;
      $title = $this->wrapTitle($title, $is_req);
      if($special_options != "")
      {
         $payload = "<div class='publish-content-rules'>$special_options</div>".$payload;
      }
      $prompt = $this->getDisplayOption($f_prefix.'_prompt');
      $help_text = $this->getDisplayOption($f_prefix.'_help');
      $this->writeFormInputField($title, $prompt, $payload, $help_text);
   }

   function writeEmbedVideoEntry($f_prefix, $select_payload, $textbox_payload, $is_story)
   {
      $title = $this->getDisplayOption($f_prefix.'_title');
      $title = $this->wrapTitle($title, false);

      $prompt = trim($this->getDisplayOption($f_prefix.'_prompt'));
      $help_text = $this->getDisplayOption($f_prefix.'_help');
      // $this->writeFormInputField($title, $prompt, $select_payload, $help_text);
      // $this->writeFormInputField($title, $prompt, $textbox_payload, $help_text);

      if($prompt != "")
      {
         $prompt = "<div class='publish-prompt'>$prompt ". getHelpHTML($help_text, false, true)."</div>";
      }
      echo "<tr class='publish'><td class='publish-title'>$title</td>\n";
      echo "<td class='publish-contents'>$prompt $select_payload ";
      echo "<p>$textbox_payload";
      echo "</td>\n</tr>\n";

   }

   function wrapTitle($txt, $is_req, $special_options=false)
   {
      $txt = "<b>$txt</b>";
      if($is_req) $txt .= "<br /><span class='publish-require'>".$this->getDisplayOption('required_text')."</span>";
      else $txt .= "";
      if($special_options)
      {
         $txt.= "<br />$special_options";
      }
      return $txt;
   }

   function writeFormInputField($title, $prompt, $payload, $help)
   {
      $prompt = trim($prompt);
      if($prompt != "")
      {
         $prompt = "<div class='publish-prompt'>$prompt ". getHelpHTML($help, false, true)."</div>";
      }
      echo "<tr class='publish'><td class='publish-title'>$title</td>\n";
      echo "<td class='publish-contents'>$prompt$payload";
      echo "</td>\n</tr>\n";
   }


   /*********************************************************************/
   /* specific section drawing functions                          */
   /*********************************************************************/
   function writeEditorReasonSection($snum)
   {
      global $system_config, $oscailt_basic_config, $editor_session;

      if(!$editor_session->isSessionOpen() or ($this->content_type=='story' && $this->isNewStory()) or ($this->content_type=='comment' && $this->isNewComment())) return $snum;

      $hed_text = $this->getDisplayOption('editorreason_sectionhead_text');
      $this->writeSectionHeader($snum, $hed_text);
      $action_reason = (isset($_REQUEST['action_reason'])) ? $_REQUEST['action_reason'] : "";
      $tp_payload = "<TEXTAREA name=action_reason rows=5 cols=50>".htmlspecialchars($action_reason)."</TEXTAREA>";
      $this->writeSectionEntry('actionreason', $tp_payload, true);

      $t_feature_id = $oscailt_basic_config['feature_type_id'];

      if( ($this->content_type=='story' &&
            ($this->story->type_id != $t_feature_id && $this->getDisplayOption('force_email_notifications') == 'true' && $this->story->hidden != true)
         || ($this->story->type_id == $t_feature_id && $this->story->hidden == false)
         || ($this->story->type_id == $t_feature_id && $this->getDisplayOption('force_feature_email_notifications') == 'true'))
         ||
         ($this->content_type=='comment' && $this->getDisplayOption('force_email_notifications') == 'true')
         )
      {
         $tp_payload = "<input type='checkbox' name='dummy' checked DISABLED>";
         $tp_payload .= "<input type='hidden' name='send_mail' value='on'>";
      }
      else
      {
         $tp_payload = "<input type='checkbox' name='send_mail'>";
      }
      $this->writeSectionEntry('send_notification', $tp_payload, true);
      return ++$snum;

   }

   function writeLegalSection($snum)
   {
      global $system_config, $editor_session;
      if($this->getDisplayOption("include_legal_section") != 'true') return $snum;
      $hed_text = $this->getDisplayOption('legal_sectionhead_text'). " - <span class='publish-require'>".$this->getDisplayOption('required_text')."</span>";
      $intro_text = $this->getDisplayOption('legal_section_intro');
      $this->writeSectionHeader($snum, $hed_text, $intro_text);
      $tp_payload = "<INPUT type=checkbox name='accept_terms' ";
      if(((($this->content_type == 'story' && $this->isNewStory()) or ($this->content_type == 'comment' && $this->isNewComment())) && $this->isNewPublication() or $this->unset_t_and_c) and (!isset($_REQUEST['accept_terms'])))
      {
         $tp_payload .= ">\n";
      }
      else
      {
         $tp_payload .= "checked>\n";
      }
      $this->writeSectionEntry('termsandconditions', $tp_payload, true);
      return ++$snum;
   }

   function writeExtraContactDetailsSection($snum)
   {
      global $system_config, $editor_session;
      if($this->getDisplayOption("include_extra_details") != 'true') return $snum;
      $hed_text = $this->getDisplayOption('contact_sectionhead_text'). " (".$this->getDisplayOption('notrequired_text').")";
      $intro_text = $this->getDisplayOption('contact_section_intro');
      $this->writeSectionHeader($snum, $hed_text, $intro_text);

      if($this->content_type == 'comment') $content =& $this->comment;
      else $content =& $this->story;

      // Generate tick box for privacy

      $tp_payload = "<INPUT maxLength=80 size=30 name='author_email' value='";
      $tp_payload .= htmlspecialchars($this->getEmailAddress($content->author_email), ENT_QUOTES)."'>\n";
      $tp_payload .= " &nbsp;<b>Do not display</b>&nbsp; ";

      $tp_payload .= "<INPUT type='checkbox' name='email_status' ";
      if (substr($content->author_email,0,5) == "hide:" OR (isset($_REQUEST['email_status']) && $_REQUEST['email_status'] == 'on')) 
          $tp_payload .= " checked";
      $tp_payload .= ">";
      $this->writeSectionEntry('authoremail', $tp_payload, false);

      $tp_payload = "<INPUT maxLength=60 size=30 name='author_phone' value='";
      $tp_payload .= htmlspecialchars($content->author_phone, ENT_QUOTES)."'>\n";
      $this->writeSectionEntry('authorphone', $tp_payload, false);

      $tp_payload = "<INPUT maxLength=120 size=50 name='author_address' value='";
      $tp_payload .= htmlspecialchars($content->author_address, ENT_QUOTES)."'>\n";
      $this->writeSectionEntry('authoraddress', $tp_payload, false);

      return ++$snum;
   }

   // If the user has ticked to not display the email address, then 'hide:' prefix needs to be added.
   function encodeEmailAddress($email_address)
   {
         if(isset($_REQUEST['email_status']) && $_REQUEST['email_status'] == 'on' && substr($email_address,0,5) != 'hide:') 
             $email_address = 'hide:'. trim($email_address);

         return trim($email_address);
   }

   function getEmailAddress($email_address)
   {
         if(substr($email_address,0,5) == 'hide:') return substr($email_address,5);
         return trim($email_address);
   }

   function writeFilterSection($tit, $payload)
   {
      $tit = trim($tit);
      if($tit != "")
      {
         $tit = "<div class='publish-filterprompt'>$tit</div>";
      }
      echo "<div class='publish-filter'>";
      echo "$tit $payload";
      echo "</div>";
   }

   function writeTLRSelects()
   {
      $type_title = $this->getDisplayOption('type_title');
      $type_help = $this->getDisplayOption('type_help');
      $type_prompt = $this->getDisplayOption('type_prompt');
      $type_select_title = $this->getDisplayOption('type_select_title');
      $toptitle = $this->getDisplayOption('topic_title');
      $top_prompt = $this->getDisplayOption('topic_prompt');
      $top_select_title = $this->getDisplayOption('topic_select_title');
      $top_help = $this->getDisplayOption('topic_help');
      $rtitle = $this->getDisplayOption('region_title');
      $rprompt = $this->getDisplayOption('region_prompt');
      $rsel_title = $this->getDisplayOption('region_select_title');
      $rhelp = $this->getDisplayOption('region_help');
      $ltitle = $this->getDisplayOption('language_title');
      $lsel_title = $this->getDisplayOption('language_select_title');
      $lprompt = $this->getDisplayOption('language_prompt');
      $lhelp = $this->getDisplayOption('language_help');
      $type_payload = $this->getTypeSelect($type_select_title). "<br />".$this->getEventDateSelect();
      $topic_payload = $this->getTopicSelect($top_select_title);
      $region_payload = $this->getRegionSelect($rsel_title);
      $lang_payload = $this->getLanguageSelect($lsel_title);
      $cls = "class='publish-section'";

      echo "<tr class='publish'>";
      echo "<td class='publish-contents' colspan='2'>";
      echo "<table $cls>";
      echo "<tr $cls>";
      echo "<th $cls>$type_title";
      echo "</th>";
      echo "<th $cls>$rtitle";
      echo "</th>";
      echo "</tr>";
      echo "<tr $cls>";
      echo "<td $cls><small>".$type_prompt."</small> ".getHelpHTML($type_help, false, false)."<br />$type_payload";
      echo "</td>";
      echo "<td $cls><small>".$rprompt."</small> ".getHelpHTML($rhelp, false, false)."<br />".$region_payload;
      echo "</td>";
      echo "</tr>";
      echo "<tr $cls>";
      echo "<th $cls>$ltitle";
      echo "</th>";
      echo "<th $cls>$toptitle";
      echo "</th>";
      echo "</tr>";
      echo "<tr $cls>";
      echo "<td $cls><small>".$lprompt."</small> ".getHelpHTML($lhelp, false, false)."<br />".$lang_payload;
      echo "</td>";
      echo "<td $cls><small>".$top_prompt."</small> ".getHelpHTML($top_help, false, false)."<br />$topic_payload";
      echo "</td>";
      echo "</tr>";
      echo "</table>";
      echo "</td></tr>\n";
   }


   function generateGenericSelect($start_val, $stop_val, $selected_val)
   {
         $str = "";
         for ($i=$start_val; $i<=$stop_val; $i++)
         {
            $str .= "<option ";
            if($i==$selected_val) $str .= "selected ";
            $str.= ' value="'.$i.'">'.$i.'</option>';
         }

	 return $str;
   }
   function getEventDateSelect()
   {
      global $system_config, $editor_session, $pageType;
      if($this->story->event_time > 0) $default_date = $this->story->event_time+$system_config->timezone_offset;
      else $default_date = time()+$system_config->timezone_offset;

      if($this->story->type_id ==$system_config->event_type_id or (is_object($pageType) && $pageType->type_id == $system_config->event_type_id))
      {
         $hide = false;
      }
      else
      {
         $hide = true;
      }

      $selected_min = date("i",$default_date);
      $selected_hr  = date("H",$default_date);
      $selected_day = date("j",$default_date);
      $selected_month = date("n",$default_date);
      $selected_year = date("Y",$default_date);
      $current_year = date("Y",time()+$system_config->timezone_offset);
      $str = '<select name="event_time_day"';
      if($hide) $str .= ' DISABLED style="display:none"';
      $str .= ">\n";

      $str .= $this->generateGenericSelect(1, 31, $selected_day);
      $str .= "</select>\n";

      $str .= '<select name="event_time_month" ';
      if($hide) $str .= ' DISABLED style="display:none"';
      $str .= ">\n";

      // Make it day 2 in mktime, to get over any complications with timezone etc
      for ($i=1; $i<=12; $i++)
      {
         $str .= "<option ";
         if($i==$selected_month) $str .= "selected ";
         $str .= "value='$i'>". strftime("%B", mktime (0,0,0,$i,2,$selected_year))."</option>\n";
      }
      $str .= "</select>\n";

      $str .= '<select name="event_time_year" ';
      if($hide) $str .= ' DISABLED style="display:none"';
      $str .= ">\n";

      if($editor_session->isSessionOpen() && $selected_year>=$current_year)
      {
         $str .= $this->generateGenericSelect( ($current_year-5), ($selected_year+5), $selected_year);
      }
      else if($editor_session->isSessionOpen() && $selected_year < $current_year)
      {
            $str .= "<option ";
            if($i==$selected_year) $str .= "selected ";
            $str.= ' value="'.$i.'">'.$i.'</option>';
      }
      else
      {
         $str .= $this->generateGenericSelect( $current_year, ($current_year+2), $selected_year);
      }
      $str .= "</select>\n";
      $str .= "&nbsp;";

      // Add in the start time: hour and minutes. Make 00:00 the default and if not changed then ignore
      $start_time = '<select name="event_time_hr" ';
      if($hide) $start_time .= ' DISABLED style="display:none"';
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
      if($hide) $start_time .= ' DISABLED style="display:none"';
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

   function writeCategorisationSection($snum)
   {
      global $system_config, $editor_session;
      $hed_text = $this->getDisplayOption('category_sectionhead_text'). " - <span class='publish-require'>".$this->getDisplayOption('required_text')."</span>";
      $intro_text = $this->getDisplayOption("category_section_intro");
      $this->writeSectionHeader($snum, $hed_text, $intro_text);
      $this->writeTLRSelects();
      return ++$snum;
   }

   function writeTypeSelects()
   {
      $f_title = $this->getDisplayOption('type_select_title');
      echo "<tr class='publish'><td class='publish'>$f_title</td>\n";
      echo "<td class='publish' colspan=2>";

      $type_title = $this->getDisplayOption('type_title');
      $type_help = $this->getDisplayOption('topic_help');
      echo $this->getTypeSelect($type_title). " ".$this->getEventDateSelect();
   }

   function getLanguageSelect($tit)
   {
      global $editor_session, $languageList, $pageLanguage;
      $str = '<select name="language_id">';
      $all_languages = $languageList->getfLanguages();
      $lang_options = array();
      foreach($all_languages as $language)
      {
         if(($language->active==true || $editor_session->isSessionOpen()) and (count($this->language_id) == 0 or in_array($language->language_id, $this->language_id)))
         {
            if($language->language_id==$this->story->language_id or (is_object($pageLanguage) && $language->language_id == $pageLanguage->language_id))
            {
               $lang_options[] = "<option selected value='".$language->language_id."'>".$language->language_name."</option>";
            }
            else
            {
               $lang_options[] = "<option value='".$language->language_id."'>".$language->language_name."</option>";
            }
         }
      }
      if(count($lang_options) > 1)
      {
         $str .= "<option>$tit</option>\n";
      }
      $str .= implode("\n", $lang_options)."</select>";
      return $str;
   }

   function getRegionSelect($tit)
   {
      global $editor_session, $regionList, $pageRegion, $userLanguage, $languageList;
      $str = '<select name="region_id">';
      if (isset($userLanguage)) {
          $all_regions = $regionList->getfRegions($userLanguage->language_id);
      } else {
          $all_regions = $regionList->getfRegions($languageList->getMinLanguageId());
      }

      $r_options = array();

      foreach($all_regions as $region)
      {
         if(($region->active==true || $editor_session->isSessionOpen()) and (count($this->region_id) == 0 or in_array($region->region_id, $this->region_id)))
         {
            if($region->region_id==$this->story->region_id or (is_object($pageRegion) && $region->region_id == $pageRegion->region_id))
            {
               $r_options[] = "<option selected value='".$region->region_id."'>".$region->region_name."</option>";
            }
            else
            {
               $r_options[] = "<option value='".$region->region_id."'>".$region->region_name."</option>";
            }
         }
      }
      if(count($r_options) > 1)
      {
         $str .= "<option>$tit</option>\n";
      }
      $str .= implode("\n", $r_options);
      $str.="</select>\n";
      return $str;
   }


   function getTopicSelect($tit)
   {
      global $editor_session, $topicList, $pageTopic, $userLanguage, $languageList;
      $str = '<select name="topic_id">';
      if (isset($userLanguage)) {
          $all_topics = $topicList->getfTopics($userLanguage->language_id);
      } else {
          $all_topics = $topicList->getfTopics($languageList->getMinLanguageId());
      }
      $t_options = array();
      foreach($all_topics as $topic)
      {
         if(($topic->active==true || $editor_session->isSessionOpen()) and (count($this->topic_id) == 0 or in_array($topic->topic_id, $this->topic_id)))
         {
            if($topic->topic_id==$this->story->topic_id or (is_object($pageTopic) && $topic->topic_id == $pageTopic->topic_id))
            {
               $t_options[] = "<option selected value='".$topic->topic_id."'>".$topic->topic_name."</option>";
            }
            else
            {
               $t_options[] = "<option value='".$topic->topic_id."'>".$topic->topic_name."</option>";
            }
         }

      }
      if(count($t_options) > 1)
      {
         $str .= "<option>$tit</option>\n";
      }
      $str .= implode("\n", $t_options);
      $str.="</select>\n";
      return $str;
   }


   function getTypeSelect($tit)
   {
      global $editor_session, $typeList, $pageType, $system_config, $oscailt_page_setting, $userLanguage, $languageList;
      $site_id = $oscailt_page_setting['site_id'];
      $str = '<select name="type_id" onchange="JavaScript:checkForEventType('.$system_config->event_type_id.')">';
      if (isset($userLanguage)) {
          $all_types = $typeList->getfTypes($userLanguage->language_id);
      } else {
          $all_types = $typeList->getfTypes($languageList->getMinLanguageId());
      }

      $t_options = array();
      foreach($all_types as $type)
      {
         if(($type->active==true || $editor_session->isSessionOpen()) and (count($this->type_id) == 0 or in_array($type->type_id, $this->type_id)))
         {
            if(($type->type_id == $system_config->feature_type_id)&& !($editor_session->isSessionOpen() and $editor_session->editor->allowedReadAccessTo("editfeature", $site_id)))
            {
                  continue;
            }
            elseif($type->type_id==$this->story->type_id or (is_object($pageType) && $type->type_id == $pageType->type_id))
            {
               $t_options[] = "<option selected value='".$type->type_id."'>".$type->type_name."</option>";
            }
            else
            {
               $t_options[] = "<option value='".$type->type_id."'>".$type->type_name."</option>";
            }
         }
      }
      if(count($t_options) > 1)
      {
         $str .= "<option>$tit</option>\n";
      }
      $str .= implode("\n", $t_options);
      $str.="</select>\n";
      return $str;
   }

   function getEmbedVideoSelect($title)
   {
      // Removed Stage 6 video as it doesn't use flash and would mean changing code.
      // Removed Ifilm too as it is just film stuff. 
      // Dailymotion probably of no interest either so removed that too.
      // Others probably not worth it are: UnCut, Break, Metacafe
      $str = '<select name="videoType" onchange="JavaScript:checkForVideoType()">';

      // It is the 2nd parameter really that says return the full array. See videos.inc
      // It returns an array of video types and indexes.
      $allTypes = getEmbeddedVideoTypes(0, true);

      $t_options = array();
      $pre_selected = -1;

      if (isset($_REQUEST['videoType'])) {
          $pre_selected = $_REQUEST['videoType'];
      }
      foreach($allTypes as $videoType => $VideoName)
      {
         $t_opt_str = "<option value='".$videoType;

         if ($pre_selected > -1 && $videoType == $pre_selected) {
             $t_opt_str .= "' selected >".$allTypes[$videoType]."</option>";
         } else {
             $t_opt_str .= "'>".$allTypes[$videoType]."</option>";
         }
         $t_options[] = $t_opt_str;
      }
      if(count($t_options) > 1)
      {
         $str .= "<option value='0'>$title</option>\n";
      }
      $str .= implode("\n", $t_options);
      $str.="</select>\n";
      return $str;
   }

   function getNumberOfEmbedVideo($min_value, $max_value)
   {
      // Generate a drop down list for selecting the number of embedded videos that will be added.
      $pre_selected = 0;
      if (isset($_REQUEST['number_embed_video'])) {
          $pre_selected = $_REQUEST['number_embed_video'];
          $this->number_embed_video = $_REQUEST['number_embed_video'];
      }
      if ($max_value == 0) return "";

      $t_opt_str = '<select name="number_embed_video" onchange="JavaScript:checkForVideoType()">';

      $t_opt_str .= $this->generateGenericSelect($min_value, $max_value, $pre_selected);
      $t_opt_str .= "</select>\n";

      return $t_opt_str;
   }
   function getNumberOfEmbedAudio($min_value, $max_value)
   {
      // Generate a drop down list for selecting the number of embedded audios that will be added.
      $pre_selected = 0;
      if (isset($_REQUEST['number_embed_audio'])) {
          $pre_selected = $_REQUEST['number_embed_audio'];
          $this->number_embed_audio = $_REQUEST['number_embed_audio'];
      }
      if ($max_value == 0) return "";

      $t_opt_str = '<select name="number_embed_audio" onchange="JavaScript:checkForAudioCount()">';
      if ($min_value > 0 ) $t_opt_str .= "<OPTION value='0'>Number of Embedded Audio</option>";

      for($j= $min_value; $j <= $max_value; $j++)
      {
         $t_opt_str .= "<OPTION";
         if ($j == $pre_selected) {
             $t_opt_str .= " selected";
         }
         if ($min_value == 0 && $j == 0 ) $t_opt_str .= " value='0'>Number of Embedded Audio</option>";
	 else $t_opt_str .= " value='".$j."'>".$j."</OPTION>";
      }
      $t_opt_str .= "</select>\n";

      return $t_opt_str;
   }


   function writeCommentContentSection($snum)
   {
      global $system_config, $editor_session, $oscailt_page_setting, $public_tags, $allowed_bb_code;
      global $load_wysiwyg_editor;
      $site_id = $oscailt_page_setting['site_id'];
      if($editor_session->isSessionOpen() && ($editor_session->editor->allowedReadAccessTo("editcomment", $site_id)))
      {
         $tp_payload = '<INPUT maxLength=100 size=50 name="time_posted" value="';
         if($this->comment->time_posted!=null)
            $tp_payload .= (date('D, d M Y H:i:s',$this->comment->time_posted+$system_config->timezone_offset));
         $tp_payload .= '">';
         $this->writeSectionEntry('timeposted', $tp_payload, false);
      }

      $tp_payload = "<INPUT maxLength=60 size=30 name='author_name' value='";
      $tp_payload .= htmlspecialchars($this->comment->author_name, ENT_QUOTES)."'>\n";
      $this->writeSectionEntry('authorname', $tp_payload, true);

      $tp_payload = "<INPUT maxLength=60 size=30 name='author_organisation' value='";
      $tp_payload .= htmlspecialchars($this->comment->author_organisation, ENT_QUOTES)."'>\n";
      $this->writeSectionEntry('authororganisation', $tp_payload, false);

      $tp_payload = "<INPUT maxLength=250 size=60 name='comment_title' value='";
      $tp_payload .= htmlspecialchars($this->comment->comment_title, ENT_QUOTES)."'>\n";
      if($editor_session->isSessionOpen() && ($editor_session->editor->allowedReadAccessTo("editcomment", $site_id)))
      {
          $tp_payload .= "&nbsp; <INPUT type='button' name='title_lower' value='Lowercase &amp; Capitalize First Letters' onClick='JavaScript:deCapitalize(2)'>";
      }
      $this->writeSectionEntry('title', $tp_payload, true);

      $extra_payload = "";
      if($this->isNewComment() && $editor_session->isSessionOpen())
      {
         if($this->comment->comment_content_html == 'on') $is_checked = "checked";
         else $is_checked = "";
         $extra_payload = "<small class='editornote'><input style='vertical-align:middle' type='checkbox' name='comment_content_html' $is_checked>use HTML</small>";
         $extra_payload.=$this->getRichContentHelp(true);
         if($system_config->allow_bb_code && count($allowed_bb_code) > 0)
         {
            $extra_payload .= "<small class='editornote'>BB Code</small>";
            $extra_payload.= $this->getRichContentHelp(false, false);
         }
      }
      elseif(!$this->isNewComment())
      {
         $extra_payload = "<small class='editornote'>HTML</small> <input type='checkbox' name='dummy1' checked disabled><input type='hidden' name='story_content_html' value='on'>";
         $extra_payload.= $this->getRichContentHelp(false);
         if($system_config->allow_bb_code && count($allowed_bb_code) > 0)
         {
            $extra_payload .= "<small class='editornote'>BB Code</small>";
            $extra_payload.= $this->getRichContentHelp(false, false);
         }

      }
      elseif($system_config->allow_rich_content_in_comment)
      {
         if(count($public_tags) > 0)
         {
            $extra_payload = "<small class='editornote'>HTML</small>";
            $extra_payload.= $this->getRichContentHelp(false);
         }
         if($system_config->allow_bb_code && count($allowed_bb_code) > 0)
         {
            $extra_payload .= "<small class='editornote'>BB Code</small>";
            $extra_payload.= $this->getRichContentHelp(false, false);
         }
      }

      $tp_payload = "";
      if ($load_wysiwyg_editor == 1) {
          $tp_payload = "<small><input type='button' name='toggle_mce' value='Toggle Editor on/off' onclick='javascript:toggleTinyMCE(0);' /></small><br>";
      } 
      if ($load_wysiwyg_editor == 2) {
          $tp_payload = "<small>Note: Switch from HTML to Text for copy &amp; paste to work</small><br>";
          $t_btn_text = "Turn On WYSIWYG Editor on Next Preview";

	  if (isset($_REQUEST['editor_mode']) && $_REQUEST['editor_mode'] != '')  {
             $tp_payload .= "<input type='hidden' name='editor_mode' value='".$_REQUEST['editor_mode']."' > ";
	     if ($_REQUEST['editor_mode'] == 'on') $t_btn_text = "Turn Off WYSIWYG Editor on Next Preview";
          } else {
             $tp_payload .= "<input type='hidden' name='editor_mode' value='off' > ";
          }
          $tp_payload .= "<input type='button' name='editor_switch' value='".$t_btn_text."' onclick='turn_on_off();' ><br> ";
      } 
      
      $tp_payload .= "<TEXTAREA name='comment_content' rows=20 cols=70 id='story_mce' class='story_mce'>";
      $tp_payload .= htmlspecialchars($this->comment->comment_content)."</TEXTAREA>\n";
      if($editor_session->isSessionOpen() && ($editor_session->editor->allowedReadAccessTo("editcomment", $site_id)))
      {
          $tp_payload .= "&nbsp; <INPUT type='button' name='content_lower' value='Lowercase &amp; Capitalize first letters of each sentence in Content' onClick='JavaScript:deCapitalize(5)'>";
      }
      if ($load_wysiwyg_editor == 2) {
          $tp_payload .= "<script>setup_html_editor();</script>";
      }
      $this->writeSectionEntry('content', $tp_payload, true, $extra_payload);

      $tp_payload = "<INPUT maxLength='250' size='60' name='related_link' value='";
      $tp_payload .= htmlspecialchars($this->comment->related_link, ENT_QUOTES)."'>\n";
      $this->writeSectionEntry('relatedlink', $tp_payload, false);

      // Bug in the system. It should produce true, but instead produces 'on'
      if ($this->getDisplayOption("youtube_id_allow_in_comments") == 'on') {
         $this->writeEmbeddedVideoForm(false);
      }
      // Write aembedded audio stuff.
      if ($this->getDisplayOption("audio_id_allow_in_comments") == 'on') {
         $this->writeEmbeddedAudioForm(false);
      }

      return ++$snum;
   }

   function writeContentSection($snum)
   {
      global $system_config, $editor_session, $oscailt_page_setting, $public_tags, $allowed_bb_code;
      global $load_wysiwyg_editor;

      $site_id = $oscailt_page_setting['site_id'];
      $hed_text = $this->getDisplayOption('content_sectionhead_text'). " - <span class='publish-require'>".$this->getDisplayOption('required_text')."</span>";
      $this->writeSectionHeader($snum, $hed_text);
      if($this->content_type == 'comment') return $this->writeCommentContentSection($snum);

      if($editor_session->isSessionOpen() && ($editor_session->editor->allowedReadAccessTo("editstory", $site_id) || $editor_session->editor->allowedReadAccessTo("editfeature", $site_id)))
      {
         $tp_payload = '<INPUT maxLength=100 size=50 name="time_posted" value="';
         if($this->story->time_posted!=null)
            $tp_payload .= (date('D, d M Y H:i:s',$this->story->time_posted+$system_config->timezone_offset));
         $tp_payload .= '">';
         $this->writeSectionEntry('timeposted', $tp_payload, false);
      }

      $tp_payload = "<INPUT maxLength=60 size=30 name='author_name' value='";
      $tp_payload .= htmlspecialchars($this->story->author_name, ENT_QUOTES)."'>\n";
      $this->writeSectionEntry('authorname', $tp_payload, true);

      $tp_payload = "<INPUT maxLength=100 size=50 name='author_organisation' value='";
      $tp_payload .= htmlspecialchars($this->story->author_organisation, ENT_QUOTES)."'>\n";
      $this->writeSectionEntry('authororganisation', $tp_payload, false);

      $tp_payload = "<div style='display:none'>Title <INPUT type=text name='title' value=''></div>";
      $tp_payload .= "<INPUT maxLength=200 size=60 name='story_title' value='";
      $tp_payload .= htmlspecialchars($this->story->story_title, ENT_QUOTES)."'>\n";
      if($editor_session->isSessionOpen() && ($editor_session->editor->allowedReadAccessTo("editstory", $site_id) || $editor_session->editor->allowedReadAccessTo("editfeature", $site_id)))
      {
          $tp_payload .= "&nbsp; <INPUT type='button' name='title_lower' value='Lowercase &amp; Capitalize First Letters' onClick='JavaScript:deCapitalize(1)'>";
      }
      $this->writeSectionEntry('title', $tp_payload, true);

      $tp_payload = "<INPUT maxLength=200 size=50 name='story_subtitle' value='";
      $tp_payload .= htmlspecialchars($this->story->story_subtitle, ENT_QUOTES)."'>\n";
      $this->writeSectionEntry('subtitle', $tp_payload, false);

      $extra_payload = "";
      if($this->isNewStory() && $editor_session->isSessionOpen())
      {
         if($this->story->story_summary_html == 'on') $is_checked = "checked";
         else $is_checked = "";
         $extra_payload = "<small class='editornote'><input style='vertical-align:middle' type='checkbox' name='story_summary_html' $is_checked>use HTML</small>";
         $extra_payload.=$this->getRichContentHelp(true);
         if($system_config->allow_bb_code && count($allowed_bb_code) > 0)
         {
            $extra_payload .= "<small class='editornote'>BB Code</small>";
            $extra_payload.= $this->getRichContentHelp(false, false);
         }
      }
      elseif(!$this->isNewStory())
      {
         $extra_payload = "<small class='editornote'>HTML</small> <input type='checkbox' name='dummy1' checked disabled><input type='hidden' name='story_summary_html' value='on'>";
         $extra_payload.= $this->getRichContentHelp(false);
         if($system_config->allow_bb_code && count($allowed_bb_code) > 0)
         {
            $extra_payload .= "<small class='editornote'>BB Code</small>";
            $extra_payload.= $this->getRichContentHelp(false, false);
         }

      }
      elseif($system_config->allow_rich_content_in_summary)
      {
         if(count($public_tags) > 0)
         {
            $extra_payload = "<small class='editornote'>HTML</small>";
            $extra_payload.= $this->getRichContentHelp(false);
         }
         if($system_config->allow_bb_code && count($allowed_bb_code) > 0)
         {
            $extra_payload .= "<small class='editornote'>BB Code</small>";
            $extra_payload.= $this->getRichContentHelp(false, false);
         }
      }
 
      $tp_payload = "<TEXTAREA name='story_summary' id='summary_mce' rows=7 cols=70 class='story_mce'>";
      $tp_payload .= htmlspecialchars($this->story->story_summary)."</TEXTAREA>\n";
      if($editor_session->isSessionOpen() && ($editor_session->editor->allowedReadAccessTo("editstory", $site_id) || $editor_session->editor->allowedReadAccessTo("editfeature", $site_id)))
      {
          $tp_payload .= "&nbsp; <INPUT type='button' name='summary_lower' value='Lowercase &amp; Capitalize first letters of each sentence in Summary' onClick='JavaScript:deCapitalize(3)'>";
          $tp_payload .= "&nbsp; <A HREF='editorstatus.php?scratchpad=true' target='new_'>Jump to Scratchpad for more edit options</a>";
      }
      $this->writeSectionEntry('summary', $tp_payload, true, $extra_payload);

      $extra_payload = "";
      if($this->isNewStory() && $editor_session->isSessionOpen())
      {
         if($this->story->story_content_html == 'on') $is_checked = "checked";
         else $is_checked = "";
         $extra_payload = "<small class='editornote'><input style='vertical-align:middle' type='checkbox' name='story_content_html' $is_checked>use HTML</small>";
         $extra_payload.=$this->getRichContentHelp(true);
         if($system_config->allow_bb_code && count($allowed_bb_code) > 0)
         {
            $extra_payload .= "<small class='editornote'>BB Code</small>";
            $extra_payload.= $this->getRichContentHelp(false, false);
         }
      }
      elseif(!$this->isNewStory())
      {
         $extra_payload = "<small class='editornote'>HTML</small> <input type='checkbox' name='dummy2' checked disabled><input type='hidden' name='story_content_html' value='on'>";
         $extra_payload.= $this->getRichContentHelp(false);
         if($system_config->allow_bb_code && count($allowed_bb_code) > 0)
         {
            $extra_payload .= "<small class='editornote'>BB Code</small>";
            $extra_payload.= $this->getRichContentHelp(false, false);
         }
      }
      elseif($system_config->allow_rich_content_in_story)
      {
         if(count($public_tags) > 0)
         {
            $extra_payload = "<small class='editornote'>HTML</small>";
            $extra_payload.= $this->getRichContentHelp(false);
         }
         if($system_config->allow_bb_code && count($allowed_bb_code) > 0)
         {
            $extra_payload .= "<small class='editornote'>BB Code</small>";
            $extra_payload.= $this->getRichContentHelp(false, false);
         }
      }
      //$tp_payload = "<input type='button' name='toggle_mce' value='JS Alert Test' onclick='javascript:callAlertTest();' />";
      $tp_payload = "";
      if ($load_wysiwyg_editor == 1) {
          $tp_payload = "<small><input type='button' name='toggle_mce' value='Toggle Editor on/off' onclick='javascript:toggleTinyMCE(1);' /></small><br>";
      } else if ($load_wysiwyg_editor == 2) {
          $tp_payload = "<small>Note: Switch from HTML to Text for copy &amp; paste to work</small>";
          $t_btn_text = "Turn On WYSIWYG Editor on Next Preview";

	  if (isset($_REQUEST['editor_mode']) && $_REQUEST['editor_mode'] != '')  {
             $tp_payload .= "<input type='hidden' name='editor_mode' value='".$_REQUEST['editor_mode']."' > ";
	     if ($_REQUEST['editor_mode'] == 'on') $t_btn_text = "Turn Off WYSIWYG Editor on Next Preview";
          } else {
             $tp_payload .= "<input type='hidden' name='editor_mode' value='off' > ";
          }
          $tp_payload .= "<input type='button' name='editor_switch' value='".$t_btn_text."' onclick='turn_on_off();' ><br> ";
      } 
      $tp_payload .= "<TEXTAREA name='story_content' id='story_mce' rows=20 cols=70 class='story_mce'>";
      $tp_payload .= htmlspecialchars($this->story->story_content)."</TEXTAREA>\n";
      if($editor_session->isSessionOpen() && ($editor_session->editor->allowedReadAccessTo("editstory", $site_id) || $editor_session->editor->allowedReadAccessTo("editfeature", $site_id)))
      {
          $tp_payload .= "&nbsp; <INPUT type='button' name='content_lower' value='Lowercase &amp; Capitalize first letters of each sentence in Content' onClick='JavaScript:deCapitalize(4)'>";
      }

      if ($load_wysiwyg_editor == 2) {
      // Call Javascript to turn on WYSIWYG editor.
          $tp_payload .= "<script>setup_html_editor();</script>";
      }
      $this->writeSectionEntry('content', $tp_payload, true, $extra_payload);

      if($editor_session->isSessionOpen()==false)
      {
         $ft_txt = $this->getDisplayOption('contentsection_prompt');
      }
      else
      {
         $ft_txt = $this->getDisplayOption('contentsection_editorprompt');
         if($editor_session->editor->allowedReadAccessTo("editfeature", $site_id))
            $ft_txt .= "<br />".$this->getDisplayOption('contentsection_editorfeatureprompt')."\n";
      }

      $tp_payload = "<INPUT maxLength=250 size=60 name='related_link' value='";
      $tp_payload .= htmlspecialchars($this->story->related_link, ENT_QUOTES)."'>\n";
      $this->writeSectionEntry('relatedlink', $tp_payload, false);

      // Bug in the system. It should produce true, but instead produces 'on'
      if ($this->getDisplayOption("youtube_id_allow_in_stories") == 'on') {
         $this->writeEmbeddedVideoForm(true);
      }
      if ($this->getDisplayOption("audio_id_allow_in_stories") == 'on') {
         $this->writeEmbeddedAudioForm(true);
      }

      if($editor_session->isSessionOpen() && $editor_session->editor->allowedReadAccessTo("editfeature", $site_id))
      {
         ++$snum;
         $this->writeSectionHeader($snum, $this->getDisplayOption("extra_feature_header_text"), $this->getDisplayOption("extra_feature_header_intro"));
         $tp_payload = "<INPUT type=checkbox name='hide_summary_on_feature_page'";
         if($this->story->hide_summary_on_feature_page) $tp_payload.=("checked");
         $tp_payload .=">\n";
         $this->writeSectionEntry('hide_summary_feature_page', $tp_payload, false);
         $tp_payload = "<INPUT type=checkbox name='hide_summary_on_story_page'";
         if($this->story->hide_summary_on_story_page) $tp_payload.=("checked");
         $tp_payload.=">\n";
         $this->writeSectionEntry('hidesummary', $tp_payload, false);
         $tp_payload = "<INPUT type=checkbox name='hide_attachments_on_feature_page'";
         if($this->story->hide_attachments_on_feature_page) $tp_payload .= " checked";
         $tp_payload .= "'>\n";
         $this->writeSectionEntry('hideattachmentsonfeaturepage', $tp_payload, false);
         if($editor_session->isSessionOpen() && $editor_session->editor->allowedReadAccessTo("editfeature", $site_id))
         {
            if($this->isNewStory() && $editor_session->isSessionOpen())
            {
               if($this->story->story_content_extra_html == 'on') $is_checked = "checked";
               else $is_checked = "";
               $extra_payload = "<small class='editornote'><input style='vertical-align:middle' type='checkbox' name='story_content_extra_html' $is_checked>use HTML</small>";
               $extra_payload.=$this->getRichContentHelp(true);
               if($system_config->allow_bb_code && count($allowed_bb_code) > 0)
               {
                  $extra_payload .= "<small class='editornote'>BB Code</small>";
                  $extra_payload.= $this->getRichContentHelp(false, false);
               }
            }
            elseif(!$this->isNewStory())
            {
               $extra_payload = "<small>HTML</small><input type='hidden' name='story_content_extra_html' value='on'>";
               $extra_payload.= $this->getRichContentHelp(false);
               if($system_config->allow_bb_code && count($allowed_bb_code) > 0)
               {
                  $extra_payload .= "<small>BB Code</small>";
                  $extra_payload.= $this->getRichContentHelp(false, false);
               }
            }
            $tp_payload = "<TEXTAREA name='story_content_extra' rows=20 cols=70>";
            $tp_payload .= htmlspecialchars($this->story->story_content_extra)."</TEXTAREA>\n";
            $this->writeSectionEntry('extracontent', $tp_payload, false, $extra_payload);
         }
      }
      return ++$snum;
   }

   function writeEmbeddedVideoForm($is_story)
   {
      // Bug in the system. It should produce true, but instead produces 'on'
      if ($is_story == true ) {
         $max_entry = $this->getDisplayOption("max_embedvideo_in_stories");
      }
      else
      {
         $max_entry = $this->getDisplayOption("max_embedvideo_in_comments");
      }


      $tp_select_payload = $this->getEmbedVideoSelect("Type of Embedded Video");

      $disabled_str = "' DISABLED style='display:none'";
      $display_str = "style='display:none'";

      if (isset($_REQUEST['videoType']) ) {
	      if ($_REQUEST['videoType'] > 0 ) {
	      	$disabled_str = "";
	      	$display_str = "";
	      }
      }
      $tp_select_number = $this->getNumberOfEmbedVideo(0, $max_entry);
      $tp_select_payload .= " &nbsp; # " . $tp_select_number;
      if ($this->number_embed_video == null)
      {
          $this->number_embed_video = 0;
      }

      $tp_payload = "" ;
      for($id_num=1;$id_num <= $max_entry;$id_num++)
      {
         // For embedded video 20 chars should be enough to hold the video id.
         // $tp_payload .= "<div id='embv_".$id_num."' style='display:none'><INPUT type='text' maxLength=20 size=20 name='youtube_id_".$id_num."' value='";
         if ($id_num <= $this->number_embed_video) 
         {
            $tp_payload .= "<div id='embv_".$id_num."' ".$display_str."><INPUT type='text' maxLength=20 size=20 name='youtube_id_".$id_num."' value='";
            if ($is_story == true ) {
               if ($id_num == 1) $tp_payload .= htmlspecialchars($this->story->youtube_id_1, ENT_QUOTES). $disabled_str;
               if ($id_num == 2) $tp_payload .= htmlspecialchars($this->story->youtube_id_2, ENT_QUOTES). $disabled_str;
               if ($id_num == 3) $tp_payload .= htmlspecialchars($this->story->youtube_id_3, ENT_QUOTES). $disabled_str;
               if ($id_num == 4) $tp_payload .= htmlspecialchars($this->story->youtube_id_4, ENT_QUOTES). $disabled_str;
               if ($id_num == 5) $tp_payload .= htmlspecialchars($this->story->youtube_id_5, ENT_QUOTES). $disabled_str;
            } else {
               if ($id_num ==1) $tp_payload .= htmlspecialchars($this->comment->youtube_id_1,ENT_QUOTES). $disabled_str;
               if ($id_num ==2) $tp_payload .= htmlspecialchars($this->comment->youtube_id_2,ENT_QUOTES). $disabled_str;
               if ($id_num ==3) $tp_payload .= htmlspecialchars($this->comment->youtube_id_3,ENT_QUOTES). $disabled_str;
               if ($id_num ==4) $tp_payload .= htmlspecialchars($this->comment->youtube_id_4,ENT_QUOTES). $disabled_str;
               if ($id_num ==5) $tp_payload .= htmlspecialchars($this->comment->youtube_id_5,ENT_QUOTES). $disabled_str;
            }

            // Don't add the line break on the last one.
            // This code is here twice to fix a bug -note the single quote is different.
            if ($id_num < $max_entry) $tp_payload .= "' onChange='validateVideoId(".$id_num.")'> &nbsp; \n";
            else $tp_payload .= "'>\n";
         }
         else
         {
            $tp_payload .= "<div id='embv_".$id_num."' style='display:none'><INPUT type='text' maxLength=20 size=20 name='youtube_id_".$id_num."' value='";
            // For the other ones, they should not show up in the display.
            $tp_payload .= "' onChange='validateVideoId(".$id_num.")' DISABLED style='display:none'";

            // Don't add the line break on the last one.
            // if ($id_num < $max_entry) $tp_payload .= "> &nbsp; \n";
            // else $tp_payload .= ">\n";
            $tp_payload .= ">\n";
         }
         $tp_payload .= " &nbsp; <b>" . $this->getDisplayOption("uploadfile_description_title"). " " .$id_num . "</b> ";

	 // For embedded video 20 chars should be enough to hold the video id.
         $tp_payload .= "<INPUT type='text' maxLength=255 size=60 name='embed_vid_desc_".$id_num."' value='";
         if ($id_num <= $this->number_embed_video) 
         {
	    $tmp_name = "embed_vid_desc_" . $id_num;
            if (isset($_REQUEST[$tmp_name])) {
               $tp_payload .= $_REQUEST[$tmp_name]. $disabled_str;
            }

            // Don't add the line break on the last one.
            // This code is here twice to fix a bug -note the single quote is different.
            if ($id_num < $max_entry) $tp_payload .= "'></div><BR>\n";
            else $tp_payload .= "'></div>\n";
         }
         else
         {
            // For the other ones, they should not show up in the display.
            $tp_payload .= "' DISABLED style='display:none'";

            // Don't add the line break on the last one.
            if ($id_num < $max_entry) $tp_payload .= "></div><BR>\n";
            else $tp_payload .= "></div>\n";
         }

         $tmp_name = "attachment_id_v" . $id_num;
         if (isset($_REQUEST[$tmp_name])) {
             $tp_payload .= "<INPUT type='hidden' name='".$tmp_name."' value='".$_REQUEST[$tmp_name]."'>\n";
         }
         $tmp_name = "attachment_tp_v" . $id_num;
         if (isset($_REQUEST[$tmp_name])) {
             $tp_payload .= "<INPUT type='hidden' name='".$tmp_name."' value='".$_REQUEST[$tmp_name]."'>\n";
         }

      }
      $this->writeEmbedVideoEntry('youtube_id_link', $tp_select_payload, $tp_payload, false, true);
   }

      
   function writeEmbeddedAudioForm($is_story)
   {
      // Bug in the system. It should produce true, but instead produces 'on'
      if ($is_story == true ) {
         $max_entry = $this->getDisplayOption("max_embedaudio_in_stories");
      }
      else
      {
         $max_entry = $this->getDisplayOption("max_embedaudio_in_comments");
      }
      if ($this->number_embed_audio == null) $this->number_embed_audio = 0;

      $tp_select_payload = $this->getNumberOfEmbedAudio(0, $max_entry);

      $disabled_str = "' DISABLED style='display:none'";
      $display_str = "style='display:none'";

      if ($max_entry > 0) $disabled_str = "";
      if ($max_entry > 0) $display_str = "";

      $tp_payload = "" ;
      for($id_num=1;$id_num <= $max_entry;$id_num++)
      {
         // For audio, we could need up to 200 chars to fit the audio URL into.
         // $tp_payload .= "<div id='emba_".$id_num."' style='display:none'><b> Audio ".$id_num."&nbsp; </b>&nbsp; <INPUT type='text' maxLength=200 size=80 name='audio_id_".$id_num."' value='";
         if ($id_num <= $this->number_embed_audio) 
         {
            $tp_payload .= "<div id='emba_".$id_num."' ".$display_str."><b> Audio ".$id_num."&nbsp; </b>&nbsp; <INPUT type='text' maxLength=200 size=80 name='audio_id_".$id_num."' value='";
            if ($is_story == true ) {
               if ($id_num== 1) $tp_payload .= htmlspecialchars($this->story->audio_id_1, ENT_QUOTES). $disabled_str;
               if ($id_num== 2) $tp_payload .= htmlspecialchars($this->story->audio_id_2, ENT_QUOTES). $disabled_str;
               if ($id_num== 3) $tp_payload .= htmlspecialchars($this->story->audio_id_3, ENT_QUOTES). $disabled_str;
               if ($id_num== 4) $tp_payload .= htmlspecialchars($this->story->audio_id_4, ENT_QUOTES). $disabled_str;
               if ($id_num== 5) $tp_payload .= htmlspecialchars($this->story->audio_id_5, ENT_QUOTES). $disabled_str;
            } else {
               if ($id_num==1) $tp_payload .= htmlspecialchars($this->comment->audio_id_1,ENT_QUOTES). $disabled_str;
               if ($id_num==2) $tp_payload .= htmlspecialchars($this->comment->audio_id_2,ENT_QUOTES). $disabled_str;
               if ($id_num==3) $tp_payload .= htmlspecialchars($this->comment->audio_id_3,ENT_QUOTES). $disabled_str;
               if ($id_num==4) $tp_payload .= htmlspecialchars($this->comment->audio_id_4,ENT_QUOTES). $disabled_str;
               if ($id_num==5) $tp_payload .= htmlspecialchars($this->comment->audio_id_5,ENT_QUOTES). $disabled_str;
            }
            // Don't add the line break on the last one.
            // This code is here twice to fix a bug -note the single quote is different.
            if ($id_num < $max_entry) $tp_payload .= "'> &nbsp; \n";
            else $tp_payload .= "'>\n";
         }
         else
         {
            $tp_payload .= "<div id='emba_".$id_num."' style='display:none'><b> Audio ".$id_num."&nbsp; </b>&nbsp; <INPUT type='text' maxLength=200 size=80 name='audio_id_".$id_num."' value='";
            $tp_payload .= "' DISABLED style='display:none'";
            $tp_payload .= "> \n";
         }
         $tp_payload .= "<br><b>" . $this->getDisplayOption("uploadfile_description_title"). " " .$id_num . "</b> ";

	 // For embedded video 20 chars should be enough to hold the video id.
         $tp_payload .= "<INPUT type='text' maxLength=255 size=40 name='embed_audio_desc_".$id_num."' value='";
         if ($id_num <= $this->number_embed_audio) 
         {
	    $tmp_name = "embed_audio_desc_" . $id_num;
            if (isset($_REQUEST[$tmp_name])) {
               $tp_payload .= $_REQUEST[$tmp_name]. $disabled_str;
            }

            // Don't add the line break on the last one.
            // This code is here twice to fix a bug -note the single quote is different.
            if ($id_num < $max_entry) $tp_payload .= "'></div><BR>\n";
            else $tp_payload .= "'></div>\n";
         }
         else
         {
            // For the other ones, they should not show up in the display.
            $tp_payload .= "' DISABLED style='display:none'";

            // Don't add the line break on the last one.
            if ($id_num < $max_entry) $tp_payload .= "></div><BR>\n";
            else $tp_payload .= "></div>\n";
         }



         $tmp_name = "attachment_id_a" . $id_num;
         if (isset($_REQUEST[$tmp_name])) {
             $tp_payload .= "<INPUT type='hidden' name='".$tmp_name."' value='".$_REQUEST[$tmp_name]."'>\n";
         }
         $tmp_name = "attachment_tp_a" . $id_num;
         if (isset($_REQUEST[$tmp_name])) {
             $tp_payload .= "<INPUT type='hidden' name='".$tmp_name."' value='".$_REQUEST[$tmp_name]."'>\n";
         }

      }
      $this->writeEmbedVideoEntry('audio_id_link', $tp_select_payload, $tp_payload, false, true);
   }


   function getRichContentHelp($is_choosable, $is_html = true)
   {
      global $editor_session, $system_config, $approved_tags, $basic_tags, $public_tags, $allowed_bb_code;
      if($is_html)
      {
         if(canUseCode())
         {
            $txt = "You are allowed to input any HTML, code or scripts into this field. If you check this box, your entire contribution will be treated as html / code.  This means that you will need to input &lt;br&gt; for new lines and so on.";
         }
         elseif(canUseFreeHTML())
         {
            $txt = "You are allowed to input any HTML into this field. The only restrictions on your content are that you can not use scripts or code.";
         }
         elseif(canUseHTML())
         {
            $tags = array_unique(array_merge($public_tags, $basic_tags));
            $tags = array_unique(array_merge($tags, $approved_tags));
            $txt = "You are allowed to input HTML into this field. You are allowed to use the following tags: &lt;".implode("&gt;, &lt;",$tags).">";
         }
         elseif($editor_session->isSessionOpen())
         {
            $tags = array_unique(array_merge($public_tags, $basic_tags));
            $txt = "You are allowed to input simple HTML into this field. You are allowed to use the following tags: &lt;".implode("&gt;, &lt;",$tags).">";
         }
         else
         {
            $txt = "You are allowed to input simple HTML into this field. You are allowed to use the following tags: &lt;".implode("&gt;, &lt;",$public_tags).">";
         }
         if($is_choosable)
         {
            $txt .= "Check the box to enable HTML";
         }
         $str = getHelpHTML($txt, false, true);
         return $str;
      }
      else
      {
         $txt = "You are allowed to use BB Code.  The following BB Code Tags are supported: [".implode("], [",$allowed_bb_code)."]";
         $str = getHelpHTML($txt, false, true);
         return $str;
      }
   }

   function writeSubmitSection($snum)
   {
      global $editor_session, $system_config, $graphics_store;
      $js_parts = array();
      if($this->getDisplayOption("javascript_termsandconditions_popup") == 'true')
      {
         $tc_pop = $this->getDisplayOption("termsandconditions_popup");
         if($tc_pop != "")
         {
             $tc_pop = preg_replace('/\n/', '\\n', $tc_pop);
             $js_parts[] = 'validateTerms("'.$tc_pop.'")';
         }
      }
      if($this->content_type == 'story' && $this->isNewStory() && $this->getDisplayOption("javascript_category_popup") == 'true')
      {
         $cat_pop = $this->getDisplayOption("categories_popup");
         if($cat_pop != "")
         {
            $cat_pop = preg_replace('/\n/', '\\n', $cat_pop);
            $js_parts[] = 'confirm("'.$cat_pop.'")';
         }
      }
      if(empty($js_parts))
      {
         $jsclick = "";
      }
      else
      {
         $jsclick = htmlspecialchars("onClick='if(".implode(' && ', $js_parts)."){ document.publishform.submit() } else {return false}'");
      }
      $intro_text = $this->getDisplayOption('submit_section_intro');
      $section_head_text = $this->getDisplayOption('submit_sectionhead_text');

      // If this is a publish comment and the publish comment delay is in effect, then tell users.
      if($system_config->publish_comment_delay > 0 ) {
         if($this->content_type=='comment' && $this->isNewComment()) {
             $intro_text .= "<p><strong>Note:</strong> Publish Delay of " .$system_config->publish_comment_delay ." minutes is in effect.";
         }
      }

      $validation_button = 0;
      $validation_text = "";
      if(isset($_REQUEST['number_of_files']) && ($_REQUEST['number_of_files'] >0))
      {
	 $is_comment = 0;
         if($this->content_type=='comment' && $this->isNewComment()) $is_comment = 1;
	 // Display the voluntary validation button for case when there are file uploads to do. Have to pass these
	 // parameters as javascript file is fixed
         $check_me_button_html = "<INPUT type='button' class='publish' name='check_files' value='Validate Fields' onClick='Javascript:CheckFile(".$_REQUEST['number_of_files'].",".$is_comment.",".$this->getDisplayOption('include_legal_section').")'> \n";
	 
         $image_check_button_html = "<INPUT type='button' class='publish' name='check_images' value='Image Dimensions' onClick='Javascript:CheckImages()'> \n";
         $validation_button = 1;
         $validation_text = "Click this button to validate before doing your file uploads. This will avoid long waits in the event of an error.";
         $imagecheck_text = "Click this button to validate image dimensions doing your file uploads. It cannot check image sizes.";

	 // Write out the Javascript for the voluntary validation button that allows browser side validation.
	 // This allows user to avoid finding out they have forgotten something after uploading their big files.
         $this->writeJsValidationSubmitSection();

         $button_text = $this->getDisplayOption('publishbutton_text');
         $intro_text = $this->getDisplayOption('nopreview_attachment_text')." ".$intro_text;

         $button_html = "<INPUT type='submit' class='publish' name='preview_confirmed' value='$button_text' ".$jsclick.">\n";
      }
      else
      {
         $button_text =  $this->getDisplayOption('previewbutton_text');
         $button_html =  "";
         $button_html .= "<INPUT type='submit' name='publish_button' value='$button_text'".$jsclick.">";
	 if ( $this->content_type == 'story' && $this->story->type_id == $system_config->feature_type_id) {
             $button_html .=  "&nbsp; &nbsp; &nbsp; ";
             $button_html .=  "<small>Article <input type=radio name=preview_mode value='article' checked>";
             $button_html .= " Feature <input type=radio name=preview_mode value='feature' ></small><BR>";
	 }
         //$button_html .= "<INPUT type='submit' name='publish_button' value='$button_text'".$jsclick.">";
      }
      $section_head_text .=  " ".$button_text;

      $this->writeSectionHeader($snum, $section_head_text, $intro_text);
      echo '<tr class="publish"><td class="publish-contents" colSpan=2>';
      if ( $validation_button == 1) {
          echo "<div class='publish-buttons'>" . $check_me_button_html ."&nbsp;" . $validation_text."</div><br>";
          echo "<div class='publish-buttons'>" . $image_check_button_html ."&nbsp;" . $imagecheck_text."</div><br>";
	  // Add in a dummy image that we can swap to.
          echo "\n<div class='object-create'><img name='check_dim_image' src='".$graphics_store."feedlogo.gif' alt='dummy image'></div> ";
      }

      $processing_images = false;
      if(isset($_REQUEST['number_of_files']) && $_REQUEST['number_of_files'] > 0)
      {
          // Do nothing
          $processing_images = true;
      }
      if ( (($this->use_captcha == true) && !isset($_REQUEST['cap_digits2'])) || $this->generate_captcha == true || $processing_images == true) {
	  $captcha = new NumericCaptcha();
	  $captcha->generateCaptcha();

          if ($this->getDisplayOption('captcha_prompt_text') != "")
          {
              echo "\n<div>". $this->getDisplayOption('captcha_prompt_text') .$captcha->getNumericString(). " ";
          }
	  else
          {
              echo "\n<div>Enter these digits in the text box: " .$captcha->getNumericString(). " ";
          }
	  echo($captcha->getFormHTML() . "</div><BR>");
      }
      // So if the captcha option is enabled, then and they were entered, write them hidden here to carry them
      // through this preview.
      // When publishing images, there is no preview so they are not written.
      if($processing_images == false ) {
         if ( $this->use_captcha == true && $this->generate_captcha == false && isset($_REQUEST['cap_digits1']) && isset($_REQUEST['cap_digits2']) ) {
            echo "<input type='hidden' name='cap_digits1' value='".$_REQUEST['cap_digits1']."'>";
            echo "<input type='hidden' name='cap_digits2' value='".$_REQUEST['cap_digits2']."'>";
         }
      }

      if ($this->allow_public_edits == true && $this->public_password_enabled == true) {
          echo "<div class='publish-contents'><b>Password</b> &nbsp; &nbsp; <INPUT type='password' name='publish_password' size=10 maxlength=15 value=''></div><br>";
      }

      echo "<div class='publish-buttons'>$button_html</div>";
      echo "</td></tr>\n";
   }

   function writeJsValidationSubmitSection()
   {
      global $fileExtensions, $system_config, $graphics_store;
      // Different JS depending whether it is a new story or comment. Need a fn to check the file types.
      ?>
         <script type="text/javascript" language="Javascript">
	 var gv_selected_image = 0;
         function CheckFormats(target_filetype)
         {
             // The . is present so strip it off.
             var image_formats = "<?=$fileExtensions['image']?>";
             var video_formats = "<?=$fileExtensions['video']?>";
             var audio_formats = "<?=$fileExtensions['audio']?>";
             var misc_formats = "<?=$fileExtensions['misc']?>";
             <?
             if ($this->content_type == 'story') {
                if ( $system_config->allow_story_image_attachments) echo("var permit_image=1;\n");
	        else echo("var permit_image=0;\n");
                if ( $system_config->allow_story_video_attachments) echo("var permit_video=1;\n");
	        else echo("var permit_video=0;\n");
                if ( $system_config->allow_story_audio_attachments) echo("var permit_audio=1;\n");
	        else echo("var permit_audio=0;\n");
                if ( $system_config->allow_story_miscellaneous_attachments) echo("var permit_misc=1;\n");
	        else echo("var permit_misc=0;\n");
             } else {
                if ( $system_config->allow_comment_image_attachments) echo("var permit_image=1;\n");
	        else echo("var permit_image=0;\n");
                if ( $system_config->allow_comment_video_attachments) echo("var permit_video=1;\n");
	        else echo("var permit_video=0;\n");
                if ( $system_config->allow_comment_audio_attachments) echo("var permit_audio=1;\n");
	        else echo("var permit_audio=0;\n");
                if ( $system_config->allow_comment_miscellaneous_attachments) echo("var permit_misc=1;\n");
	        else echo("var permit_misc=0;\n");
             }
             ?>
             var is_image = 0;
             var is_video = 0;
             var is_audio = 0;
             var is_misc = 0;
             var tmp_type = target_filetype.substring(1);
             var type_msg = "";

	     if (image_formats.indexOf(tmp_type) != -1 ) is_image = 1;
	     if (video_formats.indexOf(tmp_type) != -1 ) is_video = 1;
	     if (audio_formats.indexOf(tmp_type) != -1 ) is_audio = 1;
	     if (misc_formats.indexOf(tmp_type) != -1 ) is_misc = 1;

	     if (is_image == 1 || is_video == 1 || is_audio == 1  || is_misc == 1)
	     {
	         var verboten_msg = " uploads are NOT allowed.";
	         if (is_image == 1 && permit_image == 0 ) type_msg = " is an image file. Image" + verboten_msg;
	         if (is_video == 1 && permit_video == 0) type_msg = " is an video file. Video" + verboten_msg;
	         if (is_audio == 1 && permit_audio == 0) type_msg = " is an audio file. Audio" + verboten_msg;
	         if (is_misc == 1 && permit_misc == 0)  type_msg = " is an misc file. Other File" + verboten_msg;
	     } else {
	         // alert(tmp_type + " is not an known file type");
	         type_msg = " is NOT an known file type for uploads recognised by Oscailt.";
             }
             return type_msg;

	 }
         function CheckFileType(target_filetype)
         {
	     // This will return 1, 2, 3 or 4 depending on if it is image, video, audio or other
	     // A value of zero means it is none of the above.

             // The . is present so strip it off.
             var image_formats = "<?=$fileExtensions['image']?>";
             var video_formats = "<?=$fileExtensions['video']?>";
             var audio_formats = "<?=$fileExtensions['audio']?>";
             var misc_formats = "<?=$fileExtensions['misc']?>";
             <?
             if ($this->content_type == 'story') {
                if ( $system_config->allow_story_image_attachments) echo("var permit_image=1;\n");
	        else echo("var permit_image=0;\n");
	         // Forget about the other types for the moment.
             } else {
                if ( $system_config->allow_comment_image_attachments) echo("var permit_image=1;\n");
	        else echo("var permit_image=0;\n");
             }
             ?>
             var is_image = 0;
             var is_video = 0;
             var is_audio = 0;
             var is_misc = 0;
             var tmp_type = target_filetype.substring(1);
             var return_val = 0;

	     if (image_formats.indexOf(tmp_type) != -1 ) is_image = 1;
	     if (video_formats.indexOf(tmp_type) != -1 ) is_video = 1;
	     if (audio_formats.indexOf(tmp_type) != -1 ) is_audio = 1;
	     if (misc_formats.indexOf(tmp_type) != -1 ) is_misc = 1;

	     if (is_image == 1 || is_video == 1 || is_audio == 1  || is_misc == 1)
	     {
	         if (is_image == 1 && permit_image == 1 ) return_val = 1;
	         // Forget about the other types for the moment.
             }
             return return_val;
	 }
         <?
	 // This Js function is only used for stories
         //if($this->content_type=='story' && $this->isNewStory()) 
         if($this->content_type=='story' ) 
	 {
             ?>
             function CheckIsDigit(target_str)
             {
                var digit_str = "0123456789";
	        if ( digit_str.indexOf(target_str.charAt(0)) == -1 ) return 0;
                return 1;
	     }
             <?
	 }
         ?>
         function CheckImages()
         {
            var max_files = <?=$_REQUEST['number_of_files']?>;
            var graphics_path = "<?=$graphics_store?>/";
            CheckImagesMain(max_files, graphics_path);
         }
         </script>
         <?
   }

   function writeUploadAmountSection($snum)
   {
      global $system_config,$editor_session;
      if(($editor_session->isSessionOpen() ||
         ($this->content_type=='comment' && $system_config->commentAttachmentsAllowed()) ||
         ($this->content_type=='story' && $system_config->storyAttachmentsAllowed())))
      {
         $payload = $this->getUploadAmountSelect();
         $title = $this->getDisplayOption('choose_uploads_title');
         $title = $this->wrapTitle($title, false);
         $prompt = $this->getDisplayOption('choose_uploads_prompt')   ;
         $help = $this->getDisplayOption('choose_uploads_help');

         $this->writeSectionHeader($snum, $this->getDisplayOption('files_sectionhead_text'), $this->getDisplayOption('choose_uploads_intro') . " <strong>Maximum uploads: ".$this->max_uploads."</strong>");
         $this->writeFormInputField($title, $prompt, $payload, $help);
         $snum++;
      }
      return $snum;
   }

   function getUploadAmountSelect($num_selected=0, $txt = "")
   {
      global $system_config, $editor_session;
      $payload = "<SELECT class='publish' name='number_of_files'>";
      for($i=0;$i<=$this->max_uploads;$i++)
      {
         if($i == $num_selected)
         {
            $payload .=  "<OPTION selected>$i</OPTION>\n";
         }
         else
         {
            $payload .=  "<OPTION>$i</OPTION>\n";
         }
      }
      if($txt == "") $txt = $this->getDisplayOption('upload_button_text');
      $payload .= "</SELECT>\n";
      $payload .= "<INPUT class='publish' type='submit' value='$txt' name='file_upload_button'>";
      return $payload;
   }

   function getLimitsAsHTMLTable()
   {
      global $system_config, $graphics_store, $fileExtensions, $editor_session;
      $cls = "class='publish-section'";
      $str = "<table $cls style='background-color: #B8F6E8;'>";
      $str .= "<tr $cls>";
      $str .= "<th $cls><img src='".$graphics_store."imageicon.gif' alt='image'> ".$this->getDisplayOption("imagefiles_text");
      $str .= getHelpHTML($this->getDisplayOption("imagefiles_help"), false, true)."</th>";
      $str .= "<th $cls><img src='".$graphics_store."audioicon.gif' alt='audio'> ".$this->getDisplayOption("audiofiles_text");
      $str .= getHelpHTML($this->getDisplayOption("audiofiles_help"), false, true)."</th>";
      $str .= "<th $cls><img src='".$graphics_store."videoicon.gif' alt='video'> ".$this->getDisplayOption("videofiles_text");
      $str .= getHelpHTML($this->getDisplayOption("videofiles_help"), false, true)."</th>";
      $str .= "<th $cls><img src='".$graphics_store."texticon.gif' alt='other files'> ".$this->getDisplayOption("miscfiles_text");
      $str .= getHelpHTML($this->getDisplayOption("miscfiles_help"), false, true)."</th>";
      $str .= "<tr $cls>";
      $str .= "<td $cls>";
      $sz = $this->getDisplayOption("maxsize_text").": ";
      $ft = $this->getDisplayOption("filetypes_text").": ";
      $na = $this->getDisplayOption("not_allowed_upload_text");
      if($editor_session->canIgnoreLimits())
      {
         $str .= "No restrictions";
      }
      elseif(($this->content_type == 'story' and $system_config->allow_story_image_attachments) or ($this->content_type == 'comment' and $system_config->allow_comment_image_attachments))
      {
         $str .= "<ul $cls>";
         $str .= "<li $cls>".$sz.($system_config->image_attachment_max_size / 1024)." KB"."</li>";
         $str .= "<li $cls>".$this->getDisplayOption("maxwidth_text").": ".$system_config->image_attachment_max_width." pixels</li>";
         $str .= "<li $cls>".$ft.str_replace(" ", " .", $fileExtensions['image'])."</li>";
         $str .= "</ul>";
      }
      else
      {
         $str .= "<span class='error'>$na ".$this->getDisplayOption("imagefiles_text")."</span>";
      }
      $str .= "</td>";
      $str .= "<td $cls>";
      if($editor_session->canIgnoreLimits())
      {
         $str .= "No restrictions";
      }
      elseif(($this->content_type == 'story' and $system_config->allow_story_audio_attachments) or ($this->content_type == 'comment' and $system_config->allow_comment_audio_attachments))
      {
         $str .= "<ul $cls>";
         $str .= "<li $cls>$sz".($system_config->audio_attachment_max_size / 1024)." KB"."</li>";
         $str .= "<li $cls>$ft".str_replace(" ", " .", $fileExtensions['audio'])."</li>";
         $str .= "</ul>";
      }
      else
      {
         $str .= "<span class='error'>$na ".$this->getDisplayOption("audiofiles_text")."</span>";
      }
      $str .= "</td>";
      $str .= "<td $cls>";
      if($editor_session->canIgnoreLimits())
      {
         $str .= "No restrictions";
      }
      elseif(($this->content_type == 'story' and $system_config->allow_story_video_attachments) or ($this->content_type == 'comment' and $system_config->allow_comment_video_attachments))
      {
         $str .= "<ul $cls>";
         $str .= "<li $cls>$sz".($system_config->video_attachment_max_size / 1024)." KB"."</li>";
         $str .= "<li $cls>$ft".str_replace(" ", " .", $fileExtensions['video'])."</li>";
         $str .= "</ul>";
      }
      else
      {
         $str .= "<span class='error'>$na ".$this->getDisplayOption("videofiles_text")."</span>";
      }
      $str .= "</td>";
      $str .= "<td $cls>";
      if($editor_session->canIgnoreLimits())
      {
         $str .= "No restrictions";
      }
      elseif(($this->content_type == 'story' and $system_config->allow_story_miscellaneous_attachments) or ($this->content_type == 'comment' and $system_config->allow_comment_miscellaneous_attachments))
      {
         $str .= "<ul $cls>";
         $str .= "<li $cls>$sz".($system_config->miscellaneous_attachment_max_size / 1024)." KB</li>";
         $str .= "<li $cls>$ft".str_replace(" ", " .", $fileExtensions['misc'])."</li>";
         $str .= "</ul>";
      }
      else
      {
         $str .= "<span class='error'>$na ".$this->getDisplayOption("miscfiles_text")."</span>";
      }
      $str .= "</td>";
      $str .= "</tr>";
      $str .= "</table>";
      return $str;
   }

   function allowedUpload()
   {
      global $editor_session, $system_config;
      if($editor_session->canIgnoreLimits() or ($this->content_type == 'story' and $system_config->allow_story_image_attachments or $system_config->allow_story_audio_attachments or $system_config->allow_story_video_attachments  or $system_config->allow_story_miscellaneous_attachments) or ($this->content_type == 'comment'  and $system_config->allow_comment_image_attachments or $system_config->allow_comment_audio_attachments or $system_config->allow_comment_video_attachments  or $system_config->allow_comment_miscellaneous_attachments)) return true;
      return false;
   }

   function writeAttachmentsSection($snum)
   {
      global $system_config, $editor_session, $oscailt_page_setting;
      $site_id = $oscailt_page_setting['site_id'];
      if($this->allowedUpload() === false) return $snum;
      if(!isset($_REQUEST['number_of_files']) or $_REQUEST['number_of_files']<=0)
      {
         return $this->writeUploadAmountSection($snum);
      }
      $hed_text = $this->getDisplayOption('attachments_sectionhead_text'). " - <span class='publish-require'>".$this->getDisplayOption('required_text')."</span>";
      $intro_text = "<div class='change-publish-uploads'>".$this->getUploadAmountSelect($_REQUEST['number_of_files'], $this->getDisplayOption("change_attachments_button"))."</div>";
      $intro_text .= cleanseNumericalQueryField($_REQUEST['number_of_files'])." Uploads Selected<br /><br />";
      $intro_text .= "<P>".$this->getDisplayOption('attachments_section_intro')."</P>";
      $intro_text .= $this->getLimitsAsHTMLTable();
      $intro_text .= "<P>".$this->getDisplayOption('attachments_kindness_plea')."</P>";
      $this->writeSectionHeader($snum, $hed_text, $intro_text);
      $num = (isset($_REQUEST['number_of_files']))? cleanseNumericalQueryField($_REQUEST['number_of_files']) : 0;
      echo "<input type=hidden name='number_of_files_selected' value='$num'>\n";

      if($this->content_type == 'comment') $content =& $this->comment;
      else $content =& $this->story;
      if($num>0)
      {
         for($i=1;$i<=$num;$i++)
         {
            if ($i == 1 )
            if(isset($_FILES['file_'.$i]['name'])) $val = $_FILES['file_'.$i]['name'];
            else $val = "";
            if(isset($_REQUEST['file_'.$i.'_description'])) $descr = $_REQUEST['file_'.$i.'_description'];
            else $descr = "";

	    // Consider adding hidden field MAX_FILE_SIZE in bytes. It cannot exceed upload_max_filesize
	    // It must be added before the first file field.
            if ($i == 1 && $this->max_uploads_filesize > 0 ) {
                $tp_payload = "<input type='hidden' name='MAX_FILE_SIZE' value='$this->max_uploads_filesize'>\n";
                $tp_payload .= "<input type=file name='file_".$i."' size='30' value='$val'>\n";
            } else {
                $tp_payload = "<input type=file name='file_".$i."' size='30' value='$val'>\n";
            }
            $this->writeSectionEntry('uploadfile', $tp_payload, true, false, " $i");
            $tp_payload = "<input type=text name='file_".$i."_description' value='".htmlspecialchars($descr, ENT_QUOTES)."' size='70' MAXLENGTH='255' >\n";
            $this->writeSectionEntry('uploadfile_description', $tp_payload, false, false, " $i");
         }
      }
      return ++$snum;
   }
 
   // This copies both the embedded video and audio attachments from this object to the separate elements
   // for embedded video and audio for a story or comment to prepare it for the displayPreview.
   function transferVideoAudioAttachments($is_story)
   {
      if ($is_story == true) {
         $this->story->embed_video_attachments = array();
         $this->story->embed_audio_attachments = array();
      } else {
         $this->comment->embed_video_attachments = array();
         $this->comment->embed_audio_attachments = array();
      }

      for($i=0;$i< count($this->attachments);$i++)
      {
          if ($this->attachments[$i]->isEmbeddedVideo() == true)
          {
              if ($is_story == true) array_push($this->story->embed_video_attachments, $this->attachments[$i]);
	      else array_push($this->comment->embed_video_attachments, $this->attachments[$i]);
          }
          elseif ($this->attachments[$i]->isEmbeddedAudio() == true)
          {
              if ($is_story == true) array_push($this->story->embed_audio_attachments, $this->attachments[$i]);
	      else array_push($this->comment->embed_audio_attachments, $this->attachments[$i]);
          }
          else {
              // There should be nothing here. 
          }
      }
   }

 
   /*********************************************************************/
   /* preview page drawing functions                             */
   /*********************************************************************/
   function writePreviewPage()
   {
      global $system_config,$editor_session;

      // Preview mode is used to determine whether to preview as an article or feature as
      // displayed on a feature newswire.
      $t_preview_article = 1;

      if ($this->content_type == 'story') {
          if ($this->story->type_id == $system_config->feature_type_id) {
              if (isset($_REQUEST['preview_mode']) && $_REQUEST['preview_mode'] == 'feature') $t_preview_article = 0;
          }
      }

      if ($t_preview_article == 1) {
          require_once("objects/indyruntime/articledisplay.inc");
          $article_viewer = new articleDisplay($this->site_settings['articleobject']);
          $article_viewer->setFilters(Array("*"), Array("*"), Array("*"), Array("*"), Array(""));
          $article_viewer->setAdminOptions(Array("includerelatedlink" => 'true'));
          $article_viewer->setUserOptions(Array("subtitledetails" => Array("0" => "context", "1" => "date", "2" => "author", "3" => "author_contact"), "maximgheight" => "0", "maximgwidth" => "460", "maxsize" => "30"));
          $article_viewer->setDisplayOptions();
      } else {
          require_once("objects/indyruntime/featuredisplay.inc");
          $feature_viewer = new featureDisplay($this->site_settings['featureobject']);
          $feature_viewer->setFilters(Array("*"), Array("*"), Array("*"), Array("*"), Array(""));
          $feature_viewer->setAdminOptions(Array("includerelatedlink" => 'true'));
          $feature_viewer->setUserOptions(Array("subtitledetails" => Array("0" => "context", "1" => "date", "2" => "author", "3" => "author_contact"), "maximgheight" => "0", "maximgwidth" => "460", "maxsize" => "30"));
          $feature_viewer->setDisplayOptions();
      }
      $this->writePreviewIntro();
      $this->showLock();
      if($this->content_type == 'comment')
      {
         $this->comment->format();
         $this->comment->author_email = $this->encodeEmailAddress($this->comment->author_email); 
         $this->comment->attachments_loaded = true;
	 // Transfer the attachments onto the comment into separate areas for embedded video and audio
         $this->transferVideoAudioAttachments(false);
         $article_viewer->displayPreview($this->comment, $this->content_type);
         $this->comment->author_email = $this->getEmailAddress($this->comment->author_email); 
      }
      else
      {
         $this->story->format();
         $this->story->attachments_loaded = true;
	 // Transfer the attachments onto the story into separate areas for embedded video and audio
         $this->transferVideoAudioAttachments(true);
         $this->story->author_email = $this->encodeEmailAddress($this->story->author_email); 
         if ($t_preview_article == 1) {
             $article_viewer->displayPreview($this->story, $this->content_type);
         } else {
             $feature_viewer->writeStorySummaryContent($this->story);
         }
         $this->story->author_email = $this->getEmailAddress($this->story->author_email); 
      }
      $this->writePublishForm(true);
   }

   function writePreviewIntro()
   {
      global $system_config;
      echo "<div class='publish-header'>\n";
      echo "<h1 class='publish-header'>".$this->getDisplayOption('preview_text')."</h1>\n";
      echo "<p class='publish-header'>".$this->getDisplayOption('preview_intro')."</p>\n";
      if($this->user_msg != "")
      {
         echo "<p class='publish-header'>".$this->user_msg."</p>\n";
      }
      echo "</div>\n";
   }

   function writeConfirmPreviewSection()
   {
      global $graphics_store, $editor_session, $oscailt_basic_config;
      echo "<div class='editorconfirm'>";
      echo "<div class='confirmbox' align='center'>";
      $cancel_text = $this->getDisplayOption("cancel_button");
      $save_text = $this->getDisplayOption("confirm_preview_button");
      $edit_again_text = $this->getDisplayOption("edit_again");
      $edit_again_lnk = "<a href='#edit-again'>$edit_again_text</a>";
      echo "<p class='confirmheader'>$save_text</p>";
      echo "<div class='notifybox' align='center'>";
      if (isset($_REQUEST['send_mail']) && $_REQUEST['send_mail'] == 'on')
      {
         echo "<input type='hidden' name='send_mail' value='on'>";
         $reason = "";
         if(isset($_REQUEST['action_reason']))
         {
            echo "<b>".$this->getDisplayOption("actionreason_title")."</b><br>";
            $reason = $_REQUEST['action_reason'];
            echo "<TEXTAREA rows=6 cols=60 name='action_reason'>".htmlspecialchars($reason, ENT_QUOTES)."</TEXTAREA>";
         }
      }
      if ($editor_session->isSessionOpen() && $this->content_type == 'story') {
          if ($this->isNewStory() && $this->story->type_id != $oscailt_basic_config['feature_type_id'] && $this->getDisplayOption('auto_hide_new_stories') != 'true') {
          echo "Set story as: &nbsp; Visible <input type=radio name=set_story_mode value='visible' checked> &nbsp;  Pending <input type=radio name=set_story_mode value='pending' > &nbsp;  Hidden <input type=radio name=set_story_mode value='hide' >";
          }
      }
      echo "<div class='editorconfirmbuttons'>";
      echo "<input type=submit name='cancel' value='<< $cancel_text'> ";
      echo "<input type='submit' name='preview_confirmed' value='$save_text >>'>";
      echo " $edit_again_lnk</div>\n";
      echo "</div></div></div>";
      echo "<div class='edit-again'>";
      echo "<h1 class='publish-header'><a name='edit-again'>$edit_again_text</a></h1>";
   }

   // This looks after writing the hidden variables for embedded video and audio for stories and comments.
   // If input vars are false, they are comment and audio.
   function writeEmbeddedItemAsHidden($is_story, $is_embedded_video)
   {
      // Handle video hidden variables first. We will only ever do either a story or a comment never together
      // but we can do embedded video and audio together.
      if ($is_embedded_video == true) {

         echo "<input type='hidden' name='number_embed_video' value='".$_REQUEST['number_embed_video']."'>\n";
         $hide_prefix = "<input type='hidden' name='youtube_id_";
         // Attachment Id and Timeposted -used when an editor is editing ....
         $ds_prefix = "<input type='hidden' name='embed_vid_desc_";
         $id_prefix = "<input type='hidden' name='attachment_id_v";
         $tp_prefix = "<input type='hidden' name='attachment_tp_v";

         if ($is_story == true)
            $maxVideo = $this->getDisplayOption("max_embedvideo_in_stories");
         else
            $maxVideo = $this->getDisplayOption("max_embedvideo_in_comments");

         for ($i_vid = 1;$i_vid <=$maxVideo; $i_vid++)
         {
            if ($is_story == true) {
               if ($i_vid == 1) echo $hide_prefix . $i_vid."' value='".htmlspecialchars($this->story->youtube_id_1, ENT_QUOTES)."'>\n";
               if ($i_vid == 2) echo $hide_prefix . $i_vid."' value='".htmlspecialchars($this->story->youtube_id_2, ENT_QUOTES)."'>\n";
               if ($i_vid == 3) echo $hide_prefix . $i_vid."' value='".htmlspecialchars($this->story->youtube_id_3, ENT_QUOTES)."'>\n";
               if ($i_vid == 4) echo $hide_prefix . $i_vid."' value='".htmlspecialchars($this->story->youtube_id_4, ENT_QUOTES)."'>\n";
               if ($i_vid == 5) echo $hide_prefix . $i_vid."' value='".htmlspecialchars($this->story->youtube_id_5, ENT_QUOTES)."'>\n";
            }
            else
            {
               if ($i_vid == 1) echo $hide_prefix . $i_vid."' value='".htmlspecialchars($this->comment->youtube_id_1, ENT_QUOTES)."'>\n";
               if ($i_vid == 2) echo $hide_prefix . $i_vid."' value='".htmlspecialchars($this->comment->youtube_id_2, ENT_QUOTES)."'>\n";
               if ($i_vid == 3) echo $hide_prefix . $i_vid."' value='".htmlspecialchars($this->comment->youtube_id_3, ENT_QUOTES)."'>\n";
               if ($i_vid == 4) echo $hide_prefix . $i_vid."' value='".htmlspecialchars($this->comment->youtube_id_4, ENT_QUOTES)."'>\n";
               if ($i_vid == 5) echo $hide_prefix . $i_vid."' value='".htmlspecialchars($this->comment->youtube_id_5, ENT_QUOTES)."'>\n";
            }
   
            $tmp_name = "embed_vid_desc_" .$i_vid;
            if (isset($_REQUEST[$tmp_name])) {
                echo $ds_prefix . $i_vid."' value='".$_REQUEST[$tmp_name]."'>\n";
            }

            $tmp_name = "attachment_id_v" .$i_vid;
            if (isset($_REQUEST[$tmp_name])) {
                echo $id_prefix . $i_vid."' value='".$_REQUEST[$tmp_name]."'>\n";
            }
            else continue;

            // If the above is not set, then it's pointless testing this.
            $tmp_name = "attachment_tp_v" .$i_vid;
            if (isset($_REQUEST[$tmp_name])) {
                echo $tp_prefix . $i_vid."' value='".$_REQUEST[$tmp_name]."'>\n";
            }
         }
      }

      // Do the audio. This will be done on the 2nd call.
      if ($is_embedded_video != true) {

         echo "<input type='hidden' name='number_embed_audio' value='".$_REQUEST['number_embed_audio']."'>\n";
         $hide_prefix = "<input type='hidden' name='audio_id_";
         $ds_prefix = "<input type='hidden' name='embed_audio_desc_";
         $id_prefix = "<input type='hidden' name='attachment_id_a";
         $tp_prefix = "<input type='hidden' name='attachment_tp_a";

         if ($is_story == true)
            $maxAudio = $this->getDisplayOption("max_embedaudio_in_stories");
         else
            $maxAudio = $this->getDisplayOption("max_embedaudio_in_comments");

         for ($i_aud = 1;$i_aud <=$maxAudio; $i_aud++)
         {
            if ($is_story == true) {
               if ($i_aud == 1) echo $hide_prefix . $i_aud."' value='".htmlspecialchars($this->story->audio_id_1, ENT_QUOTES)."'>\n";
               if ($i_aud == 2) echo $hide_prefix . $i_aud."' value='".htmlspecialchars($this->story->audio_id_2, ENT_QUOTES)."'>\n";
               if ($i_aud == 3) echo $hide_prefix . $i_aud."' value='".htmlspecialchars($this->story->audio_id_3, ENT_QUOTES)."'>\n";
               if ($i_aud == 4) echo $hide_prefix . $i_aud."' value='".htmlspecialchars($this->story->audio_id_4, ENT_QUOTES)."'>\n";
               if ($i_aud == 5) echo $hide_prefix . $i_aud."' value='".htmlspecialchars($this->story->audio_id_5, ENT_QUOTES)."'>\n";
            }
            else
            {
               if ($i_aud == 1) echo $hide_prefix . $i_aud."' value='".htmlspecialchars($this->comment->audio_id_1, ENT_QUOTES)."'>\n";
               if ($i_aud == 2) echo $hide_prefix . $i_aud."' value='".htmlspecialchars($this->comment->audio_id_2, ENT_QUOTES)."'>\n";
               if ($i_aud == 3) echo $hide_prefix . $i_aud."' value='".htmlspecialchars($this->comment->audio_id_3, ENT_QUOTES)."'>\n";
               if ($i_aud == 4) echo $hide_prefix . $i_aud."' value='".htmlspecialchars($this->comment->audio_id_4, ENT_QUOTES)."'>\n";
               if ($i_aud == 5) echo $hide_prefix . $i_aud."' value='".htmlspecialchars($this->comment->audio_id_5, ENT_QUOTES)."'>\n";
            }

	       
            $tmp_name = "embed_audio_desc_" .$i_aud;
            if (isset($_REQUEST[$tmp_name])) {
                echo $ds_prefix . $i_aud."' value='".$_REQUEST[$tmp_name]."'>\n";
            }

            $tmp_name = "attachment_id_a" .$i_aud;
            if (isset($_REQUEST[$tmp_name])) {
                echo $id_prefix . $i_aud."' value='".$_REQUEST[$tmp_name]."'>\n";
            }
            else continue;

            $tmp_name = "attachment_tp_a" .$i_aud;
            if (isset($_REQUEST[$tmp_name])) {
                echo $tp_prefix . $i_aud."' value='".$_REQUEST[$tmp_name]."'>\n";
            }
         }
      }
   }

   function writeItemAsHidden()
   {
      global $system_config;
      echo "<input type='hidden' name='accept_terms' value='on'>\n";
      if (isset($_REQUEST['email_status']) && $_REQUEST['email_status'] == 'on')
          echo "<input type='hidden' name='email_status' value='on'>\n";

      
      if($this->content_type == 'comment')
      {
         if($this->comment->comment_id!=null && $this->comment->comment_id !=0)
         {
            echo "<input type='hidden' name='comment_id' value='".htmlspecialchars($this->comment->comment_id, ENT_QUOTES)."'>\n";
         }
         echo "<input type='hidden' name='story_id' value='".htmlspecialchars($this->comment->story_id, ENT_QUOTES)."'>\n";
         echo "<input type='hidden' name='related_link' value='".htmlspecialchars($this->comment->related_link, ENT_QUOTES)."'>\n";

         if ($this->getDisplayOption("youtube_id_allow_in_comments") == 'on') {
             $this->writeEmbeddedItemAsHidden(false, true);
         }
         if ($this->getDisplayOption("audio_id_allow_in_comments") == 'on') {
             $this->writeEmbeddedItemAsHidden(false, false);
         }

         echo "<input type='hidden' name='author_name' value='".htmlspecialchars($this->comment->author_name,  ENT_QUOTES)."'>\n";
         echo "<input type='hidden' name='author_organisation' value='".htmlspecialchars($this->comment->author_organisation,    ENT_QUOTES)."'>\n";
         echo "<input type='hidden' name='author_email' value='".htmlspecialchars($this->comment->author_email, ENT_QUOTES)."'>\n";
         echo "<input type='hidden' name='author_phone' value='".htmlspecialchars($this->comment->author_phone, ENT_QUOTES)."'>\n";
         echo "<input type='hidden' name='author_address' value='".htmlspecialchars($this->comment->author_address, ENT_QUOTES)."'>\n";
         echo "<input type='hidden' name='comment_title' value='".htmlspecialchars($this->comment->comment_title,   ENT_QUOTES)."'>\n";
         echo "<input type='hidden' name='comment_content' value='".htmlspecialchars($this->comment->comment_content, ENT_QUOTES)."'>\n";
         echo "<input type='hidden' name='comment_content_html' value='".htmlspecialchars($this->comment->comment_content_html,  ENT_QUOTES)."'>\n";
         if($this->comment->time_posted!=null)
         {
            echo "<input type='hidden' name='time_posted' value='".htmlspecialchars(date('D, d M Y H:i:s',$this->comment->time_posted+$system_config->timezone_offset), ENT_QUOTES)."'>";
         }
      }
      else
      {
         if($this->story->story_id !=null and $this->story->story_id !=0)
         {
            echo "<input type='hidden' name='story_id' value='".htmlspecialchars($this->story->story_id, ENT_QUOTES)."'>\n";
         }
         echo "<input type='hidden' name='topic_id'   value='".htmlspecialchars($this->story->topic_id, ENT_QUOTES)."'>\n";
         echo "<input type='hidden' name='region_id'  value='".htmlspecialchars($this->story->region_id, ENT_QUOTES)."'>\n";
         echo "<input type='hidden' name='type_id'    value='".htmlspecialchars($this->story->type_id, ENT_QUOTES)."'>\n";
         echo "<input type='hidden' name='language_id'  value='".htmlspecialchars($this->story->language_id, ENT_QUOTES)."'>\n";
         echo "<input type='hidden' name='related_link' value='".htmlspecialchars($this->story->related_link, ENT_QUOTES)."'>\n";

	 
         if ($this->getDisplayOption("youtube_id_allow_in_stories") == 'on') {
             $this->writeEmbeddedItemAsHidden(true, true);
         }
         if ($this->getDisplayOption("audio_id_allow_in_stories") == 'on') {
             $this->writeEmbeddedItemAsHidden(true, false);
         }

         echo "<input type='hidden' name='author_name' value='".htmlspecialchars($this->story->author_name, ENT_QUOTES)."'>\n";
         echo "<input type='hidden' name='author_organisation' value='".htmlspecialchars($this->story->author_organisation, ENT_QUOTES)."'>\n";
         echo "<input type='hidden' name='author_email' value='".htmlspecialchars($this->story->author_email, ENT_QUOTES)."'>\n";
         echo "<input type='hidden' name='author_phone' value='".htmlspecialchars($this->story->author_phone, ENT_QUOTES)."'>\n";
         echo "<input type='hidden' name='author_address' value='".htmlspecialchars($this->story->author_address, ENT_QUOTES)."'>\n";
         echo "<input type='hidden' name='story_title' value='".htmlspecialchars($this->story->story_title, ENT_QUOTES)."'>\n";
         echo "<input type='hidden' name='story_subtitle' value='".htmlspecialchars($this->story->story_subtitle, ENT_QUOTES)."'>\n";
         echo "<input type='hidden' name='story_summary' value='".htmlspecialchars($this->story->story_summary, ENT_QUOTES)."'>\n";
         echo "<input type='hidden' name='story_summary_html' value='".htmlspecialchars($this->story->story_summary_html,   ENT_QUOTES)."'>\n";
         echo "<input type='hidden' name='story_content' value='".htmlspecialchars($this->story->story_content, ENT_QUOTES)."'>\n";
         echo "<input type='hidden' name='story_content_html' value='".htmlspecialchars($this->story->story_content_html, ENT_QUOTES)."'>\n";
         echo "<input type='hidden' name='story_content_extra' value='".htmlspecialchars($this->story->story_content_extra, ENT_QUOTES)."'>\n";
         echo "<input type='hidden' name='story_content_extra_html' value='".htmlspecialchars($this->story->story_content_extra_html, ENT_QUOTES)."'>\n";
         if($this->story->hide_summary_on_feature_page)
         {
            echo "<input type='hidden' name='hide_summary_on_feature_page' value='on'>\n";
         }
         if($this->story->hide_summary_on_story_page)
         {
            echo "<input type='hidden' name='hide_summary_on_story_page' value='on'>\n";
         }
         if($this->story->hide_attachments_on_feature_page)
         {
            echo "<input type='hidden' name='hide_attachments_on_feature_page' value='on'>\n";
         }
         if($this->story->event_time!=null && $this->story->event_time!=0)
         {
            $selected_min = date("i",$this->story->event_time+$system_config->timezone_offset);
            $selected_hr  = date("H",$this->story->event_time+$system_config->timezone_offset);
            $selected_day = date("j",$this->story->event_time+$system_config->timezone_offset);
            $selected_month = date("n",$this->story->event_time+$system_config->timezone_offset);
            $selected_year = date("Y",$this->story->event_time+$system_config->timezone_offset);
            echo "<input type='hidden' name='event_time_min' value='".htmlspecialchars($selected_min, ENT_QUOTES)."'>";
            echo "<input type='hidden' name='event_time_hr' value='".htmlspecialchars($selected_hr, ENT_QUOTES)."'>";
            echo "<input type='hidden' name='event_time_day' value='".htmlspecialchars($selected_day, ENT_QUOTES)."'>";
            echo "<input type='hidden' name='event_time_month' value='".htmlspecialchars($selected_month, ENT_QUOTES)."'>";
            echo "<input type='hidden' name='event_time_year' value='".htmlspecialchars($selected_year, ENT_QUOTES)."'>";

         }
         if($this->story->time_posted!=null)
         {
            echo "<input type='hidden' name='time_posted' value='".htmlspecialchars(date('D, d M Y H:i:s',$this->story->time_posted+$system_config->timezone_offset), ENT_QUOTES)."'>";
         }
      }

      $formValidationData = new formValidator();
      $hiddenData = $formValidationData->rewriteHiddenData();
      echo $hiddenData;

      // Captcha variable
      if($this->use_captcha == true)
      {
         echo "<input type='hidden' name='cap_digits1' value='".$_REQUEST['cap_digits1']."'>";
         echo "<input type='hidden' name='cap_digits2' value='".$_REQUEST['cap_digits2']."'>";
      }

      //if ($this->getDisplayOption("allow_public_edits") == 'true' && trim($this->getDisplayOption("selected_public_edit_stories")) != '') {
      if ($this->allow_public_edits == true && isset($_REQUEST['publish_password'])) {
          echo "<INPUT type='hidden' name='publish_password' value='".$_REQUEST['publish_password']."'>";
      }

      if ($this->getDisplayOption("youtube_id_allow_in_stories") == 'on' || $this->getDisplayOption("youtube_id_allow_in_comments") == 'on') {
          // It has to be here so it is outside both the story and comment if blocks.
          echo "<input type='hidden' name='videoType' value='".$_REQUEST['videoType']."'>";
      }
   }

   /*********************************************************************/
   /* Result Page drawing functions                              */
   /*********************************************************************/
   function writeFailurePage()
   {
      writeFailureMessage($this->getDisplayOption("publication_failed_text"), $this->user_msg);
   }

   function getFeedHTML($story_id)
   {
      global $graphics_store, $system_config;

      $rss_feed_id = $this->getDisplayOption("rssfeedexportobject");
      $atom_feed_id = $this->getDisplayOption("atomfeedexportobject");

      $ret = "";

      if($rss_feed_id > 0 || $atom_feed_id > 0)
      {
      writeHelpJS();
      $site_link = "http://".$system_config->site_url.'/';
         $feed_expl = htmlspecialchars($this->getDisplayOption("feed_text"), ENT_QUOTES);
         $feed_help = $this->getDisplayOption("feed_publish_help");
         $help_html = getHelpHTML($feed_help, false, true);
         $ret = "<p class='publish-header'><small>".$feed_expl.$help_html."</small></p>";
      $ret .= "<div class='user-link'>";
         if($atom_feed_id > 0)
         {
            $atom_link = $this->getSimpleLink($atom_feed_id);
            $atom_link = addLinkParameters($atom_link, array("story_id=$story_id"));
            $atom_txt = htmlspecialchars($this->getDisplayOption("atom_text"), ENT_QUOTES);
            $ret .= "<img class='icon' src='".$graphics_store."atomcommentsbutton.png' alt='atom feed of comments' /> ".$site_link.$atom_link."<br>";
         }
         if($rss_feed_id > 0)
         {
            $rss_link = $this->getSimpleLink($rss_feed_id);
            $rss_link = addLinkParameters($rss_link, array("story_id=$story_id"));
            $rss_txt = htmlspecialchars($this->getDisplayOption("rss_text"), ENT_QUOTES);
            $ret .= "<img class='icon' src='".$graphics_store."rsscommentsbutton.png' alt='rss feed of comments' /> ".$site_link.$rss_link;
         }
      $ret .= "</div>";
      }
      return $ret;
   }

   function writeResultsPage()
   {
      global $editor_session, $system_config, $graphics_store;
      $link_text = "";
      if($this->content_type == 'comment')
      {
         $story_id = $this->comment->story_id;
         $lnk = $this->getCommentLink();
         if($this->comment->hidden == true  && !$editor_session->isSessionOpen())
         {
            $result_text = $this->getDisplayOption("comment_submitted_title");
            $explanation_text = $this->getDisplayOption("comment_submitted_text");
         }
         else
         {
            $result_text = $this->getDisplayOption("comment_published_title");
            $explanation_text = $this->getDisplayOption("comment_published_text");
            $link_text .= "<a href='$lnk'>".$this->getDisplayOption("view_comment_text")."</a><br />";
         }
         $user_lnk_txt = $this->getDisplayOption("userlink_text");
         $user_lnk = "http://".$system_config->site_url.'/'.$lnk;
         $lnk_icon = "<img src='$graphics_store"."copylinktag.gif' alt='link' />";

      }
      else
      {
         $story_id = $this->story->story_id;
         if($this->story->hidden == true && !$editor_session->isSessionOpen())
         {
            if($this->story->type_id==$system_config->feature_type_id && $this->new_feature==true)
            {
               $result_text = $this->getDisplayOption("feature_created_title");
               $explanation_text = $this->getDisplayOption("feature_created_text");
            }
            elseif($this->story->type_id==$system_config->event_type_id)
            {
               $result_text = $this->getDisplayOption("event_submitted_title");
               $explanation_text = $this->getDisplayOption("event_submitted_text");
            }
            else
            {
               $result_text = $this->getDisplayOption("story_submitted_title");
               $explanation_text = $this->getDisplayOption("story_submitted_text");
            }
         }
         else
         {
            if($this->story->type_id==$system_config->event_type_id)
            {
               $result_text = $this->getDisplayOption("event_published_title");
               $explanation_text = $this->getDisplayOption("event_published_text");
               $lnk = $this->getEventCalendarLink();
               $link_text .= "<a href='$lnk'>".$this->getDisplayOption("view_events_text")."</a><br />";
            }
            elseif($this->story->type_id==$system_config->feature_type_id)
            {
               $result_text = $this->getDisplayOption("feature_published_title");
               $explanation_text = $this->getDisplayOption("feature_published_text");
               if($this->new_feature==true)
               {
                  $explanation_text .= $this->getDisplayOption("hidden_newfeature_text");
               }
               $lnk = $this->getFeatureLink();
               $link_text .= "<a href='$lnk'>".$this->getDisplayOption("view_feature_text")."</a><br />";
            }
            else
            {
               $result_text = $this->getDisplayOption("story_published_title");
               $explanation_text = $this->getDisplayOption("story_published_text");
            }
            $lnk = $this->getNewswireLink();
            $link_text .= "<a href='$lnk'>".$this->getDisplayOption("view_newswire_text")."</a><br />";
            $lnk = $this->getStoryLink();
            $link_text .= "<a href='$lnk'>".$this->getDisplayOption("view_story_text")."</a><br />";
         }
         $lnk = $this->getStoryLink();
         $user_lnk_txt = $this->getDisplayOption("userlink_text");
         $user_lnk = "http://".$system_config->site_url.'/'.$lnk;
         $lnk_icon = "<img src='$graphics_store"."copylinktag.gif' alt='link' />";

      }
      $icon = "<p align='center'><img src='$graphics_store"."oscailtlogo100.gif' alt='result' style='border: #000 1pt solid'></p>";
      echo "<div class='user-message'>$icon\n";
      echo "<h3 align='center' class='publish-header'>$result_text</h3>\n";
      // List the attachments if any.
      if(count($this->attachments) > 0)
      {
         echo "<p class='publish-header'>". count($this->attachments)." Attachment(s) Published</p>\n";
         echo "<p class='publish-header'>";
         for($j=0;$j<count($this->attachments);$j++)
         {
            echo ($j+1).". File = " .$this->attachments[$j]->attachment_file." Description: ".$this->attachments[$j]->description . "<br>";
         }
         echo "</p>\n";
      }

      if($this->user_msg != "")
      {
         echo "<p class='publish-header'>".$this->user_msg."</p>\n";
      }
      echo "<p class='publish-header'>$explanation_text</p>\n";
      echo "<p class='publish-header'>$link_text</p>\n";
      echo "<p class='publish-header'><small>$user_lnk_txt</small></p>\n";
      echo "<div class='user-link'>$lnk_icon $user_lnk</div>";
      echo $this->getFeedHTML($story_id);


      if ($this->content_type == 'story' && ($system_config->enable_public_editing == true || $this->allow_public_edits == true))
      {
	  echo "<BR>Your session is [".$_REQUEST['PHPSESSID']."]<BR>";

	  $t_public_edits = new EditEntriesList();
	  $t_public_edits->load();

	  if ($t_public_edits->isStoryInList($this->story->story_id) == true) {
	      $t_public_edits->add($this->story->story_id,time(), "system", "", $_REQUEST['PHPSESSID']);
	      $t_public_edits->discardOldRecords();
	      $t_public_edits->save();
              echo "<p class='publish-header'>You can edit this story for up to ".($t_public_edits->edit_hours/3600). " hour(s).</p>\n";
          } else {
	      $t_password = generatePassword();
	      // Password wont be updated if it already exists
	      $t_public_edits->add($this->story->story_id,time(), "system", $t_password, $_REQUEST['PHPSESSID']);
	      $t_public_edits->discardOldRecords();
	      $t_public_edits->save();
              echo "<p class='publish-header'>You can edit this story for up to ".($t_public_edits->edit_hours/3600). " hour(s) with this password <b>".$t_password."</b></p>\n";
          }
      }

      echo "</div>";
   }
}
?>
