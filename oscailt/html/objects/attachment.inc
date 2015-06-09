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
include("config/attachments.php");

class Attachment
{
   var $attachment_id = NULL;
   var $story_id;
   var $comment_id;
   var $time_posted;
   var $attachment_file;
   var $image = NULL;
   var $video = NULL;
   var $audio = NULL;
   var $hidden;
   var $featurized;
   var $description;

   function Attachment()
   {
   }

   function addslashes()
   {
      $this->description = addslashes($this->description);
   }

   function stripslashes()
   {
      $this->description = stripslashes($this->description);
   }

   function save()
   {
      global $prefix, $dbconn;
      if($this->image==null) $this->image="NULL";
      if($this->video==null) $this->video="NULL";
      if($this->audio==null) $this->audio="NULL";
      if($this->attachment_id == NULL)
      {
         $this->story_id = cleanseNumericalQueryField($this->story_id);
         $this->comment_id = cleanseNumericalQueryField($this->comment_id);
         if($this->time_posted==null) $time_posted="NULL";
         else $time_posted="FROM_UNIXTIME(".$this->time_posted.")";
         checkForError(sql_query("INSERT INTO ".$prefix."_attachments values ('$this->attachment_id','$this->story_id', '$this->comment_id', $time_posted, '$this->attachment_file', '$this->description', $this->image, $this->video, $this->audio, '$this->hidden', '$this->featurized')", $dbconn,2));
         $this->attachment_id = mysql_insert_id($dbconn);
         // hack to set attachment type to unique value to allow count operation in story query
         $this->load();
         if((string)$this->image!=null) $this->image=$this->attachment_id;
         else $this->image="NULL";
         if((string)$this->video!=null) $this->video=$this->attachment_id;
         else $this->video="NULL";
         if((string)$this->audio!=null) $this->audio=$this->attachment_id;
         else $this->audio="NULL";
         $this->addslashes();
         checkForError(sql_query("UPDATE ".$prefix."_attachments SET story_id = '$this->story_id', comment_id = '$this->comment_id', time_posted = FROM_UNIXTIME($this->time_posted), attachment_file = '$this->attachment_file', description = '$this->description', image = $this->image, video = $this->video, audio = $this->audio, hidden = '$this->hidden', featurized = '$this->featurized' WHERE attachment_id = $this->attachment_id", $dbconn, 2));
         $this->stripslashes();
      }
      else
      {
         $this->attachment_id = cleanseNumericalQueryField($this->attachment_id);
         $this->story_id = cleanseNumericalQueryField($this->story_id);
         $this->comment_id = cleanseNumericalQueryField($this->comment_id);
         checkForError(sql_query("UPDATE ".$prefix."_attachments SET story_id = '$this->story_id', comment_id = '$this->comment_id', time_posted = FROM_UNIXTIME($this->time_posted), attachment_file = '$this->attachment_file', description = '$this->description', image = $this->image, video = $this->video, audio = $this->audio, hidden = '$this->hidden', featurized = '$this->featurized' WHERE attachment_id = $this->attachment_id", $dbconn,2));
      }
      if((string)$this->image=="NULL") $this->image=null;
      if((string)$this->video=="NULL") $this->video=null;
      if((string)$this->audio=="NULL") $this->audio=null;
   }

