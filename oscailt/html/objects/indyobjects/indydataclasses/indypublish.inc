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

class indyPublishObject extends indySiteModuleObject
{
   var $container_settables = array(
      "title" => 'text',
      "hsize" => 'select',
   );

   var $runtime_requires = array("publishdisplay.inc");

   var $display_options = array(
      "Short Title",
      "Icon",
      "Icon And Title",
      "Long Title",
      "Full Page"
   );

    function indyPublishObject ($a, $b)
    {
        $this->indySiteModuleObject($a, $b);
    }

    function getBasicFilterPrefix()
   {
      return "publishfilter";
   }

   function getDefaultDisplayMode($depth)
   {
      if($depth == FULL_DEPTH) $display = "Full Page";
      else $display = "Short Title";
      return $display;
   }

   function getFullPageSettingPHP($title)
   {
      $full_str = $this->assembleAdminOptionString($title);
      $full_str .= '$site_module->setUserOptions(array());'."\n";
      return $full_str;
   }

   function assembleAdminOptionString($title)
   {
      $basic_text_fields = array(
      "header",
      "introtext",
      "no_session_text",
      "no_downgrade_permission_text",
      "no_upgrade_permission_text",
      "no_editfeature_permission_text",
      "no_editstory_permission_text",
      "invalid_eventdate_text",
      "invalid_content_text",
      "invalid_summary_text",
      "invalid_input_text",
      "invalid_token_text",
      "invalid_no_cookie_text",
      "seconds_till_publish_text",
      "invalid_token_notpublished_text",
      "story_submitted_text",
      "comment_submitted_text",
      "comment_submitted_title",
      "comment_published_text",
      "event_published_text",
      "story_published_text",
      "feature_published_text",
      "story_submitted_title",
      "event_published_title",
      "story_published_title",
      "feature_published_title",
      "comment_published_title",
      "new_text",
      "edit_text",
      "event_text",
      "story_text",
      "new_intro",
      "submit_sectionhead_text",
      "submit_section_intro",
      "legal_sectionhead_text",
      "submit_nopreview_text",
      "nopreview_attachment_text",
      "submit_text",

      "extra_feature_header_text",
      "extra_feature_header_intro",
      "content_sectionhead_text",
      "contact_sectionhead_text",
      "contact_section_intro",
      "contentsection_prompt",
      "contentsection_editorprompt",
      "contentsection_editorfeatureprompt",
      "timeposted_title",
      "timeposted_prompt",
      "timeposted_help",
      "category_sectionhead_text",
      "category_section_intro",
      "topic_title",
      "topic_prompt",
      "topic_help",
      "topic_select_title",
      "type_title",
      "type_prompt",
      "type_help",
      "type_select_title",
      "language_title",
      "language_prompt",
      "language_help",
      "language_select_title",
      "region_title",
      "region_prompt",
      "region_help",
      "region_select_title",
      "files_sectionhead_text",
      "upload_button_text",
      "required_text",
      "notrequired_text",
      "choose_uploads_intro",
      "choose_uploads_title",
      "choose_uploads_prompt",
      "choose_uploads_help",
      "attachments_sectionhead_text",
      "attachments_section_intro",
      "attachments_chosen_text",
      "change_attachments_button",
      "attachments_kindness_plea",
      'uploadfile_title',
      'uploadfile_prompt',
      'uploadfile_help',
      'uploadfile_description_title',
      'uploadfile_description_prompt',
      'uploadfile_description_help',
      "maxsize_text",
      "maxwidth_text",
      "filetypes_text",
      "imagefiles_text",
      "imagefiles_help",
      "audiofiles_text",
      "audiofiles_help",
      "videofiles_text",
      "videofiles_help",
      "miscfiles_text",
      "miscfiles_help",
      "not_allowed_upload_text",
      "authorname_title",
      "authorname_prompt",
      "authorname_help",
      "authoremail_title",
      "authoremail_prompt",
      "authoremail_help",
      "authorphone_title",
      "authorphone_prompt",
      "authorphone_help",
      "authoraddress_title",
      "authoraddress_prompt",
      "authoraddress_help",
      "authororganisation_title",
      "authororganisation_prompt",
      "authororganisation_help",
      "title_title",
      "title_prompt",
      "title_help",
      "relatedlink_title",
      "relatedlink_prompt",
      "relatedlink_help",
      "youtube_id_allow_in_stories",
      "youtube_id_allow_in_comments",
      "youtube_id_link_title",
      "youtube_id_link_prompt",
      "youtube_id_link_help",
      "max_embedvideo_in_stories",
      "max_embedvideo_in_comments",
      "audio_id_allow_in_stories",
      "audio_id_allow_in_comments",
      "audio_id_link_title",
      "audio_id_link_prompt",
      "audio_id_link_help",
      "max_embedaudio_in_stories",
      "max_embedaudio_in_comments", 
      "termsandconditions_title",
      "termsandconditions_prompt",
      "termsandconditions_help",
      "legal_section_intro",
      "captcha_prompt_text",
      "subtitle_title",
      "subtitle_prompt",
      "subtitle_help",
      "summary_title",
      "summary_prompt",
      "summary_help",
      "content_title",
      "content_prompt",
      "content_help",
      "send_notification_title",
      "send_notification_prompt",
      "send_notification_help",
      "extracontent_title",
      "extracontent_prompt",
      "extracontent_help",
      "footer",
      "publish_block",
      "publish_writingtime",
      "comment_max_attachments",
      "story_max_attachments",
      "editor_max_attachments",
      'image_attachment_max_width',
      'publishbutton_text',
      'previewbutton_text',
      'publishcomments_writingtime',
      'publishcomments_block',
      'comment_text',
      "newcomment_intro",
      'invalid_commentnostory_text',
      'invalid_commentstoryid_text',
      'invalid_commentstorylocked_text',
      'editorreason_sectionhead_text',
      'editorreason_sectionhead_intro',
      'actionreason_title',
      'actionreason_prompt',
      'actionreason_help',
      'preview_text',
      'preview_intro',
      'confirm_preview_button',
      'edit_again',
      'feature_page_text',
      'story_page_text',
      'view_comment_text',
      'view_events_text',
      'view_feature_text',
      'view_newswire_text',
      'view_story_text',
      'commentdate_format',
      'userlink_text',
      'feature_created_title',
      'feature_created_text',
      'cancel_button',
      "hidesummary_title",
      "hidesummary_prompt",
      "hidesummary_help",
      "hide_summary_feature_page_title",
      "hide_summary_feature_page_prompt",
      "hide_summary_feature_page_help",
      'hideattachmentsonfeaturepage_title',
      'hideattachmentsonfeaturepage_prompt',
      'hideattachmentsonfeaturepage_help',
      "feature_content_prefill",
      "no_publish_permission_text",
      "this_form_not_for_publishing",
      "only_editors_may_publish",
      "only_editors_may_comment",
      "this_form_not_for_comments",
      "publication_failed_text",
      "edit_intro",
      "termsandconditions_popup",
      "categories_popup",
      "who_can_publish",
      "who_can_comment",
      "rssfeedexportobject",
      "atomfeedexportobject",
      "feed_text",
      "feed_publish_help",
      'include_numeric_captcha',
      'selected_public_edit_stories'
       );

      $basic_checkbox_fields = array(
      "include_extra_details",
      "include_legal_section",
      'allow_story_image_attachments',
      'allow_story_video_attachments',
      'allow_story_audio_attachments',
      'allow_story_miscellaneous_attachments',
      'allow_comment_image_attachments',
      'allow_comment_video_attachments',
      'allow_comment_audio_attachments',
      'allow_comment_miscellaneous_attachments',
      'force_feature_email_notifications',
      'force_email_notifications',
      'auto_hide_new_comments',
      'auto_hide_new_stories',
      'allow_public_edits',
      'force_hidden_email_notifications',
      "force_submissions_email_notifications",
      "force_publications_email_notifications",
      "javascript_category_popup",
      "javascript_termsandconditions_popup"
      );

      $multiplier_fields = array(
      'editor_max_summary',
      'editor_max_contents',
      'user_max_summary',
      'user_max_contents',
      'image_attachment_max_size',
      'video_attachment_max_size',
      'miscellaneous_attachment_max_size',
      'audio_attachment_max_size',
      "editor_max_comment",
      "user_max_comment"
      );

      $admin_settings = array();
      $admin_settings['title'] = $title;
      foreach($multiplier_fields as $f)
      {
         if($this->getMeta($f) > 0)
         {
            $admin_settings[$f] = $this->getMeta($f) * 1024;
         }
         else
         {
            $admin_settings[$f] = 0;
         }
      }
      $admin_settings = $this->populateBasicSettingFields($admin_settings, $basic_text_fields, $basic_checkbox_fields);
      $param_str = serialiseArrayParams($admin_settings);
      $admin_str = '$site_module->setAdminOptions('.$param_str.');'."\n";
      return $admin_str;
   }

