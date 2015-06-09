<?php
/************************************************************************/
/* Oscailt                                                              */
/* Indepenent Media Centre Content Management System                    */
/* ==================================================================== */
/* Copyright (c)2003-2005 by Independent Media Centre Ireland           */
/* http://www.indymedia.ie                                              */
/* Development List: oscailt@lists.indymedia.org                        */
/* See contributions.txt for the list of contributors                   */
/*                                                                      */
/* This program is free software. You can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation. http://www.gnu.org/copyleft/gpl.html   */
/*                                                                      */
/* This program is distributed in the hope that it will be useful,      */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of       */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        */
/* GNU General Public License for more details.                         */
/************************************************************************/

$OSCAILT_SCRIPT = "viewarticles.php";
include("config/attachments.php");
require_once("oscailt_init.inc");

require_once("objects/statistics.inc");
require_once("objects/memorymgmt.inc");
require_once("objects/generatebargraph.inc");
require_once("objects/indyobjects/indydataobjects.inc");

$textLabels = array("title" => "Debug Articles",
	            "display_last_msg" => "Display Last",
	            "view_templates" => "View Templates",
	            "most_recent_publish_msg" => "Most Recently Published",
	            "stories_word" => "Stories",
	            "comments_word" => "Comments",
	            "comment_id_word" => "Comment Id",
	            "featurized_word" => "Featurized",
	            "story_id_word" => "Story Id",
	            "storytitle_word" => "Story Title",
	            "by_author_msg" => "By Author",
	            "time_posted_msg" => "Time Posted",
	            "hidden_status_msg" => "Hidden Status",
	            "visible_word" => "Visible",
	            "hidden_word" => "Hidden",
	            "pending_word" => "Pending",
	            "attachments_word" => "Attachments",
	            "attachment_id_word" => "Attachment Id",
	            "filename_word" => "Filename",
	            "story_gone_msg" => "??story gone??",
	            "most_recent_msg" => "Most Recent",
	            "search_btn_text" => "Search",
	            "search_extra_text" => "for embedded video id:",
	            "setup_translation_link" => "Setup Story Translation Links",
	            "type_word" => "Type",
	            "original_word" => "Original",
	            "original_story_title_msg" => "Original Story Title",
	            "original_language_msg" => "Original Language",
	            "translated_word" => "Translated",
	            "translated_story_title_msg" => "Translated Story Title",
	            "translated_language_msg" => "Translated Language", 
	            "recent_stories_msg" => "Recent Stories",
	            "recent_comments_msg" => "Recent Comments",
	            "recent_attachments_msg" => "Recent Attachments",
		    "recent_translated_links_msg" => "Recent Translated Links",
		    "lost_stories" => "Validate Db Entries",
                    "validation_intro" => "There should be no stories listed in this screen otherwise there are stories with invalid ids for either topics, regions or types",
                    "validate_topics_text" => "Validation of stories table for topic ids not in database",
                    "stories_undef_topic"  => "# stories where with undefined topic ids",
                    "validate_regions_text" => "Validation of stories table for regions ids not in database",
                    "stories_undef_region"  => "# stories where with undefined region ids",
                    "validate_type_text"  => "Validation of stories table for type ids not in database",
                    "stories_undef_type"  => "# stories where with undefined type ids ",
                    "validate_language_text" => "Validation of stories table for language ids not in database",
		    "stories_undef_language" => "# stories where with undefined language ids "
	            ); 

$textObj = new indyItemSet();

if($textObj->load($system_config->xml_store, "viewarticles") === false)
{
    $textObj->setUserMessage( array( USER_ERROR, "Failed to get text strings for the View Articles. -Using defaults",""));
} else {
    foreach (array_keys($textLabels) as $str_key ) {
        if (trim($textObj->getString($str_key)) != "" )
            $textLabels[$str_key] = $textObj->getString($str_key);
	else
        {
            if ($system_config->user_error_reporting == 8) $textLabels[$str_key] .= " using default ";
	}
    }
}

addToPageTitle($textLabels["title"]);


class debugDisplay
{
   // Writes a choice for controlling the last N stories or attachments.
   function writeSizeSelection($hidden_data_name, $show_types="", $last_filter_type=0)
   {
       global $textLabels;
       $ActionFormData ="<select name='size_filter' onchange=submit()><option value='5'>5</option><option value='10' >10</option><option value='15' selected>15</option><option value='20'>20</option><option value='25'>25</option><option value='30'>30</option><option value='40'>40</option><option value='50'>50</option><option value='60'>60</option><option value='90'>90</option><option value='120'>120</option><option value='150'>150</option><option value='200'>200</option></select> ";

       ?>
       <FORM name='stats_last_size_filter' action='' method=POST> 
	    <BR> <?=$textLabels['display_last_msg']?> <?=$ActionFormData?> <?=ucfirst($hidden_data_name)?> 
            <input type=hidden name=<?=$hidden_data_name?> value="true"> 
       <?
       if ($show_types == "types") $this->writeTypeSelection($last_filter_type);
       ?>
       </FORM>
       <?
   }