   // This function does the db update to swap the attachment Id of the target with the destination.
   // This will control the order of display of images in stories and comments. 
   function swap_order($swap_target, $swap_dest)
   {
      global $prefix, $dbconn;

      // The image field is set to the same value as the attachment_id and we sort on this, so just
      // swap them around.

      // Since we never store the values for image in the itemaction object, then we have to select them, 
      // because if they are already swapped, then assuming they are not swapped will not swap them.
      // These two queries are a bit messy, but it is not likely to be called often.
      // NOTE: We need to pick up the timestamps too.
      $image_swap_target =0;
      $image_swap_dest   =0;
      $target_timestamp =NULL;
      $dest_timestamp   =NULL;
      $result1 = sql_query("SELECT image,UNIX_TIMESTAMP(time_posted) FROM ".$prefix."_attachments WHERE attachment_id=".$swap_target, $dbconn,2);
      checkForError($result1);
      if(sql_num_rows( $result1 ) > 0) { list($image_swap_target, $target_timestamp) = sql_fetch_row($result1, $dbconn); }

      $result2=sql_query("SELECT image, UNIX_TIMESTAMP(time_posted) FROM ".$prefix."_attachments WHERE attachment_id=".$swap_dest, $dbconn,2);
      checkForError($result2);
      if(sql_num_rows( $result2 ) > 0) { list($image_swap_dest, $dest_timestamp) = sql_fetch_row($result2, $dbconn); }

      // echo("PreSelect: image_swap_targe " .$image_swap_target . " timestamp " .$target_timestamp . "<BR>");
      // echo("PreSelect: image_swap_targe " .$image_swap_dest   . " timestamp " .$dest_timestamp . "<BR>");

      if ($image_swap_dest > 0 && $image_swap_target > 0 ) {
          checkForError(sql_query("UPDATE ".$prefix."_attachments SET image=" .$image_swap_dest . ", time_posted=FROM_UNIXTIME(" .$target_timestamp . ") WHERE attachment_id=" .$swap_target, $dbconn, 2));
          checkForError(sql_query("UPDATE ".$prefix."_attachments SET image=" .$image_swap_target . ", time_posted=FROM_UNIXTIME(" .$dest_timestamp . ") WHERE attachment_id=" .$swap_dest, $dbconn, 2));
      } else {
          logMessage("Error retrieving swap values for " . $swap_target ." and ". $swap_dest . " for image during swap update. Aborted.", "sitelog.txt");
      }

   }

   function hide()
   {
      global $prefix, $dbconn;
      $this->hidden=1;
      $this->attachment_id = cleanseNumericalQueryField($this->attachment_id);
      checkForError(sql_query("UPDATE ".$prefix."_attachments SET hidden = '$this->hidden', time_posted = FROM_UNIXTIME($this->time_posted) WHERE attachment_id = $this->attachment_id", $dbconn,2));
   }

   function unhide()
   {
      global $prefix, $dbconn;
      $this->hidden=false;
      $this->attachment_id = cleanseNumericalQueryField($this->attachment_id);
      checkForError(sql_query("UPDATE ".$prefix."_attachments SET hidden = '$this->hidden', time_posted = FROM_UNIXTIME($this->time_posted) WHERE attachment_id = $this->attachment_id", $dbconn,2));
   }

   function featurize()
   {
      global $prefix, $dbconn;
      $this->featurized=1;
      $this->attachment_id = cleanseNumericalQueryField($this->attachment_id);
      checkForError(sql_query("UPDATE ".$prefix."_attachments SET featurized = '$this->featurized', time_posted = FROM_UNIXTIME($this->time_posted) WHERE attachment_id = $this->attachment_id", $dbconn,2));
   }

   function unfeaturize()
   {
      global $prefix, $dbconn;
      $this->featurized=false;
      $this->attachment_id = cleanseNumericalQueryField($this->attachment_id);
      checkForError(sql_query("UPDATE ".$prefix."_attachments SET featurized = '$this->featurized', time_posted = FROM_UNIXTIME($this->time_posted) WHERE attachment_id = $this->attachment_id", $dbconn,2));
   }

   function updateAttachmentFilename()
   {
      global $prefix, $dbconn;
      $this->attachment_id = cleanseNumericalQueryField($this->attachment_id);
      if ($this->attachment_id != null AND $this->attachment_id != 0) { 
          checkForError(sql_query("UPDATE ".$prefix."_attachments SET attachment_file = '$this->attachment_file', time_posted = time_posted WHERE attachment_id = $this->attachment_id", $dbconn,2));
      }
   }

   function load()
   {
      if($this->attachment_id != NULL)
      {
         global $prefix, $dbconn;
         $this->attachment_id = cleanseNumericalQueryField($this->attachment_id);
         $result = sql_query("SELECT story_id, comment_id, UNIX_TIMESTAMP(time_posted), attachment_file, description, image, video, audio, hidden, featurized FROM ".$prefix."_attachments WHERE attachment_id=$this->attachment_id", $dbconn,2);
         checkForError($result);
         if(sql_num_rows( $result ) > 0)
         {
            list($this->story_id, $this->comment_id, $this->time_posted, $this->attachment_file, $this->description, $this->image, $this->video, $this->audio, $this->hidden, $this->featurized) = sql_fetch_row($result, $dbconn);
            return true;
         }
         else
         {
            reportError("No Attachment with ID $this->attachment_id exists");
            return false;
         }
      }
      else
      {
         reportError("The attachment id is null");
	 return false;
      }
   }

