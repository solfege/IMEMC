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

/************************************************************************/
/* class to represent a basic site sub-division
/************************************************************************/

class indySiteObject extends indyBasicObject
{ 
   var $site_filters; 
   var $principal_site_views = array('newswireobject', 'articleobject', 'featureobject', 'galleryobject', 'searchobject', 'archiveobject', 'contactobject', 'commentsobject', 'publishobject', 'eventsobject'); 

   var $display_options = array( 
      "Administration Link", 
      "Administration Icon", 
      "Admin Icon And Text", 
      "Full Page"); 

   function indySiteObject ($a, $b) 
   {

      $this->indyBasicObject($a, $b); 
   }

   function getDefaultDisplayMode($depth) 
   { 
      if($depth == FULL_DEPTH) $display = "Full Page"; 
      else $display = "Administration Link";

      return $display; 
   }

   function getSiteSettings() 
   { 
      $sections = $this->principal_site_views; 
      $ret = array(); 
      $ret['siteobject'] = $this->id();

      foreach($sections as $s) 
      { 
         $ret[$s] = $this->getMeta($s); 
      } 
      return $ret; 
   }

   function getBasicEmailSettings() 
   { 
      global $system_config;

      if(isValidEmailAddress(trim($this->getMeta("notify_content_toemail")))) 
      { 
         $to = trim($this->getMeta("notify_content_toemail")); 
      } 
      else 
      { 
         $to = $system_config->notification_to_email_address; 
      }

      if(isValidEmailAddress(trim($this->getMeta("notify_content_replyemail")))) 
      { 
         $reply = trim($this->getMeta("notify_content_replyemail")); 
      } 
      elseif(isValidEmailAddress($system_config->notification_replyto_email_address)) 
      { 
         $reply = $system_config->notification_replyto_email_address; 
      } 
      else 
      { 
         $reply = $to; 
      }

      $from = $system_config->notification_from_email_address; 
      $subj_prefix = $this->getMeta("notify_content_subject_prefix"); 
      $lvl = $this->getMeta("notify_editcontent_level"); 
      $ret = array("to" => $to, 'from' => $from, "subject_prefix" => $subj_prefix, "replyto" => $reply, 'level' => $lvl); 
      return $ret; 
   }

    function getHTML($cls, $depth, &$obj_set, $caller_settings, $live=false) 
    { 
      list($wrap, $display) = $this->getDisplayMode($caller_settings, $depth); 
      $cls_str = ($cls == "") ? "" : "class='$cls'";

      list($h, $h_e) = $this->getHeaderTags($caller_settings, $depth, $cls_str, true);

      if($display != "Full Page") 
      { 
         if($caller_settings && is_array($caller_settings) && isset($caller_settings['title']) && $caller_settings['title'] != "") 
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

         if($display ==    "Admin Icon And Text") 
         { 
            $ret .= $lnk.$img.$title.$lnk_end; 
         } 
         elseif($display == "Administration Icon") 
         { 
            $ret .= $lnk.$img.$lnk_end; 
         } 
         else $ret.= $lnk.$title.$lnk_end;

         $ret .= $this->getFootHTML($cls_str, $wrap); 
         return $ret; 
      } 

      $php_str = ' 
         global $editor_session; 
         require_once($path_prefix."objects/adminpage.inc"); 
         $admin_obj= new AdminPageObject(); 
         if($editor_session->isSessionOpen()) 
         { 
            $_REQUEST["site_id"] = '.$this->id().'; 
            $admin_obj->processRequest(); 
         } 
         else 
         { 
            $admin_obj->writeLoginBox(); 
         } 
      ';

      $php_str = '<?php '.$php_str.' ?>'; 
      $ret = $php_str;

      return $ret; 
   } 

   function arrayToString ($arr) 
   { 
      if(is_array($arr) && (! in_array("None", $arr))) 
      { 
         $params = 'Array("'. implode('", "', $arr).'")'; 
      } 
      else $params = 'Array()';

      return $params; 
   }

   function getSiteFilters($prefix) 
   { 
      $this->site_filters['topic'] = $this->getMeta($prefix.'topic'); 
      $this->site_filters['region'] = $this->getMeta($prefix.'region'); 
      $this->site_filters['language'] = $this->getMeta($prefix.'language'); 
      $this->site_filters['type'] = $this->getMeta($prefix.'type'); 
      $this->site_filters['author'] = $this->getMeta("author_filter");

      return $this->site_filters; 
   } 

   function getBasicSiteFilterArray() 
   { 
      $arr = $this->getSiteFilters("sitebasicfilters");

      if(in_array("None", $arr["topic"])) $arr['topic'] = array("*"); 
      if(in_array("None", $arr["region"])) $arr['region'] = array("*"); 
      if(in_array("None", $arr["language"])) $arr['language'] = array("*"); 
      if(in_array("None", $arr["type"])) $arr['type'] = array("*");

      return $arr; 
   }

   function getAllParams ($prefix) 
   { 
      $t = $this->arrayToString($this->getMeta($prefix.'topic')); 
      $r = $this->arrayToString($this->getMeta($prefix.'region')); 
      $l = $this->arrayToString($this->getMeta($prefix.'language')); 
      $ty = $this->arrayToString($this->getMeta($prefix.'type'));

      return '('.$t.', '.$r.', '.$l.', '.$ty.');'; 
   } 
} 
?>
