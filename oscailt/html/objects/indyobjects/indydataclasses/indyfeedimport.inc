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

class indyFeedimportObject extends indySiteModuleObject
{
   var $container_settables = array(
      "title" => 'text',
      "hsize" => 'select',
   );

   var $runtime_requires = array("feedimportdisplay.inc");

   var $display_options = array(
      "Link",
      "Headlines Listing",
      "Title and Headlines",
      "Full Page"
   );

    function indyFeedimportObject ($a, $b)
    {
        $this->indySiteModuleObject($a, $b);
    }


   function getDefaultDisplayMode($depth)
   {
      if($depth == FULL_DEPTH) $display = "Full Page";
      else $display = "Title and Headlines";
      return $display;
   }

   function getHeadlineColumnSettingPHP()
   {
      $basic_text_fields = array("header", "footer", "emptyresultmsg", "promote_text", "maxhlength",  "url", 'dateformat', "title", "republish_summary_prefill", "language_codes", "max_img_width");
      $basic_checkbox_fields = array("allowuser_republish", "striptags", "convertfromutf8", "language_filter", "pre_parse_utf8");
      $display_specific_text_fields = array("pagelimit", 'titlesize', "max_word");
      $display_specific_checkbox_fields = array("show_republish", "includedate", "articleincludedate", "articleincludeicons", "articleincludeauthor", "articleincludedescription", "articleincludelanguage", "includedate", "includedescription", "includephoto", "includetitle", "includelanguage", "includecontext");
      $prefix = 'headline_';
      $user_settings = array();
      $user_settings = $this->populateBasicSettingFields($user_settings, $basic_text_fields, $basic_checkbox_fields);
      $user_settings = $this->populateBasicSettingFields($user_settings, $display_specific_text_fields, $display_specific_checkbox_fields, $prefix);
      $param_str = serialiseArrayParams($user_settings);
      $user_str = "\t".'$site_module->setAdminOptions('.$param_str.');'."\n";
      $user_str .= "\t".'$site_module->setUserOptions(Array());'."\n";
      return $user_str;
   }



   function getFullPageSettingPHP()
   {
      $basic_text_fields = array("header", "footer", "emptyresultmsg", "promote_text", "pagelimit", "maxhlength", "max_word", "url", 'dateformat', 'titlesize', "title", "republish_summary_prefill", "language_codes");
      $basic_checkbox_fields = array("show_republish", "allowuser_republish", "republish_full", "includedate", "articleincludedate", "articleincludeicons", "articleincludeauthor", "articleincludedescription", "articleincludelanguage", "articleincludecontent", "includedate", "includedescription", "includephoto", "includetitle", "includelanguage", "includecontext", "striptags", "convertfromutf8", "language_filter", "pre_parse_utf8");
      $admin_settings = array();
      $admin_settings = $this->populateBasicSettingFields($admin_settings, $basic_text_fields, $basic_checkbox_fields);
      $param_str = serialiseArrayParams($admin_settings);
      $admin_str = '$site_module->setAdminOptions('.$param_str.');'."\n";
      $admin_str .= '$site_module->setUserOptions(Array());'."\n";
      return $admin_str;
   }

    function getHTML($cls, $depth, &$obj_set, $caller_settings, $live=false)
    {
      list($wrap, $display) = $this->getDisplayMode($caller_settings, $depth);
      $cls_str = ($cls == "") ? "" : "class='$cls'";
      list($h, $h_e) = $this->getHeaderTags($caller_settings, $depth, $cls_str, true);
      $x = $this->getCallerHeaderSize($caller_settings);
      if($x != 0)
      {
         $this->setMetaData('headline_titlesize', $x);
      }
      if($caller_settings && is_array($caller_settings) && isset($caller_settings['title']) && $caller_settings['title'] != "")
      {
         $title = $caller_settings['title'];
      }
      else
      {
         $title = $this->getMeta('shortlinktext');
      }

      $ret = $this->getHeadHTML($cls_str, $wrap);
      if($display ==    "Link")
      {
         $href = "href='".$this->getSelfLink()."'";
         $lnk = "$h<a $href $cls_str title='$title'>";
         $lnk_end = "</a>$h_e";
         $ret .= $lnk.$title.$lnk_end;
      }
      else
      {
         if(!$this->loadSiteObject($obj_set))
         {
            $ret .= "INTERNAL ERROR";
            return $ret;
         }
         if($display == "Headlines Listing")
         {
            $this->setMetaData('headline_includetitle', "");
         }
         else
         {
            $this->setMetaData('headline_includetitle', "on");
            $this->setMetaData('includetitle', "on");
         }
         $param_str = $this->getAllParams(array());
         $siteset_str = $this->getAmalgamatedSiteSettingString();
         $base_str = $this->getPHPBaseString('feedimportDisplay', $param_str, $siteset_str);

         if($display == "Full Page")
         {
            $xtra_str = $this->getFullPageSettingPHP($title);
            $end_str = $this->getPHPEndString();
         }
         else
         {
            $xtra_str = $this->getHeadlineColumnSettingPHP($title);
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
