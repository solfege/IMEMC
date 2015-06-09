<?php global $pseudo_directory_offset; $site_file_store = $pseudo_directory_offset."attachments/sites/default/"; 
$oscailt_page_setting = Array("site_module_settings" => Array("siteobject" => "1", "newswireobject" => "2126", "articleobject" => "53", "featureobject" => "54", "galleryobject" => "64", "searchobject" => "2212", "archiveobject" => "55", "contactobject" => "84", "commentsobject" => "", "publishobject" => "57", "eventsobject" => "58"), "site_mail_settings" => Array("to" => "news@imemc.org", "from" => "site@imemc.org", "subject_prefix" => "Article edit:", "replyto" => "news@imemc.org", "level" => "none"), "site_filters" => Array("topic" => Array("0" => "*"), "region" => Array("0" => "20", "1" => "19", "2" => "18", "3" => "17", "4" => "16", "5" => "15", "6" => "14", "7" => "13", "8" => "12", "9" => "11", "10" => "10", "11" => "9", "12" => "8", "13" => "7", "14" => "6", "15" => "5", "16" => "4", "17" => "3", "18" => "2", "19" => "1"), "language" => Array("0" => "en"), "type" => Array("0" => "*"), "author" => ""), "obj_id" => "24", "obj_type" => "DOCUMENT", "site_id" => "1");
$oscailt_story_query = new StoryQuery();
$oscailt_story_query->loadFormValues();
$PAGE_LAYOUT_ID = 2032; ?>
<?
         addToPageDescription("The IMEMC is a media collective. We are independent journalists producing and distributing the authentic voices of the people.  We are located in Beit Sahour, in the Occupied West Bank, and we welcome new volunteers and contributors.");
      addToPageTitle("About Us");
         addToPageDescription("The document describing the basic principles of the International Middle East Media Center."); ?>

<!-- <About us obj id 24> -->
<h1 class='indypagebody'>About the International Middle East Media Center</h1><BLOCKQUOTE class='indypagebody'>IMEMC is a media center developed in collaboration between Palestinian and International journalists to provide independent media coverage of Israel-Palestine.</BLOCKQUOTE><DIV class='indypagebody'>IMEMC was founded by the Palestinian Centre for Rapprochement between People in 2003. (<a href="http://www.pcr.ps" target="_blank">www.pcr.ps</a>)<br /><br />

<p> Being a joint Palestinian-International effort, IMEMC combines Palestinian journalists' deep understanding of the context, history, and the socio-political environment with International journalists' skills in non-partisan reporting. </p>
IMEMC provides fair and comprehensive coverage of events and developments in Israel-Palestine.<br />
<br />
In 2008, IMEMC Started to provide Spanish Language coverage as an attempt to outreach a wider readership. The Spanish edition of IMEMC was completely run by volunteers, from Barcelona, Palestine and other places. The Spanish service has stopped due to lack of funding. We are still interested in providing news in other languages, thus your help is needed.<br />
<br />

<p> IMEMC provides coverage of news, political developments and daily incidents combined with feature stories, political analysis, interviews and selected opinion pieces. </p>
<p> IMEMC provides a daily audible news cast in English language, which provides nearly five minutes featuring main incidents of the day.<br />
<br />
In addition IMEMC produces a weekly audio summary of socio-political developments in Israel-Palestine to keep you updated.<br />
<br />
 IMEMC also provides field reports on main issues of interest to its targeted audience. </p>
<p><strong> What you need to know to make the most of the IMEMC news website:</strong> </p>
<p> In the center of the IMEMC site, the most recent news articles, covering the main socio-political developments, are shown. </p>
<p> The 'latest news' section provides an up to the hour coverage of developments. The list is meant to update readers on incidents taking place in all West Bank and Gaza Strip areas. Information is mainly provided by IMEMC affiliates and collaborators in the field. </p>
<p> The human interest section presents stories which give a more personal side to the conflict. </p>
<p>Opinion/Analysis articles can be contributed to the site by any number of contributing writers, from both inside and outside Palestine. Including you!</p>
<p> As you open any article, a list of articles related to the topic presented is available in a sidebox. These are meant to provide readers of specific interests with a convenient and easy way to look at other developments related to a specific issue. You can also use the filters to view all articles of a certain type, area, or topic that particularly interests you.</p> <br /></DIV>
<!-- </About us obj id 24> -->

<?php getPageLanguageOptions("en", Array("0" => "en", "1" => "sp"), "24"); ?>
         <?php
          global $editor_session, $pseudo_directory_offset;
          if(($editor_session->canEditLayout()|| $editor_session->canEditContent()) && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjdocument", 1)))
          {
          ?>
             <br>
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=document&amp;target_id=24&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0  align="left" title="Edit Static Document [About us] Configuration" alt="Edit Static Document [About us] Configuration" src="<?php echo $pseudo_directory_offset?>graphics/mediaicons/32x32/editorial2.png" /></a>
             
          <?
          }
          ?>
          