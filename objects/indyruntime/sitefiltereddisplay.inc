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

require_once("objects/itemaction.inc");
require_once("objects/storyquery.inc");
require_once("displayobject.inc");



class siteFilteredDisplay extends displayObject
{
   var $topic_id;
   var $region_id;
   var $language_id;
   var $type_id;
   var $empty_display = false;
   var $author_names = array();


   function siteFilteredDisplay($obj_id)
   {
      $this->displayObject($obj_id);
   }

   /*******************************************
   //function to set the basic filters of this display object
   //defines what it is allowed 'see' in the database
   /******************************************/
   function setFilters ($topics, $regions, $languages, $types, $authors=null)
   {
      global $topicList,$regionList,$languageList, $typeList;
      if(count($topics) > 0)
      {
         if(in_array("*", $topics))
         {
            $this->topic_id = array();
         }
         else
         {
            foreach($topics as $one_id)
            {
               $temp_topic = $topicList->getTopicByID($one_id);
               if($temp_topic!=NULL)
               {
                  $this->topic_id[] = $one_id;
               }
            }
         }
      }
      else $this->empty_display = true;
      if(count($regions) > 0)
      {
         if(in_array("*", $regions))
         {
            $this->region_id = array();
         }
         else
         {
            foreach($regions as $one_id)
            {
               $temp_region = $regionList->getRegionByID($one_id);
               if($temp_region!=NULL)
               {
                  $this->region_id[] = $one_id;
               }
            }
         }
      }
      else $this->empty_display = true;

      if(count($languages) > 0)
      {
         if(in_array("*", $languages))
         {
            $this->language_id = array();
         }
         else
         {
            foreach($languages as $one_id)
            {
               $temp_language = $languageList->getLanguageByCode($one_id);
               if($temp_language!=NULL)
               {
                  $this->language_id[] = $temp_language->language_id;
               }
            }
         }
      }
      else $this->empty_display = true;

      if(count($types) > 0)
      {
         if(in_array("*", $types))
         {
            $this->type_id = array();
         }
         else
         {
            foreach($types as $one_id)
            {
               $temp_type = $typeList->getTypeByID($one_id);
               if($temp_type!=NULL)
               {
                  $this->type_id[] = $one_id;
               }
            }
         }
      }
      else $this->empty_display = true;
      if($authors != null && is_array($authors))
      {
         foreach($authors as $auth)
         {
            if($auth != "")
            {
               $this->author_names[] = $auth;
            }
         }
      }
    }

   function integrateFilters($use_page_topic=true, $use_page_region=true, $use_page_language=true, $use_page_type=true, $use_page_author=true)
   {
      //this is where we integrate the user filters with object set ones
      //do it through the query
      $this->query->multi_filter=1;
      global $pageTopic, $pageType, $pageRegion, $pageLanguage, $oscailt_page_setting;
      if($pageTopic==null or !$use_page_topic)
      {
         $this->query->topic_id = $this->topic_id;
         $this->onwards_query->topic_id = null;
      }
      else
      {
         if(in_array($pageTopic->topic_id, $this->topic_id) or count($this->topic_id) == 0)
         {
            $this->query->topic_id = array($pageTopic->topic_id);
            if(count($this->topic_id) == 1) $this->onwards_query->topic_id = null;
         }
         else
         {
            $this->query->topic_id = $this->topic_id;
            $this->onwards_query->topic_id = null;
         }
      }

      if($pageRegion==null or !$use_page_region) $this->query->region_id = $this->region_id;
      else
      {
         if(in_array($pageRegion->region_id, $this->region_id) or count($this->region_id) == 0)
         {
            $this->query->region_id = array($pageRegion->region_id);
         }
         else $this->query->region_id = $this->region_id;

      }
      if($pageLanguage==null or !$use_page_language) $this->query->language_id = $this->language_id;
      else
      {
         if(in_array($pageLanguage->language_id, $this->language_id)or count($this->language_id) == 0)
         {
            $this->query->language_id = array($pageLanguage->language_id);
         }
         else $this->query->language_id = $this->language_id;

      }
      if($pageType==null or !$use_page_type)
      {
         $this->query->type_id = $this->type_id;
         $this->onwards_query->type_id = null;
      }
      else
      {
         if(in_array($pageType->type_id, $this->type_id)or count($this->type_id) == 0)
         {
            if(count($this->type_id) == 1) $this->onwards_query->type_id = null;
            $this->query->type_id = array($pageType->type_id);

         }
         else
         {
            $this->query->type_id = $this->type_id;
            $this->onwards_query->type_id = null;
         }
      }
      if(isset($oscailt_page_setting['page_author']) and $use_page_author)
      {
         if(in_array($oscailt_page_setting['page_author'],$this->author_names ) or count($this->author_names) == 0)
         {
            $this->author_names = array($oscailt_page_setting['page_author']);
         }
      }
   }



   function includesTopicID($t_id)
   {
      if (count($this->topic_id) == 0 or in_array($t_id, $this->topic_id)) return true;
      return false;
   }

   function includesTypeID($t_id)
   {
      if (count($this->type_id) == 0 or in_array($t_id, $this->type_id)) return true;
      return false;
   }

   function includesRegionID($r_id)
   {
      if (count($this->region_id) == 0 or in_array($r_id, $this->region_id)) return true;
      return false;
   }

   function includesLanguageID($l_id)
   {
      if (count($this->language_id) == 0 or in_array($l_id, $this->language_id)) return true;
      return false;
   }

   function getItemHighlights(&$item, $is_comment=false)
   {
      global $system_config, $editor_session, $graphics_store;
      if(!$editor_session->isSessionOpen()) return;
      $highlights = array();
      if($this->getDisplayOption('showhidden') == 'true' && $item->hidden==true)
      {
         if($this->getDisplayOption('usestatusicons') == 'true')
         {
            $highlights[] = "<img src='$graphics_store"."hide.gif' class='itemoption' alt='hidden'>";
         }
         else $highlights[] = 'hidden';
      }
      if(!$is_comment && $this->getDisplayOption('showlocked') == 'true' && $item->locked==true)
      {
         if($this->getDisplayOption('usestatusicons') == 'true')
         {
            $highlights[] = "<img src='$graphics_store"."lock.gif' class='itemoption' alt='locked'>";
         }
         else $highlights[] = 'locked';
      }
      if(!$is_comment && $this->getDisplayOption('showsticky') == 'true' && $item->sticky_time>0)
      {
         if($this->getDisplayOption('usestatusicons') == 'true')
         {
            $highlights[] = "<img src='$graphics_store"."stick.gif' class='itemoption' alt='sticky'> sticky until ".strftime("%d/%b",$item->sticky_time+$system_config->timezone_offset);
         }
         else $highlights[] = "sticky until ".strftime("%d/%b",$item->sticky_time+$system_config->timezone_offset);
      }

      if(count($highlights) > 0)
      {
         $str = "<span class='itemstatus'>";
         foreach($highlights as $high)
         {
            $str .= $high;
         }
         return $str."</span>\n";
      }
      return "";
   }


}


