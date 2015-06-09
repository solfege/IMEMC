<?php
global $userLanguage;
$langCode = $userLanguage->getLanguageCodePrefix();
if($langCode=="sp") include("cache/objectcache/objectstore_1/53_sp.inc");
else include("cache/objectcache/objectstore_1/53_en.inc");
?>
