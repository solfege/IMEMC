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

include_once "objects/imagetool.inc";
include_once "config/configuration.php";
include_once "objects/constants.inc";


class SystemConfig
{
    //the first group of configuration variables require editing of this file
    //they are not settable in editconfiguration - must be changed here
  /*  var $software_name="Oscailt";
    var $software_version="3.00.beta-3";
    var $disallow_ipless_publication = false;
    var $event_type_id=5;

    var $max_unshortened_tag_length = 60;
    var $feature_type_id=1;
    var $cookie_life=365;// 1 year
    var $session_life = 3600; //1 hour
    var $sql_query_caching_enabled = true;
    var $sql_query_caching_healing_timeout = 300; // 5 mins
    var $default_language_code = 'en';
    var $logging_enabled = true;
    var $xml_store = "xmldata/";
    var $log_store = "logs/";
    var $xmltypedef_dir = "xmldata/indydatatypes/";
    var $indyobject_code_dir = "objects/indyobjects/indydataclasses/";
    var $support_v1_url_remaps = true;
    var $debug_level = 5;//higher for more logging info, lower for less. lower than 1 is a no-no
    var $use_vague_author_name = false;

    //needs to be made writable if you want to save templates
    var $object_template_store = "xmldata/templates/";
    //this should really be generated to cache but currently isn't
    var $item_action_cache_store = "xmldata/itemcache/";
    var $exported_indices_name = "exported_site_";

    var $objects_per_cache_directory = 50;
    var $back_up_data_object_index = true;
    var $forbid_consecutive_locks_without_save = false;
    var $rss_fetch_timeout = 5;
    var $default_writable_directory_permissions = 0777;
    var $monitor_size_limit = 20;
*/
    //these values are just for install - after this, they are all relative to the private cache
    // YOU MUST ADD THE TRAILING SLASH TO THESE directories.
    var $query_cache_store = "cache/querycache/";
    var $html_cache_store = "cache/objectcache/";
    var $rss_cache = "cache/rsscache/";
    var $object_name_store = "cache/objectnamecache/";
    var $type_details_store = "cache/typedetailscache/";
    var $redirection_cache_store = "cache/redirectcache.inc";
    var $publish_monitor_cache_store = "cache/publishmonitorcache.inc";

    // This sub bunch cannot be set from the configuration screen
    var $status_monitor_cache_store = "cache/publishstatecache.inc";

    var $reminder_cache_store = "cache/reminder.inc";
    // var $reminder_size_limit= 100;
    // Spam related settings.
    // Maximum URI request length before we log it as suspected spam. Beware some of the archive and
    // search queries can be quite long. Should be at least greater than 240 or so.
    var $max_uri_suspect_spam = 240;
    // If a URI request is greater length than this then we are pretty sure it is spam and just redirect
    // the request to say the about page rather than index as that would use more resources to rsp with.
    var $max_uri_redirect_spam = 650;

    //everything from here on can be set
    //from editconfiguration.php
    //these default values just used for install

    var $use_friendly_urls = true;
    var $publish_monitor_enabled = false;
    var $image_cache = "cache/imagecache";
    var $site_file_store_base = "attachments/sites/";
    //if you want to store the metadata in the file system, you can
    var $object_index_storage = "sql";
    var $new_objects_storage = "sql";
    var $object_export_dir = "xmldata/exports/";
    var $private_cache = "cache/";
    var $attachment_store = "attachments/";
    var $attachment_store_url = "attachments/";


    //0 -> none, 1 -> definitely malicious attempts, 2 -> all forbidden input
    //log settings
    var $security_recording_level = 1;
    var $security_ip_recording = 1; //log malicious attempts

    // *** Important Note for Debugging ****
    // For the value of user_error_reporting set manually in the file here to override
    // the value in the database, you must set the override to true. Otherwise leave it at false.
    var $system_file_override = true;
    var $user_error_reporting = 8;//0, 1, 2 upto 8. (2 is for debugging) See constants.inc

    //html and code input settings
    var $allow_rich_content_in_summary = true;
    var $allow_rich_content_in_story = true;
    var $allow_rich_content_in_comment = true;
    var $allow_bb_code = false;
    var $maximum_allowed_bbed_text = 200;//to stop everything in bold or complete quotes of posts
    var $enforce_strict_tag_closing = true;
    var $force_correct_html_input = true;
    var $force_correct_html_object_input = true;


