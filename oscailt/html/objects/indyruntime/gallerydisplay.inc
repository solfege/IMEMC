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
require_once('objects/magpie/image_fetch.inc');

class galleryDisplay extends newswireDisplay
{

   var $number_images_display = 0;
   var $number_images_preload = 0;

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

      $t_type = $this->getDisplayOption("show_type");

      if($t_type == 'audios') $this->query->media_type="AUDIO";
      else if($t_type == 'videos') $this->query->media_type="VIDEO";
      else if($t_type == 'files') $this->query->media_type="MISCELLANEOUS";
      else $this->query->media_type="IMAGE";

      $this->stories = $this->query->execute();
      //op_r($this->stories);
      $this->preloadImageTitles();
      $this->writeGallery();
   }

   // Handles audio automatically if the query was for audio
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
      if ($this->getDisplayOption("show_type") == 'audios') $this->query->media_type="AUDIO";
      else $this->query->media_type="IMAGE";
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


      // For banner image mode see if the preload buffer is set and if so read that many
      // extra images over and above the number that will be displayed.
      if ($this->getDisplayOption("show_type") != 'audios') {

         $this->number_images_display = $this->query->results_limit; 

	 if ($this->getDisplayOption("row_buffersize") > 0 )
         {
             $this->query->results_limit  = $this->query->results_limit+ $this->getDisplayOption("row_buffersize");
             $this->number_images_preload = $this->query->results_limit;
	 } 
      }

      $this->stories = $this->query->execute();

      if($performance_test > 6) markTime("SQL statement executed");
      $this->preloadImageTitles();
      if($performance_test > 6) markTime("Image titles pre-loaded");

      if ($this->getDisplayOption("show_type") == 'audios') $this->writeHeadlineGallery(true);
      else $this->writeHeadlineGallery();
   }


   function writeHeadlineJS($obj_id, $number_shown=4,$max_number=6)
   {
       ?>
         <script type='text/javascript' language='JavaScript'>
         var g_number_to_show=<?=$number_shown?>;
         var g_max_images =<?=$max_number?>;
         var g_obj_id =<?=$obj_id?>;
         var g_image_total =0;
         var g_active_start=0;
         var list_images = new Array();
         var list_links  = new Array();
         function image_preloader(i_src,i_title,i_alt, i_link) 
         { 
              if (g_image_total < g_max_images) {
                  list_images[g_image_total] = new Image(); 
                  list_images[g_image_total].src  =i_src;
                  list_images[g_image_total].title=i_title;
                  list_images[g_image_total].alt  =i_alt;
                  list_links[g_image_total]       =i_link;
                  g_image_total++;
              } 
         } 
         function shiftImages(direction)
         {
	      if (direction < 1) {
                  g_active_start--;
                  if (g_active_start < 0 ) g_active_start=g_image_total-1;
	      } else if (direction == 1) {
                  g_active_start++;
                  if ((g_active_start) > g_image_total) g_active_start=0;
	      }

              for(j=0; j < g_number_to_show ; j++) {
		 var i_name="gallery_hl_"+g_obj_id+"_"+(j+1);
		 var i_link="gallery_link_"+g_obj_id+"_"+(j+1);
		 var i_ref=g_active_start+j;
		 if (i_ref >= g_image_total) i_ref = i_ref - g_image_total;

                 document.images[i_name].src=list_images[i_ref].src;
                 document.images[i_name].alt=list_images[i_ref].alt;
                 document.images[i_name].title=list_images[i_ref].title;
                 document.links[i_link].href=list_links[i_ref];
              }
         }
 
         </script>
       <?
   }
   function writeHeadlineGallery($show_audio=false)
   {
      global $system_config, $editor_session, $page_style, $pseudo_directory_offset;

      $images = $this->stories;
      $rheight = $this->getDisplayOption("resizeheight");
      $rwidth = $this->getDisplayOption("resizewidth");
      $maxfsize = $this->getDisplayOption("maxfilesize");
      if($maxfsize == 0) $maxfsize =  1000;
      $image_htmls = array();
      $image_data  = array();

      // Update this now that the query is done with the actual number retrieved
      $this->number_images_preload = count($images);
      if(count($images) == 0)
      {
            $this->writeEmptyResultMessage('columnnotice');
      }
      else
      {
         if ($this->number_images_display > 0 && $this->number_images_preload > $this->number_images_display)
             $js_active = true; 
	 else
             $js_active = false;

         if ( $show_audio != true && $js_active == true ) 
             $this->writeHeadlineJS($this->obj_id, $this->number_images_display, $this->number_images_preload);

         echo "<table class='$page_style"."-gallery' >\n";
         echo "<tr class='$page_style"."-gallery'>";
         if ( $show_audio == true) {
            foreach($images as $each_audio)
            {
               $image_htmls[] = $this->getAudioHTML($each_audio, true);
            }
	 } else {
            $t_count=1;
            foreach($images as $image)
            {
               $imageConfig = new ImageConfig($rwidth,$rheight,true,true,$maxfsize,array(0xFF, 0xFF, 0xFF),0,0,0,0,0);
               $transformedImage_info=$image->getTransformedImageURL($imageConfig);
               if($transformedImage_info)
               {
                  $image_htmls[] = $this->getImageHTML($image, $transformedImage_info, $t_count);
	          if ( $show_audio == false && $js_active == true) 
                      $image_data[] = $this->getImageData($image, $transformedImage_info);
		  $t_count++;
               }
            }
         }
         $item_count = 1;

         $image_indexes= array_keys($image_htmls);

         $t_count=1;
	 if ( $show_audio == false && $js_active == true) 
	     echo '<td><a href="#" onclick="javascript:shiftImages(0);"><img src="'.$pseudo_directory_offset.'graphics/backward_btn.jpg" title="backward" alt="backward"></a></td>';

         foreach($image_indexes as $image_key)
         {
	    $t_count++;
	   
            if ($show_audio != true && $item_count > $this->number_images_display ) continue;
            $img = $image_htmls[$image_key];
            if($item_count > 1 && $this->getDisplayOption("display") == 'column') echo "</tr><tr>\n";

	    if ( $show_audio == true) {
		if ($item_count == 1) echo "<td class='$page_style"."-gallery'>";
		else echo "<br>";
	    }
	    else echo "<td class='$page_style"."-gallery'>";
            echo $img;
            if ( $show_audio != true) echo '</td>';
	    $item_count++;
         }
         if ( $show_audio == true) echo '</td>';
	 if ( $show_audio == false && $js_active == true) 
	     echo '<td><a href="#" onclick="javascript:shiftImages(1);"><img src="'.$pseudo_directory_offset.'graphics/forward_btn.jpg" title="forward" alt="forward"></a></td>';

         echo "</tr></table>";
	 if ( $show_audio == false && $js_active == true) {
             echo "<script type='text/javascript' language='JavaScript'>";
             foreach($image_data as $image_info_item)
             {
                 echo 'image_preloader("'.$image_info_item['src'].'","'.$image_info_item['title'].'","'.$image_info_item['alt'].'","'.$image_info_item['link'].'");';
                 echo "\n";
             }
             echo "</script>";
	 }
      }
   }

   function writeGallery()
   {
      global $system_config, $editor_session;
      $show_type = $this->getDisplayOption("show_type");

      if ($show_type == null OR $show_type == "") $show_type = "photos";

      $this->writeDisplayHeader($show_type);
      // echo "Photo Gallery | Audio Gallery | Video Gallery | File Gallery ";

      if($show_type == 'audios') $this->writeGalleryAudios();
      else if($show_type == 'videos') $this->writeGalleryVideos();
      else if($show_type == 'files') $this->writeGalleryMiscs();
      else $this->writeGalleryImages();

      $currentoffset=$this->query->results_offset;
      $this->writeDisplayFooter($show_type);
   }


   function getImageTitle(&$image)
   {
      if(strlen($image->description) > 0) $image_title=$image->getDisplayName();
      else
      {
         $story = $this->loadStory($image->story_id);
         $image_title=$story->story_title;
      }
      return $image_title;
   }

   function getImageHTML(&$image, $url_info, $link_counter=0)
   {
      global $pseudo_directory_offset;
      if(isRelativePath($url_info[0]))
         $url_info[0] = $pseudo_directory_offset.$url_info[0];
      $this->query->story_id=$image->story_id;
      $lnk = $this->getStoryLink();
      if($image->comment_id > 0)
      {
         $lnk = addLinkParameters($lnk, array("comment_limit=0", "condense_comments=false"));
      }
      $lnk .= '#attachment'.$image->attachment_id;

      if ($link_counter > 0 ) $lnk_html = "<a name='gallery_link_".$this->obj_id."_".$link_counter."' href='$lnk'>";
      else $lnk_html = "<a href='$lnk'>";

      $img_title = $this->getImageTitle($image);
      $img_disp = $image->getDisplayName();

      $t_name_ref="";
      if ($link_counter > 0 ) $t_name_ref="name='gallery_hl_".$this->obj_id."_".$link_counter."'";
      $img_html = "<img ".$t_name_ref." class='gallery-thumbnail' src='$url_info[0]' $url_info[2] title='$img_title' alt='$img_disp'>";

      $this->query->story_id=null;
      return $lnk_html.$img_html."</a>";
   }

   function getImageData(&$image, $url_info, $link_counter=null)
   {
      global $pseudo_directory_offset;
      if(isRelativePath($url_info[0]))
         $url_info[0] = $pseudo_directory_offset.$url_info[0];
      $this->query->story_id=$image->story_id;
      $lnk = $this->getStoryLink();
      if($image->comment_id > 0)
      {
         $lnk = addLinkParameters($lnk, array("comment_limit=0", "condense_comments=false"));
      }
      $lnk .= '#attachment'.$image->attachment_id;
      $image_data = array();
      $image_data['link'] = $lnk;

      $img_title = $this->getImageTitle($image);
      $img_disp = $image->getDisplayName();

      // $img_html = "<img class='gallery-thumbnail' src='$url_info[0]' $url_info[2] title='$img_title' alt='$img_disp'>";
      $image_data['src'] =  $url_info[0];
      $image_data['src2'] = $url_info[2];
      $image_data['title']= $img_title;
      $image_data['alt']  = $img_disp;

      $this->query->story_id=null;
      return $image_data;
   }

   function showImageManagerLinks($attachment_file, $rwidth, $rheight)
   {
       global $system_config;
       $http_mode = isset($_SERVER['HTTPS']) ? "https" : "http";
       $t_image_mgr_url = '<a href="'.$http_mode.'://'.$system_config->site_url.'/editimage.php?subpage=edit&image='.$system_config->attachment_store . $attachment_file.'&w='.$rwidth.'&h='.$rheight.'&obj_type=gallery">Image Manager</a>';
       echo "<BR><small>".$t_image_mgr_url."</small>" ;
   }

   function writeGalleryImages()
   {
      global $regionList,$topicList,$typeList,$system_config, $editor_session;
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
         echo '<table border=0 class="gallery" align="center">'."\n";
         for($i=0;$i<count($images);$i++)
         {
            $image=$images[$i];
            $imageConfig = new ImageConfig($rwidth,$rheight,true,true,$maxfsize,array(0xFF, 0xFF, 0xFF),0,0,0,0,0);
            $transformedImage_info=$image->getTransformedImageURL($imageConfig);
            if($transformedImage_info)
            {
               $t_width = $rwidth;
               $t_height= $rheight;
               if($transformedImage_info[2] == "") {
                   $transformedImage_info[2] = "width='$rwidth' height='$rheight'";
	       } else {
                   $t_width = $transformedImage_info[3];
                   $t_height = $transformedImage_info[4];
	       }
               if($row==0)
               {
                  echo'<tr class="gallery">';
               }
               echo '<td class="gallery">';
               echo $this->getImageHTML($image, $transformedImage_info);
	       if($editor_session->isSessionOpen()) {
                  $this->showImageManagerLinks($image->attachment_file, $t_width, $t_height);
	       }
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

   // function getAudioHTML(&$audio, $url_info)
   function getAudioHTML(&$audio, $truncate_name=false )
   {
      global $pseudo_directory_offset;

      $this->query->story_id=$audio->story_id;
      $item_lnk = $this->getStoryLink();
      if($audio->comment_id > 0)
      {
         $item_lnk = addLinkParameters($item_lnk, array("comment_limit=0", "condense_comments=false"));
      }
      $item_lnk .= '#attachment'.$audio->attachment_id;


      if ($audio->isEmbeddedAudio() ) $url = "http://" .substr($audio->attachment_file,14);
      else $url = $audio->getURL();

      if(!isAbsoluteURL($url) && isRelativePath($url))
      {
          $url = $pseudo_directory_offset.$url;
      }

      $audio_link = "<a href='$url'>".  $audio->getIconTag(true) . "</a> ";

      if ($truncate_name == true ) {
          $t_name = $audio->getDisplayName();
	  if (strlen($t_name) < 15) {
              $audio_link .= '<a name="attachment'.$audio->attachment_id.'" href="'.$item_lnk.'">'.$audio->getDisplayName().'</a> ';
	  } else {
              $t_name = substr($t_name,0,15) . '...';
              $audio_link .= $t_name . '</a>';
	  }
      } else {
          $audio_link .= '<a name="attachment'.$audio->attachment_id.'" href="'.$item_lnk.'">'.$audio->getDisplayName().'</a> ';
      }

      if (!$audio->isEmbeddedAudio() ) $audio_link .= $audio->getFileSize(true). " Mb";

      //$img_html = "<img class='gallery-thumbnail' src='$url_info[0]' $url_info[2] title='$img_title' alt='$img_disp'>";
      $this->query->story_id=null;
      return $audio_link;
   }


   function getVideoHTML(&$video, $show_listing)
   {
      global $pseudo_directory_offset, $oscailt_basic_config;

      $this->query->story_id=$video->story_id;
      $item_lnk = $this->getStoryLink();
      if($video->comment_id > 0)
      {
         $item_lnk = addLinkParameters($item_lnk, array("comment_limit=0", "condense_comments=false"));
      }
      $item_lnk .= '#attachment'.$video->attachment_id;


      if ($show_listing == false) {
          $videoType = substr($video->attachment_file,11,2);
          $video_id = substr($video->attachment_file,14);

          $video_link = $video->getIconTag(true)." <BR>"; 

          $safe_video_list = $oscailt_basic_config['video_cover_image_retrieval'];
          if (in_array($videoType, $safe_video_list)) {
               $ampersand_found = strpos($video_id,'&');
               if ($ampersand_found === false);
               else
               {
                   $video_id = substr($video_id,0,$ampersand_found);
               } 
		  
	       $default_img_url = getVideoCoverImage($videoType, $video_id );
	       if ($default_img_url != "") {
                   // We know cover images for other types come in all sizes so use width and height
                   // tags to get browser to display them same size as youtube ones.
                   if ($videoType != 1)
                       $video_link = '<img src="'.$default_img_url.'" width="120" height="90" ';
                   else
                       $video_link = '<img src="'.$default_img_url.'" ';
                   $video_link .= ' title="Video Image for Video Id '.$video_id.'"><BR />';
	       } else {
                   $video_link = 'Unable to retrieve cover image<BR>';
	       }
          } else {
              $video_link = getEmbeddedVideoTypes($videoType) . " Video <BR>";
              $video_link .= $video->getIconTag(true)." <BR>"; 
          }
          if (strlen($video->getDisplayName()) > 24 ) $video_link .= "<small>";
          $video_link .= '<a name="attachment'.$video->attachment_id.'" href="'.$item_lnk.'">'.$video->getDisplayName().'</a> ';
          if (strlen($video->getDisplayName()) > 24 ) $video_link .= "<small>";
       
      } else {
          /*
          $videoType = substr($video->attachment_file,11,2);
          $videoId = substr($video->attachment_file,14);
    
          if ($video->isEmbeddedVideo() ) $url = "http://" .substr($video->attachment_file,14);
          else $url = $video->getURL();
    
          if(!isAbsoluteURL($url) && isRelativePath($url))
          {
              $url = $pseudo_directory_offset.$url;
          }
          */
    
          //$video_link = "<a href='$url'>".  $video->getIconTag(true) . "</a> ";
          $video_link = $video->getIconTag(true)." ";
    
          $video_link .= '<a name="attachment'.$video->attachment_id.'" href="'.$item_lnk.'">'.$video->getDisplayName().'</a> ';
      }

      if (!$video->isEmbeddedVideo() ) $video_link .= $video->getFileSize(true). " Mb";

      //$img_html = "<img class='gallery-thumbnail' src='$url_info[0]' $url_info[2] title='$img_title' alt='$img_disp'>";
      $this->query->story_id=null;
      return $video_link;
   }


   function writeGalleryAudios()
   {
      global $regionList,$topicList,$typeList,$system_config;
      $show_color_bg = false;
      $show_listing  = true;
      // $show_listing  = false;

      $audios = $this->stories;
      if(count($audios) == 0)
      {
         $this->writeEmptyResultMessage();
      }
      else
      {
         $row=0;
         echo '<table class="gallery" align="center" border=0 width="90%">'."\n";
	 if($show_listing == true) {
             $http_mode = isset($_SERVER['HTTPS']) ? "https://" : "http://";
             $t_url = $http_mode . $system_config->site_url."/article/" ;
             echo '<tr><th align="left">Audio</th><th align="left">Description</th><th align="left">Story Link</th></tr>';
	 }

         for($i=0;$i<count($audios);$i++)
         {
            $audio = $audios[$i];
            {
               if($row==0 || $show_listing == true)
               {
                  echo'<tr class="gallery">';
               }
               echo '<td class="gallery">';
               if ($show_color_bg == true) {
                   if ($audio->isEmbeddedAudio() ) echo '<div style="background-color:#8080FF;">';
	           else echo '<div style="background-color:#80FF80;">';
	       }
               echo $this->getAudioHTML($audio, false);
               if ($show_color_bg == true) {
                   if ($audio->isEmbeddedAudio() ) echo '</div>';
	           else echo '</div>';
	       }
               echo '</td>';
	       if($show_listing == true) {
                   echo '<td align="left"> ';
                   echo $audio->description;
                   echo ' </td>';

                   $story = $this->loadStory($audio->story_id);

                   //$t_url = $http_mode . $system_config->site_url."/article/" . $audio->story_id;
                   $story_link = "<a href='".$t_url. $audio->story_id ."'>". $story->story_title . "</a>";

                   echo '<td align="left"> ';
                   echo $story_link. ' </td>';
	       }
               $row++;
               if($row==5 || $show_listing == true)
               {
                  echo "</tr>\n";
                  $row=0;
               }
            }
         }
         echo "</table>\n";
      }
   }

   function getMiscHTML(&$audio, $show_filename)
   {
      global $pseudo_directory_offset;

      $this->query->story_id=$audio->story_id;
      $item_lnk = $this->getStoryLink();
      if($audio->comment_id > 0)
      {
         $item_lnk = addLinkParameters($item_lnk, array("comment_limit=0", "condense_comments=false"));
      }
      $item_lnk .= '#attachment'.$audio->attachment_id;


      $url = $audio->getURL();

      if(!isAbsoluteURL($url) && isRelativePath($url))
      {
          $url = $pseudo_directory_offset.$url;
      }

      $audio_link = "<a href='$url'>".  $audio->getIconTag(true) . "</a> ";
      

      if ($show_filename == true)
          $audio_link .= '<a name="attachment'.$audio->attachment_id.'" href="'.$item_lnk.'">'.basename($audio->attachment_file).'</a> ';
      else
          $audio_link .= '<a name="attachment'.$audio->attachment_id.'" href="'.$item_lnk.'">'.$audio->getDisplayName().'</a> ';

      if (!$audio->isEmbeddedAudio() ) $audio_link .= $audio->getFileSize(true). " Mb";

      //$img_html = "<img class='gallery-thumbnail' src='$url_info[0]' $url_info[2] title='$img_title' alt='$img_disp'>";
      $this->query->story_id=null;
      return $audio_link;
   }

   function writeGalleryMiscs()
   {
      global $regionList,$topicList,$typeList,$system_config;
      $show_color_bg = false;
      $show_listing  = true;
      $show_filename  = true;
      $max_cells  = 3;  // Do not make bigger than 6
      $cellsizes  = array(1 => 100,2 => 50,3 => 33,4 => 25,5 =>20,6 =>15);
      // $show_listing  = false;

      $audios = $this->stories;
      if(count($audios) == 0)
      {
         $this->writeEmptyResultMessage();
      }
      else
      {
         $row=0;
         echo '<table class="gallery" align="center" border=0 width="90%">'."\n";
	 if($show_listing == true) {
             $http_mode = isset($_SERVER['HTTPS']) ? "https://" : "http://";
             $t_url = $http_mode . $system_config->site_url."/article/" ;
             echo '<tr><th align="left">File / Document </th><th align="left">Description</th><th align="left">Story Link</th></tr>';
	 }

         for($i=0;$i<count($audios);$i++)
         {
            $audio = $audios[$i];
            {
               if($row==0 || $show_listing == true)
               {
                  echo'<tr class="gallery">';
               }
               if($show_listing == true)
                   echo '<td class="gallery">';
	       else
	           echo '<td width="'.$cellsizes[$max_cells].'%" class="gallery">';

               if ($show_color_bg == true) {
	           echo '<div style="background-color:#80FF80;">';
	       }
               echo $this->getMiscHTML($audio, $show_filename);
               if ($show_color_bg == true) {
	           echo '</div>';
	       }
               echo '</td>';
	       if($show_listing == true) {
                   echo '<td align="left"> ';
                   echo $audio->description;
                   echo ' </td>';

                   $story = $this->loadStory($audio->story_id);

                   //$t_url = $http_mode . $system_config->site_url."/article/" . $audio->story_id;
                   $story_link = "<a href='".$t_url. $audio->story_id ."'>". $story->story_title . "</a>";

                   echo '<td align="left"> ';
                   echo $story_link. ' </td>';
	       }
               $row++;
               if($row==$max_cells || $show_listing == true)
               {
                  echo "</tr>\n";
                  $row=0;
               }
            }
         }
         echo "</table>\n";
      }
   }


   function writeGalleryVideos()
   {
      global $regionList,$topicList,$typeList,$system_config;
      $show_color_bg = false;
      $show_listing  = true;
      $show_listing  = false;
      $max_cells  = 3;  // Do not make bigger than 6
      $cellsizes  = array(1 => 100,2 => 50,3 => 33,4 => 25,5 =>20,6 =>15);
      // $show_listing  = false;

      $videos = $this->stories;
      if(count($videos) == 0)
      {
         $this->writeEmptyResultMessage();
      }
      else
      {
         $row=0;
         echo '<table class="gallery" align="center" border=0 width="90%">'."\n";
	 if($show_listing == true) {
             $http_mode = isset($_SERVER['HTTPS']) ? "https://" : "http://";
             $t_url = $http_mode . $system_config->site_url."/article/" ;
             echo '<tr><th align="left">Video</th><th align="left">Description</th><th align="left">Story Link</th></tr>';
	 }

         for($i=0;$i<count($videos);$i++)
         {
            $video = $videos[$i];
            {
               if($row==0 || $show_listing == true)
               {
                  if($show_listing == true)
                      echo'<tr class="gallery">';
		  else
                      echo'<tr align=center class="gallery">';
               }
               if($show_listing == true)
                   echo '<td class="gallery">';
	       else
	           echo '<td width="'.$cellsizes[$max_cells].'%" class="gallery">';

               if ($show_color_bg == true) {
                   if ($video->isEmbeddedVideo() ) echo '<div style="background-color:#8080FF;">';
	           else echo '<div style="background-color:#80FF80;">';
	       }
               echo $this->getVideoHTML($video, $show_listing);
               if ($show_color_bg == true) {
                   if ($video->isEmbeddedVideo() ) echo '</div>';
	           else echo '</div>';
	       }
               echo '</td>';
	       if($show_listing == true) {
                   echo '<td align="left"> ';
                   echo $video->description;
                   echo ' </td>';

                   $story = $this->loadStory($video->story_id);

                   //$t_url = $http_mode . $system_config->site_url."/article/" . $video->story_id;
                   $story_link = "<a href='".$t_url. $video->story_id ."'>". $story->story_title . "</a>";

                   echo '<td align="left"> ';
                   echo $story_link. ' </td>';
	       }
               $row++;
               if($row== $max_cells || $show_listing == true)
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
