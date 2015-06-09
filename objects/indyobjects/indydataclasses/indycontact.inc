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
/* Class to represent a contact form
/************************************************************************/
class indyContactObject extends indySiteModuleObject
{
   var $display_options = array(
      "Title",
      "Icon",
      "Icon And Title",
      "Long Title",
      "Full Page"
   );

   var $runtime_requires = array("contactdisplay.inc");

    function indyContactObject ($a, $b)
    {
        $this->indySiteModuleObject($a, $b);
    }

    function getDefaultDisplayMode($depth)
    {
       if($depth == FULL_DEPTH) $display = "Full Page";
       else $display = "Title";
       return $display;
   }

   function getJScriptFill($txt)
   {
      $jscriptf = '<script language="javascript">'."\n";
      $jscriptf .= '<!--'."\n";
      $jscriptf .= 'function clearSearch()'."\n";
      $jscriptf .= '{'."\n";
      $jscriptf .= 'if (document.forms.searchform.search_text.value == "'.$this->getMeta("filltext").'")'."\n";
      $jscriptf .= '{'."\n";
      $jscriptf .= 'document.forms.searchform.search_text.value = "";'."\n";
      $jscriptf .= '}}'."\n";
      $jscriptf .= '//-->'."\n";
      $jscriptf .= '</script>'."\n";
      return $jscripf;
   }

   function getFullPageHTML(&$obj_set, $title, $live)
   {
      if(!$this->loadSiteObject($obj_set))
      {
         return "INTERNAL ERROR";
      }
      $param_str = $this->getAllParams("");
      $siteset_str = $this->getAmalgamatedSiteSettingString();
      $base_str = $this->getPHPBaseString('contactDisplay', $param_str, $siteset_str);
      $end_str = $this->getPHPEndString();
      $basic_text_fields = array("header", "footer", "nametitle", "contentstitle", "subjecttitle", "fromtitle", "introtext", "emailto", "emailfrom", "emailreply", "formheadtext", "buttontext", "successtext", "finalchecktext", "spammandatorywords");
      $basic_checkbox_fields = array("editorprivatemail", "logemails", "hidesubjectfield", "spambotcheck", "logspamcheck");
      $admin_settings = array("title" => $title);
      $admin_settings = $this->populateBasicSettingFields($admin_settings, $basic_text_fields, $basic_checkbox_fields);
      $display_param_str = serialiseArrayParams($admin_settings);
      $admin_str = '$site_module->setAdminOptions('.$display_param_str.');'."\n";
      $admin_str .= "\n".'$site_module->setUserOptions(array());'."\n";
      $php_str = '<?php '.$base_str.$admin_str.$end_str.' ?>';
      $ret = $this->evaluateStringWithPHP($live, $php_str);
      return $ret;
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
       elseif(($display == "Long Title" or $display == "Full Page") && $this->getMeta("longlinktext") != "")
       {
          $title = $this->getMeta("longlinktext");
       }
       else
       {
          $title = $this->getMeta("shortlinktext");
       }

       $img = $this->getObjectIconHTML($this->getMeta('image'), $this->name(), $this->name());
       $link_target = $this->getSelfLink();
       $link_html = "$h<a href='$link_target' $cls_str>";
       $link_end = "</a>$h_e";

       if($display == "Full Page")
       {
         return $this->getFullPageHTML($obj_set, $title, $live);
       }

       if($display == "Icon" and $img != "")
       {
          $link_full = $link_html.$img.$link_end;
       }
       elseif($display == "Icon And Title" and $img != "")
       {
          $link_full = $link_html.$img.$title.$link_end;
       }
       elseif($display == "Title" or $display == "Icon And Title" or $display == "Icon" or $display == "Long Title")
       {
          $link_full = $link_html.$title.$link_end;
       }
       else
       {
         return "";
       }
       $ret = $this->getHeadHTML($cls_str, $wrap);
       $ret .= $link_full;
       $ret .= $this->getFootHTML($cls_str, $wrap);
       return $ret;
    }

}
?>
