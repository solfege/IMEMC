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

/************************************************************************/
/* Abstract base class of all runtime display classes - has common      */
/* functionality to read stubs from cache files and a few other things  */
/************************************************************************/

require_once("objects/storyquery.inc");

class displayObject
{
   var $obj_id;
   var $site_settings;
   var $docs;
   //associative arrays of option settings for this object...
   var $admin_options;//can't be changed by user in request
   var $user_options;//can be set by user in request..
   var $display_options;//array of what is to be displayed...
   //the query object for fetching data from the database for display
   var $query;
   //the query object for making links.
   var $onwards_query;

   function displayObject($obj_id)
   {
      $this->obj_id = $obj_id;
      $this->initiallise();
   }

   function initiallise()
   {
      global $pseudo_directory_offset, $OSCAILT_SCRIPT;
      $this->docs = $pseudo_directory_offset.$OSCAILT_SCRIPT;
   }

   function setFilters($a, $b, $c, $d, $e){}

   function getDisplayOption($key)
   {
      if(!isset($this->display_options[$key])) return "";
      return $this->display_options[$key];
   }

   function setAdminOptions($opt)
   {
      $this->admin_options = stripslashes_deep($opt);
   }

   function setUserOptions($opt)
   {
      $this->user_options = $opt;
   }

   function setDisplayOptions()
   {
      global $user_prefs, $page_title;
      $this->display_options = $this->admin_options;
      foreach(array_keys($this->user_options) as $opt)
      {
         if(isset($user_prefs->non_default_preferences[$opt]))
         {
            if($user_prefs->non_default_preferences[$opt] == 'on')
            {
               $this->display_options[$opt] = 'true';
            }
            else
            {
               $this->display_options[$opt] = $user_prefs->non_default_preferences[$opt];
            }
         }
         else
         {
            $this->display_options[$opt] = $this->user_options[$opt];
         }
      }
   }

   function setDisplayOption($k, $v)
   {
      $this->display_options[$k] = $v;
   }

   function setSiteSettings($settings_array)
   {
      $this->site_settings = $settings_array;
   }


   function writeNavArray($hednavs, $story_type="")
   {
      echo "<div class='site-navigation'>";
      foreach($hednavs as $hednav)
      {
         if(count($hednavs) > 1 && ($hednav == 'otherviews'))
         {
            echo "<br />";
         }
         echo "<span class='navigation-element'>";
         if($hednav == 'context')
         {
            $this->writePageContext($this->getDisplayOption('title'));
         }
         elseif($hednav == 'prevnext')
         {
            $this->writeNextPrevLinks($story_type);

         }
         elseif($hednav == 'otherviews')
         {
            $this->writeOtherViewLinks();
         }
         echo "</span>\n";
      }
      echo "</div>\n";
   }

   function writeUserMessage($msg)
   {
      echo "<div class='user-message'>".$msg."</div>";
   }

   function writePageContext($tit)
   {
      global $pageFilterText, $system_config;
      echo "<b>";
      echo $pageFilterText. " ". $tit;
      echo "</b>";
   }

   function writeOtherViewLinks()
   {}

   function writeNextPrevLinks($story_type="stories")
   {}

   function writeDisplayHeader($story_type="stories")
   {
      if(isset($this->display_options['header']) && $this->getDisplayOption('header') != "")
      echo $this->getDisplayOption('header');

      if(isset($this->display_options['headernavigation']) and count($this->getDisplayOption('headernavigation')) > 0)
      {
         $this->writeNavArray($this->getDisplayOption('headernavigation'), $story_type);
      }
   }

   function writeDisplayFooter($story_type="stories")
   {
      if(isset($this->display_options['footernavigation']) and count($this->getDisplayOption('footernavigation')) > 0)
      {
         $this->writeNavArray($this->getDisplayOption('footernavigation'), $story_type);
      }
      if(isset($this->display_options['footer']) && $this->getDisplayOption('footer') != "")
      echo "<div class='display-footer'>".$this->getDisplayOption('footer')."</div>";
   }

   //generate text links to other objects
   function getFullLink($obj_id=-1)
   {
      return $this->getFilterLink($obj_id, true);
   }

   function getExtendedLink($extras)
   {
      $lnk = $this->getSimpleLink();
      $lnk = addLinkParameters($lnk, $extras);
      return $lnk;
   }

