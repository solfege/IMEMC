

<!-- <article view page obj id 1868> -->
<div class='indypage'>


<!-- begin article view page header -->
<div class='indyheader'>
<table class='indyheader'>
<tr class='indyheader'>
<td class='indyheader'>
<?php $page_style = "indyheader"; ?><?php include("cache/objectcache/objectstore_1/76.inc");?>
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
<!-- end article view page header -->
<?php if($performance_test > 2) markTime("Got Page Header") ?>
<!-- Begin article view page object id 1868 Main body -->
<table class='indypagebody'>
<tr class='indypagebody'>

<!-- begin article view page leftcolumn -->
<td class='indyleftcolumn'>
<?php $page_style = "indyleftcolumn"; ?><?php include("cache/objectcache/objectstore_42/2135.inc");?>
</td>
<!-- end article view page leftcolumn -->
<?php if($performance_test > 2) markTime("Got Left column") ?>
<!-- begin article view page object id 1868 contents column -->
<td class='indypagebody'>
<?php $page_style = "indypagebody"; ?>
<?=$PAGE_CONTENTS?>
         <?php
          global $editor_session, $pseudo_directory_offset;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjpage", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=page&amp;target_id=1868&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0  align="right" title="Edit Page Layout [article view page] Configuration" alt="Edit Page Layout [article view page] Configuration" src="<?php echo $pseudo_directory_offset?>graphics/mediaicons/32x32/tv.png" /></a>
             
          <?
          }
          ?>
          <?php global $suppress_page_insets; if(!$suppress_page_insets){ ?>

<!-- begin article view page contents inset boxes -->
<?php $page_style = "indypagebody"; ?><?php include("cache/objectcache/objectstore_44/2205.inc");?>
<?php $page_style = "indypagebody"; ?><?php include("cache/objectcache/objectstore_43/2199.inc");?>
<?php if($performance_test > 2) markTime("Got Inset Boxes") ?><!-- end article view page contents inset boxes -->
<?php } ?>
</td>
<!-- end article view page contents column -->

<!-- begin article view page right column -->
<td class='indyrightcolumn'>
<?php $page_style = "indyrightcolumn"; ?><?php include("cache/objectcache/objectstore_44/2215.inc");?>
</td>
<!-- end article view page right column -->
<?php if($performance_test > 2) markTime("Got Right column") ?></tr>
</table>
<!-- End article view page object id 1868 Main body -->
<?php if($performance_test > 2) markTime("Got Main Page Section") ?>

<!-- begin article view page footer -->
<div class='indyfooter'>
<table class='indyfooter'>
<tr class='indyfooter'>
<td class='indyfooter'>
<?php $page_style = "indyfooter"; ?><?php include("cache/objectcache/objectstore_40/2036.inc");?>
</td>
</tr>
</table>
</div>
<!-- end article view page footer -->
<?php if($performance_test > 2) markTime("Got Footer Div") ?>
</div>
<!-- </article view page obj id 1868> -->

