

<!-- <filter bar obj id 2035> -->
<TABLE class='<?php echo $page_style; ?>'>
<TR class='<?php echo $page_style; ?>'>

         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjbar", 1)))
          {
          ?>
             <TD style='width: 1%; padding: 0; vertical-align: middle;'>
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=bar&amp;target_id=2035&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Horizontal Bar [filter bar] Configuration" alt="Edit Horizontal Bar [filter bar] Configuration" src="<?php echo $graphics_store?>editmenu.gif" /></a>
             </TD>
          <?
          }
          ?>
       <TD class='<?php echo $page_style; ?>'>
         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjfilterbox", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=filterbox&amp;target_id=1928&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Filter Box [slim filter] Configuration" alt="Edit Filter Box [slim filter] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <slim filter obj id 1928> -->
<?php require_once "objects/indyruntime/filterdisplay.inc";
$fb = new filterDisplay(1928);
$display_options = Array("title" => "newsfilter", "buttontext" => "Go!", "onchange" => "false", "replacebutton" => "true", "includelanguages" => "true", "includetypes" => "false", "includemediatypes" => "false", "includespecials" => "false", "includetitle" => "false", "filter_separator" => "<br />", "horizontal" => "false", "button_first" => "false", "wrap" => "false", "hsize" => "0", "buttonimage" => "attachments/sites/site_1/go-button.gif", "style_class" => "$page_style");
$fb->setDisplayOptions($display_options);
$fb->drawFilters();
 ?>
<!-- </slim filter obj id 1928> -->

</TD>
</TR>
</TABLE>

<!-- </filter bar obj id 2035> -->

