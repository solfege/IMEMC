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
/* Class to represent an archive search module for oscailt
/************************************************************************/

class indySearchObject extends indySiteModuleObject
{

   var $display_options = array(
      "Title",
      "Icon",
      "Icon And Title",
      "Flat",
      "Flat With Link",
      "Short Box",
      "Box",
      "Box With Links",
      "Full Page"
   );

   var $runtime_requires = array("searchdisplay.inc");

    function indySearchObject ($a, $b)
    {
        $this->indySiteModuleObject($a, $b);
    }

    function getDefaultDisplayMode($depth)
    {
       if($depth == FULL_DEPTH) $display = "Full Page";

       elseif($depth <= LONG_LIST_DEPTH)  $display = "Box With Link";

       elseif($depth <= SHORT_LIST_DEPTH)  $display = "Short Box";

       else $display = "Flat";

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

   function getFullPageHTML(&$obj_set, $live)
   {
      if(!$this->loadSiteObject($obj_set))
      {
         $ret .= "INTERNAL ERROR";
         return $ret;
      }

      $param_str = $this->getAllParams("");
      $siteset_str = $this->getAmalgamatedSiteSettingString();
      $base_str = $this->getPHPBaseString('searchDisplay', $param_str, $siteset_str);
      $end_str = $this->getPHPEndString();

      $basic_text_fields = array("header", "footer", "search_page_title", "search_page_description",
       "search_all_fields_label", "search_all_fields_description",
       "search_dates_label", "search_beforeafter", "search_dates_description",
       "search_specific_fields_label", "search_specific_fields_description", "author_name_label",
       "author_org_label", "author_addr_label", "author_ph_label", "author_email_label", "story_title_label",
       "story_subtitle_label", "story_summary_label", "story_content_label", "search_comments_label", "exact_label");

      $basic_checkbox_fields = array();

      $admin_settings = array();
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

      if($display == "Full Page")
      {
         return $this->getFullPageHTML($obj_set, $live);
      }

      if($caller_settings && is_array($caller_settings) && isset($caller_settings['title']) && $caller_settings['title'] != "")
      {

         $title = $caller_settings['title'];
      }
      else
      {
         $title = $this->getMeta("shortlinktext");
      }

      $img = $this->getObjectIconHTML($this->getMeta('image'), $this->name(), $this->name());
      $img2 = $this->getObjectIconHTML($this->getMeta('image2'), $this->name(), $this->name());

      $link_target = $this->getSelfLink();
      $link_html = "$h<a href='$link_target' $cls_str>";
      $link_end = "</a>$h_e";


      if($display == "Icon" and $img != "")
      {
         $link_full = $link_html.$img.$link_end;

      }
      elseif($display == "Icon And Title" and $img != "")
      {
         $link_full = $link_html.$img.$title.$link_end;
      }
      elseif($display == "Title" or $display == "Icon And Title" or $display == "Icon")
      {
         $link_full = $link_html.$title.$link_end;
      }
      else
      {
         $link_full = "";
      }


      $ret = $this->getHeadHTML($cls_str, $wrap); 
      $ret .= $link_full;

      if($display == "Flat" or $display == "Flat With Link"  or $display == "Box" or $display == "Short Box" or $display == "Box With Links")
      { 
        if(!$this->loadSiteObject($obj_set))
        {
           $ret .= "INTERNAL ERROR";
           return $ret;
        }

        $nform_extra = "";
        $gform_extra = "";

        $nw_id = $this->getMeta("newswireobject");

        if($nw_id == "")
        {
           $st = $this->site_object->getSiteSettings();
           $nw_id = $st['newswireobject'];
        }

        $gl_id = $this->getMeta("galleryobject");

        if($gl_id == "")
        {
           $gl = $this->site_object->getSiteSettings();
           $gl_id = $st['galleryobject'];
        }

        $nform_target = $this->getObjLink($nw_id, false);

        if(strpos($nform_target, "obj_id=") !== false)
        {
          $nform_extra = "<input type='hidden' name='obj_id' value='$nw_id'>";
        }

        $gform_target = $this->getObjLink($gl_id, false);

        if(strpos($gform_target, "obj_id=") !== false)
        {
          $gform_extra = "<input type='hidden' name='obj_id' value='$gl_id'>";
        }

        if($display == "Flat" or $display =="Flat With Link")
        {
           $css_style = "style='display: inline; margin: 0'";
        }
        else
        {
           $css_style = "style='margin: 0'";
        }

        $ret_php = '
        global $oscailt_page_setting, $pseudo_directory_offset;
        if(isset($oscailt_page_setting["obj_type"]) &&  strcasecmp($oscailt_page_setting["obj_type"], "GALLERY") == 0)
        {
            $form_target = $pseudo_directory_offset."'.$gform_target.'";
            $form_extra = "'.$gform_extra.'";
        }
        else
        {
            $form_target = $pseudo_directory_offset."'.$nform_target.'";
            $form_extra = "'.$nform_extra.'";
        }
        ';

        $ret .= '<? '.$ret_php.' ?>'; 
	// To avoid 2 forms with same names on same page
        if($display == "Box With Links")
            $ret .= "<form $css_style name='searchform_box' method='get' action=".'"<?=$form_target?>">'; 
        else
            $ret .= "<form $css_style name='searchform' method='get' action=".'"<?=$form_target?>">'; 
        $ret .= '<?= $form_extra ?>';

        //wrapping bit...
        if($display != "Flat" and $display != "Flat With Link")
        {
           $ret .= "$h$img $title $img2$h_e";
           if($h == "") $ret .= "<BR />";

        }
	if($display == "Flat With Link")
	{
	        $ret.="<a $cls_str href='$link_target'>";
                $ret.="Advanced Search</a> ";
	}
        $ret .= $this->getFormHTML($cls, $display, $link_target, $live);
      }

      $ret .= $this->getFootHTML($cls_str, $wrap);

      return $ret;
   }


    function getFormHTML($cls, $display, $target, $live)
    {

       $cls_str = ($cls == "") ? "" : "class='$cls'";

       $jscript = 'onFocus="JavaScript:if(this.value ==\''.$this->getMeta("filltext")."'){this.value='';}\"";
       // $input_html = "<input type='hidden' name='input_name' value='xyz'>";
       $input_html = "<input $cls_str type='text' name='search_text' maxlength='200' $jscript value='";

       $php_tmpl_str = '
       if(isset($_REQUEST["search_text"]) and $_REQUEST["search_text"] != "")
       {
          $search_fill = htmlspecialchars($_REQUEST["search_text"], ENT_QUOTES);
       }
       else $search_fill = "[@[@0]@]";
       ';

       $php_setting = $this->populatePHPTemplateString($php_tmpl_str, array($this->getMeta("filltext")));


       if($live)
       {
         eval($php_setting);
         $input_html .= $search_fill."'>\n";
       }
       else
       {
         $input_html .= '<?php '.$php_setting.'echo $search_fill; ?>'."'>\n";
       }

       if($display != "Flat" and $display != "Flat With Link" and $display != "Short Box")
       {
          $input_html .= "<BR />";
       }

       if($this->getMeta('replacebutton') == 'on')
       {
          $imgcls_str = ($cls == "") ? "class=image" : "class='$cls"."-image'";

          $input_html .= "<input $imgcls_str type='image' src='".'<?php echo $pseudo_directory_offset ?>'.$this->getMeta('buttonimage')."'";
          $input_html .= ' alt="'.$this->getMeta( 'buttontext' ).'">'."\n";
       }
       else
       {
          $btncls_str = ($cls == "") ? "class=button" : "class='$cls"."-button'";
          $input_html .= "<input $btncls_str value='".htmlspecialchars( $this->getMeta( 'buttontext' ) );
          $input_html .= "' name='button' type='submit'>";
       }

       if($display == "Box With Links")
       {
          $btncls_str = ($cls == "") ? "class=checkbox" : "class='$cls"."-checkbox'";
          $input_html.="<p $cls_str><input $btncls_str name='search_comments' type='checkbox'>\n";
          $input_html.="<small $cls_str>Search comments</small></p>\n";
          $input_html.="<p $cls_str><a $cls_str href='$target'>";
          $input_html.="Advanced Search</a></p>\n";
       }
       return $input_html."</form>";
    }

}

?>

