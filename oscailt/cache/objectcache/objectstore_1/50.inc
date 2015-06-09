<?php
global $userLanguage;
$langCode = $userLanguage->getLanguageCodePrefix();
if($langCode=="sp") include("cache/objectcache/objectstore_1/50_sp.inc");
else include("cache/objectcache/objectstore_1/50_en.inc");
?>
