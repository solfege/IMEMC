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
/* Class to represent a basic chunk of code
/************************************************************************/
class indyCodeboxObject extends indyBasicObject
{
   var $display_options = array(
     "Contents"
   );

   function indyCodeboxObject ($a, $b)
   {
      $this->indyBasicObject($a, $b);
   }
   function getDefaultDisplayMode($depth)
   {
      return "Contents";
     }


     function getHTML($cls, $depth, &$obj_set, $caller_settings, $live=false)
   {
      list($wrap, $display) = $this->getDisplayMode($caller_settings, $depth);
      if($this->getMeta('contents') == "") return "";
      $cls_str = ($cls == "") ? "" : "class='$cls'";
      $ret = $this->getHeadHTML($cls_str, $wrap);
      $ret .= $this->evaluateStringWithPHP($live, $this->getMeta('contents'));
      $ret .= $this->getFootHTML($cls_str, $wrap);
      return $ret;
   }
}
?>