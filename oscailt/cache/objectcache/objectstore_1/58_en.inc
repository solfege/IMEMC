<?php global $pseudo_directory_offset; $site_file_store = $pseudo_directory_offset."attachments/sites/default/"; 
$oscailt_page_setting = Array("site_module_settings" => Array("siteobject" => "1", "newswireobject" => "2126", "articleobject" => "53", "featureobject" => "54", "galleryobject" => "64", "searchobject" => "2212", "archiveobject" => "55", "contactobject" => "84", "commentsobject" => "", "publishobject" => "57", "eventsobject" => "58"), "site_mail_settings" => Array("to" => "news@imemc.org", "from" => "site@imemc.org", "subject_prefix" => "Article edit:", "replyto" => "news@imemc.org", "level" => "none"), "site_filters" => Array("topic" => Array("0" => "*"), "region" => Array("0" => "20", "1" => "19", "2" => "18", "3" => "17", "4" => "16", "5" => "15", "6" => "14", "7" => "13", "8" => "12", "9" => "11", "10" => "10", "11" => "9", "12" => "8", "13" => "7", "14" => "6", "15" => "5", "16" => "4", "17" => "3", "18" => "2", "19" => "1"), "language" => Array("0" => "en"), "type" => Array("0" => "*"), "author" => ""), "obj_id" => "58", "obj_type" => "EVENTS", "site_id" => "1", "page_filters" => Array("topic" => Array("0" => "*"), "region" => Array("0" => "20", "1" => "19", "2" => "18", "3" => "17", "4" => "16", "5" => "15", "6" => "14", "7" => "13", "8" => "12", "9" => "11", "10" => "10", "11" => "9", "12" => "8", "13" => "7", "14" => "6", "15" => "5", "16" => "4", "17" => "3", "18" => "2", "19" => "1"), "language" => Array("0" => "en"), "type" => Array("0" => "5"), "author" => Array("0" => "")));
$oscailt_story_query = new StoryQuery();
$oscailt_story_query->loadFormValues();
$PAGE_LAYOUT_ID = 2034; ?>
<?
         addToPageDescription("The IMEMC is a media collective. We are independent journalists producing and distributing the authentic voices of the people.  We are located in Beit Sahour, in the Occupied West Bank, and we welcome new volunteers and contributors.");
      addToPageTitle("Events");
         addToPageDescription("The basic Events Calendar for the site"); ?>

<!-- <events obj id 58> -->
<?php require_once "objects/indyruntime/newswiredisplay.inc";
require_once "objects/indyruntime/eventsdisplay.inc";

         $site_module = new eventsDisplay(58);
         $site_module->setFilters(Array("*"), Array("20", "19", "18", "17", "16", "15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1"), Array("en"), Array("5"), Array(""));
         $object_display_settings = (Array("siteobject" => "1", "newswireobject" => "2126", "articleobject" => "53", "featureobject" => "54", "galleryobject" => "64", "searchobject" => "2212", "archiveobject" => "55", "contactobject" => "84", "commentsobject"=> "0", "publishobject" => "57", "eventsobject" => "58"));
         $site_module->setSiteSettings($object_display_settings);
      $site_module->setAdminOptions(Array("title" => "Event Calendar", "header" => "", "footer" => "", "emptyresultmsg" => "Sorry, there are currently no events in the system which match your query!", "weekly_text" => "Weekly Listing", "monthly_text" => "Monthly Listing", "biweekly_text" => "Bi-weekly Listing", "yeartext" => "Year", "nexttext" => "Next", "prevtext" => "Previous", "introtext" => "This is a calendar of events and activities submitted by the site\'s readers which often includes notices of courses, workshops, film & video showings, outings, broadcasts on topics such as peace, the environment, sustainable living, social justice and much more.", "pub_event_notice_text" => "", "past_events_text" => "", "todays_events_text" => "", "future_events_text" => "", "datelabel_text" => "", "titlelabel_text" => "", "regionlabel_text" => "", "ignorehidden" => "false", "respectsticky" => "false", "allowmultis" => Array("0" => "clip")));
	$site_module->setUserOptions(Array("pagelimit" => "", "includedate" => "", "eventtimeformat" => "", "event_display" => "monthly", "hiddenonly" => "false", "nohidden" => "false", "lockedonly" => "false", "nolocked" => "false", "nosticky" => "false", "stickyonly" => "false", "upcomingevents" => "false", "statusonly" => "false", "includeicons" => "false", "includephoto" => "false", "showhidden" => "false", "showlocked" => "false", "showsticky" => "false", "showstatus" => "false", "usestatusicons" => "false", "headernavigation" => Array("0" => "context", "1" => "prevnext"), "footernavigation" => Array("0" => "context", "1" => "otherviews", "2" => "prevnext"), "subtitledetails" => Array(), "enddetails" => Array(), "time_posted_lower_limit" => "0", "time_posted_upper_limit" => "0"));

         $site_module->processRequest();
       ?>
<!-- </events obj id 58> -->


         <?php
          global $editor_session, $pseudo_directory_offset;
          if(($editor_session->canEditLayout()|| $editor_session->canEditContent()) && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjevents", 1)))
          {
          ?>
             <br>
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=events&amp;target_id=58&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0  align="left" title="Edit Events Calendar Module [events] Configuration" alt="Edit Events Calendar Module [events] Configuration" src="<?php echo $pseudo_directory_offset?>graphics/mediaicons/32x32/event.png" /></a>
             
          <?
          }
          ?>
          