   function getHTML($cls, $depth, &$obj_set, $caller_settings, $live=false)
   {
      list($wrap, $display) = $this->getDisplayMode($caller_settings, $depth);
      $cls_str = ($cls == "") ? "" : "class='$cls'";
      list($h, $h_e) = $this->getHeaderTags($caller_settings, $depth, $cls_str, true);

      if($caller_settings && is_array($caller_settings) && isset($caller_settings['title']) && $caller_settings['title'] != "")
      {
         $title = $caller_settings['title'];
      }
      elseif(($display == "Full Page" or $display =="Long Title") and $this->getMeta('longlinktext') != "")
      {
         $title = $this->getMeta('longlinktext');
      }
      else
      {
         $title = $this->getMeta('shortlinktext');
      }
      $img = $this->getObjectIconHTML($this->getMeta('image'), $this->name(), $this->name());
      $href = "href='".$this->getSelfLink()."'";

      $lnk = "$h<a $href $cls_str title='$title'>";
      $lnk_end = "</a>$h_e";

      $ret = $this->getHeadHTML($cls_str, $wrap);

      if($display ==    "Icon And Title")
      {
         $ret .= $lnk.$img.$title.$lnk_end;
      }
      elseif($display == "Icon")
      {
         $ret .= $lnk.$img.$lnk_end;
      }
      elseif($display != "Full Page")
      {
         $ret .= $lnk.$title.$lnk_end;
      }
      else
      {
         if(!$this->loadSiteObject($obj_set))
         {
            $ret .= "INTERNAL ERROR";
            return $ret;
         }
         //params to be passed to the create
         $param_str = $this->getAllParams("publishfilter");
         $siteset_str = $this->getAmalgamatedSiteSettingString();
         $base_str = $this->getPHPBaseString('publishDisplay', $param_str, $siteset_str);
         $xtra_str = $this->getFullPageSettingPHP($title);
         $end_str = $this->getPHPEndString();
         $php_str = '<?php '.$base_str.$xtra_str.$end_str.' ?>';
         $ret .= $this->evaluateStringWithPHP($live, $php_str);
      }
      $ret .= $this->getFootHTML($cls_str, $wrap);
        return $ret;
    }
}

?>
