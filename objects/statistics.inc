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

class Statistics{
   var $db_cache_number = 5;

   var $item_count;
   var $non_feature_item_count;
   var $story_count;
   var $feature_story_count;
   var $events_story_count;
   var $non_feature_story_count;
   var $comment_count;
   var $attachment_count;
   var $image_attachment_count;
   var $video_attachment_count;
   var $audio_attachment_count;
   var $embed_video_attachment_count;
   var $embed_audio_attachment_count;
   var $miscellaneous_attachment_count;
   var $hidden;
   var $time_posted_upper_limit;
   var $time_posted_lower_limit;

   var $item_hidden_count;
   var $story_hidden_count;
   var $feature_story_hidden_count;
   var $events_story_hidden_count;
   var $non_feature_story_hidden_count;
   var $comment_hidden_count;
   var $attachment_hidden_count;
   var $image_attachment_hidden_count;
   var $video_attachment_hidden_count;
   var $audio_attachment_hidden_count;
   var $embed_video_attachment_hidden_count;
   var $embed_audio_attachment_hidden_count;
   var $miscellaneous_attachment_hidden_count;

   var $topicList;
   var $story_by_category_count;

   // Not really appropriate in this class. It holds the header text for that would be display
   // in a table heading for the cell for the stats from this instance of the stats object.
   // But it is a handy place to store it.
   var $header_text =NULL;

   function reset()
   {
      $this->story_count=null;
      $this->comment_count=null;
      $this->attachment_count=null;
      $this->image_attachment_count=null;
      $this->video_attachment_count=null;
      $this->audio_attachment_count=null;
      $this->embed_video_attachment_count=null;
      $this->embed_audio_attachment_count=null;
      $this->miscellaneous_attachment_count=null;
      $this->hidden=null;
      $this->time_posted_upper_limit=null;
      $this->time_posted_lower_limit=null;

      $this->item_hidden_count=null;
      $this->story_hidden_count=null;
      $this->feature_story_hidden_count=null;
      $this->non_feature_story_hidden_count=null;
      $this->comment_hidden_count=null;
      $this->attachment_hidden_count=null;
      $this->image_attachment_hidden_count=null;
      $this->video_attachment_hidden_count=null;
      $this->audio_attachment_hidden_count=null;
      $this->embed_video_attachment_hidden_count=null;
      $this->embed_audio_attachment_hidden_count=null;
      $this->miscellaneous_attachment_hidden_count=null;
   }

   function setDbCache($db_cache_group)
   {
      // If it is a daily stat or one for this month, one should use db cache 2 as that regularly gets
      // deleted since stats in the current week or so will change. Otherwise, use 5 as that rarely gets deleted.
      $this->db_cache_number = $db_cache_group;
   }

   function setHeaderText($table_header_text)
   {
      $this->header_text = $table_header_text;
   }

   function getHeaderText()
   {
      return $this->header_text;
   }

   function getItemCount($count_hidden=false)
   {
      if ($count_hidden == false)
      {
          if($this->item_count==null)
          {
             $this->item_count=$this->getStoryCount()+$this->getCommentCount()+$this->getAttachmentCount();
          }
          return $this->item_count;
      }
      else
      {
          if($this->item_hidden_count==null)
          {
             $this->item_hidden_count=$this->getStoryCount(true)+$this->getCommentCount(true)+$this->getAttachmentCount(true);
          }
          return $this->item_hidden_count;
      }
   }

   function getNonFeatureItemCount()
   {
      if($this->non_feature_item_count==null)
      {
         $this->non_feature_item_count=$this->getNonFeatureStoryCount()+$this->getCommentCount()+$this->getAttachmentCount();
      }
      return $this->non_feature_item_count;
   }

   function getStoryCount($count_hidden=false)
   {
      if ($count_hidden == false)
      {
          if($this->story_count==null) $this->story_count=$this->getCount("stories",null);
          return $this->story_count;
      }
      else
      {
          $this->hidden=1;
          if($this->story_hidden_count==null) $this->story_hidden_count=$this->getCount("stories",null);
          $this->hidden=null;

          return $this->story_hidden_count;
      }
   }