   // Writes choices for controlling types of attachments to display or filter on.
   function writeTypeSelection($last_filter_type)
   {
       $form_array = array(1 => 'All Types',2 =>'Images', 3 => 'Audio', 4 => 'Video', 5 => 'Files', 6 => 'Embedded Audio', 7 => 'Embedded Video' );
       $ActionFormData ="<select name='filter_types' onchange=submit()>";

       foreach ($form_array as $key => $display_option) {
	   if ($key == $last_filter_type) {
               $ActionFormData .="<option value=".$key." selected>".$display_option. "</option>";
	   } else {
               $ActionFormData .="<option value=".$key." >".$display_option. "</option>";
	   }
       }
       $ActionFormData .= "</select>";

       ?>
            for type <?=$ActionFormData?> <input type=hidden name=attachment_type value="true">
       <?
   }
   // Writes search box for embedded video 
   function writeSearchOption()
   {
       global $textLabels;
       ?>
       <FORM name='stats_search_box' action='' method=POST> 
       <BR> <input type=submit name=search_btn value="<?=$textLabels['search_btn_text']?>"> <?=$textLabels['search_extra_text']?>  
            <input type=text name="search_recent" size=12 value="">
            <input type=hidden name=filter_types value="7">
            <input type=hidden name=size_filter value="<?=$_REQUEST['size_filter']?>">
            <input type=hidden name=attachments value="true">
       </FORM>
       <?
   }


   // Show most recent stories or comments published
   function writeStoryRecentIds($show_stories=true)
   {
       global $prefix, $dbconn, $system_config, $textLabels;

       $last_recs = 15;
       if (isset($_REQUEST['size_filter']) && $_REQUEST['size_filter'] > 0) $last_recs = $_REQUEST['size_filter'];

       $http_mode = isset($_SERVER['HTTPS']) ? "https" : "http";
       $url_base = '<a href="'.$http_mode.'://'.$system_config->site_url.'/';

       if ($show_stories == true) {
           $result = sql_query("SELECT story_id, story_title, author_name,UNIX_TIMESTAMP(time_posted), hidden FROM ".$prefix."_stories ORDER BY time_posted DESC LIMIT 0, ".$last_recs, $dbconn,0);
	   //$stories_or_comments = "Stories";
	   $stories_or_comments = $textLabels['stories_word'];
	   $cols_to_span = 7;
       } else {
           $result = sql_query("SELECT comment_id, story_id,comment_title, author_name,UNIX_TIMESTAMP(time_posted), hidden FROM ".$prefix."_comments ORDER BY time_posted DESC LIMIT 0, ".$last_recs, $dbconn,0);
	   //$stories_or_comments = "Comments";
	   $stories_or_comments = $textLabels['comments_word'];
	   $cols_to_span = 8;
       }
       checkForError($result);

       $total = 0;
       if(sql_num_rows( $result ) > 0)
       {
           $stories = array();
           $story_titles = array();
           $story_author = array();
           $story_times = array();
           $story_stati = array();
	   $comments = array();

           for ($j = 0; $j < sql_num_rows( $result ); $j++ )
	   {
               if ($show_stories == true) {
                   list($s_id, $s_title, $s_author, $s_time, $s_hide) = sql_fetch_row($result, $dbconn);
	       } else {
                   list($c_id, $s_id, $s_title, $s_author, $s_time, $s_hide) = sql_fetch_row($result, $dbconn);
                   $comments[$j] = $c_id;
	       }

               $stories[$j] = $s_id;
               $story_titles[$j] = $s_title;
               $story_author[$j] = $s_author;
               $story_times[$j] = $s_time + $system_config->timezone_offset;
               $story_stati[$j] = $s_hide;
	       $total++;
           }
	   ?>
           <p>
           <table width=100% cellspacing=2 cellpadding=2>
           <tr class=admin>
	   <th class=admin colspan=<?=$cols_to_span?>>&nbsp; <?=$textLabels['most_recent_publish_msg']?> <?=$total?> <?=$stories_or_comments?> &nbsp;</th>
           </tr>
	   <tr class="admin">
              <th class="admin" align=center>&nbsp;#&nbsp;</th>
           <?
           if ($show_stories == false) {
	      ?>
		      <th class="admin" align=center>&nbsp;<?=$textLabels['comment_id_word']?>&nbsp;</th>
	      <?
           }
	   ?>
              <th class="admin" align=center>&nbsp;<?=$textLabels['comment_id_word']?>&nbsp;</th>
              <th class="admin" align=center><?=$textLabels['story_id_word']?></th>
              <th class="admin" align=center>&nbsp;<?=$textLabels['by_author_msg']?>&nbsp;</th>
              <th class="admin" align=center>&nbsp;<?=$textLabels['time_posted_msg']?>&nbsp;</th>
              <th class="admin" align=center>&nbsp;<?=$textLabels['hidden_status_msg']?>&nbsp;</th>
           </tr>
           <?
           for ($j = 0; $j < $total; $j++ )
	   {
               $s_id = $stories[$j] ;
               $s_title = $story_titles[$j] ;
               $s_author = $story_author[$j] ;
               if ($show_stories == false) {
                   $c_id = $comments[$j] ;
	           $c_id_url = $url_base . 'article/' . $s_id.'#comment'.$c_id.'">' .$c_id.'</a>';
	           $s_title_url = $url_base . 'article/' . $s_id.'#comment'.$c_id.'">' .$s_title.'</a>';
	       } else {
	           $s_title_url = $url_base . 'article/' . $s_id. '">' .$s_title.'</a>';
	       }
	       $s_id_url = $url_base . 'article/' . $s_id. '">' .$s_id.'</a>';

               $s_time = strftime("%a %b %d, %T %H:%M", $story_times[$j]);
               if ($story_stati[$j] == null || $story_stati[$j] == 0 ) $hide_str = "Visible";
	       else if ($story_stati[$j] == 1) $hide_str = "<b>Hidden</b>";
	       else $hide_str = "<b>Pending</b>";

	      ?>
	      <tr class="stats">
              <td align=center><?=($j+1)?></td>
	      <?
              if ($show_stories == false) {
	          ?> <td align=center><?=$c_id_url?></td> <?
              }
	      ?>
              <td align=center><?=$s_id_url?></td>
              <td align=left >&nbsp;<?=$s_title_url ?></td>
              <td align=left>&nbsp;<?=$s_author ?></td>
              <td align=right><?=$s_time ?></td>
              <td align=center><?=$hide_str ?></td>
              </tr>
              <?
           }
	   ?>
	      <tr class="stats">
              <td align=center colspan=7>
           <?
           if ($show_stories == true) $this->writeSizeSelection("stories");
	   else $this->writeSizeSelection("comments");
	   ?>
              </td>
              </tr>
           </table>
           <?
       }
   }

