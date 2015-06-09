<?php global $pseudo_directory_offset; $site_file_store = $pseudo_directory_offset."attachments/sites/default/"; 
$oscailt_page_setting = Array("site_module_settings" => Array("siteobject" => "1", "newswireobject" => "2126", "articleobject" => "53", "featureobject" => "54", "galleryobject" => "64", "searchobject" => "2212", "archiveobject" => "55", "contactobject" => "84", "commentsobject" => "", "publishobject" => "57", "eventsobject" => "58"), "site_mail_settings" => Array("to" => "news@imemc.org", "from" => "site@imemc.org", "subject_prefix" => "Article edit:", "replyto" => "news@imemc.org", "level" => "none"), "site_filters" => Array("topic" => Array("0" => "*"), "region" => Array("0" => "20", "1" => "19", "2" => "18", "3" => "17", "4" => "16", "5" => "15", "6" => "14", "7" => "13", "8" => "12", "9" => "11", "10" => "10", "11" => "9", "12" => "8", "13" => "7", "14" => "6", "15" => "5", "16" => "4", "17" => "3", "18" => "2", "19" => "1"), "language" => Array("0" => "en"), "type" => Array("0" => "*"), "author" => ""), "obj_id" => "84", "obj_type" => "CONTACT", "site_id" => "1", "page_filters" => Array("topic" => Array("0" => "*"), "region" => Array("0" => "20", "1" => "19", "2" => "18", "3" => "17", "4" => "16", "5" => "15", "6" => "14", "7" => "13", "8" => "12", "9" => "11", "10" => "10", "11" => "9", "12" => "8", "13" => "7", "14" => "6", "15" => "5", "16" => "4", "17" => "3", "18" => "2", "19" => "1"), "language" => Array("0" => "en"), "type" => Array("0" => "*"), "author" => Array("0" => "")));
$oscailt_story_query = new StoryQuery();
$oscailt_story_query->loadFormValues();
$PAGE_LAYOUT_ID = 2032; ?>
<?
         addToPageDescription("The IMEMC is a media collective. We are independent journalists producing and distributing the authentic voices of the people.  We are located in Beit Sahour, in the Occupied West Bank, and we welcome new volunteers and contributors.");
      addToPageTitle("Contact Form");
         addToPageDescription("the contact form for getting in touch with the editors"); ?><?php require_once "objects/indyruntime/contactdisplay.inc";

         $site_module = new contactDisplay(84);
         $site_module->setFilters(Array("*"), Array("20", "19", "18", "17", "16", "15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1"), Array("en"), Array("*"), Array(""));
         $object_display_settings = (Array("siteobject" => "1", "newswireobject" => "2126", "articleobject" => "53", "featureobject" => "54", "galleryobject" => "64", "searchobject" => "2212", "archiveobject" => "55", "contactobject" => "84", "commentsobject"=> "0", "publishobject" => "57", "eventsobject" => "58"));
         $site_module->setSiteSettings($object_display_settings);
      $site_module->setAdminOptions(Array("title" => "Contact Us", "header" => "", "footer" => "", "nametitle" => "Your Name", "contentstitle" => "Your Message", "subjecttitle" => "The Subject of your Message", "fromtitle" => "Your Email Address", "introtext" => "If you wish to contact or get involved with the collective, please fill in the Email Form below and click on the \'Send Message\' button. 
</p>
<p>You must fill in all fields to submit an email.", "emailto" => "news@imemc.org", "emailfrom" => "IMEMC Contact Form", "emailreply" => "", "formheadtext" => "Fill in the form below to submit a message to the collective.", "buttontext" => "Send Message", "successtext" => "Your message has been sent to the admins who run this site. If you included a valid email address, we may contact you soon with a response. We cannot guarantee a response within any time frame. We will do our best, however you will get a reply for sure.", "finalchecktext" => "Please let us know you are a human:", "spammandatorywords" => "", "editorprivatemail" => "false", "logemails" => "false", "hidesubjectfield" => "true", "spambotcheck" => "true", "logspamcheck" => "true"));

$site_module->setUserOptions(array());

         $site_module->processRequest();
       ?><?php getPageLanguageOptions("en", Array("0" => "en", "1" => "sp"), "84"); ?>
         <?php
          global $editor_session, $pseudo_directory_offset;
          if(($editor_session->canEditLayout()|| $editor_session->canEditContent()) && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjcontact", 1)))
          {
          ?>
             <br>
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=contact&amp;target_id=84&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0  align="left" title="Edit Contact Form Module [contact form] Configuration" alt="Edit Contact Form Module [contact form] Configuration" src="<?php echo $pseudo_directory_offset?>graphics/mediaicons/32x32/editorial.png" /></a>
             
          <?
          }
          ?>
          