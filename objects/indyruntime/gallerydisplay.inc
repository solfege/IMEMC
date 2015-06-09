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
require_once('newswiredisplay.inc');

class galleryDisplay extends newswireDisplay
{

   function galleryDisplay ($obj_id)
   {
      $this->newswireDisplay($obj_id);
   }

   function display($res = 0)
   {
      global $system_config, $editor_session;
      if($this->empty_display)
      {
         echo "<p class=editornotice>problem with site filter settings</p>";
         return;
      }

      $this->integrateFilters();
      $this->setQueryOptions(true);
      $this->query->attachment_query=true;
      $this->query->media_type="IMAGE";
      $this->stories = $this->query->execute();
      //op_r($this->stories);
      $this->preloadImageTitles();
      $this->writeGallery();
   }

   function preloadImageTitles()
   {
      global $oscailt_page_setting;
      $story_titles_to_fetch = array();
      for($i = 0; $i<count($this->stories); $i++)
      {
         if($this->stories[$i]->description == "")
         {
            $s_id = $this->stories[$i]->story_id;
            if(isset($oscailt_page_setting['story_titles']) && isset($oscailt_page_setting['story_titles'][$s_id]))
            {
               $this->stories[$i]->description = $oscailt_page_setting['story_titles'][$s_id];
            }
            elseif(isset($oscailt_page_setting['loaded_stories']) && isset($oscailt_page_setting['loaded_stories'][$s_id]))
            {
               $this->stories[$i]->description = $oscailt_page_setting['loaded_stories'][$s_id]->story_id;
            }
            else
            {
               if(!isset($story_titles_to_fetch[$s_id])) $story_titles_to_fetch[$s_id] = array();
               $story_titles_to_fetch[$s_id][] = $i;
            }
         }
      }
      if(count($story_titles_to_fetch) > 0)
      {
         global $prefix, $dbconn;
         $sql = "SELECT story_id, story_title from ".$prefix."_stories where story_id=".implode(" OR story_id=", array_keys($story_titles_to_fetch));
         $result = sql_query($sql, $dbconn, 2);
         while(list($story_id,$story_title) = sql_fetch_row($result, $dbconn))
         {
            if(!isset($oscailt_page_setting['story_titles']))
            {
               $oscailt_page_setting['story_titles'] = array();
            }
            if(!isset($oscailt_page_setting['story_titles'][$story_id]))
            {
               $oscailt_page_setting['story_titles'][$story_id] = $story_title;
            }
            if(isset($story_titles_to_fetch[$story_id]))
            {
               foreach($story_titles_to_fetch[$story_id] as $position)
               {
                  $this->stories[$position]->description = $story_title;
               }
            }
         }
      }

   }

   function retrieveHeadlines()
   {
      global $oscailt_story_query;
      $this->setDisplayOptions();
      if(!isset($oscailt_story_query))
      {
         $oscailt_story_query = new StoryQuery();
         $oscailt_story_query->loadFormValues();
      }
      $this->query = $oscailt_story_query->copy();
      $use_ptype = true;
      $use_ptopic = true;
      $use_pregion = true;
      $use_planguage = true;
      $use_pauthor = true;
      if($this->getDisplayOption("context_sensitive_type") != 'true') $use_ptype = false;
      if($this->getDisplayOption("context_sensitive_language") != 'true') $use_planguage = false;
      if($this->getDisplayOption("context_sensitive_region") != 'true') $use_pregion = false;
      if($this->getDisplayOption("context_sensitive_topic") != 'true') $use_ptopic = false;
      if($this->getDisplayOption("context_sensitive_author") != 'true') $use_pauthor = false;
      if($this->getDisplayOption("time_delay") > 0)
      {
         //set up the value for
         $time_offset = (((int)(time() / 3600)) - $this->getDisplayOption("time_delay")) * 3600;
         $this->setDisplayOption("time_posted_upper_limit", $time_offset);
      }

      $this->integrateFilters($use_ptopic, $use_pregion, $use_planguage, $use_ptype, $use_pauthor);
      $this->setQueryOptions(false);
      $this->query->maskHeadlineQueryFields($use_pauthor);
      $this->query->attachment_query=true;
      $this->query->media_type="IMAGE";
      if($this->getDisplayOption("oneperstory") == 'true')
      {
         $this->query->one_image_per_story = true;
      }
      if($this->getDisplayOption("featurized_only") == 'true')
      {
         $this->query->featurized_attachment_query=true;
      }
      global $performance_test;
      if($performance_test > 6) markTime("SQL statement prepared");
      $this->stories = $this->query->execute();
      if($performance_test > 6) markTime("SQL statement executed");
      $this->preloadImageTitles();
      if($performance_test > 6) markTime("Image titles pre-loaded");
      $this->writeHeadlineGallery();
   }


