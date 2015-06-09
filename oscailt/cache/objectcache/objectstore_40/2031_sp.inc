<?php global $pseudo_directory_offset; $site_file_store = $pseudo_directory_offset."attachments/sites/default/"; 
$oscailt_page_setting = Array("site_module_settings" => Array("siteobject" => "1", "newswireobject" => "2126", "articleobject" => "53", "featureobject" => "54", "galleryobject" => "64", "searchobject" => "2212", "archiveobject" => "55", "contactobject" => "84", "commentsobject" => "", "publishobject" => "57", "eventsobject" => "58"), "site_mail_settings" => Array("to" => "news@imemc.org", "from" => "site@imemc.org", "subject_prefix" => "Article edit:", "replyto" => "news@imemc.org", "level" => "none"), "site_filters" => Array("topic" => Array("0" => "*"), "region" => Array("0" => "20", "1" => "19", "2" => "18", "3" => "17", "4" => "16", "5" => "15", "6" => "14", "7" => "13", "8" => "12", "9" => "11", "10" => "10", "11" => "9", "12" => "8", "13" => "7", "14" => "6", "15" => "5", "16" => "4", "17" => "3", "18" => "2", "19" => "1"), "language" => Array("0" => "en"), "type" => Array("0" => "*"), "author" => ""), "obj_id" => "2031", "obj_type" => "PREFERENCES", "site_id" => "1", "page_filters" => Array("topic" => Array("0" => "*"), "region" => Array("0" => "20", "1" => "19", "2" => "18", "3" => "17", "4" => "16", "5" => "15", "6" => "14", "7" => "13", "8" => "12", "9" => "11", "10" => "10", "11" => "9", "12" => "8", "13" => "7", "14" => "6", "15" => "5", "16" => "4", "17" => "3", "18" => "2", "19" => "1"), "language" => Array("0" => "en"), "type" => Array("0" => "*"), "author" => Array("0" => "")));
$oscailt_story_query = new StoryQuery();
$oscailt_story_query->loadFormValues();
$PAGE_LAYOUT_ID = 2032; ?>
<?
         addToPageDescription("The IMEMC is a media collective. We are independent journalists producing and distributing the authentic voices of the people.  We are located in Beit Sahour, in the Occupied West Bank, and we welcome new volunteers and contributors.");
      addToPageTitle("Preferencias De Uso Del Recuadro");
         addToPageDescription("the page for changing user preferences"); ?>

<!-- <preferencias de uso del recuadro obj id 2031> -->
<h1 class='indypagebody'>ajustes del usuario</h1><?php require_once "objects/indyruntime/preferencesdisplay.inc";

         $site_module = new preferencesDisplay(2031);
         $site_module->setFilters(Array("*"), Array("20", "19", "18", "17", "16", "15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1"), Array("en"), Array("*"), Array(""));
         $object_display_settings = (Array("siteobject" => "1", "newswireobject" => "2126", "articleobject" => "53", "featureobject" => "54", "galleryobject" => "64", "searchobject" => "2212", "archiveobject" => "55", "contactobject" => "84", "commentsobject"=> "0", "publishobject" => "57", "eventsobject" => "58"));
         $site_module->setSiteSettings($object_display_settings);
      $site_module->setAdminOptions(Array("title" => "ajustes del usuario", "display" => "Full Page", "hsize" => "1", "class" => "indypagebody", "header" => "<P>You can change some of the basic settings for viewing this site with the form below. </P>", "footer" => "", "optiontext" => "Preference Type", "valuetext" => "Setting Value", "descriptiontext" => "Description", "universaloptionstext" => "Universal Settings", "articleoptionstext" => "article section", "eventsoptionstext" => "event section", "commentsoptionstext" => "comment text", "saveoptionstext" => "Save Setting >>", "languageoptiontitle" => "Default Page Language", "languageoptiondescription" => "This allows you to change the default setting for the language that will be used to display pages on the site.  Note, you can still view individual articles and pages in all languages", "fontsizeoptiontitle" => "Font Size", "fontsizeoptiondescription" => "This allows you to increase the size of fonts on the site", "comment_titles_title" => "comm title field", "comment_titles_description" => "comm tit desc", "all_comments_title" => "all comm", "all_comments_description" => "all comm desc", "event_displaytitle" => "event displ mode", "event_displaydescription" => "event mod desc", "condense_commentstitle" => "comments", "condense_commentsdescription" => "comments desc", "comment_ordertitle" => "comm order", "comment_orderdescription" => "comm oreder desc", "comment_limittitle" => "comment page size", "comment_limitdescription" => "comm page size desc", "languagefiltertitle" => "Mono-Lingual Filter", "languagefilterdescription" => "Do you want to view articles in all languages, or just your own?", "languagefilteroff" => "View All Languages", "languagefilteron" => "Only My Language", "preferences_updated" => "Your Preference Settings have been Updated!", "comments_text_title" => "", "comments_text_description" => "", "video_mode_title" => "", "video_mode_description" => ""));

$site_module->setUserOptions(array());

         $site_module->processRequest();
       ?>
<!-- </preferencias de uso del recuadro obj id 2031> -->

<?php getPageLanguageOptions("ot", Array("0" => "en", "1" => "sp"), "2031"); ?>
         <?php
          global $editor_session, $pseudo_directory_offset;
          if(($editor_session->canEditLayout()|| $editor_session->canEditContent()) && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjpreferences", 1)))
          {
          ?>
             <br>
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=preferences&amp;target_id=2031&amp;action=edit&amp;site_id=1&amp;obj_language=ot&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0  align="left" title="Edit User Preferences [preferencias de uso del recuadro] Configuration" alt="Edit User Preferences [preferencias de uso del recuadro] Configuration" src="<?php echo $pseudo_directory_offset?>graphics/mediaicons/32x32/process4.png" /></a>
             
          <?
          }
          ?>
          