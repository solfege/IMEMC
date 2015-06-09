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
/* Class to represent a listing of topics, regions, languages or types
/************************************************************************/
class indyCategorylistObject extends indySiteModuleObject
{
   var $legalCategories = array("topic", "region", "language", "type");
   var $display_options = array(
      "List",
      "Drop-Down",
      "String",
      "Titled List",
      "Titled Drop-Down"
   );
   var $runtime_requires = array("categorylistdisplay.inc");

   function indyCategorylistObject ($a, $b)
   {
      $this->indySiteModuleObject($a, $b);
   }

   function getDefaultDisplayMode($depth)
   {
      return "List";
   }



   /************************************************************************/
   /* Returns html of list
   /* output html string
   /************************************************************************/
   function getListingHTML($cls_str, $display, $dest_id, &$obj_set)
   {
      $html="";
      if($display == "Drop-Down" or $display == "Titled Drop-Down")
      {
         $rt_divider = "1";
         $target = $this->getObjLink($dest_id);
         $html = "<form style='display: inline; margin: 0;' method='get' action='$target' name=";
         $html .= $this->getMeta('listing')."form'>\n";
         $html .= "<select name='".$this->getMeta('listing');
         $html .= "' onChange='this.submit()'>";
      }
      elseif ($display == "String")
      {
         $rt_divider = "2";
      }
      else
      {
         $rt_divider = "3";
      }
      if(!$this->loadSiteObject($obj_set))
      {
          $ret = "INTERNAL ERROR";
          return $ret;
      }
      $param_str = $this->getAllParams(array());
      $siteset_str = $this->getAmalgamatedSiteSettingString();
      $base_str = $this->getPHPBaseString('categorylistDisplay', $param_str, $siteset_str);
      $user_settings = array("listing" => $this->getMeta('listing'), "divider" => $rt_divider, "linksto" => $dest_id);
      $new_param_str = serialiseArrayParams($user_settings);
      $xtra_str = "\t".'$site_module->setAdminOptions('.$new_param_str.');'."\n";
      $xtra_str .= "\t".'$site_module->setUserOptions(Array());'."\n";
      $end_str = $this->getPHPHeadlineEndString();
      $php_str = '<?php '.$base_str.$xtra_str.$end_str.' ?>';
      //$html .= htmlspecialchars($php_str);
      $html .= $php_str;
      if ($display == "Titled Drop-Down" or $display == "Drop-Down")
      {
          $html .= "</select></form>";
      }
      return $html;
   }

   function getHTML($cls, $depth, &$obj_set, $caller_settings, $live=false)
   {
      list($wrap, $display) = $this->getDisplayMode($caller_settings, $depth);
      $cls_str = ($cls == "") ? "" : "class='$cls'";
       list($h, $h_e) = $this->getHeaderTags($caller_settings, $depth, $cls_str, true);
      $img = $this->getObjectIconHTML($this->getMeta('image'), $this->name(), $this->name());
      $ret = $this->getHeadHTML($cls_str, $wrap);
      if(($display == "Titled List" or $display == "Titled Drop-Down") and $this->getMeta("shortlinktext") != "")
      {
         $ret .= $h.$this->getMeta("shortlinktext").$h_e;
         if($h == "") $ret .= "<BR />";

      }
      elseif($display == "String")
      {
         $ret .= "<b>".$this->getMeta("shortlinktext")."</b> ";
      }
      $dest_id = $this->getMeta("destinationobject");
      if($dest_id === false or $dest_id == "")
      {
         if(!$this->loadSiteObject($obj_set))
         {
            return "internal error: failed to load site object";
         }
         $dest_id = $this->site_object->getMeta("featureobject");
      }
      $ret .= $this->evaluateStringWithPHP($live, $this->getListingHTML($cls_str, $display, $dest_id, $obj_set));
      $ret .= $this->getFootHTML($cls_str, $wrap);
      return $ret;
   }
}
?>