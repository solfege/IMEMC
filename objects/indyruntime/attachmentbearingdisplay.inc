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
/* Abstract class for runtime objects with lists of attachments         */
/************************************************************************/

require_once('newswiredisplay.inc');

class attachmentBearingDisplay extends newswireDisplay
{
   function attachmentBearingDisplay($obj_id)
   {
      $this->newswireDisplay($obj_id);
   }

   function getTransformedImageTag(&$image, $cls)
   {
      global $pseudo_directory_offset, $system_config, $performance_test;
      $max_width = $this->getDisplayOption("maximgwidth");
      $max_height = $this->getDisplayOption("maximgheight");
      $max_size = $this->getDisplayOption("maximgsize");
      if($max_size == 0) $max_size = 10000000;
      $imageConfig = new ImageConfig($max_width, $max_height, false, false, $max_size,array(0xFF, 0xFF, 0xFF),0,0,0,0,0);
      $imageConfig->error_height = 120;
      $imageConfig->error_width = 120;
      $img_info = $image->getTransformedImageURL($imageConfig, true);
      if(!$img_info) return "";
      $img_url = $img_info[0];
      $full_file_url = $img_info[1];
      if(!isAbsoluteURL($img_url) && isRelativePath($img_url))
      {
          $img_url = $pseudo_directory_offset.$img_url;
      }
      if(!isAbsoluteURL($full_file_url) && isRelativePath($full_file_url))
      {
          $full_file_url = $pseudo_directory_offset.$full_file_url;
      }
      if(($full_file_url != $img_url) && $this->getDisplayOption('originallink') == 'true')
      {
         $a = "<a href='$full_file_url'>";
         $a_e = "</a>";
      }
      else
      {
         $a = "";
         $a_e = "";
      }
      $title = $system_config->full_photo_link_text;
      $tag = "$a<img $img_info[2] class='$cls' src='$img_url' alt='".$image->getDisplayName()."' title='$title'>$a_e";
      return $tag;
   }


   function writeAttachmentList(&$story,$feature_mode,$attachment_count_loaded=true)
   {
      if($attachment_count_loaded && $story->attachments_amount == 0) return;
      $image = $story->getImageAttachments();
      $video = $story->getVideoAttachments();
      $audio = $story->getAudioAttachments();
      $embed_video = $story->getEmbeddedVideoAttachments();
      $embed_audio = $story->getEmbeddedAudioAttachments();
      $miscellaneous = $story->getMiscellaneousAttachments();
      if($feature_mode==true && ( count($image)>0 || count($video)>0 || count($audio)>0 ||count($miscellaneous)>0)) echo("<P>");
      if($story->type_id==1) $this->writeAttachmentLinks($story,$feature_mode,$image,"image",0);
      else $this->writeAttachmentLinks($story,$feature_mode,$image,"image",1);
      if($feature_mode==false && ( count($video)>0 || count($audio)>0 ||count($miscellaneous)>0)) echo("<P>");

      $this->writeAttachmentLinks($story,$feature_mode,$video,"video",0);
      $this->writeAttachmentLinks($story,$feature_mode,$audio,"audio",0);
      $this->writeAttachmentLinks($story,$feature_mode,$miscellaneous,"misc",0);

      // Displaying embedded video and audio
      $this->writeAttachmentLinks($story,$feature_mode,$embed_video,"video",0);
      $this->writeAttachmentLinks($story,$feature_mode,$embed_audio,"audio",0);
   }

