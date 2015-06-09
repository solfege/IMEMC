<?php global $pseudo_directory_offset; $site_file_store = $pseudo_directory_offset."attachments/sites/default/"; 
$oscailt_page_setting = Array("site_module_settings" => Array("siteobject" => "1", "newswireobject" => "2126", "articleobject" => "53", "featureobject" => "54", "galleryobject" => "64", "searchobject" => "2212", "archiveobject" => "55", "contactobject" => "84", "commentsobject" => "", "publishobject" => "57", "eventsobject" => "58"), "site_mail_settings" => Array("to" => "news@imemc.org", "from" => "site@imemc.org", "subject_prefix" => "Article edit:", "replyto" => "news@imemc.org", "level" => "none"), "site_filters" => Array("topic" => Array("0" => "*"), "region" => Array("0" => "20", "1" => "19", "2" => "18", "3" => "17", "4" => "16", "5" => "15", "6" => "14", "7" => "13", "8" => "12", "9" => "11", "10" => "10", "11" => "9", "12" => "8", "13" => "7", "14" => "6", "15" => "5", "16" => "4", "17" => "3", "18" => "2", "19" => "1"), "language" => Array("0" => "en"), "type" => Array("0" => "*"), "author" => ""), "obj_id" => "67", "obj_type" => "LIST", "site_id" => "1");
$oscailt_story_query = new StoryQuery();
$oscailt_story_query->loadFormValues();
$PAGE_LAYOUT_ID = 2032; ?>
<?
         addToPageDescription("The IMEMC is a media collective. We are independent journalists producing and distributing the authentic voices of the people.  We are located in Beit Sahour, in the Occupied West Bank, and we welcome new volunteers and contributors.");
      addToPageTitle("La NavegaciÃ?Â³n Por El Sitio");
         addToPageDescription("La lista bÃ?Â¡sica de navegaciÃ?Â³n para el sitio"); ?>

<!-- <La navegaciÃ?Â³n por el sitio obj id 67> -->
<h1 class='indypagebody'>La navegaciÃ?Â³n por el sitio</h1><P class='indypagebody'>
         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjnewswire", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=newswire&amp;target_id=2119&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Newswire Module [Newsbrief wire] Configuration" alt="Edit Newswire Module [Newsbrief wire] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <Newsbrief wire obj id 2119> -->
<h3 class='indypagebody'><a href='<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=2119' class='indypagebody' title='Breaking News'>Breaking News</a></h3><?php require_once "objects/indyruntime/newswiredisplay.inc";

         $site_module = new newswireDisplay(2119);
         $site_module->setFilters(Array("36", "35", "34", "33", "32", "15", "24", "23", "22", "21", "25", "26", "10", "30", "16", "31", "20", "3", "27", "28"), Array("20", "19", "18", "17", "16", "15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1"), Array("en"), Array("12", "11"), Array(""));
         $object_display_settings = (Array("siteobject" => "1", "newswireobject" => "2126", "articleobject" => "53", "featureobject" => "54", "galleryobject" => "64", "searchobject" => "2212", "archiveobject" => "55", "contactobject" => "84", "commentsobject"=> "0", "publishobject" => "57", "eventsobject" => "58"));
         $site_module->setSiteSettings($object_display_settings);
      	$site_module->setAdminOptions(Array("pagelimit" => "10", "maxhlength" => "0", "articleincludedate" => "%H:%M %a %d %b", "includedate" => "", "eventtimeformat" => "", "read_more_text" => "", "hiddenonly" => "false", "nohidden" => "false", "lockedonly" => "false", "nolocked" => "false", "nosticky" => "false", "stickyonly" => "false", "upcomingevents" => "false", "statusonly" => "false", "includecontext" => "false", "includeicons" => "false", "showhidden" => "true", "showlocked" => "true", "showsticky" => "true", "showstatus" => "false", "usestatusicons" => "true", "ignorehidden" => "false", "respectsticky" => "false", "articleincludeicons" => "false", "includeauthor" => "false", "includeorganisation" => "false", "includecommentcount" => "false", "includephoto" => "false", "highlightfeatures" => "false", "context_sensitive_region" => "false", "context_sensitive_type" => "false", "context_sensitive_topic" => "false", "context_sensitive_language" => "false", "context_sensitive_author" => "false", "promoted" => "false", "emptyresultmsg" => "no breaking news at the moment", "filter_author" => "", "time_posted_lower_limit" => "0", "time_posted_upper_limit" => "0", "full_page_id" => "2119"));
	$site_module->setUserOptions(Array());

         $site_module->retrieveHeadlines();
       ?>