    var $timezone_offset = 0;
    var $prepopulate_editor_details = true;

    var $editor_max_attachments;
    var $editor_max_summary;
    var $editor_max_contents;
    var $editor_max_comment;
    var $user_max_summary;
    var $user_max_contents;
    var $user_max_comment;
    var $story_max_attachments;
    var $comment_max_attachments;

    var $image_attachment_max_size;
    var $image_attachment_max_width;
    var $video_attachment_max_size;
    var $audio_attachment_max_size;
    var $miscellaneous_attachment_max_size;

    var $allow_story_image_attachments;
    var $allow_story_video_attachments;
    var $allow_story_audio_attachments;
    var $allow_story_miscellaneous_attachments;

    var $allow_comment_image_attachments;
    var $allow_comment_video_attachments;
    var $allow_comment_audio_attachments;
    var $allow_comment_miscellaneous_attachments;

    var $audio_player_installed;
    var $memory_mgmt_installed;
    var $memory_mgmt_activated;
    var $memory_mgmt_activate_time; // Switch on time.

    var $notification_from_email_address;
    var $notification_to_email_address;
    var $notification_replyto_email_address;
    var $contact_email_address;

    var $site_short_name;
    var $site_long_name;
    var $site_url;
    var $site_description;
    var $site_keywords;

    var $rss_query_cache_expiry;

    var $auto_cache_objects;
    var $use_live_objects;
    var $allow_live_objects;

    var $default_strftime_format;
    var $front_page_id;
    var $edit_locking_time;

    var $articleview_preview_story;

    var $featured_image;

    var $mono_lingual_filter;

    var $adminpage_obj_id;

    var $banned_ips;
    var $banned_referers;
    var $redirect_banned_url;
    // Banned spam bot lists. First is block on sight, second is block when they POST
    var $spam_ip_list;
    var $spam_post_ip_list;

    //pre-built image configuraions
    var $newswire_thumbnails_enabled;
    var $story_summary_thumbnail;
    //var $newswire_bar_thumbnail;
    var $rss_bar_thumbnail;

    // Added for extra traceability
    var $publish_monitor_began;
    var $publish_monitor_due_off;

    var $lastindex_generated;


    function SystemConfig()
    {
        // These are defined in the other systemconfig.inc file in the config directory.
        global $oscailt_basic_config;
        foreach($oscailt_basic_config as $k => $v){
            $this->$k=$v;
        }


    }

   function storyAttachmentsAllowed()
   {
       if($this->allow_story_image_attachments ||
          $this->allow_story_video_attachments ||
          $this->allow_story_audio_attachments ||
          $this->allow_story_miscellaneous_attachments) return true;
       else return false;
   }

   function commentAttachmentsAllowed()
   {
       if($this->allow_comment_image_attachments ||
          $this->allow_comment_video_attachments ||
          $this->allow_comment_audio_attachments ||
          $this->allow_comment_miscellaneous_attachments) return true;
       else return false;
   }

   function updateMemoryDate()
   {
       global $prefix, $dbconn;
       $result = sql_query("DELETE FROM ".$prefix."_configuration where name='memory_mgmt_activate_time'", $dbconn, 1);
       checkForError($result);
       $this->saveConfigItem("memory_mgmt_activate_time",$this->memory_mgmt_activate_time);
   }

   function saveMemoryState()
   {
       global $prefix, $dbconn;
       $result = sql_query("DELETE FROM ".$prefix."_configuration where name='memory_mgmt_activated'", $dbconn, 1);
       checkForError($result);
       $result = sql_query("DELETE FROM ".$prefix."_configuration where name='memory_mgmt_activate_time'", $dbconn, 1);
       checkForError($result);

       $this->saveBooleanConfigItem("memory_mgmt_activated",$this->memory_mgmt_activated);
       $this->saveConfigItem("memory_mgmt_activate_time",$this->memory_mgmt_activate_time);
   }
  
