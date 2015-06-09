

<!-- <H-Toolbar obj id 2203> -->
<div class='<?php echo $page_style; ?>'>

         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjbox", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=box&amp;target_id=2203&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Inset Box [H-Toolbar] Configuration" alt="Edit Inset Box [H-Toolbar] Configuration" src="<?php echo $graphics_store?>editmenu.gif" /></a>
             
          <?
          }
          ?>
       <table class='<?php echo $page_style; ?>'><tr class='<?php echo $page_style; ?>'><td class='<?php echo $page_style; ?>'>
         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjdocument", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=document&amp;target_id=2202&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Static Document [H-Toolbar] Configuration" alt="Edit Static Document [H-Toolbar] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <H-Toolbar obj id 2202> -->
<h4 class='<?php echo $page_style; ?>'></h4><BLOCKQUOTE class='<?php echo $page_style; ?>'></BLOCKQUOTE><DIV class='<?php echo $page_style; ?>'><table style='width:0px;border:0px;'><tr><td><script src="http://hosting.conduit.com/DrawToolbar/?ct=CT1752195&cover=0&exe=true"></script></td></tr><tr><td style='font-size: 9px;font-family: Verdana;text-align: right;color:#888888'><a href='http://www.Conduit.com' style='text-decoration:none;color:#888888;'>toolbar</a> powered by Conduit</td></tr></table></DIV>
<!-- </H-Toolbar obj id 2202> -->

</td></tr></table>
</div>
<!-- </H-Toolbar obj id 2203> -->