<!-- </Newsbrief wire obj id 2119> -->

</P>
<P class='indypagebody'>
         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjnewswire", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=newswire&amp;target_id=2126&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Newswire Module [IMEMC News] Configuration" alt="Edit Newswire Module [IMEMC News] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <IMEMC News obj id 2126> -->
<h3 class='indypagebody'><a href='<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=2126' class='indypagebody' title='Latest News Articles'>Latest News Articles</a></h3><?php require_once "objects/indyruntime/newswiredisplay.inc";

         $site_module = new newswireDisplay(2126);
         $site_module->setFilters(Array("*"), Array("20", "19", "18", "17", "16", "15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1", "20", "19", "18", "17", "16", "15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1"), Array("en", "en"), Array("2"), Array(""));
         $object_display_settings = (Array("siteobject" => "1", "newswireobject" => "2126", "articleobject" => "53", "featureobject" => "54", "galleryobject" => "64", "searchobject" => "2212", "archiveobject" => "55", "contactobject" => "84", "commentsobject"=> "0", "publishobject" => "57", "eventsobject" => "58"));
         $site_module->setSiteSettings($object_display_settings);
      	$site_module->setAdminOptions(Array("pagelimit" => "10", "maxhlength" => "0", "articleincludedate" => "%H:%M %a %d %b", "includedate" => "", "eventtimeformat" => "", "read_more_text" => "Full Story", "hiddenonly" => "false", "nohidden" => "false", "lockedonly" => "false", "nolocked" => "false", "nosticky" => "false", "stickyonly" => "false", "upcomingevents" => "false", "statusonly" => "false", "includecontext" => "false", "includeicons" => "false", "showhidden" => "true", "showlocked" => "true", "showsticky" => "true", "showstatus" => "false", "usestatusicons" => "true", "ignorehidden" => "false", "respectsticky" => "false", "articleincludeicons" => "false", "includeauthor" => "false", "includeorganisation" => "false", "includecommentcount" => "false", "includephoto" => "true", "highlightfeatures" => "false", "context_sensitive_region" => "false", "context_sensitive_type" => "false", "context_sensitive_topic" => "false", "context_sensitive_language" => "false", "context_sensitive_author" => "false", "promoted" => "false", "emptyresultmsg" => "No news stories at this time.", "filter_author" => "", "time_posted_lower_limit" => "0", "time_posted_upper_limit" => "0", "full_page_id" => "2126"));
	$site_module->setUserOptions(Array());

         $site_module->retrieveHeadlines();
       ?>
<!-- </IMEMC News obj id 2126> -->

</P>
<P class='indypagebody'>
         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjnewswire", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=newswire&amp;target_id=2128&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Newswire Module [audio] Configuration" alt="Edit Newswire Module [audio] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <audio obj id 2128> -->
<h3 class='indypagebody'><a href='<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=2128' class='indypagebody' title='Latest Audio'>Latest Audio</a></h3><?php require_once "objects/indyruntime/newswiredisplay.inc";

         $site_module = new newswireDisplay(2128);
         $site_module->setFilters(Array("36", "35", "34", "33", "32", "15", "24", "23", "22", "21", "25", "26", "10", "30", "16", "28", "27", "3", "20", "31"), Array("20", "19", "18", "17", "16", "15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1", "20", "19", "18", "17", "16", "15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1", "20", "19", "18", "17", "16", "15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1"), Array("en", "en", "en"), Array("10"), Array(""));
         $object_display_settings = (Array("siteobject" => "1", "newswireobject" => "2126", "articleobject" => "53", "featureobject" => "54", "galleryobject" => "64", "searchobject" => "2212", "archiveobject" => "55", "contactobject" => "84", "commentsobject"=> "0", "publishobject" => "57", "eventsobject" => "58"));
         $site_module->setSiteSettings($object_display_settings);
      	$site_module->setAdminOptions(Array("pagelimit" => "3", "maxhlength" => "0", "articleincludedate" => "", "includedate" => "", "eventtimeformat" => "", "read_more_text" => "", "hiddenonly" => "false", "nohidden" => "false", "lockedonly" => "false", "nolocked" => "false", "nosticky" => "false", "stickyonly" => "false", "upcomingevents" => "false", "statusonly" => "false", "includecontext" => "false", "includeicons" => "false", "showhidden" => "true", "showlocked" => "true", "showsticky" => "true", "showstatus" => "false", "usestatusicons" => "true", "ignorehidden" => "false", "respectsticky" => "false", "articleincludeicons" => "false", "includeauthor" => "false", "includeorganisation" => "false", "includecommentcount" => "false", "includephoto" => "false", "highlightfeatures" => "false", "context_sensitive_region" => "true", "context_sensitive_type" => "true", "context_sensitive_topic" => "false", "context_sensitive_language" => "false", "context_sensitive_author" => "false", "promoted" => "false", "emptyresultmsg" => "no audio posted", "filter_author" => "", "time_posted_lower_limit" => "0", "time_posted_upper_limit" => "0", "full_page_id" => "2128"));
	$site_module->setUserOptions(Array());

         $site_module->retrieveHeadlines();
       ?>
