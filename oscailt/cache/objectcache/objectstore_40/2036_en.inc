

<!-- <footer obj id 2036> -->
<TABLE class='<?php echo $page_style; ?>'>
<TR class='<?php echo $page_style; ?>'>

         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjbar", 1)))
          {
          ?>
             <TD style='width: 1%; padding: 0; vertical-align: middle;'>
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=bar&amp;target_id=2036&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Horizontal Bar [footer] Configuration" alt="Edit Horizontal Bar [footer] Configuration" src="<?php echo $graphics_store?>editmenu.gif" /></a>
             </TD>
          <?
          }
          ?>
       <TD class='<?php echo $page_style; ?>'>
         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjcodebox", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=codebox&amp;target_id=1931&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Code Box [copyright notice] Configuration" alt="Edit Code Box [copyright notice] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <copyright notice obj id 1931> -->
<?php global $system_config; ?>© 2001-<?= strftime("%Y", time())?> <?= $system_config->site_long_name?>. Unless otherwise stated by the author, all content is free for non-commercial reuse, reprint, and rebroadcast, on the net and elsewhere. Opinions are those of the contributors and are not necessarily endorsed by <?= $system_config->site_long_name?>. 
<a href="disclaimer.php">Disclaimer</a> | <a href="privacy.php">Privacy</a>
<!-- </copyright notice obj id 1931> -->

</TD>
<TD class='<?php echo $page_style; ?>'>
         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjpicturebox", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=picturebox&amp;target_id=80&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Picture Box [oscailt logo] Configuration" alt="Edit Picture Box [oscailt logo] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <oscailt logo obj id 80> -->
<a href='http://www.indymedia.ie/oscailt/' class='<?php echo $page_style; ?>'><img class='<?php echo $page_style; ?>-thumbnail' src='<?php echo getResizedImageURL("attachments/sites/site_1/oscailtpowered.gif", 0, 0, false, 0); ?>' alt='' title='' /></a>
<!-- </oscailt logo obj id 80> -->

</TD>
</TR>
</TABLE>

<!-- </footer obj id 2036> -->