   function save()
   {
       global $prefix, $dbconn;
       $this->delete();
       $this->saveConfigItem("timezone_offset",$this->timezone_offset);
       $this->saveConfigItem("attachment_store",$this->attachment_store);
       $this->saveConfigItem("attachment_store_url",$this->attachment_store_url);
       $this->saveConfigItem("editor_max_attachments",$this->editor_max_attachments);
       $this->saveConfigItem("story_max_attachments",$this->story_max_attachments);
       $this->saveConfigItem("comment_max_attachments",$this->comment_max_attachments);
       $this->saveConfigItem("user_max_comment",$this->user_max_comment);
       $this->saveConfigItem("user_max_summary",$this->user_max_summary);
       $this->saveConfigItem("user_max_contents",$this->user_max_contents);
       $this->saveConfigItem("editor_max_comment",$this->editor_max_comment);
       $this->saveConfigItem("editor_max_summary",$this->editor_max_summary);
       $this->saveConfigItem("editor_max_contents",$this->editor_max_contents);

       $this->saveConfigItem("image_attachment_max_size",$this->image_attachment_max_size);
       $this->saveConfigItem("image_attachment_max_width",$this->image_attachment_max_width);
       $this->saveConfigItem("video_attachment_max_size",$this->video_attachment_max_size);
       $this->saveConfigItem("audio_attachment_max_size",$this->audio_attachment_max_size);
       $this->saveConfigItem("miscellaneous_attachment_max_size",$this->miscellaneous_attachment_max_size);
       $this->saveBooleanConfigItem("allow_story_image_attachments",$this->allow_story_image_attachments);
       $this->saveBooleanConfigItem("allow_story_video_attachments",$this->allow_story_video_attachments);
       $this->saveBooleanConfigItem("allow_story_audio_attachments",$this->allow_story_audio_attachments);
       $this->saveBooleanConfigItem("allow_story_miscellaneous_attachments",$this->allow_story_miscellaneous_attachments);
       $this->saveBooleanConfigItem("allow_comment_image_attachments",$this->allow_comment_image_attachments);
       $this->saveBooleanConfigItem("allow_comment_video_attachments",$this->allow_comment_video_attachments);
       $this->saveBooleanConfigItem("allow_comment_audio_attachments",$this->allow_comment_audio_attachments);
       $this->saveBooleanConfigItem("allow_comment_miscellaneous_attachments",$this->allow_comment_miscellaneous_attachments);
       $this->saveBooleanConfigItem("audio_player_installed",$this->audio_player_installed);
       $this->saveBooleanConfigItem("memory_mgmt_installed",$this->memory_mgmt_installed);
       $this->saveBooleanConfigItem("memory_mgmt_activated",$this->memory_mgmt_activated);
       $this->saveBooleanConfigItem("memory_mgmt_activate_time",$this->memory_mgmt_activate_time);

       $this->saveBooleanConfigItem("prepopulate_editor_details",$this->prepopulate_editor_details);
       $this->saveConfigItem("notification_from_email_address",$this->notification_from_email_address);
       $this->saveConfigItem("notification_to_email_address",$this->notification_to_email_address);
       $this->saveConfigItem("contact_email_address",$this->contact_email_address);
       $this->saveConfigItem("notification_replyto_email_address",$this->notification_replyto_email_address);
       $this->saveConfigItem("site_short_name",$this->site_short_name);
       $this->saveConfigItem("site_long_name",$this->site_long_name);
       $this->saveConfigItem("site_url",$this->site_url);
       $this->saveConfigItem("site_description",$this->site_description);
       $this->saveConfigItem("site_keywords",$this->site_keywords);
       $this->saveConfigItem("rss_query_cache_expiry",$this->rss_query_cache_expiry);

       $this->saveConfigItem("featured_image",$this->featured_image);
       $this->saveBooleanConfigItem("mono_lingual_filter",$this->mono_lingual_filter);
       $this->saveBooleanConfigItem("newswire_thumbnails_enabled",$this->newswire_thumbnails_enabled);

       $this->saveConfigItem("site_file_store_base",$this->site_file_store_base);
       $this->saveConfigItem("object_template_store",$this->object_template_store);
       $this->saveConfigItem("object_index_storage",$this->object_index_storage);
       $this->saveConfigItem("new_objects_storage",$this->new_objects_storage);
       $this->saveConfigItem("object_export_dir",$this->object_export_dir);
       $this->saveConfigItem("image_cache",$this->image_cache);
       $this->saveConfigItem("private_cache",$this->private_cache);

       $this->saveBooleanConfigItem("auto_cache_objects",$this->auto_cache_objects);
       $this->saveBooleanConfigItem("use_live_objects",$this->use_live_objects);
       $this->saveBooleanConfigItem("allow_live_objects",$this->allow_live_objects);

       $this->saveConfigItem("default_strftime_format",$this->default_strftime_format);
       $this->saveConfigItem("front_page_id",$this->front_page_id);
       $this->saveConfigItem("edit_locking_time",$this->edit_locking_time);
       $this->saveConfigItem("articleview_preview_story",$this->articleview_preview_story);
       $this->saveConfigItem("adminpage_obj_id",$this->adminpage_obj_id);
       $this->saveConfigItem("banned_ips",$this->banned_ips);
       $this->saveConfigItem("banned_referers",$this->banned_referers);
       $this->saveConfigItem("redirect_banned_url",$this->redirect_banned_url);
       $this->saveConfigItem("spam_ip_list",$this->spam_ip_list);
       $this->saveConfigItem("spam_post_ip_list",$this->spam_post_ip_list);

       $this->saveConfigItem("status_monitor_size_limit", $this->status_monitor_size_limit);
       $this->saveConfigItem("monitor_size_limit", $this->monitor_size_limit);
       $this->saveBooleanConfigItem("use_friendly_urls", $this->use_friendly_urls);
       $this->saveBooleanConfigItem("publish_monitor_enabled", $this->publish_monitor_enabled);
       $this->saveConfigItem("publish_monitor_began", $this->publish_monitor_began);
       $this->saveConfigItem("publish_monitor_due_off", $this->publish_monitor_due_off);
       $this->saveConfigItem("publish_comment_delay", $this->publish_comment_delay);
       $this->saveConfigItem("lastindex_generated", $this->lastindex_generated);

       $this->saveConfigItem("security_recording_level", $this->security_recording_level);
       $this->saveConfigItem("security_ip_recording", $this->security_ip_recording);
       $this->saveConfigItem("user_error_reporting", $this->user_error_reporting);
       $this->saveConfigItem("maximum_allowed_bbed_text", $this->maximum_allowed_bbed_text);
       $this->saveBooleanConfigItem("allow_rich_content_in_summary", $this->allow_rich_content_in_summary);
       $this->saveBooleanConfigItem("allow_rich_content_in_story", $this->allow_rich_content_in_story);
       $this->saveBooleanConfigItem("allow_rich_content_in_comment", $this->allow_rich_content_in_comment);
       $this->saveBooleanConfigItem("allow_bb_code", $this->allow_bb_code);
       $this->saveBooleanConfigItem("enforce_strict_tag_closing", $this->enforce_strict_tag_closing);
       $this->saveBooleanConfigItem("force_correct_html_input", $this->force_correct_html_input);
       $this->saveBooleanConfigItem("force_correct_html_object_input", $this->force_correct_html_object_input);
    }

