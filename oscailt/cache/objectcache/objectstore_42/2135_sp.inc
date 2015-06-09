

<!-- <MenÃ?Âº de la izquierda para ver el obj id 2135> -->

         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjmenu", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=menu&amp;target_id=2135&amp;action=edit&amp;site_id=1&amp;obj_language=ot&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Vertical Menu [MenÃ?Âº de la izquierda para ver el] Configuration" alt="Edit Vertical Menu [MenÃ?Âº de la izquierda para ver el] Configuration" src="<?php echo $graphics_store?>editmenu.gif" /></a>
             
          <?
          }
          ?>
       
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
<div class='<?php echo $page_style; ?>'>
<h4 class='<?php echo $page_style; ?>'>Donate Online</h4><BLOCKQUOTE class='<?php echo $page_style; ?>'><center>
<a href="https://co.clickandpledge.com/default.aspx?wid=17066" target="_blank"> <IMG src="/graphics/clickandpledge.jpg" width="172" height="41" border="0" alt="ClickandPledge" title="ClickandPledge"></a>
<hr>
</center></BLOCKQUOTE><DIV class='<?php echo $page_style; ?>'><center>
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
</div>
<!-- </Online Donation obj id 2158> -->


         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjlist", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=list&amp;target_id=67&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Listing Box [site navigation] Configuration" alt="Edit Listing Box [site navigation] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <site navigation obj id 67> -->
<div class='<?php echo $page_style; ?>'>
<h4 class='<?php echo $page_style; ?>'>Navigation</h4><P class='<?php echo $page_style; ?>'>
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
<a href='<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=2126' class='<?php echo $page_style; ?>' title='Latest News Articles'>Latest News Articles</a>
<!-- </IMEMC News obj id 2126> -->

</P>
<P class='<?php echo $page_style; ?>'>
         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjnewswire", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=newswire&amp;target_id=2128&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Newswire Module [audio] Configuration" alt="Edit Newswire Module [audio] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <audio obj id 2128> -->
<a href='<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=2128' class='<?php echo $page_style; ?>' title='Latest Audio'>Latest Audio</a>
<!-- </audio obj id 2128> -->

</P>
<P class='<?php echo $page_style; ?>'>
         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjnewswire", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=newswire&amp;target_id=2123&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Newswire Module [human interest] Configuration" alt="Edit Newswire Module [human interest] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <human interest obj id 2123> -->
<a href='<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=2123' class='<?php echo $page_style; ?>' title='Human Interest'>Human Interest</a>
<!-- </human interest obj id 2123> -->

</P>
<P class='<?php echo $page_style; ?>'>
         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjnewswire", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=newswire&amp;target_id=2118&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Newswire Module [opinion/analysis] Configuration" alt="Edit Newswire Module [opinion/analysis] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <opinion/analysis obj id 2118> -->
<a href='<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=2118' class='<?php echo $page_style; ?>' title='Opinion/Analysis'>Opinion/Analysis</a>
<!-- </opinion/analysis obj id 2118> -->

</P>
<P class='<?php echo $page_style; ?>'>
         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjnewswire", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=newswire&amp;target_id=2143&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Newswire Module [Interviews] Configuration" alt="Edit Newswire Module [Interviews] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <Interviews obj id 2143> -->
<a href='<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=2143' class='<?php echo $page_style; ?>' title='Interviews'>Interviews</a>
<!-- </Interviews obj id 2143> -->

</P>
<P class='<?php echo $page_style; ?>'>
         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjfeature", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=feature&amp;target_id=54&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Featurewire Module [features] Configuration" alt="Edit Featurewire Module [features] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <features obj id 54> -->
<a href='<?php echo $pseudo_directory_offset ?>features' class='<?php echo $page_style; ?>' title='features'>features</a>
<!-- </features obj id 54> -->

</P>


</div>
<!-- </site navigation obj id 67> -->


         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjcategorylist", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=categorylist&amp;target_id=2130&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Category Listing [Site Topics] Configuration" alt="Edit Category Listing [Site Topics] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <Site Topics obj id 2130> -->
<div class='<?php echo $page_style; ?>'>
<h4 class='<?php echo $page_style; ?>'>Site Topics</h4><?php require_once "objects/indyruntime/categorylistdisplay.inc";

         $site_module = new categorylistDisplay(2130);
         $site_module->setFilters(Array("*"), Array("20", "19", "18", "17", "16", "15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1"), Array("en"), Array("*"), Array(""));
         $object_display_settings = (Array("siteobject" => "1", "newswireobject" => "2126", "articleobject" => "53", "featureobject" => "54", "galleryobject" => "64", "searchobject" => "2212", "archiveobject" => "55", "contactobject" => "84", "commentsobject"=> "0", "publishobject" => "57", "eventsobject" => "58"));
         $site_module->setSiteSettings($object_display_settings);
      	$site_module->setAdminOptions(Array("listing" => "topic", "divider" => "3", "linksto" => "50"));
	$site_module->setUserOptions(Array());

         $site_module->retrieveHeadlines();
       ?>
