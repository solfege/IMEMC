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
class indyPreferencesObject extends indySiteModuleObject
{
   var $display_options = array(
      "Link",
      "Icon",
      "Icon And Link",
      "Menu",
      "Expandable Menu",
      "Full Page"
   );
   var $runtime_requires = array("preferencesdisplay.inc");

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
      elseif($depth == LONG_MENU_DEPTH) $display =   "Expandable Menu";
      else $display = "Link";
      return $display;
   }

   function getMenuHTML(&$obj_set, $title, $disp, $hsize, $cls, $live)
   {
      if(!$this->loadSiteObject($obj_set))
      {
         return "INTERNAL ERROR";
      }
      $param_str = $this->getAllParams("");
      $siteset_str = $this->getAmalgamatedSiteSettingString();
      $base_str = $this->getPHPBaseString('preferencesDisplay', $param_str, $siteset_str);
      if($disp == "Full Page")
      {

         $end_str = $this->getPHPEndString();
         $basic_text_fields = array("header", "footer", "optiontext", "valuetext", "descriptiontext", "universaloptionstext", "articleoptionstext", "eventsoptionstext", "commentsoptionstext", "saveoptionstext", "languageoptiontitle", "languageoptiondescription", "fontsizeoptiontitle", "fontsizeoptiondescription", "comment_titles_title", "comment_titles_description","all_comments_title", "all_comments_description", "event_displaytitle", "event_displaydescription", "condense_commentstitle", "condense_commentsdescription", "comment_ordertitle", "comment_orderdescription", "comment_limittitle", "comment_limitdescription", "languagefiltertitle", "languagefilterdescription", "languagefilteroff", "languagefilteron", "preferences_updated", "comments_text_title", "comments_text_description", "video_mode_title", "video_mode_description",);
         $basic_checkbox_fields = array();
      }
      else
      {
         $end_str = $this->getPHPHeadlineEndString();
         $basic_text_fields = array("fontsizetext", "languagetext", "profiletext", "logouttext", "normaltext", "alltext", "contenttext", "sitebuildertext", "editmodetext", "allprefstext", "oscailthometext", "languagetext", "admintext", "userstatustext");
         $basic_checkbox_fields = array("showfontsize", "showlanguage", "showoscailthome", "showaddbookmark", "showallprefs", "showeditmode", "showlogout", "showprofile", "showuserstatus");
      }
      $admin_settings = array("title" => $title, "display" => $disp, "hsize" => $hsize, "class" => $cls);
      $admin_settings = $this->populateBasicSettingFields($admin_settings, $basic_text_fields, $basic_checkbox_fields);
      $display_param_str = serialiseArrayParams($admin_settings);
      $admin_str = '$site_module->setAdminOptions('.$display_param_str.');'."\n";
      $admin_str .= "\n".'$site_module->setUserOptions(array());'."\n";
      $php_str = '<?php '.$base_str.$admin_str.$end_str.' ?>';
      $ret = $this->evaluateStringWithPHP($live, $php_str);
      return $ret;
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

      if($display ==    "Icon And Link")
      {
         $ret .= $lnk.$img.$title.$lnk_end;
      }
      elseif($display == "Icon")
      {
         $ret .= $lnk.$img.$lnk_end;
      }
      elseif($display == "Link")
      {
         $ret .= $lnk.$title.$lnk_end;
      }
      elseif($display == "Menu" or $display == "Expandable Menu")
      {
         global $page_style;

         if(stristr($cls, '<?php')) $cls = "";
	 else $cls = "indyleftcolumn";         

         $hsize = $this->getAppropriateHeaderSize($caller_settings, $depth);
         $ret .= $this->getMenuHTML($obj_set, $title, $display, $hsize, $cls, $live);
      }
      elseif($display == "Full Page")
      {
         if(stristr($cls, '<?php')) $cls = "";
         $hsize = $this->getAppropriateHeaderSize($caller_settings, $depth);
         $ret .= $h.$title.$h_e;
         $ret .= $this->getMenuHTML($obj_set, $title, $display, $hsize, $cls, $live);
      }

      $ret .= $this->getFootHTML($cls_str, $wrap);
      return $ret;
   }
}
?>
