<?php global $pseudo_directory_offset; $site_file_store = $pseudo_directory_offset."attachments/sites/default/"; 
$oscailt_page_setting = Array("site_module_settings" => Array("siteobject" => "1", "newswireobject" => "2126", "articleobject" => "53", "featureobject" => "54", "galleryobject" => "64", "searchobject" => "2212", "archiveobject" => "55", "contactobject" => "84", "commentsobject" => "", "publishobject" => "57", "eventsobject" => "58"), "site_mail_settings" => Array("to" => "news@imemc.org", "from" => "site@imemc.org", "subject_prefix" => "Article edit:", "replyto" => "news@imemc.org", "level" => "none"), "site_filters" => Array("topic" => Array("0" => "*"), "region" => Array("0" => "20", "1" => "19", "2" => "18", "3" => "17", "4" => "16", "5" => "15", "6" => "14", "7" => "13", "8" => "12", "9" => "11", "10" => "10", "11" => "9", "12" => "8", "13" => "7", "14" => "6", "15" => "5", "16" => "4", "17" => "3", "18" => "2", "19" => "1"), "language" => Array("0" => "en"), "type" => Array("0" => "*"), "author" => ""), "obj_id" => "2041", "obj_type" => "DOCUMENT", "site_id" => "1");
$oscailt_story_query = new StoryQuery();
$oscailt_story_query->loadFormValues();
$PAGE_LAYOUT_ID = 2032; ?>
<?
         addToPageDescription("The IMEMC is a media collective. We are independent journalists producing and distributing the authentic voices of the people.  We are located in Beit Sahour, in the Occupied West Bank, and we welcome new volunteers and contributors.");
      addToPageTitle("Disclaimer");
         addToPageDescription("Content (text, image, audio, or video) posted to this web site is owned
by its author. Unless otherwise stated by the author, all content is
free for non-commercial reuse, reprint, and rebroadcast, on the net and
elsewhere. Opinions are those of the people contributing to the site
and are not necessarily endorsed by the International Middle East Media Center (IMEMC)."); ?>

<!-- <disclaimer obj id 2041> -->
<h1 class='indypagebody'>Legal Disclaimer</h1><BLOCKQUOTE class='indypagebody'>Content (text, image, audio, or video) posted to this web site is owned
by its author. Unless otherwise stated by the author, all content is
free for non-commercial reuse, reprint, and rebroadcast, on the net and
elsewhere. Opinions are those of the people contributing to the site
and are not necessarily endorsed by the International Middle East Media Center (IMEMC).</BLOCKQUOTE><DIV class='indypagebody'><p>As an 
all-volunteer organization, the International Middle East Media Center (IMEMC) does not have the means 
to guarantee the accuracy of news or opinions posted to the newswire. 
</p><p>Because, unless otherwise stated, site content is licensed free 
of charge for non-commercial use, there is no warranty for the content. 
Except when otherwise stated in writing, the Indymedia Ireland 
collective and/or other parties provide the content "as is" without 
warranty of any kind, either expressed or implied, including, but not 
limited to, the implied warranties of merchantability and fitness for a 
particular purpose. The entire risk of use of the content is with you. 
Should the content prove faulty, inaccurate, or otherwise unacceptable 
you assume the cost of all necessary repair or correction. 
</p><p>In no event unless agreed to in writing will the Indymedia 
Ireland collective, or any other party who may mirror and/or 
redistribute the content as permitted above, be liable to you for 
damages, including any general, special, incidental or consequential 
damages arising out of the use or inability to use the content, even if 
the Indymedia Ireland collective or other party has been advised of the 
possibility of such damages. 
</p><p> 
Last updated October 2006</p></DIV>
<!-- </disclaimer obj id 2041> -->


         <?php
          global $editor_session, $pseudo_directory_offset;
          if(($editor_session->canEditLayout()|| $editor_session->canEditContent()) && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjdocument", 1)))
          {
          ?>
             <br>
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=document&amp;target_id=2041&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0  align="left" title="Edit Static Document [disclaimer] Configuration" alt="Edit Static Document [disclaimer] Configuration" src="<?php echo $pseudo_directory_offset?>graphics/mediaicons/32x32/editorial2.png" /></a>
             
          <?
          }
          ?>
          