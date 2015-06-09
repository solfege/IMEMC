

<!-- <Vertical-Ad obj id 2226> -->
<TABLE class='<?php echo $page_style; ?>'>
<TR class='<?php echo $page_style; ?>'>

         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjbar", 1)))
          {
          ?>
             <TD style='width: 1%; padding: 0; vertical-align: middle;'>
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=bar&amp;target_id=2226&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Horizontal Bar [Vertical-Ad] Configuration" alt="Edit Horizontal Bar [Vertical-Ad] Configuration" src="<?php echo $graphics_store?>editmenu.gif" /></a>
             </TD>
          <?
          }
          ?>
       <TD class='<?php echo $page_style; ?>'>
         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjpicturebox", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=picturebox&amp;target_id=2225&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Picture Box [vertical-Ad] Configuration" alt="Edit Picture Box [vertical-Ad] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <vertical-Ad obj id 2225> -->
<P class='<?php echo $page_style; ?>'><a href='<?php echo $pseudo_directory_offset; ?>mailto:"thepeacecycle@aol.com"' class='<?php echo $page_style; ?>'><img class='<?php echo $page_style; ?>-thumbnail' src='<?php echo getResizedImageURL("attachments/sites/site_1/tpc2013v.jpg", 400, 210, false, 0); ?>' alt='' title='' /></a></P>
<!-- </vertical-Ad obj id 2225> -->

</TD>
</TR>
</TABLE>

<!-- </Vertical-Ad obj id 2226> -->