   function writeAttachmentLinks(&$item,$feature_mode,$attachments,$icon,$start)
   {
      global $editor_session, $pseudo_directory_offset;
      if(!isset($item->comment_id)) $content=$item->story_summary.$item->story_content.$item->story_content_extra;
      else $content=$item->comment_content;

      $show_video_as_icon = $this->getDisplayOption('video_mode');
      $Javascript_body = "";
      $id_array = array();
      for($i=0;$i<count($attachments);$i++)
      {
          $id_array[] = $attachments[$i]->attachment_id;
	  // Need to build up a list of strings for the Javascript function if the hide is in effect.
          if ($show_video_as_icon != 'image' ) {
              if($attachments[$i]->isEmbeddedVideo() == true) {
                  $videoType = substr($attachments[$i]->attachment_file,11,2);
                  $videoId = substr($attachments[$i]->attachment_file,14);
                  $t_id = $attachments[$i]->attachment_id;
	          $embed_str = getEmbeddedVideoHTML($videoType, $videoId);
                  $Javascript_body .= "if (sVideoBlockDiv==\"videoBlock_".$t_id."\") document.getElementById(sVideoBlockDiv).innerHTML='".$embed_str."';";
	      }
	  }
      }

      $collate_audio = false;
      $embed_audios = 0;
      if ($collate_audio == true ) {
          $audio_descriptions = array();
          for($i=$start;$i<count($attachments);$i++) {
              if (substr($attachments[$i]->attachment_file,0,11) == "embedaudio:") {
                  $embed_audios++;
                  $audio_descriptions[] = $attachments[$i]->description;
              }
          }
      }
      $total_embed_audios = $embed_audios;
      $embed_audios = 0;
      $wrote_javascript = false;

      for($i=$start;$i<count($attachments);$i++)
      {
         // For embedded video and audio we have to look at the attachment_file element because it is formatted
         $t_file = $attachments[$i]->attachment_file;
         if(substr($t_file,0,11) == "embedaudio:" || substr($t_file,0,11) == "embedvideo:") $t_file = substr($t_file,14);

         if(strstr($content,$t_file)==false)
         {
            if($attachments[$i]->isImage() && $feature_mode==false)
            {
               ?>
                  <p class="article-photo">
                  <a name="attachment<?=$attachments[$i]->attachment_id?>"></a>
                  <?php
                  echo $this->getTransformedImageTag($attachments[$i], 'standard-image');
                  $descr_str = $this->getItemHighlights($attachments[$i], true);
                  $descr_str .= $attachments[$i]->description;
                  if(strlen($descr_str)>0)
                  {
                     echo "<BR />$descr_str";
                  }
                  ?><BR><?
                  if($editor_session->canEditContent() ) {
                      global $system_config;
                      $http_mode = isset($_SERVER['HTTPS']) ? "https" : "http";
                      $t_image_mgr_url = '<a href="'.$http_mode.'://'.$system_config->site_url.'/editimage.php?subpage=edit&image='.$system_config->attachment_store . $attachments[$i]->attachment_file.'">Image Manager: Click for details on ' .$attachments[$i]->attachment_file.'</a>';
                      echo $t_image_mgr_url ;
                      ?><BR><?
                  }
                  $options = $this->getItemOptions($attachments[$i],"attachment",false, $i, $id_array);
                  if($options != "") echo $options;
                  ?>
                  </p>
               <?
            }
            else if($attachments[$i]->isEmbeddedVideo() == true)
            {
               ?>
                  <p class="article-photo">
                  <a name="attachment<?=$attachments[$i]->attachment_id?>"></a>
                  <?php
		  $videoType = substr($attachments[$i]->attachment_file,11,2);
                  $videoId = substr($attachments[$i]->attachment_file,14);

                  $descr_str = $this->getItemHighlights($attachments[$i], true);
                  $descr_str .= $attachments[$i]->description;

		  $show_video_as_icon = $this->getDisplayOption('video_mode');
		  if ($show_video_as_icon != 'image' ) {
		      // $embed_str = getEmbeddedVideoHTML($videoType, $videoId);
		      $provider = getEmbeddedVideoTypes(number_format($videoType));
                      $t_fn_id = "_".$attachments[$i]->story_id;
                      if ($attachments[$i]->comment_id != null) $t_fn_id .= "_".$attachments[$i]->comment_id;
		      if ($wrote_javascript == false ) {
		          ?><script type="text/javascript">
		            function loadvideoblock<?=$t_fn_id?>(sVideoBlockDiv) {
                               <?=$Javascript_body?>
                            }
		            </script>
		          <?
                          $wrote_javascript = true;
                      }
		      // style=width:352px;height:312px;background:pink;border:1px;
		      ?>
		         <div id="videoBlock_<?=$attachments[$i]->attachment_id?>" style="background:pink;margin: auto;border:1px solid;">
		         <br><p><i>To stop your IP being automatically logged by the provider of the (<?=$provider?>) video content, we have not loaded it automatically. If you wish to proceed to watch the video, then please <a href="#" onclick="loadvideoblock<?=$t_fn_id?>('videoBlock_<?=$attachments[$i]->attachment_id?>');">Click here to load the embedded video player for video Id <?=$videoId?></a><BR><BR>This setting can be controlled by your User Preference settings.</i></p>
		         </div>
		      <?
                      // Have to center here as previous paragraph tag cancels out the one for article-photo
                      if(strlen($descr_str)>0) {
                         echo "<p class='article-photo'>Embedded Video Description: $descr_str </p>";
                      }
                  } else {
		      echo getEmbeddedVideoHTML($videoType, $videoId);
                      if(strlen($descr_str)>0) {
                          echo "<BR />$descr_str";
                      }
                  }
                  ?><BR><?
		  // If it exists we are in publish so don't show options. Bit of a hack really.
                  if (!isset($_REQUEST['videoType'])) {
                     $options = $this->getItemOptions($attachments[$i],"attachment",false, $i, $id_array);
                     if($options != "") echo $options;
                  }
                  ?>
                  </p>
               <?
            }
            else if($attachments[$i]->isEmbeddedAudio() == true)
            {
               $embed_audios++;
               if($collate_audio == true && $total_embed_audios > 0 && $embed_audios < $total_embed_audios) {
                   continue;
               }
               ?>
                  <p class="article-photo">
                  <a name="attachment<?=$attachments[$i]->attachment_id?>"></a>
                  <?php
		  $audioType = substr($attachments[$i]->attachment_file,11,2);
                  $audioId = substr($attachments[$i]->attachment_file,14);

                  if($collate_audio == true && $total_embed_audios > 0 ) 
		      echo getEmbeddedAudioHTML(true, $audioId, "", $audio_descriptions);
                  else
		      echo getEmbeddedAudioHTML(true, $audioId, $attachments[$i]->description);

                  $descr_str = $this->getItemHighlights($attachments[$i], true);
                  $descr_str .= $attachments[$i]->description;
                  if(strlen($descr_str)>0)
                  {
                     echo "<BR />$descr_str";
                  }
                  ?><BR><?
		  // If it exists we are in publish so don't show options. Bit of a hack really.
                  if (!isset($_REQUEST['audioType'])) {
                     $options = $this->getItemOptions($attachments[$i],"attachment",false, $i, $id_array);
                     if($options != "") echo $options;
                  }
                  ?>
                  </p>
               <?
            }
            else
            {
                  echo $this->getItemHighlights($attachments[$i], true);
		  if ($attachments[$i]->isAudio() == true) {
                      global $system_config;
		      // And only if it is a MP3 file
                      if ($this->getDisplayOption('show_audio_player') == 'true' && $system_config->audio_player_installed == true) {
                         if (strtolower(substr($attachments[$i]->attachment_file,(strlen($attachments[$i]->attachment_file)-4))) == ".mp3") {
                            ?><BR><DIV align=center> <?
                            echo getEmbeddedAudioHTML(false, $attachments[$i]->attachment_file, $attachments[$i]->description);
                            ?> </div><BR> <?
                         }
                      }
                  }
                  echo $attachments[$i]->getIconTag();
                  $url = $attachments[$i]->getURL();
                  if(!isAbsoluteURL($url) && isRelativePath($url))
                  {
                     $url = $pseudo_directory_offset.$url;
                  }
                  ?>
                  <a name="attachment<?=$attachments[$i]->attachment_id?>" href="<?=$url?>"><?=$attachments[$i]->getDisplayName()?></a>
                  <?=$attachments[$i]->getFileSize(true)?> Mb 
                  <? echo $this->getItemOptions($attachments[$i],"attachment",false); ?>
                  <!-- <BR>-->
               <?
            }
         }
         else if($editor_session->canEditContent() && !isset($_REQUEST['print_page']))
         {
            $editor_note_text = $this->getDisplayOption('editor_note_text');
            $self_link_text = $this->getDisplayOption('self_link_text');
            if($self_link_text == "") $self_link_text = "SELF LINK TEXT UnDEFINED";
            if($editor_note_text == "") $editor_note_text = "Editor NOTE UnDEFINED";
            echo "<DIV class='attachment-note'>";
            echo "$editor_note_text $self_link_text - ";
            echo $attachments[$i]->getIconTag();
            echo " <a name='attachment".$attachments[$i]->attachment_id."' href='";
            $url = $attachments[$i]->getURL();
            if(!isAbsoluteURL($url) && isRelativePath($url))
            {
               echo $pseudo_directory_offset;
            }
            echo $url."'>";
            echo $attachments[$i]->getDisplayName();
            echo "</a> </DIV>";
            echo $this->getItemOptions($attachments[$i],"attachment",false);
         }
      }
   }

