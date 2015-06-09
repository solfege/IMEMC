<?php
global $userLanguage;
$langCode = $userLanguage->getLanguageCodePrefix();
if($langCode=="sp") include("cache/objectcache/objectstore_0/1_sp.inc");
else include("cache/objectcache/objectstore_0/1_en.inc");
?>
