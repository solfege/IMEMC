

<!-- <article types obj id 2122> -->
<?php require_once "objects/indyruntime/categorylistdisplay.inc";

         $site_module = new categorylistDisplay(2122);
         $site_module->setFilters(Array("*"), Array("20", "19", "18", "17", "16", "15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1"), Array("en"), Array("*"), Array(""));
         $object_display_settings = (Array("siteobject" => "1", "newswireobject" => "2126", "articleobject" => "53", "featureobject" => "54", "galleryobject" => "64", "searchobject" => "2212", "archiveobject" => "55", "contactobject" => "84", "commentsobject"=> "0", "publishobject" => "57", "eventsobject" => "58"));
         $site_module->setSiteSettings($object_display_settings);
      	$site_module->setAdminOptions(Array("listing" => "type", "divider" => "3", "linksto" => "50"));
	$site_module->setUserOptions(Array());

         $site_module->retrieveHeadlines();
       ?>
<!-- </article types obj id 2122> -->