</div>
<!-- </Site Topics obj id 2130> -->


         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjlist", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=list&amp;target_id=27&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Listing Box [documentation list] Configuration" alt="Edit Listing Box [documentation list] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <documentation list obj id 27> -->
<div class='<?php echo $page_style; ?>'>
<h4 class='<?php echo $page_style; ?>'><a href='<?php echo $pseudo_directory_offset ?>documentation_list' class='<?php echo $page_style; ?>'>Documents</a></h4><UL class='<?php echo $page_style; ?>'>
<LI class='<?php echo $page_style; ?>'>
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
<a href='<?php echo $pseudo_directory_offset ?>about_us' class='<?php echo $page_style; ?>' title='About us'>About us</a>
<!-- </About us obj id 24> -->

</LI>
<LI class='<?php echo $page_style; ?>'>
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
<a href='<?php echo $pseudo_directory_offset ?>editorial' class='<?php echo $page_style; ?>' title='Editorial Guidelines'>Editorial Guidelines</a>
<!-- </editorial obj id 2038> -->

</LI>
<LI class='<?php echo $page_style; ?>'>
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
<a href='<?php echo $pseudo_directory_offset ?>privacy' class='<?php echo $page_style; ?>' title='privacy'>privacy</a>
<!-- </privacy obj id 2042> -->

</LI>
<LI class='<?php echo $page_style; ?>'>
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
<a href='<?php echo $pseudo_directory_offset ?>disclaimer' class='<?php echo $page_style; ?>' title='disclaimer'>disclaimer</a>
<!-- </disclaimer obj id 2041> -->

</LI>
<LI class='<?php echo $page_style; ?>'>
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
<a href='<?php echo $pseudo_directory_offset ?>getting_involved' class='<?php echo $page_style; ?>' title='get involved'>get involved</a>
<!-- </getting involved obj id 2037> -->

</LI>
</UL>

</div>
<!-- </documentation list obj id 27> -->


         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjlist", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=list&amp;target_id=2100&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Listing Box [Feeds] Configuration" alt="Edit Listing Box [Feeds] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <Feeds obj id 2100> -->
<div class='<?php echo $page_style; ?>'>
<h4 class='<?php echo $page_style; ?>'><a href='<?php echo $pseudo_directory_offset ?>feeds' class='<?php echo $page_style; ?>'>Feeds</a></h4><UL class='<?php echo $page_style; ?>'>
<LI class='<?php echo $page_style; ?>'>
         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjlink", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=link&amp;target_id=2103&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Simple Link [Atom Summaries Link] Configuration" alt="Edit Simple Link [Atom Summaries Link] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <Atom Summaries Link obj id 2103> -->
<a href='http://www.imemc.org/atom.xml' class='<?php echo $page_style; ?>' title='An atom feed of article summaries'><img class='object-icon' alt='Atom Summaries Link' title='Atom Summaries Link' src='<?php global $pseudo_directory_offset; echo $pseudo_directory_offset ?>attachments/sites/site_1/atomsummariesbutton.png'></a>
<!-- </Atom Summaries Link obj id 2103> -->

</LI>
<LI class='<?php echo $page_style; ?>'>
         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjlink", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=link&amp;target_id=2104&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Simple Link [Atom Full Posts Link] Configuration" alt="Edit Simple Link [Atom Full Posts Link] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <Atom Full Posts Link obj id 2104> -->
<a href='<? echo $pseudo_directory_offset; ?>atomfullposts.xml' class='<?php echo $page_style; ?>' title='An Atom Feed of articles with full content'><img class='object-icon' alt='Atom Full Posts Link' title='Atom Full Posts Link' src='<?php global $pseudo_directory_offset; echo $pseudo_directory_offset ?>attachments/sites/site_1/atomfullpostsbutton.png'></a>
<!-- </Atom Full Posts Link obj id 2104> -->

</LI>
<LI class='<?php echo $page_style; ?>'>
         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjlink", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=link&amp;target_id=2101&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Simple Link [RSS Summaries Link] Configuration" alt="Edit Simple Link [RSS Summaries Link] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <RSS Summaries Link obj id 2101> -->
