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

//general oscailt config values
$oscailt_basic_config = array();

//software version configuration
$oscailt_basic_config['software_name']="Oscailt";
$oscailt_basic_config['software_version']="3.3";

//special article types
$oscailt_basic_config['event_type_id']=5;
$oscailt_basic_config['feature_type_id']=1;

//cookie and session life times
$oscailt_basic_config['cookie_life']=365;
$oscailt_basic_config['session_life'] = 3600; //the length of time in seconds for an editor's session to time out

//sql caching
$oscailt_basic_config['sql_query_caching_enabled'] = true;
$oscailt_basic_config['sql_query_caching_healing_timeout'] = 300;
$oscailt_basic_config['remote_image_query_caching_healing_timeout'] = 900;
$oscailt_basic_config['width_height_on_all_image_tags'] = true;

//logging
$oscailt_basic_config['log_store'] = "logs/";
$oscailt_basic_config['logging_enabled'] = true;
//higher for more logging info, lower for less. lower than 1 is a no-no
$oscailt_basic_config['debug_level'] = 5;

//storing xml data
$oscailt_basic_config['xml_store'] = "xmldata/";
$oscailt_basic_config['object_import_dir'] = $oscailt_basic_config['xml_store']."imports/";
$oscailt_basic_config['xmltypedef_dir'] = $oscailt_basic_config['xml_store']."indydatatypes/";
$oscailt_basic_config['object_template_store'] = $oscailt_basic_config['xml_store']."templates/";
$oscailt_basic_config['item_action_cache_store'] = $oscailt_basic_config['xml_store']."itemcache/";

//exporting data objects and caching them
$oscailt_basic_config['objects_per_cache_directory'] = 50;
$oscailt_basic_config['back_up_data_object_index'] = true;
//the directory path to the data modules directory
$oscailt_basic_config['indyobject_code_dir'] = "objects/indyobjects/indydataclasses/";


$oscailt_basic_config['default_language_code'] = 'en';

//disallow publication from invalid ip addresses
$oscailt_basic_config['disallow_ipless_publication'] = false;

//urls over this length will be concatenated with ...
$oscailt_basic_config['max_unshortened_tag_length'] = 60;

//support remapping of oscailt 1.0 urls? (only if you have upgraded from 1->2->3
$oscailt_basic_config['support_v1_url_remaps'] = true;

//in author searches, use the %LIKE% mysql search?
$oscailt_basic_config['use_vague_author_name'] = false;

//allow content to include oscailt macros
$oscailt_basic_config['allow_oscailt_macros'] = true;

//show options for different language versions of pages
$oscailt_basic_config['show_page_translations_box'] = true;


//late additions - should be in newswire module.
$oscailt_basic_config['use_only_featured_photo_thumbnails_in_headlines'] = false;
$oscailt_basic_config['full_photo_link_text'] = "Click on image to see full-sized version";

//edit lock rule -> can you keep an item locked for 2 periods without saving it?
$oscailt_basic_config['forbid_consecutive_locks_without_save'] = false;

//how long to wait for rss feeds.
$oscailt_basic_config['rss_fetch_timeout'] = 5;

//the permissions to create directories with.
$oscailt_basic_config['default_writable_directory_permissions'] = 0777;

//how many is the maximum number of user status and user messages stored at any time in the user status
$oscailt_basic_config['status_monitor_size_limit'] = 40;
//how many is the maximum number of ip addresses stored at any time in the publish monitor
$oscailt_basic_config['monitor_size_limit'] = 20;

//publish delay time for comments. If greater than zero then it is in effect.
$oscailt_basic_config['publish_comment_delay'] = 0;

//use icons for address, phone, etc.
$oscailt_basic_config['use_icons_for_author_details'] = true;
$oscailt_basic_config['use_icons_for_newswire_details'] = false;

//some basic image configurations (could be moved to object configurations)
$oscailt_basic_config['story_summary_thumbnail'] = new ImageConfig(100,75,true,true,5,array(0xFF, 0xFF, 0xFF),0,0,0,0,0);
$oscailt_basic_config['story_headline_thumbnail'] = new ImageConfig(40,50,true,true,2,array(0xFF, 0xFF, 0xFF),0,0,0,0,0);
$oscailt_basic_config['newswire_bar_thumbnail'] = new ImageConfig(140,200,false,false,5,array(0xFF, 0xFF, 0xFF),0,0,0,0,0);
$oscailt_basic_config['rss_bar_thumbnail'] = new ImageConfig(100,200,false,false,5,array(0xFF, 0xFF, 0xFF),0,0,0,0,0);
$oscailt_basic_config['max_feature_imagesize'] = 20;
$oscailt_basic_config['error_image'] = "graphics/error.png";

//anything you don't find here, is probably in one of the other config files, or settable through the edit configuration page.


//setting global debugging info..

//settings for debugging
$last_sql_query="";
$sql_debug=0;
$query_count=0; // counter for counting queries per request.
$query_cached_count=0; // counter for counting queries per request that were cached.
$performance_test=0; //0 for production, 1 for page time, 2 for all stages
error_reporting(0);//for production - set to E_ALL for debug / devel
if($performance_test)
{
   $time_list = array(array("start", microtime()));
}
?>
