

<!-- <Donate - top bar obj id 2230> -->
<div class='<?php echo $page_style; ?>'>

         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjbox", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=box&amp;target_id=2230&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Inset Box [Donate - top bar] Configuration" alt="Edit Inset Box [Donate - top bar] Configuration" src="<?php echo $graphics_store?>editmenu.gif" /></a>
             
          <?
          }
          ?>
       <table class='<?php echo $page_style; ?>'><tr class='<?php echo $page_style; ?>'><td class='<?php echo $page_style; ?>'>
         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjcodebox", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=codebox&amp;target_id=2229&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Code Box [Donate - top bar] Configuration" alt="Edit Code Box [Donate - top bar] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <Donate - top bar obj id 2229> -->
<table width="100%">
<tr>
<td bgcolor=#DC143C align="center">
<p style="color:white;margin-left:20px;"><a style="color:#FFFFF0; font-size:large; font-weight:bold;text-decoration: none; align:center;" href="https://co.clickandpledge.com/sp/d1/default.aspx?wid=48434">Please help IMEMC continue to bring you news by donating now</a></p>
</td>
</tr>
</table>
<!-- </Donate - top bar obj id 2229> -->

</td></tr></table>
</div>
<!-- </Donate - top bar obj id 2230> -->