   function getFilterLink($obj_id=-1, $is_full = false, $query_obj = null)
   {
      global $system_config, $redirectList, $topicList, $typeList, $regionList, $languageList, $pseudo_directory_offset, $oscailt_story_query;
      if(!isset($oscailt_story_query))
      {
         $oscailt_story_query = new StoryQuery();
         $oscailt_story_query->loadFormValues();
      }
      if($query_obj == null)
      {
         if(isset($this->onwards_query) and $this->onwards_query != null)
         {
            $query_obj = $this->onwards_query->copy();
         }
         else
         {
            $query_obj = $oscailt_story_query->copy();
         }
         if(isset($this->query) and $this->query != null)
         {
            $query_obj = $this->query->copyDetails($query_obj);
         }
      }
      if($obj_id == -1) $obj_id = $this->obj_id;
      if($system_config->use_friendly_urls)
      {
         $red = $redirectList->getRedirect($obj_id);
         if($red !== false)
         {
            $prim = $red->getPrimaryRedirect();
            if($prim !== false)
            {
               $red_class = strtolower(get_class($red));
               //$this->onwards_query = $this->query->copy();
               $query_obj->obj_id = null;
               $lnk_base = $pseudo_directory_offset.$prim;
               if($red_class == 'filteredredirector')
               {
                  if(!is_array($query_obj->topic_id) && ($query_obj->topic_id != null))
                  {
                      $topic=$topicList->getTopicByID($query_obj->topic_id);
                      if($topic!= null)
                      {
                        $lnk_base.= "/".$topic->getStrippedName();
                        $query_obj->topic_id= null;
                     }
                  }
                  $region=$regionList->getRegionByID($query_obj->region_id);
                  if($region!=null)
                  {
                     $lnk_base.= "/".$region->getStrippedName();
                     $query_obj->region_id= null;
                  }
                  $type=$typeList->getTypeByID($query_obj->type_id);
                  if($type!=null)
                  {
                     $lnk_base.= "/".$type->getStrippedName();
                     $query_obj->type_id= null;
                  }
                  $language=$languageList->getLanguageByID($query_obj->language_id);
                  if($language!=null)
                  {
                     $lnk_base.= "/".$language->getStrippedName();
                     $query_obj->language_id= null;
                  }
               }
               elseif($red_class == 'articleredirector')
               {
                  if($query_obj->story_id != null)
                  {
                     $lnk_base .= "/".$query_obj->story_id;
                     $query_obj->story_id = null;
                  }
               }
               //elseif($red_class == 'redirector')
               //{

               //}
               $str = $lnk_base.$query_obj->getQueryString($is_full);
               return $str;
            }
         }
      }
      $query_obj->obj_id = $obj_id;
      $str = $this->docs.$query_obj->getQueryString($is_full);
      return $str;
   }

   function getSimpleLink($obj_id=-1)
   {
      global $redirectList, $pseudo_directory_offset, $system_config;
      if($obj_id == -1) $obj_id = $this->obj_id;
      if($system_config->use_friendly_urls)
      {
         $red = $redirectList->getRedirect($obj_id);
         if($red !== false)
         {
            $prim = $red->getPrimaryRedirect();
            if($prim !== false)
            {
               return $pseudo_directory_offset.$prim;
            }
         }
      }
      return $this->docs.'?obj_id='.$obj_id;
   }

   function getStoryLink($story_id=-1)
   {
      global $system_config, $redirectList;
      if($story_id == -1)
      {
         $story_id =  $this->query->story_id;
      }
      $lnk = $this->getSimpleLink($this->site_settings['articleobject']);
      if(strpos($lnk, '?') !== false or $system_config->use_friendly_urls == false)
      {
         $lnk = addLinkParameters($lnk, array("story_id=$story_id"));
      }
      else
      {
         $red = $redirectList->getRedirect($this->site_settings['articleobject']);
         $lnk_extra = "/$story_id";
         if($red)
         {
            $title_lnk = $red->getStoryTitleLink($story_id);
            if($title_lnk)
            {
               $lnk_extra = "/$title_lnk";
            }
         }
         $lnk = $lnk.$lnk_extra;
      }
      return $lnk;
   }



   function writeSavePrefsLink($extra = "", $obj_id=-1)
   {
      $lnk = $this->getFullLink($obj_id);
      $lnk = addLinkParameters($lnk, array("save_prefs=true"));
      echo "<a class='save-prefs' href='$lnk"."$extra'>save preference</a>";
   }

   function writeEmptyResultMessage($cls='usernotice')
   {
      echo "<div class='$cls'>".$this->getDisplayOption('emptyresultmsg')."</div>";
   }

   //function added at last moment to minimise sql calls
   //means that all stories are only loaded once per page
   function loadStory($id)
   {
      global $oscailt_page_setting;
      if(!isset($oscailt_page_setting['loaded_stories']))
      {
         $oscailt_page_setting['loaded_stories'] = array();
      }
      if(!isset($oscailt_page_setting['loaded_stories'][$id]))
      {
         $story = new Story();
         $story->story_id = $id;
         $story->load();
         $oscailt_page_setting['loaded_stories'][$id] =& $story;
      }
      $loaded_story =& $oscailt_page_setting['loaded_stories'][$id];
      return $loaded_story;
   }

   //starts main processing for object when it is on its own page
   function processRequest()
   {
      global $editor_session, $user_prefs, $oscailt_story_query, $pageFilterText;
      $this->setDisplayOptions();
      $user_prefs->readPrefsFromForm($this->user_options, $this->display_options);
      if(!isset($oscailt_story_query) or $oscailt_story_query == null)
      {
         $oscailt_story_query = new StoryQuery();
         $oscailt_story_query->loadFormValues();
      }
      $this->query = $oscailt_story_query->copy();
      $this->onwards_query = $oscailt_story_query->copy();
      if(isset($pageFilterText) && $pageFilterText != "") addToPageTitle($pageFilterText);
   }


}


