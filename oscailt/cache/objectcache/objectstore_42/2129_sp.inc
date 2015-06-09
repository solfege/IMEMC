

<!-- <Audios obj id 2129> -->

         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjmenu", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=menu&amp;target_id=2129&amp;action=edit&amp;site_id=1&amp;obj_language=ot&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Vertical Menu [Audios] Configuration" alt="Edit Vertical Menu [Audios] Configuration" src="<?php echo $graphics_store?>editmenu.gif" /></a>
             
          <?
          }
          ?>
       
         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjnewswire", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=newswire&amp;target_id=2128&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Newswire Module [audio] Configuration" alt="Edit Newswire Module [audio] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <audio obj id 2128> -->
<div class='<?php echo $page_style; ?>'>
<h4 class='<?php echo $page_style; ?>'><a href='<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=2128' class='<?php echo $page_style; ?>' title='Latest Audio'>Latest Audio</a></h4>
</div>
<!-- </audio obj id 2128> -->


         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjlink", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=link&amp;target_id=2105&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Simple Link [Podcast Link] Configuration" alt="Edit Simple Link [Podcast Link] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <Podcast Link obj id 2105> -->
<div class='<?php echo $page_style; ?>'>
<a href='<? echo $pseudo_directory_offset; ?>podcast.xml' class='<?php echo $page_style; ?>' title='A podcast of the sites audio and video content'>Podcast</a>
</div>
<!-- </Podcast Link obj id 2105> -->


         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjlink", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=link&amp;target_id=2106&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Simple Link [iTunes Store Podcast Link] Configuration" alt="Edit Simple Link [iTunes Store Podcast Link] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <iTunes Store Podcast Link obj id 2106> -->
<div class='<?php echo $page_style; ?>'>
<a href='itms://phobos.apple.com/WebObjects/MZStore.woa/wa/viewPodcast?id=120904798' class='<?php echo $page_style; ?>' title='A link to the sites podcast in the iTunes music store'>iTunes Podcast</a>
</div>
<!-- </iTunes Store Podcast Link obj id 2106> -->


<!-- </Audios obj id 2129> -->