<!-- </audio obj id 2128> -->

</P>
<P class='indypagebody'>
         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjnewswire", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=newswire&amp;target_id=2123&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Newswire Module [human interest] Configuration" alt="Edit Newswire Module [human interest] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <human interest obj id 2123> -->
<h3 class='indypagebody'><a href='<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=2123' class='indypagebody' title='Human Interest'>Human Interest</a></h3><?php require_once "objects/indyruntime/newswiredisplay.inc";

         $site_module = new newswireDisplay(2123);
         $site_module->setFilters(Array("76", "36", "35", "34", "33", "32", "15", "24", "23", "22", "21", "25", "26", "10", "30", "16", "28", "27", "3", "20", "31"), Array("20", "19", "18", "17", "16", "15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1", "20", "19", "18", "17", "16", "15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1", "20", "19", "18", "17", "16", "15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1", "20", "19", "18", "17", "16", "15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1"), Array("en", "en", "en", "en"), Array("8"), Array(""));
         $object_display_settings = (Array("siteobject" => "1", "newswireobject" => "2126", "articleobject" => "53", "featureobject" => "54", "galleryobject" => "64", "searchobject" => "2212", "archiveobject" => "55", "contactobject" => "84", "commentsobject"=> "0", "publishobject" => "57", "eventsobject" => "58"));
         $site_module->setSiteSettings($object_display_settings);
      	$site_module->setAdminOptions(Array("pagelimit" => "3", "maxhlength" => "60", "articleincludedate" => "", "includedate" => "", "eventtimeformat" => "", "read_more_text" => "", "hiddenonly" => "false", "nohidden" => "false", "lockedonly" => "false", "nolocked" => "false", "nosticky" => "false", "stickyonly" => "false", "upcomingevents" => "false", "statusonly" => "false", "includecontext" => "false", "includeicons" => "false", "showhidden" => "true", "showlocked" => "true", "showsticky" => "true", "showstatus" => "false", "usestatusicons" => "true", "ignorehidden" => "false", "respectsticky" => "false", "articleincludeicons" => "false", "includeauthor" => "true", "includeorganisation" => "false", "includecommentcount" => "false", "includephoto" => "true", "highlightfeatures" => "false", "context_sensitive_region" => "false", "context_sensitive_type" => "false", "context_sensitive_topic" => "false", "context_sensitive_language" => "false", "context_sensitive_author" => "false", "promoted" => "false", "emptyresultmsg" => "Sorry no articles have been posted matching your request.", "filter_author" => "", "time_posted_lower_limit" => "0", "time_posted_upper_limit" => "0", "full_page_id" => "2123"));
	$site_module->setUserOptions(Array());

         $site_module->retrieveHeadlines();
       ?>
<!-- </human interest obj id 2123> -->

</P>
<P class='indypagebody'>
         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjnewswire", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=newswire&amp;target_id=2118&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Newswire Module [opinion/analysis] Configuration" alt="Edit Newswire Module [opinion/analysis] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <opinion/analysis obj id 2118> -->
<h3 class='indypagebody'><a href='<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=2118' class='indypagebody' title='Opinion/Analysis'>Opinion/Analysis</a></h3><?php require_once "objects/indyruntime/newswiredisplay.inc";

         $site_module = new newswireDisplay(2118);
         $site_module->setFilters(Array("*"), Array("20", "19", "18", "17", "16", "15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1"), Array("en"), Array("3"), Array(""));
         $object_display_settings = (Array("siteobject" => "1", "newswireobject" => "2126", "articleobject" => "53", "featureobject" => "54", "galleryobject" => "64", "searchobject" => "2212", "archiveobject" => "55", "contactobject" => "84", "commentsobject"=> "0", "publishobject" => "57", "eventsobject" => "58"));
         $site_module->setSiteSettings($object_display_settings);
      	$site_module->setAdminOptions(Array("pagelimit" => "3", "maxhlength" => "60", "articleincludedate" => "", "includedate" => "", "eventtimeformat" => "", "read_more_text" => "", "hiddenonly" => "false", "nohidden" => "false", "lockedonly" => "false", "nolocked" => "false", "nosticky" => "false", "stickyonly" => "false", "upcomingevents" => "false", "statusonly" => "false", "includecontext" => "false", "includeicons" => "false", "showhidden" => "true", "showlocked" => "true", "showsticky" => "true", "showstatus" => "false", "usestatusicons" => "true", "ignorehidden" => "false", "respectsticky" => "false", "articleincludeicons" => "false", "includeauthor" => "true", "includeorganisation" => "false", "includecommentcount" => "false", "includephoto" => "true", "highlightfeatures" => "false", "context_sensitive_region" => "false", "context_sensitive_type" => "false", "context_sensitive_topic" => "false", "context_sensitive_language" => "false", "context_sensitive_author" => "false", "promoted" => "false", "emptyresultmsg" => "Sorry, no articles match your request.", "filter_author" => "", "time_posted_lower_limit" => "0", "time_posted_upper_limit" => "0", "full_page_id" => "2118"));
	$site_module->setUserOptions(Array());

         $site_module->retrieveHeadlines();
       ?>
