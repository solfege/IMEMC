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

// TO DO: Add in comments to show what way this file is called. There are at least 2 modes

require_once( "indylist.inc" );
/************************************************************************/
/* Class to represent a page component (such as a side menu).
/* Output to a cache file when any of its elements is updated.
/************************************************************************/
class indyPageObject extends indyListObject
{
   var $display_options = array("Full Page");

   function indyPageObject ($a, $b)
   {
      $this->indyListObject ($a, $b);
   }


   // This is called by buildObjectCacheFiles in indyObjectSet class and returns a string to be written
   // to the cache file.
   function fill(&$obj, &$obj_set, $site_id)
   {
      global $system_config;
      $site_obj = $obj_set->fetchObject($site_id, $this->getLangCode());
      if($site_obj->getMeta('different_css') == 'on')
      {
         $st_store = $system_config->site_file_store_base.'site_'.$site_id.'/';
      }
      else
      {
         $st_store = $system_config->site_file_store_base.'default/';
      }
      $rethtml = '<?php global $pseudo_directory_offset; $site_file_store = $pseudo_directory_offset."'.$st_store.'"; '."\n";

      $rethtml .= $obj->getPageSettingString($obj_set)."\n";
      $rethtml .= '$oscailt_story_query = new StoryQuery();'."\n";
      $rethtml .= '$oscailt_story_query->loadFormValues();'."\n";
      $rethtml .= '$PAGE_LAYOUT_ID = '.$this->objref.'; ?>'."\n";
      $rethtml .= $obj->getPageTitleSettingPHP($obj_set);
      $c_s = array();
      $pagestyle = ($this->getMeta('pagestyle') == "") ? 'indypage' : $this->getMeta('pagestyle');
      $cls = ($this->getMeta('mainpagestyle') == "") ? $pagestyle : $this->getMeta('mainpagestyle');
      if($obj->isProtected()) $rethtml .= $obj->getProtectionHTMLHead($site_id);
      $rethtml .= $obj->getHTML($cls, FULL_DEPTH, $obj_set, $c_s);
      $rethtml .= $obj->getObjectLanguageHTML($obj_set);
      $rethtml .= $obj->getObjectEditingHTML($obj_set, "<br>", "", true);
      if($obj->isProtected()) $rethtml .= $obj->getProtectionHTMLFoot(true);
      return $rethtml;
   }

   function getObjectEditingHTML(&$obj_set, $y, $x)
   {
      return parent::getObjectEditingHTML($obj_set, $y, $x, true);
   }

   function getMainBodyAsTable(&$obj_set, &$obj, $mcls, $lcls, $rcls, $live)
   {
      $rethtml = "<table class='$mcls'>\n";
      $rethtml .= "<tr class='$mcls'>\n";
      $lm_objects = isset($this->containedItems['leftcolumncontained'])? $this->containedItems['leftcolumncontained']: array();
      $rm_objects = isset($this->containedItems['rightcolumncontained'])?$this->containedItems['rightcolumncontained']: array();
      $mp_objects = isset($this->containedItems['mainpagecontained'])? $this->containedItems['mainpagecontained']: array();
      if($this->getMeta("includeemptyleft") == 'on' or count($lm_objects) > 0)
      {
         $rethtml .= "\n<!-- begin ".$this->name()." leftcolumn -->\n";
         $rethtml .= $this->getColumnLayout($obj_set, $obj, $lm_objects, $lcls, $live);
         $rethtml .= "<!-- end ".$this->name()." leftcolumn -->\n";
         $rethtml .= '<?php if($performance_test > 2) markTime("Got Left column") ?>';
      }

      $rethtml .= "\n<!-- begin ".$this->name()." object id ".$this->id()." contents column -->\n";
      $rethtml .= "<td class='$mcls'>\n";
      $rethtml .= '<?php $page_style = "'.$mcls.'"; ?>'."\n";

      if($this->getMeta("insetafterpage") == 'on') {
          $rethtml .= '<?=$PAGE_CONTENTS?>';
          $rethtml .= $this->getObjectEditingHTML($obj_set, "", "");
      }
      if(count($mp_objects) > 0)
      {
         $rethtml .= '<?php global $suppress_page_insets; if(!$suppress_page_insets){ ?>'."\n";
         $rethtml .= "\n<!-- begin ".$this->name()." contents inset boxes -->\n";
         $rethtml .= $this->getMainBodyInsets($obj_set, $mp_objects, $mcls, $live);
         $rethtml .= '<?php if($performance_test > 2) markTime("Got Inset Boxes") ?>';
         $rethtml .= "<!-- end ".$this->name()." contents inset boxes -->\n";
         $rethtml .= '<?php } ?>'."\n";
      }

      if($this->getMeta("insetafterpage") != 'on') {
         $rethtml .= '<?=$PAGE_CONTENTS?>';
         $rethtml .= $this->getObjectEditingHTML($obj_set, "", "");
      }

      $rethtml .= "</td>\n";
      $rethtml .= "<!-- end ".$this->name()." contents column -->\n";

      if($this->getMeta("includeemptyright") == 'on' or count($rm_objects) > 0)
      {
         $rethtml .= "\n<!-- begin ".$this->name()." right column -->\n";
         $rethtml .= $this->getColumnLayout($obj_set, $obj, $rm_objects, $rcls, $live);
         $rethtml .= "<!-- end ".$this->name()." right column -->\n";
         $rethtml .= '<?php if($performance_test > 2) markTime("Got Right column") ?>';
      }

      $rethtml .= "</tr>\n</table>\n";
      return $rethtml;
   }

