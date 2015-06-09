<?php
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
require_once('displayobject.inc');


class filterDisplay extends displayObject
{
   function filterDisplay ($obj_id)
   {
      $this->displayObject($obj_id);
   }

   function setDisplayOptions(&$options)
   {
      $this->display_options = $options;
   }

   function getFilterLink($obj_id)
   {
      $lnk = parent::getFilterLink($obj_id);
      //have to deal with all the event calendar stuff - not well integrated into story query...
      $extra_params = array();
      if(isset($_GET['event_display']))
      {
         $extra_params[] = 'event_display='.$_GET['event_display'];
      }
      if(isset($_GET['display']))
      {
         $extra_params[] = 'display='.$_GET['display'];
      }
      if(isset($_GET['day']))
      {
         $extra_params[] = 'day='.$_GET['day'];
      }
      if(isset($_GET['month']))
      {
         $extra_params[] = 'month='.$_GET['month'];
      }
      if(isset($_GET['year']))
      {
         $extra_params[] = 'year='.$_GET['year'];
      }
      if(count($extra_params) > 0)
      {
         $lnk = addLinkParameters($lnk, $extra_params);
      }
      return $lnk;
   }

   function drawFilters()
   {
      global $oscailt_page_setting, $pseudo_directory_offset, $oscailt_story_query;
      if(!isset($oscailt_story_query))
      {
         $oscailt_story_query = new StoryQuery();
         $oscailt_story_query->loadFormValues();
      }
      $this->onwards_query = $oscailt_story_query->copy();
      $this->onwards_query->region_id=null;
      $this->onwards_query->topic_id=null;
      $this->onwards_query->type_id=null;
      $this->onwards_query->language_id=null;
      $this->onwards_query->results_offset=0;
      $html_str = "";
      if(isset($oscailt_page_setting['obj_id']))
      {
         $lnk = $this->getFilterLink($oscailt_page_setting['obj_id']);
      }
      else $lnk = "";
      $sels = array();
      if($this->getDisplayOption('style_class') != "")
      {
         $cls_str = "class='".$this->getDisplayOption('style_class')."'";
      }
      else $cls_str = "";
      $sels = $this->getPageFilters($sels, $cls_str);
      if(count($sels) > 0)
      {
         $fil_sep = $this->getDisplayOption('filter_separator');
         if($fil_sep == "") $fil_sep = " ";
         $filters_html = implode($fil_sep, $sels);
         if($this->getDisplayOption("onchange") == 'true')
         {
            $button_html = "";
         }
         elseif($this->getDisplayOption("replacebutton") == 'true' and $this->getDisplayOption("buttonimage") != "")
         {
            $button_html = "<input class='".$this->getDisplayOption('style_class')."buttonimage' type='image' name='filtersubmit' alt='".htmlspecialchars($this->getDisplayOption("buttontext"), ENT_QUOTES)."' src='$pseudo_directory_offset".$this->getDisplayOption("buttonimage")."'>";
         }
         else
         {
            $button_html = "<input $cls_str type='submit' name='filtersubmit' value='".htmlspecialchars($this->getDisplayOption("buttontext"), ENT_QUOTES)."'>";
         }
         if($this->getDisplayOption('includetitle') == 'true' && $this->getDisplayOption("title") != "")
         {
            $hsize = $this->getDisplayOption('hsize');
            $title = htmlspecialchars($this->getDisplayOption('title'));
            if($hsize > 0) $title_html = "<h".$hsize." $cls_str>$title</h".$hsize.">";
            else $title_html = "<span $cls_str>$title</span>";
         }
         else
         {
            $title_html = "";
         }

         if($this->getDisplayOption('wrap') == 'true')
         {
            $html_str .= "<DIV $cls_str>\n";
         }
         if(strpos($lnk, "?") !== false)
         {
            $method = "POST";
         }
         else
         {
            $method = "GET";
         }
         $html_str .=  "<form style='display:inline; margin:0;' name='filterform".$this->obj_id."' method='$method' action='$lnk'>";
         if($this->getDisplayOption("button_first") == 'true')
         {
            $html_str .= "$title_html $button_html ".$fil_sep." $filters_html";
         }
         else $html_str .= "<P $cls_str>$title_html $filters_html".$this->getDisplayOption("filter_separator")."  $button_html</p>";
         $html_str .= "</form>";
         if($this->getDisplayOption('wrap') == 'true')
         {
            $html_str .= "</DIV>\n";
         }
      }
      echo $html_str;
   }


