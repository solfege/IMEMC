

<!-- <imc-features obj id 1980> -->
<?php require_once "objects/indyruntime/feedimportdisplay.inc";

         $site_module = new feedimportDisplay(1980);
         $site_module->setFilters(Array("*"), Array("20", "19", "18", "17", "16", "15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1"), Array("en"), Array("*"), Array(""));
         $object_display_settings = (Array("siteobject" => "1", "newswireobject" => "2126", "articleobject" => "53", "featureobject" => "54", "galleryobject" => "64", "searchobject" => "2212", "archiveobject" => "55", "contactobject" => "84", "commentsobject"=> "0", "publishobject" => "2390", "eventsobject" => "58"));
         $site_module->setSiteSettings($object_display_settings);
      $site_module->setAdminOptions(Array("header" => "", "footer" => "", "emptyresultmsg" => "", "promote_text" => "promote", "pagelimit" => "25", "maxhlength" => "80", "max_word" => "80", "url" => "http://newswire.indymedia.org/localfeatures-features-content.rss", "dateformat" => "%H:%M %a %b %d, %Y", "titlesize" => "1", "title" => "Global Indymedia Features", "republish_summary_prefill" => "REPLACE SUMMARY WITH ORIGINAL INTRODUCTION", "language_codes" => "", "show_republish" => "true", "allowuser_republish" => "true", "republish_full" => "true", "includedate" => "true", "articleincludedate" => "true", "articleincludeicons" => "true", "articleincludeauthor" => "true", "articleincludedescription" => "true", "articleincludelanguage" => "true", "articleincludecontent" => "true", "includedescription" => "true", "includephoto" => "true", "includetitle" => "true", "includelanguage" => "false", "includecontext" => "false", "striptags" => "false", "convertfromutf8" => "false", "language_filter" => "false", "pre_parse_utf8" => "false"));
$site_module->setUserOptions(Array());

         $site_module->processRequest();
       ?>
<!-- </imc-features obj id 1980> -->

