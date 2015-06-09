<?php
global $editor_session;
if($editor_session->isSessionOpen() and ($editor_session->editor->allowedReadAccessTo("viewprotected", 1))){
?>


<!-- <Publish at IMEMC obj id 2141> -->
<a href='http://www.imemc.org/publish.php' class='<?php echo $page_style; ?>' title='Publish Your story'>Publish</a>
<!-- </Publish at IMEMC obj id 2141> -->


<?
}
?>
