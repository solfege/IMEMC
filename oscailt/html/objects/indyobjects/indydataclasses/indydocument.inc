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
/* Class to represent a static document
/************************************************************************/
class indyDocumentObject extends indyBasicObject
{ 
   var $display_options = array( 
      "Short Title", 
      "Icon", 
      "Icon And Title", 
      "Long Title", 
      "Long Title and Sub-title", 
      "Summary", 
      "Full Page" 
   ); 

   var $container_settables = array( 
      "title" => 'text', 
      "hsize" => 'select', 
   ); 

   function indyDocumentObject ($a, $b) 
   { 
      $this->indyBasicObject($a, $b); 
   }

   function getDefaultDisplayMode($depth) 
   { 
      if($depth == FULL_DEPTH) $display = "Full Page"; 
      elseif($depth == SUMMARY_DEPTH) $display = "Summary"; 
      elseif($depth == LONG_MENU_DEPTH) $display = "Long Title and Sub-title"; 
      else $display = "Short Title";

      return $display; 
   }

   /************************************************************************/ 
   /* Returns html to display object 
   /* input class, depth of viewing, and whether to wrap in html <div> (t/f) 
   /* output html string 
   /************************************************************************/ 
   function getHTML($cls, $depth, &$obj_set, &$caller_settings, $live=false) 
   { 
      list($wrap, $display) = $this->getDisplayMode($caller_settings, $depth); 
      $cls_str = ($cls == "") ? "" : "class='$cls'"; 
      list($h, $h_e) = $this->getHeaderTags($caller_settings, $depth, $cls_str, true);

      if($caller_settings && is_array($caller_settings) && isset($caller_settings['title']) && $caller_settings['title'] != "") 
      { 
         $title = $caller_settings['title']; 
      } 
      elseif(($display == "Full Page" or $display == "Summary" or $display == "Long Title and Sub-title" or $display =="Long Title") and $this->getMeta('longlinktext') != "") 
      { 
         //$title = htmlspecialchars($this->getMeta('longlinktext')); 
	 //$title = str_replace(array("'","\"","& ", "<", ">"),array("&#039;","&quot;","&amp; ", "&lt;", "&gt;"), $this->getMeta('longlinktext'));               
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

      if($display == "Icon And Title") 
      { 
         $ret .= $lnk.$img.$title.$lnk_end; 
      } 
      elseif($display == "Icon") 
      { 
         $ret .= $lnk.$img.$lnk_end; 
      } 
      elseif($display == "Short Title" or $display == "Long Title" or ($display == "Long Title and Sub-title" and $this->getMeta('subtitle') == "")) 
      { 
         $ret .= $lnk.$title.$lnk_end; 
      } 
      elseif($display == "Long Title and Sub-title") 
      { 
         $ret .= $lnk.$title.$lnk_end." ".$this->getMeta('subtitle'); 
      } 
      elseif($display == "Summary") 
      { 
         $ret .= $lnk.$title.$lnk_end; 
         $ret .= "<P $cls_str>$img".$this->getMeta('summary')."</P>"; 
      } 
      elseif($display == "Full Page") 
      { 
         $ret .= $h.$title.$h_e; 
         $ret .= "<BLOCKQUOTE $cls_str>".$this->getMeta('summary')."</BLOCKQUOTE>"; 
         $ret .= "<DIV $cls_str>".$this->getMeta('longdescription')."</DIV>"; 
      } 

      $ret .= $this->getFootHTML($cls_str, $wrap); 
      return $ret; 
   } 
} 
?>