   function getStoryByCategoryCount($count_hidden=false)
   {
      if ($this->topicList == null) {
          $topicListObj = new TopicList();
          $this->topicList = $topicListObj->getTopics();
      }
      if ($count_hidden == false)
      {
          $this->topicList = $topicListObj->getTopics();

          $this->story_by_category_count = array();
          foreach ($this->topicList as $topic ) {
             $topic_id = $topic->topic_id;
             $topic_name = $topic->topic_name;
             // echo("topic id " . $topic_id . " name " . $topic_name . "<BR>");

             $where_clauses = array();
             array_push($where_clauses, "topic_id=".$topic_id);
             $this->story_by_category_count[$topic_name] = $this->getCount("stories", $where_clauses);
          }
          return $this->story_by_category_count;
      }
      else
      {
          // This has to be updated for hidden story by category counts. No real need at moment.
          $this->hidden=1;
          if($this->story_hidden_count==null) $this->story_hidden_count=$this->getCount("stories",null);
          $this->hidden=null;

          return $this->story_hidden_count;
      }
   }


   function getFeatureStoryCount($count_hidden=false)
   {
      if ($count_hidden == false)
      {
         if($this->feature_story_count==null)
         {
            $this->feature_story_count=$this->getCount("stories",null);
            $where_clauses = array();
            array_push($where_clauses, "type_id=1");
            $this->feature_story_count=$this->getCount("stories",$where_clauses);
         }
         return $this->feature_story_count;
      }
      else
      {
         if($this->feature_story_hidden_count==null)
         {
            $this->feature_story_hidden_count=$this->getCount("stories",null);
            $this->hidden=1;
            $where_clauses = array();
            array_push($where_clauses, "type_id=1");
            $this->feature_story_hidden_count=$this->getCount("stories",$where_clauses);
            $this->hidden=NULL;
         }
         return $this->feature_story_hidden_count;
      }
   }
   function getEventsCount($count_hidden=false)
   {
      if ($count_hidden == false)
      {
         if($this->events_story_count==null)
         {
            $this->events_story_count=$this->getCount("stories",null);
            $where_clauses = array();
            array_push($where_clauses, "type_id=5");
            $this->events_story_count=$this->getCount("stories",$where_clauses);
         }
         return $this->events_story_count;
      }
      else
      {
         if($this->events_story_hidden_count==null)
         {
            $this->hidden=1;
            $this->events_story_hidden_count=$this->getCount("stories",null);
            $where_clauses = array();
            array_push($where_clauses, "type_id=5");
            $this->events_story_hidden_count=$this->getCount("stories",$where_clauses);
            $this->hidden=NULL;
         }
         return $this->events_story_hidden_count;
      }
   }


   function getNonFeatureStoryCount($count_hidden=false)
   {
      if ($count_hidden == false)
      {
         if($this->non_feature_story_count==null)
         {
            $this->non_feature_story_count=$this->getCount("stories",null);
            $where_clauses = array();
            array_push($where_clauses, "type_id!=1");
            $this->non_feature_story_count=$this->getCount("stories",$where_clauses);

         }
         return $this->non_feature_story_count;
      }
      else
      {
         if($this->non_feature_story_hidden_count==null)
         {
            $this->hidden=1;
            $this->non_feature_story_hidden_count=$this->getCount("stories",null);
            $where_clauses = array();
            array_push($where_clauses, "type_id!=1");
            $this->non_feature_story_hidden_count=$this->getCount("stories",$where_clauses);
            $this->hidden=null;

         }
         return $this->non_feature_story_hidden_count;
      }
   }

   function getCommentCount($count_hidden=false)
   {
      if ($count_hidden == false)
      {
          if($this->comment_count==null) $this->comment_count=$this->getCount("comments",null);
          return $this->comment_count;
      }
      else
      {
          $this->hidden=1;
          if($this->comment_hidden_count==null) $this->comment_hidden_count=$this->getCount("comments",null);
          $this->hidden=null;

          return $this->comment_hidden_count;
      }
   }

