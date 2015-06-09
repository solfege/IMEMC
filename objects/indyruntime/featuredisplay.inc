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

require_once('attachmentbearingdisplay.inc');

class featureDisplay extends attachmentBearingDisplay
{

   function featureDisplay ($obj_id)
   {
      $this->attachmentBearingDisplay($obj_id);
      $this->style_prefix = 'feature';
   }

   //just needs to override the extent bit...
   function setQueryOptions()
   {
      parent::setQueryOptions(true);
      $this->query->selectFullMode();
      //$this->query->orderbyTimePosted();
   }

   function postQueryStoryFiddling()
   {
      global $oscailt_page_setting;
      parent::postQueryStoryFiddling();
      if(!isset($oscailt_page_setting['loaded_stories']))
      {
         $oscailt_page_setting['loaded_stories'] = array();
      }
      for($i=0; $i<count($this->stories); $i++)
      {
         $oscailt_page_setting['loaded_stories'][$this->stories[$i]->story_id] =& $this->stories[$i];
      }

   }

   function writeStorySummaryContent(&$story)
   {
      global $system_config;
      $story->splitFeatureContent();
      $content = "";
      if($story->hide_summary_on_feature_page)
      {
         $proc_summary= false;
      }
      else
      {
         $proc_summary = true;
      }
      if($proc_summary)
      {
         $content = "<P>$story->story_summary</P>";
      }
      $story->correctLinkDirectoryOffset($proc_summary, false);
      if($system_config->allow_oscailt_macros)
      {
         $story->applyOscailtMacros();
      }
      $content .= $story->story_content;
      echo "<div class='$this->style_prefix"."-content'>\n$content";
      $this->writeRelatedLink($story, $this->style_prefix);
      if(!$story->hide_attachments_on_feature_page)
      {
         $this->writeAttachmentList($story, true);
      }
      echo "</div>\n";

   }

   function writeStoryFooter(&$story)
   {
      $lnk = $this->getStoryLink();
      $read_more = $this->getDisplayOption('read_more_text');
      if($read_more == "") $read_more = "read / comment >>";
      $read_more_link = "<A class='$this->style_prefix"."-readmore' href='$lnk'>$read_more</A>";
      echo "<div class='$this->style_prefix"."-footer'>\n"; //div comes first for IE clearance hack
      echo "<span class='read-more'>$read_more_link</span>";
      $opts = $this->getDisplayOption('enddetails');
      if(isset($opts) and count($opts) > 0)
      {
         $this->writeStoryDetails($story, $opts, $this->style_prefix);
      }
      echo '</div>';
   }

   function writeRelatedLink(&$story, $style_prefix)
   {
   }
}
?>
