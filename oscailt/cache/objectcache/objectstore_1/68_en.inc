

<!-- <newswire menu obj id 68> -->

         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjmenu", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=menu&amp;target_id=68&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Vertical Menu [newswire menu] Configuration" alt="Edit Vertical Menu [newswire menu] Configuration" src="<?php echo $graphics_store?>editmenu.gif" /></a>
             
          <?
          }
          ?>
       
         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjfeedimport", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=feedimport&amp;target_id=1980&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Imported Feed Module [imc-features] Configuration" alt="Edit Imported Feed Module [imc-features] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <imc-features obj id 1980> -->
<div class='<?php echo $page_style; ?>'>
<?php require_once "objects/indyruntime/feedimportdisplay.inc";

         $site_module = new feedimportDisplay(1980);
         $site_module->setFilters(Array("*"), Array("20", "19", "18", "17", "16", "15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1"), Array("en"), Array("*"), Array(""));
         $object_display_settings = (Array("siteobject" => "1", "newswireobject" => "2126", "articleobject" => "53", "featureobject" => "54", "galleryobject" => "64", "searchobject" => "2212", "archiveobject" => "55", "contactobject" => "84", "commentsobject"=> "0", "publishobject" => "2390", "eventsobject" => "58"));
         $site_module->setSiteSettings($object_display_settings);
      	$site_module->setAdminOptions(Array("header" => "", "footer" => "", "emptyresultmsg" => "", "promote_text" => "promote", "maxhlength" => "80", "url" => "http://newswire.indymedia.org/localfeatures-features-content.rss", "dateformat" => "%H:%M %a %b %d, %Y", "title" => "Global Indymedia Features", "republish_summary_prefill" => "REPLACE SUMMARY WITH ORIGINAL INTRODUCTION", "language_codes" => "", "max_img_width" => "110", "allowuser_republish" => "true", "striptags" => "false", "convertfromutf8" => "false", "language_filter" => "false", "pre_parse_utf8" => "false", "pagelimit" => "10", "titlesize" => "3", "max_word" => "40", "show_republish" => "false", "includedate" => "true", "articleincludedate" => "true", "articleincludeicons" => "true", "articleincludeauthor" => "true", "articleincludedescription" => "false", "articleincludelanguage" => "true", "includedescription" => "true", "includephoto" => "false", "includetitle" => "true", "includelanguage" => "false", "includecontext" => "false"));
	$site_module->setUserOptions(Array());

         $site_module->retrieveHeadlines();
       ?>
</div>
<!-- </imc-features obj id 1980> -->


         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjfeedimport", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=feedimport&amp;target_id=2109&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Imported Feed Module [imc-global] Configuration" alt="Edit Imported Feed Module [imc-global] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <imc-global obj id 2109> -->
<div class='<?php echo $page_style; ?>'>
<?php require_once "objects/indyruntime/feedimportdisplay.inc";

         $site_module = new feedimportDisplay(2109);
         $site_module->setFilters(Array("*"), Array("20", "19", "18", "17", "16", "15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1"), Array("en"), Array("*"), Array(""));
         $object_display_settings = (Array("siteobject" => "1", "newswireobject" => "2126", "articleobject" => "53", "featureobject" => "54", "galleryobject" => "64", "searchobject" => "2212", "archiveobject" => "55", "contactobject" => "84", "commentsobject"=> "0", "publishobject" => "2390", "eventsobject" => "58"));
         $site_module->setSiteSettings($object_display_settings);
      	$site_module->setAdminOptions(Array("header" => "", "footer" => "", "emptyresultmsg" => "", "promote_text" => "promote", "maxhlength" => "80", "url" => "http://indymedia.org/en/main-features.rss", "dateformat" => "%a %b %d, %Y %H:%M", "title" => "Indymedia.org Features", "republish_summary_prefill" => "REPLACE SUMMARY WITH ORIGINAL INTRODUCTION", "language_codes" => "", "max_img_width" => "110", "allowuser_republish" => "true", "striptags" => "true", "convertfromutf8" => "false", "language_filter" => "false", "pre_parse_utf8" => "false", "pagelimit" => "10", "titlesize" => "5", "max_word" => "35", "show_republish" => "false", "includedate" => "false", "articleincludedate" => "true", "articleincludeicons" => "true", "articleincludeauthor" => "true", "articleincludedescription" => "false", "articleincludelanguage" => "true", "includedescription" => "true", "includephoto" => "true", "includetitle" => "true", "includelanguage" => "false", "includecontext" => "false"));
	$site_module->setUserOptions(Array());

         $site_module->retrieveHeadlines();
       ?>
</div>
<!-- </imc-global obj id 2109> -->


<!-- </newswire menu obj id 68> -->

