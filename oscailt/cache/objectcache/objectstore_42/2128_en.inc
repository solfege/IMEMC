<?php global $pseudo_directory_offset; $site_file_store = $pseudo_directory_offset."attachments/sites/default/"; 
$oscailt_page_setting = Array("site_module_settings" => Array("siteobject" => "1", "newswireobject" => "2126", "articleobject" => "53", "featureobject" => "54", "galleryobject" => "64", "searchobject" => "2212", "archiveobject" => "55", "contactobject" => "84", "commentsobject" => "", "publishobject" => "57", "eventsobject" => "58"), "site_mail_settings" => Array("to" => "news@imemc.org", "from" => "site@imemc.org", "subject_prefix" => "Article edit:", "replyto" => "news@imemc.org", "level" => "none"), "site_filters" => Array("topic" => Array("0" => "*"), "region" => Array("0" => "20", "1" => "19", "2" => "18", "3" => "17", "4" => "16", "5" => "15", "6" => "14", "7" => "13", "8" => "12", "9" => "11", "10" => "10", "11" => "9", "12" => "8", "13" => "7", "14" => "6", "15" => "5", "16" => "4", "17" => "3", "18" => "2", "19" => "1"), "language" => Array("0" => "en"), "type" => Array("0" => "*"), "author" => ""), "obj_id" => "2128", "obj_type" => "NEWSWIRE", "site_id" => "1", "page_filters" => Array("topic" => Array("0" => "36", "1" => "35", "2" => "34", "3" => "33", "4" => "32", "5" => "15", "6" => "24", "7" => "23", "8" => "22", "9" => "21", "10" => "25", "11" => "26", "12" => "10", "13" => "30", "14" => "16", "15" => "28", "16" => "27", "17" => "3", "18" => "20", "19" => "31"), "region" => Array("0" => "20", "1" => "19", "2" => "18", "3" => "17", "4" => "16", "5" => "15", "6" => "14", "7" => "13", "8" => "12", "9" => "11", "10" => "10", "11" => "9", "12" => "8", "13" => "7", "14" => "6", "15" => "5", "16" => "4", "17" => "3", "18" => "2", "19" => "1"), "language" => Array("0" => "en"), "type" => Array("0" => "10"), "author" => Array("0" => "")));
$oscailt_story_query = new StoryQuery();
$oscailt_story_query->loadFormValues();
$PAGE_LAYOUT_ID = 1868; ?>
<?
         addToPageDescription("The IMEMC is a media collective. We are independent journalists producing and distributing the authentic voices of the people.  We are located in Beit Sahour, in the Occupied West Bank, and we welcome new volunteers and contributors.");
      addToPageTitle("Audio");
         addToPageDescription("IMEMC audio reports"); ?>

<!-- <audio obj id 2128> -->
<?php require_once "objects/indyruntime/newswiredisplay.inc";

         $site_module = new newswireDisplay(2128);
         $site_module->setFilters(Array("36", "35", "34", "33", "32", "15", "24", "23", "22", "21", "25", "26", "10", "30", "16", "28", "27", "3", "20", "31"), Array("20", "19", "18", "17", "16", "15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1", "20", "19", "18", "17", "16", "15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1"), Array("en", "en"), Array("10"), Array(""));
         $object_display_settings = (Array("siteobject" => "1", "newswireobject" => "2126", "articleobject" => "53", "featureobject" => "54", "galleryobject" => "64", "searchobject" => "2212", "archiveobject" => "55", "contactobject" => "84", "commentsobject"=> "0", "publishobject" => "57", "eventsobject" => "58"));
         $site_module->setSiteSettings($object_display_settings);
      $site_module->setAdminOptions(Array("title" => "Latest Audio", "header" => "The IMEMC produces 4-5-minute daily(Palestine Today) and 10-15-minute weekly (This Week in Palestine) audio newscasts. These are free for re-use on your station or webstream, and are available for download each day, Monday through Friday, by 4 pm GMT (1 pm EST).", "footer" => "", "emptyresultmsg" => "no audio posted", "read_more_text" => "Full Report", "filter_author" => "", "editor_note_text" => "", "self_link_text" => "", "relatedlink_text" => "", "promoted" => "false", "ignorehidden" => "false", "respectsticky" => "false", "allowmultis" => Array("0" => "clip", "1" => "unstick", "2" => "hide", "3" => "unhide", "4" => "unlock")));
	$site_module->setUserOptions(Array("pagelimit" => "12", "storydate_format" => "%A %B %d, %Y", "video_mode" => "", "hiddenonly" => "false", "nohidden" => "false", "lockedonly" => "false", "nolocked" => "false", "nosticky" => "false", "stickyonly" => "false", "upcomingevents" => "false", "statusonly" => "false", "includeicons" => "false", "includephoto" => "false", "showhidden" => "true", "showlocked" => "true", "showsticky" => "true", "showstatus" => "false", "usestatusicons" => "true", "includerelatedlink" => "false", "include_embed_video" => "false", "include_embed_audio" => "true", "usealternatestyles" => "false", "include_translations" => "false", "headernavigation" => Array(), "footernavigation" => Array("0" => "prevnext", "1" => "otherviews"), "subtitledetails" => Array("0" => "context", "1" => "date", "2" => "author", "3" => "attachments"), "enddetails" => Array(), "time_posted_lower_limit" => "0", "time_posted_upper_limit" => "0"));

         $site_module->processRequest();
       ?>
<!-- </audio obj id 2128> -->

<?php getPageLanguageOptions("en", Array("0" => "en", "1" => "sp"), "2128"); ?>
         <?php
          global $editor_session, $pseudo_directory_offset;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjnewswire", 1)))
          {
          ?>
             <br>
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=newswire&amp;target_id=2128&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0  align="left" title="Edit Newswire Module [audio] Configuration" alt="Edit Newswire Module [audio] Configuration" src="<?php echo $pseudo_directory_offset?>graphics/mediaicons/32x32/print.png" /></a>
             
          <?
          }
          ?>
          