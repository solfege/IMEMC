

<!-- <regular filter obj id 39> -->
<?php require_once "objects/indyruntime/filterdisplay.inc";
$fb = new filterDisplay(39);
$display_options = Array("title" => "", "buttontext" => "filter >>", "onchange" => "false", "replacebutton" => "false", "includelanguages" => "true", "includetypes" => "true", "includemediatypes" => "true", "includespecials" => "true", "includetitle" => "false", "filter_separator" => "<br />", "horizontal" => "false", "button_first" => "false", "wrap" => "false", "hsize" => "0", "buttonimage" => "attachments/sites/site_1/go-button.gif", "style_class" => "$page_style");
$fb->setDisplayOptions($display_options);
$fb->drawFilters();
 ?>
<!-- </regular filter obj id 39> -->