   function getAttachmentCount($count_hidden=false)
   {
      if ($count_hidden == false)
      {
         if($this->attachment_count==null) $this->attachment_count=$this->getCount("attachments",null);
         return $this->attachment_count;
      }
      else
      {
         $this->hidden=1;
         if($this->attachment_hidden_count==null) $this->attachment_hidden_count=$this->getCount("attachments",null);
         $this->hidden=NULL;
         return $this->attachment_hidden_count;
      }
   }

   function getImageAttachmentCount($count_hidden=false)
   {
      if ($count_hidden == false)
      {
         if($this->image_attachment_count==null)
         {
            $where_clauses = array();
            array_push($where_clauses, "image IS NOT NULL");
            $this->image_attachment_count=$this->getCount("attachments",$where_clauses);
         }
         return $this->image_attachment_count;
      }
      else
      {
         if($this->image_attachment_hidden_count==null)
         {
            $this->hidden=1;
            $where_clauses = array();
            array_push($where_clauses, "image IS NOT NULL");
            $this->image_attachment_hidden_count=$this->getCount("attachments",$where_clauses);
            $this->hidden=NULL;
         }
         return $this->image_attachment_hidden_count;
      }
   }

   function getVideoAttachmentCount($count_hidden=false)
   {
      if ($count_hidden == false)
      {
         if($this->video_attachment_count==null)
         {
            $where_clauses = array();
            array_push($where_clauses, "video IS NOT NULL AND SUBSTRING(attachment_file,1,11) !='embedvideo:'");
            $this->video_attachment_count=$this->getCount("attachments",$where_clauses);
         }
         return $this->video_attachment_count;
      }
      else
      {
         if($this->video_attachment_hidden_count==null)
         {
            $this->hidden=1;
            $where_clauses = array();
            array_push($where_clauses, "video IS NOT NULL AND SUBSTRING(attachment_file,1,11) !='embedvideo:'");
            $this->video_attachment_hidden_count=$this->getCount("attachments",$where_clauses);
            $this->hidden=NULL;
         }
         return $this->video_attachment_hidden_count;
      }
   }

   function getEmbeddedVideoAttachmentCount($count_hidden=false)
   {
      if ($count_hidden == false)
      {
         if($this->embed_video_attachment_count==null)
         {
            $where_clauses = array();
            array_push($where_clauses, "video IS NOT NULL AND SUBSTRING(attachment_file,1,11)='embedvideo:'");
            $this->embed_video_attachment_count=$this->getCount("attachments",$where_clauses);
         }
         return $this->embed_video_attachment_count;
      }
      else
      {
         if($this->embed_video_attachment_hidden_count==null)
         {
            $this->hidden=1;
            $where_clauses = array();
            array_push($where_clauses, "video IS NOT NULL AND SUBSTRING(attachment_file,1,11)='embedvideo:'");
            $this->embed_video_attachment_hidden_count=$this->getCount("attachments",$where_clauses);
            $this->hidden=NULL;
         }
         return $this->embed_video_attachment_hidden_count;
      }
   }

