

<!-- <regional and topical headlines obj id 1924> -->
<h1 class='<?php echo $page_style; ?>'><a href='<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=2126&amp;topic_id=63&amp;type_id=18&amp;language_id=sp&amp;region_id=30' class='<?php echo $page_style; ?>' title='Ultimos articulos sobre'>Ultimos articulos sobre</a></h1><?php require_once "objects/indyruntime/newswiredisplay.inc";

         $site_module = new newswireDisplay(1924);
         $site_module->setFilters(Array("63"), Array(), Array(), Array("18"), Array(""));
         $object_display_settings = (Array("siteobject" => "1", "newswireobject" => "2126", "articleobject" => "53", "featureobject" => "54", "galleryobject" => "64", "searchobject" => "2212", "archiveobject" => "55", "contactobject" => "84", "commentsobject"=> "0", "publishobject" => "57", "eventsobject" => "58"));
         $site_module->setSiteSettings($object_display_settings);
      	$site_module->setAdminOptions(Array("pagelimit" => "3", "maxhlength" => "70", "articleincludedate" => "%b %d %y", "eventtimeformat" => "", "includedate" => "", "emptyresultmsg" => "no articulos relacionados", "title" => "", "hsize" => "", "read_more_text" => "Lee mas", "hiddenonly" => "false", "nohidden" => "false", "lockedonly" => "false", "nolocked" => "false", "nosticky" => "false", "stickyonly" => "false", "upcomingevents" => "false", "statusonly" => "false", "includecontext" => "false", "includeicons" => "false", "includephoto" => "false", "showhidden" => "false", "showlocked" => "false", "showsticky" => "false", "showstatus" => "false", "usestatusicons" => "false", "ignorehidden" => "false", "respectsticky" => "false", "articleincludeicons" => "false", "includeauthor" => "false", "includeorganisation" => "true", "includecommentcount" => "false", "latestevents" => "false", "context_sensitive_region" => "false", "context_sensitive_type" => "false", "context_sensitive_topic" => "false", "context_sensitive_language" => "false", "context_sensitive_author" => "false", "contextual_mode" => "false", "vote_highlight" => "false", "promoted" => "false", "time_posted_lower_limit" => "0", "time_posted_upper_limit" => "0", "full_page_id" => "2126"));
	$site_module->setUserOptions(Array());

         $site_module->retrieveHeadlines();
       ?>
<!-- </regional and topical headlines obj id 1924> -->