   function getRowBasedLayout(&$obj_set, &$obj, $container_name, $cls, $live)
   {
      $rethtml = '';
      $included_objects = isset($this->containedItems[$container_name])? $this->containedItems[$container_name]: array();
      if(count($included_objects) > 0)
      {
         $rethtml .= "<div class='$cls'>\n<table class='$cls'>\n";
         foreach($included_objects as $h_obj)
         {
            $clsOn = ($h_obj->getStyle() == "") ? $cls : $h_obj->getStyle();
            $rethtml .= "<tr class='$cls'>\n<td class='$cls'>\n";
            $rethtml .= $this->getContainedObjectHTML($obj_set, $clsOn, $h_obj, $live);
            $rethtml .= "</td>\n</tr>\n";
         }
         $rethtml .= "</table>\n</div>\n";
      }
      return $rethtml;
   }

   function getMainBodyInsets(&$obj_set, &$included_objects, $cls, $live)
   {
      $rethtml = '';
      if(!isset($included_objects)) return $rethtml;
      foreach($included_objects as $h_obj)
      {
         $clsOn = ($h_obj->getStyle() == "") ? $cls : $h_obj->getStyle();
         $rethtml .= $this->getContainedObjectHTML($obj_set, $clsOn, $h_obj, $live);
      }
      return $rethtml;
   }


   function getContainedObjectHTML(&$obj_set, $cls, &$h_obj, $live)
   {
      global $userLanguage;
      $rethtml = '<?php $page_style = "'.$cls.'"; ?>';
      if($live)
      {
         $c_s = $h_obj->getAsCallerSettings();
         $indy_obj = $obj_set->fetchObject($h_obj->Id(), $userLanguage->getLanguageCodePrefix());
         if($indy_obj)
         {
            if($indy_obj->isProtected() or $h_obj->isORProtected())
            {
               if(!$this->checkObjectAccess($obj_set->itemCollection[$indy_obj->objref]->site_id))
               {
                  return "";
               }
            }
            $rethtml .= $indy_obj->getHTML($cls, FULL_DEPTH, $obj_set, $c_s);
         }
      }
      else
      {
         $inc_file = $obj_set->getIncludeFileRef($h_obj->Id());
         $rethtml .= '<?php include("'.$inc_file.'");?>'."\n";
      }
      return $rethtml;
   }

   function getColumnLayout(&$obj_set, &$obj, &$included_objects, $cls, $live)
   {
      $rethtml = '';
      if(!isset($included_objects)) return $rethtml;
      foreach($included_objects as $h_obj)
      {
         $clsOn = ($h_obj->getStyle() == "") ? $cls : $h_obj->getStyle();
         $rethtml .= "<td class='$clsOn'>\n";
         $rethtml .= $this->getContainedObjectHTML($obj_set, $clsOn, $h_obj, $live);
         $rethtml .= "</td>\n";
      }
      return $rethtml;
   }


   function getColumnBasedDivLayout(&$obj_set, $container_name, $cls, $live)
   {
      $rethtml = '';
      if(!isset($this->containedItems[$container_name])) return "";
      $included_objects = $this->containedItems[$container_name];
      if(count($included_objects) > 0)
      {
         foreach($included_objects as $h_obj)
         {
            $clsOn = ($h_obj->getStyle() == "") ? $cls : $h_obj->getStyle();
            $rethtml .= "<div class='$cls'>\n";
            $rethtml .= $this->getContainedObjectHTML($obj_set,$clsOn, $h_obj, $live);
            $rethtml .= "</div>\n";
         }
      }
      return $rethtml;
   }


   function getCachedLeftMenuDiv(&$obj_set, $cls, $live)
   {
      return "\n\n".'<!-- begin leftcolumn -->'."\n".$this->getColumnBasedDivLayout($obj_set, 'leftcolumncontained', $cls, $live). '<!-- end leftcolumn -->'."\n";
   }

   function getCachedRightMenuDiv(&$obj_set, $cls, $live)
   {
      return "\n\n<!-- begin ".$this->name()." rightcolumn -->\n".$this->getColumnBasedDivLayout($obj_set, 'rightcolumncontained', $cls, $live). "<!-- end ".$this->name()." rightcolumn -->\n\n";
   }

