

<!-- <context box-sp obj id 47> -->
<div class='<?php echo $page_style; ?>'>

         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjbox", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=box&amp;target_id=47&amp;action=edit&amp;site_id=1&amp;obj_language=ot&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Inset Box [context box-sp] Configuration" alt="Edit Inset Box [context box-sp] Configuration" src="<?php echo $graphics_store?>editmenu.gif" /></a>
             
          <?
          }
          ?>
       
         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjheadlinebox", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=headlinebox&amp;target_id=1924&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Headlines Box [regional and topical headlines] Configuration" alt="Edit Headlines Box [regional and topical headlines] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <regional and topical headlines obj id 1924> -->
<?php require_once "objects/indyruntime/newswiredisplay.inc";

         $site_module = new newswireDisplay(1924);
         $site_module->setFilters(Array("*"), Array("20", "19", "18", "17", "16", "15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1"), Array("en"), Array("*"), Array(""));
         $object_display_settings = (Array("siteobject" => "1", "newswireobject" => "50", "articleobject" => "53", "featureobject" => "54", "galleryobject" => "64", "searchobject" => "2212", "archiveobject" => "55", "contactobject" => "84", "commentsobject"=> "0", "publishobject" => "57", "eventsobject" => "58"));
         $site_module->setSiteSettings($object_display_settings);
      	$site_module->setAdminOptions(Array("pagelimit" => "3", "maxhlength" => "70", "articleincludedate" => "%b %d %y", "eventtimeformat" => "", "includedate" => "", "emptyresultmsg" => "No related articles found", "title" => "Recent Articles about", "hsize" => "", "read_more_text" => "", "hiddenonly" => "false", "nohidden" => "false", "lockedonly" => "false", "nolocked" => "false", "nosticky" => "false", "stickyonly" => "false", "upcomingevents" => "false", "statusonly" => "false", "includecontext" => "false", "includeicons" => "false", "includephoto" => "false", "showhidden" => "false", "showlocked" => "false", "showsticky" => "false", "showstatus" => "false", "usestatusicons" => "false", "ignorehidden" => "false", "respectsticky" => "false", "articleincludeicons" => "false", "includeauthor" => "true", "includeorganisation" => "false", "includecommentcount" => "false", "latestevents" => "false", "context_sensitive_region" => "true", "context_sensitive_type" => "false", "context_sensitive_topic" => "true", "context_sensitive_language" => "false", "context_sensitive_author" => "false", "contextual_mode" => "true", "vote_highlight" => "false", "promoted" => "false", "time_posted_lower_limit" => "0", "time_posted_upper_limit" => "0", "full_page_id" => "50"));
	$site_module->setUserOptions(Array());

         $site_module->retrieveHeadlines();
       ?>
<!-- </regional and topical headlines obj id 1924> -->


</div>
<!-- </context box-sp obj id 47> -->

