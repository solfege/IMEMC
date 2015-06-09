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

class indyNewswireObject extends indySiteModuleObject
{
   var $container_settables = array(
      "title" => 'text',
      "hsize" => 'select',
   );

   var $runtime_requires = array("newswiredisplay.inc");

   var $display_options = array(
      "Short Title",
      "Icon",
      "Icon And Title",
      "Long Title",
      "Headlines Listing",
      "Headlines Full List",
      "Full Page"
   );

   function indyNewswireObject ($a, $b)
   {
      $this->indySiteModuleObject($a, $b);
   }

   function getBasicFilterPrefix()
   {
      return 'newswirefilter';
   }

   function getDefaultDisplayMode($depth)
   {
      if($depth == FULL_DEPTH) $display = "Full Page";
      else $display = "Headlines Listing";
      return $display;
   }

   function getHeadlineColumnSettingPHP()
   {
      $basic_text_fields = array("pagelimit", "maxhlength", "articleincludedate", "includedate","eventtimeformat", "read_more_text");
      $basic_checkbox_fields = array("hiddenonly", "nohidden", "lockedonly", "nolocked",  "nosticky","stickyonly", "upcomingevents", "statusonly", "includecontext", "includeicons", "showhidden", "showlocked", "showsticky", "showstatus", "usestatusicons", "ignorehidden", "respectsticky", "articleincludeicons", "includeauthor", "includeorganisation", "includecommentcount", "includephoto", "highlightfeatures", "context_sensitive_region", "context_sensitive_type", "context_sensitive_topic", "context_sensitive_language", "context_sensitive_author", "promoted");

      $prefix = 'headline_';

      $user_settings = array();
      $user_settings = $this->populateBasicSettingFields($user_settings, $basic_text_fields, $basic_checkbox_fields, $prefix);
      $user_settings = $this->populateBasicSettingFields($user_settings, array('emptyresultmsg', "filter_author"), array());
      if($this->getMeta('headline_listingtime') > 0)
      {
         $user_settings['time_posted_lower_limit']=strtotime("11:59pm ".$this->getMeta('headline_listingtime')." days ago");
        }
      else
      {
         $user_settings['time_posted_lower_limit']=0;
      }
      $user_settings['time_posted_upper_limit']=0;
      $user_settings['full_page_id'] = $this->id();
      $param_str = serialiseArrayParams($user_settings);
      $user_str = "\t".'$site_module->setAdminOptions('.$param_str.');'."\n";
      $user_str .= "\t".'$site_module->setUserOptions(Array());'."\n";
      return $user_str;
   }

   function getFullPageSettingPHP($title)
   {
      $full_str = $this->assembleAdminOptionString($title);
      $full_str .= $this->assembleUserOptionString();
      return $full_str;
   }


   function assembleUserOptionString()
   {
      $basic_text_fields = array("pagelimit", "storydate_format","video_mode");
      $basic_checkbox_fields = array("hiddenonly", "nohidden", "lockedonly", "nolocked", "nosticky","stickyonly", "upcomingevents", "statusonly", "includeicons", "includephoto", "showhidden", "showlocked", "showsticky", "showstatus", "usestatusicons", "includerelatedlink", "include_embed_video", "include_embed_audio", "usealternatestyles", "include_translations");
      $user_settings = array();
      $user_settings = $this->populateBasicSettingFields($user_settings, $basic_text_fields, $basic_checkbox_fields);
      $optional_navigs = array('context', 'prevnext', 'otherviews');
      $header_navig = array();
      $multi = 'headernavigation';
      $header_navig = $this->populateMultiCheckboxFields($header_navig, $optional_navigs, $multi);
      $user_settings[$multi] = $header_navig;
      $footer_navig = array();
      $multi = 'footernavigation';
      $footer_navig = $this->populateMultiCheckboxFields($footer_navig, $optional_navigs, $multi);
      $user_settings[$multi] = $footer_navig;
      $summary_options = array('context', 'date', 'author', 'comments', 'attachments');
      $summary_fields = array('subtitle', 'end');
      foreach($summary_fields as $f)
      {
         $got_options = array();
         foreach($summary_options as $s_o)
         {
            if($this->getMeta('story'.$s_o) == 'both' or $this->getMeta('story'.$s_o) == $f)
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


   function assembleAdminOptionString($title)
   {
      $basic_text_fields = array("header", "footer", "emptyresultmsg", "read_more_text", "filter_author", "editor_note_text", "self_link_text", "relatedlink_text");
      $basic_checkbox_fields = array("promoted", "ignorehidden", "respectsticky");
      $admin_settings = array();
      $admin_settings['title'] = $title;
      $admin_settings = $this->populateBasicSettingFields($admin_settings, $basic_text_fields, $basic_checkbox_fields);
      $allowed_multis = array('clip');
      $optional_multis = array('unstick', 'hide', 'unhide', 'lock', 'unlock');
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
      list($h, $h_e) = $this->getHeaderTags($caller_settings, $depth, $cls_str, true);

      if($caller_settings && is_array($caller_settings) && isset($caller_settings['title']) && $caller_settings['title'] != "")
      {
         $title = $caller_settings['title'];
      }
      elseif(($display == "Full Page" or $display =="Long Title" or $display =="Headlines Full List") and $this->getMeta('longlinktext') != "")
      {
         // Took out use of htmlspecialchars
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

      if($this->isFullerDisplayThan($display,"Long Title"))
      {
         if(!$this->loadSiteObject($obj_set))
         {
            $ret .= "INTERNAL ERROR";
            return $ret;
         }
         //params to be passed to the create
         $param_str = $this->getAllParams('newswirefilter');
         $siteset_str = $this->getAmalgamatedSiteSettingString();
         $base_str = $this->getPHPBaseString('newswireDisplay', $param_str, $siteset_str);

         if($display == "Full Page")
         {
            $xtra_str = $this->getFullPageSettingPHP($title);
            $end_str = $this->getPHPEndString();
         }
         else
         {
            $xtra_str = $this->getHeadlineColumnSettingPHP();
            $end_str = $this->getPHPHeadlineEndString();
         }

         //
         $php_str = '<?php '.$base_str.$xtra_str.$end_str.' ?>';
         $ret .= $this->evaluateStringWithPHP($live, $php_str);
      }
      $ret .= $this->getFootHTML($cls_str, $wrap);
        return $ret;
    }
}

?>