   function getAudioAttachmentCount($count_hidden=false)
   {
      if ($count_hidden == false)
      {
         if($this->audio_attachment_count==null)
         {
            $where_clauses = array();
            array_push($where_clauses, "audio IS NOT NULL AND SUBSTRING(attachment_file,1,11) !='embedaudio:'");
            $this->audio_attachment_count=$this->getCount("attachments",$where_clauses);
         }
         return $this->audio_attachment_count;
      }
      else
      {
         if($this->audio_attachment_hidden_count==null)
         {
            $this->hidden=1;
            $where_clauses = array();
            array_push($where_clauses, "audio IS NOT NULL AND SUBSTRING(attachment_file,1,11) !='embedaudio:'");
            $this->audio_attachment_hidden_count=$this->getCount("attachments",$where_clauses);
            $this->hidden=NULL;
         }
         return $this->audio_attachment_hidden_count;
      }
   }
   function getEmbeddedAudioAttachmentCount($count_hidden=false)
   {
      if ($count_hidden == false)
      {
         if($this->embed_audio_attachment_count==null)
         {
            $where_clauses = array();
            array_push($where_clauses, "audio IS NOT NULL AND SUBSTRING(attachment_file,1,11)='embedaudio:'");
            $this->embed_audio_attachment_count=$this->getCount("attachments",$where_clauses);
         }
         return $this->embed_audio_attachment_count;
      }
      else
      {
         if($this->embed_audio_attachment_hidden_count==null)
         {
            $this->hidden=1;
            $where_clauses = array();
            array_push($where_clauses, "audio IS NOT NULL AND SUBSTRING(attachment_file,1,11)='embedaudio:'");
            $this->embed_audio_attachment_hidden_count=$this->getCount("attachments",$where_clauses);
            $this->hidden=NULL;
         }
         return $this->embed_audio_attachment_hidden_count;
      }
   }


   function getMiscellaneousAttachmentCount($count_hidden=false)
   {
      if ($count_hidden == false)
      {
         if($this->miscellaneous_attachment_count==null)
         {
            $this->miscellaneous_attachment_count=$this->getAttachmentCount()-($this->getImageAttachmentCount()+$this->getVideoAttachmentCount()+$this->getAudioAttachmentCount()+$this->getEmbeddedVideoAttachmentCount()+$this->getEmbeddedAudioAttachmentCount());
         }
         return $this->miscellaneous_attachment_count;
      }
      else
      {
         if($this->miscellaneous_attachment_hidden_count==null)
         {
            $this->miscellaneous_attachment_hidden_count=$this->getAttachmentCount(true)-($this->getImageAttachmentCount(true)+$this->getVideoAttachmentCount(true)+$this->getAudioAttachmentCount(true)+$this->getEmbeddedVideoAttachmentCount(true)+$this->getEmbeddedAudioAttachmentCount(true));
         }
         return $this->miscellaneous_attachment_hidden_count;
      }
   }

   function getCount($table_name,$where_clauses)
   {
      global $prefix, $dbconn;
      $stmt = "SELECT COUNT(*) FROM ".$prefix."_".$table_name.$this->getWhereClause($where_clauses);
      # Really only do stats on stories, comments and attachments
      //echo("SQL: " . $stmt . "<br>");
      $result = sql_query($stmt, $dbconn, $this->db_cache_number);
      checkForError($result);
      $count_result=0;
      if(sql_num_rows( $result ) > 0)
      {
         list($count_result) = sql_fetch_row($result, $dbconn);
      }
      return $count_result;
   }

   function getWhereClause($where_clauses)
   {
      global $system_config;
      $stmt="";
      if($where_clauses==null) $where_clauses = array();
      if($this->time_posted_upper_limit != NULL) array_push($where_clauses, "UNIX_TIMESTAMP(time_posted) < ".($this->time_posted_upper_limit-$system_config->timezone_offset));
      if($this->time_posted_lower_limit != NULL) array_push($where_clauses, "UNIX_TIMESTAMP(time_posted) > ".($this->time_posted_lower_limit-$system_config->timezone_offset));
      if($this->hidden != NULL) array_push($where_clauses, "hidden = '$this->hidden'");
      if(count($where_clauses)>0) $stmt = $stmt." WHERE ";
      for($i=0;$i<count($where_clauses);$i++)
      {
         $stmt = $stmt.$where_clauses[$i];
         if($i<count($where_clauses)-1) $stmt = $stmt." AND ";
      }
      return $stmt;
   }

   function setTimePostedUpperLimit($time)
   {
      $this->time_posted_upper_limit = strtotime($time);
   }

   function setTimePostedLowerLimit($time)
   {
      $this->time_posted_lower_limit = strtotime($time);
   }
}
?>
