

<!-- <standard top bar obj id 46> -->
<UL class='<?php echo $page_style; ?>'>

         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjbar", 1)))
          {
          ?>
             <LI class='<?php echo $page_style; ?>'>
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=bar&amp;target_id=46&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Horizontal Bar [standard top bar] Configuration" alt="Edit Horizontal Bar [standard top bar] Configuration" src="<?php echo $graphics_store?>editmenu.gif" /></a>
             </LI>
          <?
          }
          ?>
       <LI class='<?php echo $page_style; ?>'>
         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjlink", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=link&amp;target_id=2134&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Simple Link [Home] Configuration" alt="Edit Simple Link [Home] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <Home obj id 2134> -->
<a href='<? echo $pseudo_directory_offset; ?>index.php' class='<?php echo $page_style; ?>' >Home</a>
<!-- </Home obj id 2134> -->

</LI>
<LI class='<?php echo $page_style; ?>'>
         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjdocument", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=document&amp;target_id=24&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Static Document [About us] Configuration" alt="Edit Static Document [About us] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <About us obj id 24> -->
<a href='<?php echo $pseudo_directory_offset ?>about_us' class='<?php echo $page_style; ?>' title='About us'>About us</a>
<!-- </About us obj id 24> -->

</LI>
<LI class='<?php echo $page_style; ?>'>
         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjcontact", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=contact&amp;target_id=84&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Contact Form Module [contact form] Configuration" alt="Edit Contact Form Module [contact form] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <contact form obj id 84> -->
<a href='<?php echo $pseudo_directory_offset ?>contact_us' class='<?php echo $page_style; ?>'>Contact us</a>
<!-- </contact form obj id 84> -->

</LI>
<LI class='<?php echo $page_style; ?>'>
         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjdocument", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=document&amp;target_id=2124&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Static Document [Donate to IMEMC] Configuration" alt="Edit Static Document [Donate to IMEMC] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <Donate to IMEMC obj id 2124> -->
<a href='<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=2124' class='<?php echo $page_style; ?>' title='Donate!'>Donate!</a>
<!-- </Donate to IMEMC obj id 2124> -->

</LI>
<LI class='<?php echo $page_style; ?>'>
         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjarchive", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=archive&amp;target_id=55&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Archive Viewer Module [archives] Configuration" alt="Edit Archive Viewer Module [archives] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <archives obj id 55> -->
<a href='<?php echo $pseudo_directory_offset ?>archives' class='<?php echo $page_style; ?>' title='Archives'>Archives</a>
<!-- </archives obj id 55> -->

</LI>
<LI class='<?php echo $page_style; ?>'>
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
<a href='<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=2128' class='<?php echo $page_style; ?>' title='Latest Audio'>Latest Audio</a>
<!-- </audio obj id 2128> -->

</LI>
<LI class='<?php echo $page_style; ?>'>
         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjdocument", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=document&amp;target_id=2193&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Static Document [IMEMC Video Production Unit] Configuration" alt="Edit Static Document [IMEMC Video Production Unit] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <IMEMC Video Production Unit obj id 2193> -->
<a href='<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=2193' class='<?php echo $page_style; ?>' title='Video'>Video</a>
<!-- </IMEMC Video Production Unit obj id 2193> -->

</LI>
<?php
global $editor_session;
if($editor_session->isSessionOpen() and ($editor_session->editor->allowedReadAccessTo("viewprotected", 1))){
?>
<LI class='<?php echo $page_style; ?>'>
         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjpublish", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=publish&amp;target_id=57&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Publisher Module [publish] Configuration" alt="Edit Publisher Module [publish] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <publish obj id 57> -->
<a href='<?php echo $pseudo_directory_offset ?>publish' class='<?php echo $page_style; ?>' title='Publish'>Publish</a>
<!-- </publish obj id 57> -->

</LI>

<?
}
?>
<LI class='<?php echo $page_style; ?>'>
         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjsearch", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=search&amp;target_id=2212&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Search Module [Search IMEMC] Configuration" alt="Edit Search Module [Search IMEMC] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <Search IMEMC obj id 2212> -->
<? 
        global $oscailt_page_setting, $pseudo_directory_offset;
        if(isset($oscailt_page_setting["obj_type"]) &&  strcasecmp($oscailt_page_setting["obj_type"], "GALLERY") == 0)
        {
            $form_target = $pseudo_directory_offset."gallery";
            $form_extra = "";
        }
        else
        {
            $form_target = $pseudo_directory_offset."newswire";
            $form_extra = "";
        }
         ?><form style='display: inline; margin: 0' name='searchform' method='get' action="<?=$form_target?>"><?= $form_extra ?><input class='<?php echo $page_style; ?>' type='text' name='search_text' maxlength='200' onFocus="JavaScript:if(this.value =='Search IMEMC'){this.value='';}" value='<?php 
       if(isset($_REQUEST["search_text"]) and $_REQUEST["search_text"] != "")
       {
          $search_fill = htmlspecialchars($_REQUEST["search_text"], ENT_QUOTES);
       }
       else $search_fill = "Search IMEMC";
       echo $search_fill; ?>'>
<input class='<?php echo $page_style; ?>-button' value='Search &gt;&gt;' name='button' type='submit'></form>
<!-- </Search IMEMC obj id 2212> -->

</LI>
</UL>

<!-- </standard top bar obj id 46> -->

