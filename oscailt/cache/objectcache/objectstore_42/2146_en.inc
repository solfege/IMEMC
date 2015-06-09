

<!-- <Radio Free Palestine obj id 2146> -->
<div class='<?php echo $page_style; ?>'>

         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjbox", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=box&amp;target_id=2146&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Inset Box [Radio Free Palestine] Configuration" alt="Edit Inset Box [Radio Free Palestine] Configuration" src="<?php echo $graphics_store?>editmenu.gif" /></a>
             
          <?
          }
          ?>
       
         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjpicturebox", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=picturebox&amp;target_id=2197&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Picture Box [Radio Free Palestine] Configuration" alt="Edit Picture Box [Radio Free Palestine] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <Radio Free Palestine obj id 2197> -->
<a href='http://www.imemc.org/index.php?obj_id=2196' class='center'><img class='center-thumbnail' src='<?php echo getResizedImageURL("http://www.imemc.org/attachments/sites/site_1/radiofreepalestine_archive.jpg", 0, 0, false, 0); ?>' alt='' title='' /></a>
<!-- </Radio Free Palestine obj id 2197> -->


</div>
<!-- </Radio Free Palestine obj id 2146> -->

