

<!-- <Portada menu derecho obj id 45> -->

         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjmenu", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=menu&amp;target_id=45&amp;action=edit&amp;site_id=1&amp;obj_language=ot&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Vertical Menu [Portada menu derecho] Configuration" alt="Edit Vertical Menu [Portada menu derecho] Configuration" src="<?php echo $graphics_store?>editmenu.gif" /></a>
             
          <?
          }
          ?>
       
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
<div class='<?php echo $page_style; ?>'>
<h4 class='<?php echo $page_style; ?>'><a href='<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=2123' class='<?php echo $page_style; ?>' title='Human Interest Stories'>Human Interest Stories</a></h4><?php require_once "objects/indyruntime/newswiredisplay.inc";

         $site_module = new newswireDisplay(2123);
         $site_module->setFilters(Array("76", "36", "35", "34", "33", "32", "15", "24", "23", "22", "21", "25", "26", "10", "30", "16", "28", "27", "3", "20", "31"), Array("20", "19", "18", "17", "16", "15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1", "20", "19", "18", "17", "16", "15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1"), Array("en", "en"), Array("8"), Array(""));
         $object_display_settings = (Array("siteobject" => "1", "newswireobject" => "2126", "articleobject" => "53", "featureobject" => "54", "galleryobject" => "64", "searchobject" => "2212", "archiveobject" => "55", "contactobject" => "84", "commentsobject"=> "0", "publishobject" => "57", "eventsobject" => "58"));
         $site_module->setSiteSettings($object_display_settings);
      	$site_module->setAdminOptions(Array("pagelimit" => "3", "maxhlength" => "60", "articleincludedate" => "", "includedate" => "", "eventtimeformat" => "", "read_more_text" => "", "hiddenonly" => "false", "nohidden" => "false", "lockedonly" => "false", "nolocked" => "false", "nosticky" => "false", "stickyonly" => "false", "upcomingevents" => "false", "statusonly" => "false", "includecontext" => "false", "includeicons" => "false", "showhidden" => "true", "showlocked" => "true", "showsticky" => "true", "showstatus" => "false", "usestatusicons" => "true", "ignorehidden" => "false", "respectsticky" => "false", "articleincludeicons" => "false", "includeauthor" => "true", "includeorganisation" => "false", "includecommentcount" => "false", "includephoto" => "true", "highlightfeatures" => "false", "context_sensitive_region" => "false", "context_sensitive_type" => "false", "context_sensitive_topic" => "false", "context_sensitive_language" => "false", "context_sensitive_author" => "false", "promoted" => "false", "emptyresultmsg" => "Sorry no articles have been posted matching your request.", "filter_author" => "", "time_posted_lower_limit" => "0", "time_posted_upper_limit" => "0", "full_page_id" => "2123"));
	$site_module->setUserOptions(Array());

         $site_module->retrieveHeadlines();
       ?>
</div>
<!-- </human interest obj id 2123> -->


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
<div class='<?php echo $page_style; ?>'>
<h4 class='<?php echo $page_style; ?>'><a href='<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=2118' class='<?php echo $page_style; ?>' title='Opinion and Analysis'>Opinion and Analysis</a></h4><?php require_once "objects/indyruntime/newswiredisplay.inc";

         $site_module = new newswireDisplay(2118);
         $site_module->setFilters(Array("*"), Array("20", "19", "18", "17", "16", "15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1"), Array("en"), Array("3"), Array(""));
         $object_display_settings = (Array("siteobject" => "1", "newswireobject" => "2126", "articleobject" => "53", "featureobject" => "54", "galleryobject" => "64", "searchobject" => "2212", "archiveobject" => "55", "contactobject" => "84", "commentsobject"=> "0", "publishobject" => "57", "eventsobject" => "58"));
         $site_module->setSiteSettings($object_display_settings);
      	$site_module->setAdminOptions(Array("pagelimit" => "3", "maxhlength" => "60", "articleincludedate" => "", "includedate" => "", "eventtimeformat" => "", "read_more_text" => "", "hiddenonly" => "false", "nohidden" => "false", "lockedonly" => "false", "nolocked" => "false", "nosticky" => "false", "stickyonly" => "false", "upcomingevents" => "false", "statusonly" => "false", "includecontext" => "false", "includeicons" => "false", "showhidden" => "true", "showlocked" => "true", "showsticky" => "true", "showstatus" => "false", "usestatusicons" => "true", "ignorehidden" => "false", "respectsticky" => "false", "articleincludeicons" => "false", "includeauthor" => "true", "includeorganisation" => "false", "includecommentcount" => "false", "includephoto" => "true", "highlightfeatures" => "false", "context_sensitive_region" => "false", "context_sensitive_type" => "false", "context_sensitive_topic" => "false", "context_sensitive_language" => "false", "context_sensitive_author" => "false", "promoted" => "false", "emptyresultmsg" => "Sorry, no articles match your request.", "filter_author" => "", "time_posted_lower_limit" => "0", "time_posted_upper_limit" => "0", "full_page_id" => "2118"));
	$site_module->setUserOptions(Array());

         $site_module->retrieveHeadlines();
       ?>
