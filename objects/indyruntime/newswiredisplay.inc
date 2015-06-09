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

require_once('managingdisplay.inc');
require_once("objects/story.inc");
require_once("objects/attachment.inc");

$is_lc = false;

class newswireDisplay extends managingDisplay
{
   var $number_of_pic_stories;
   var $style_prefix = 'storysummary';
   var $style_prefix_alt = 'storysummary-alternative';
   var $style_prefix_promo = 'storysummary-promote';
   var $wrote_javascript = false;


   function newswireDisplay ($obj_id)
   {
      $this->managingDisplay($obj_id);
   }

   function display($res = 0)
   {
      global $system_config, $editor_session, $performance_test, $is_lc;
      if($this->getDisplayOption('upcomingevents') == "true") $is_lc = true;

      if($this->empty_display)
      {
         echo "<p class=editornotice>problem with site filter settings</p>";
         return;
      }
      SharedMemoryUpdate("newswire");
      $this->integrateFilters();
      $this->setQueryOptions(true);
      if($editor_session->isSessionOpen())
      {
         //$this->query->select_votes = true;
         $this->query->selectVotes();
      }
      $this->stories = $this->query->execute();
      if($performance_test > 6) markTime("Executed Newswire Query");
      $this->postQueryStoryFiddling(); //sticky merging & tidying up
      $this->writeNewswireHeader();
      if($this->getDisplayOption('includephoto') == "true")
      {
         $this->preloadStoriesImage();
      }
      if ( ($this->getDisplayOption('include_embed_video') == "true") || ($this->getDisplayOption('include_embed_audio') == "true") )
      {
         $preload_mode = 0;
	 // One or the other will be true, so value will be either 1, 2 or 3
         if ( $this->getDisplayOption('include_embed_video') == "true") $preload_mode++;
	 if ( $this->getDisplayOption('include_embed_audio') == "true") $preload_mode = $preload_mode + 2;
         $this->preloadStoriesEmbedVideo($preload_mode);
      }

      $this->writeStorySummaries();
      if($performance_test > 6) markTime("wrote summaries");
      $currentoffset=$this->query->results_offset;
      $this->writeMultiEditOptions("story", $this->getDisplayOption('allowmultis'), count($this->stories));
      $this->writeNewswireFooter();
      if($performance_test > 6) markTime("completed drawing newswire");
      if($editor_session->isSessionOpen())
      {
         $this->query->results_offset=$currentoffset;
         $editor_session->clipboard->writeClipboard($this->query);
      }
   }

   function preloadStoriesImage()
   {
      $stories_to_load = array();
      foreach($this->stories as $s)
      {
         if($s->image_amount > 0)
         {
            $stories_to_load[] = $s->story_id;
         }
      }
      if(count($stories_to_load) > 0)
      {
         global $prefix, $dbconn, $system_config, $editor_session;
         $preload_attachments = array();
         $where_part = "comment_id=0 AND (story_id=".implode (" OR story_id=", $stories_to_load).")";
         if($system_config->use_only_featured_photo_thumbnails_in_headlines)
         {
             $where_part .= " AND featurized=1";
         }
         if(!($editor_session->isSessionOpen() && $editor_session->editor->allowedReadAccessTo("viewhiddencontent")))
         {
            $where_part .= " AND hidden=0";
         }
         $where_part .= " AND image IS NOT NULL ";
         $grp_part = " GROUP BY story_id";
         $select_part = "SELECT attachment_id, story_id, comment_id, UNIX_TIMESTAMP(time_posted), attachment_file, description, image, video, audio, hidden, featurized FROM $prefix"."_attachments";
         $sql = $select_part." WHERE ".$where_part.$grp_part;

         $result = sql_query($sql, $dbconn, 2);
         $attachment = new Attachment();
         while(list($attachment->attachment_id,$attachment->story_id, $attachment->comment_id, $attachment->time_posted, $attachment->attachment_file, $attachment->description, $attachment->image, $attachment->video, $attachment->audio, $attachment->hidden, $attachment->featurized) = sql_fetch_row($result, $dbconn))
         {
            if(!isset($preload_attachments[$attachment->story_id]))
            {
               $preload_attachments[$attachment->story_id] = $attachment;
            }
            $attachment = new Attachment();
         }
         //op_r($preload_attachments);
         for($i=0; $i<count($this->stories);$i++)
         {
            $this->stories[$i]->attachments_loaded = true;
            if(isset($preload_attachments[$this->stories[$i]->story_id]))
            {
               $this->stories[$i]->image_attachments[] = $preload_attachments[$this->stories[$i]->story_id];
            }
         }
      }
   }

   // This probably could be merged with the one for image.
   function preloadStoriesEmbedVideo($preload_mode)
   {
      $stories_to_load = array();
      foreach($this->stories as $s)
      {
         if($preload_mode == 1) {
             if($s->video_amount > 0)
             {
                $stories_to_load[] = $s->story_id;
             }
         }
	 else if($preload_mode == 2) {
             if($s->audio_amount > 0)
             {
                $stories_to_load[] = $s->story_id;
             }
         }
	 else if($preload_mode == 3) {
             if($s->video_amount > 0 || $s->audio_amount > 0)
             {
                $stories_to_load[] = $s->story_id;
             }
         }
      }
      if(count($stories_to_load) > 0)
      {
         global $prefix, $dbconn, $system_config, $editor_session;
         $preload_attachments = array();
         $where_part = "comment_id=0 AND (story_id=".implode (" OR story_id=", $stories_to_load).")";
         if($system_config->use_only_featured_photo_thumbnails_in_headlines)
         {
             $where_part .= " AND featurized=1";
         }
         if(!($editor_session->isSessionOpen() && $editor_session->editor->allowedReadAccessTo("viewhiddencontent")))
         {
            $where_part .= " AND hidden=0";
         }
         switch ($preload_mode) {
             case 3:
             $where_part .= " AND (video IS NOT NULL OR audio IS NOT NULL) ";
             break;

             case 2:
             $where_part .= " AND audio IS NOT NULL ";
             break;

             case 1:
             default:
             $where_part .= " AND video IS NOT NULL ";
             break;
         }
         $grp_part = " GROUP BY story_id";
         $select_part = "SELECT attachment_id, story_id, comment_id, UNIX_TIMESTAMP(time_posted), attachment_file, description, image, video, audio, hidden, featurized FROM $prefix"."_attachments";
         $sql = $select_part." WHERE ".$where_part.$grp_part;

         $result = sql_query($sql, $dbconn, 2);
         $attachment = new Attachment();
         while(list($attachment->attachment_id,$attachment->story_id, $attachment->comment_id, $attachment->time_posted, $attachment->attachment_file, $attachment->description, $attachment->image, $attachment->video, $attachment->audio, $attachment->hidden, $attachment->featurized) = sql_fetch_row($result, $dbconn))
         {
            if(!isset($preload_attachments[$attachment->story_id]))
            {
               $preload_attachments[$attachment->story_id] = $attachment;
            }
            $attachment = new Attachment();
         }
         //op_r($preload_attachments);
         for($i=0; $i<count($this->stories);$i++)
         {
            $this->stories[$i]->attachments_loaded = true;
            if(isset($preload_attachments[$this->stories[$i]->story_id]))
            {
               if($preload_attachments[$this->stories[$i]->story_id]->isEmbeddedVideo() == true)
                  $this->stories[$i]->embed_video_attachments[] = $preload_attachments[$this->stories[$i]->story_id];

               if($preload_attachments[$this->stories[$i]->story_id]->isEmbeddedAudio() == true)
                  $this->stories[$i]->embed_audio_attachments[] = $preload_attachments[$this->stories[$i]->story_id];
            }
         }
      }
   }


