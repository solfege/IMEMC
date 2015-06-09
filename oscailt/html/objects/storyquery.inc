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
    class StoryQuery
    {
        var $obj_id;//the data object that 'owns' this query

        // the specific story or comment to display
        var $story_id;
        var $comment_id;

        var $multi_filter; // Whether we allow multiple values in the filter fields

        var $topic_id;
        var $region_id;
        var $type_id;
        var $language_id;
        var $time_posted;
        var $related_link;
        var $author_name;
        var $author_organisation;
        var $author_email;
        var $author_phone;
        var $author_address;
        var $story_title;
        var $story_subtitle;
        var $story_summary;
        var $story_content;
        var $hidden=false;

        var $locked=false;
        var $event_time;
        var $sticky_time;
        var $ignore_hidden=false;
        var $search_text;
        var $search_comments=false;
        var $search_attachments=false;
        var $time_posted_upper_limit;
        var $time_posted_lower_limit;

        var $event_time_upper_limit;
        var $event_time_lower_limit;
        var $sticky_time_upper_limit;
        var $sticky_time_lower_limit;        // these settings indicate whether to search for exact matches or substrings
        var $related_link_exact = false;
        var $author_name_exact = false;
        var $author_organisation_exact = false;
        var $author_email_exact = false;
        var $author_phone_exact = false;
        var $author_address_exact = false;
        var $story_title_exact = false;
        var $story_subtitle_exact = false;
        var $story_summary_exact = false;
        var $story_content_exact = false;
        var $order_direction = "DESC";
        var $order_mode = "timeposted";
        var $media_type = "ALL";
        var $results_limit = 10;
        var $results_offset = 0;

        var $select_mode = "FULL";
        var $select_extent = "ALL";
        var $select_votes = false;
        var $text_where_mode="OR";
        var $custom_where_clauses = array();

        var $use_excludes=false;

        var $image_query=false;
        var $attachment_query=false;
        var $one_image_per_story=false;
        var $featurized_attachment_query=false;

        var $loaded_from_form = false;

        var $filetypes=array();

        function copyFilters(&$other)
        {
           $other->topic_id = isset($this->topic_id) ? $this->topic_id: null;
           $other->region_id = isset($this->region_id) ? $this->region_id: null;
           $other->type_id = isset($this->type_id) ? $this->type_id: null;
           $other->language_id = isset($this->language_id) ? $this->language_id: null;
           $other->author_name = isset($this->author_name) ? $this->author_name: null;
           return $other;
        }

        function copy($inc_filters=true)
        {
           $other = new StoryQuery();
           if($inc_filters) $other = $this->copyFilters($other);
           return $this->copyDetails($other);
        }

        function copyDetails(&$other)
        {
           $other->obj_id = isset($this->obj_id) ? $this->obj_id : null;
           $other->story_id = isset($this->story_id) ? $this->story_id : null;
           $other->comment_id = isset($this->comment_id) ? $this->comment_id : null;
           $other->multi_filter = isset($this->multi_filter) ? $this->multi_filter: null;
           $other->time_posted = isset($this->time_posted) ? $this->time_posted: null;
           $other->related_link = isset($this->related_link) ? $this->related_link: null;
           $other->author_organisation = isset($this->author_organisation) ? $this->author_organisation: null;
           $other->author_email = isset($this->author_email) ? $this->author_email: null;
           $other->author_phone = isset($this->author_phone) ? $this->author_phone: null;
           $other->author_address = isset($this->author_address) ? $this->author_address: null;
           $other->story_title = isset($this->story_title) ? $this->story_title: null;
           $other->story_subtitle = isset($this->story_subtitle) ? $this->story_subtitle: null;
           $other->story_summary = isset($this->story_summary) ? $this->story_summary: null;
           $other->story_content = isset($this->story_content) ? $this->story_content: null;
           $other->hidden = isset($this->hidden) ? $this->hidden: null;
           $other->event_time = isset($this->event_time) ? $this->event_time: null;
           $other->sticky_time = isset($this->sticky_time) ? $this->sticky_time: null;
           $other->ignore_hidden = isset($this->ignore_hidden) ? $this->ignore_hidden: null;
           $other->search_text = isset($this->search_text) ? $this->search_text: null;
           $other->search_comments = isset($this->search_comments) ? $this->search_comments: null;
           $other->search_attachments = isset($this->search_attachments) ? $this->search_attachments: null;
           $other->time_posted_upper_limit = isset($this->time_posted_upper_limit) ? $this->time_posted_upper_limit: null;
           $other->time_posted_lower_limit = isset($this->time_posted_lower_limit) ? $this->time_posted_lower_limit: null;
           $other->event_time_upper_limit = isset($this->event_time_upper_limit) ? $this->event_time_upper_limit: null;
           $other->event_time_lower_limit = isset($this->event_time_lower_limit) ? $this->event_time_lower_limit: null;
           $other->sticky_time_upper_limit = isset($this->sticky_time_upper_limit) ? $this->sticky_time_upper_limit: null;
           $other->sticky_time_lower_limit = isset($this->sticky_time_lower_limit) ? $this->sticky_time_lower_limit: null;
           $other->related_link_exact = isset($this->related_link_exact) ? $this->related_link_exact: null;
           $other->author_name_exact = isset($this->author_name_exact) ? $this->author_name_exact: null;
           $other->author_organisation_exact = isset($this->author_organisation_exact) ? $this->author_organisation_exact: null;
           $other->author_email_exact = isset($this->author_email_exact) ? $this->author_email_exact: null;
           $other->author_phone_exact = isset($this->author_phone_exact) ? $this->author_phone_exact: null;
           $other->author_address_exact = isset($this->author_address_exact) ? $this->author_address_exact: null;
           $other->story_title_exact = isset($this->story_title_exact) ? $this->story_title_exact: null;
           $other->story_subtitle_exact = isset($this->story_subtitle_exact) ? $this->story_subtitle_exact: null;
           $other->story_summary_exact = isset($this->story_summary_exact) ? $this->story_summary_exact: null;
           $other->story_content_exact = isset($this->story_content_exact) ? $this->story_content_exact: null;
           $other->order_direction = isset($this->order_direction) ? $this->order_direction: null;
           $other->order_mode = isset($this->order_mode) ? $this->order_mode: null;
           $other->media_type = isset($this->media_type) ? $this->media_type: null;
           $other->results_limit = isset($this->results_limit) ? $this->results_limit: null;
           $other->results_offset = isset($this->results_offset) ? $this->results_offset: null;
           $other->select_mode = isset($this->select_mode) ? $this->select_mode: null;
           $other->select_extent = isset($this->select_extent) ? $this->select_extent: null;
           $other->text_where_mode = isset($this->text_where_mode) ? $this->text_where_mode: null;

           $other->custom_where_clauses = isset($this->custom_where_clauses) ? $this->custom_where_clauses: array();
           $other->use_excludes = isset($this->use_excludes) ? $this->use_excludes: null;
           $other->image_query = isset($this->image_query) ? $this->image_query: null;
           $other->attachment_query = isset($this->attachment_query) ? $this->attachment_query: null;
           $other->featurized_attachment_query = isset($this->attachment_query) ? $this->featurized_attachment_query: null;
           $other->one_image_per_story = isset($this->one_image_per_story) ? $this->one_image_per_story: null;
           $other->loaded_from_form = isset($this->loaded_from_form) ? $this->loaded_from_form: null;
           $other->filetypes = isset($this->filetypes) ? $this->filetypes: array();
           return $other;
        }


        function addCustomWhereClause($clause)
        {
           array_push($this->custom_where_clauses, $clause);
        }


        function clearCustomWhereClauses()
        {
           $this->custom_where_clauses = array();
        }


        function selectFromAllTables()
        {
           $this->select_extent = "ALL";
        }


        function selectFromStoryTableOnly()
        {
           $this->select_extent = "STORY";
        }


        function selectTickerMode()
        {
           $this->select_mode = "TICKER";
        }

        function selectSummaryMode()
        {
           $this->select_mode = "SUMMARY";
        }

        function selectFullMode()
        {
           $this->select_mode = "FULL";
        }
	
        function selectVotes()
        {
           $this->select_votes = true;
        }


        function orderAscending()
        {
           $this->order_direction = "ASC";
        }

        function orderDescending()
        {
           $this->order_direction = "DESC";
        }
        function orderByTimePosted()
        {
           $this->order_mode = "timeposted";
        }

        function orderByTimeCommentsPosted()
        {
           $this->order_mode = "timecommentsposted";
        }

        function orderByEventTime()
        {
           $this->order_mode = "eventtime";
        }

        function selectAllMedia()
        {
           $this->media_type="ALL";
        }

        function selectTextMediaOnly()
        {
           $this->media_type="TEXT";
        }

        function selectImageMediaOnly()
        {
           $this->media_type="IMAGE";
        }

        function selectVideoMediaOnly()
        {
           $this->media_type="VIDEO";
        }

        function selectAudioMediaOnly()
        {
           $this->media_type="AUDIO";
        }

        function selectMiscellaneousMediaOnly()
        {
           $this->media_type="MISCELLANEOUS";
        }

        function addToWhereClause(&$where_clauses, $field_name, $field_value, $exact, $tokenize= false)
        {
           if($field_value != NULL)
           {
              if($exact == true) array_push($where_clauses, "$field_name = '$field_value'");
              else {
                  if($tokenize == true) $field_value = $this->tokenizeSearchString($field_value);

		  array_push($where_clauses, "$field_name LIKE '%$field_value%'");
              }
           }
        }

        function execute()
        {
           global $prefix, $dbconn, $system_config, $topicList, $regionList, $typeList, $editor_session, $languageList;
           $order_clauses = array();
           $where_clauses = array();
           $text_where_clauses = array();
           $comment_text_where_clauses = array();
           $having_clauses = array();
	   // Need a default language id for topics. Just use the lowest id since that will be main one.
	   $def_lang_id = $languageList->getMinLanguageId();

            // build select fields
            $select_fields = "";
            if($this->attachment_query==true)
            {
                $select_fields = "a.attachment_id, a.story_id, a.comment_id, UNIX_TIMESTAMP(a.time_posted), a.attachment_file, a.description, a.image, a.video, a.audio, a.hidden, a.featurized"; //COUNT(DISTINCT c.comment_id)";//last field not needed but for some reason it prevents use of temporary table
                //taken out as it was slowing down some queries.
            }
            else if($this->select_extent=="ALL")
            {
               if($this->select_mode=="TICKER") {
                   if($this->select_votes==false) $select_fields = "s.story_id, s.topic_id, s.region_id, s.type_id, s.language_id, UNIX_TIMESTAMP(s.time_posted), s.author_name, s.author_organisation, s.story_title, s.hidden, s.locked, UNIX_TIMESTAMP(s.event_time), UNIX_TIMESTAMP(s.sticky_time), COUNT(DISTINCT c.comment_id) as comments_amount, UNIX_TIMESTAMP(MAX(c.time_posted)) as time_last_comment_posted, COUNT(DISTINCT a.attachment_id) as attachments_amount, COUNT(DISTINCT a.image) as image_amount, COUNT(DISTINCT a.video) as video_amount, COUNT(DISTINCT a.audio) as audio_amount, s.story_subtitle";
		   else 
                       $select_fields = "s.story_id, s.topic_id, s.region_id, s.type_id, s.language_id, UNIX_TIMESTAMP(s.time_posted), s.author_name, s.author_organisation, s.story_title, s.hidden, s.locked, UNIX_TIMESTAMP(s.event_time), UNIX_TIMESTAMP(s.sticky_time), IFNULL(v.vote_value,0), COUNT(DISTINCT c.comment_id) as comments_amount, UNIX_TIMESTAMP(MAX(c.time_posted)) as time_last_comment_posted, COUNT(DISTINCT a.attachment_id) as attachments_amount, COUNT(DISTINCT a.image) as image_amount, COUNT(DISTINCT a.video) as video_amount, COUNT(DISTINCT a.audio) as audio_amount, s.story_subtitle";
               }
               else if($this->select_mode=="SUMMARY" && $this->select_votes == false) $select_fields = "s.story_id, s.topic_id, s.region_id, s.type_id, s.language_id, UNIX_TIMESTAMP(s.time_posted), s.related_link, s.author_name, s.author_organisation, s.author_email, s.author_phone, s.author_address, s.story_title, s.story_subtitle, s.story_summary, s.hidden, s.locked, UNIX_TIMESTAMP(s.event_time), UNIX_TIMESTAMP(s.sticky_time), COUNT(DISTINCT c.comment_id) as comments_amount, UNIX_TIMESTAMP(MAX(c.time_posted)) as time_last_comment_posted, COUNT(DISTINCT a.attachment_id) as attachments_amount, COUNT(DISTINCT a.image) as image_amount, COUNT(DISTINCT a.video) as video_amount, COUNT(DISTINCT a.audio) as audio_amount";
               else if($this->select_mode=="SUMMARY" && $this->select_votes == true) $select_fields = "s.story_id, s.topic_id, s.region_id, s.type_id, s.language_id, UNIX_TIMESTAMP(s.time_posted), s.related_link, s.author_name, s.author_organisation, s.author_email, s.author_phone, s.author_address, s.story_title, s.story_subtitle, s.story_summary, s.hidden, s.locked, UNIX_TIMESTAMP(s.event_time), UNIX_TIMESTAMP(s.sticky_time), IFNULL(v.vote_value,0), COUNT(DISTINCT c.comment_id) as comments_amount, UNIX_TIMESTAMP(MAX(c.time_posted)) as time_last_comment_posted, COUNT(DISTINCT a.attachment_id) as attachments_amount, COUNT(DISTINCT a.image) as image_amount, COUNT(DISTINCT a.video) as video_amount, COUNT(DISTINCT a.audio) as audio_amount";
               else if($this->select_mode=="FULL") $select_fields = "s.story_id, s.topic_id, s.region_id, s.type_id, s.language_id, UNIX_TIMESTAMP(s.time_posted), s.related_link, s.author_name, s.author_organisation, s.author_email, s.author_phone, s.author_address, s.story_title, s.story_subtitle, s.story_summary, s.story_content, s.hidden, s.locked, UNIX_TIMESTAMP(s.event_time), UNIX_TIMESTAMP(s.sticky_time), COUNT(DISTINCT c.comment_id) as comments_amount, UNIX_TIMESTAMP(MAX(c.time_posted)) as time_last_comment_posted, COUNT(DISTINCT a.attachment_id) as attachments_amount, COUNT(DISTINCT a.image) as image_amount, COUNT(DISTINCT a.video) as video_amount, COUNT(DISTINCT a.audio) as audio_amount";
            }
            else if($this->select_extent=="STORY")
            {
		if($this->select_mode=="TICKER") {
		    if($this->select_votes == true)
			    $select_fields = "s.story_id, s.topic_id, s.region_id, s.type_id, s.language_id, UNIX_TIMESTAMP(s.time_posted), s.author_name, s.author_organisation, s.story_title, s.hidden, s.locked, UNIX_TIMESTAMP(s.event_time), UNIX_TIMESTAMP(s.sticky_time), IFNULL(v.vote_value,0)";
		    else
			    $select_fields = "s.story_id, s.topic_id, s.region_id, s.type_id, s.language_id, UNIX_TIMESTAMP(s.time_posted), s.author_name, s.author_organisation, s.story_title, s.hidden, s.locked, UNIX_TIMESTAMP(s.event_time), UNIX_TIMESTAMP(s.sticky_time)";
		} else if($this->select_mode=="SUMMARY") $select_fields = "s.story_id, s.topic_id, s.region_id, s.type_id, s.language_id, UNIX_TIMESTAMP(s.time_posted), s.related_link, s.author_name, s.author_organisation, s.author_email, s.author_phone, s.author_address, s.story_title, s.story_subtitle, s.story_summary, s.hidden, s.locked, UNIX_TIMESTAMP(s.event_time), UNIX_TIMESTAMP(s.sticky_time)";
               else if($this->select_mode=="FULL") $select_fields = "s.story_id, s.topic_id, s.region_id, s.type_id, s.language_id, UNIX_TIMESTAMP(s.time_posted), s.related_link, s.author_name, s.author_organisation, s.author_email, s.author_phone, s.author_address, s.story_title, s.story_subtitle, s.story_summary, s.story_content, s.hidden, s.locked, UNIX_TIMESTAMP(s.event_time), UNIX_TIMESTAMP(s.sticky_time)";
            }
            //  build order clauses
            if($this->attachment_query==true)
            {
                array_push($order_clauses, "a.attachment_id ".$this->order_direction);
            }
            else if($this->order_mode=="timeposted")
            {
                array_push($order_clauses, "s.time_posted ".$this->order_direction);
            }
            else if($this->order_mode=="eventtime")
            {
                array_push($order_clauses, "s.event_time ".$this->order_direction);
            }
            else if($this->order_mode=="stickytime")
            {
                array_push($order_clauses, "s.sticky_time ".$this->order_direction);
            }
            else if($this->order_mode=="timecommentsposted")
            {
                array_push($order_clauses, "time_last_comment_posted ".$this->order_direction);
                if($this->select_extent=="ALL") array_push($where_clauses, "s.story_id=c.story_id");
            }
            else if($this->order_mode=="hiddenitems")
            {
                array_push($order_clauses, "s.time_posted ".$this->order_direction);
                $this->ignore_hidden=false;
                $this->hidden=true;
            }
            //  build where clauses
            if($this->story_id != NULL)  array_push($where_clauses, "s.story_id = $this->story_id");
            // if it is a multi-filter query, the _id fields contain arrays.
            if($this->multi_filter == 1)
            {
                //echo "<P>this is a multi-filter</P>";
                $sub_clauses = count($this->topic_id);
                //stuff here
                 if($sub_clauses > 0)
                 {
                    $topic_where_part = "(s.topic_id = ".$this->topic_id[0];
                    $i = 1;
                    while($i < $sub_clauses)
                    {
                        $topic_where_part .= " OR s.topic_id = ".$this->topic_id[$i];
                        $i++;
                    }
                    $topic_where_part .= ")";
                    array_push($where_clauses, $topic_where_part);
                 }
                 $sub_clauses = count($this->region_id);
                 if($sub_clauses > 0)
                 {
                     $region_where_part = "( s.region_id = ".$this->region_id[0];
                     $i = 1;
                     while($i < $sub_clauses)
                     {
                         $region_where_part .= " OR s.region_id = ".$this->region_id[$i];
                         $i++;
                     }
                     $region_where_part .= ")";
                     array_push($where_clauses, $region_where_part);
                 }
                 $sub_clauses = count($this->language_id);
                 if($sub_clauses > 0)
                 {
                     //print_r($this->language_id);
                     $language_where_part = "( s.language_id = ".$this->language_id[0];
                     $i = 1;
                     while($i < $sub_clauses)
                     {
                         $language_where_part .= " OR s.language_id = ".$this->language_id[$i];
                         $i++;
                     }
                     $language_where_part .= ")";
                     array_push($where_clauses, $language_where_part);
                 }
                 $sub_clauses = count($this->type_id);
                 if($sub_clauses > 0)
                 {
                     $type_where_part = "(s.type_id = ".$this->type_id[0];
                     $i = 1;
                     while($i < $sub_clauses)
                     {
                         $type_where_part .= " OR s.type_id = ".$this->type_id[$i];
                         $i++;
                     }
                     $type_where_part .= ")";
                     array_push($where_clauses, $type_where_part);
                 }
                 if($this->use_excludes && !($editor_session->isSessionOpen() and $editor_session->editor->allowedReadAccessTo("viewprotectedcategories")))
                 {
                    $excludedTopics=$topicList->getExcludedTopics($def_lang_id);
                    for ($i=0; $i<count($excludedTopics); $i++)
                    {
                       array_push($where_clauses, "s.topic_id != ".$excludedTopics[$i]->topic_id);
                    }

                    $excludedRegions=$regionList->getExcludedRegions($def_lang_id);
                    for ($i=0; $i<count($excludedRegions); $i++)
                    {
                        array_push($where_clauses, "s.region_id != ".$excludedRegions[$i]->region_id);
                    }
                    $excludedTypes=$typeList->getExcludedTypes($def_lang_id);
                    for ($i=0; $i<count($excludedTypes); $i++)
                    {
                        array_push($where_clauses, "s.type_id != ".$excludedTypes[$i]->type_id);
                    }
                    $excludedLanguages=$languageList->getExcludedLanguages();
                    for ($i=0; $i<count($excludedLanguages); $i++)
                    {
                        array_push($where_clauses, "s.language_id != ".$excludedLanguages[$i]->language_id);
                    }
                }
            }
            else //not a multifilter
            {
                if($this->topic_id > 0)
                {
                   array_push($where_clauses, "s.topic_id = $this->topic_id");
                }
                else if($this->topic_id < 0)
                   array_push($where_clauses, "s.topic_id != ".($this->topic_id-($this->topic_id*2)));
                else if($this->use_excludes)
                {
                    $excludedTopics=$topicList->getExcludedTopics($def_lang_id);
                    for ($i=0; $i<count($excludedTopics); $i++)
                    {
                        array_push($where_clauses, "s.topic_id != ".$excludedTopics[$i]->topic_id);
                    }
                }
                if($this->region_id > 0)
                   array_push($where_clauses, "s.region_id = $this->region_id");
                else if($this->region_id < 0)
                   array_push($where_clauses, "s.region_id != ".($this->region_id-($this->region_id*2)));
                else if($this->use_excludes)
                {
                    $excludedRegions=$regionList->getExcludedRegions($def_lang_id);
                    for ($i=0; $i<count($excludedRegions); $i++)
                    {
                       array_push($where_clauses, "s.region_id != ".$excludedRegions[$i]->region_id);
                    }
                }

                if($this->type_id > 0)
                   array_push($where_clauses, "s.type_id = $this->type_id");
                else if($this->type_id < 0)
                   array_push($where_clauses, "s.type_id != ".($this->type_id-($this->type_id*2)));
                else if($this->use_excludes)
                {
                    $excludedTypes=$typeList->getExcludedTypes($def_lang_id);
                    for ($i=0; $i<count($excludedTypes); $i++)
                    {
                        array_push($where_clauses, "s.type_id != ".$excludedTypes[$i]->type_id);
                   }
                }
                if($this->language_id > 0)
                   array_push($where_clauses, "s.language_id = $this->language_id");
                else if($this->language_id < 0)
                   array_push($where_clauses, "s.language_id != ".($this->language_id-($this->language_id*2)));
                else if($this->use_excludes)
                {
                    $excludedLanguages=$languageList->getExcludedLanguages();
                    for ($i=0; $i<count($excludedLanguages); $i++)
                    {
                        array_push($where_clauses, "s.language_id != ".$excludedLanguages[$i]->language_id);
                    }
                }
            }
            if($this->time_posted != NULL)
               array_push($where_clauses, "s.time_posted = $this->time_posted");
            if($this->event_time != NULL)
               array_push($where_clauses, "s.event_time = $this->event_time");
            if($this-> sticky_time != NULL)
               array_push($where_clauses, "s.sticky_time = $this->sticky_time");
            if($this->attachment_query==true)
            {
               if(count($this->filetypes)>0)
               {
                  $filetype_clause="";
                  foreach($this->filetypes as $filetype)
                  {
                     if(strlen($filetype_clause)>0)
                        $filetype_clause=$filetype_clause." OR ";
                     $filetype_clause=$filetype_clause."a.attachment_file LIKE '%.$filetype%'";
                  }
                  array_push($where_clauses, "(".$filetype_clause.")");
               }
               if($this->featurized_attachment_query == true)
               {
                  array_push($where_clauses, "a.featurized>0");
                  if(!strcasecmp($this->media_type,"AUDIO")) array_push($where_clauses, "a.audio>0");
		  else array_push($where_clauses, "a.image>0");
               }
               elseif(!strcasecmp($this->media_type,"TEXT")) array_push($where_clauses, "a.attachment_id IS NULL");
               else if(!strcasecmp($this->media_type,"IMAGE")) array_push($where_clauses, "a.image>0");
               else if(!strcasecmp($this->media_type,"VIDEO")) array_push($where_clauses, "a.video>0");
               else if(!strcasecmp($this->media_type,"AUDIO")) array_push($where_clauses, "a.audio>0");
               else if(!strcasecmp($this->media_type,"MISCELLANEOUS"))
               {
                  array_push($where_clauses, "a.image IS NULL");
                  array_push($where_clauses, "a.video IS NULL");
                  array_push($where_clauses, "a.audio IS NULL");
               }
            }
            else if($this->select_extent=="ALL")
            {
               if(count($this->filetypes)>0)
               {
                  $filetype_clause="";
                  foreach($this->filetypes as $filetype)
                  {
                     if(strlen($filetype_clause)>0)
                        $filetype_clause=$filetype_clause." OR ";
                     $filetype_clause=$filetype_clause."s.story_summary LIKE '%.$filetype%' OR ";
                     $filetype_clause=$filetype_clause."s.story_content LIKE '%.$filetype%' OR ";
                     $filetype_clause=$filetype_clause."s.related_link LIKE '%.$filetype%' OR ";
                     $filetype_clause=$filetype_clause."c.comment_content LIKE '%.$filetype%'";
                  }
                  array_push($where_clauses, "(".$filetype_clause.")");
               }
               if(!strcasecmp($this->media_type,"TEXT"))
                  array_push($where_clauses, "a.attachment_id IS NULL");
               else if(!strcasecmp($this->media_type,"IMAGE"))
                  array_push($having_clauses, "image_amount > 0");
               else if(!strcasecmp($this->media_type,"VIDEO"))
               {
                  array_push($having_clauses, "video_amount > 0");
                  //optimisation based on assumption that video is rare
                  array_push($where_clauses, "a.video > 0");
               }
               else if(!strcasecmp($this->media_type,"AUDIO"))
               {
                  array_push($having_clauses, "audio_amount > 0");
                  //optimisation based on assumption that audio is rare
                  array_push($where_clauses, "a.audio > 0");
               }
               else if(!strcasecmp($this->media_type,"MISCELLANEOUS"))
               {
                  array_push($having_clauses, "attachments_amount > (image_amount+video_amount+audio_amount)");
               }
            }
            if($this->time_posted_upper_limit != NULL)
               //array_push($where_clauses, "UNIX_TIMESTAMP(s.time_posted) < ".($this->time_posted_upper_limit-$system_config->timezone_offset));
               array_push($where_clauses, "s.time_posted < FROM_UNIXTIME(".($this->time_posted_upper_limit-$system_config->timezone_offset).")");
            if($this->time_posted_lower_limit != NULL)
               //array_push($where_clauses, "UNIX_TIMESTAMP(s.time_posted) > ".($this->time_posted_lower_limit-$system_config->timezone_offset));
               array_push($where_clauses, "s.time_posted > FROM_UNIXTIME(".($this->time_posted_lower_limit-$system_config->timezone_offset).")");
            if($this->type_id==$system_config->event_type_id or (is_array($this->type_id) && in_array($system_config->event_type_id, $this->type_id)))
            {
               if($this->event_time_upper_limit != NULL)
                  array_push($where_clauses, "UNIX_TIMESTAMP(s.event_time) < ".($this->event_time_upper_limit-$system_config->timezone_offset));
               if($this->event_time_lower_limit != NULL)
                  array_push($where_clauses, "UNIX_TIMESTAMP(s.event_time) > ".($this->event_time_lower_limit-$system_config->timezone_offset));
               if($this->type_id == $system_config->event_type_id or count($this->type_id) == 1 && $this->type_id[0] = $system_config->event_type_id)
               {
                  array_unshift($where_clauses, "s.event_time > 0");
               }
            }
            if($this->sticky_time_upper_limit != NULL)
            {
               array_unshift($where_clauses, "s.sticky_time > 0");
               array_push($where_clauses, "UNIX_TIMESTAMP(s.sticky_time) < ".($this->sticky_time_upper_limit-$system_config->timezone_offset));
            }
            if($this->sticky_time_lower_limit != NULL)
            {
               array_unshift($where_clauses, "s.sticky_time > 0");
               array_push($where_clauses, "UNIX_TIMESTAMP(s.sticky_time) > ".($this->sticky_time_lower_limit-$system_config->timezone_offset));
            }
            if($this->search_text != NULL)
            {
               $token_srch_string = $this->tokenizeSearchString($this->search_text);

               if (isset($_REQUEST['related_link']) && strlen($_REQUEST['related_link']) > 0 )
                   array_push($text_where_clauses, "s.related_link LIKE '%$this->search_text%'");

               array_push($text_where_clauses, "s.author_name LIKE '%$this->search_text%'");
               array_push($text_where_clauses, "s.author_organisation LIKE '%$this->search_text%'");

               if (isset($_REQUEST['author_email']) && strlen($_REQUEST['author_email']) > 0 )
                   array_push($text_where_clauses, "s.author_email LIKE '%$this->search_text%'");

               if (isset($_REQUEST['author_phone']) && strlen($_REQUEST['author_phone']) > 0 )
                   array_push($text_where_clauses, "s.author_phone LIKE '%$this->search_text%'");

               if (isset($_REQUEST['author_address']) && strlen($_REQUEST['author_address']) > 0 )
	       	   array_push($text_where_clauses, "s.author_address LIKE '%$this->search_text%'");

               //array_push($text_where_clauses, "s.story_title LIKE '%$this->search_text%'");
               array_push($text_where_clauses, "s.story_title LIKE '%".$token_srch_string."%'");
               array_push($text_where_clauses, "s.story_subtitle LIKE '%$token_srch_string%'");
               array_push($text_where_clauses, "s.story_summary LIKE '%$token_srch_string%'");
	       // For this field use RLIKE ANDed against the original LIKE clause
               $rlike_token_srch_string = $this->tokenizeSearchString($this->search_text, false, "story_content");
               array_push($text_where_clauses, "(s.story_content LIKE '%$token_srch_string%' ". $rlike_token_srch_string.")");
               if($this->select_extent=="ALL")
               {
                  if($this->search_attachments == true)
                  {
                     array_push($text_where_clauses, "a.description LIKE '%$this->search_text%'");
                  }
                  if($this->search_comments==true)
                  {
                     array_push($comment_text_where_clauses, "c.related_link LIKE '%$this->search_text%'");
                     array_push($comment_text_where_clauses, "c.author_name LIKE '%$this->search_text%'");
                     array_push($comment_text_where_clauses, "c.author_organisation LIKE '%$this->search_text%'");
                     array_push($comment_text_where_clauses, "c.author_email LIKE '%$this->search_text%'");
                     array_push($comment_text_where_clauses, "c.author_phone LIKE '%$this->search_text%'");
                     array_push($comment_text_where_clauses, "c.author_address LIKE '%$this->search_text%'");
                     array_push($comment_text_where_clauses, "c.comment_title LIKE '%$token_srch_string%'");
                     array_push($comment_text_where_clauses, "c.comment_content LIKE '%$token_srch_string%'");
                  }
               }
            }
            else
            {
               $this->addToWhereClause($text_where_clauses,"s.related_link",$this->related_link,$this->related_link_exact);
               $this->addToWhereClause($text_where_clauses,"s.author_name",$this->author_name,$this->author_name_exact);
               $this->addToWhereClause($text_where_clauses,"s.author_organisation",$this->author_organisation,$this->author_organisation_exact);
               $this->addToWhereClause($text_where_clauses,"s.author_email",$this->author_email,$this->author_email_exact);
               $this->addToWhereClause($text_where_clauses,"s.author_phone",$this->author_phone,$this->author_phone_exact);
               $this->addToWhereClause($text_where_clauses,"s.author_address",$this->author_address,$this->author_address_exact);
               $this->addToWhereClause($text_where_clauses,"s.story_title",$this->story_title,$this->story_title_exact, true);
               $this->addToWhereClause($text_where_clauses,"s.story_subtitle",$this->story_subtitle,$this->story_subtitle_exact, true);
               $this->addToWhereClause($text_where_clauses,"s.story_summary",$this->story_summary,$this->story_summary_exact, true);
               $this->addToWhereClause($text_where_clauses,"s.story_content",$this->story_content,$this->story_content_exact, true);
               if($this->search_comments==true && $this->select_extent=="ALL" )
               {
                  $this->addToWhereClause($comment_text_where_clauses,"c.related_link",$this->related_link,$this->related_link_exact);
                  $this->addToWhereClause($comment_text_where_clauses,"c.author_name",$this->author_name,$this->author_name_exact);
                  $this->addToWhereClause($comment_text_where_clauses,"c.author_organisation",$this->author_organisation,$this->author_organisation_exact);
                  $this->addToWhereClause($comment_text_where_clauses,"c.author_email",$this->author_email,$this->author_email_exact);
                  $this->addToWhereClause($comment_text_where_clauses,"c.author_phone",$this->author_phone,$this->author_phone_exact);
                  $this->addToWhereClause($comment_text_where_clauses,"c.author_address",$this->author_address,$this->author_address_exact);
                  $this->addToWhereClause($comment_text_where_clauses,"c.comment_title",$this->story_title,$this->story_title_exact, true);
                  $this->addToWhereClause($comment_text_where_clauses,"c.comment_content",$this->story_content,$this->story_content_exact, true);
               }
            }
            // hack to prevent cross dependency on image query
            if($this->attachment_query > 0) array_push($where_clauses, "s.story_id>0");
            if($this->ignore_hidden==false && $this->hidden==false)
            {
               if($this->attachment_query==true)
               {
                  array_push($where_clauses, "a.hidden = '$this->hidden'");
                  array_push($where_clauses, "(c.hidden = '$this->hidden' || a.comment_id = 0)");
                  $stmt = "SELECT $select_fields FROM ".$prefix."_attachments as a ";
                  // Seems to be a problem in MySQL 5 or something. The order matters
                  $stmt = $stmt."LEFT JOIN ".$prefix."_stories as s ON (s.story_id=a.story_id AND s.hidden='$this->hidden') ";
                  $stmt = $stmt."LEFT JOIN ".$prefix."_comments as c ON (s.story_id=c.story_id AND c.comment_id=a.comment_id AND c.hidden='$this->hidden') ";
                  if($this->select_votes == true)
                  {
                     $stmt = $stmt."LEFT JOIN ".$prefix."_votes as v ON (s.story_id=v.item_id ) ";
                  }
               }
               else
               {
                  array_push($where_clauses, "s.hidden = '$this->hidden'");
                  $stmt = "SELECT $select_fields FROM ".$prefix."_stories as s ";
                  if($this->select_extent=="ALL")
                  {
                     $stmt = $stmt."LEFT JOIN ".$prefix."_comments as c ON (s.story_id=c.story_id AND c.hidden='$this->hidden') ";
                    $stmt = $stmt."LEFT JOIN ".$prefix."_attachments as a ON (s.story_id=a.story_id AND a.hidden='$this->hidden') ";
                  }
                  if($this->select_votes == true)
                  {
                     $stmt = $stmt."LEFT JOIN ".$prefix."_votes as v ON (s.story_id=v.item_id ) ";
                  }
               }
            }
            else
            {
               if($this->ignore_hidden==false && $this->hidden==true)
               {
                  if($this->select_extent=="ALL")
                  {
                     //array_push($where_clauses, "(s.hidden = '$this->hidden' OR c.hidden = '$this->hidden' OR a.hidden = '$this->hidden')");
                     array_push($where_clauses, "(s.hidden > 0 OR c.hidden > 0 OR a.hidden > 0)");
                  }
                  else
                  {
                     //array_push($where_clauses, "(s.hidden = '$this->hidden')");
                     array_push($where_clauses, "(s.hidden > 0)");
                  }
               }
               if($this->attachment_query==true)
               {
                  $stmt = "SELECT $select_fields FROM ".$prefix."_attachments as a ";
                  //this next line was not needed (I think) and it causes the thing to crash when on mysql 5
                  // $stmt = $stmt."LEFT JOIN ".$prefix."_comments as c ON s.story_id=c.story_id ";
                  $stmt = $stmt."LEFT JOIN ".$prefix."_stories as s ON s.story_id=a.story_id ";
               }
               else
               {
                  $stmt = "SELECT $select_fields FROM ".$prefix."_stories as s ";
                  if($this->select_extent=="ALL")
                  {
                     $stmt = $stmt."LEFT JOIN ".$prefix."_comments as c ON s.story_id=c.story_id ";
                     $stmt = $stmt."LEFT JOIN ".$prefix."_attachments as a ON s.story_id=a.story_id ";
                  }
               }
               if($this->select_votes == true)
               {
                  $stmt = $stmt."LEFT JOIN ".$prefix."_votes as v ON (s.story_id=v.item_id ) ";
               }
            }
            // add custom where clauses
            for($i=0;$i<count($this->custom_where_clauses);$i++)
            {
               array_push($where_clauses, $this->custom_where_clauses[$i]);
            }
            if(count($where_clauses)>0 || count($text_where_clauses) >0)
            {
               $stmt = $stmt." WHERE ";
            }

            for($i=0;$i<count($where_clauses);$i++)
            {
               $stmt = $stmt.$where_clauses[$i];
               if ($i < count($where_clauses)-1 || count($text_where_clauses) > 0 || count($comment_text_where_clauses) > 0)
                  $stmt = $stmt." AND ";
            }
            if(count($text_where_clauses)>0 && count($comment_text_where_clauses) >0)
            {
               $stmt = $stmt." ( ";
            }

            for($i=0;$i<count($text_where_clauses);$i++)
            {
               if($i==0) $stmt = $stmt." ( ";
               $stmt = $stmt.$text_where_clauses[$i];
               if ($i < count($text_where_clauses)-1)
                  $stmt = $stmt." ".$this->text_where_mode." ";
               else $stmt = $stmt." ) ";
            }
            if(count($text_where_clauses)>0 && count($comment_text_where_clauses)>0)
            {
               $stmt = $stmt." OR ";
            }
            for($i=0;$i<count($comment_text_where_clauses);$i++)
            {
               if($i==0) $stmt = $stmt." ( ";
               $stmt = $stmt.$comment_text_where_clauses[$i];
               if ($i < count($comment_text_where_clauses)-1)
               $stmt = $stmt." ".$this->text_where_mode." ";
               else $stmt = $stmt." ) ";
            }
            if(count($text_where_clauses)>0 && count($comment_text_where_clauses) >0)
            {
               $stmt = $stmt." ) ";
            }
            if($this->attachment_query==true)
            {
               //if($this->one_image_per_story == true)
               //{
               //   $stmt=$stmt." GROUP BY a.story_id";
               //}
               //else
               //{
                  $stmt=$stmt." GROUP BY a.attachment_id";
               //}
            }
            else if($this->select_extent=="ALL")  $stmt=$stmt." GROUP BY s.time_posted";
            for($i=0;$i<count($having_clauses);$i++)
            {
               if($i==0) $stmt = $stmt." HAVING ";
               $stmt = $stmt.$having_clauses[$i];
               if($i<count($having_clauses)-1) $stmt = $stmt." AND ";
            }
            for($i=0;$i<count($order_clauses);$i++)
            {
               if($i==0) $stmt = $stmt." ORDER BY ";
               $stmt = $stmt.$order_clauses[$i];
               if($i<count($order_clauses)-1) $stmt = $stmt.", ";
            }
            $stmt=$stmt." LIMIT $this->results_offset,$this->results_limit";
            //execute statement and it is always for a story or comment 
            $result = sql_query($stmt, $dbconn, 2);
            $res = checkForError($result);
            if(!$res) return;
            // process and return array of stories
            $story_list = array();
            $story = new Story();
            $attachment_list = array();
            $attachment = new Attachment();
            if($this->attachment_query==true)
            {
                while(list($attachment->attachment_id,$attachment->story_id, $attachment->comment_id, $attachment->time_posted, $attachment->attachment_file, $attachment->description, $attachment->image, $attachment->video, $attachment->audio, $attachment->hidden, $attachment->featurized) = sql_fetch_row($result, $dbconn))
                {
                    array_push($attachment_list,$attachment);
                    $attachment = new Attachment();
                }
            }
            else if($this->select_extent=="ALL")
            {
               if($this->select_mode=="TICKER")
               {
                  if($this->select_votes == false) {
                      while(list($story->story_id,$story->topic_id, $story->region_id, $story->type_id, $story->language_id, $story->time_posted, $story->author_name, $story->author_organisation, $story->story_title, $story->hidden, $story->locked, $story->event_time, $story->sticky_time, $story->comments_amount, $story->time_last_comment_posted, $story->attachments_amount, $story->image_amount, $story->video_amount, $story->audio_amount) = sql_fetch_row($result, $dbconn)) {
                         $story->miscellaneous_amount=$story->attachments_amount-($story->image_amount+$story->video_amount+$story->audio_amount);
                         array_push($story_list,$story);
                         $story = new Story();
                      }
                  } else {
                      // select_votes is true 
                      while(list($story->story_id,$story->topic_id, $story->region_id, $story->type_id, $story->language_id, $story->time_posted, $story->author_name, $story->author_organisation, $story->story_title, $story->hidden, $story->locked, $story->event_time, $story->sticky_time, $story->vote, $story->comments_amount, $story->time_last_comment_posted, $story->attachments_amount, $story->image_amount, $story->video_amount, $story->audio_amount) = sql_fetch_row($result, $dbconn)) {
                         $story->miscellaneous_amount=$story->attachments_amount-($story->image_amount+$story->video_amount+$story->audio_amount);
                         array_push($story_list,$story);
                         $story = new Story();
                      }
                  }
               }
               else if($this->select_mode=="SUMMARY" && $this->select_votes == false)
               {
                  while(list($story->story_id,$story->topic_id, $story->region_id, $story->type_id, $story->language_id, $story->time_posted, $story->related_link, $story->author_name, $story->author_organisation, $story->author_email, $story->author_phone, $story->author_address, $story->story_title, $story->story_subtitle, $story->story_summary, $story->hidden, $story->locked, $story->event_time, $story->sticky_time, $story->comments_amount, $story->time_last_comment_posted, $story->attachments_amount, $story->image_amount, $story->video_amount, $story->audio_amount) = sql_fetch_row($result, $dbconn))
                  {
                     $story->miscellaneous_amount=$story->attachments_amount-($story->image_amount+$story->video_amount+$story->audio_amount);
                     array_push($story_list,$story);
                     $story = new Story();
                  }
               }
               else if($this->select_mode=="SUMMARY" && $this->select_votes == true)
               {
                  while(list($story->story_id,$story->topic_id, $story->region_id, $story->type_id, $story->language_id, $story->time_posted, $story->related_link, $story->author_name, $story->author_organisation, $story->author_email, $story->author_phone, $story->author_address, $story->story_title, $story->story_subtitle, $story->story_summary, $story->hidden, $story->locked, $story->event_time, $story->sticky_time, $story->vote, $story->comments_amount, $story->time_last_comment_posted, $story->attachments_amount, $story->image_amount, $story->video_amount, $story->audio_amount) = sql_fetch_row($result, $dbconn))
                  {
                     $story->miscellaneous_amount=$story->attachments_amount-($story->image_amount+$story->video_amount+$story->audio_amount);
                     array_push($story_list,$story);
                     $story = new Story();
                  }
               }
               else if($this->select_mode=="FULL")
               {
                  while(list($story->story_id,$story->topic_id, $story->region_id, $story->type_id, $story->language_id, $story->time_posted, $story->related_link, $story->author_name, $story->author_organisation, $story->author_email, $story->author_phone, $story->author_address, $story->story_title, $story->story_subtitle, $story->story_summary, $story->story_content, $story->hidden, $story->locked, $story->event_time, $story->sticky_time, $story->comments_amount, $story->time_last_comment_posted, $story->attachments_amount, $story->image_amount, $story->video_amount, $story->audio_amount) = sql_fetch_row($result, $dbconn))
                  {
                     $story->miscellaneous_amount=$story->attachments_amount-($story->image_amount+$story->video_amount+$story->audio_amount);
                     array_push($story_list,$story);
                     $story = new Story();
                  }
               }
            }
            else if($this->select_extent=="STORY")
            {
                if($this->select_mode=="TICKER")
                {
                   if($this->select_votes == true)
                   {
                       while(list($story->story_id,$story->topic_id, $story->region_id, $story->type_id, $story->language_id, $story->time_posted, $story->author_name, $story->author_organisation, $story->story_title, $story->hidden, $story->locked, $story->event_time, $story->sticky_time, $story->vote) = sql_fetch_row($result, $dbconn))
                       {
                          array_push($story_list,$story);
                          $story = new Story();
                       }
                   } else {
                       while(list($story->story_id,$story->topic_id, $story->region_id, $story->type_id, $story->language_id, $story->time_posted, $story->author_name, $story->author_organisation, $story->story_title, $story->hidden, $story->locked, $story->event_time, $story->sticky_time) = sql_fetch_row($result, $dbconn))
                       {
                          array_push($story_list,$story);
                          $story = new Story();
                       }
                   }
                }
                else if($this->select_mode=="SUMMARY")
                {
                   while(list($story->story_id,$story->topic_id, $story->region_id, $story->type_id, $story->language_id, $story->time_posted, $story->related_link, $story->author_name, $story->author_organisation, $story->author_email, $story->author_phone, $story->author_address, $story->story_title, $story->story_subtitle, $story->story_summary, $story->hidden, $story->locked, $story->event_time, $story->sticky_time) = sql_fetch_row($result, $dbconn))
                   {
                      array_push($story_list,$story);
                      $story = new Story();
                   }
                }
                else if($this->select_mode=="FULL")
                {
                   while(list($story->story_id,$story->topic_id, $story->region_id, $story->type_id, $story->language_id, $story->time_posted, $story->related_link, $story->author_name, $story->author_organisation, $story->author_email, $story->author_phone, $story->author_address, $story->story_title, $story->story_subtitle, $story->story_summary, $story->story_content, $story->hidden, $story->locked, $story->event_time, $story->sticky_time) = sql_fetch_row($result, $dbconn))
                   {
                      array_push($story_list,$story);
                      $story = new Story();
                   }
                }
            }
            if($this->attachment_query==true)  return $attachment_list;
            else return $story_list;
        }

        function loadFormValues()
        {
           global $topicList,$regionList,$typeList,$languageList,$system_config,$pagelanguage, $userLanguage, $user_prefs;
           //turn off strict error reporting -> used for silencing this big during development
           $e_original = error_reporting(E_ERROR);
           $this->story_id = cleanseNumericalQueryField($_REQUEST["story_id"]);
           $this->obj_id = cleanseNumericalQueryField($_REQUEST["obj_id"]);
           // folowing line is hack to allow urls from old system to map to new system
           if($this->story_id==null) $this->story_id = cleanseNumericalQueryField($_REQUEST["id"]);
           $this->comment_id = cleanseNumericalQueryField($_REQUEST["comment_id"]);

           $this->language_id = $_REQUEST["language_id"];
           if($this->language_id==NULL && $_REQUEST["language"]!=NULL && $_REQUEST["language"]!="all")
           {
              $temp_language = $languageList->getLanguageByName($_REQUEST["language"]);
              if($temp_language==NULL) $temp_language = $languageList->getLanguageByCode($_REQUEST["language"]);
              if($temp_language!=NULL) $this->language_id = $temp_language->language_id;
           }
           if($this->language_id==null && $_REQUEST["language"]!="all" && ($system_config->mono_lingual_filter==true or (isset($_REQUEST['mono_lingual_filter']) && $_REQUEST['mono_lingual_filter'] == 'on')))
           {
              //$acceptLanguage = $languageList->getAcceptLanguage();
              //if($acceptLanguage!=null) $this->language_id = $acceptLanguage->language_id;
              //op_r($userLanguage);
              $this->language_id = $userLanguage->language_id;
           }
           else if($this->language_id=="all")
           {
              $this->language_id==null;
           }
           $this->language_id = cleanseNumericalQueryField($this->language_id);

           $this->topic_id = cleanseNumericalQueryField($_REQUEST["topic_id"]);
           if($this->topic_id==NULL && $_REQUEST["topic"]!=NULL)
           {
              $temp_topic = $topicList->getTopicByName($_REQUEST["topic"],$this->language_id);
              if($temp_topic!=NULL) $this->topic_id = $temp_topic->topic_id;
           }
           $this->region_id = cleanseNumericalQueryField($_REQUEST["region_id"]);
           if($this->region_id==NULL && $_REQUEST["region"]!=NULL)
           {
              $temp_region = $regionList->getRegionByName($_REQUEST["region"],$this->language_id);
              if($temp_region!=NULL) $this->region_id = $temp_region->region_id;
           }
           $this->type_id = cleanseNumericalQueryField($_REQUEST["type_id"]);
           if($this->type_id==NULL && $_REQUEST["type"]!=NULL)
           {
              $temp_type = $typeList->getTypeByName($_REQUEST["type"],$this->language_id);
              if($temp_type!=NULL) $this->type_id = $temp_type->type_id;
              //else if($_REQUEST["type"]=="all") $this->type_id="all";
           }

           $this->time_posted = cleanseNumericalQueryField($_REQUEST["time_posted"]);
	   /*
           $this->related_link = addslashes($_REQUEST["related_link"]);
           $this->author_name = addslashes($_REQUEST["author_name"]);
           $this->author_organisation = addslashes($_REQUEST["author_organisation"]);
           $this->author_email = addslashes($_REQUEST["author_email"]);
           $this->author_phone = addslashes($_REQUEST["author_phone"]);
           $this->author_address = addslashes($_REQUEST["author_address"]);
           $this->story_title = addslashes($_REQUEST["story_title"]);
           $this->story_subtitle = addslashes($_REQUEST["story_subtitle"]);
           $this->story_summary = addslashes($_REQUEST["story_summary"]);
	   $this->story_content = addslashes($_REQUEST["story_content"]);
	   */
	   $this->validateSearchData();
           
           global $editor_session;
           if($editor_session->isSessionOpen() && $editor_session->editor->allowedReadAccessTo("viewhiddencontent"))
           {
              if($_REQUEST["hidden"]!=null) $this->hidden = $_REQUEST["hidden"];
              else $this->ignore_hidden=true;
           }
           if($_REQUEST["search_comments"]!=null) $this->search_comments = addslashes($_REQUEST["search_comments"]);
           if($_REQUEST["search_attachments"]!=null) $this->search_attachments = addslashes($_REQUEST["search_attachments"]);
           if($_REQUEST["text_where_mode"]!=null) $this->text_where_mode = addslashes($_REQUEST["text_where_mode"]);
           if($_REQUEST["search_text"]!=null) 
	   {
	   	// Add length checks. A search string longer than 100 is probably not a real search but a bot
		$t_search = trim(addslashes(urldecode($_REQUEST["search_text"])));
		if (strlen($t_search) > 0 && strlen($t_search) <= 100)
	   	    $this->search_text = $t_search;
		else echo "Search [".$t_search."] longer than 100 chars <BR>";
           }
           if($_REQUEST["media_type"]!=null) $this->media_type = addslashes($_REQUEST["media_type"]);
	   if($_REQUEST["convert_dates"]!=null)
           {
              if($_REQUEST["time_posted_upper_limit"]!=null) $this->time_posted_upper_limit = strtotime($_REQUEST["time_posted_upper_limit"]);
              if($_REQUEST["time_posted_lower_limit"]!=null) $this->time_posted_lower_limit = strtotime($_REQUEST["time_posted_lower_limit"]);
              if($this->type_id==$system_config->event_type_id)
              {
                 if($_REQUEST["event_time_upper_limit"]!=null) $this->event_time_upper_limit = strtotime($_REQUEST["event_time_upper_limit"]);
                 if($_REQUEST["event_time_lower_limit"]!=null) $this->event_time_lower_limit = strtotime($_REQUEST["event_time_lower_limit"]);
              }
              if($_REQUEST["sticky_time_upper_limit"]!=null) $this->sticky_time_upper_limit = strtotime($_REQUEST["sticky_time_upper_limit"]);
              if($_REQUEST["sticky_time_lower_limit"]!=null) $this->sticky_time_lower_limit = strtotime($_REQUEST["sticky_time_lower_limit"]);
           }
           else
           {
              if($_REQUEST["time_posted_upper_limit"]!=null) $this->time_posted_upper_limit = cleanseNumericalQueryField($_REQUEST["time_posted_upper_limit"]);
              if($_REQUEST["time_posted_lower_limit"]!=null) $this->time_posted_lower_limit = cleanseNumericalQueryField($_REQUEST["time_posted_lower_limit"]);
              if($this->type_id==$system_config->event_type_id)
              {
                 if($_REQUEST["event_time_upper_limit"]!=null) $this->event_time_upper_limit = cleanseNumericalQueryField($_REQUEST["event_time_upper_limit"]);
                 if($_REQUEST["event_time_lower_limit"]!=null) $this->event_time_lower_limit = cleanseNumericalQueryField($_REQUEST["event_time_lower_limit"]);
              }
              if($_REQUEST["sticky_time_upper_limit"]!=null) $this->sticky_time_upper_limit = cleanseNumericalQueryField($_REQUEST["sticky_time_upper_limit"]);
              if($_REQUEST["sticky_time_lower_limit"]!=null) $this->sticky_time_lower_limit = cleanseNumericalQueryField($_REQUEST["sticky_time_lower_limit"]);
           }
	   
           if($_REQUEST["related_link_exact"]!=null) $this->related_link_exact = $_REQUEST["related_link_exact"];
           if($_REQUEST["author_name_exact"]!=null) $this->author_name_exact = $_REQUEST["author_name_exact"];
           if($_REQUEST["author_organisation_exact"]!=null) $this->author_organisation_exact = $_REQUEST["author_organisation_exact"];
           if($_REQUEST["author_email_exact"]!=null) $this->author_email_exact = $_REQUEST["author_email_exact"];
           if($_REQUEST["author_phone_exact"]!=null) $this->author_phone_exact = $_REQUEST["author_phone_exact"];
           if($_REQUEST["author_address_exact"]!=null) $this->author_address_exact = $_REQUEST["author_address_exact"];
           if($_REQUEST["story_title_exact"]!=null) $this->story_title_exact = $_REQUEST["story_title_exact"];
           if($_REQUEST["story_subtitle_exact"]!=null) $this->story_subtitle_exact = $_REQUEST["story_subtitle_exact"];
           if($_REQUEST["story_summary_exact"]!=null) $this->story_summary_exact = $_REQUEST["story_summary_exact"];
           if($_REQUEST["story_content_exact"]!=null) $this->story_content_exact = $_REQUEST["story_content_exact"];
	  
           if($_REQUEST["order_mode"]!=null) $this->order_mode = addslashes($_REQUEST["order_mode"]);
           if($_REQUEST["order_direction"]!=null) $this->order_direction = addslashes($_REQUEST["order_direction"]);
           if(isset($_REQUEST["results_offset"]))
           {
              $this->results_offset = cleanseNumericalQueryField($_REQUEST["results_offset"]);
           }
           if($_REQUEST["results_limit"]!=null) $this->results_limit = cleanseNumericalQueryField($_REQUEST["results_limit"]);
           //op_r($this);
           error_reporting($e_original);
           $this->loaded_from_form = true;
        }

        function validateSearchData()
        {
           if ($_REQUEST["related_link"] != null) {
               if (strlen(trim(addslashes($_REQUEST["related_link"]))) > 0) {
                   $this->related_link = trim(addslashes($_REQUEST["related_link"]));
	       }
	   }
           if ($_REQUEST["author_name"] != null) {
               if (strlen(trim(addslashes($_REQUEST["author_name"]))) > 0) {
                   $this->author_name = trim(addslashes($_REQUEST["author_name"]));
	       }
	   }
           if ($_REQUEST["author_organisation"] != null) {
               if (strlen(trim(addslashes($_REQUEST["author_organisation"]))) > 0) {
                   $this->author_organisation = trim(addslashes($_REQUEST["author_organisation"]));
	       }
	   }
           if ($_REQUEST["author_email"] != null) {
               if (strlen(trim(addslashes($_REQUEST["author_email"]))) > 0) {
                   $this->author_email = trim(addslashes($_REQUEST["author_email"]));
	       }
	   }
           if ($_REQUEST["author_phone"] != null) {
               if (strlen(trim(addslashes($_REQUEST["author_phone"]))) > 0) {
                   $this->author_phone = trim(addslashes($_REQUEST["author_phone"]));
	       }
	   }
           if ($_REQUEST["author_address"] != null) {
               if (strlen(trim(addslashes($_REQUEST["author_address"]))) > 0) {
                   $this->author_address = trim(addslashes($_REQUEST["author_address"]));
	       }
	   }
           if ($_REQUEST["story_title"] != null) {
               if (strlen(trim(addslashes($_REQUEST["story_title"]))) > 0) {
                   $this->story_title = trim(addslashes($_REQUEST["story_title"]));
	       }
	   }
           if ($_REQUEST["story_subtitle"] != null) {
               if (strlen(trim(addslashes($_REQUEST["story_subtitle"]))) > 0) {
                   $this->story_subtitle = trim(addslashes($_REQUEST["story_subtitle"]));
	       }
	   }
           if ($_REQUEST["story_summary"] != null) {
               if (strlen(trim(addslashes($_REQUEST["story_summary"]))) > 0) {
                   $this->story_summary = trim(addslashes($_REQUEST["story_summary"]));
	       }
	   }
           if ($_REQUEST["story_content"] != null) {
               if (strlen(trim($_REQUEST["story_content"])) > 0) {
                   $this->story_content = trim(addslashes($_REQUEST["story_content"]));
	       }
	   }
        }

        function getQueryString($useprefs=false, $escapeChars=true)
        {
           global $system_config,$topicList,$regionList,$typeList,$languageList;
           $strings = array();
           if($this->story_id!=null) array_push($strings,"story_id=".$this->story_id);
           if($this->comment_id!=null) array_push($strings,"comment_id=".$this->comment_id);
	   if($this->obj_id!=null) array_push($strings,"obj_id=".$this->obj_id);

           $language = null;
           if($this->language_id != null)
           {
              $language=$languageList->getLanguageByID($this->language_id);
              if($language!=null) array_push($strings,"language=".$language->getStrippedCode());
           }
           /*if($language==null && ($system_config->mono_lingual_filter==true or (isset($_REQUEST['mono_lingual_filter']) && $_REQUEST['mono_lingual_filter'] == 'on')))
           {
               array_push($strings,"language=all");
           }*/
           if($this->topic_id != null)
           {
              $topic=$topicList->getTopicByID($this->topic_id,$this->language_id);
              if($topic!=null) array_push($strings,"topic=".$topic->getStrippedName());
           }
           if($this->region_id != null)
           {
              $region=$regionList->getRegionByID($this->region_id,$this->language_id);
              if($region!=null) array_push($strings,"region=".$region->getStrippedName());
           }
           if($this->type_id != null)
           {
              $type=$typeList->getTypeByID($this->type_id,$this->language_id);
              if($type!=null) array_push($strings,"type=".$type->getStrippedName());
           }

           if($this->time_posted!=null) array_push($strings,"time_posted=".$this->time_posted);
           if($this->text_where_mode!="OR")
              array_push($strings,"text_where_mode=".stripslashes($this->text_where_mode));
           if($this->search_text!=null)
              array_push($strings,"search_text=".stripslashes($this->search_text));
           if($this->search_comments!=null)
              array_push($strings,"search_comments=".stripslashes($this->search_comments));
           if($this->search_attachments!=null)
              array_push($strings,"search_attachments=".stripslashes($this->search_attachments));
           if($this->media_type!=null && $this->media_type!="ALL")
              array_push($strings,"media_type=".$this->media_type);
           if($this->time_posted_upper_limit!=null)
              array_push($strings,"time_posted_upper_limit=".$this->time_posted_upper_limit);
           if($this->time_posted_lower_limit!=null)
              array_push($strings,"time_posted_lower_limit=".$this->time_posted_lower_limit);
           if($this->type_id==$system_config->event_type_id)
           {
              if($this->event_time_upper_limit!=null)
                 array_push($strings,"event_time_upper_limit=".$this->event_time_upper_limit);
              if($this->event_time_lower_limit!=null)
                 array_push($strings,"event_time_lower_limit=".$this->event_time_lower_limit);
           }
           if($this->sticky_time_upper_limit!=null)
              array_push($strings,"sticky_time_upper_limit=".$this->sticky_time_upper_limit);
           if($this->sticky_time_lower_limit!=null)
              array_push($strings,"sticky_time_lower_limit=".$this->sticky_time_lower_limit);
           if($this->related_link != null)
              $this->addToQueryString($strings, "related_link", $this->related_link, $this->related_link_exact);
           if($this->author_name != null)
              $this->addToQueryString($strings, "author_name", $this->author_name, $this->author_name_exact);
           if($this->author_organisation != null)
              $this->addToQueryString($strings, "author_organisation", $this->author_organisation, $this->author_organisation_exact);
           if($this->author_email != null)
              $this->addToQueryString($strings, "author_email", $this->author_email, $this->author_email_exact);
           if($this->author_phone != null)
              $this->addToQueryString($strings, "author_phone", $this->author_phone, $this->author_phone_exact);
           if($this->author_address != null)
              $this->addToQueryString($strings, "author_address", $this->author_address, $this->author_address_exact);
           if($this->story_title != null)
              $this->addToQueryString($strings, "story_title", $this->story_title, $this->story_title_exact);
           if($this->story_subtitle != null)
              $this->addToQueryString($strings, "story_subtitle", $this->story_subtitle, $this->story_subtitle_exact);
           if($this->story_summary != null)
              $this->addToQueryString($strings, "story_summary", $this->story_summary, $this->story_summary_exact);
           if($this->story_content != null) $this->addToQueryString($strings, "story_content", $this->story_content, $this->story_content_exact);
           if($this->hidden!=null) array_push($strings,"hidden=".$this->hidden);
           if($this->locked!=null) array_push($strings,"locked=".$this->locked);
           if($this->order_direction!="DESC") array_push($strings,"order_direction=".stripslashes($this->order_direction));
           if($this->order_mode!="timeposted") array_push($strings,"order_mode=".stripslashes($this->order_mode));
	   if($this->results_offset!=0)  array_push($strings,"results_offset=".$this->results_offset);
	   $returnstring = "";
           for($i=0;$i<count($strings);$i++)
           {
              if($i==0) $returnstring = $returnstring."?";
              $returnstring = $returnstring.$strings[$i];
              if($i<count($strings)-1) $returnstring = $returnstring."&";
           }
           if($useprefs)
           {
              global $user_prefs;
              $pref_str = $user_prefs->getQueryString();
              if(strrchr($returnstring, '?') === false && strlen($pref_str) > 0) $pref_str = substr_replace($pref_str, '?', 0,1);
              $returnstring .= $pref_str;
           }
           if($escapeChars) return htmlspecialchars($returnstring, ENT_QUOTES);
           else return $returnstring;
        }

        function addToQueryString(&$strings, $field_name, $field_value, $exact)
        {
           if($field_value != NULL && strlen(trim($field_value)) > 0)
           {
              array_push($strings, $field_name."=".stripslashes($field_value));
	      if($exact == true) array_push($strings, $field_name."_exact=".$exact);
           }
        }

        function getStopwordsList()
        {
           return array("a","the", "of", "in", "an", "to", "this");
        }

        function tokenizeSearchString($search_string, $for_like=true, $field_name="")
        {
           // Example search string: Peak Oil Motorways -parse the 3 words
           $stopword_list = $this->getStopwordsList();
	   //	SELECT * FROM post WHERE postbody LIKE '%dog%' AND postbody RLIKE '[[::]]';
           if($search_string != NULL)
           {
              $str_array = explode(" ",$search_string);
              if(count($str_array) > 1 ) {
                  $search_string = "";
                  $t_max = count($str_array);
                  for ($j=0; $j < ($t_max -1); $j++){
		      //echo "chk ".$str_array[$j] . " ";
		      if (trim($str_array[$j]) == "") continue;
		      if ($for_like == true) {
		          //if (in_array($str_array[$j], $stopword_list)) echo "stopword ".$str_array[$j] . " ";
		          if (in_array($str_array[$j], $stopword_list)) $search_string .= $str_array[$j] . " ";
		          else $search_string .= $str_array[$j] . "%";
		      } else {
		          if (in_array($str_array[$j], $stopword_list)) continue;
		          else $search_string .= " AND ".$field_name." RLIKE '".$str_array[$j] . "[[:>:]]' ";
		      }
                  }
		  if ($for_like == true) {
		      if (in_array($str_array[($t_max-1)], $stopword_list)) $search_string .= $str_array[($t_max-1)]."";
		      else $search_string .= $str_array[($t_max-1)] ;
		  } else {
		      if (in_array($str_array[($t_max-1)], $stopword_list)) ;
		      else $search_string .= " AND ".$field_name." RLIKE '".$str_array[($t_max-1)] . "[[:>:]]' ";
		  }
	      } else if ($for_like == false) {
		  if (in_array(trim($search_string), $stopword_list)) ;
		  else $search_string = " AND ".$field_name." RLIKE '".trim($search_string) . "[[:>:]]' ";
	      }
           }
           return $search_string;
        }


        function isDetailedQuery()
        {
            //Basically checks if the query is scanning for more than the basic categories
            if($this->search_text!=null) return true;
            if($this->related_link!=null) return true;
            if($this->author_name!=null) return true;
            if($this->author_organisation!=null) return true;
            if($this->author_email!=null) return true;
            if($this->author_phone!=null) return true;
            if($this->author_address!=null) return true;
            if($this->story_title!=null) return true;
            if($this->story_subtitle!=null) return true;
            if($this->story_summary!=null) return true;
            if($this->story_content!=null) return true;
            if($this->time_posted_upper_limit!=null) return true;
            if($this->time_posted_lower_limit!=null) return true;
            return false;
        }


        function maskHeadlineQueryFields($use_author=false)
        {
            if(!$use_author)
            {
               $this->author_name = null;
            }
            $this->search_text=null;
            $this->related_link=null;
            $this->author_organisation=null;
            $this->author_email=null;
            $this->author_phone=null;
            $this->author_address=null;
            $this->story_title=null;
            $this->story_subtitle=null;
            $this->story_summary=null;
            $this->story_content=null;
            $this->results_offset=0;
            $this->media_type = "ALL";
        }


        function isBasicQuery()
        {
            if($this->topic_id!=null) return true;
            if($this->region_id!=null) return true;
            if($this->type_id!=null) return true;
            if($this->language_id!=null) return true;
            return false;
        }


        function isSpecificStoryQuery()
        {
            if($this->story_id!=null) return true;
            return false;
        }


        function isSpecificCommentQuery()
        {
            if($this->comment_id!=null) return true;
            return false;
        }
    }
?>
