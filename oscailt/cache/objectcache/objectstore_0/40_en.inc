<?php global $pseudo_directory_offset; $site_file_store = $pseudo_directory_offset."attachments/sites/default/"; 
$oscailt_page_setting = Array("site_module_settings" => Array("siteobject" => "1", "newswireobject" => "50", "articleobject" => "53", "featureobject" => "54", "galleryobject" => "64", "searchobject" => "41", "archiveobject" => "55", "contactobject" => "84", "commentsobject" => "56", "publishobject" => "57", "eventsobject" => "58"), "site_mail_settings" => Array("to" => "", "from" => "", "subject_prefix" => "", "replyto" => "", "level" => "high"), "site_filters" => Array("topic" => Array("0" => "*"), "region" => Array("0" => "*"), "language" => Array("0" => "*"), "type" => Array("0" => "*"), "author" => ""), "obj_id" => "40", "obj_type" => "CITYLIST", "site_id" => "1");
$oscailt_story_query = new StoryQuery();
$oscailt_story_query->loadFormValues();
$PAGE_LAYOUT_ID = 2032; ?>
<?
         addToPageDescription("Indymedia Ireland is a media collective. We are independent volunteer journalists producing and distributing the authentic voices of the people");
      addToPageTitle("Indymedia City Listing");
         addToPageDescription("A listing of all the cities in the Indymedia Global Network ."); ?>

<!-- <indymedia city listing obj id 40> -->
<h1 class='indypagebody'>Global Independent Media Centre Network</h1><P class='indypagebody'>The Global Network of Independent Media Centres - aka Indymedia. It started in 1999 in Seattle with a media centre and a website to organise coverage of the protests against the WTO there. That was then, this is now:</P><H3 class='indypagebody'><B class='indypagebody'>Global:</B></H3>
<P class='indypagebody'><a class='indypagebody' href='http://www.indymedia.org'>Global site</a> </P>
<!-- </indymedia city listing obj id 40> -->


         <?php
          global $editor_session, $pseudo_directory_offset;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjcitylist", 1)))
          {
          ?>
             <br>
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=citylist&amp;target_id=40&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0  align="left" title="Edit IMC City Listing [indymedia city listing] Configuration" alt="Edit IMC City Listing [indymedia city listing] Configuration" src="<?php echo $pseudo_directory_offset?>graphics/mediaicons/32x32/indymedia_gold.png" /></a>
             
          <?
          }
          ?>
          