   function loadByFileName()
   {
      if($this->attachment_file != NULL)
      {
         global $prefix, $dbconn;
         $result = sql_query("SELECT attachment_id, story_id, comment_id, UNIX_TIMESTAMP(time_posted), attachment_file, description, image, video, audio, hidden, featurized FROM ".$prefix."_attachments WHERE attachment_file=\"$this->attachment_file\"", $dbconn, 2);
         checkForError($result);
         if(sql_num_rows( $result ) > 0)
         {
            list($this->attachment_id, $this->story_id, $this->comment_id, $this->time_posted, $this->attachment_file, $this->description, $this->image, $this->video, $this->audio, $this->hidden, $this->featurized) = sql_fetch_row($result, $dbconn);
            return true;
         }
         else return false;
      }
      else return false;
   }

   function loadByURL($url)
   {
      // This function attempts to load the file
      // If its in the database it loads it.
      // If its not in db but file exists then it sets the attachment_file field
      // Otherwise all fields left as they are (null)
      global $system_config;
      $this->attachment_file=$url;
      // in case of absolute attachment store url
      $this->attachment_file=ltrim(str_replace($system_config->attachment_store_url,"",$this->attachment_file),"/");
      //echo("<BR>checking1:".$this->attachment_file."<BR>");
      if($this->loadByFileName()==true) return;
      else if($this->fileExists()) return;

      // in case of relative attachment store url
      $this->attachment_file=ltrim(str_replace("http://".$system_config->site_url."/","",$this->attachment_file),"/");
      //echo("<BR>checking1:".$this->attachment_file."<BR>");
      if($this->loadByFileName()==true) return;
      else if($this->fileExists()) return;

      // in case of relative attachment url
      $relativeAttachmentStoreUrl=ltrim(str_replace("http://".$system_config->site_url."/","",$system_config->attachment_store_url),"/");
      //echo("<BR>relativeAttachmentStoreUrl:".$relativeAttachmentStoreUrl."<BR>");
      $this->attachment_file=ltrim(str_replace($relativeAttachmentStoreUrl,"",$this->attachment_file),"/");
      //echo("<BR>checking3:".$this->attachment_file."<BR>");
      if($this->loadByFileName()==true) return;
      else if($this->fileExists()) return;
      $this->attachment_file=null;
   }

   function fileExists()
   {
      global $system_config;
      if(strlen(trim($this->attachment_file))==0) return false;
      return file_exists($system_config->attachment_store.$this->attachment_file);
   }

   function getFileSize($in_mbytes)
   {
      global $system_config;
      if( $this->fileExists() == false) return 0;

      $number_bytes = filesize($system_config->attachment_store.$this->attachment_file);
      if ($in_mbytes == true)
      {
          $number_bytes = round(100 * $number_bytes /1048576 ) / 100 ;
      }
      return $number_bytes;
   }

   function delete()
   {
      if($this->attachment_id != NULL)
      {
         global $prefix, $dbconn,$editor_session, $system_config;
         $this->attachment_id = cleanseNumericalQueryField($this->attachment_id);
         $result = sql_query("DELETE FROM ".$prefix."_attachments where attachment_id=$this->attachment_id", $dbconn, 2);
	 checkForError($result);
         if($editor_session->isSessionOpen()) $editor_session->clipboard->unclip($this);
         if($this->attachment_id==$system_config->featured_image)
         {
            $system_config->featured_image=null;
            $system_config->save();
         }
      }
      else
      {
         reportError("The attachment id is null");
      }
   }

   function deleteEmptyEmbeddedVideo()
   {
      if($this->story_id != NULL)
      {
         global $prefix, $dbconn;
         $this->story_id = cleanseNumericalQueryField($this->story_id);

	 $search_result = sql_query("SELECT attachment_id, story_id, comment_id,attachment_file FROM ".$prefix."_attachments WHERE story_id=$this->story_id AND comment_id=$this->comment_id AND substring(attachment_file, 1,12) = 'embedvideo:0' ", $dbconn, 2);
         checkForError($search_result);

         if(sql_num_rows( $search_result ) > 0)
         {
             $result = sql_query("DELETE FROM ".$prefix."_attachments where story_id=$this->story_id AND comment_id=$this->comment_id AND substring(attachment_file, 1,12) = 'embedvideo:0' ", $dbconn, 2);
	     checkForError($result);
	 }
      }
      else
      {
         reportError("The attachment story id is null");
      }
   }