    function saveConfigItem($name,$value)
    {
        global $prefix, $dbconn;
        $name=addslashes($name);
        $value=addslashes($value);
        checkForError(sql_query("INSERT INTO ".$prefix."_configuration values ('$name', '$value')", $dbconn, 1));
    }

    function saveBooleanConfigItem($name,$value)
    {
        global $prefix, $dbconn;
        $name=addslashes($name);
        if($value=="true" || $value=="1") checkForError(sql_query("INSERT INTO ".$prefix."_configuration values ('$name', '1')", $dbconn, 1));
        else checkForError(sql_query("INSERT INTO ".$prefix."_configuration values ('$name', NULL)", $dbconn, 1));
    }

    function load()
    {
        global $prefix, $dbconn;
        $result = sql_query("SELECT * from ".$prefix."_configuration", $dbconn, 1);
        checkForError($result);
        $name="";
        $value="";
        if(sql_num_rows( $result ) > 0)
        {
            for ($i=0; $i<sql_num_rows( $result ); $i++)
            {
                list($name, $value) = sql_fetch_row($result, $dbconn);
                if($name=="attachment_store") $this->attachment_store = $value;
                else if($name=="attachment_store_url") $this->attachment_store_url = $value;
                else if($name=="prepopulate_editor_details") $this->prepopulate_editor_details = $value;
                else if($name=="timezone_offset") $this->timezone_offset = $value;
                else if($name=="editor_max_contents") $this->editor_max_contents = $value;
                else if($name=="editor_max_summary") $this->editor_max_summary = $value;
                else if($name=="editor_max_comment") $this->editor_max_comment = $value;
                else if($name=="user_max_contents") $this->user_max_contents = $value;
                else if($name=="user_max_summary") $this->user_max_summary = $value;
                else if($name=="user_max_comment") $this->user_max_comment = $value;
                else if($name=="image_attachment_max_size") $this->image_attachment_max_size = $value;
                else if($name=="image_attachment_max_width") $this->image_attachment_max_width = $value;
                else if($name=="video_attachment_max_size") $this->video_attachment_max_size = $value;
                else if($name=="audio_attachment_max_size") $this->audio_attachment_max_size = $value;
                else if($name=="miscellaneous_attachment_max_size") $this->miscellaneous_attachment_max_size = $value;
                else if($name=="allow_story_image_attachments") $this->allow_story_image_attachments = $value;
                else if($name=="allow_story_video_attachments") $this->allow_story_video_attachments = $value;
                else if($name=="allow_story_audio_attachments") $this->allow_story_audio_attachments = $value;
                else if($name=="allow_story_miscellaneous_attachments") $this->allow_story_miscellaneous_attachments = $value;
                else if($name=="allow_comment_image_attachments") $this->allow_comment_image_attachments = $value;
                else if($name=="allow_comment_video_attachments") $this->allow_comment_video_attachments = $value;
                else if($name=="allow_comment_audio_attachments") $this->allow_comment_audio_attachments = $value;
                else if($name=="allow_comment_miscellaneous_attachments") $this->allow_comment_miscellaneous_attachments = $value;
                else if($name=="audio_player_installed") $this->audio_player_installed = $value;
                else if($name=="memory_mgmt_installed") $this->memory_mgmt_installed = $value;
                else if($name=="memory_mgmt_activated") $this->memory_mgmt_activated = $value;
                else if($name=="memory_mgmt_activate_time") $this->memory_mgmt_activate_time = $value;
                else if($name=="editor_max_attachments") $this->editor_max_attachments = $value;
                else if($name=="story_max_attachments") $this->story_max_attachments = $value;
                else if($name=="comment_max_attachments") $this->comment_max_attachments = $value;
                else if($name=="notification_from_email_address") $this->notification_from_email_address = $value;
                else if($name=="notification_to_email_address") $this->notification_to_email_address = $value;
                else if($name=="contact_email_address") $this->contact_email_address = $value;
                else if($name=="notification_replyto_email_address") $this->notification_replyto_email_address = $value;
                else if($name=="site_short_name") $this->site_short_name = $value;
                else if($name=="site_long_name") $this->site_long_name = $value;
                else if($name=="site_url") $this->site_url = $value;
                else if($name=="site_description") $this->site_description = $value;
                else if($name=="site_keywords") $this->site_keywords = $value;
                else if($name=="rss_query_cache_expiry") $this->rss_query_cache_expiry = $value;
                else if($name=="site_file_store_base") $this->site_file_store_base = $value;
                else if($name=="object_index_storage") $this->object_index_storage = $value;
                else if($name=="new_objects_storage") $this->new_objects_storage = $value;
                else if($name=="object_export_dir") $this->object_export_dir = $value;
                else if($name=="image_cache") $this->image_cache = $value;
                else if($name=="private_cache") $this->private_cache = $value;
                else if($name=="auto_cache_objects") $this->auto_cache_objects = $value;
                else if($name=="use_live_objects") $this->use_live_objects = $value;
                else if($name=="allow_live_objects") $this->allow_live_objects = $value;
                else if($name=="default_strftime_format") $this->default_strftime_format = $value;
                else if($name=="mono_lingual_filter") $this->mono_lingual_filter = $value;
                else if($name=="front_page_id") $this->front_page_id = $value;
                else if($name=="edit_locking_time") $this->edit_locking_time = $value;
                else if($name=="articleview_preview_story") $this->articleview_preview_story = $value;
                else if($name=="newswire_thumbnails_enabled") $this->newswire_thumbnails_enabled = $value;
                else if($name=="featured_image") $this->featured_image = $value;
                else if($name=="adminpage_obj_id") $this->adminpage_obj_id = $value;
                else if($name=="banned_ips") $this->banned_ips = $value;
                else if($name=="banned_referers") $this->banned_referers = $value;
                else if($name=="redirect_banned_url") $this->redirect_banned_url = $value;
                else if($name=="spam_ip_list") $this->spam_ip_list = $value;
                else if($name=="spam_post_ip_list") $this->spam_post_ip_list = $value;
                else if($name=="status_monitor_size_limit") $this->status_monitor_size_limit = $value;
                else if($name=="monitor_size_limit") $this->monitor_size_limit = $value;
                else if($name=="use_friendly_urls") $this->use_friendly_urls = $value;
                else if($name=="publish_monitor_enabled") $this->publish_monitor_enabled = $value;
                else if($name=="publish_monitor_began") $this->publish_monitor_began = $value;
                else if($name=="publish_monitor_due_off") $this->publish_monitor_due_off = $value;
                else if($name=="publish_comment_delay") $this->publish_comment_delay = $value;
                else if($name=="lastindex_generated") $this->lastindex_generated = $value;
                else if($name=="security_recording_level") $this->security_recording_level = $value;
                else if($name=="security_ip_recording") $this->security_ip_recording = $value;
                else if($name=="user_error_reporting" && $this->system_file_override==false) $this->user_error_reporting = $value;
                else if($name=="maximum_allowed_bbed_text") $this->maximum_allowed_bbed_text = $value;
                else if($name=="security_recording_level") $this->security_recording_level = $value;
                else if($name=="allow_rich_content_in_summary") $this->allow_rich_content_in_summary = $value;
                else if($name=="allow_rich_content_in_comment") $this->allow_rich_content_in_comment = $value;
                else if($name=="allow_rich_content_in_story") $this->allow_rich_content_in_story = $value;
                else if($name=="allow_bb_code") $this->allow_bb_code = $value;
                else if($name=="enforce_strict_tag_closing") $this->enforce_strict_tag_closing = $value;
                else if($name=="force_correct_html_input") $this->force_correct_html_input = $value;
                else if($name=="force_correct_html_object_input") $this->force_correct_html_object_input = $value;
            }
            $this->query_cache_store = $this->private_cache."querycache/";
            $this->object_name_store = $this->private_cache."objectnamecache/";
            $this->type_details_store = $this->private_cache."typedetailscache/";
            $this->html_cache_store = $this->private_cache."objectcache/";
            $this->rss_cache = $this->private_cache."rsscache/";
            $this->redirection_cache_store = $this->private_cache."redirectcache.inc";
            $this->publish_monitor_cache_store = $this->private_cache.'publishmonitorcache.inc';
        }
        else
        {
            reportError("No configuration data found in database");
        }
    }

