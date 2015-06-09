<?php global $pseudo_directory_offset; $site_file_store = $pseudo_directory_offset."attachments/sites/default/"; 
$oscailt_page_setting = Array("site_module_settings" => Array("siteobject" => "1", "newswireobject" => "2126", "articleobject" => "53", "featureobject" => "54", "galleryobject" => "64", "searchobject" => "2212", "archiveobject" => "55", "contactobject" => "84", "commentsobject" => "", "publishobject" => "57", "eventsobject" => "58"), "site_mail_settings" => Array("to" => "news@imemc.org", "from" => "site@imemc.org", "subject_prefix" => "Article edit:", "replyto" => "news@imemc.org", "level" => "none"), "site_filters" => Array("topic" => Array("0" => "*"), "region" => Array("0" => "20", "1" => "19", "2" => "18", "3" => "17", "4" => "16", "5" => "15", "6" => "14", "7" => "13", "8" => "12", "9" => "11", "10" => "10", "11" => "9", "12" => "8", "13" => "7", "14" => "6", "15" => "5", "16" => "4", "17" => "3", "18" => "2", "19" => "1"), "language" => Array("0" => "en"), "type" => Array("0" => "*"), "author" => ""), "obj_id" => "2126", "obj_type" => "NEWSWIRE", "site_id" => "1", "page_filters" => Array("topic" => Array("0" => "77", "1" => "75", "2" => "74", "3" => "73", "4" => "72", "5" => "71", "6" => "70", "7" => "69", "8" => "68", "9" => "64", "10" => "63", "11" => "62", "12" => "61", "13" => "60", "14" => "59", "15" => "58", "16" => "57", "17" => "56", "18" => "65", "19" => "66", "20" => "67"), "type" => Array("0" => "18"), "author" => Array("0" => "")));
$oscailt_story_query = new StoryQuery();
$oscailt_story_query->loadFormValues();
$PAGE_LAYOUT_ID = 59; ?>
<?
         addToPageDescription("The IMEMC is a media collective. We are independent journalists producing and distributing the authentic voices of the people.  We are located in Beit Sahour, in the Occupied West Bank, and we welcome new volunteers and contributors.");
      addToPageTitle("Noticias Del IMEMC");
         addToPageDescription("International Middle East Media Center"); ?>

<!-- <Noticias del IMEMC obj id 2126> -->
<?php require_once "objects/indyruntime/newswiredisplay.inc";

         $site_module = new newswireDisplay(2126);
         $site_module->setFilters(Array("77", "75", "74", "73", "72", "71", "70", "69", "68", "64", "63", "62", "61", "60", "59", "58", "57", "56", "65", "66", "67"), Array(), Array(), Array("18"), Array(""));
         $object_display_settings = (Array("siteobject" => "1", "newswireobject" => "2126", "articleobject" => "53", "featureobject" => "54", "galleryobject" => "64", "searchobject" => "2212", "archiveobject" => "55", "contactobject" => "84", "commentsobject"=> "0", "publishobject" => "57", "eventsobject" => "58"));
         $site_module->setSiteSettings($object_display_settings);
      $site_module->setAdminOptions(Array("title" => "Los ultimos articulos", "header" => "", "footer" => "", "emptyresultmsg" => "No news stories at this time.", "read_more_text" => "toda la historia", "filter_author" => "", "editor_note_text" => "", "self_link_text" => "", "relatedlink_text" => "Related Link", "promoted" => "false", "ignorehidden" => "false", "respectsticky" => "true", "allowmultis" => Array("0" => "clip", "1" => "unstick", "2" => "hide", "3" => "unhide", "4" => "unlock")));
	$site_module->setUserOptions(Array("pagelimit" => "12", "storydate_format" => "%A %B %d, %Y - %H:%M", "video_mode" => "", "hiddenonly" => "false", "nohidden" => "false", "lockedonly" => "false", "nolocked" => "false", "nosticky" => "false", "stickyonly" => "false", "upcomingevents" => "false", "statusonly" => "false", "includeicons" => "false", "includephoto" => "true", "showhidden" => "true", "showlocked" => "true", "showsticky" => "true", "showstatus" => "false", "usestatusicons" => "true", "includerelatedlink" => "false", "include_embed_video" => "false", "include_embed_audio" => "false", "usealternatestyles" => "false", "include_translations" => "false", "headernavigation" => Array(), "footernavigation" => Array("0" => "prevnext"), "subtitledetails" => Array("0" => "date"), "enddetails" => Array(), "time_posted_lower_limit" => "0", "time_posted_upper_limit" => "0"));

         $site_module->processRequest();
       ?>
<!-- </Noticias del IMEMC obj id 2126> -->

<?php getPageLanguageOptions("ot", Array("0" => "en", "1" => "sp"), "2126"); ?>
         <?php
          global $editor_session, $pseudo_directory_offset;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjnewswire", 1)))
          {
          ?>
             <br>
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=newswire&amp;target_id=2126&amp;action=edit&amp;site_id=1&amp;obj_language=ot&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0  align="left" title="Edit Newswire Module [Noticias del IMEMC] Configuration" alt="Edit Newswire Module [Noticias del IMEMC] Configuration" src="<?php echo $pseudo_directory_offset?>graphics/mediaicons/32x32/print.png" /></a>
             
          <?
          }
          ?>
          