<?php global $pseudo_directory_offset; $site_file_store = $pseudo_directory_offset."attachments/sites/default/"; 
$oscailt_page_setting = Array("site_module_settings" => Array("siteobject" => "1", "newswireobject" => "2126", "articleobject" => "53", "featureobject" => "54", "galleryobject" => "64", "searchobject" => "2212", "archiveobject" => "55", "contactobject" => "84", "commentsobject" => "", "publishobject" => "57", "eventsobject" => "58"), "site_mail_settings" => Array("to" => "news@imemc.org", "from" => "site@imemc.org", "subject_prefix" => "Article edit:", "replyto" => "news@imemc.org", "level" => "none"), "site_filters" => Array("topic" => Array("0" => "*"), "region" => Array("0" => "20", "1" => "19", "2" => "18", "3" => "17", "4" => "16", "5" => "15", "6" => "14", "7" => "13", "8" => "12", "9" => "11", "10" => "10", "11" => "9", "12" => "8", "13" => "7", "14" => "6", "15" => "5", "16" => "4", "17" => "3", "18" => "2", "19" => "1"), "language" => Array("0" => "en"), "type" => Array("0" => "*"), "author" => ""), "obj_id" => "27", "obj_type" => "LIST", "site_id" => "1");
$oscailt_story_query = new StoryQuery();
$oscailt_story_query->loadFormValues();
$PAGE_LAYOUT_ID = 2032; ?>
<?
         addToPageDescription("The IMEMC is a media collective. We are independent journalists producing and distributing the authentic voices of the people.  We are located in Beit Sahour, in the Occupied West Bank, and we welcome new volunteers and contributors.");
      addToPageTitle("Documentation List");
         addToPageDescription("A list of the major documentation pages on the site"); ?>

<!-- <documentation list obj id 27> -->
<h1 class='indypagebody'>Document Listing</h1><P>This is a collection of some of the basic documents which describe IMEMC and how we work</P><UL class='indypagebody'>
<LI class='indypagebody'>
         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjdocument", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=document&amp;target_id=24&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Static Document [About us] Configuration" alt="Edit Static Document [About us] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <About us obj id 24> -->
<h3 class='indypagebody'><a href='<?php echo $pseudo_directory_offset ?>about_us' class='indypagebody' title='About the International Middle East Media Center'>About the International Middle East Media Center</a></h3><P class='indypagebody'>IMEMC is a media center developed in collaboration between Palestinian and International journalists to provide independent media coverage of Israel-Palestine.</P>
<!-- </About us obj id 24> -->

</LI>
<LI class='indypagebody'>
         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjdocument", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=document&amp;target_id=2038&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Static Document [editorial] Configuration" alt="Edit Static Document [editorial] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <editorial obj id 2038> -->
<h3 class='indypagebody'><a href='<?php echo $pseudo_directory_offset ?>editorial' class='indypagebody' title='International Middle East Media Center's Editorial Guidelines'>International Middle East Media Center's Editorial Guidelines</a></h3><P class='indypagebody'>The International Middle East Media Center is built on the foundation of an open and democratic newswire. We want to see and hear real stories, news, and opinions from users of the site, both from inside Palestine, and in the rest of the world.  At the same time, we are committed to an accurate recounting of events that doesn\'t sensationalize or judge, but presents the reality of the conflict as it occurs in the occupied Palestinian territories, where our collective is located.

Anyone is welcome to contribute to the site - if you are interested in contributing, please read through the editorial policy and make sure your article is in adherence, then click publish and fill out the form to contribute your article to the site.</P>
<!-- </editorial obj id 2038> -->

</LI>
<LI class='indypagebody'>
         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjdocument", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=document&amp;target_id=2042&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Static Document [privacy] Configuration" alt="Edit Static Document [privacy] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <privacy obj id 2042> -->
<h3 class='indypagebody'><a href='<?php echo $pseudo_directory_offset ?>privacy' class='indypagebody' title='International Middle East Media Center Privacy Policy'>International Middle East Media Center Privacy Policy</a></h3><P class='indypagebody'>The International Middle East Media Center does not normally retain server logs of activity on the website.   In special cases, where persistent patterns of abuse are noticed, we may decide to use the logs to identify the source of the abuse. The information in these logs is not shared with any other body and is not used for any other purposes other than ensuring the secure functioning of the site.</P>
<!-- </privacy obj id 2042> -->

</LI>
<LI class='indypagebody'>
         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjdocument", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=document&amp;target_id=2041&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Static Document [disclaimer] Configuration" alt="Edit Static Document [disclaimer] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <disclaimer obj id 2041> -->
<h3 class='indypagebody'><a href='<?php echo $pseudo_directory_offset ?>disclaimer' class='indypagebody' title='Legal Disclaimer'>Legal Disclaimer</a></h3><P class='indypagebody'>Content (text, image, audio, or video) posted to this web site is owned
by its author. Unless otherwise stated by the author, all content is
free for non-commercial reuse, reprint, and rebroadcast, on the net and
elsewhere. Opinions are those of the people contributing to the site
and are not necessarily endorsed by the International Middle East Media Center (IMEMC).</P>
<!-- </disclaimer obj id 2041> -->

</LI>
<LI class='indypagebody'>
         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjdocument", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=document&amp;target_id=2037&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Static Document [getting involved] Configuration" alt="Edit Static Document [getting involved] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <getting involved obj id 2037> -->
<h3 class='indypagebody'><a href='<?php echo $pseudo_directory_offset ?>getting_involved' class='indypagebody' title='Getting Involved with the International Middle East Media Center (IMEMC)'>Getting Involved with the International Middle East Media Center (IMEMC)</a></h3><P class='indypagebody'>The International Middle East Media Center (IMEMC) website is run in an open and free atmosphere. We welcome
participation and contributions from everyone who feels they have
something constructive to add. You don\'t have to be a member to contribute articles. We collaborate on the mailing
lists and have face to face meetings every month or as the
need arises.</P>
<!-- </getting involved obj id 2037> -->

</LI>
</UL>

<!-- </documentation list obj id 27> -->


         <?php
          global $editor_session, $pseudo_directory_offset;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjlist", 1)))
          {
          ?>
             <br>
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=list&amp;target_id=27&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0  align="left" title="Edit Listing Box [documentation list] Configuration" alt="Edit Listing Box [documentation list] Configuration" src="<?php echo $pseudo_directory_offset?>graphics/mediaicons/32x32/listingbox.png" /></a>
             
          <?
          }
          ?>
          