   // Show most recent attachments published
   function writeAttachmentRecentIds()
   {
       global $prefix, $dbconn, $system_config, $fileExtensions, $textLabels;

       $last_recs = 15;
       if (isset($_REQUEST['size_filter']) && $_REQUEST['size_filter'] > 0) $last_recs = $_REQUEST['size_filter'];

       $http_mode = isset($_SERVER['HTTPS']) ? "https" : "http";
       $url_base = '<a href="'.$http_mode.'://'.$system_config->site_url.'/';
       $url_base_attach = $url_base . $system_config->attachment_store;

       $filter_by = "";
       $filter_types=1;
       if (isset($_REQUEST['filter_types'])) {
           if ($_REQUEST['filter_types'] == 1) $filter_by = "";
	   elseif ($_REQUEST['filter_types'] == 2) $filter_by = "image > 0";
	   elseif ($_REQUEST['filter_types'] == 3) $filter_by = "audio > 0";
	   elseif ($_REQUEST['filter_types'] == 4) $filter_by = "video > 0";
	   elseif ($_REQUEST['filter_types'] == 5) $filter_by = "image is null and audio is null and video is null ";
	   elseif ($_REQUEST['filter_types'] == 6) $filter_by = "substring(attachment_file, 1,12) = 'embedaudio:0'";
	   elseif ($_REQUEST['filter_types'] == 7) $filter_by = "substring(attachment_file, 1,12) = 'embedvideo:0'";

           $filter_types = $_REQUEST['filter_types'];
       }

       if (isset($_REQUEST['search_btn']) && isset($_REQUEST['search_recent']) && trim($_REQUEST['search_recent']) != "")
	   $filter_by = "attachment_file like 'embedvideo:%".$_REQUEST['search_recent']."%'";

       // $result = sql_query("UPDATE ".$prefix."_attachments SET attachment_file='mar2009/image045_j.jpg' WHERE attachment_file='attachments/mar2009/image045_j.jpg'", $dbconn,0);
       if ($filter_by == "") {
           $result = sql_query("SELECT attachment_id, story_id, comment_id, featurized, attachment_file, UNIX_TIMESTAMP(time_posted), hidden FROM ".$prefix."_attachments ORDER BY time_posted DESC LIMIT 0, ".$last_recs, $dbconn,0);
       } else {
           $result = sql_query("SELECT attachment_id, story_id, comment_id, featurized, attachment_file, UNIX_TIMESTAMP(time_posted), hidden FROM ".$prefix."_attachments WHERE ".$filter_by." ORDER BY time_posted DESC LIMIT 0, ".$last_recs, $dbconn,0);
       }
       checkForError($result);

       $total = 0;
       if(sql_num_rows( $result ) > 0)
       {
           $attachments = array();
           $attachment_story = array();
           $attachment_comment = array();
           $attachment_files = array();
           $attachment_times = array();
           $attachment_featured = array();
           $attachment_status = array();
           for ($j = 0; $j < sql_num_rows( $result ); $j++ )
	   {
               list($a_id, $a_story, $a_comment, $a_feat, $a_file, $a_time, $a_hide) = sql_fetch_row($result, $dbconn);
               $attachments[$j] = $a_id;
               $attachment_story[$j] = $a_story;
               $attachment_comment[$j]=$a_comment;
               $attachment_files[$j] = $a_file;
               $attachment_featured[$j] = $a_feat;
               $attachment_times[$j] = $a_time + $system_config->timezone_offset;
               $attachment_status[$j]= $a_hide;
	       $total++;
           }
	   ?>
           <p>
           <table width=100% cellspacing=2 cellpadding=2>
           <tr class=admin>
	   <th class=admin colspan=8>&nbsp; <?=$textLabels['most_recent_publish_msg']?> <?=$total?> <?=$textLabels['attachments_word']?> &nbsp;</th>
           </tr>
	   <tr class="admin">
              <th class="admin" align=center>&nbsp;#&nbsp;</th>
	      <th class="admin" align=center>&nbsp;<?=$textLabels['attachment_id_word']?>&nbsp;</th>
	      <th class="admin" align=center>&nbsp;<?=$textLabels['story_id_word']?>&nbsp;</th>
	      <th class="admin" align=center>&nbsp;<?=$textLabels['comment_id_word']?>&nbsp;</th>
	      <th class="admin" align=center>&nbsp;<?=$textLabels['featurized_word']?>&nbsp;</th>
	      <th class="admin" align=center><?=$textLabels['filename_word']?> </th>
	      <th class="admin" align=center>&nbsp;<?=$textLabels['time_posted_msg']?> &nbsp;</th>
	      <th class="admin" align=center>&nbsp;<?=$textLabels['hidden_status_msg']?>&nbsp;</th>
           </tr>
           <?
           for ($j = 0; $j < $total; $j++ )
	   {
               $a_id = $attachments[$j] ;
               $a_story = $attachment_story[$j] ;
               $a_comment = $attachment_comment[$j] ;
	       if ($a_comment > 0 ) {
	           $a_story_url = $url_base . 'article/' . $a_story. '">' .$a_story.'</a>';
	           $a_comment_url = $url_base . 'article/' . $a_story.'#comment'.$a_comment. '">' .$a_comment.'</a>';
               } else {
	           $a_story_url = $url_base . 'article/' . $a_story. '">' .$a_story.'</a>';
	           $a_comment_url = "N/A";
               }
	       $a_feat =  $attachment_featured[$j];
	       $a_file =  $attachment_files[$j];
	       if (substr($a_file,0,11) == "embedvideo:" || substr($a_file,0,11) == "embedaudio:") {
		   $extra_info = "";
	           if (substr($a_file,0,11) == "embedvideo:") {
		       $v_id = (int) substr($a_file,11,2);
		       $extra_info = " (". getEmbeddedVideoTypes($v_id,false) . " Id: ".substr($a_file,14).")";
		   }
		   if (strlen($a_file) > 70 ) {
		       $t_real_file=substr($a_file,14);
		       $t_path=dirname($t_real_file);
		       $t_name=basename($t_real_file);
		       $a_file_url=substr($a_file,0,13).$extra_info."<br><small>URL=".$t_path."/<BR>Filename=".$t_name."</small>";
		   } else {
		       $a_file_url =  $a_file . $extra_info;
		   }
               } else {
	           $a_file_url =  $url_base_attach .$a_file .'">' .$a_file.'</a>';
		   if (strchr($fileExtensions['image'], strtolower(substr(strrchr($a_file, "."),1)) ) )
	           	$info_url =  $url_base."/editimage.php?subpage=edit&image=".$system_config->attachment_store.$a_file.'">Info</a>';
	           else
	           	$info_url =  $url_base."/editimage.php?subpage=edit&other=".$system_config->attachment_store.$a_file.'">Info</a>';
	           $a_file_url .= " " . $info_url;
               }

               $a_time = strftime("%a %b %d, %T %H:%M", $attachment_times[$j]);
               if ($attachment_status[$j] == null || $attachment_status[$j] == 0 ) $hide_str = "Visible";
	       else if ($attachment_status[$j] == 1) $hide_str = "<b>Hidden</b>";
	       else $hide_str = "<b>Pending</b>";

	      ?>
	      <tr class="stats">
              <td align=center><?=($j+1)?></td>
              <td align=center><?=$a_id?></td>
              <td align=left >&nbsp;<?=$a_story_url ?></td>
              <td align=center><?=$a_comment_url ?></td>
              <td align=center><?=$a_feat?></td>
              <td align=left ><?=$a_file_url?></td>
              <td align=right><?=$a_time?></td>
              <td align=center><?=$hide_str?></td>
              </tr>
              <?
           }
 
	   ?>
	      <tr class="stats">
              <td align=center colspan=8>
           <?
           $this->writeSizeSelection("attachments", "types", $filter_types);
	   if (isset($_REQUEST['filter_types']) && $_REQUEST['filter_types'] == 7) $this->writeSearchOption();
	   ?>
              </td>
              </tr>
           </table>
           <?
       }
   }

