<?php global $pseudo_directory_offset; $site_file_store = $pseudo_directory_offset."attachments/sites/default/"; 
$oscailt_page_setting = Array("site_module_settings" => Array("siteobject" => "1", "newswireobject" => "2126", "articleobject" => "53", "featureobject" => "54", "galleryobject" => "64", "searchobject" => "2212", "archiveobject" => "55", "contactobject" => "84", "commentsobject" => "", "publishobject" => "57", "eventsobject" => "58"), "site_mail_settings" => Array("to" => "news@imemc.org", "from" => "site@imemc.org", "subject_prefix" => "Article edit:", "replyto" => "news@imemc.org", "level" => "none"), "site_filters" => Array("topic" => Array("0" => "*"), "region" => Array("0" => "20", "1" => "19", "2" => "18", "3" => "17", "4" => "16", "5" => "15", "6" => "14", "7" => "13", "8" => "12", "9" => "11", "10" => "10", "11" => "9", "12" => "8", "13" => "7", "14" => "6", "15" => "5", "16" => "4", "17" => "3", "18" => "2", "19" => "1"), "language" => Array("0" => "en"), "type" => Array("0" => "*"), "author" => ""), "obj_id" => "2132", "obj_type" => "LIST", "site_id" => "1");
$oscailt_story_query = new StoryQuery();
$oscailt_story_query->loadFormValues();
$PAGE_LAYOUT_ID = 2032; ?>
<?
         addToPageDescription("The IMEMC is a media collective. We are independent journalists producing and distributing the authentic voices of the people.  We are located in Beit Sahour, in the Occupied West Bank, and we welcome new volunteers and contributors.");
      addToPageTitle("News From External Sources");
         addToPageDescription("RSS feeds of news from other websites"); ?>

<!-- <news from external sources obj id 2132> -->
<h1 class='indypagebody'>news from external sources</h1><P class='indypagebody'>
         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjfeedimport", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=feedimport&amp;target_id=2117&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Imported Feed Module [palestine chronicle] Configuration" alt="Edit Imported Feed Module [palestine chronicle] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <palestine chronicle obj id 2117> -->
<?php require_once "objects/indyruntime/feedimportdisplay.inc";

         $site_module = new feedimportDisplay(2117);
         $site_module->setFilters(Array("*"), Array("20", "19", "18", "17", "16", "15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1"), Array("en"), Array("*"), Array(""));
         $object_display_settings = (Array("siteobject" => "1", "newswireobject" => "2126", "articleobject" => "53", "featureobject" => "54", "galleryobject" => "64", "searchobject" => "2212", "archiveobject" => "55", "contactobject" => "84", "commentsobject"=> "0", "publishobject" => "57", "eventsobject" => "58"));
         $site_module->setSiteSettings($object_display_settings);
      	$site_module->setAdminOptions(Array("header" => "", "footer" => "", "emptyresultmsg" => "", "promote_text" => "palestinechronicle", "maxhlength" => "0", "url" => "http://palestinechronicle.com/rss.php", "dateformat" => "", "title" => "", "republish_summary_prefill" => "", "language_codes" => "", "max_img_width" => "50", "allowuser_republish" => "false", "striptags" => "false", "convertfromutf8" => "false", "language_filter" => "false", "pre_parse_utf8" => "false", "pagelimit" => "4", "titlesize" => "4", "max_word" => "20", "show_republish" => "false", "includedate" => "false", "articleincludedate" => "true", "articleincludeicons" => "false", "articleincludeauthor" => "true", "articleincludedescription" => "false", "articleincludelanguage" => "false", "includedescription" => "false", "includephoto" => "false", "includetitle" => "true", "includelanguage" => "false", "includecontext" => "false"));
	$site_module->setUserOptions(Array());

         $site_module->retrieveHeadlines();
       ?>
<!-- </palestine chronicle obj id 2117> -->

</P>
<P class='indypagebody'>
         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjfeedimport", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=feedimport&amp;target_id=2136&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Imported Feed Module [scotsman.com] Configuration" alt="Edit Imported Feed Module [scotsman.com] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <scotsman.com obj id 2136> -->
<?php require_once "objects/indyruntime/feedimportdisplay.inc";

         $site_module = new feedimportDisplay(2136);
         $site_module->setFilters(Array("*"), Array("20", "19", "18", "17", "16", "15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1"), Array("en"), Array("*"), Array(""));
         $object_display_settings = (Array("siteobject" => "1", "newswireobject" => "2126", "articleobject" => "53", "featureobject" => "54", "galleryobject" => "64", "searchobject" => "2212", "archiveobject" => "55", "contactobject" => "84", "commentsobject"=> "0", "publishobject" => "57", "eventsobject" => "58"));
         $site_module->setSiteSettings($object_display_settings);
      	$site_module->setAdminOptions(Array("header" => "", "footer" => "", "emptyresultmsg" => "", "promote_text" => "scotsman", "maxhlength" => "0", "url" => "http://news.scotsman.com/topics.cfm?tid=13&format=rss", "dateformat" => "%a %b %d, %Y %H:%M", "title" => "", "republish_summary_prefill" => "", "language_codes" => "", "max_img_width" => "0", "allowuser_republish" => "false", "striptags" => "false", "convertfromutf8" => "false", "language_filter" => "false", "pre_parse_utf8" => "false", "pagelimit" => "5", "titlesize" => "4", "max_word" => "10", "show_republish" => "true", "includedate" => "false", "articleincludedate" => "true", "articleincludeicons" => "false", "articleincludeauthor" => "true", "articleincludedescription" => "false", "articleincludelanguage" => "false", "includedescription" => "false", "includephoto" => "false", "includetitle" => "true", "includelanguage" => "false", "includecontext" => "false"));
	$site_module->setUserOptions(Array());

         $site_module->retrieveHeadlines();
       ?>
<!-- </scotsman.com obj id 2136> -->

</P>


<!-- </news from external sources obj id 2132> -->


         <?php
          global $editor_session, $pseudo_directory_offset;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjlist", 1)))
          {
          ?>
             <br>
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=list&amp;target_id=2132&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0  align="left" title="Edit Listing Box [news from external sources] Configuration" alt="Edit Listing Box [news from external sources] Configuration" src="<?php echo $pseudo_directory_offset?>graphics/mediaicons/32x32/listingbox.png" /></a>
             
          <?
          }
          ?>
          