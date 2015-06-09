<?php global $pseudo_directory_offset; $site_file_store = $pseudo_directory_offset."attachments/sites/default/"; 
$oscailt_page_setting = Array("site_module_settings" => Array("siteobject" => "1", "newswireobject" => "2126", "articleobject" => "53", "featureobject" => "54", "galleryobject" => "64", "searchobject" => "2212", "archiveobject" => "55", "contactobject" => "84", "commentsobject" => "", "publishobject" => "57", "eventsobject" => "58"), "site_mail_settings" => Array("to" => "news@imemc.org", "from" => "site@imemc.org", "subject_prefix" => "Article edit:", "replyto" => "news@imemc.org", "level" => "none"), "site_filters" => Array("topic" => Array("0" => "*"), "region" => Array("0" => "20", "1" => "19", "2" => "18", "3" => "17", "4" => "16", "5" => "15", "6" => "14", "7" => "13", "8" => "12", "9" => "11", "10" => "10", "11" => "9", "12" => "8", "13" => "7", "14" => "6", "15" => "5", "16" => "4", "17" => "3", "18" => "2", "19" => "1"), "language" => Array("0" => "en"), "type" => Array("0" => "*"), "author" => ""), "obj_id" => "2131", "obj_type" => "BOX", "site_id" => "1");
$oscailt_story_query = new StoryQuery();
$oscailt_story_query->loadFormValues();
$PAGE_LAYOUT_ID = 2032; ?>
<?
         addToPageDescription("The IMEMC is a media collective. We are independent journalists producing and distributing the authentic voices of the people.  We are located in Beit Sahour, in the Occupied West Bank, and we welcome new volunteers and contributors.");
      addToPageTitle("Latest Articles");
         addToPageDescription("all of the latest news articles"); ?>

<!-- <latest articles obj id 2131> -->
<div class='indypagebody'>

         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjbox", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=box&amp;target_id=2131&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Inset Box [latest articles] Configuration" alt="Edit Inset Box [latest articles] Configuration" src="<?php echo $graphics_store?>editmenu.gif" /></a>
             
          <?
          }
          ?>
       
         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjnewswire", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=newswire&amp;target_id=2126&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Newswire Module [IMEMC News] Configuration" alt="Edit Newswire Module [IMEMC News] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <IMEMC News obj id 2126> -->
<h4 class='indy-insetbox'><a href='<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=2126' class='indy-insetbox' title='Latest News Articles'>Latest News Articles</a></h4><?php require_once "objects/indyruntime/newswiredisplay.inc";

         $site_module = new newswireDisplay(2126);
         $site_module->setFilters(Array("*"), Array("20", "19", "18", "17", "16", "15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1", "20", "19", "18", "17", "16", "15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1", "20", "19", "18", "17", "16", "15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1"), Array("en", "en", "en"), Array("2"), Array(""));
         $object_display_settings = (Array("siteobject" => "1", "newswireobject" => "2126", "articleobject" => "53", "featureobject" => "54", "galleryobject" => "64", "searchobject" => "2212", "archiveobject" => "55", "contactobject" => "84", "commentsobject"=> "0", "publishobject" => "57", "eventsobject" => "58"));
         $site_module->setSiteSettings($object_display_settings);
      	$site_module->setAdminOptions(Array("pagelimit" => "10", "maxhlength" => "0", "articleincludedate" => "%H:%M %a %d %b", "includedate" => "", "eventtimeformat" => "", "read_more_text" => "Full Story", "hiddenonly" => "false", "nohidden" => "false", "lockedonly" => "false", "nolocked" => "false", "nosticky" => "false", "stickyonly" => "false", "upcomingevents" => "false", "statusonly" => "false", "includecontext" => "false", "includeicons" => "false", "showhidden" => "true", "showlocked" => "true", "showsticky" => "true", "showstatus" => "false", "usestatusicons" => "true", "ignorehidden" => "false", "respectsticky" => "false", "articleincludeicons" => "false", "includeauthor" => "false", "includeorganisation" => "false", "includecommentcount" => "false", "includephoto" => "true", "highlightfeatures" => "false", "context_sensitive_region" => "false", "context_sensitive_type" => "false", "context_sensitive_topic" => "false", "context_sensitive_language" => "false", "context_sensitive_author" => "false", "promoted" => "false", "emptyresultmsg" => "No news stories at this time.", "filter_author" => "", "time_posted_lower_limit" => "0", "time_posted_upper_limit" => "0", "full_page_id" => "2126"));
	$site_module->setUserOptions(Array());

         $site_module->retrieveHeadlines();
       ?>
<!-- </IMEMC News obj id 2126> -->


         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjdocument", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=document&amp;target_id=2158&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Static Document [Online Donation] Configuration" alt="Edit Static Document [Online Donation] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <Online Donation obj id 2158> -->
<h4 class='indy-insetbox'>Donate Online</h4><BLOCKQUOTE class='indy-insetbox'><center>
<a href="https://co.clickandpledge.com/default.aspx?wid=17066" target="_blank"> <IMG src="/graphics/clickandpledge.jpg" width="172" height="41" border="0" alt="ClickandPledge" title="ClickandPledge"></a>
<hr>
</center></BLOCKQUOTE><DIV class='indy-insetbox'><center>
<br><b>Donations in Euro</b><br />
<form action="https://www.paypal.com/cgi-bin/webscr" method="post">
<input type="hidden" name="cmd" value="_donations">
<input type="hidden" name="business" value="donation@imemc.org">
<input type="hidden" name="item_name" value="IMEMC News">
<input type="hidden" name="page_style" value="PayPal">
<input type="hidden" name="no_shipping" value="1">
<input type="hidden" name="return" value="http://www.imemc.org">
<input type="hidden" name="cn" value="Please leave us a note">
<input type="hidden" name="currency_code" value="EUR">
<input type="hidden" name="tax" value="0">
<input type="hidden" name="bn" value="PP-DonationsBF">
<input type="image" src="/graphics/paypaleuro.jpg" border="0" name="submit" alt="PayPal - Euro">
</form>
<b>Donations in USD</b><br />
 <form action="https://www.paypal.com/cgi-bin/webscr" method="post">
<input type="hidden" name="cmd" value="_donations">
<input type="hidden" name="business" value="donation@imemc.org">
<input type="hidden" name="item_name" value="IMEMC News">
<input type="hidden" name="no_shipping" value="0">
<input type="hidden" name="no_note" value="1">
<input type="hidden" name="currency_code" value="USD">
<input type="hidden" name="tax" value="0">
<input type="hidden" name="lc" value="US">
<input type="hidden" name="bn" value="PP-DonationsBF">
<input type="image" src="/graphics/paypal-dollar.gif" border="0" name="submit" alt="PayPal - Dollar">
</form>
</center></DIV>
<!-- </Online Donation obj id 2158> -->


</div>
<!-- </latest articles obj id 2131> -->

<?php getPageLanguageOptions("en", Array("0" => "en", "1" => "sp"), "2131"); ?>
         <?php
          global $editor_session, $pseudo_directory_offset;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjbox", 1)))
          {
          ?>
             <br>
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=box&amp;target_id=2131&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0  align="left" title="Edit Inset Box [latest articles] Configuration" alt="Edit Inset Box [latest articles] Configuration" src="<?php echo $pseudo_directory_offset?>graphics/mediaicons/32x32/unclassified2.png" /></a>
             
          <?
          }
          ?>
          