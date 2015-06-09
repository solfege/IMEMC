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
require_once('indynewswire.inc');

class indyFeatureObject extends indyNewswireObject 
{ 
   var $display_options = array( 
      "Short Title", 
      "Icon", 
      "Icon And Title", 
      "Long Title", 
      "Headlines Listing", 
      "Full Page" 
   ); 

   var $runtime_requires = array("newswiredisplay.inc", "featuredisplay.inc"); 

   function indyFeatureObject ($a, $b) 
   { 
        $this->indyNewswireObject($a, $b); 
   } 

   function getBasicFilterPrefix() 
   { 
      return "featurefilter"; 
   } 

   function getPageSettingArray(&$obj_set) 
   { 
      global $system_config; 
      $page_settings = parent::getPageSettingArray($obj_set); 
      $this->setMetaData("featurefiltertype", array($system_config->feature_type_id)); 
      $page_settings['page_filters'] = $this->getPageFilterArray($obj_set);

      return $page_settings; 
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

      if($this->isFullerDisplayThan($display,"Long Title")) 
      { 
         if(!$this->loadSiteObject($obj_set)) 
         { 
            $ret .= "INTERNAL ERROR"; 
            return $ret; 
         }

         //params to be passed to the create 
         global $system_config;

         $this->setMetaData("featurefiltertype", array($system_config->feature_type_id)); 
         $param_str = $this->getAllParams('featurefilter'); 
         $siteset_str = $this->getAmalgamatedSiteSettingString(); 

         if($display == "Full Page") 
         { 
            $base_str = $this->getPHPBaseString('featureDisplay', $param_str, $siteset_str); 
            $xtra_str = $this->getFullPageSettingPHP($title); 
            $end_str = $this->getPHPEndString(); 
         } 
         else 
         { 
            $base_str = $this->getPHPBaseString('newswireDisplay', $param_str, $siteset_str); 
            $xtra_str = $this->getHeadlineColumnSettingPHP(); 
            $end_str = $this->getPHPHeadlineEndString(); 
         } 
         // 
         $php_str = '<?php '.$base_str.$xtra_str.$end_str.' ?>'; 
         $ret .= $php_str; 
      }

      $ret .= $this->getFootHTML($cls_str, $wrap); 
      return $ret; 
    } 
} 

?>
