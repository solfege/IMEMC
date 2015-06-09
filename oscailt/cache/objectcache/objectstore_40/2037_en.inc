<?php global $pseudo_directory_offset; $site_file_store = $pseudo_directory_offset."attachments/sites/default/"; 
$oscailt_page_setting = Array("site_module_settings" => Array("siteobject" => "1", "newswireobject" => "2126", "articleobject" => "53", "featureobject" => "54", "galleryobject" => "64", "searchobject" => "2212", "archiveobject" => "55", "contactobject" => "84", "commentsobject" => "", "publishobject" => "57", "eventsobject" => "58"), "site_mail_settings" => Array("to" => "news@imemc.org", "from" => "site@imemc.org", "subject_prefix" => "Article edit:", "replyto" => "news@imemc.org", "level" => "none"), "site_filters" => Array("topic" => Array("0" => "*"), "region" => Array("0" => "20", "1" => "19", "2" => "18", "3" => "17", "4" => "16", "5" => "15", "6" => "14", "7" => "13", "8" => "12", "9" => "11", "10" => "10", "11" => "9", "12" => "8", "13" => "7", "14" => "6", "15" => "5", "16" => "4", "17" => "3", "18" => "2", "19" => "1"), "language" => Array("0" => "en"), "type" => Array("0" => "*"), "author" => ""), "obj_id" => "2037", "obj_type" => "DOCUMENT", "site_id" => "1");
$oscailt_story_query = new StoryQuery();
$oscailt_story_query->loadFormValues();
$PAGE_LAYOUT_ID = 2032; ?>
<?
         addToPageDescription("The IMEMC is a media collective. We are independent journalists producing and distributing the authentic voices of the people.  We are located in Beit Sahour, in the Occupied West Bank, and we welcome new volunteers and contributors.");
      addToPageTitle("Getting Involved");
         addToPageDescription("Getting Involved in the International Middle East Media Center (IMEMC) group - our mailing lists and meetings"); ?>

<!-- <getting involved obj id 2037> -->
<h1 class='indypagebody'>Getting Involved with the International Middle East Media Center (IMEMC)</h1><BLOCKQUOTE class='indypagebody'>The International Middle East Media Center (IMEMC) website is run in an open and free atmosphere. We welcome
participation and contributions from everyone who feels they have
something constructive to add. You don\'t have to be a member to contribute articles. We collaborate on the mailing
lists and have face to face meetings every month or as the
need arises.</BLOCKQUOTE><DIV class='indypagebody'><H4>Mailing List</H4> 
<p>In addition to the collective on the ground in Palestine, we also have any contributors from around the world who use the email list to communicate with us and with each other. Join the mailing list to get involved, to ask questions and hear about upcoming events and how you can help. 

<ul>
<li><a href="http://lists.indymedia.org/mailman/listinfo/imc-palestine">http://lists.indymedia.org/mailman/listinfo/imc-palestine</a> 

</li></ul>
</p>

<p> 

<!-- Tech list details --> 

<b>Oscailt list</b><br> 

Our site is running the Oscailt software, created by Indymedia Ireland. If you are interested in producing free software, the Oscailt team is always looking for developers, testers, documentation writers and just about anybody to help out with developing the code. If you can spare some time please join the oscailt list at: 

<ul> 

<li><a href="http://lists.indymedia.org/mailman/listinfo/imc-ireland-tech">http://lists.indymedia.org/mailman/listinfo/oscailt</a> 

</li></ul></DIV>
<!-- </getting involved obj id 2037> -->


         <?php
          global $editor_session, $pseudo_directory_offset;
          if(($editor_session->canEditLayout()|| $editor_session->canEditContent()) && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjdocument", 1)))
          {
          ?>
             <br>
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=document&amp;target_id=2037&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0  align="left" title="Edit Static Document [getting involved] Configuration" alt="Edit Static Document [getting involved] Configuration" src="<?php echo $pseudo_directory_offset?>graphics/mediaicons/32x32/editorial2.png" /></a>
             
          <?
          }
          ?>
          