   function writeNewswireHeader()
   {
      $this->writeDisplayHeader();
   }

   function writeNewswireFooter()
   {
      $this->writeDisplayFooter();
   }

   function writeStorySummaries()
   {
      global $performance_test;
      if(count($this->stories) == 0)
      {
         $this->writeEmptyResultMessage();
      }
      else
      {
         // See if any stories are hidden and if so, get the list of reasons for the hides
         $reasons_array = $this->getHideReasonsArray();

         for($i=0;$i<count($this->stories);$i++)
         {
            $story=$this->stories[$i];
            $this->query->story_id=$story->story_id;
            $this->writeStorySummary($story, $i+1, $reasons_array);
            $this->query->story_id=NULL;
            if($performance_test > 6) markTime("Wrote story summary $i");
         }
      }
   }

   function getHideReasonsArray()
   {
      global $editor_session, $dbconn;

      // See if any stories are hidden and if so, get the list of reasons for the hides
      if(($editor_session->isSessionOpen() && $editor_session->editor->allowedReadAccessTo("viewhiddencontent")))
      {
	 $hide_cnt=0;
	 $in_list="";
         for($i=0;$i<count($this->stories);$i++)
         {
               $story=$this->stories[$i];
	       if ($story->hidden != true) continue;
	       $hide_cnt++;
               $in_list .= "'".$story->story_id ."',";
         }
	 if ($hide_cnt > 0 ) {
               $reasons_array = getEditorialReasons($in_list, true);
         } else {
               $reasons_array = array();
         }
      } else {
         $reasons_array = array();
      }
      return $reasons_array;
   }

   function writeNextPrevLinks($story_type="stories")
   {
      global $pageFilterText, $system_config, $oscailt_story_query;
      $save_query = $this->query->copy();
      $currentoffset=$this->query->results_offset;
      $this->query->results_offset=0;
      $prevOffset=$currentoffset-$this->query->results_limit;
      if ($prevOffset < 0 ) $prevOffset = 0;
      $nextOffset=$currentoffset+$this->query->results_limit;
      $extra_query = "";
      $lnk_base = $this->getFullLink();
      if (  $prevOffset > 0)
      {
         ?>
         <A class="sitenavigation" href="<?=$lnk_base.$extra_query?>">first page</A>
         <?
      }
      if ( $currentoffset > 0)
      {
         if (  $prevOffset > 0)  echo "&nbsp;|&nbsp;";
         $this->query->results_offset=$prevOffset;
         $lnk = $lnk_base;
         $lnk = addLinkParameters($lnk, "results_offset=" .$this->query->results_offset);
         ?>
         <A class="sitenavigation" href="<?=$lnk.$extra_query?>">&lt;&lt; newer <?=$story_type?></A>
         <?
      }
      if (($currentoffset > 0) || (count($this->stories) == $this->query->results_limit) )
      {
         echo("[");
         $this->query->results_offset=$currentoffset;

         $n_page = $currentoffset / $this->query->results_limit + 1;
         # +1 because offset 9 or 10 is still the first page
         $cur_page = $n_page;
         if ($n_page > 5 )
         {
            $n_page = $n_page - 5;
            $this->query->results_offset=$currentoffset-(5 * $this->query->results_limit);
         }
         else if ($n_page > 0 )
         {
            $this->query->results_offset=$currentoffset-(($n_page -1) * $this->query->results_limit);
            $n_page = 1;
         }
         $n_prompts = 10;
         if(count($this->stories) < $this->query->results_limit)
         {
            $n_prompts = $cur_page-$n_page+1;
         }
         for ($i=$n_page; $i < $n_page+$n_prompts; $i++)
         {

            if ($i == $cur_page )
            {
            ?>
               <span class="navigationselected"><?=$i?></span>
            <?
            }
            else
            {
               $lnk = $lnk_base;
               $lnk = addLinkParameters($lnk, "results_offset=" .$this->query->results_offset);
            ?>
               <A class="sitenavigation" href="<?=$lnk.$extra_query?>"><?=$i?></A>
            <?
            }
            if($i<$n_page+$n_prompts-1) echo("|");
            $this->query->results_offset = $this->query->results_offset + $this->query->results_limit;
         }
         echo "] ";

         $this->query->results_offset=$prevOffset;
      }
      if(count($this->stories) == $this->query->results_limit)
      {
         $this->query->results_offset=$nextOffset;
         $lnk = $lnk_base;
         $lnk = addLinkParameters($lnk, "results_offset=" .$this->query->results_offset);
         ?>
            <A class="sitenavigation" href="<?=$lnk.$extra_query?>">older <?=$story_type?> &gt;&gt;</A>
         <?
      }
      $this->query->results_offset = 0;
      if($this->query->time_posted_upper_limit > 0 or $this->query->time_posted_lower_limit > 0)
      {
         echo "<BR>";
         $extra_query = "";//'&newswire_mode=archive';
         $upper_limit = $this->query->time_posted_upper_limit;
         $lower_limit = $this->query->time_posted_lower_limit;
         $strftime = "%A %B %d, %Y";
         if($lower_limit > 0)
         {
             if($upper_limit > 0)
             {
                $time_str = "<b>".strftime($strftime, $upper_limit - 60) ."</b> to <b>". strftime($strftime, $lower_limit - 60)."</b>";
             }
             else
             {
                $time_str = "<b>Stories published since ".strftime($strftime, $lower_limit - 60)."</b>";
             }
         }
         else
         {
                $time_str = "<b>Stories published before ".strftime($strftime, $upper_limit - 60)."</b>";
         }
         echo $time_str;
         if($upper_limit > 0)
            $this->query->time_posted_upper_limit = $upper_limit + (60*60*24);
         if($lower_limit > 0)
            $this->query->time_posted_lower_limit = $lower_limit + (60*60*24);
         ?>
           [ <A class="sitenavigation" href="<?=$this->getFullLink().$extra_query?>">&lt;&lt; Next Day </A> |
         <?
         if($upper_limit > 0)
            $this->query->time_posted_upper_limit = $upper_limit - (60*60*24);
         if($lower_limit > 0)
            $this->query->time_posted_lower_limit = $lower_limit - (60*60*24);
         ?>
             <A class="sitenavigation" href="<?=$this->getFullLink().$extra_query?>"> Previous Day &gt;&gt; </A> ]
         <?
         $this->query->time_posted_upper_limit = $upper_limit;
         $this->query->time_posted_lower_limit = $lower_limit;
      }
      $this->query =& $save_query;
   }

