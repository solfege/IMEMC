<?php global $pseudo_directory_offset; $site_file_store = $pseudo_directory_offset."attachments/sites/default/"; 
$oscailt_page_setting = Array("site_module_settings" => Array("siteobject" => "1", "newswireobject" => "2126", "articleobject" => "53", "featureobject" => "54", "galleryobject" => "64", "searchobject" => "2212", "archiveobject" => "55", "contactobject" => "84", "commentsobject" => "", "publishobject" => "57", "eventsobject" => "58"), "site_mail_settings" => Array("to" => "news@imemc.org", "from" => "site@imemc.org", "subject_prefix" => "Article edit:", "replyto" => "news@imemc.org", "level" => "none"), "site_filters" => Array("topic" => Array("0" => "*"), "region" => Array("0" => "20", "1" => "19", "2" => "18", "3" => "17", "4" => "16", "5" => "15", "6" => "14", "7" => "13", "8" => "12", "9" => "11", "10" => "10", "11" => "9", "12" => "8", "13" => "7", "14" => "6", "15" => "5", "16" => "4", "17" => "3", "18" => "2", "19" => "1"), "language" => Array("0" => "en"), "type" => Array("0" => "*"), "author" => ""), "obj_id" => "2151", "obj_type" => "DOCUMENT", "site_id" => "1");
$oscailt_story_query = new StoryQuery();
$oscailt_story_query->loadFormValues();
$PAGE_LAYOUT_ID = 1868; ?>
<?
         addToPageDescription("The IMEMC is a media collective. We are independent journalists producing and distributing the authentic voices of the people.  We are located in Beit Sahour, in the Occupied West Bank, and we welcome new volunteers and contributors.");
      addToPageTitle("Sudoku");
         addToPageDescription("Sudoku Game online"); ?>

<!-- <Sudoku obj id 2151> -->
<h1 class='indypagebody'>Sudoku</h1><BLOCKQUOTE class='indypagebody'>Play Sudoku on IMEMC Online for free

The rules of the sudoku are quite simple. A classic sudoku grid is made of nine rows and nine columns, giving 81 cells.

You must fill each cell with numbers going from 1 to 9, with the restriction that one single number can only appear once in the same row, in the same column, or in the same box of 3x3 cells.</BLOCKQUOTE><DIV class='indypagebody'><script type="text/javascript">
var live_sudoku_size = 9;
var live_sudoku_level = 1;
var live_sudoku_width = 450;
var live_sudoku_background = "FFFFFF";
</script>
<script type="text/javascript" src="http://www.live-

sudoku.com/perso/sudoku.js">
</script>
<noscript>You must accept javascript to play <a href="http://www.live

-sudoku.com" target="_blank">sudoku</a>.</noscript></DIV>
<!-- </Sudoku obj id 2151> -->


         <?php
          global $editor_session, $pseudo_directory_offset;
          if(($editor_session->canEditLayout()|| $editor_session->canEditContent()) && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjdocument", 1)))
          {
          ?>
             <br>
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=document&amp;target_id=2151&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0  align="left" title="Edit Static Document [Sudoku] Configuration" alt="Edit Static Document [Sudoku] Configuration" src="<?php echo $pseudo_directory_offset?>graphics/mediaicons/32x32/editorial2.png" /></a>
             
          <?
          }
          ?>
          