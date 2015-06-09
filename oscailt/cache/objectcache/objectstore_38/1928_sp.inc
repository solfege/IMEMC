

<!-- <slim filter-es obj id 1928> -->
<?php require_once "objects/indyruntime/filterdisplay.inc";
$fb = new filterDisplay(1928);
$display_options = Array("title" => "newsfilter", "buttontext" => "Go!", "onchange" => "false", "replacebutton" => "true", "includelanguages" => "true", "includetypes" => "false", "includemediatypes" => "false", "includespecials" => "false", "includetitle" => "false", "filter_separator" => "<br />", "horizontal" => "false", "button_first" => "false", "wrap" => "false", "hsize" => "0", "buttonimage" => "attachments/sites/site_1/go-button.gif", "style_class" => "$page_style");
$fb->setDisplayOptions($display_options);
$fb->drawFilters();
 ?>
<!-- </slim filter-es obj id 1928> -->