   // Show most recent translations links setup
   function writeTranslationRecentIds()
   {
       global $prefix, $dbconn, $system_config, $fileExtensions, $textLabels;
       global $languageList;

       $last_recs = 15;
       if (isset($_REQUEST['size_filter']) && $_REQUEST['size_filter'] > 0) $last_recs = $_REQUEST['size_filter'];

       $http_mode = isset($_SERVER['HTTPS']) ? "https" : "http";
       $url_base = '<a href="'.$http_mode.'://'.$system_config->site_url.'/';
       // $url_base_attach = $url_base . $system_config->attachment_store;


       // $start_pg = 0;
       // $stop_pg = $start_pg + $last_recs
       // BEWARE: If table contains types other than stories -say comment, then query will have to change.
       $result = sql_query("SELECT ct.translation_id, ct.content_type, s.story_title, ct.original_content_id, ct.translated_content_id, s.language_id FROM ".$prefix."_content_translations as ct LEFT JOIN ".$prefix."_stories as s ON (s.story_id=ct.translated_content_id) ORDER BY ct.original_content_id DESC LIMIT 0, ".$last_recs, $dbconn,0);

       checkForError($result);

       $total = 0;
       if(sql_num_rows( $result ) > 0)
       {
           $trans = array();
           $trans_id = array();
           $trans_type = array();
           $trans_org_id = array();
           $trans_org_title = array();
           $trans_org_lang = array();
           $trans_cont_id = array();
           $trans_cont_title = array();
           $trans_cont_lang = array();
           for ($j = 0; $j < sql_num_rows( $result ); $j++ )
	   {
               list($t_id, $t_type, $t_story_title, $t_org_id, $t_content_id, $t_content_lang_id ) = sql_fetch_row($result, $dbconn);
               // echo($t_id.",". $t_type.",". $t_org_id .",". $t_content_id .",". $t_content_lang_id ."<BR>");
               $trans_id[$j] = $t_id;
               $trans_type[$j] = $t_type;
               $trans_org_id[$j]=$t_org_id;
               $trans_cont_id[$j] = $t_content_id;
               $trans_cont_title[$j]=$t_story_title;
               if ($t_content_lang_id == null OR strlen($t_content_lang_id) == 0) {
                   $trans_cont_lang[$j] = "??story gone??";
               } else {
                   $t_langObj = $languageList->getLanguageByID($t_content_lang_id);
// echo("t_content_lang_id ". $t_content_lang_id ."<BR>");
                   $trans_cont_lang[$j] = $t_langObj->getName();
               }
	       $total++;
           }
           for ($j = 0; $j < $total; $j++ )
	   {
               $t_story_id = $trans_org_id[$j];

               $org_result = sql_query("SELECT story_title, language_id FROM ".$prefix."_stories WHERE story_id= ".$t_story_id, $dbconn,0);
               checkForError($org_result);
	       if(sql_num_rows( $result ) > 0) {
                   list($t_story_id, $t_lang_id) = sql_fetch_row($org_result, $dbconn);
                   $trans_org_title[$j] = $t_story_id;
                   if ($t_lang_id == null OR strlen($t_lang_id) == 0) {
                       $trans_org_lang[$j] = $textLabels['story gone_msg'];
                   } else {
                       $t_langObj = $languageList->getLanguageByID($t_lang_id);
                       $trans_org_lang[$j] = $t_langObj->getName();
                   }
	       }
           }

	   ?>
           <p>
           <table width=100% cellspacing=2 cellpadding=2>
           <tr class=admin>
	   <th class=admin colspan=8>&nbsp; <?=$textLabels['most_recent_msg']?> <?=$total?> <?=$textLabels['setup_translation_link']?> &nbsp;</th>
           </tr>
	   <tr class="admin">
              <th class="admin" align=center>&nbsp;#&nbsp;</th>
	      <th class="admin" align=center>&nbsp;<?=$textLabels['type_word']?>&nbsp;</th>
		      <th class="admin" align=center>&nbsp;<?=$textLabels['original_word']?>&nbsp;<br> <?=$textLabels['story_id_word']?>&nbsp;</th>
              <th class="admin" align=center>&nbsp;<?=$textLabels['original_story_title_msg']?>&nbsp;</th>
              <th class="admin" align=center>&nbsp;<?=$textLabels['original_language_msg']?>&nbsp;</th>
              <th class="admin" align=center>&nbsp;<?=$textLabels['translated_word']?>&nbsp;<br> <?=$textLabels['story_id_word']?>&nbsp;</th>
              <th class="admin" align=center>&nbsp;<?=$textLabels['translated_story_title_msg']?>&nbsp;</th>
              <th class="admin" align=center>&nbsp;<?=$textLabels['translated_language_msg']?>&nbsp;</th>
           </tr>
           <?
           $use_class ="admin_highlight";
           $next_class ="admin_fade";
          
           for ($j = 0; $j < $total; $j++ )
	   {
               $t_id = $trans_id[$j] ;
               $t_type = $trans_type[$j] ;
               $t_org_id = $trans_org_id[$j] ;
               $t_org_title = $trans_org_title[$j] ;
               $t_org_lang = $trans_org_lang[$j] ;
               $t_content_id = $trans_cont_id[$j] ;
               $t_cont_title = $trans_cont_title[$j] ;
               $t_content_lang = $trans_cont_lang[$j] ;
	   
	       $a_story_url = $url_base . 'article/' . $t_org_id. '">' .$t_org_id.'</a>';
	       $b_story_url = $url_base . 'article/' . $t_content_id. '">' .$t_content_id.'</a>';
               if ($j > 0 && $t_org_id != $trans_org_id[$j-1]) {
                   $use_class = $next_class;
                   if ($next_class == "admin_highlight") $next_class = "admin_fade";
                   else $next_class = "admin_highlight";
               }

	      ?>
	      <tr class="stats">
              <td align=center><?=($j+1)?></td>
              <td align=center><?=$t_type?></td>
              <td align=center class=<?=$use_class?>>&nbsp;<?=$a_story_url ?></td>
              <td align=center class=<?=$use_class?>>&nbsp;<?=$t_org_title ?></td>
              <td align=center><?=$t_org_lang?></td>
              <td align=center><?=$b_story_url ?></td>
              <td align=center><?=$t_cont_title?></td>
              <td align=center><?=$t_content_lang?></td>
              </tr>
              <?
           }
 
	   ?>
	      <tr class="stats">
              <td align=center colspan=8>
           <?
           $this->writeSizeSelection("translations");
	   ?>
              </td>
              </tr>
           </table>
           <?
       }
   }

