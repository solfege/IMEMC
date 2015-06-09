<?php global $pseudo_directory_offset; $site_file_store = $pseudo_directory_offset."attachments/sites/default/"; 
$oscailt_page_setting = Array("site_module_settings" => Array("siteobject" => "1", "newswireobject" => "2126", "articleobject" => "53", "featureobject" => "54", "galleryobject" => "64", "searchobject" => "2212", "archiveobject" => "55", "contactobject" => "84", "commentsobject" => "", "publishobject" => "57", "eventsobject" => "58"), "site_mail_settings" => Array("to" => "news@imemc.org", "from" => "site@imemc.org", "subject_prefix" => "Article edit:", "replyto" => "news@imemc.org", "level" => "none"), "site_filters" => Array("topic" => Array("0" => "*"), "region" => Array("0" => "20", "1" => "19", "2" => "18", "3" => "17", "4" => "16", "5" => "15", "6" => "14", "7" => "13", "8" => "12", "9" => "11", "10" => "10", "11" => "9", "12" => "8", "13" => "7", "14" => "6", "15" => "5", "16" => "4", "17" => "3", "18" => "2", "19" => "1"), "language" => Array("0" => "en"), "type" => Array("0" => "*"), "author" => ""), "obj_id" => "50", "obj_type" => "NEWSWIRE", "site_id" => "1", "page_filters" => Array("topic" => Array("0" => "*"), "region" => Array("0" => "20", "1" => "19", "2" => "18", "3" => "17", "4" => "16", "5" => "15", "6" => "14", "7" => "13", "8" => "12", "9" => "11", "10" => "10", "11" => "9", "12" => "8", "13" => "7", "14" => "6", "15" => "5", "16" => "4", "17" => "3", "18" => "2", "19" => "1"), "language" => Array("0" => "en"), "type" => Array("0" => "22", "1" => "12", "2" => "11", "3" => "10", "4" => "1", "5" => "2", "6" => "3", "7" => "9", "8" => "8"), "author" => Array("0" => "")));
$oscailt_story_query = new StoryQuery();
$oscailt_story_query->loadFormValues();
$PAGE_LAYOUT_ID = 59; ?>
<?
         addToPageDescription("The IMEMC is a media collective. We are independent journalists producing and distributing the authentic voices of the people.  We are located in Beit Sahour, in the Occupied West Bank, and we welcome new volunteers and contributors.");
      addToPageTitle("Newswire");
         addToPageDescription("This newswire covers every story posted to the site"); ?>

<!-- <newswire obj id 50> -->
<?php require_once "objects/indyruntime/newswiredisplay.inc";

         $site_module = new newswireDisplay(50);
         $site_module->setFilters(Array("*"), Array("20", "19", "18", "17", "16", "15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1", "20", "19", "18", "17", "16", "15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1"), Array("en", "en"), Array("22", "12", "11", "10", "1", "2", "3", "9", "8"), Array(""));
         $object_display_settings = (Array("siteobject" => "1", "newswireobject" => "2126", "articleobject" => "53", "featureobject" => "54", "galleryobject" => "64", "searchobject" => "2212", "archiveobject" => "55", "contactobject" => "84", "commentsobject"=> "0", "publishobject" => "57", "eventsobject" => "58"));
         $site_module->setSiteSettings($object_display_settings);
      $site_module->setAdminOptions(Array("title" => "all content posted to the site", "header" => "", "footer" => "", "emptyresultmsg" => "Sorry, no stories matched your search, maybe try again with different settings.", "read_more_text" => "read full story  / add a comment", "filter_author" => "", "editor_note_text" => "", "self_link_text" => "", "relatedlink_text" => "related link", "promoted" => "false", "ignorehidden" => "false", "respectsticky" => "false", "allowmultis" => Array("0" => "clip", "1" => "unstick", "2" => "hide", "3" => "unhide", "4" => "lock", "5" => "unlock")));
	$site_module->setUserOptions(Array("pagelimit" => "14", "storydate_format" => "%A %B %d, %Y %H:%M", "video_mode" => "", "hiddenonly" => "false", "nohidden" => "false", "lockedonly" => "false", "nolocked" => "false", "nosticky" => "false", "stickyonly" => "false", "upcomingevents" => "false", "statusonly" => "false", "includeicons" => "true", "includephoto" => "true", "showhidden" => "true", "showlocked" => "true", "showsticky" => "true", "showstatus" => "false", "usestatusicons" => "true", "includerelatedlink" => "true", "include_embed_video" => "false", "include_embed_audio" => "false", "usealternatestyles" => "false", "include_translations" => "false", "headernavigation" => Array("0" => "context", "1" => "prevnext"), "footernavigation" => Array("0" => "context", "1" => "prevnext", "2" => "otherviews"), "subtitledetails" => Array("0" => "context", "1" => "date", "2" => "attachments"), "enddetails" => Array("0" => "author"), "time_posted_lower_limit" => "0", "time_posted_upper_limit" => "0"));

         $site_module->processRequest();
       ?>
<!-- </newswire obj id 50> -->

<?php getPageLanguageOptions("en", Array("0" => "en", "1" => "sp"), "50"); ?>
         <?php
          global $editor_session, $pseudo_directory_offset;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjnewswire", 1)))
          {
          ?>
             <br>
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=newswire&amp;target_id=50&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0  align="left" title="Edit Newswire Module [newswire] Configuration" alt="Edit Newswire Module [newswire] Configuration" src="<?php echo $pseudo_directory_offset?>graphics/mediaicons/32x32/print.png" /></a>
             
          <?
          }
          ?>
          