   function writeOtherViewLinks()
   {
      $archlink = $this->getSimpleLink($this->site_settings['archiveobject']);
      $comlink = $this->getSimpleLink($this->site_settings['commentsobject']);
      $gallink = $this->getSimpleLink($this->site_settings['galleryobject']);
      $featlink = $this->getSimpleLink($this->site_settings['featureobject']);
   ?>
      <A class="sitenavigation" href="<?=$archlink?>">view stories by date</A>&nbsp; | &nbsp;
      <A class="sitenavigation" href="<?=$featlink?>">view features</A>&nbsp; | &nbsp;
      <A class="sitenavigation" href="<?=$comlink?>">view latest comments</A>&nbsp; | &nbsp;
      <A class="sitenavigation" href="<?=$gallink?>">view images</A>
   <?
   }


   function writeStorySummary(&$story, $story_num, $hide_reasons)
   {
      global $editor_session;
      if($story->type_id==1)
      {
         // we need to load full story to get feature tags.
         $story->load();
         $story->splitFeatureContent();
      }
      if($this->getDisplayOption('usealternatestyles') == 'true' && ($story_num % 2) == 0)
      {
         $style = $this->style_prefix_alt;
      }
      else
      {
         $style = $this->style_prefix;
      }
      if($this->getDisplayOption("promoted") == "true")
      {
         $style = $this->style_prefix_promo;
      }

      echo $this->getItemOptions($story,"story",true, $story_num);
      echo "<div class='$style'>\n";
      if($this->getDisplayOption('includephoto') == "true") $this->writeStoryImage($story);
      $this->writeStoryTitle($story, $style);
      if ($story->hidden == true ) {
         if (isset($hide_reasons[$story->story_id]) ) {
             $logs_url ="<a href='viewsitelog.php?log_type=action_db&filter_id=".$story->story_id."'>logs</a>";
             $this->writeUserMessage("<b>Hide Reason: (".$logs_url.")</b> " . $hide_reasons[$story->story_id] );
         }
      }
      $this->writeStorySubtitle($story, $style);
      $this->writeStorySummaryContent($story, $style);
      $this->writeStoryFooter($story, $style);
      echo "</div>\n";
   }

   /**********************************************
   /* Functions to write each section of the story summary
   /* These are seperated out to allow them to be more easily configurable
   /* Input: story reference
   /* Output: to screen
   /* Transforms: writes information about story depending on settings..
   //********************************************/
   function writeStoryTitle(&$story, $style_prefix)
   {
   ?>
      <div class="<?=$style_prefix?>-title">
   <?
   if ($story->hidden == true ) echo $story->story_id;
   ?>
         <?=$this->getItemHighlights($story);?> 
         <A class=<?=$style_prefix?>-title href="<?=$this->getStoryLink()?>"><?=$story->story_title?></A>
      </div>
   <?
   }

   function writeStorySummaryContent(&$story, $style_prefix)
   {
      global $newswire_summary_display_tags;
      $tag_str = '<'.implode("><",$newswire_summary_display_tags).'>';
      $lnk = $this->getStoryLink();
      $read_more = $this->getDisplayOption('read_more_text');
      if($read_more == "") $read_more = " more &gt;&gt;";
      $read_more_link = "<A class='$style_prefix"."-readmore' href='$lnk'>$read_more</A>";

      $this->writeEmbedVideo($story, $style_prefix);
      echo "<div class='$style_prefix"."-content'>\n";
      echo strip_tags($story->story_summary, $tag_str);
      echo " <span class='read-more'>$read_more_link</span>";
      $this->writeRelatedLink($story, $style_prefix);
      echo "</div>";
   }

   function writeEmbedVideo(&$story, $style_prefix)
   {
      $cls_str = "class='$style_prefix"."-relatedlink'";
      if($this->getDisplayOption("include_embed_video") == 'true')
      {
          if($story->embed_video_attachments != null)
	  {
		  
             $show_video_as_icon = $this->getDisplayOption('video_mode');
             for($kv=0; $kv < count($story->embed_video_attachments); $kv++)
	     {
                $vid_attachment = $story->embed_video_attachments[$kv];
                $videoType = substr($vid_attachment->attachment_file,11,2);
                $videoId = substr($vid_attachment->attachment_file,14);

		if ($show_video_as_icon == 'hidden' ) {
		    $embed_str = getEmbeddedVideoHTML($videoType, $videoId);
		    $provider = getEmbeddedVideoTypes(number_format($videoType));
		    // style=width:352px;height:312px;background:pink;border:1px;
		    if ($this->wrote_javascript == false ) {
		          ?><script type="text/javascript">
		            function loadvideoblock(sVideoBlockDiv) {
                             document.getElementById(sVideoBlockDiv).innerHTML='<?=$embed_str?>';
                            }
		            </script>
		          <?
                          $this->wrote_javascript = true;
                    }
		    // style="background:pink;border:1x;"
		    ?>
		         <br><div align=center id="videoBlock_<?=$vid_attachment->attachment_id?>" >
		         <div style="background:pink;border:1px;" >
		         <br><p><i>To stop your IP being automatically logged by the provider of the (<?=$provider?>) video content, we have not loaded it automatically. If you wish to proceed to watch the video, then please <a href="#" onclick="loadvideoblock('videoBlock_<?=$vid_attachment->attachment_id?>');">Click here to load the embedded video player for video Id <?=$videoId?></a></i></p><br>
		         </div>
		         </div>
		    <?

                } else {
                    ?><BR><DIV align=center> <?
		    echo getEmbeddedVideoHTML($videoType, $videoId, 210, 175);
                    ?> </div><BR> <?
                }
	     }
	  }
      }
      if($this->getDisplayOption("include_embed_audio") == 'true')
      {
          if($story->embed_audio_attachments != null)
	  {
             for($kv=0; $kv < count($story->embed_audio_attachments); $kv++)
	     {
                $aud_attachment = $story->embed_audio_attachments[$kv];
                $audioType = substr($aud_attachment->attachment_file,11,2);
                $audioId = substr($aud_attachment->attachment_file,14);

                ?><BR><DIV align=center> <?
                echo getEmbeddedAudioHTML(true, $audioId, $aud_attachment[$kv]->description);
                ?> </div><BR> <?
	     }
	  }
      }
   }

