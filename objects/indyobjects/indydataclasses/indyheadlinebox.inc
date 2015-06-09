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

class indyHeadlineboxObject extends indySiteModuleObject
{
   var $container_settables = array(
      "title" => 'text',
      "hsize" => 'select',
   );

   var $runtime_requires = array("newswiredisplay.inc");

   var $display_options = array(
      "Short Title",
      "Icon",
      "Icon And Title",
      "Long Title",
      "Headlines Listing",
      "Contextual Headlines Listing",
      "Headlines Full List"
   );

    function indyHeadlineboxObject ($a, $b)
    {
        $this->indySiteModuleObject($a, $b);
    }


   function getDefaultDisplayMode($depth)
   {
      $display = "Headlines Listing";
      return $display;
   }

   function getHeadlineBoxSettingPHP($nw_id)
   {
      $basic_text_fields = array("pagelimit", "maxhlength", "articleincludedate", "eventtimeformat",  "includedate", "emptyresultmsg", "title", "hsize", "read_more_text");
      $basic_checkbox_fields = array("hiddenonly", "nohidden", "lockedonly", "nolocked", "nosticky", "stickyonly", "upcomingevents", "statusonly", "includecontext", "includeicons", "includephoto", "showhidden", "showlocked", "showsticky", "showstatus", "usestatusicons", "ignorehidden", "respectsticky", "articleincludeicons", "includeauthor", "includeorganisation", "includecommentcount", "latestevents", "context_sensitive_region", "context_sensitive_type", "context_sensitive_topic", "context_sensitive_language", "context_sensitive_author", "contextual_mode", "promoted");
      $user_settings = array();
      $user_settings = $this->populateBasicSettingFields($user_settings, $basic_text_fields, $basic_checkbox_fields);
      if($this->getMeta('listingtime') > 0)
      {
         $user_settings['time_posted_lower_limit']=strtotime("11:59pm ".$this->getMeta('listingtime')." days ago");
      }
      else
      {
         $user_settings['time_posted_lower_limit']=0;
      }
      $user_settings['time_posted_upper_limit']=0;
      $user_settings['full_page_id'] = $nw_id;
      $param_str = serialiseArrayParams($user_settings);
      $user_str = "\t".'$site_module->setAdminOptions('.$param_str.');'."\n";
      $user_str .= "\t".'$site_module->setUserOptions(Array());'."\n";
      return $user_str;
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
      elseif(($display == "Full Page" or $display =="Long Title" or $display == 'Headlines Full List') and $this->getMeta('longlinktext') != "")
      {
         $title = $this->getMeta('longlinktext');
      }
      else
      {
         $title = $this->getMeta('shortlinktext');
      }
      $img = $this->getObjectIconHTML($this->getMeta('image'), $this->name(), $this->name());

      $ret = $this->getHeadHTML($cls_str, $wrap);

      if(!$this->loadSiteObject($obj_set))
      {
         $ret .= "INTERNAL ERROR";
         return $ret;
      }
      if($this->getMeta('upcomingevents') == 'on')
      {

         $nw_id = $this->getMeta("eventsobject");
         if($nw_id == "")
         {
            $st = $this->site_object->getSiteSettings();
            $nw_id = $st['eventsobject'];
         }
      }
      else
      {
         $nw_id = $this->getMeta("newswireobject");
         if($nw_id == "")
         {
            $st = $this->site_object->getSiteSettings();
            $nw_id = $st['newswireobject'];
         }
      }
      $lnk_extra = "";
      $nw_lnk = $this->getObjLink($nw_id);
      $extras = array();
      $tpic = $this->getMeta('headlinefiltertopic');
      if($tpic != "" && $tpic != "None" && !is_array($tpic))
      {
         $extras[] = "topic_id=$tpic";
      }
      $typ = $this->getMeta('headlinefiltertype');
      if($typ != "" && $typ != "None" && !is_array($typ))
      {
         $extras[] = "type_id=$typ";
      }
      $lan = $this->getMeta('headlinefilterlanguage');
      if($lan != "" && $lan != "None" && !is_array($lan))
      {
         $extras[] = "language_id=$lan";
      }
      $reg = $this->getMeta('headlinefilterregion');
      if($reg != "" && $reg != "None" && !is_array($reg))
      {
         $extras[] = "region_id=$reg";
      }
      $nw_lnk = addLinkParameters($nw_lnk, $extras);
      $href = "href='$nw_lnk'";
      $lnk = "$h<a $href $cls_str title='$title'>";
      $lnk_end = "</a>$h_e";


      if($display ==    "Icon And Title")
      {
         $ret .= $lnk.$img.$title.$lnk_end;
      }
      elseif($display == "Icon")
      {
         $ret .= $lnk.$img.$lnk_end;
      }
      elseif($display == "Contextual Headlines Listing")
      {
         $this->setMetaData('title', $title);
         $this->setMetaData('contextual_mode', 'on');
      }
      elseif($display != "Full Page")
      {
         $ret .= $lnk.$title.$lnk_end;
      }

      if($this->isFullerDisplayThan($display,"Long Title"))
      {
         if(!$this->loadSiteObject($obj_set))
         {
            $ret .= "INTERNAL ERROR";
            return $ret;
         }
         //params to be passed to the create
         $param_str = $this->getAllParams('headlinefilter');
         $siteset_str = $this->getAmalgamatedSiteSettingString();
         $base_str = $this->getPHPBaseString('newswireDisplay', $param_str, $siteset_str);
         $xtra_str = $this->getHeadlineBoxSettingPHP($nw_id);
         $end_str = $this->getPHPHeadlineEndString();
         $php_str = '<?php '.$base_str.$xtra_str.$end_str.' ?>';
         $ret .= $this->evaluateStringWithPHP($live, $php_str);
      }
      $ret .= $this->getFootHTML($cls_str, $wrap);
        return $ret;
    }
}

?>
