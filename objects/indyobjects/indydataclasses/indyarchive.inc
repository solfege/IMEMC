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

class indyArchiveObject extends indySiteModuleObject 
{ 
   var $container_settables = array( 
      "title" => 'text', 
      "hsize" => 'select', 
   );

   var $runtime_requires = array("archivedisplay.inc"); 
   var $display_options = array( 
      "Short Title", 
      "Icon", 
      "Icon And Title", 
      "Long Title", 
      "Full Page" 
   ); 

   function indyArchiveObject ($a, $b) 
   { 
        $this->indySiteModuleObject($a, $b); 
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
      $full_str .= $this->assembleUserOptionString(); 
      return $full_str; 
   } 

   function assembleUserOptionString() 
   { 
      $basic_text_fields = array("pagelimit"); 
      $basic_checkbox_fields = array(); 
      $user_settings = array(); 
      $param_str = serialiseArrayParams($user_settings); 
      $user_str = "\t".'$site_module->setUserOptions('.$param_str.');'."\n";

      return $user_str; 
   } 

   function assembleAdminOptionString($title) 
   { 
      $basic_text_fields = array("header", "footer", "nwtitle", "gltitle", "yeartxt", "nexttxt", "prevtxt", "nwtext", "gltext" ); 
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
         //$title = htmlspecialchars($this->getMeta('longlinktext')); 
         $title = $this->getMeta('longlinktext'); 
      } 
      else 
      { 
         //$title = htmlspecialchars($this->getMeta('shortlinktext')); 
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

         //params to be passed to the runtime object 
         $param_str = $this->getAllParams(array()); 
         $siteset_str = $this->getAmalgamatedSiteSettingString(); 
         $base_str = $this->getPHPBaseString('archiveDisplay', $param_str, $siteset_str); 
         $xtra_str = $this->getFullPageSettingPHP($title); 
         $end_str = $this->getPHPEndString(); 
         $php_str = '<?php '.$base_str.$xtra_str.$end_str.' ?>'; 
         $ret .= $this->evaluateStringWithPHP($live, $php_str); 
         //$ret .= $php_str; 
      } 
      $ret .= $this->getFootHTML($cls_str, $wrap);

      return $ret; 
    } 
}

?>