   // This runs a big query to make sure there are no stories with topic, region and type ids that do not exist
   // in the database. This could happen if somehow the ids for these changed.
   function validateStoryTableData()
   {
       global $prefix, $dbconn, $topicList, $regionList, $typeList, $languageList;
       global $textLabels;

       // All topics, regions and types should be already loaded due to init.
       $topicArray  = $topicList->getTopics($languageList->getMinLanguageId());
       $regionArray = $regionList->getRegions($languageList->getMinLanguageId());
       $typeArray   = $typeList->getTypes($languageList->getMinLanguageId());

       $where_str ="";
       foreach ($topicArray as $topic) {
           $where_str .= $topic->topic_id.",";
       }
       $where_str = substr($where_str,0,(strlen($where_str)-1));
       // $where_str = "1,2,3,4,5,7,9,10,11,16,17,18,19";
       // $where_str = "2,3,4,5,7,9,10,11,16,17,18,19";

       $validateQuery = "SELECT COUNT(1) FROM ".$prefix."_stories WHERE topic_id NOT IN (".$where_str.")";
       $cnt_result = sql_query($validateQuery, $dbconn,0);
       checkForError($cnt_result);
       ?>
       <p>
           <table width=100% cellspacing=2 cellpadding=2>
           <tr class=admin>
	   <td class=admin colspan=6>&nbsp; <?=$textLabels['validation_intro']?> &nbsp;</th>
           </tr>
           <tr class=admin>
	   <th class=admin colspan=6>&nbsp; <?=$textLabels['validate_topics_text']?> &nbsp;</th>
           </tr>
       <?

       if(sql_num_rows( $cnt_result ) > 0) {
           list($t_count) = sql_fetch_row($cnt_result, $dbconn);
	   ?><tr class=admin>
	     <td class=admin colspan=6>&nbsp; <?=$textLabels['stories_undef_topic']?> = <?=$t_count?> &nbsp;</td>
	   </tr>
	   <?

           $result = sql_query("SELECT story_id, story_title, author_name, topic_id, UNIX_TIMESTAMP(time_posted), hidden FROM ".$prefix."_stories WHERE topic_id NOT IN (".$where_str.") ORDER BY time_posted DESC LIMIT 0, 50", $dbconn,0);
           $this->retrieveAndDisplayStories("Topic Id", $result);
       }

       // Validate regions
       $where_str ="";
       foreach ($regionArray as $region) {
           $where_str .= $region->region_id.",";
       }
       $where_str = substr($where_str,0,(strlen($where_str)-1));

       $validateQuery = "SELECT COUNT(1) FROM ".$prefix."_stories WHERE region_id NOT IN (".$where_str.")";
       $cnt_result = sql_query($validateQuery, $dbconn,0);
       checkForError($cnt_result);
       ?>
           <tr class=admin> <td class=admin colspan=6>&nbsp; &nbsp;</td> </tr>
           <tr class=admin>
	   <th class=admin colspan=6>&nbsp; <?=$textLabels['validate_regions_text']?> &nbsp;</th>
           </tr>
       <?
       if(sql_num_rows( $cnt_result ) > 0) {
           list($t_count) = sql_fetch_row($cnt_result, $dbconn);
	   ?><tr class=admin>
	     <td class=admin colspan=6>&nbsp; <?=$textLabels['stories_undef_region']?> = <?=$t_count?> &nbsp;</td>
	   </tr>
	   <?

           $result = sql_query("SELECT story_id, story_title, author_name, region_id, UNIX_TIMESTAMP(time_posted), hidden FROM ".$prefix."_stories WHERE region_id NOT IN (".$where_str.") ORDER BY time_posted DESC LIMIT 0, 50", $dbconn,0);
           $this->retrieveAndDisplayStories("Region Id", $result);
       }


       // Validate for types
       $where_str ="";
       foreach ($typeArray as $type) {
           $where_str .= $type->type_id.",";
       }
       $where_str = substr($where_str,0,(strlen($where_str)-1));

       $validateQuery = "SELECT COUNT(1) FROM ".$prefix."_stories WHERE type_id NOT IN (".$where_str.")";
       $cnt_result = sql_query($validateQuery, $dbconn,0);
       checkForError($cnt_result);
       ?>
           <tr class=admin> <td class=admin colspan=6>&nbsp; &nbsp;</td> </tr>
           <tr class=admin>
	   <th class=admin colspan=6>&nbsp; <?=$textLabels['validate_type_text']?> &nbsp;</th>
           </tr>
       <?

       if(sql_num_rows( $cnt_result ) > 0) {
           list($t_count) = sql_fetch_row($cnt_result, $dbconn);
	   ?><tr class=admin>
	     <td class=admin colspan=6>&nbsp;<?=$textLabels['stories_undef_type']?> = <?=$t_count?> &nbsp;</td>
	   </tr>
	   <?

           $result = sql_query("SELECT story_id, story_title, author_name, type_id, UNIX_TIMESTAMP(time_posted), hidden FROM ".$prefix."_stories WHERE type_id NOT IN (".$where_str.") ORDER BY time_posted DESC LIMIT 0, 50", $dbconn,0);
           $this->retrieveAndDisplayStories("Type Id", $result);
       }

       // Validate languages
       $where_str ="";
       $languageArray   = $languageList->getLanguages();
       foreach ($languageArray as $t_lang) {
           $where_str .= $t_lang->language_id.",";
       }
       $where_str = substr($where_str,0,(strlen($where_str)-1));

       $validateQuery = "SELECT COUNT(1) FROM ".$prefix."_stories WHERE language_id NOT IN (".$where_str.")";
       $cnt_result = sql_query($validateQuery, $dbconn,0);
       checkForError($cnt_result);
       ?>
           <tr class=admin> <td class=admin colspan=6>&nbsp; &nbsp;</td> </tr>
           <tr class=admin>
	   <th class=admin colspan=6>&nbsp; <?=$textLabels['validate_language_text']?> &nbsp;</th>
           </tr>
       <?
       if(sql_num_rows( $cnt_result ) > 0) {
           list($t_count) = sql_fetch_row($cnt_result, $dbconn);
	   ?><tr class=admin>
	     <td class=admin colspan=6>&nbsp;<?=$textLabels['stories_undef_language']?> = <?=$t_count?> &nbsp;</td>
	   </tr>
	   <?

           $result = sql_query("SELECT story_id, story_title, author_name, language_id, UNIX_TIMESTAMP(time_posted), hidden FROM ".$prefix."_stories WHERE language_id NOT IN (".$where_str.") ORDER BY time_posted DESC LIMIT 0, 50", $dbconn,0);
           $this->retrieveAndDisplayStories("Language Id", $result);
       }

       ?>
           </table>
       <?
   }