<!-- </opinion/analysis obj id 2118> -->

</P>
<P class='indypagebody'>
         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjnewswire", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=newswire&amp;target_id=2143&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Newswire Module [Interviews] Configuration" alt="Edit Newswire Module [Interviews] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <Interviews obj id 2143> -->
<h3 class='indypagebody'><a href='<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=2143' class='indypagebody' title='Interviews'>Interviews</a></h3><?php require_once "objects/indyruntime/newswiredisplay.inc";

         $site_module = new newswireDisplay(2143);
         $site_module->setFilters(Array("36", "35", "34", "33", "32", "15", "24", "23", "22", "21", "25", "26", "10", "30", "16", "31", "20", "3", "27", "28"), Array("20", "19", "18", "17", "16", "15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1", "20", "19", "18", "17", "16", "15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1", "20", "19", "18", "17", "16", "15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1", "20", "19", "18", "17", "16", "15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1"), Array("en", "en", "en", "en"), Array("9"), Array(""));
         $object_display_settings = (Array("siteobject" => "1", "newswireobject" => "2126", "articleobject" => "53", "featureobject" => "54", "galleryobject" => "64", "searchobject" => "2212", "archiveobject" => "55", "contactobject" => "84", "commentsobject"=> "0", "publishobject" => "57", "eventsobject" => "58"));
         $site_module->setSiteSettings($object_display_settings);
      	$site_module->setAdminOptions(Array("pagelimit" => "3", "maxhlength" => "60", "articleincludedate" => "", "includedate" => "", "eventtimeformat" => "", "read_more_text" => "", "hiddenonly" => "false", "nohidden" => "false", "lockedonly" => "false", "nolocked" => "false", "nosticky" => "false", "stickyonly" => "false", "upcomingevents" => "false", "statusonly" => "false", "includecontext" => "false", "includeicons" => "false", "showhidden" => "true", "showlocked" => "true", "showsticky" => "true", "showstatus" => "false", "usestatusicons" => "true", "ignorehidden" => "false", "respectsticky" => "false", "articleincludeicons" => "false", "includeauthor" => "true", "includeorganisation" => "false", "includecommentcount" => "false", "includephoto" => "true", "highlightfeatures" => "false", "context_sensitive_region" => "false", "context_sensitive_type" => "false", "context_sensitive_topic" => "false", "context_sensitive_language" => "false", "context_sensitive_author" => "false", "promoted" => "false", "emptyresultmsg" => "We are sorry, the story you have requested is not available, please contact contact@imemc.org for more information", "filter_author" => "", "time_posted_lower_limit" => "0", "time_posted_upper_limit" => "0", "full_page_id" => "2143"));
	$site_module->setUserOptions(Array());

         $site_module->retrieveHeadlines();
       ?>
<!-- </Interviews obj id 2143> -->

</P>


<!-- </La navegaciÃ?Â³n por el sitio obj id 67> -->

<?php getPageLanguageOptions("ot", Array("0" => "en", "1" => "sp"), "67"); ?>
         <?php
          global $editor_session, $pseudo_directory_offset;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjlist", 1)))
          {
          ?>
             <br>
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=list&amp;target_id=67&amp;action=edit&amp;site_id=1&amp;obj_language=ot&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0  align="left" title="Edit Listing Box [La navegaciÃ?Â³n por el sitio] Configuration" alt="Edit Listing Box [La navegaciÃ?Â³n por el sitio] Configuration" src="<?php echo $pseudo_directory_offset?>graphics/mediaicons/32x32/listingbox.png" /></a>
             
          <?
          }
          ?>
          