   function writeRelatedLink(&$story, $style_prefix)
   {
      $cls_str = "class='$style_prefix"."-relatedlink'";
      if($this->getDisplayOption("includerelatedlink") == 'true')
      {
         $txt_lnk = getURLLinkString($story->related_link, $cls_str);
         if($txt_lnk != "")
         {
            echo "<div $cls_str>";
            $intro = $this->getDisplayOption("relatedlink_text");
            if($intro != "")
            {
               echo "$intro: ";
            }
            echo $txt_lnk."</div>";
         }
      }
   }

   function writeStoryFooter(&$story, $style_prefix)
   {
      echo "<div class='$style_prefix"."-footer'>\n"; //div comes first for IE clearance hack
      $opts = $this->getDisplayOption('enddetails');
      if(isset($opts) and count($opts) > 0)
      {
         $this->writeStoryDetails($story, $opts, $style_prefix);
      }
      echo '</div>';
   }

   function writeStoryDetails(&$story, $opts, $style_prefix)
   {
      global $system_config, $graphics_store;
      echo "<span class='$style_prefix"."-details'>\n";

      foreach($opts as $opt)
      {
         if($opt == "br")
         {
            echo "<br />";
            continue;
         }
         if($opt == 'attachments')
         {
            $this->writeAttachmentDetails($story);
         }
         elseif($opt == 'comments')
         {
            $this->writeCommentDetails($story);
         }
         elseif($opt == 'author')
         {
            echo "<span class='$style_prefix"."-detail'>";
            if($system_config->use_icons_for_newswire_details)
            {
               echo "<img class='icon' src='".$graphics_store."person.gif' alt='author'> by ";
            }
            else
            {
              echo "by ";
            }
            echo $story->author_name;
            echo "</span> ";
         }
         elseif($opt == 'context')
         {
            echo "<span class='$style_prefix"."-detail'>";
            $this->writeCategoryContext($story);
            echo "</span> ";
         }
         elseif($opt == 'date')
         {
            echo " <span class='$style_prefix"."-detail'>";
            $time_format = $this->getDisplayOption('storydate_format');
            if($time_format == "") $time_format = $system_config->default_strftime_format;
            if($system_config->use_icons_for_newswire_details)
            {
               echo "<img class='icon' src='".$graphics_store."date.gif' alt='author'> ";
            }
            echo strftime($time_format, $story->time_posted+$system_config->timezone_offset);
            echo "</span> ";
         }
      }
      echo "</span>\n";
   }

   function writeStorySubtitle(&$story, $style_prefix)
   {
      $opts = $this->getDisplayOption('subtitledetails');
      if(isset($opts) and count($opts) > 0)
      {
         echo "<div class='$style_prefix"."-subtitle'>";
         $this->writeStoryDetails($story, $opts, $style_prefix);
         echo "</div>\n";
      }
   }

   function writeStoryImage(&$story)
   {
      global $system_config, $pseudo_directory_offset;
      if($system_config->newswire_thumbnails_enabled && $story->image_amount > 0)
      {
         $images = $story->getImageAttachments();
         if(count($images)>0)
         {
            $this->number_of_pic_stories++;
            if($this->number_of_pic_stories % 2 == 0) $style = "leftsummarypic";
            else $style="rightsummarypic";
            $transformedImage_info=$images[0]->getTransformedImageURL($system_config->story_summary_thumbnail);

            if($transformedImage_info!=null)
            {
               $transformedImageURL = $transformedImage_info[0];
               if(isRelativePath($transformedImageURL))
               {
                  $transformedImageURL = $pseudo_directory_offset.$transformedImageURL;
               }
               $lnk = $this->getStoryLink($story->story_id);
               ?>
               <a href="<?=$lnk?>"><img class="<?=$style?>" <?=$transformedImage_info[2]?> src="<?=$transformedImageURL?>" title="<?=$images[0]->getDisplayName()?>" alt="<?=$images[0]->getDisplayName()?>"></a>
               <?
            }
         }
      }
   }

   function writeCategoryContext(&$story, $show_lang=false)
   {
      global $system_config, $graphics_store;
      $n_query = new StoryQuery();
      $topic=$story->getTopic();
      $region=$story->getRegion();
      $type=$story->getType();
      $lang=$story->getLanguage();
      $parts = array();
      if(count($this->region_id) == 0 or (in_array($region->region_id, $this->region_id) and count($this->region_id) > 1))
      {
         $n_query->region_id = $region->region_id;
         $region_link = $this->getFilterLink(-1, false, $n_query);
         $n_query->region_id = null;
         $parts[] = "<a class='category-text' href='$region_link'>".strtolower($region->region_name)."</a>";
      }
      if(count($this->topic_id) == 0 or (in_array($topic->topic_id, $this->topic_id) and count($this->topic_id) > 1))
      {
         $n_query->topic_id = $topic->topic_id;
         $topic_link = $this->getFilterLink(-1, false, $n_query);
         $n_query->topic_id = null;
         $parts[] = "<a class='category-text' href='$topic_link'>".strtolower($topic->topic_name)."</a>";
      }
      if(count($this->type_id) == 0 or (in_array($type->type_id, $this->type_id) and count($this->type_id) > 1))
      {
         $n_query->type_id = $type->type_id;
         $type_link = $this->getFilterLink(-1, false, $n_query);
         $n_query->type_id = null;
         $parts[] = "<a class='category-text' href='$type_link'>".strtolower($type->type_name)."</a>";
      }
      if($show_lang and (count($this->language_id) == 0 or (in_array($lang->language_id, $this->language_id) and count($this->language_id) > 1)))
      {
         $n_query->language_id = $lang->language_id;
         $lang_link = $this->getFilterLink(-1, false, $n_query);
         $n_query->language_id = null;
         $parts[] = "<a class='category-text' href='$lang_link'>".strtolower($lang->language_name)."</a>";
      }

      if($system_config->use_icons_for_newswire_details)
      {
         echo "<img class='icon' src='".$graphics_store."star.gif' alt='category'> ";
      }
      echo implode(" / ", $parts);
   }

