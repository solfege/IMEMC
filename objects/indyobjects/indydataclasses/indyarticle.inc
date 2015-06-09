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
class indyArticleObject extends indySiteModuleObject
{
   var $display_options = array(
      "Full Page"
   );

   var $runtime_requires = array("articledisplay.inc");

   function indyArticleObject ($a, $b)
   {
      $this->indySiteModuleObject($a, $b);
   }

   function getPageSettingArray(&$obj_set)
   {
      $page_settings = parent::getPageSettingArray($obj_set);
      unset($page_settings['page_filters']);
      return $page_settings;
   }


   function getDefaultDisplayMode($depth)
   {
      $display = "Full Page";
      return $display;
   }

   function getFullPageSettingPHP()
   {
      $full_str = $this->assembleAdminOptionString();
      $full_str .= $this->assembleUserOptionString();
      return $full_str;
   }

   function assembleUserOptionString()
   {
      $basic_text_fields = array("comment_limit", "comment_order" ,"video_mode");
      $basic_checkbox_fields = array("hiddenonly", "nohidden", "statusonly", "showhidden", "showlocked", "showsticky", "showstatus", "usestatusicons", "include_comments", "condense_comments", "show_comment_numbers", "social_bookmark");
      $user_settings = array();
      $user_settings = $this->populateBasicSettingFields($user_settings, $basic_text_fields, $basic_checkbox_fields);
      $optional_navigs = array('context', 'otherviews', 'prevnext');
      $header_navig = array();
      $multi = 'headernavigation';
      $header_navig = $this->populateMultiCheckboxFields($header_navig, $optional_navigs, $multi);
      $user_settings[$multi] = $header_navig;
      $footer_navig = array();
      $multi = 'footernavigation';
      $footer_navig = $this->populateMultiCheckboxFields($footer_navig, $optional_navigs, $multi);
      $user_settings[$multi] = $footer_navig;
      if($this->getMeta("comment_order") == 'asc') $user_settings['comment_order'] = 'asc';
      else $user_settings['comment_order'] = 'desc';

      $summary_options = array('context', 'date', 'author', 'author_contact', 'attachments');
      $summary_fields = array('subtitle', 'end');
      foreach($summary_fields as $f)
      {
         $got_options = array();
         foreach($summary_options as $s_o)
         {
            if($this->getMeta($s_o) == 'both' or $this->getMeta($s_o) == $f)
            {
               $got_options[] = $s_o;
            }
         }
         $user_settings[$f.'details'] = $got_options;
      }

      if($this->getMeta('listingtime') > 0)
      {
         $user_settings['time_posted_lower_limit']=strtotime("11:59pm ".$this->getMeta('listingtime')." days ago");
      }
      else
      {
         $user_settings['time_posted_lower_limit']=0;
      }
      $user_settings['time_posted_upper_limit']=0;
      $param_str = serialiseArrayParams($user_settings);
      $user_str = "\t".'$site_module->setUserOptions('.$param_str.');'."\n";
      return $user_str;
   }


   function assembleAdminOptionString()
   {
      $basic_text_fields = array("title", "header", "footer", "emptyresultmsg", "hiddenmessage", "lockedmessage", "lockedmessage", "add_comment_message", "titles_only_message", "full_comments_message", "oldest_first_message", "latest_first_message", "comments_title", "comment_page_text", "comments_title", "comment_page_text", "comment_number_text", "back_to_article_message", "back_to_newswire_message", "no_story_with_id_message", "no_comment_with_id_message", "user_error_message_noid", "external_image_link_warning", 'fix_text', 'fixandcopy_text', 'external_image_link_warning', 'orig_ref_text', 'fix_copyandunhide_text', 'fix_copy_text', 'fix_create_text', 'cant_fix_text', 'manual_fix_text', 'fix_explanation_text', 'hidden_attachment_text', 'fix_unhide_text', 'hidden_explanation_text', 'editor_note_text', 'self_link_text', 'event_date_format', 'storydate_format', 'comments_page_text', 'comments_number_text', 'commentdate_format', 'set_comment_pagesize', "relatedlink_text", "notviewable_message", "maximgheight", "maximgwidth", "maximgsize", "content_reporting_contact", "report_post_text", "report_post_subject", "rss_article_feed", "atom_article_feed", "feed_article_help", "rss_text", "atom_text", "hidden_comments_note_text");
      $basic_checkbox_fields = array("ignorehidden", "includerelatedlink", "originallink", "show_audio_player");
      $admin_settings = array();
      $admin_settings = $this->populateBasicSettingFields($admin_settings, $basic_text_fields, $basic_checkbox_fields);
      $allowed_multis = array('clip');
      $optional_multis = array('unstick', 'hide', 'unhide');
      $multi = 'allowmultis';
      $allowed_multis = $this->populateMultiCheckboxFields($allowed_multis, $optional_multis, $multi);
      $admin_settings[$multi] = $allowed_multis;
      $param_str = serialiseArrayParams($admin_settings);
      $admin_str = '$site_module->setAdminOptions('.$param_str.');'."\n";
      return $admin_str;
   }

    function getHTML($cls, $depth, &$obj_set, $caller_settings, $live=false)
    {
      list($wrap, $display) = $this->getDisplayMode($caller_settings, $depth);
      $cls_str = ($cls == "") ? "" : "class='$cls'";
      $ret = $this->getHeadHTML($cls_str, $wrap);
      if(!$this->loadSiteObject($obj_set))
      {
         $ret .= "INTERNAL ERROR";
         return $ret;
      }
      //params to be passed to the runtime object
      $param_str = $this->getAllParams();
      $siteset_str = $this->getAmalgamatedSiteSettingString();
      $base_str = $this->getPHPBaseString('articleDisplay', $param_str, $siteset_str);
      $xtra_str = $this->getFullPageSettingPHP();
      $end_str = $this->getPHPEndString();
      $php_str = '<?php '.$base_str.$xtra_str.$end_str.' ?>';
      $ret .= $this->evaluateStringWithPHP($live, $php_str);
      $ret .= $this->getFootHTML($cls_str, $wrap);
        return $ret;
    }

}

?>