    function delete()
    {
        global $prefix, $dbconn;
        $result = sql_query("DELETE FROM ".$prefix."_configuration", $dbconn, 1);
        checkForError($result);
    }


    function loadFormValues($form_variable ="")
    {
        global $editor_session;
	if ($form_variable != "" ) {
            // This is called from the Shared Memory Activation tab and will have a value 'on' when ticked.
            if(isset($_REQUEST["memory_mgmt_activated"]) && ($_REQUEST["memory_mgmt_activated"] == 1 || $_REQUEST["memory_mgmt_activated"] == "true" || $_REQUEST["memory_mgmt_activated"] == "on")) $this->memory_mgmt_activated = true;
            else $this->memory_mgmt_activated = false;
            return;
        }

        $this->editor_max_summary = $_REQUEST["editor_max_summary"];
        $this->editor_max_contents = $_REQUEST["editor_max_contents"];
        $this->editor_max_comment = $_REQUEST["editor_max_comment"];
        $this->user_max_summary = $_REQUEST["user_max_summary"];
        $this->user_max_contents = $_REQUEST["user_max_contents"];
        $this->user_max_comment = $_REQUEST["user_max_comment"];
        $this->timezone_offset = $_REQUEST["timezone_offset"];
        $this->attachment_store = $_REQUEST["attachment_store"];
        if($this->attachment_store{strlen($this->attachment_store) - 1} != "/")
        {
           $this->attachment_store .= "/";
        }
        $this->attachment_store_url = $_REQUEST["attachment_store_url"];
        if($this->attachment_store_url{strlen($this->attachment_store_url) - 1} != "/")
        {
          $this->attachment_store_url .= "/";
        }
        $this->image_attachment_max_size = $_REQUEST["image_attachment_max_size"];
        $this->image_attachment_max_width = $_REQUEST["image_attachment_max_width"];
        $this->video_attachment_max_size = $_REQUEST["video_attachment_max_size"];
        $this->audio_attachment_max_size = $_REQUEST["audio_attachment_max_size"];
        $this->miscellaneous_attachment_max_size = $_REQUEST["miscellaneous_attachment_max_size"];
        if(isset($_REQUEST["prepopulate_editor_details"])) $this->prepopulate_editor_details = true;
        else $this->prepopulate_editor_details = null;
        if(isset($_REQUEST["allow_story_image_attachments"])) $this->allow_story_image_attachments = true;
        else $this->allow_story_image_attachments = null;
        if(isset($_REQUEST["allow_story_video_attachments"])) $this->allow_story_video_attachments= true;
        else $this->allow_story_video_attachments = null;
        if(isset($_REQUEST["allow_story_audio_attachments"])) $this->allow_story_audio_attachments =true;
        else $this->allow_story_audio_attachments = null;
        if(isset($_REQUEST["allow_story_miscellaneous_attachments"])) $this->allow_story_miscellaneous_attachments =true;
        else $this->allow_story_miscellaneous_attachments = null;
        if(isset($_REQUEST["allow_comment_image_attachments"])) $this->allow_comment_image_attachments = true;
        else $this->allow_comment_image_attachments = null;
        if(isset($_REQUEST["allow_comment_video_attachments"])) $this->allow_comment_video_attachments = true;
        else $this->allow_comment_video_attachments = null;
        if(isset($_REQUEST["allow_comment_audio_attachments"])) $this->allow_comment_audio_attachments = true;
        else $this->allow_comment_audio_attachments = null;
        if(isset($_REQUEST["allow_comment_miscellaneous_attachments"])) $this->allow_comment_miscellaneous_attachments = true;
        else $this->allow_comment_miscellaneous_attachments = null;
        if(isset($_REQUEST["audio_player_installed"])) $this->audio_player_installed = true;
        else $this->audio_player_installed = false;
        if(isset($_REQUEST["memory_mgmt_installed"])) $this->memory_mgmt_installed = true;
        else $this->memory_mgmt_installed = false;
        if(isset($_REQUEST["memory_mgmt_activated"]) && ($_REQUEST["memory_mgmt_activated"] == 1 || $_REQUEST["memory_mgmt_activated"] == "true")) $this->memory_mgmt_activated = true;
        else $this->memory_mgmt_activated = false;

        $this->editor_max_attachments = $_REQUEST["editor_max_attachments"];
        $this->story_max_attachments = $_REQUEST["story_max_attachments"];
        $this->comment_max_attachments = $_REQUEST["comment_max_attachments"];
        $this->notification_from_email_address = $_REQUEST["notification_from_email_address"];
        $this->notification_to_email_address = $_REQUEST["notification_to_email_address"];
        $this->contact_email_address = $_REQUEST["contact_email_address"];
        $this->notification_replyto_email_address = $_REQUEST["notification_replyto_email_address"];
        $this->site_short_name = $_REQUEST["site_short_name"];
        $this->site_long_name = $_REQUEST["site_long_name"];
        $this->site_url = $_REQUEST["site_url"];
        if($this->site_url{strlen($this->site_url) - 1} == "/")
        {
            $this->site_url = substr($this->site_url, 0, -1);
        }
        $this->site_description = $_REQUEST["site_description"];
        $this->site_keywords = $_REQUEST["site_keywords"];
        $this->rss_query_cache_expiry = $_REQUEST["rss_query_cache_expiry"];
        $this->private_cache = $_REQUEST["private_cache"];
        if($this->private_cache{strlen($this->private_cache) - 1} != "/")
        {
            $this->private_cache .= "/";
        }
        $this->image_cache = $_REQUEST["image_cache"];
        if($this->image_cache{strlen($this->image_cache) - 1} == "/")
        {
            $this->image_cache = substr($this->image_cache, 0, -1);
        }
        $this->site_file_store_base = $_REQUEST["site_file_store_base"];
        if($this->site_file_store_base{strlen($this->site_file_store_base) - 1} != "/")
        {
            $this->site_file_store_base .= "/";
        }
        $this->object_index_storage = $_REQUEST["object_index_storage"];
        $this->new_objects_storage = $_REQUEST["new_objects_storage"];
        $this->object_export_dir = $_REQUEST["object_export_dir"];
        if($this->object_export_dir{strlen($this->object_export_dir) - 1} != "/")
        {
            $this->object_export_dir .= "/";
        }
        if(isset($_REQUEST["auto_cache_objects"])) $this->auto_cache_objects = true;
        else $this->auto_cache_objects = null;
        if(isset($_REQUEST["use_live_objects"])) $this->use_live_objects = true;
        else $this->use_live_objects = null;
        if(isset($_REQUEST["allow_live_objects"])) $this->allow_live_objects = true;
        else $this->allow_live_objects = null;
        $this->default_strftime_format = $_REQUEST["default_strftime_format"];
        $this->front_page_id = $_REQUEST["front_page_id"];
        $this->edit_locking_time = $_REQUEST["edit_locking_time"];
        $this->articleview_preview_story = $_REQUEST["articleview_preview_story"];
        $this->adminpage_obj_id = $_REQUEST["adminpage_obj_id"];
        if(isset($_REQUEST["mono_lingual_filter"])) $this->mono_lingual_filter = true;
        else $this->mono_lingual_filter = null;
        if(isset($_REQUEST["newswire_thumbnails_enabled"])) $this->newswire_thumbnails_enabled = true;
        else $this->newswire_thumbnails_enabled = null;
        $this->banned_ips = $_REQUEST["banned_ips"];
        $this->banned_referers = $_REQUEST["banned_referers"];
        $this->redirect_banned_url = $_REQUEST["redirect_banned_url"];
        $this->spam_ip_list = $_REQUEST["spam_ip_list"];
        $this->spam_post_ip_list = $_REQUEST["spam_post_ip_list"];
        if(isset($_REQUEST["use_friendly_urls"])) $this->use_friendly_urls = true;
        else $this->use_friendly_urls = null;
        if(isset($_REQUEST["publish_monitor_enabled"])) $this->publish_monitor_enabled = true;
        else $this->publish_monitor_enabled = null;
        $this->publish_comment_delay = $_REQUEST["publish_comment_delay"];
        $this->status_monitor_size_limit = $_REQUEST["status_monitor_size_limit"];
        $this->monitor_size_limit = $_REQUEST["monitor_size_limit"];
        $this->security_recording_level = $_REQUEST["security_recording_level"];
        $this->security_ip_recording = $_REQUEST["security_ip_recording"];
        $this->user_error_reporting = $_REQUEST["user_error_reporting"];
        $this->maximum_allowed_bbed_text = $_REQUEST["maximum_allowed_bbed_text"];
        if(isset($_REQUEST["allow_rich_content_in_summary"])) $this->allow_rich_content_in_summary = true;
        else $this->allow_rich_content_in_summary = null;
        if(isset($_REQUEST["allow_rich_content_in_story"])) $this->allow_rich_content_in_story = true;
        else $this->allow_rich_content_in_story = null;
        if(isset($_REQUEST["allow_rich_content_in_comment"])) $this->allow_rich_content_in_comment = true;
        else $this->allow_rich_content_in_comment = null;
        if(isset($_REQUEST["allow_bb_code"])) $this->allow_bb_code = true;
        else $this->allow_bb_code = null;
        if(isset($_REQUEST["enforce_strict_tag_closing"])) $this->enforce_strict_tag_closing = true;
        else $this->enforce_strict_tag_closing = null;
        if(isset($_REQUEST["force_correct_html_input"])) $this->force_correct_html_input = true;
        else $this->force_correct_html_input = null;
        if(isset($_REQUEST["force_correct_html_object_input"])) $this->force_correct_html_object_input = true;
        else $this->force_correct_html_object_input = null;
    }

}

?>
