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
class indyGalleryObject extends indySiteModuleObject 
{ 
   var $container_settables = array( 
      "title" => 'text', 
      "hsize" => 'select', 
   ); 

   var $runtime_requires = array("gallerydisplay.inc"); 

   var $display_options = array( 
      "Short Title", 
      "Icon", 
      "Icon And Title", 
      "Long Title", 
      "Banner", 
      "Menu Listing", 
      "Full Page" 
   ); 

   function getBasicFilterPrefix() 
   { 
      return "galleryfilter"; 
   } 

   function getDefaultDisplayMode($depth) 
   { 
      if($depth == FULL_DEPTH) $display = "Full Page"; 
      else $display = "Menu Listing";

      return $display; 
   } 

   function getFullPageSettingPHP($title) 
   { 
      $full_str = $this->assembleAdminOptionString($title); 
      $full_str .= $this->assembleUserOptionString();

      return $full_str; 
   } 

   function getBannerSettingPHP($title) 
   { 
      $user_settings = array(); 
      $user_settings['pagelimit'] = $this->getMeta('row_pagelimit');

      $user_settings['row_buffersize'] = $this->getMeta('row_buffersize');

      if($this->getMeta('row_listingtime') > 0) 
      { 
         $user_settings['time_posted_lower_limit']=strtotime("11:59pm ".$this->getMeta('row_listingtime')." days ago"); 
      } 
      else 
      { 
         $user_settings['time_posted_lower_limit']=0; 
      }

      $user_settings['time_posted_upper_limit']=0; 
      $user_settings['title'] = $title; 
      $user_settings['display'] = 'banner'; 
      $user_settings['resizeheight'] = $this->getMeta("row_resizeheight"); 
      $user_settings['resizewidth'] = $this->getMeta("row_resizewidth"); 
      $user_settings['maxfilesize'] = $this->getMeta("row_maxfilesize"); 
      $user_settings["filter_author"] = $this->getMeta("filter_author"); 
      $user_settings['title'] = $this->getMeta("row_title"); 
      $user_settings['time_delay'] = $this->getMeta("time_delay");

      $basic_text_fields = array("show_type"); 
      $basic_checkbox_fields = array("oneperstory", "context_sensitive_type", "context_sensitive_topic", "context_sensitive_region", "context_sensitive_language", "context_sensitive_author", "featurized_only");

      $user_settings = $this->populateBasicSettingFields($user_settings, $basic_text_fields, $basic_checkbox_fields);

      $param_str = serialiseArrayParams($user_settings); 
      $user_str = "\t".'$site_module->setUserOptions('.$param_str.');'."\n"; 
      $admin_str = '$site_module->setAdminOptions(Array());'."\n";

      return $admin_str.$user_str; 
   }

   function getColumnSettingPHP($title) 
   { 
      $user_settings = array(); 
      $user_settings['pagelimit'] = $this->getMeta('column_pagelimit');

      if($this->getMeta('column_listingtime') > 0) 
      { 
         $user_settings['time_posted_lower_limit']=strtotime("11:59pm ".$this->getMeta('column_listingtime')." days ago"); 
      } 
      else 
      { 
         $user_settings['time_posted_lower_limit']=0; 
      } 
      $user_settings['time_posted_upper_limit']=0; 
      $user_settings['title'] = $title; 
      $user_settings['resizeheight'] = $this->getMeta("column_resizeheight"); 
      $user_settings['resizewidth'] = $this->getMeta("column_resizewidth"); 
      $user_settings['maxfilesize'] = $this->getMeta("column_maxfilesize"); 
      $user_settings["filter_author"] = $this->getMeta("filter_author"); 
      $user_settings['display'] = 'column'; 
      $user_settings['title'] = $this->getMeta("column_title"); 
      $user_settings['time_delay'] = $this->getMeta("time_delay");

      $basic_text_fields = array("show_type"); 
      $basic_checkbox_fields = array("oneperstory", "context_sensitive_type", "context_sensitive_topic", "context_sensitive_region", "context_sensitive_language", "context_sensitive_author", "featurized_only"); 
      $user_settings = $this->populateBasicSettingFields($user_settings, $basic_text_fields, $basic_checkbox_fields);

      $param_str = serialiseArrayParams($user_settings); 
      $user_str = "\t".'$site_module->setUserOptions('.$param_str.');'."\n";

      $admin_str = '$site_module->setAdminOptions(Array());'."\n";

      return $admin_str.$user_str; 
   } 

   function assembleUserOptionString() 
   { 
      $basic_text_fields = array("pagelimit", "resizeheight", "resizewidth", "maxfilesize", "filter_author", "show_type"); 
      $basic_checkbox_fields = array("oneperstory"); 
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
      $basic_text_fields = array("header", "footer", "emptyresultmsg"); 
      $basic_checkbox_fields = array(); 
      $admin_settings = array(); 
      $admin_settings['title'] = $title; 
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
      elseif($display == "Long Title" or $display == "Short Title") 
      { 
         $ret .= $lnk.$title.$lnk_end; 
      } 
      elseif($display == "Banner") 
      { 
         $title = $this->getMeta('row_title'); 
         if($title != "") 
         { 
            $ret .= $lnk.$title.$lnk_end; 
         } 
      } 
      elseif($display == "Menu Listing") 
      { 
         $title = $this->getMeta('column_title'); 
         if($title != "") 
         { 
            $ret .= $lnk.$title.$lnk_end; 
         } 
      } 

      if($this->isFullerDisplayThan($display,"Long Title")) 
      { 
         if(!$this->loadSiteObject($obj_set)) 
         { 
            $ret .= "INTERNAL ERROR"; 
            return $ret; 
         } 
         //params to be passed to the create

         $param_str = $this->getAllParams('galleryfilter'); 
         $siteset_str = $this->getAmalgamatedSiteSettingString(); 
         $base_str = $this->getPHPBaseString('galleryDisplay', $param_str, $siteset_str); 

         if($display == "Full Page") 
         { 
            //echo "<p>in here</p>"; 
            $xtra_str = $this->getFullPageSettingPHP($title); 
            $end_str = $this->getPHPEndString(); 
         } 
         elseif($display == "Banner") 
         { 
            $xtra_str = $this->getBannerSettingPHP($title); 
            $end_str = $this->getPHPHeadlineEndString(); 
         } 
         else 
         { 
            $xtra_str = $this->getColumnSettingPHP($title); 
            $end_str = $this->getPHPHeadlineEndString(); 
         } 
         $php_str = '<?php '.$base_str.$xtra_str.$end_str.' ?>'; 
         $ret .= $php_str; 
      } 
      $ret .= $this->getFootHTML($cls_str, $wrap);

      return $ret; 
    } 
} 
?>