   // This both retries and display the stories from the query made by the caller.
   function retrieveAndDisplayStories($validate_field_text, $db_result)
   {
       global $dbconn;

       checkForError($db_result);
       if(sql_num_rows( $db_result ) > 0) {
	   $this->writeStoryHdr($validate_field_text);

           for ($j = 0; $j < sql_num_rows( $db_result ); $j++ )
	   {
	       // Actually topic can be region, type or language also.
               list($t_story_id, $t_story_title, $t_author, $t_topic, $t_time, $t_hide) = sql_fetch_row($db_result, $dbconn);
               $this->writeStoryData($t_story_id,$t_story_title,$t_author, $t_topic, $t_time, $t_hide);
           }
       } 
   }

   function writeStoryHdr($variable_column)
   {
       $label_story_id = "Story Id";
       $label_story_title="Story Title";
       $label_author="Author";

       $label_time  = "Time";
       $label_hide  ="Hidden";
       echo "<tr class=admin>";
       echo "<th class=admin>".$label_story_id."</th>";
       echo "<th class=admin>".$label_story_title."</th>";
       echo "<th class=admin>".$label_author."</th>";
       echo "<th class=admin>".$variable_column."</th>";
       echo "<th class=admin>".$label_time."</th>";
       echo "<th class=admin>".$label_hide."</th>";
       echo "</tr>";

   }
   function writeStoryData($t_story_id,$t_story_title,$t_author, $t_topic, $t_time, $t_hide)
   {
       $time_str = strftime("%a %b %d %Y, %H:%M", $t_time);

       if ($t_hide == null || $t_hide == 0 ) $hide_str = "Visible";
       else if ($t_hide == 1) $hide_str = "<b>Hidden</b>";
       else $hide_str = "<b>Pending</b>";

       echo "<tr class=admin>";
       echo "<td class=admin>".$t_story_id."</td>";
       echo "<td class=admin>".$t_story_title."</td>";
       echo "<td class=admin>".$t_author."</td>";
       echo "<td class=admin>".$t_topic."</td>";
       echo "<td class=admin>".$time_str."</td>";
       echo "<td class=admin>".$hide_str."</td>";
       echo "</tr>"; 
   }