   function writeAttachmentWarnings(&$story)
   {
      global $editor_session, $oscailt_page_setting, $pseudo_directory_offset;
      $site_id = $oscailt_page_setting['site_id'];
      $cls_str = "class='attachment-warning'";
      if($editor_session->isSessionOpen())
      {
         $fix_text = $this->getDisplayOption('fix_text');
         $editor_note_text = $this->getDisplayOption('editor_note_text');

         $fix_copy_text = $this->getDisplayOption('fixandcopy_text');
         $external_image_text = $this->getDisplayOption('external_image_link_warning');
         $orig_ref_text = $this->getDisplayOption('orig_ref_text');
         $fix_copyandunhide_text = $this->getDisplayOption('fix_copyandunhide_text');
         $fix_copy_text = $this->getDisplayOption('fix_copy_text');
         $fix_create_text = $this->getDisplayOption('fix_create_text');
         $cant_fix_text = $this->getDisplayOption('cant_fix_text');
         $manual_fix_text = $this->getDisplayOption('manual_fix_text');
         $fix_explanation_text = $this->getDisplayOption('fix_explanation_text');
         $hidden_attachment_text = $this->getDisplayOption('hidden_attachment_text');
         $fix_unhide_text = $this->getDisplayOption('fix_unhide_text');
         $hidden_explanation_text = $this->getDisplayOption('hidden_explanation_text');
         $externalImages = $story->scanForExternalImages();
         if(count($externalImages)>0)
         {
            echo "<div $cls_str>";
            echo "<span class='error'>$external_image_text</span>";
            echo "<UL $cls_str>\n";
            foreach($externalImages as $externalImage)
            {
               $cached_version = getAttachmentFileFromCacheFile($externalImage);
               if($cached_version != false) $externalImage = $cached_version;
               echo "<LI $cls_str><A $cls_str href='$externalImage'>$externalImage</a>";
               $source = new Attachment();
               $source->loadByURL(str_replace($pseudo_directory_offset,"",$externalImage))   ;
               if($source->attachment_id!=null)
               {
                  $lnk = $this->getLinkToAttachment($source->story_id, $source->attachment_id);
                  echo " [ <a href='$lnk' $cls_str>$orig_ref_text</a> ]";
                  if($editor_session->editor->allowedReadAccessTo("copyattachment", $site_id) && $editor_session->editor->allowedReadAccessTo("hideattachment", $site_id))
                  {
                     if($source->hidden)
                     {
                        $lnk = $this->getLinkToAttachmentAction($source->story_id, $source->attachment_id, 'copyandunhideattachment');
                        echo " [ <a href='$lnk' $cls_str>$fix_text $fix_copyandunhide_text</a> ]";
                     }
                     else
                     {
                        $lnk = $this->getLinkToAttachmentAction($source->story_id, $source->attachment_id, 'copyattachment');
                        echo " [ <a href='$lnk' $cls_str>$fix_text $fix_copy_text</a> ]";
                     }
                  }
                  else
                  {
                     echo " [ $cant_fix_text ]";
                  }
               }
               else if($source->attachment_file!=null)
               {
                  if($editor_session->editor->allowedReadAccessTo("editattachment", $site_id) && $editor_session->editor->allowedReadAccessTo("hideattachment", $site_id))
                  {
                     $lnk = $this->getLinkToAttachmentAction($source->story_id, $source->attachment_file, 'createattachment');
                     echo " [ <a href='$lnk' $cls_str>$fix_text $fix_create_text</a> ]";
                  }
                  else
                  {
                     echo " [ $cant_fix_text ]";
                  }
               }
               else
               {
                  echo " [ $manual_fix_text ]";
               }
            }
            echo "</ul>\n";
            echo "$fix_explanation_text</div>\n";
         }
         $hiddenAttachments = $story->scanForLinksToHiddenAttachments();
         if(count($hiddenAttachments)>0)
         {
            echo "<DIV $cls_str>";
            echo "<span class='error'>$hidden_attachment_text</span>";
            echo "<UL $cls_str>\n";
            foreach($hiddenAttachments as $hiddenAttachment)
            {
               echo "<LI $cls_str><A $cls_str href='".$hiddenAttachment->getURL()."'>".$hiddenAttachment->getURL()."</a>";
               if($editor_session->editor->allowedReadAccessTo("unhideattachment", $site_id))
               {
                  $lnk = $this->getLinkToAttachmentAction("", $hiddenAttachment->attachment_id, 'unhideattachment');

                  echo " [ <a href='$lnk' $cls_str>$fix_text $fix_unhide_text</a> ]";
               }
               else
               {
                  echo " [ $cant_fix_text ]";
               }

            }
            echo "</ul>\n";
            echo "<p $cls_str>$hidden_explanation_text</p></div>\n";
         }
      }
   }

   function getLinkToAttachment($story_id, $attachment_id)
   {
      $lnk = $this->getSimpleLink();
      $lnk = addLinkParameters($lnk, array("story_id=$story_id"));
      $lnk .= '#attachment'.$attachment_id;
      return $lnk;
   }

   function getLinkToAttachmentAction($target_host, $target, $action)
   {
      $lnk = $this->getFullLink();
      $lnk = addLinkParameters($lnk, array("$action=$target"));
      return $lnk;
   }

}
?>