   function deleteFile()
   {
      global $system_config;
      unlink($system_config->attachment_store.$this->attachment_file);
   }

   function deleteFileIfSingleReference()
   {
      // There are no actual files stored for either embedded video or audio.
      if($this->isEmbeddedVideo()) return;
      if($this->isEmbeddedAudio()) return;

      if($this->referenceCount()==1)
      {
         global $system_config;
         unlink($system_config->attachment_store.$this->attachment_file);
      }
   }

   function referenceCount()
   {
      global $prefix, $dbconn;
      $result = sql_query("SELECT COUNT(*) FROM ".$prefix."_attachments WHERE attachment_file='$this->attachment_file'", $dbconn, 2);
      checkForError($result);
      $reference_count=0;
      if(sql_num_rows( $result ) > 0)
      {
         list($reference_count) = sql_fetch_row($result, $dbconn);
      }
      return $reference_count;
   }

   function isImage()
   {
      if($this->image!=null && $this->image==true) return true;
      else return false;
   }

   function isVideo()
   {
      if($this->video!=null && $this->video==true) {
         // make sure embedvideo: or embedaudio: are not present
         if ((substr($this->attachment_file, 0,11) == "embedvideo:") || (substr($this->attachment_file, 0,11) == "embedaudio:")) return false;
         else return true;
      }
      else return false;
   }

   function isAudio()
   {
      if($this->audio==true) {
         // make sure embedvideo: or embedaudio: are not present
         if((substr($this->attachment_file, 0,11) == "embedvideo:") || (substr($this->attachment_file, 0,11) == "embedaudio:")) return false;
         else return true;
      }

      else return false;
   }

   function isEmbeddedVideo()
   {
      if($this->video==true) {
         // This special check is made because of the way the attachment_file is used for embedded video
         // to signify that it is an embedded video rather than a real video file.
         if(substr($this->attachment_file, 0,11) == "embedvideo:") return true;
         else return false;
      }
      else return false;
   }

   function isEmbeddedAudio()
   {
      if($this->audio==true) {
         if(substr($this->attachment_file, 0,11) == "embedaudio:") return true;
         else return false;
      }
      else return false;
   }


   function getIcon()
   {
      global $iconArray,$iconDescriptionArray;
      if($this->isImage()) return "imageicon.gif";
      else if($this->isAudio()) return "audioicon.gif";
      else if($this->isVideo()) return "videoicon.gif";
      else if($this->isEmbeddedVideo()) return "videoicon.gif";
      else if($this->isEmbeddedAudio()) return "audioicon.gif";
      else
      {
         $file_extension = strtolower(strrchr($this->attachment_file, "."));
         if($iconArray[$file_extension]!=null) return $iconArray[$file_extension];
         else return "miscicon.gif";
      }
   }

   function getIconTag($get_small=false)
   {
      global $path_prefix,$iconArray,$iconDescriptionArray, $graphics_store;
      if ($get_small == true) $audio_gif = "audioicon.gif";
      else $audio_gif = "audio_big.gif";

      if($this->isImage()) return "<IMG alt=\"image\" class='icon' src=\"".$graphics_store."imageicon.gif\" border=0>";
      else if($this->isAudio()) return "<IMG alt=\"audio\"  class='icon' src=\"".$graphics_store.$audio_gif."\" border=0>";
      else if($this->isVideo()) return "<IMG alt=\"video\"  class='icon' src=\"".$graphics_store."video_big.gif\" border=0>";
      else if($this->isEmbeddedVideo()) return "<IMG alt=\"video\"  class='icon' src=\"".$graphics_store."video_big.gif\" border=0>";
      else if($this->isEmbeddedAudio()) return "<IMG alt=\"audio\"  class='icon' src=\"".$graphics_store.$audio_gif."\" border=0>";
      else
      {
         $file_extension = strtolower(strrchr($this->attachment_file, "."));
	 if ($file_extension == ".pdf") {
             if ($get_small == true) $file_extension = ".bigpdf";
	 }
         if(isset($iconArray[$file_extension])) return "<IMG  class='icon' alt=\"".$iconDescriptionArray[$file_extension]."\" src=\"".$graphics_store.$iconArray[$file_extension]."\" border=0>";
         else return "<IMG  class='icon' alt=\"attachment\" src=\"".$graphics_store."miscicon.gif\" border=0>";
      }
   }

