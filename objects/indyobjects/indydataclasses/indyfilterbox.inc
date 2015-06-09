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
/* Class to represent a set of filters
/************************************************************************/
class indyFilterboxObject extends indySiteModuleObject
{
   var $display_options = array(
     "Horizontal",
     "Horizontal Button First",
     "Vertical",
     "Vertical Button First",
     "Titled Horizontal",
     "Titled Horizontal Button First",
     "Titled Vertical",
     "Titled Vertical Button First"
   );
   var $runtime_requires = array("filterdisplay.inc");

   function indyFilterboxObject ($a, $b)
   {
      $this->indySiteModuleObject($a, $b);
   }
   function getDefaultDisplayMode($depth)
   {
      return "Vertical";
   }




   function assembleDisplayOptionString($cls, $display, $wrap, $hs)
   {
      $basic_text_fields = array("title", "buttontext");
      $basic_checkbox_fields = array("onchange", "replacebutton",  "includelanguages", "includetypes", "includemediatypes", "includespecials");
      $display_settings = $this->populateBasicSettingFields($display_settings, $basic_text_fields, $basic_checkbox_fields);
      if($display ==  "Titled Horizontal" or $display == "Titled Vertical" or $display == "Titled Horizontal Button First" or $display == "Titled Vertical Button First")
      {
         $display_settings['includetitle'] = 'true';
      }
      else
      {
         $display_settings['includetitle'] = 'false';
      }
      if($display == "Titled Horizontal" or $display == "Horizontal" or $display == "Titled Horizontal Button First" or $display == "Horizontal Button First")
      {
         $display_settings['filter_separator'] = '';
         $display_settings['horizontal'] = 'true';
         if($display == "Titled Horizontal Button First" or $display == "Horizontal Button First")
         {
            $display_settings['button_first'] = 'true';
         }
         else
         {
            $display_settings['button_first'] = 'false';
         }
      }
      else
      {
         $display_settings['filter_separator'] = '<br />';
         $display_settings['horizontal'] = 'false';
         if($display == "Titled Vertical Button First" or $display == "Horizontal Vertical First")
         {
            $display_settings['button_first'] = 'true';
         }
         else
         {
            $display_settings['button_first'] = 'false';
         }
      }
      if($wrap) $display_settings['wrap'] = 'true';
      else $display_settings['wrap'] = 'false';
      $display_settings['hsize'] = $hs;
      $display_settings['buttonimage'] = $this->getMeta("image");
      $display_settings['style_class'] = $cls;
      $param_str = serialiseArrayParams($display_settings);
      $php_str = $this->getRequires();
      $php_str .= '$fb = new filterDisplay('.$this->id().');'."\n";
      $php_str .= '$display_options = '.$param_str.';'."\n";
      $php_str .= '$fb->setDisplayOptions($display_options);'."\n";
      $php_str .= '$fb->drawFilters();'."\n";
      return $php_str;
   }

   function getHTML($cls, $depth, &$obj_set, $caller_settings, $live=false)
   {
      list($wrap, $display) = $this->getDisplayMode($caller_settings, $depth);
      if($cls == "" or $cls == '<?php echo $page_style; ?>') $cls = '$page_style';
      $hs = $this->getCallerHeaderSize($caller_settings);
      $cls_str = "class='$cls'";
      $ret = $this->getHeadHTML($cls_str, false);//coz it could be empty
      $php_str = '<?php '.$this->assembleDisplayOptionString($cls, $display, $wrap, $hs).' ?>';
      $ret .= $php_str;
      $ret .= $this->getFootHTML($cls_str, false);
      return $ret;
   }
}
?>