   function getCachedHeader(&$obj_set, &$obj, $hcls, $live)
   {

      $header_str = "\n\n<!-- begin ".$this->name()." header -->\n".$this->getRowBasedLayout($obj_set, $obj,'headercontained', $hcls, $live)."<!-- end ".$this->name()." header -->\n";
      return $header_str;
   }

   function getPrintCheckingPHP()
   {
      $str = '<?php if(!isset($_REQUEST["print_page"])){ ?>';
      return $str;
   }

   function getPrintCheckingPHPEnd()
   {
      $str = '<?php } ?>';
      return $str;
   }

   function getCachedFooter(&$obj_set, &$obj, $fcls, $live)
   {
      $str = "\n\n<!-- begin ".$this->name()." footer -->\n".$this->getRowBasedLayout($obj_set, $obj,'footercontained', $fcls, $live)."<!-- end ".$this->name()." footer -->\n";
      return $str;

   }

   function getHTML($cls, $depth, &$obj_set, $caller_settings, $live=false)
   {
      $pagestyle = ($this->getMeta('pagestyle') == "") ? 'indypage' : $this->getMeta('pagestyle');
      $headerstyle = ($this->getMeta('headerstyle') == "") ? $pagestyle : $this->getMeta('headerstyle');
      $leftcolumnstyle = ($this->getMeta('leftcolumnstyle') == "") ? $pagestyle : $this->getMeta('leftcolumnstyle');
      $rightcolumnstyle = ($this->getMeta('rightcolumnstyle') == "") ? $pagestyle : $this->getMeta('rightcolumnstyle');
      $mainpagestyle = ($this->getMeta('mainpagestyle') == "") ? $pagestyle : $this->getMeta('mainpagestyle');
      $footerstyle = ($this->getMeta('footerstyle') == "") ? $pagestyle : $this->getMeta('footerstyle');
      $rethtml = $this->getHeadHTML("class='$pagestyle'", true);
      $rethtml .= $this->getCachedHeader($obj_set, $obj, $headerstyle, $live);
      $rethtml .= '<?php if($performance_test > 2) markTime("Got Page Header") ?>';
      $rethtml .= "\n<!-- Begin ".$this->name()." object id ".$this->id()." Main body -->\n";
      if($this->getMeta("tablebasedlayout") == 'on')
      {
         $rethtml .= $this->getMainBodyAsTable($obj_set, $obj, $mainpagestyle, $leftcolumnstyle, $rightcolumnstyle, $live);
      }
      else
      {
         //lets us set a fixed width to the content pane different from the header pane
         $rethtml .= "<div class='$pagestyle"."-body'>\n";
         $rethtml .= $this->getCachedLeftMenuDiv($obj_set, $leftcolumnstyle, $live);
         $rethtml .= '<?php if($performance_test > 2) markTime("Got Left Menu Div") ?>';
         $rethtml .= $this->getCachedRightMenuDiv($obj_set, $rightcolumnstyle, $live);
         $rethtml .= '<?php if($performance_test > 2) markTime("Got Right Menu Div") ?>';
         $rethtml .= "\n\n<!-- main ".$this->name()." page -->\n";
         $included_objects = (isset($this->containedItems['mainpagecontained'])) ? $this->containedItems['mainpagecontained'] : array();
         $rethtml .= "<div class='$mainpagestyle'>\n";
         $rethtml .= '<?php $page_style = "'.$mainpagestyle .'"; ?>'."\n";
         $rethtml .= '<?php global $suppress_page_insets; if(!$suppress_page_insets){ ?>'."\n";
         $rethtml .= $this->getMainBodyInsets($obj_set, $included_objects, $mainpagestyle, $live);
         $rethtml .= '<?php if($performance_test > 2) markTime("Got Inset Div") ?>';
         //turned off for no good reason
         //      $site_header = $this->site_object->getMeta('header');
         $rethtml .= '<?=$PAGE_CONTENTS?>';
         //      $site_footer = $this->site_object->getMeta('footer');
         $rethtml .= $this->getObjectEditingHTML($obj_set, "", "");
         $rethtml .= '<?php } ?>'."\n";
         $rethtml .= "</div>\n";
      }
      $rethtml .= "<!-- End ".$this->name()." object id ".$this->id()." Main body -->\n";
      $rethtml .= '<?php if($performance_test > 2) markTime("Got Main Page Section") ?>';
      $rethtml .= $this->getCachedFooter($obj_set, $obj, $footerstyle, $live);
      $rethtml .= '<?php if($performance_test > 2) markTime("Got Footer Div") ?>';
      $rethtml .= $this->getFootHTML("class=$pagestyle", true);
      return $rethtml;
   }
}
?>
