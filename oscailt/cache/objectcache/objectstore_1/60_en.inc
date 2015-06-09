

<!-- <front page obj id 60> -->
<div class='indypage'>


<!-- begin front page header -->
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
<!-- end front page header -->
<?php if($performance_test > 2) markTime("Got Page Header") ?>
<!-- Begin front page object id 60 Main body -->
<table class='indypagebody'>
<tr class='indypagebody'>

<!-- begin front page leftcolumn -->
<td class='indyleftcolumn'>
<?php $page_style = "indyleftcolumn"; ?><?php include("cache/objectcache/objectstore_0/44.inc");?>
</td>
<!-- end front page leftcolumn -->
<?php if($performance_test > 2) markTime("Got Left column") ?>
<!-- begin front page object id 60 contents column -->
<td class='indypagebody'>
<?php $page_style = "indypagebody"; ?>
<?=$PAGE_CONTENTS?>
         <?php
          global $editor_session, $pseudo_directory_offset;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjpage", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=page&amp;target_id=60&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0  align="right" title="Edit Page Layout [front page] Configuration" alt="Edit Page Layout [front page] Configuration" src="<?php echo $pseudo_directory_offset?>graphics/mediaicons/32x32/tv.png" /></a>
             
          <?
          }
          ?>
          </td>
<!-- end front page contents column -->

<!-- begin front page right column -->
<td class='indyrightcolumn'>
<?php $page_style = "indyrightcolumn"; ?><?php include("cache/objectcache/objectstore_0/45.inc");?>
</td>
<!-- end front page right column -->
<?php if($performance_test > 2) markTime("Got Right column") ?></tr>
</table>
<!-- End front page object id 60 Main body -->
<?php if($performance_test > 2) markTime("Got Main Page Section") ?>

<!-- begin front page footer -->
<div class='indyfooter'>
<table class='indyfooter'>
<tr class='indyfooter'>
<td class='indyfooter'>
<?php $page_style = "indyfooter"; ?><?php include("cache/objectcache/objectstore_40/2036.inc");?>
</td>
</tr>
<tr class='indyfooter'>
<td class='indyfooter'>
<?php $page_style = "indyfooter"; ?><?php include("cache/objectcache/objectstore_44/2203.inc");?>
</td>
</tr>
</table>
</div>
<!-- end front page footer -->
<?php if($performance_test > 2) markTime("Got Footer Div") ?>
</div>
<!-- </front page obj id 60> -->

