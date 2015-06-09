

<!-- <author specific headlines obj id 1867> -->
<h1 class='<?php echo $page_style; ?>'><a href='<?php echo $pseudo_directory_offset ?>newswire' class='<?php echo $page_style; ?>' title='Recent articles by'>Recent articles by</a></h1><?php require_once "objects/indyruntime/newswiredisplay.inc";

         $site_module = new newswireDisplay(1867);
         $site_module->setFilters(Array("*"), Array("20", "19", "18", "17", "16", "15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1"), Array("en"), Array("*"), Array(""));
         $object_display_settings = (Array("siteobject" => "1", "newswireobject" => "50", "articleobject" => "53", "featureobject" => "54", "galleryobject" => "64", "searchobject" => "2212", "archiveobject" => "55", "contactobject" => "84", "commentsobject"=> "0", "publishobject" => "57", "eventsobject" => "58"));
         $site_module->setSiteSettings($object_display_settings);
      	$site_module->setAdminOptions(Array("pagelimit" => "3", "maxhlength" => "70", "articleincludedate" => "", "eventtimeformat" => "", "includedate" => "", "emptyresultmsg" => "This author has not submitted any other articles.", "title" => "", "hsize" => "", "read_more_text" => "", "hiddenonly" => "false", "nohidden" => "false", "lockedonly" => "false", "nolocked" => "false", "nosticky" => "false", "stickyonly" => "false", "upcomingevents" => "false", "statusonly" => "false", "includecontext" => "false", "includeicons" => "false", "includephoto" => "false", "showhidden" => "false", "showlocked" => "false", "showsticky" => "false", "showstatus" => "false", "usestatusicons" => "false", "ignorehidden" => "false", "respectsticky" => "false", "articleincludeicons" => "true", "includeauthor" => "false", "includeorganisation" => "false", "includecommentcount" => "true", "latestevents" => "false", "context_sensitive_region" => "false", "context_sensitive_type" => "false", "context_sensitive_topic" => "false", "context_sensitive_language" => "false", "context_sensitive_author" => "true", "contextual_mode" => "false", "vote_highlight" => "false", "promoted" => "false", "time_posted_lower_limit" => "0", "time_posted_upper_limit" => "0", "full_page_id" => "50"));
	$site_module->setUserOptions(Array());

         $site_module->retrieveHeadlines();
       ?>
<!-- </author specific headlines obj id 1867> -->

