

<!-- <Urgent appeal for funds obj id 2199> -->
<div class='<?php echo $page_style; ?>'>

         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjbox", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=box&amp;target_id=2199&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Inset Box [Urgent appeal for funds] Configuration" alt="Edit Inset Box [Urgent appeal for funds] Configuration" src="<?php echo $graphics_store?>editmenu.gif" /></a>
             
          <?
          }
          ?>
       
         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjcodebox", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=codebox&amp;target_id=2208&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Code Box [Funding Request] Configuration" alt="Edit Code Box [Funding Request] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <Funding Request obj id 2208> -->
<center>
<b>
<!-- The International Middle East Media Center is running an URGENT online fundraising campaign.
<</b><br /><br />
<strong>Please SUPPORT the IMEMC</strong>-->
Your donation helps keep the IMEMC FREE and INDEPENDENT</b><br />
<br />
<a href="https://co.clickandpledge.com/sp/d1/default.aspx?wid=48434" target="_blank">Click here to donate</a>. <br />
<!---You can give a one-time donation, or a monthly deduction from your bank account.<br />--->
</center>
<!-- </Funding Request obj id 2208> -->


</div>
<!-- </Urgent appeal for funds obj id 2199> -->

