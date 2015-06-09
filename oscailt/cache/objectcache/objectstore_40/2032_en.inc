

<!-- <plain page obj id 2032> -->
<div class='indypage'>


<!-- begin plain page header -->
<div class='indyheader'>
<table class='indyheader'>
<tr class='indyheader'>
<td class='indyheader'>
<?php $page_style = "banner-left"; ?><?php include("cache/objectcache/objectstore_1/76.inc");?>
</td>
</tr>
<tr class='indyheader'>
<td class='indyheader'>
<?php $page_style = "indyheader"; ?><?php include("cache/objectcache/objectstore_0/46.inc");?>
</td>
</tr>
<tr class='indyheader'>
<td class='indyheader'>
<?php $page_style = "indyheader"; ?><?php include("cache/objectcache/objectstore_44/2230.inc");?>
</td>
</tr>
</table>
</div>
<!-- end plain page header -->
<?php if($performance_test > 2) markTime("Got Page Header") ?>
<!-- Begin plain page object id 2032 Main body -->
<table class='indypagebody'>
<tr class='indypagebody'>

<!-- begin plain page leftcolumn -->
<td class='indyleftcolumn'>
<?php $page_style = "indyleftcolumn"; ?><?php include("cache/objectcache/objectstore_0/44.inc");?>
</td>
<!-- end plain page leftcolumn -->
<?php if($performance_test > 2) markTime("Got Left column") ?>
<!-- begin plain page object id 2032 contents column -->
<td class='indypagebody'>
<?php $page_style = "indypagebody"; ?>
<?=$PAGE_CONTENTS?>
         <?php
          global $editor_session, $pseudo_directory_offset;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjpage", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=page&amp;target_id=2032&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0  align="right" title="Edit Page Layout [plain page] Configuration" alt="Edit Page Layout [plain page] Configuration" src="<?php echo $pseudo_directory_offset?>graphics/mediaicons/32x32/tv.png" /></a>
             
          <?
          }
          ?>
          </td>
<!-- end plain page contents column -->
</tr>
</table>
<!-- End plain page object id 2032 Main body -->
<?php if($performance_test > 2) markTime("Got Main Page Section") ?>

<!-- begin plain page footer -->
<div class='indyfooter'>
<table class='indyfooter'>
<tr class='indyfooter'>
<td class='indyfooter'>
<?php $page_style = "indyfooter"; ?><?php include("cache/objectcache/objectstore_40/2036.inc");?>
</td>
</tr>
</table>
</div>
<!-- end plain page footer -->
<?php if($performance_test > 2) markTime("Got Footer Div") ?>
</div>
<!-- </plain page obj id 2032> -->

