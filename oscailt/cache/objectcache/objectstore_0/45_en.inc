

<!-- <front page right menu obj id 45> -->

         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjmenu", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=menu&amp;target_id=45&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Vertical Menu [front page right menu] Configuration" alt="Edit Vertical Menu [front page right menu] Configuration" src="<?php echo $graphics_store?>editmenu.gif" /></a>
             
          <?
          }
          ?>
       
         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjcodebox", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=codebox&amp;target_id=2214&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Code Box [Twitter] Configuration" alt="Edit Code Box [Twitter] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <Twitter obj id 2214> -->
<div class='<?php echo $page_style; ?>'>
<center><a href="http://www.twitter.com/imemcnews"><img src="http://www.imemc.org/attachments/sites/site_1/followimemc.png" alt="Follow imemcnews on Twitter"/></a></center>
</div>
<!-- </Twitter obj id 2214> -->


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
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=newswire&amp;target_id=2123&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Newswire Module [human interest] Configuration" alt="Edit Newswire Module [human interest] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <human interest obj id 2123> -->
<div class='<?php echo $page_style; ?>'>
<h4 class='<?php echo $page_style; ?>'><a href='<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=2123' class='<?php echo $page_style; ?>' title='Human Interest Stories'>Human Interest Stories</a></h4><?php require_once "objects/indyruntime/newswiredisplay.inc";

         $site_module = new newswireDisplay(2123);
         $site_module->setFilters(Array("76", "36", "35", "34", "33", "32", "15", "24", "23", "22", "21", "25", "26", "10", "30", "16", "28", "27", "3", "20", "31"), Array("20", "19", "18", "17", "16", "15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1"), Array("en"), Array("8"), Array(""));
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
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=newswire&amp;target_id=2143&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Newswire Module [Interviews] Configuration" alt="Edit Newswire Module [Interviews] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <Interviews obj id 2143> -->
<div class='<?php echo $page_style; ?>'>
<h4 class='<?php echo $page_style; ?>'><a href='<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=2143' class='<?php echo $page_style; ?>' title='Interviews'>Interviews</a></h4><?php require_once "objects/indyruntime/newswiredisplay.inc";

         $site_module = new newswireDisplay(2143);
         $site_module->setFilters(Array("36", "35", "34", "33", "32", "15", "24", "23", "22", "21", "25", "26", "10", "30", "16", "31", "20", "3", "27", "28"), Array("20", "19", "18", "17", "16", "15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1"), Array("en"), Array("9"), Array(""));
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
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjcodebox", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=codebox&amp;target_id=2228&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Code Box [Google-Analytics] Configuration" alt="Edit Code Box [Google-Analytics] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <Google-Analytics obj id 2228> -->
<div class='<?php echo $page_style; ?>'>
<script type="text/javascript">

 var _gaq = _gaq || [];
 _gaq.push(['_setAccount', 'UA-662121-1']);
 _gaq.push(['_trackPageview']);

 (function() {
 var ga = document.createElement('script'); ga.type = 'text/javascript'; ga.async = true;
 ga.src = ('https:' == document.location.protocol ? 'https://ssl' : 'http://www') + '.google-analytics.com/ga.js';
 var s = document.getElementsByTagName('script')[0]; s.parentNode.insertBefore(ga, s);
 })();

</script>
</div>
<!-- </Google-Analytics obj id 2228> -->


<!-- </front page right menu obj id 45> -->