   function getPageFilters(&$selects, $cls_str)
   {
      global $oscailt_page_setting, $page_style;
      if(!isset($oscailt_page_setting['page_filters']))
      {
         return $selects;
      }
      $filters = $oscailt_page_setting['page_filters'];
      if($this->getDisplayOption("onchange") == 'true')
      {
         $onchange_str = "onchange='document.filterform".$this->obj_id.".submit()'";
      }
      else
      {
         $onchange_str = "";
      }

      $mysel = $this->getTopicSelect($filters, $cls_str, $onchange_str);
      if($mysel != "") $selects[] = $mysel;
      $mysel = $this->getRegionSelect($filters, $cls_str, $onchange_str);
      if($mysel != "") $selects[] = $mysel;
      if($this->getDisplayOption("includelanguages") == 'true')
      {
         $mysel = $this->getLanguageSelect($filters, $cls_str, $onchange_str);
         if($mysel != "") $selects[] = $mysel;
      }
      if($this->getDisplayOption("includetypes") == 'true')
      {
         $mysel = $this->getTypeSelect($filters, $cls_str, $onchange_str);
         if($mysel != "") $selects[] = $mysel;
      }
      if($this->getDisplayOption("includemediatypes") == 'true')
      {
         $mysel = $this->getMediaTypeSelect($filters, $cls_str, $onchange_str);
         if($mysel != "") $selects[] = $mysel;
      }
      if($this->getDisplayOption("includespecials") == 'true')
      {
         $mysel = $this->getSpecialsSelect($filters, $cls_str, $onchange_str);
         if($mysel != "") $selects[] = $mysel;
      }
      return $selects;
   }


   function getTopicSelect(&$filters, $cls_str, $onchange_str)
   {
      global $topicList, $pageTopic;
      if(count($filters['topic']) > 1 or $filters['topic'][0] == "*")
      {
         $options = array();
         $topics=$topicList->getfTopics();
         foreach($topics as $t)
         {
            if(in_array($t->topic_id, $filters['topic']) or in_array("*", $filters['topic']))
            {
               if(isset($pageTopic) && $pageTopic->topic_id==$t->topic_id)
               {
                  $options[] = "<OPTION selected value='".$t->topic_id."'>".$t->topic_name."</OPTION>\n";
               }
               else
               {
                  $options[] = "<OPTION value='".$t->topic_id."'>".$t->topic_name."</OPTION>\n";
               }
            }
         }
         if(count($options) > 1)
         {
            array_unshift($options, "<option value=''>All Topics</option>");
            $filter_str = "<select $cls_str name='topic_id' $onchange_str>";
            $filter_str .= implode("", $options);
            $filter_str .= "</select>";
            return $filter_str;
         }
      }
      return "";
   }

   function getRegionSelect(&$filters, $cls_str, $onchange_str)
   {
      global $regionList, $pageRegion;
      if(count($filters['region']) > 1 or $filters['region'][0] == "*")
      {
         $options = array();
         $regions=$regionList->getfRegions();
         foreach($regions as $t)
         {
            if(in_array($t->region_id, $filters['region']) or in_array("*", $filters['region']))
            {
               if(isset($pageRegion) and $pageRegion->region_id==$t->region_id)
               {
                  $options[] =  "<OPTION selected value='".$t->region_id."'>".$t->region_name."</OPTION>\n";
               }
               else
               {
                  $options[] =  "<OPTION value='".$t->region_id."'>".$t->region_name."</OPTION>\n";
               }
            }
         }
         if(count($options) > 1)
         {
            array_unshift($options, "<option value=''>All Regions</option>");
            $filter_str = "<select $cls_str name='region_id' $onchange_str>";
            $filter_str .= implode("", $options);
            $filter_str .= "</select>";
            return $filter_str;
         }
      }
      return "";
   }

   function getLanguageSelect(&$filters, $cls_str, $onchange_str)
   {
      global $languageList, $pageLanguage;
      if(count($filters['language']) > 1 or $filters['language'][0] == "*")
      {
         $options = array();
         $languages=$languageList->getfLanguages();
         foreach($languages as $t)
         {
            if(in_array($t->language_id, $filters['language']) or in_array("*", $filters['language']))
            {
               if(is_object($pageLanguage) && $pageLanguage->language_id==$t->language_id)
               {
                  $options[] =  "<OPTION selected value='".$t->language_id."'>".$t->language_name."</OPTION>\n";
               }
               else
               {
                  $options[] =  "<OPTION value='".$t->language_id."'>".$t->language_name."</OPTION>\n";
               }
            }
         }
         if(count($options) > 1)
         {
            array_unshift($options, "<option value=''>All Languages</option>");
            $filter_str = "<select $cls_str name='language_id' $onchange_str>";
            $filter_str .= implode("", $options);
            $filter_str .= "</select>";
            return $filter_str;
         }
      }
      return "";
   }