   function getURL()
   {
      global $system_config;
      return $system_config->attachment_store_url.$this->attachment_file;
   }

   function getTransformedImageURL($imageConfig, $show_error=false)
   {
      if($this->isImage())
      {
         global $system_config;
         $tool = new ImageTool();
         $tool->show_error = $show_error;
         if($this->fileExists())
         {
            $url_info = $tool->getTransformedImageURL($system_config->attachment_store.$this->attachment_file,$imageConfig);
         }
         else
         {
            //try to load it from remote...
            $url_info = $tool->getTransformedImageURL($system_config->attachment_store_url.$this->attachment_file,$imageConfig);
         }
         return $url_info;
      }
      else return null;
   }


   function getFilePath()
   {
      global $system_config;
      return $system_config->attachment_store.$this->attachment_file;
   }

   function getDisplayName()
   {
      if(strlen($this->description)>0)
         return htmlspecialchars($this->description,ENT_QUOTES);
      else
         $str = htmlspecialchars(substr(strrchr($this->attachment_file, "/"), 1),ENT_QUOTES);
      if(strlen($str) == 0)
         $str = $this->attachment_file;
      return $str;
   }


   function determineTypeFromFileName()
   {
      global $fileExtensions;
      $path_parts = pathinfo($this->attachment_file);
      $extension = $path_parts['extension'];
      $this->image=null;
      $this->video=null;
      $this->audio=null;
      if(stristr($fileExtensions['image'],$extension)!=false) $this->image=true;
      else if(stristr($fileExtensions['video'],$extension)!=false) $this->video=true;
      else if(stristr($fileExtensions['audio'],$extension)!=false) $this->audio=true;
   }

   function determineType($mime_string)
   {
      $this->image=null;
      $this->video=null;
      $this->audio=null;
      if(stristr($mime_string,"image")!=false) $this->image=true;
      else if(stristr($mime_string,"video")!=false) $this->video=true;
      else if(stristr($mime_string,"audio")!=false) $this->audio=true;
   }

   function isFileTypeAllowed()
   {
      global $fileExtensions;
      $path_parts = pathinfo($this->attachment_file);
      $extension = $path_parts['extension'];
      if(stristr($fileExtensions['image'],$extension)!=false) return true;
      else if(stristr($fileExtensions['video'],$extension)!=false) return true;
      else if(stristr($fileExtensions['audio'],$extension)!=false) return true;
      else if(stristr($fileExtensions['misc'],$extension)!=false) return true;
      else return false;
   }

