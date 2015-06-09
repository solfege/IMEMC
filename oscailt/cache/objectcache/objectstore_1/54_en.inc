<?php global $pseudo_directory_offset; $site_file_store = $pseudo_directory_offset."attachments/sites/default/"; 
$oscailt_page_setting = Array("site_module_settings" => Array("siteobject" => "1", "newswireobject" => "2126", "articleobject" => "53", "featureobject" => "54", "galleryobject" => "64", "searchobject" => "2212", "archiveobject" => "55", "contactobject" => "84", "commentsobject" => "", "publishobject" => "57", "eventsobject" => "58"), "site_mail_settings" => Array("to" => "news@imemc.org", "from" => "site@imemc.org", "subject_prefix" => "Article edit:", "replyto" => "news@imemc.org", "level" => "none"), "site_filters" => Array("topic" => Array("0" => "*"), "region" => Array("0" => "20", "1" => "19", "2" => "18", "3" => "17", "4" => "16", "5" => "15", "6" => "14", "7" => "13", "8" => "12", "9" => "11", "10" => "10", "11" => "9", "12" => "8", "13" => "7", "14" => "6", "15" => "5", "16" => "4", "17" => "3", "18" => "2", "19" => "1"), "language" => Array("0" => "en"), "type" => Array("0" => "*"), "author" => ""), "obj_id" => "54", "obj_type" => "FEATURE", "site_id" => "1", "page_filters" => Array("topic" => Array("0" => "76", "1" => "35", "2" => "33", "3" => "32", "4" => "15", "5" => "24", "6" => "23", "7" => "22", "8" => "21", "9" => "25", "10" => "26", "11" => "10", "12" => "30", "13" => "16", "14" => "28", "15" => "27", "16" => "3", "17" => "20", "18" => "31"), "region" => Array("0" => "20", "1" => "19", "2" => "18", "3" => "17", "4" => "16", "5" => "15", "6" => "14", "7" => "13", "8" => "12", "9" => "11", "10" => "10", "11" => "9", "12" => "8", "13" => "7", "14" => "6", "15" => "5", "16" => "4", "17" => "3", "18" => "2", "19" => "1", "20" => "20", "21" => "19", "22" => "18", "23" => "17", "24" => "16", "25" => "15", "26" => "14", "27" => "13", "28" => "12", "29" => "11", "30" => "10", "31" => "9", "32" => "8", "33" => "7", "34" => "6", "35" => "5", "36" => "4", "37" => "3", "38" => "2", "39" => "1"), "language" => Array("0" => "en", "1" => "en"), "type" => Array("0" => "1"), "author" => Array("0" => "")));
$oscailt_story_query = new StoryQuery();
$oscailt_story_query->loadFormValues();
$PAGE_LAYOUT_ID = 60; ?>
<?
         addToPageDescription("The IMEMC is a media collective. We are independent journalists producing and distributing the authentic voices of the people.  We are located in Beit Sahour, in the Occupied West Bank, and we welcome new volunteers and contributors.");
      addToPageTitle("Features");
         addToPageDescription("Featured Stories."); ?>

<!-- <features obj id 54> -->
<?php require_once "objects/indyruntime/newswiredisplay.inc";
require_once "objects/indyruntime/featuredisplay.inc";

         $site_module = new featureDisplay(54);
         $site_module->setFilters(Array("76", "35", "33", "32", "15", "24", "23", "22", "21", "25", "26", "10", "30", "16", "28", "27", "3", "20", "31"), Array("20", "19", "18", "17", "16", "15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1", "20", "19", "18", "17", "16", "15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1", "20", "19", "18", "17", "16", "15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1"), Array("en", "en", "en"), Array("1"), Array(""));
         $object_display_settings = (Array("siteobject" => "1", "newswireobject" => "2126", "articleobject" => "53", "featureobject" => "54", "galleryobject" => "64", "searchobject" => "2212", "archiveobject" => "55", "contactobject" => "84", "commentsobject"=> "0", "publishobject" => "57", "eventsobject" => "58"));
         $site_module->setSiteSettings($object_display_settings);
      $site_module->setAdminOptions(Array("title" => "features", "header" => "", "footer" => "", "emptyresultmsg" => "There is no feature story at the moment, however, soon there will be a new one. Thanks for bearing with us.", "read_more_text" => "Full Story >>", "filter_author" => "", "editor_note_text" => "Editor Note", "self_link_text" => "This item contains a link to it\'s own attachment in the html content, so it will not be displayed again in the attachment list that the user sees. You may still hide or delete this attachment but it must be manually removed from the html content.", "relatedlink_text" => "External Link:", "promoted" => "false", "ignorehidden" => "true", "respectsticky" => "true", "allowmultis" => Array("0" => "clip", "1" => "unstick", "2" => "hide", "3" => "unhide", "4" => "lock", "5" => "unlock")));
	$site_module->setUserOptions(Array("pagelimit" => "1", "storydate_format" => "", "video_mode" => "", "hiddenonly" => "false", "nohidden" => "false", "lockedonly" => "false", "nolocked" => "false", "nosticky" => "false", "stickyonly" => "false", "upcomingevents" => "false", "statusonly" => "false", "includeicons" => "false", "includephoto" => "false", "showhidden" => "true", "showlocked" => "true", "showsticky" => "true", "showstatus" => "false", "usestatusicons" => "true", "includerelatedlink" => "true", "include_embed_video" => "false", "include_embed_audio" => "false", "usealternatestyles" => "false", "include_translations" => "false", "headernavigation" => Array(), "footernavigation" => Array(), "subtitledetails" => Array("0" => "date"), "enddetails" => Array(), "time_posted_lower_limit" => "1417305540", "time_posted_upper_limit" => "0"));

         $site_module->processRequest();
       ?>
<!-- </features obj id 54> -->


         <?php
          global $editor_session, $pseudo_directory_offset;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjfeature", 1)))
          {
          ?>
             <br>
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=feature&amp;target_id=54&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0  align="left" title="Edit Featurewire Module [features] Configuration" alt="Edit Featurewire Module [features] Configuration" src="<?php echo $pseudo_directory_offset?>graphics/mediaicons/32x32/film.png" /></a>
             
          <?
          }
          ?>
          