<a href='<? echo $pseudo_directory_offset; ?>rss.xml' class='<?php echo $page_style; ?>' title='An RSS feed of article summaries'><img class='object-icon' alt='RSS Summaries Link' title='RSS Summaries Link' src='<?php global $pseudo_directory_offset; echo $pseudo_directory_offset ?>attachments/sites/site_1/rsssummariesbutton.png'></a>
<!-- </RSS Summaries Link obj id 2101> -->

</LI>
<LI class='<?php echo $page_style; ?>'>
         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjlink", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=link&amp;target_id=2102&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Simple Link [RSS Full Posts Link] Configuration" alt="Edit Simple Link [RSS Full Posts Link] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <RSS Full Posts Link obj id 2102> -->
<a href='<? echo $pseudo_directory_offset; ?>rssfullposts.xml' class='<?php echo $page_style; ?>' title='A feed articles containing their full content'><img class='object-icon' alt='RSS Full Posts Link' title='RSS Full Posts Link' src='<?php global $pseudo_directory_offset; echo $pseudo_directory_offset ?>attachments/sites/site_1/rssfullpostsbutton.png'></a>
<!-- </RSS Full Posts Link obj id 2102> -->

</LI>
<LI class='<?php echo $page_style; ?>'>
         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjlink", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=link&amp;target_id=2105&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Simple Link [Podcast Link] Configuration" alt="Edit Simple Link [Podcast Link] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <Podcast Link obj id 2105> -->
<a href='<? echo $pseudo_directory_offset; ?>podcast.xml' class='<?php echo $page_style; ?>' title='A podcast of the sites audio and video content'><img class='object-icon' alt='Podcast Link' title='Podcast Link' src='<?php global $pseudo_directory_offset; echo $pseudo_directory_offset ?>attachments/sites/site_1/rsspodcastbutton.png'></a>
<!-- </Podcast Link obj id 2105> -->

</LI>
<LI class='<?php echo $page_style; ?>'>
         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjlink", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=link&amp;target_id=2106&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Simple Link [iTunes Store Podcast Link] Configuration" alt="Edit Simple Link [iTunes Store Podcast Link] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <iTunes Store Podcast Link obj id 2106> -->
<a href='itms://phobos.apple.com/WebObjects/MZStore.woa/wa/viewPodcast?id=120904798' class='<?php echo $page_style; ?>' title='A link to the sites podcast in the iTunes music store'><img class='object-icon' alt='iTunes Store Podcast Link' title='iTunes Store Podcast Link' src='<?php global $pseudo_directory_offset; echo $pseudo_directory_offset ?>attachments/sites/site_1/itunesbutton.gif'></a>
<!-- </iTunes Store Podcast Link obj id 2106> -->

</LI>
</UL>

</div>
<!-- </Feeds obj id 2100> -->


         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjpreferences", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=preferences&amp;target_id=2031&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit User Preferences [user preference box] Configuration" alt="Edit User Preferences [user preference box] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <user preference box obj id 2031> -->
<div class='<?php echo $page_style; ?>'>
<?php require_once "objects/indyruntime/preferencesdisplay.inc";

         $site_module = new preferencesDisplay(2031);
         $site_module->setFilters(Array("*"), Array("20", "19", "18", "17", "16", "15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1"), Array("en"), Array("*"), Array(""));
         $object_display_settings = (Array("siteobject" => "1", "newswireobject" => "2126", "articleobject" => "53", "featureobject" => "54", "galleryobject" => "64", "searchobject" => "2212", "archiveobject" => "55", "contactobject" => "84", "commentsobject"=> "0", "publishobject" => "57", "eventsobject" => "58"));
         $site_module->setSiteSettings($object_display_settings);
      $site_module->setAdminOptions(Array("title" => "user preferences", "display" => "Menu", "hsize" => "4", "class" => "", "fontsizetext" => "text size", "languagetext" => "Language", "profiletext" => "Edit Your Profile", "logouttext" => "Logout", "normaltext" => "Normal", "alltext" => "Edit All", "contenttext" => "Edit Content", "sitebuildertext" => "Edit Layout", "editmodetext" => "Set Editor Mode >>", "allprefstext" => "All Preferences", "oscailthometext" => "make this your IMEMC front page", "admintext" => "Oscailt Administration", "userstatustext" => "", "showfontsize" => "false", "showlanguage" => "true", "showoscailthome" => "false", "showaddbookmark" => "false", "showallprefs" => "false", "showeditmode" => "false", "showlogout" => "true", "showprofile" => "false", "showuserstatus" => "false"));

$site_module->setUserOptions(array());

         $site_module->retrieveHeadlines();
       ?>
</div>
<!-- </user preference box obj id 2031> -->


<!-- </MenÃ?Âº de la izquierda para ver el obj id 2135> -->

