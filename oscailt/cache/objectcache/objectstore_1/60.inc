<?php
global $userLanguage;
$langCode = $userLanguage->getLanguageCodePrefix();
if($langCode=="sp") include("cache/objectcache/objectstore_1/60_sp.inc");
else include("cache/objectcache/objectstore_1/60_en.inc");
?>
