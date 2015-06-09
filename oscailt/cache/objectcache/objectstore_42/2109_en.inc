

<!-- <imc-global obj id 2109> -->
<?php require_once "objects/indyruntime/feedimportdisplay.inc";

         $site_module = new feedimportDisplay(2109);
         $site_module->setFilters(Array("*"), Array("20", "19", "18", "17", "16", "15", "14", "13", "12", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1"), Array("en"), Array("*"), Array(""));
         $object_display_settings = (Array("siteobject" => "1", "newswireobject" => "2126", "articleobject" => "53", "featureobject" => "54", "galleryobject" => "64", "searchobject" => "2212", "archiveobject" => "55", "contactobject" => "84", "commentsobject"=> "0", "publishobject" => "2390", "eventsobject" => "58"));
         $site_module->setSiteSettings($object_display_settings);
      $site_module->setAdminOptions(Array("header" => "", "footer" => "", "emptyresultmsg" => "", "promote_text" => "promote", "pagelimit" => "25", "maxhlength" => "80", "max_word" => "80", "url" => "http://indymedia.org/en/main-features.rss", "dateformat" => "%a %b %d, %Y %H:%M", "titlesize" => "1", "title" => "Indymedia.org Features", "republish_summary_prefill" => "REPLACE SUMMARY WITH ORIGINAL INTRODUCTION", "language_codes" => "", "show_republish" => "true", "allowuser_republish" => "true", "republish_full" => "true", "includedate" => "true", "articleincludedate" => "true", "articleincludeicons" => "true", "articleincludeauthor" => "true", "articleincludedescription" => "true", "articleincludelanguage" => "true", "articleincludecontent" => "true", "includedescription" => "true", "includephoto" => "true", "includetitle" => "true", "includelanguage" => "true", "includecontext" => "false", "striptags" => "true", "convertfromutf8" => "false", "language_filter" => "false", "pre_parse_utf8" => "false"));
$site_module->setUserOptions(Array());

         $site_module->processRequest();
       ?>
<!-- </imc-global obj id 2109> -->