   function getTypeSelect(&$filters, $cls_str, $onchange_str)
   {
      global $typeList, $pageType;
      if(count($filters['type']) > 1 or $filters['type'][0] == "*")
      {
         $options = array();
         $types=$typeList->getfTypes();
         foreach($types as $t)
         {
            if(in_array($t->type_id, $filters['type']) or in_array("*", $filters['type']))
            {
               if(is_object($pageType) && $pageType->type_id==$t->type_id)
               {
                  $options[] =  "<OPTION selected value='".$t->type_id."'>".$t->type_name."</OPTION>\n";
               }
               else
               {
                  $options[] =  "<OPTION value='".$t->type_id."'>".$t->type_name."</OPTION>\n";
               }
            }
         }
         if(count($options) > 1)
         {
            array_unshift($options, "<option value=''>All Types</option>");
            $filter_str = "<select $cls_str name='type_id' $onchange_str>";
            $filter_str .= implode("", $options);
            $filter_str .= "</select>";
            return $filter_str;
         }
      }
      return "";
   }

   function getMediaTypeSelect(&$filters, $cls_str, $onchange_str)
   {
      global $system_config;
      if($system_config->storyAttachmentsAllowed())
      {
         $options = array();
         if(is_object($this->query) && $this->query->media_type=="TEXT")
         {
            $options[] = "<option selected value='TEXT'>Text</option>";
         }
         else
         {
            $options[] = "<option value='TEXT'>Text</option>";
         }
         if($system_config->allow_story_image_attachments || $system_config->allow_comment_image_attachments)
         {
            if(is_object($this->query) && $this->query->media_type=="IMAGE")
            {
               $options[] = "<option selected value='IMAGE'>Image</option>";
            }
            else
            {
               $options[] = "<option value='IMAGE'>Image</option>";
            }
         }
         if($system_config->allow_story_video_attachments || $system_config->allow_comment_video_attachments)
         {
            if(is_object($this->query) && $this->query->media_type=="VIDEO")
            {
               $options[] = "<option selected value='VIDEO'>Video</option>";
            }
            else
            {
               $options[] = "<option value='VIDEO'>Video</option>";
            }
         }
         if($system_config->allow_story_audio_attachments || $system_config->allow_comment_audio_attachments)
         {
            if(is_object($this->query) && $this->query->media_type=="AUDIO")
            {
               $options[] = "<option selected value='AUDIO'>Audio</option>";
            }
            else
            {
               $options[] = "<option value='AUDIO'>Audio</option>";
            }
         }
         if($system_config->allow_story_miscellaneous_attachments || $system_config->allow_comment_miscellaneous_attachments)
         {
            if(is_object($this->query) && $this->query->media_type=="MISCELLANEOUS")
            {
               $options[] = "<option selected value='MISCELLANEOUS'>Miscellaneous</option>";
            }
            else
            {
               $options[] = "<option value='MISCELLANEOUS'>Miscellaneous</option>";
            }
         }
         if(count($options) > 1)
         {
            array_unshift($options, "<option value=''>All Media</option>");
            $filter_str = "<select $cls_str name='media_type' $onchange_str>";
            $filter_str .= implode("", $options);
            $filter_str .= "</select>";
            return $filter_str;
         }
      }
      return "";
   }

   function getSpecialsSelect(&$filters, $cls_str, $onchange_str)
   {
      global $editor_session;
      if($editor_session->isSessionOpen())
      {
         $options = array();
         $options[] = "<option value=''>Latest Stories</option>";
         if(isset($this->query) and $this->query->order_mode=="hiddenitems")
         {
            $options[] = "<option selected value='hiddenitems'>Hidden Items Only</option>";
         }
         else
         {
            $options[] = "<option value='hiddenitems'>Hidden Items Only</option>";
         }
         if(isset($this->query) and $this->query->order_mode=="timecommentsposted")
         {
            $options[] = "<option selected value='timecommentsposted'>Latest Comments</option>";
         }
         else
         {
            $options[] = "<option value='timecommentsposted'>Latest Comments</option>";
         }
         $filter_str = "<select $cls_str name='order_mode' $onchange_str>";
         $filter_str .= implode("", $options);
         $filter_str .= "</select>";
         return $filter_str;
      }
      return "";
   }




}
?>