   function writeInfoItem($hdr_colspan, $details_colspan, $header,$details)
   {
   ?>
   <tr class=admin>
      <td class=admin colspan=<?=$hdr_colspan?>><?=$header?></td><td class=admin colspan=<?=$details_colspan?>> <?=$details?></td>
   </tr>
   <?
   }

}

ob_start();

$admin_table_width = "85%";
if($editor_session->isSessionOpen())
{
   
   //writeAdminHeader($OSCAILT_SCRIPT."?stories=true","Recent stories",array("?comments=true" => "Recent Comments", "?attachments=true" => "Recent Attachments", "?translations=true" => "Recent Translation Links"));
   writeAdminHeader($OSCAILT_SCRIPT."?stories=true", $textLabels['recent_stories_msg'], array("?comments=true" => $textLabels['recent_comments_msg'], "?attachments=true" => $textLabels['recent_attachments_msg'], "?translations=true" => $textLabels['recent_translated_links_msg'], "?lost=true" => $textLabels['lost_stories']));

   $debugObj = new debugDisplay();

   // There are 3 modes of display....

   if ( isset($_REQUEST['stories'])) $debugObj->writeStoryRecentIds();
   else if ( isset($_REQUEST['comments'])) $debugObj->writeStoryRecentIds(false);
   else if ( isset($_REQUEST['attachments'])) $debugObj->writeAttachmentRecentIds();
   else if ( isset($_REQUEST['translations'])) $debugObj->writeTranslationRecentIds();
   else if ( isset($_REQUEST['lost'])) $debugObj->validateStoryTableData();
   else $debugObj->writeStoryRecentIds();

   // Found in adminutils.inc
   writeAdminPageFooter();
}
else
{
   $editor_session->writeNoSessionError();
}

// The footer calls the code to disconnect from the db and whatever else needs to be done.
require_once("adminfooter.inc");
?> 
