

<!-- <list obj id 2142> -->
<div class='indypage'>


<!-- begin list header -->
<div class='indypage'>
<table class='indypage'>
<tr class='indypage'>
<td class='indypage'>
<?php $page_style = "indypage"; ?><?php include("cache/objectcache/objectstore_0/46.inc");?>
</td>
</tr>
<tr class='indypage'>
<td class='indypage'>
<?php $page_style = "indypage"; ?><?php include("cache/objectcache/objectstore_1/76.inc");?>
</td>
</tr>
</table>
</div>
<!-- end list header -->
<?php if($performance_test > 2) markTime("Got Page Header") ?>
<!-- Begin list object id 2142 Main body -->
<table class='indypage'>
<tr class='indypage'>

<!-- begin list leftcolumn -->
<td class='indypage'>
<?php $page_style = "indypage"; ?><?php include("cache/objectcache/objectstore_42/2129.inc");?>
</td>
<td class='indypage'>
<?php $page_style = "indypage"; ?><?php include("cache/objectcache/objectstore_0/44.inc");?>
</td>
<!-- end list leftcolumn -->
<?php if($performance_test > 2) markTime("Got Left column") ?>
<!-- begin list object id 2142 contents column -->
<td class='indypage'>
<?php $page_style = "indypage"; ?>
<?php global $suppress_page_insets; if(!$suppress_page_insets){ ?>

<!-- begin list contents inset boxes -->
<?php $page_style = "indypage"; ?><?php include("cache/objectcache/objectstore_42/2137.inc");?>
<?php if($performance_test > 2) markTime("Got Inset Boxes") ?><!-- end list contents inset boxes -->
<?php } ?>
<?=$PAGE_CONTENTS?>
         <?php
          global $editor_session, $pseudo_directory_offset;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjpage", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=page&amp;target_id=2142&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0  align="right" title="Edit Page Layout [list] Configuration" alt="Edit Page Layout [list] Configuration" src="<?php echo $pseudo_directory_offset?>graphics/mediaicons/32x32/tv.png" /></a>
             
          <?
          }
          ?>
          </td>
<!-- end list contents column -->
</tr>
</table>
<!-- End list object id 2142 Main body -->
<?php if($performance_test > 2) markTime("Got Main Page Section") ?>

<!-- begin list footer -->
<div class='indypage'>
<table class='indypage'>
<tr class='indypage'>
<td class='indypage'>
<?php $page_style = "indypage"; ?><?php include("cache/objectcache/objectstore_40/2036.inc");?>
</td>
</tr>
</table>
</div>
<!-- end list footer -->
<?php if($performance_test > 2) markTime("Got Footer Div") ?>
</div>
<!-- </list obj id 2142> -->

