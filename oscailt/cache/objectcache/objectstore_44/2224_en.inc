

<!-- <FP-Banner-ad obj id 2224> -->
<TABLE class='<?php echo $page_style; ?>'>
<TR class='<?php echo $page_style; ?>'>

         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjbar", 1)))
          {
          ?>
             <TD style='width: 1%; padding: 0; vertical-align: middle;'>
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=bar&amp;target_id=2224&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Horizontal Bar [FP-Banner-ad] Configuration" alt="Edit Horizontal Bar [FP-Banner-ad] Configuration" src="<?php echo $graphics_store?>editmenu.gif" /></a>
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
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=picturebox&amp;target_id=2223&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Picture Box [FP-H-ad] Configuration" alt="Edit Picture Box [FP-H-ad] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <FP-H-ad obj id 2223> -->
<P class='<?php echo $page_style; ?>'><a href='<?php echo $pseudo_directory_offset; ?>mailto:"thepeacecycle@aol.com"' class='<?php echo $page_style; ?>'><img class='<?php echo $page_style; ?>-thumbnail' src='<?php echo getResizedImageURL("attachments/sites/site_1/TPC2013H.jpg", 0, 0, false, 0); ?>' alt='' title='' /></a></P>
<!-- </FP-H-ad obj id 2223> -->

</TD>
</TR>
</TABLE>

<!-- </FP-Banner-ad obj id 2224> -->