   function createFromUpload($upload,$is_story_attachment, $directory_base = "")   {
      global $system_config,$editor_session;
      // In PHP earlier then 4.1.0, $HTTP_POST_FILES  should be used instead of $_FILES.
      if(is_uploaded_file($_FILES[$upload]['tmp_name']) && $_FILES[$upload]['error'] == UPLOAD_ERR_OK)
      {
         // Add code to check type via exif_imagetype or imagegetsize() but may not be able to do until moved
         $this->determineType($_FILES[$upload]['type']);
         // if not an editor with correct rights apply media type and size restrictions
         if($editor_session->isSessionOpen()==false  || $editor_session->editor->allowedWriteAccessTo("ignoreattachmentlimits")==false)
         {
            if($this->isImage())
            {
               if(($is_story_attachment && !$system_config->allow_story_image_attachments) || (!$is_story_attachment && !$system_config->allow_comment_image_attachments)) {
                  return "Sorry, but image uploads are not currently permitted : " .$_FILES[$upload]['name'];
               }
               if($_FILES[$upload]['size'] > $system_config->image_attachment_max_size) {
                  return "Filesize exceeds maximum image filesize : " .$_FILES[$upload]['name']. " Maximum: ".($system_config->image_attachment_max_size/1024)." Kb";
               }
               $size = getimagesize($_FILES[$upload]['tmp_name']);
               if($size==false)
               {
                  return "Image file is not valid image or may be corrupted : " .$_FILES[$upload]['name'];
               }
               else if($size[0] > $system_config->image_attachment_max_width)
               {
                  return "Image width exceeds maximum image width : " .$_FILES[$upload]['name']." Max width:  $system_config->image_attachment_max_width";
               }
            }
            else if($this->isVideo())
            {
               if(($is_story_attachment && !$system_config->allow_story_video_attachments) || (!$is_story_attachment && !$system_config->allow_comment_video_attachments)) {
                  return "Sorry, but video uploads are not currently permitted : " .$_FILES[$upload]['name'];
               }
               if($_FILES[$upload]['size'] > $system_config->video_attachment_max_size) {
                  return "Filesize exceeds maximum video filesize : " .$_FILES[$upload]['name'];
               }
            }
            else if($this->isAudio())
            {
               if(($is_story_attachment && !$system_config->allow_story_audio_attachments) || (!$is_story_attachment && !$system_config->allow_comment_audio_attachments)) {
                  return "Sorry, but audio uploads are not currently permitted : " .$_FILES[$upload]['name'];
               }
               if($_FILES[$upload]['size'] > $system_config->audio_attachment_max_size) {
                  return "Filesize exceeds maximum audio filesize : " .$_FILES[$upload]['name'];
               }
            }
            else
            {
               if(($is_story_attachment && !$system_config->allow_story_miscellaneous_attachments) || (!$is_story_attachment && !$system_config->allow_comment_miscellaneous_attachments)) {
                  return "Sorry, but uploads of this type are not currently permitted : " .$_FILES[$upload]['name'];
               }
               if($_FILES[$upload]['size'] > $system_config->miscellaneous_attachment_max_size) {
                  return "Filesize exceeds maximum filesize allowed : " .$_FILES[$upload]['name'];
               }
            }
         }
         // tidy up file name (convert to lower case, replace space with underscore);
         $this->attachment_file = ereg_replace("[^a-z0-9._]", "", str_replace(" ", "_", str_replace("%20", "_", strtolower(trim($_FILES[$upload]['name'])))));
         if($this->attachment_file == "")
         {
            return "Upload has invalid or empty filename: " .$_FILES[$upload]['name'] . " error : " .$_FILES[$upload]['error'] ;
         }
         if($this->isFileTypeAllowed() === false)
         {
            return "Sorry, but uploads with this file extension are not allowed : "
                  .$_FILES[$upload]['name'];
         }
         //get sub folder to place in
         //In Windows you may have to use getcwd() to add the drive letter.
         if($directory_base == "")
         {
            $subfolder =  strtolower(date("MY", mktime()))."/";
            if(!file_exists($system_config->attachment_store.$subfolder))
               mkdir($system_config->attachment_store.$subfolder,$system_config->default_writable_directory_permissions);
            $this->attachment_file= $subfolder.$this->attachment_file;
         }
         // check if file of same name already exists, if so add numerical prefix

         $file_extension = strrchr($this->attachment_file, ".");
         if($file_extension=="") $file_without_extension=$this->attachment_file;
         else $file_without_extension = substr($this->attachment_file, 0, strrpos ( $this->attachment_file, "."));
         $duplicate_prefix = "";
         $n = 1;
         if($directory_base == "")
         {
            $directory_base = $system_config->attachment_store;
         }

         while(file_exists($directory_base.$file_without_extension.$duplicate_prefix.$file_extension)) {
            $duplicate_prefix = "_".$n;
            $n++;
         }

         $this->attachment_file = $file_without_extension.$duplicate_prefix.$file_extension;
         // now move file to attachment store
         if(!move_uploaded_file($_FILES[$upload]['tmp_name'], $directory_base.$this->attachment_file)) {
            return "An error was encountered moving the uploaded file : " .$_FILES[$upload]['name'] . " to ".$directory_base.$this->attachment_file;
         }
         // Read and write for owner(webserver), read for everybody else
         chmod ($directory_base.$this->attachment_file, 0644);
         return "OK";
      }
      else
      {
         if ($system_config->user_error_reporting > 1 )
         	logMessage("Error code (". $_FILES[$upload]['error'].") uploading file: ".$_FILES[$upload]['tmp_name'], "sitelog.txt");

         switch ($_FILES[$upload]['error']) {
         case 1:
                $t_upload = $_SERVER['CONTENT_LENGTH'];
                $t_php_upload = ini_get('upload_max_filesize');
         	return "The uploaded file " .$_FILES[$upload]['name'] ." (".$t_upload." bytes) exceeds the upload_max_filesize (".$t_php_upload.") directive in php.ini.";
         	break;

         case 2:
         	return "The uploaded file " .$_FILES[$upload]['name'] ." exceeds the MAX_FILE_SIZE directive that was specified in the html form.";
         	break;

         case 3:
         	return "The uploaded file " .$_FILES[$upload]['name'] ." was only partially uploaded.";
         	break;

         case 4:
         	return "No file was uploaded for file: " .$_FILES[$upload]['name'];
         	break;

         case 6:
		// Introduced in PHP 4.3.10 and PHP 5.0.3
         	return "No file was uploaded for file: " .$_FILES[$upload]['name']. " because of a missing temporary directory.";
         	break;

         case 7:
		// Introduced in PHP 5.1.0
         	return "Failed to write file " .$_FILES[$upload]['name']. " to disk";
         	break;

         case 8:
		// Introduced in PHP 5.2.0
         	return "File upload stopped by extension for file: " .$_FILES[$upload]['name'];
         	break;

         default:
         	return "Upload has invalid or empty filename : " .$_FILES[$upload]['name'] . " or possibly filesize exceeds the post_max_size PHP directive. error: ". $_FILES[$upload]['error'] ." size: ".$_FILES[$upload]['size'] . " Request content size = ".$_SERVER['CONTENT_LENGTH'];
         }
      }
   }