   function setQueryOptions($use_author = false)
   {
      global $system_config, $editor_session, $oscailt_page_setting;
      if(isset($oscailt_page_setting['obj_type']) && (strtolower($oscailt_page_setting['obj_type']) == 'article') && $this->query->story_id != null)
      {
         $this->query->addCustomWhereClause("s.story_id != ".$this->query->story_id);
      }
      $this->query->story_id=null;
      $this->query->comment_id=null;
      $this->query->selectSummaryMode();

      $this->query->use_excludes = true;
      if(isset($this->display_options['pagelimit']) and $this->getDisplayOption('pagelimit') > 0)
      {
         $this->query->results_limit=$this->getDisplayOption("pagelimit");
      }
      else $this->query->results_limit = 24;
      if($this->getDisplayOption('time_posted_upper_limit') != 0  and $this->query->time_posted_upper_limit == 0)
      {
         //echo "<P>set the upper limit to ".$this->getDisplayOption('time_posted_upper_limit')." ".strftime("%c", $this->getDisplayOption('time_posted_upper_limit'));
         $this->query->time_posted_upper_limit=$this->getDisplayOption('time_posted_upper_limit');
      }
      if($this->getDisplayOption('time_posted_lower_limit') != 0 and $this->query->time_posted_lower_limit == 0)
      {
         $this->query->time_posted_lower_limit=$this->getDisplayOption('time_posted_lower_limit');

      }
      if($this->getDisplayOption("ignorehidden") == "true")
      {
         $this->query->ignore_hidden = true;
      }
      elseif($this->getDisplayOption("hiddenonly") == "true")
      {
         $this->query->addCustomWhereClause("s.hidden = 1");
      }
      elseif($this->getDisplayOption("nohidden") == "true")
      {
         $this->query->addCustomWhereClause("s.hidden = 0");
      }
      if($this->getDisplayOption("lockedonly") == "true")
      {
         $this->query->addCustomWhereClause("s.locked = 1");
      }
      elseif($this->getDisplayOption("nolocked") == "true")
      {
         $this->query->addCustomWhereClause("s.locked = 0");
      }
      if($this->getDisplayOption("stickyonly") == "true")
      {
         $this->query->addCustomWhereClause("s.sticky_time > 0");
      }
      if($this->getDisplayOption("nosticky") == "true")
      {
         $this->query->addCustomWhereClause("s.sticky_time <= 0");
      }

      // Promoted stories
      if($this->getDisplayOption("promoted") == "true")
      {
         // The select votes option must be switch on for the where clause to work.
         $this->query->selectVotes();
         $this->query->addCustomWhereClause("IFNULL(v.vote_value,0) > 0");
      }

      if($this->getDisplayOption('upcomingevents') == "true")
      {
         $this->query->orderByEventTime();
         $this->query->orderAscending();
         // set lower time to today unless a historical display is requested
         //$this->query->event_time_upper_limit=$this->getDisplayOption('time_posted_upper_limit');
         if($this->query->event_time_lower_limit==null)
         {
            $this->query->event_time_lower_limit=strtotime("11:59pm yesterday");
         }
         // override any other time fields passed in request
         //$this->query->event_time_upper_limit=null;
         $this->query->time_posted_lower_limit=null;
         $this->query->time_posted_upper_limit=null;
         $this->query->event_time=null;
         $this->query->time_posted=null;
         # Forces the type_id to be set to the value for Events
         $this->query->type_id=array($system_config->event_type_id);
         //$this->query->selectFromStoryTableOnly();
      }

      if($this->query->ignore_hidden && $editor_session->isSessionOpen())
      {
         if(isset($_REQUEST["hidden"])) $query->hidden = $_REQUEST["hidden"];
         else $query->ignore_hidden=true;
      }

      if(count($this->author_names) > 0)
      {
         $this->query->author_name = null;
         $auths = array();
         foreach($this->author_names as $auth_name)
         {
            $auths[]= addslashes($auth_name);
         }
         if($system_config->use_vague_author_name)
         {
            $custom_str = "s.author_name LIKE '%". implode("%' OR s.author_name LIKE '%", $auths)."%'";
         }
         else
         {
            $custom_str = "s.author_name='". implode("' OR s.author_name='", $auths)."'";
         }
         $this->query->addCustomWhereClause($custom_str);
      }
      $this->query->selectFromAllTables();
   }

   function postQueryStoryFiddling()
   {
      if($this->getDisplayOption("respectsticky") == "true" and $this->getDisplayOption('upcomingevents') != "true")
      {
         $stickies = $this->getStickyStories();
         if(count($stickies) > 0)
         {
            $this->mergeStickyStories($stickies);
         }

      }
      # set fields back to defaults so they don't show in query string
      if($this->getDisplayOption('upcomingevents') == "true")
      {
         $this->query->type_id=null;
         $this->query->event_time_lower_limit=null;
         $this->query->event_time_upper_limit=null;
      }
      $this->query->orderDescending();
      $this->query->orderByTimePosted();
   }

   function getStickyStories()
   {
      global $system_config;
      $squery = $this->query->copy();
      $squery->selectSummaryMode();
      //$squery->results_limit=null;
      $squery->addCustomWhereClause("s.sticky_time > 0");
      $stories = $squery->execute();
      $stickystories= array();
      if(is_array($stories) and count($stories) > 0)
      {
         foreach($stories as $story)
         {
            if($story->sticky_time<time()) $story->unstick();
            else array_push($stickystories,$story);
         }
      }
      return $stickystories;
   }

   function mergeStickyStories(&$stickystories)
   {
      global $system_config;
      $storiesToMerge = array();
      foreach($stickystories as $story)
      {
         $alreadypresent=false;
         foreach($this->stories as $checkstory)
         {
            if($story->story_id==$checkstory->story_id)
            {
               $alreadypresent=true;
               break;
            }
         }
         if($alreadypresent) continue;
         array_push($storiesToMerge,$story);
      }

      $this->stories = array_merge($storiesToMerge,array_reverse($this->stories));
      // sort by stickiness and then by date
      usort($this->stories,"latestStoryCompare");
      //op_r($this->stories);
      if($this->getDisplayOption('pagelimit') > 0 && count($this->stories) > $this->getDisplayOption('pagelimit')) $this->stories = array_slice ($this->stories, 0, $this->getDisplayOption('pagelimit'));

   }

