<?php global $pseudo_directory_offset; $site_file_store = $pseudo_directory_offset."attachments/sites/default/"; 
$oscailt_page_setting = Array("site_module_settings" => Array("siteobject" => "1", "newswireobject" => "2126", "articleobject" => "53", "featureobject" => "54", "galleryobject" => "64", "searchobject" => "2212", "archiveobject" => "55", "contactobject" => "84", "commentsobject" => "", "publishobject" => "57", "eventsobject" => "58"), "site_mail_settings" => Array("to" => "news@imemc.org", "from" => "site@imemc.org", "subject_prefix" => "Article edit:", "replyto" => "news@imemc.org", "level" => "none"), "site_filters" => Array("topic" => Array("0" => "*"), "region" => Array("0" => "20", "1" => "19", "2" => "18", "3" => "17", "4" => "16", "5" => "15", "6" => "14", "7" => "13", "8" => "12", "9" => "11", "10" => "10", "11" => "9", "12" => "8", "13" => "7", "14" => "6", "15" => "5", "16" => "4", "17" => "3", "18" => "2", "19" => "1"), "language" => Array("0" => "en"), "type" => Array("0" => "*"), "author" => ""), "obj_id" => "2108", "obj_type" => "DOCUMENT", "site_id" => "1");
$oscailt_story_query = new StoryQuery();
$oscailt_story_query->loadFormValues();
$PAGE_LAYOUT_ID = 2032; ?>
<?
         addToPageDescription("The IMEMC is a media collective. We are independent journalists producing and distributing the authentic voices of the people.  We are located in Beit Sahour, in the Occupied West Bank, and we welcome new volunteers and contributors.");
      addToPageTitle("Open Publishing");
         addToPageDescription("A brief description of  the International Middle East Media Center (IMEMC)\'s use of open publishing"); ?>

<!-- <open publishing obj id 2108> -->
<h1 class='indypagebody'>What is the Indymedia Newswire?</h1><BLOCKQUOTE class='indypagebody'>The Indymedia Ireland newswire works on the principle of Open
Publishing, an essential element of the Indymedia project that allows anyone to
instantaneously self-publish their work on a globally accessible web
site. The Indymedia newswire encourages people to become the media by
posting their articles, analysis, and photographs directly to the web
site. You can publish to the newswire by clicking the "publish" link
and following the easy instructions.</BLOCKQUOTE><DIV class='indypagebody'><p>Indymedia relies on the people who post to the Indymedia
newswire to present their information in a thorough, honest, accurate
manner. While Indymedia reserves the right to develop sections of the
site that provide edited articles, there is no designated Indymedia
editorial collective that edits articles posted to the newswire. At any
given time several editors keep track of what is being posted and apply
basic editorial guidelines to keep the newswire free of spam and
duplicate postings.
</p><p>You can find the current Indymedia editorial guidelines at the
top of the page you reach after clicking the "publish" link. All
stories and comments removed from the the newswire are posted to a
mailing list which which anyone can join to discuss deletions and ask
for their reinstatement.
</p><p>If you disagree with the content of a particular article that
someone has posted on Indymedia, you may comment on the article through
the "add your comments" link at the bottom of each post.
</p><p>
Find out more about <a href="http://www.physics.usyd.edu.au/%7Ematthewa/catk/openpub.html">OPEN PUBLISHING</a>.
<br>&nbsp;</DIV>
<!-- </open publishing obj id 2108> -->


         <?php
          global $editor_session, $pseudo_directory_offset;
          if(($editor_session->canEditLayout()|| $editor_session->canEditContent()) && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjdocument", 1)))
          {
          ?>
             <br>
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=document&amp;target_id=2108&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0  align="left" title="Edit Static Document [open publishing] Configuration" alt="Edit Static Document [open publishing] Configuration" src="<?php echo $pseudo_directory_offset?>graphics/mediaicons/32x32/editorial2.png" /></a>
             
          <?
          }
          ?>
          