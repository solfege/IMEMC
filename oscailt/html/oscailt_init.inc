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
$path_prefix = "";//this is a legacy thing I don't think it's ever been used

require_once($path_prefix."objects/systemconfig.inc");
require_once($path_prefix."objects/memorymgmt.inc");
require_once($path_prefix."objects/imagetool.inc");
require_once($path_prefix."objects/querycache.inc");
require_once($path_prefix."objects/sqllayer.inc");
require_once($path_prefix."objects/role.inc");
require_once($path_prefix."objects/language.inc");
require_once($path_prefix."objects/editorpermission.inc");
require_once($path_prefix."objects/editor.inc");
require_once($path_prefix."objects/editorsession.inc");
require_once($path_prefix."objects/preferencequery.inc");
require_once($path_prefix."objects/topic.inc");
require_once($path_prefix."objects/region.inc");
require_once($path_prefix."objects/type.inc");
require_once($path_prefix."objects/utilities.inc");
require_once($path_prefix."objects/clipboard.inc");
require_once($path_prefix."objects/redirector.inc");
require_once($path_prefix."objects/storyquery.inc");

require_once($path_prefix."objects/videos.inc");

//the system globals
$system_config = new SystemConfig();
$editor_session = new EditorSession();
$topicList= new TopicList();
$languageList = new LanguageList();
$regionList = new RegionList();
$typeList= new TypeList();
$user_prefs = new PreferenceQuery();
$query_cache = new QueryCache();
$redirectList = new RedirectList();
$system_config->load();

// Save many calls to time function for each request.
$current_hour = strftime("%H");

// This must be called as early as possible
GetSemaphoreId();

SharedMemoryUpdate("Main");

if ($_SERVER['REQUEST_METHOD'] == "POST") SharedMemoryUpdate("POST_Req");

if(isRequestBanned())
{
   redirectBannedRequest();
}

if(isRequestBannedBot())
{
   redirectBannedRequest(401);
}
UpdateMemoryHttpReferer();

$editor_session->start();
if($performance_test > 1)  markTime("session started and basic libraries loaded");

//variables which combine to set html meta tags
$pageFilterText = "";
$pageTitle = $system_config->site_short_name;
$pageDescription = $system_config->site_description;
$pageKeywords = loadSiteKeywords();
$encode_charset = "iso-8859-1";

//where things are stored -> used for phony directories in redirects
$site_file_store = "";
$graphics_store = "graphics/";
$style_sheet = "style.css";
$pseudo_directory_offset = "";

//two different language globals - user language is the user's chosen, or locale based language
//user language determines the language of the static elements etc.
//page language refers to the language of a particular article requested.
//Having two language globals enables viewing articles in foreign languages
//through the a native language interface.
if(isset($_GET["userlanguage"])){
   //get it from the code...
   $userLanguage = $languageList->getLanguageByCode($_GET["userlanguage"]);
   $_REQUEST['userlanguage'] = $_GET["userlanguage"];
}
elseif(isset($_POST['userlanguage']))
{
   $userLanguage = $languageList->getLanguageByCode($_POST["userlanguage"]);
}
elseif(isset($_REQUEST['userlanguage']))
{
   $userLanguage = $languageList->getLanguageByCode($_REQUEST["userlanguage"]);
}
if(!isset($userLanguage)){
   $userLanguage = $languageList->getAcceptLanguage();
}
if($userLanguage == null){
   $userLanguage = $languageList->getLanguageByCode($system_config->default_language_code);
}
if($userLanguage == null){
   $languages = $languageList->getfLanguages();
   $userLanguage = $languageList[0];
}
if($userLanguage == null){
   $userLanguage = new Language();
   $userLanguage->createFakeLanguage();
}

setClosestLocale();

if(!isset( $pageLanguage ) && !isset( $_REQUEST["language_id"] ) && isset( $_REQUEST["language"] ) )
{
   $pageLanguage = $languageList->getLanguageByName($_REQUEST["language"]);
   if($pageLanguage==NULL) $pageLanguage = $languageList->getLanguageByCode($_REQUEST["language"]);
}
else if(!isset( $pageLanguage ) && isset( $_REQUEST["language_id"] ) )
{
   $pageLanguage = $languageList->getLanguageByID($_REQUEST["language_id"]);
}
else if($system_config->mono_lingual_filter==true or (isset($_REQUEST['mono_lingual_filter']) and $_REQUEST['mono_lingual_filter'] == 'on'))
{
   $pageLanguage = $userLanguage;
}
if(!isset( $pageTopic ) && !isset($_REQUEST["topic_id"]) && isset($_REQUEST["topic"]) )
{
   $pageTopic = $topicList->getTopicByName($_REQUEST["topic"], $pageLanguage->language_id);
}
else if( !isset($pageTopic) && isset($_REQUEST["topic_id"]) )
{
   $pageTopic = $topicList->getTopicByID($_REQUEST["topic_id"], $userLanguage->language_id);
}
else if( !isset($pageTopic) && isset($_REQUEST["topic_name"]) )
{
   $pageTopic = $topicList->getTopicByName($_REQUEST["topic_name"], $userLanguage->language_id);
}

if(!isset($pageRegion) && !isset($_REQUEST["region_id"]) && isset( $_REQUEST["region"]) )
{
   $pageRegion = $regionList->getRegionByName($_REQUEST["region"], $pageLanguage->language_id);
}
else if( !isset($pageRegion) && isset($_REQUEST["region_id"]) )
{
   $pageRegion = $regionList->getRegionByID($_REQUEST["region_id"], $userLanguage->language_id);
}
else if( !isset($pageRegion) && isset($_REQUEST["region_name"]) )
{
   $pageRegion = $regionList->getRegionByName($_REQUEST["region_name"], $userLanguage->language_id);
}


if( !isset($pageType) && !isset($_REQUEST["type_id"]) && isset($_REQUEST["type"]) )
{
   $pageType = $typeList->getTypeByName($_REQUEST["type"], $pageLanguage->language_id);
}
else if( !isset($pageType)  && isset($_REQUEST["type_id"]) )
{
   $pageType = $typeList->getTypeByID($_REQUEST["type_id"], $userLanguage->language_id);
}
else if( !isset($pageType)  && isset($_REQUEST["type_name"]) )
{
   $pageType = $typeList->getTypeByName($_REQUEST["type_name"], $userLanguage->language_id);
}

makePageFilterText();
if($performance_test > 1)  markTime("initiallisation completed");

?>