   function writeAttachmentDetails(&$story)
   {
      global $system_config;
      if($story->image_amount > 0)
      {
         $this->write_attachment_detail_seperator();
         $cmt = 'image';
         if($story->image_amount > 1) $cmt .= 's';
         echo $story->getIconTag("image"). " $story->image_amount $cmt ";
      }

      if($story->video_amount > 0)
      {
         $this->write_attachment_detail_seperator();
         $cmt = 'video file';
         if($story->video_amount > 1) $cmt .= 's';
         echo $story->getIconTag("video"). " $story->video_amount $cmt ";
      }

      if($story->audio_amount > 0)
      {
         $this->write_attachment_detail_seperator();
         $cmt = 'audio file';
         if($story->audio_amount > 1) $cmt .= 's';
         echo $story->getIconTag("audio"). " $story->audio_amount $cmt ";
      }

      if($story->miscellaneous_amount>0)
      {
         $this->write_attachment_detail_seperator();
         $cmt = 'attached file';
         if($story->miscellaneous_amount > 1) $cmt .= 's';
         echo $story->getIconTag("miscellaneous"). " $story->miscellaneous_amount $cmt ";
      }
   }


   function write_attachment_detail_seperator()
   {
      echo "&nbsp; ";
   }

   function writeCommentDetails(&$story)
   {
      global $system_config;
      if($story->comments_amount > 0)
      {
         $time_format = $this->getDisplayOption('storydate_format');
         if($time_format == "") $time_format = $system_config->default_strftime_format;
         $this->write_attachment_detail_seperator();
         $cmt = 'comment';
         if($story->comments_amount>1) $cmt .= 's';
         $cmtlnk = '<a class="category-text" href="'.$this->getStoryLink().'#comments">'.$story->comments_amount." ".$cmt."</a>";
         echo $story->getIconTag("comment");
         echo " ".$cmtlnk;
         echo " (last - ". strtolower(strftime($time_format,$story->time_last_comment_posted+$system_config->timezone_offset)).") ";
      }
   }


   /**********************************************
   /* Functions to write the headline columns
   /********************************************/
   function retrieveHeadlines()
   {
      global $editor_session, $oscailt_story_query, $performance_test;
      $this->setDisplayOptions();
      if(!isset($oscailt_story_query))
      {
         $oscailt_story_query = new StoryQuery();
         $oscailt_story_query->loadFormValues();
      }
      if($performance_test > 7) markTime("Preparing Headline Query");

      $this->query = $oscailt_story_query->copy();
      $this->onwards_query = $oscailt_story_query->copy();
      $use_ptype = true;
      $use_ptopic = true;
      $use_pregion = true;
      $use_planguage = true;
      $use_pauthor = true;
      if($this->getDisplayOption("context_sensitive_type") != 'true') $use_ptype = false;
      if($this->getDisplayOption("context_sensitive_language") != 'true') $use_planguage = false;
      if($this->getDisplayOption("context_sensitive_region") != 'true') $use_pregion = false;
      if($this->getDisplayOption("context_sensitive_topic") != 'true') $use_ptopic = false;
      if($this->getDisplayOption("context_sensitive_author") != 'true' or $this->getDisplayOption("filter_author") != "") $use_pauthor = false;
      $this->integrateFilters($use_ptopic, $use_pregion, $use_planguage, $use_ptype, $use_pauthor);
      $this->setQueryOptions(true);
      $this->query->maskHeadlineQueryFields($use_pauthor);
      $this->query->selectTickerMode();
      if($this->getDisplayOption('includecommentcount') != 'true' and $this->getDisplayOption('includephoto') != 'true')
      {
         $this->query->selectFromStoryTableOnly();
         if($this->query->order_mode == "timecommentsposted")
         {
            $this->query->orderByTimePosted();
         }
      }
      $this->stories = $this->query->execute();
      if($performance_test > 7) markTime("Headline Query Executed");
      $this->postQueryStoryFiddling(); //sticky merging & tidying up
      if($this->getDisplayOption('includephoto') == "true")
      {
         $this->preloadStoriesImage();
      }
      if($this->getDisplayOption('contextual_mode') == 'true')
      {
         $this->writeContextualHeader($this->getDisplayOption('title'), $this->getDisplayOption('hsize'), $use_ptype, $use_planguage, $use_pregion, $use_ptopic, $use_pauthor);
      }
      $this->writeStoryHeadlines();
      $this->writeHeadlinesFooter();
      $currentoffset=$this->query->results_offset;
   }

   function writeContextualHeader($title, $hsize, $use_ptype, $use_planguage, $use_pregion, $use_ptopic, $use_pauthor)
   {
      global $pageTopic, $pageLanguage, $pageRegion, $pageType, $pageFilterText, $oscailt_page_setting, $page_style;
      $this_filter_text = "";
      $save_query = $this->onwards_query->copy();
      $this->query = new StoryQuery();
      if($this->getDisplayOption("context_sensitive_author") =='true' && isset($oscailt_page_setting['page_author']))
      {
         $this->onwards_query->author_name = $oscailt_page_setting['page_author'];
         $url = $this->getFilterLink($this->site_settings['newswireobject']);
         $this->onwards_query->author_name = null;
         $this_filter_text .= " ".$oscailt_page_setting['page_author'];
      }
      if($this->getDisplayOption("context_sensitive_region") =='true' && $pageRegion != null)
      {
         $this->onwards_query->region_id = $pageRegion->region_id;
         $url = $this->getFilterLink($this->site_settings['newswireobject']);
         $this->onwards_query->region_id = null;
         $this_filter_text .= " ".$pageRegion->getName();
      }
      if($this->getDisplayOption("context_sensitive_topic") =='true' && $pageTopic != null)
      {
         $this->onwards_query->topic_id = $pageTopic->topic_id;
         $url = $this->getFilterLink($this->site_settings['newswireobject']);
         $this->onwards_query->topic_id = null;
         $this_filter_text .= " ".$pageTopic->getName();
      }
      if($this->getDisplayOption("context_sensitive_type") =='true' && $pageType != null)
      {
         $this->onwards_query->type_id = $pageType->type_id;
         $url = $this->getFilterLink($this->site_settings['newswireobject']);
         $this->onwards_query->type_id = null;
         $this_filter_text .= " ".$pageType->getName();
      }
      if($this->getDisplayOption("context_sensitive_language") =='true'&& $pageLanguage != null)
      {
         $this->onwards_query->language_id = $pageLanguage->language_id;
         $url = $this->getFilterLink($this->site_settings['newswireobject']);
         $this->onwards_query->language_id = null;
         $this_filter_text .= " (".$pageLanguage->getLanguageCodePrefix().")";
      }
      if($this_filter_text != "")
      {
         $title .= htmlspecialchars($this_filter_text);
      }
      $lnk = "<A class='$page_style' href='$url'>";
      if($hsize > 0 and $hsize <= 5)
      {
         $str = "<H".$hsize." class='$page_style'>".$lnk.$title."</A></H".$hsize.">";
      }
      else $str = $lnk.$title."</A><BR>";
      echo $str;
      $this->query =& $save_query;
   }