   function moveToStory($story_id)
   {
      $this->story_id=$story_id;
      $this->comment_id=null;
      $this->addslashes();
      $this->save();
   }

   function copyToStory($story_id)
   {
      $this->attachment_id=null;
      $this->story_id=$story_id;
      $this->comment_id=null;
      $this->addslashes();
      $this->save();
   }

   function moveToComment($comment_id)
   {
      $comment = new Comment();
      $comment->comment_id=$comment_id;
      $comment->load();
      $this->comment_id=$comment->comment_id;
      $this->story_id=$comment->story_id;
      $this->addslashes();
      $this->save();
   }

   function copyToComment($comment_id)
   {
      $comment = new Comment();
      $comment->comment_id=$comment_id;
      $comment->load();
      $this->attachment_id=null;
      $this->comment_id=$comment->comment_id;
      $this->story_id=$comment->story_id;
      $this->addslashes();
      $this->save();
   }

   function dump($export_dir)
   {
      global $system_config;
      if(!is_dir($export_dir))
      {
          if(!mkdir($export_dir, $system_config->default_writable_directory_permissions))
          {
              $this->setUserMessage( array ( INDY_ERROR, "Failed to Make Directory $export_dir for export of story $this->attachment_id", "sf5"));
              return false;
          }
      }
      $file = $export_dir."/".$this->attachment_id.".dump";
      $fp = fopen($file,"w");
      if (flock($fp, LOCK_EX))
      {
         fputs($fp, serialize($this));
         flock($fp, LOCK_UN);
         fclose($fp);
      }
      else
      {
         reportError("<P>failed to get lock on export dump file $file</p>");
         return false;
      }
      return true;
   }


   function toString($show_parent_link)
   {
      global $system_config;
      $buffer = "--- Attachment ".$this->attachment_id." ---"."\r\n";
      if($show_parent_link==true && $this->story_id>0) $buffer = $buffer."Parent Story : "."http://".$system_config->site_url."/newswire.php?story_id=".$this->story_id."\r\n";
      if($show_parent_link==true && $this->comment_id>0)
      {
         $comment = new Comment();
         $comment->comment_id=$this->comment_id;
         $comment->load();
         $buffer = $buffer."Parent Comm. : "."http://".$system_config->site_url."/newswire.php?story_id=".$comment->story_id."#comment".$comment->comment_id."\r\n";
      }
      $buffer = $buffer."Time Posted   : ".date("l, M j Y, g:ia",$this->time_posted+$system_config->timezone_offset)."\r\n";
      if($this->isImage()) $buffer = $buffer."Media Type   : image"."\r\n";
      else if($this->isVideo()) $buffer = $buffer."Media Type   : video"."\r\n";
      else if($this->isAudio()) $buffer = $buffer."Media Type   : audio"."\r\n";
      else $buffer = $buffer."Media Type   : miscellaneous"."\r\n";
      if(stristr($this->getURL(),"http://")==false)
      {
         $buffer = $buffer."File         : "."http://".$system_config->site_url."/".$this->getURL()."\r\n";
      }
      else
      {
         $buffer = $buffer."File         : ".$this->getURL()."\r\n";
      }
      $buffer."Description  : ".$this->description."\r\n";
      return $buffer;
   }
}
?>
