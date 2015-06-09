<?php global $pseudo_directory_offset; $site_file_store = $pseudo_directory_offset."attachments/sites/default/"; 
$oscailt_page_setting = Array("site_module_settings" => Array("siteobject" => "1", "newswireobject" => "50", "articleobject" => "53", "featureobject" => "54", "galleryobject" => "64", "searchobject" => "41", "archiveobject" => "55", "contactobject" => "84", "commentsobject" => "56", "publishobject" => "57", "eventsobject" => "58"), "site_mail_settings" => Array("to" => "", "from" => "", "subject_prefix" => "", "replyto" => "", "level" => "high"), "site_filters" => Array("topic" => Array("0" => "*"), "region" => Array("0" => "*"), "language" => Array("0" => "*"), "type" => Array("0" => "*"), "author" => ""), "obj_id" => "2093", "obj_type" => "CONTACT", "site_id" => "1", "page_filters" => Array("topic" => Array("0" => "*"), "region" => Array("0" => "*"), "language" => Array("0" => "*"), "type" => Array("0" => "*"), "author" => Array("0" => "")));
$oscailt_story_query = new StoryQuery();
$oscailt_story_query->loadFormValues();
$PAGE_LAYOUT_ID = 2032; ?>
<?
         addToPageDescription("Indymedia Ireland is a media collective. We are independent volunteer journalists producing and distributing the authentic voices of the people");
      addToPageTitle("Report Posts");
         addToPageDescription("the contact form for reporting abusive posts"); ?><?php require_once "objects/indyruntime/contactdisplay.inc";

         $site_module = new contactDisplay(2093);
         $site_module->setFilters(Array("*"), Array("*"), Array("*"), Array("*"), Array(""));
         $object_display_settings = (Array("siteobject" => "1", "newswireobject" => "50", "articleobject" => "53", "featureobject" => "54", "galleryobject" => "64", "searchobject" => "41", "archiveobject" => "55", "contactobject" => "84", "commentsobject" => "56", "publishobject" => "57", "eventsobject" => "58"));
         $site_module->setSiteSettings($object_display_settings);
      $site_module->setAdminOptions(Array("title" => "Report Post", "header" => "", "footer" => "", "nametitle" => "Your Name", "contentstitle" => "Why should this post be removed?", "subjecttitle" => "The Subject of your Message", "fromtitle" => "Your Email Address", "introtext" => "<div class=\'user-message\'>Please use this form to notify the editorial collective of articles or comments on the site that are against our <a href=\"editorial\">editorial guidelines</a><br> 
<b>Note</b> abuse of this form will not be looked upon kindly. </div> 
<p>You must fill in all fields to submit an email. </p>", "emailto" => "imc-ireland-editorial@lists.indymedia.org", "emailfrom" => "", "emailreply" => "", "formheadtext" => "Fill in the your reason for considering this posting inappropriate", "buttontext" => "Send Message", "successtext" => "Your message has been sent to the editorial collective who run this site. If we agree with your opinion, the offending content will be removed.", "finalchecktext" => "", "spammandatorywords" => "", "editorprivatemail" => "false", "logemails" => "false", "hidesubjectfield" => "false", "spambotcheck" => "false", "logspamcheck" => "false"));

$site_module->setUserOptions(array());

         $site_module->processRequest();
       ?>
         <?php
          global $editor_session, $pseudo_directory_offset;
          if(($editor_session->canEditLayout()|| $editor_session->canEditContent()) && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjcontact", 1)))
          {
          ?>
             <br>
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=contact&amp;target_id=2093&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0  align="left" title="Edit Contact Form Module [report posts] Configuration" alt="Edit Contact Form Module [report posts] Configuration" src="<?php echo $pseudo_directory_offset?>graphics/mediaicons/32x32/editorial.png" /></a>
             
          <?
          }
          ?>
          