</div>
<!-- </opinion/analysis obj id 2118> -->


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
<div class='<?php echo $page_style; ?>'>
<h4 class='<?php echo $page_style; ?>'><a href='<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=2143' class='<?php echo $page_style; ?>' title='Interviews'>Interviews</a></h4><?php require_once "objects/indyruntime/newswiredisplay.inc";

         $site_module = new newswireDisplay(2143);
         $site_module->setFilters(Array("36", "35", "34", "33", "32", "15", "24", "23", "22", "21", "25", "26", "10", "30", "16", "31", "20", "3", "27", "28"), Array("20", "19", "18", "17", "16", "15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1", "20", "19", "18", "17", "16", "15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1"), Array("en", "en"), Array("9"), Array(""));
         $object_display_settings = (Array("siteobject" => "1", "newswireobject" => "2126", "articleobject" => "53", "featureobject" => "54", "galleryobject" => "64", "searchobject" => "2212", "archiveobject" => "55", "contactobject" => "84", "commentsobject"=> "0", "publishobject" => "57", "eventsobject" => "58"));
         $site_module->setSiteSettings($object_display_settings);
      	$site_module->setAdminOptions(Array("pagelimit" => "3", "maxhlength" => "60", "articleincludedate" => "", "includedate" => "", "eventtimeformat" => "", "read_more_text" => "", "hiddenonly" => "false", "nohidden" => "false", "lockedonly" => "false", "nolocked" => "false", "nosticky" => "false", "stickyonly" => "false", "upcomingevents" => "false", "statusonly" => "false", "includecontext" => "false", "includeicons" => "false", "showhidden" => "true", "showlocked" => "true", "showsticky" => "true", "showstatus" => "false", "usestatusicons" => "true", "ignorehidden" => "false", "respectsticky" => "false", "articleincludeicons" => "false", "includeauthor" => "true", "includeorganisation" => "false", "includecommentcount" => "false", "includephoto" => "true", "highlightfeatures" => "false", "context_sensitive_region" => "false", "context_sensitive_type" => "false", "context_sensitive_topic" => "false", "context_sensitive_language" => "false", "context_sensitive_author" => "false", "promoted" => "false", "emptyresultmsg" => "We are sorry, the story you have requested is not available, please contact contact@imemc.org for more information", "filter_author" => "", "time_posted_lower_limit" => "0", "time_posted_upper_limit" => "0", "full_page_id" => "2143"));
	$site_module->setUserOptions(Array());

         $site_module->retrieveHeadlines();
       ?>
</div>
<!-- </Interviews obj id 2143> -->


         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjfilterbox", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=filterbox&amp;target_id=1928&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Filter Box [slim filter] Configuration" alt="Edit Filter Box [slim filter] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <slim filter obj id 1928> -->
<?php require_once "objects/indyruntime/filterdisplay.inc";
$fb = new filterDisplay(1928);
$display_options = Array("title" => "newsfilter", "buttontext" => "Go!", "onchange" => "false", "replacebutton" => "true", "includelanguages" => "true", "includetypes" => "false", "includemediatypes" => "false", "includespecials" => "false", "includetitle" => "false", "filter_separator" => "<br />", "horizontal" => "false", "button_first" => "false", "wrap" => "true", "hsize" => "0", "buttonimage" => "attachments/sites/site_1/go-button.gif", "style_class" => "$page_style");
$fb->setDisplayOptions($display_options);
$fb->drawFilters();
 ?>
<!-- </slim filter obj id 1928> -->


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
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjdocument", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=document&amp;target_id=2151&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Static Document [Sudoku] Configuration" alt="Edit Static Document [Sudoku] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <Sudoku obj id 2151> -->
<div class='<?php echo $page_style; ?>'>
<h4 class='<?php echo $page_style; ?>'><a href='<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=2151' class='<?php echo $page_style; ?>' title='Sudoku'><img class='object-icon' alt='Sudoku' title='Sudoku' src='<?php global $pseudo_directory_offset; echo $pseudo_directory_offset ?>graphics/sudoku.jpg'></a></h4>
</div>
<!-- </Sudoku obj id 2151> -->


<!-- </Portada menu derecho obj id 45> -->