   function writeStoryHeadlines()
   {
      // global $regionList,$topicList,$typeList,$system_config;
      $this->writeHeadlinesHeaderInfo();
      if(count($this->stories) == 0)
      {
         $this->writeEmptyResultMessage('columnnotice');
      }
      else
      {
         for($i=0;$i<count($this->stories);$i++)
         {
            $story =& $this->stories[$i];
            $this->query->story_id=$story->story_id;
            $this->writeStoryHeadline($story, $i+1);
            $this->query->story_id=NULL;
         }
      }
   }

   function getNewswireFeaturedImage()
   {
      global $system_config;
      return $system_config->featured_image;
   }

   function writeHeadlinesHeaderInfo()
   {
      //options
      global $system_config, $page_style;
      $contextstring = "";
      if($this->getDisplayOption('includecontext') == 'true')
      {
         $href = "";
         $contextstring = $this->getContextString();
      }

      $datestring = "";
      if($this->getDisplayOption('includedate') != '')
      {
         $datestring = strftime($this->getDisplayOption('includedate'),time()+$system_config->timezone_offset);
      }
      $iconstring = "";
      if($this->getDisplayOption('includeicons') == 'true')
      {
         $icons = array();
         $media_type_query = new StoryQuery();
         $media_type_query->loadFormValues();
         if($media_type_query->type_id==1) $media_type_query->type_id=null;
         $media_type_query->results_offset=0;
         $media_type_query->media_type="TEXT";
         $icons[] = $this->getMediaTypeIconLink('text', $media_type_query);
         if($system_config->allow_story_image_attachments || $system_config->   allow_comment_image_attachments)
         {
            $media_type_query->media_type="IMAGE";
            $icons[] = $this->getMediaTypeIconLink('image', $media_type_query);
         }
         if($system_config->allow_story_video_attachments || $system_config->allow_comment_video_attachments)
         {
            $media_type_query->media_type="VIDEO";
            $icons[] = $this->getMediaTypeIconLink('video', $media_type_query);
         }
         if($system_config->allow_story_audio_attachments || $system_config->allow_comment_audio_attachments)
         {
            $media_type_query->media_type="AUDIO";
            $icons[] = $this->getMediaTypeIconLink('audio', $media_type_query);
         }
         foreach($icons as $icon_str)
         {
            $iconstring .= "<span class='mediaicon'>$icon_str</span>";
         }
      }
      $cls = $page_style;
      if($contextstring != "") $contextstring = "<P class='$cls"."-context'>$contextstring</p>";
      if($datestring != "") $datestring = "<P class='$cls"."-date'>$datestring</p>";
      if($iconstring != "") $iconstring = "<P class='$cls"."-icons'>$iconstring</p>";
      echo $contextstring.$datestring.$iconstring;
   }


   function writeHeadlineThumbnail(&$story)
   {
      global $system_config, $pseudo_directory_offset;
      $images = $story->getImageAttachments();
      if(count($images)>0)
      {
         $this->number_of_pic_stories++;
         if($this->number_of_pic_stories % 2 == 0) $style = "leftheadlinepic";
         else $style="rightheadlinepic";
         $transformedImage_info=$images[0]->getTransformedImageURL($system_config->story_headline_thumbnail);
         if($transformedImage_info!=null)
         {
            $transformedImageURL = $transformedImage_info[0];
            if(isRelativePath($transformedImageURL))
               $transformedImageURL = $pseudo_directory_offset.$transformedImageURL;
            $lnk = $this->getStoryLink($story->story_id);
            ?>
            <a href="<?=$lnk?>"><img class="<?=$style?>" <?=$transformedImage_info[2]?> src="<?=$transformedImageURL?>" title="<?=$images[0]->getDisplayName()?>" alt="<?=$images[0]->getDisplayName()?>"></a>
            <?
            return true;
         }
      }
      return false;
   }

   function writeHeadlinesFooter()
   {
      global $page_style;
      $txt = trim($this->getDisplayOption("read_more_text"));
      if($txt != "" && count($this->stories) == $this->query->results_limit)
      {
         $lnk = $this->getFilterLink($this->getDisplayOption("full_page_id"));
         ?>
         <span class="<?=$page_style?>-readmore"><a class="<?=$page_style?>" href="<?=$lnk?>"><?=$txt?></a></span>
         <?
      }
   }