   function writeHeadlineGallery()
   {
      global $system_config, $editor_session, $page_style;
      $images = $this->stories;
      $rheight = $this->getDisplayOption("resizeheight");
      $rwidth = $this->getDisplayOption("resizewidth");
      $maxfsize = $this->getDisplayOption("maxfilesize");
      if($maxfsize == 0) $maxfsize =  1000;
      $image_htmls = array();
      if(count($images) == 0)
      {
            $this->writeEmptyResultMessage('columnnotice');
      }
      else
      {
         echo "<table class='$page_style"."-gallery'>\n";
         echo "<tr class='$page_style"."-gallery'>";
         foreach($images as $image)
         {
            $imageConfig = new ImageConfig($rwidth,$rheight,true,true,$maxfsize,array(0xFF, 0xFF, 0xFF),0,0,0,0,0);
            $transformedImage_info=$image->getTransformedImageURL($imageConfig);
            if($transformedImage_info)
            {
               $image_htmls[] = $this->getImageHTML($image, $transformedImage_info);
            }
         }
         $first = true;
         foreach($image_htmls as $img)
         {
            if($first) $first = false;
            elseif($this->getDisplayOption("display") == 'column') echo "</tr><tr>\n";
            echo "<td class='$page_style"."-gallery'>";
            echo $img;
            echo '</td>';
         }
         echo "</tr></table>";
      }
   }

   function writeGallery()
   {
      global $system_config, $editor_session;
      $this->writeDisplayHeader("photos");
      $this->writeGalleryImages();
      $currentoffset=$this->query->results_offset;
      $this->writeDisplayFooter("photos");
   }


   function getImageTitle(&$image)
   {
      if(strlen($image->description)>0) $image_title=$image->getDisplayName();
      else
      {
         $story = $this->loadStory($image->story_id);
         $image_title=$story->story_title;
      }
      return $image_title;
   }

   function getImageHTML(&$image, $url_info)
   {
      global $pseudo_directory_offset;
      if(isRelativePath($url_info[0]))
         $url_info[0] = $pseudo_directory_offset.$url_info[0];
      $this->query->story_id=$image->story_id;
      $lnk = $this->getStoryLink();
      if($image->comment_id>0)
      {
         $lnk = addLinkParameters($lnk, array("comment_limit=0", "condense_comments=false"));
      }
      $lnk .= '#attachment'.$image->attachment_id;
      $lnk_html = "<a href='$lnk'>";

      $img_title = $this->getImageTitle($image);
      $img_disp = $image->getDisplayName();

      $img_html = "<img class='gallery-thumbnail' src='$url_info[0]' $url_info[2] title='$img_title' alt='$img_disp'>";
      $this->query->story_id=null;
      return $lnk_html.$img_html."</a>";
   }

   function writeGalleryImages()
   {
      global $regionList,$topicList,$typeList,$system_config;
      $images = $this->stories;
      if(count($images) == 0)
      {
         $this->writeEmptyResultMessage();
      }
      else
      {
         $rheight = $this->getDisplayOption("resizeheight");
         $rwidth = $this->getDisplayOption("resizewidth");
         $maxfsize = $this->getDisplayOption("maxfilesize");
         $row=0;
         echo '<table class="gallery" align="center">'."\n";
         for($i=0;$i<count($images);$i++)
         {
            $image=$images[$i];
            $imageConfig = new ImageConfig($rwidth,$rheight,true,true,$maxfsize,array(0xFF, 0xFF, 0xFF),0,0,0,0,0);
            $transformedImage_info=$image->getTransformedImageURL($imageConfig);
            if($transformedImage_info)
            {
               if($row==0)
               {
                  echo'<tr class="gallery">';
               }
               echo '<td class="gallery">';
               echo $this->getImageHTML($image, $transformedImage_info);
               echo '</td>';
               $row++;
               if($row==5)
               {
                  echo "</tr>\n";
                  $row=0;
               }
            }
         }
         echo "</table>\n";
      }
   }

}
?>
