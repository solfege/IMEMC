<?php
global $editor_session;
if($editor_session->isSessionOpen() and ($editor_session->editor->allowedReadAccessTo("viewprotected", 1))){
?>


<!-- <Logout Link obj id 2116> -->
<a href='<? echo $pseudo_directory_offset; ?>index.php?logout=true' class='<?php echo $page_style; ?>' title='Log out of your user account'>Logout of your Account</a>
<!-- </Logout Link obj id 2116> -->


<?
}
?>