   function getContextString()
   {
      global $page_style, $pageRegion, $pageTopic, $pageLanguage, $pageType, $oscailt_page_setting;
      $obj_id = $this->getDisplayOption("full_page_id");
      $n_query = $this->onwards_query->copy();
      $parts = array();;
      $n_query->region_id = null;
      $n_query->topic_id = null;
      $n_query->type_id = null;
      $n_query->language_id = null;
      if(isset($pageRegion) && $pageRegion != null && $this->getDisplayOption("context_sensitive_region") == 'true' and (count($this->region_id) == 0 or (in_array($pageRegion->region_id, $this->region_id) and count($this->region_id) > 1)))
      {
         $n_query->region_id = $pageRegion->region_id;
         $lnk = $this->getFilterLink($obj_id, false, $n_query);
         $parts[] = "<a href='$lnk' rel='nofollow' class='$page_style'>$pageRegion->region_name</a>";
         $n_query->region_id = null;
      }
      if(isset($pageTopic) && $this->getDisplayOption("context_sensitive_topic") == 'true' and (count($this->topic_id) == 0 or (in_array($pageTopic->topic_id, $this->topic_id) and count($this->topic_id) > 1)))
      {
         $n_query->topic_id = $pageTopic->topic_id;
         $lnk = $this->getFilterLink($obj_id, false, $n_query);
         $parts[] = "<a href='$lnk' rel='nofollow' class='$page_style'>$pageTopic->topic_name</a>";
         $n_query->topic_id = null;
      }
      if(isset($pageType) && $this->getDisplayOption("context_sensitive_type") == 'true' and (count($this->type_id) == 0 or (in_array($pageType->type_id, $this->type_id) and count($this->type_id) > 1)))
      {
         $n_query->type_id = $pageType->type_id;
         $lnk = $this->getFilterLink($obj_id, false, $n_query);
         $parts[] = "<a href='$lnk' class='$page_style'>$pageType->type_name</a>";
         $n_query->language_id = null;
      }
      if(isset($pageLanguage) && $this->getDisplayOption("context_sensitive_language") == 'true' and (count($this->language_id) == 0 or (in_array($pageLanguage->language_id, $this->language_id) and count($this->language_id) > 1)))
      {
         $n_query->language_id = $pageLanguage->language_id;
         $lnk = $this->getFilterLink($obj_id, false, $n_query);
         $parts[] = "<a href='$lnk' class='$page_style'>".$pageLanguage->getLanguageCodePrefix()."</a>";
         $n_query->language_id = null;
      }
      if((isset($oscailt_page_setting['page_author']) or $this->query->author_name != null) && $this->getDisplayOption("context_sensitive_author") == 'true')
      {
         if(isset($oscailt_page_setting['page_author']))
         {
            $n_query->author_name = $oscailt_page_setting['page_author'];
         }
         $lnk = $this->getFullLink($obj_id, false, $n_query);
         $parts[] = "<a href='$lnk' rel='nofollow' class='$page_style'>".$oscailt_page_setting['page_author']."</a>";
      }
      return implode (" | ", $parts);
   }


   function getMediaTypeIconLink($media_type, &$media_type_query)
   {
      global $graphics_store;
      $href = $this->getFilterLink($this->site_settings['newswireobject'], false, $media_type_query);
      $html_link = "<A href='$href'>";
      $html_img = "<IMG alt='browse $media_type' src='$graphics_store"."$media_type"."icon.gif' title='$media_type' align=middle border=0>";
      return $html_link.$html_img."</A>\n";
   }


   function writeStoryHeadline(&$story, $num)
   {
      global $system_config, $page_style, $regionList, $topicList;
      $format_code = "P I E H D A O C";
      echo "<P class='$page_style'>";
      echo $this->getItemHighlights($story);
      $reg = $regionList->getRegionByID($story->region_id);
      $tpc = $topicList->getTopicByID($story->topic_id);
      // Bugfix Dec 2007. Sometimes reg and or tpc could be null. Generating error otherwise in those cases.
      // Probably caused by incorrect values passed through URI.
      if ($reg == null || $tpc == null ) {
          if ($reg == null || $tpc != null )
             $lnk_title = htmlspecialchars($tpc->getName() .": ". $story->story_title, ENT_QUOTES);
          elseif ($reg != null || $tpc == null ) 
             $lnk_title = htmlspecialchars($reg->getName() .": ". $story->story_title, ENT_QUOTES);
          else
             $lnk_title = htmlspecialchars($story->story_title, ENT_QUOTES);
      }
      else
          $lnk_title = htmlspecialchars($reg->getName() ." | ". $tpc->getName() .": ". $story->story_title, ENT_QUOTES);
      $image_written = false;
      $format_codes = explode(" ", $format_code);
      foreach($format_codes as $f_c)
      {
         if($f_c == 'P' && $this->getDisplayOption('includephoto') == 'true' && $story->image_amount > 0)
         {
            if($this->writeHeadlineThumbnail($story))
            {
               $image_written = true;
            }
         }
         elseif($f_c == 'I' && ($this->getDisplayOption('articleincludeicons') == 'true'))
         {
            $icon_tags = $story->getIconTags();
            foreach($icon_tags as $icon_tag)
            {
               echo($icon_tag);
            }
         }
         elseif($f_c == 'E' && $story->type_id == $system_config->event_type_id && $this->getDisplayOption('eventtimeformat') != '')
         {
            $format = $this->getDisplayOption('eventtimeformat');
            echo " <SPAN class='$page_style"."eventdate'>";
            echo strftime($format, $story->event_time+$system_config->timezone_offset);
            echo "</SPAN> ";
         }
         elseif($f_c == 'H')
         {
            $txt = $story->story_title;
            if($this->getDisplayOption('maxhlength') > 0 && (strlen($txt) > $this->getDisplayOption('maxhlength')))
            {
               $txt = substr($txt, 0, $this->getDisplayOption('maxhlength')) . "...";
            }
            echo "<A class='$page_style' title='$lnk_title' href='".$this->getStoryLink()."'>$txt</A> ";
         }
         elseif($f_c == 'D' && $this->getDisplayOption('articleincludedate') != '')
         {
            $format = $this->getDisplayOption('articleincludedate');
            echo "<SPAN class='$page_style"."date'>";
            echo strftime($format, $story->time_posted+$system_config->timezone_offset);
            echo "</SPAN>";
         }
         elseif($f_c == 'A' && $this->getDisplayOption('includeauthor') == 'true')
         {
            echo " <SPAN class='$page_style"."author'>by $story->author_name";
            echo "</SPAN>";
         }

         elseif($f_c == 'O' && $this->getDisplayOption('includeorganisation') == 'true')
         {
            echo " <SPAN class='$page_style"."organisation'>$story->author_organisation";
            echo "</SPAN>";
         }
         elseif($f_c == 'C' && $this->getDisplayOption('includecommentcount') == 'true')
         {
            echo " <SPAN class='$page_style"."comments'>$story->comments_amount comments";
            echo "</SPAN>";
         }
      }
      echo "</P>\n";
      $this->query->obj_id = $this->obj_id;

   }


}

function latestStoryCompare($a, $b)
{
   global $system_config, $is_lc;
   if($b->sticky_time<=0 && $a->sticky_time>0) return -1;
   if($a->sticky_time<=0 && $b->sticky_time>0) return 1;
   //return 0;
   if($a->type_id==$system_config->event_type_id && $is_lc)
   {
      if ($b->event_time == $a->event_time) return 0;
      return ($b->event_time < $a->event_time) ? 1 : -1;
   }
   else
   {
      if ($b->time_posted == $a->time_posted) return 0;
      return ($b->time_posted < $a->time_posted) ? -1 : 1;
   }
}

?>
