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

/************************************************************************/
/* Latest comments view of system - this will soon be a full bulletin   */
/* board.  All it needs is to be divided up by category and to have     */
/* links to further pages.                                              */
/************************************************************************/
require_once('sitefiltereddisplay.inc');
require_once('objects/comment.inc');

class commentsDisplay extends siteFilteredDisplay
{

   function commentsDisplay ($obj_id)
   {
      $this->siteFilteredDisplay($obj_id);
   }

   //placeholder for bb
   function addSection($section_parameters, $filters)
   {

   }

   function getFilterClauses()
   {
      global $topicList, $regionList, $typeList, $languageList, $editor_session;
      $this->integrateFilters();
      $current_filters = array();
      $all_filters = array();
      foreach($this->query->topic_id as $t_id)
      {
         $current_filters[] = "s.topic_id=$t_id";
      }
      if(!empty($current_filters))
      {
         $all_filters[] = implode(" OR ", $current_filters);
      }
      $current_filters = array();
      foreach($this->query->type_id as $t_id)
      {
         $current_filters[] = "s.type_id=$t_id";
      }
      if(!empty($current_filters))
      {
         $all_filters[] = implode(" OR ", $current_filters);
      }
      $current_filters = array();
      foreach($this->query->region_id as $r_id)
      {
         $current_filters[] = "s.region_id=$r_id";
      }
      if(!empty($current_filters))
      {
         $all_filters[] = implode(" OR ", $current_filters);
      }
      $current_filters = array();
      foreach($this->query->language_id as $l_id)
      {
         $current_filters[] = "s.language_id=$l_id";
      }
      if(!empty($current_filters))
      {
         $all_filters[] = implode(" OR ", $current_filters);
      }
      $current_filters = array();
      foreach($this->author_names as $a)
      {
         $a = addslashes($a);
         if($system_config->use_vague_author_name)
         {
            $current_filters[] = "s.author_name LIKE '%$a%'";
         }
         else
         {
            $current_filters[] = "s.author_name='$a'";
         }
      }
      if(!empty($current_filters))
      {
         $all_filters[] = implode(" OR ", $current_filters);
      }
      if(!$editor_session->isSessionOpen())
      {
         $current_filters = array();
         $excludedTopics=$topicList->getExcludedTopics();
         for ($i=0; $i<count($excludedTopics); $i++)
         {
            $current_filters[] = "s.topic_id != ".$excludedTopics[$i]->topic_id;
         }
         if(!empty($current_filters))
         {
            $all_filters[] = "(".implode(" OR ", $current_filters).")";
         }
         $current_filters = array();
         $excludedRegions=$regionList->getExcludedRegions();
         for ($i=0; $i<count($excludedRegions); $i++)
         {
            $current_filters[] = "s.region_id != ".$excludedRegions[$i]->region_id;
         }
         if(!empty($current_filters))
         {
            $all_filters[] = "(".implode(" OR ", $current_filters).")";
         }
         $current_filters = array();
         $excludedTypes=$typeList->getExcludedTypes();
         for ($i=0; $i<count($excludedTypes); $i++)
         {
            $current_filters[] = "s.type_id != ".$excludedTypes[$i]->type_id;
         }
         if(!empty($current_filters))
         {
            $all_filters[] = "(".implode(" OR ", $current_filters).")";
         }
         $current_filters = array();
         $excludedLanguages=$languageList->getExcludedLanguages();
         for ($i=0; $i<count($excludedLanguages); $i++)
         {
            $current_filters[] = "s.language_id != ".$excludedLanguages[$i]->language_id;
         }
         if(!empty($current_filters))
         {
            $all_filters[] = "(".implode(" OR ", $current_filters).")";
         }
      }
      if(empty($all_filters))
      {
         return "";
      }
      else
      {
         $str = implode(" AND ", $all_filters);
         return $str;
      }
   }

   function getLatestComments($show_hidden_only = false)
   {
       global $dbconn,$editor_session,$prefix;
       $where_clause = $this->getFilterClauses();

       $read_content = false;
       if($editor_session->isSessionOpen() && $editor_session->editor->allowedReadAccessTo("viewhiddencontent"))
       {
           $read_content = true;
           if($where_clause != "") $where_clause = "WHERE ".$where_clause;

           $stmt = "select c.comment_id, c.story_id, UNIX_TIMESTAMP(c.time_posted), c.author_name, c.comment_title, c.hidden, c.comment_content, s.story_title, s.hidden from ".$prefix."_comments as c left join ".$prefix."_stories as s on s.story_id=c.story_id $where_clause order by c.time_posted DESC limit 0, 100";
       }
       else
       {
           if($where_clause != "") $where_clause = "AND ".$where_clause;
           $stmt = "select c.comment_id, c.story_id, UNIX_TIMESTAMP(c.time_posted), c.author_name, c.comment_title, c.hidden, s.story_title, s.hidden from ".$prefix."_comments as c left join ".$prefix."_stories as s on s.story_id=c.story_id where c.hidden='' and s.hidden='' $where_clause order by c.time_posted DESC limit 0, 100";
       }
       //execute statement
       $result = sql_query($stmt, $dbconn, 2);
       checkForError($result);
       $comments = array();
       if(sql_num_rows( $result ) > 0)
       {
           for ($i=0; $i<sql_num_rows( $result ); $i++)
           {
               $comment = new Comment();

               if($read_content == true ) {
                   list($comment->comment_id, $comment->story_id, $comment->time_posted, $comment->author_name, $comment->comment_title, $comment->hidden, $comment->comment_content, $comment->parent_story_title, $comment->parent_story_hidden) = sql_fetch_row($result, $dbconn);
               }
	       else
               {
                   list($comment->comment_id, $comment->story_id, $comment->time_posted, $comment->author_name, $comment->comment_title, $comment->hidden, $comment->parent_story_title, $comment->parent_story_hidden) = sql_fetch_row($result, $dbconn);
               }

               if($this->getDisplayOption('show_all_comments')=="true") array_push($comments,$comment);
               else $this->addToComments($comments,$comment);
           }
       }
       return $comments;
   }



   function addToComments(&$comments,&$comment)
   {
       for ($i=0; $i<count( $comments ); $i++)
       {
          if($comments[$i]->story_id==$comment->story_id) return;
       }
       array_push($comments,$comment);
   }

   function writeLatestCommentTable()
   {
       global $system_config, $editor_session;

       if (isset($_REQUEST['hiddenonly']) && $_REQUEST['hiddenonly'] == 'true') {
           $comments = $this->getLatestComments(true);
       } else {
           $comments = $this->getLatestComments();
       }
       $cls_str = "class='latestcomments'";
       $number_cols = 3;
       ?>
       <table width="100%" class=latestcomments>
       <TR class=latestcomments>
       <TH class=latestcomments> Story Title </th>
       <?
       if ($this->getDisplayOption("show_comment_titles") == "true")
       {
           $number_cols = 4;
           ?> <TH class=latestcomments> Comment Title </font></th> <?
       }
       ?>
       <TH class=latestcomments> Comment Author </th>
       <TH class=latestcomments> Comment Posted </th>
       </tr>
       <?
       // Count pending comments if delay on.
       if ($system_config->publish_comment_delay > 0 && !$editor_session->isSessionOpen()) {
           $pending_cnt = 0;
	   $t_diff = time() - 60*$system_config->publish_comment_delay;
           for($j=0;$j<count($comments);$j++)
           {
               $comment=$comments[$j];
	       if ($comment->time_posted > $t_diff) {
                   $pending_cnt++;
               }
           }
	   if ($pending_cnt > 0) {
               if ($pending_cnt == 1) {
                   ?> <TD colspan=<?=$number_cols?> class=latestcomments_delay>1 comment is pending</TD><?
               } else {
                   ?> <TD colspan=<?=$number_cols?> class=latestcomments_delay> <?=$pending_cnt?> comments are pending </TD> <?
               }
           }
       }

       for($j=0;$j<count($comments);$j++)
       {
           $comment=$comments[$j];
           $cls_str = "class='latestcomments'";
	   $cmt_cls = "latestcomments";
           $cls_txt_str = "latestcomments_content";

	   if ($system_config->publish_comment_delay > 0 ) {
	       if ($comment->time_posted > (time() - 60*$system_config->publish_comment_delay)) {
                   // If you are not logged in, then you can't see them.
                   if(!$editor_session->isSessionOpen()) continue;

                   $cls_str = "class='latestcomments_delay'";
                   $cmt_cls = "latestcomments_delay";
                   $cls_txt_str = "latestcomments_delay";
               }
           }

           ?>
           <TR class=latestcomments>
           <?

           if ($this->getDisplayOption("show_comment_titles") == "true")
           {
               $this->query->story_id = $comment->story_id;
               $lnk = $this->getFullLink($this->site_settings['articleobject']);
               echo "<TD $cls_str>\n";
               if($comment->parent_story_hidden==true)
               {
                  echo("<B><font class=editornotice>[hidden]</font></B>");
               }
               echo "<A $cls_str href='$lnk'>" . trim($comment->parent_story_title)."</a>\n";
               echo "</TD>\n";
               $lnk = addLinkParameters($lnk, array("comment_limit=0", "condense_comments=false"));
               $lnk.="#comment".$comment->comment_id;
               echo "<TD $cls_str>\n";
               echo $this->getItemHighlights($comment, true);
               echo "<A href='$lnk' $cls_str>".trim($comment->comment_title)."</A>";
               echo "</TD>\n";
               $this->query->story_id = null;
           }
           else
           {
               $this->query->story_id = $comment->story_id;
               $lnk = $this->getFullLink($this->site_settings['articleobject']);
               $lnk .= "&amp;comment_limit=0&amp;condense_comments=false#comment".$comment->comment_id;
               echo "<TD $cls_str>\n";
               if($comment->parent_story_hidden==true)
               {
                  echo("<B><font class=editornotice>[story hidden]</font></B>");
               }
               echo $this->getItemHighlights($comment, true);
               echo "<A href='$lnk' $cls_str>".trim($comment->parent_story_title)."</a>\n";
               echo "</TD>\n";
           }
           ?>
           <TD class=<?=$cmt_cls?>><?=trim(substr($comment->author_name,0,15))?></TD>
           <TD class=<?=$cmt_cls?>><I><?=date("g:ia ",$comment->time_posted+$system_config->timezone_offset)?><?=substr(date("M",($comment->time_posted+$system_config->timezone_offset)),0,1)?><?=date("j",$comment->time_posted+$system_config->timezone_offset)?></I></TD>
           </TR>
           <?
           if($editor_session->isSessionOpen() && $editor_session->editor->allowedReadAccessTo("viewhiddencontent"))
           {
               if ($this->getDisplayOption("show_comment_text") != "true") continue;

	       if ($comment->hidden == true) $cls_txt_str = "latestcomments_hidden";

               ?>
               <TR class=latestcomments>
               <TD colspan='<?=$number_cols?>' class=<?=$cls_txt_str?>><?=trim($comment->comment_content)?><BR><BR></TD>
               </TR>
               <?
           }
       }
       ?>
       <tr>
       <td colspan=3><br></td></tr></table><br>
       <?
   }

   function writeCommentViewOptions()
   {
      global $user_prefs, $editor_session;
      //all of our non-standard view stuff should be added to links....
      echo "<div class=events-viewoptions>";
      echo '<span class="view-option">';
      if ($this->getDisplayOption("show_comment_titles") == "true")
      {
         $msg = $this->getDisplayOption('no_titles');
         $user_prefs->setQueryOption("show_comment_titles", "false");
      }
      else
      {
         $msg = $this->getDisplayOption('comment_titles');
         $user_prefs->setQueryOption("show_comment_titles", "true");
      }
      echo '<A class="view-option" href="'.$this->getFullLink().'">'.$msg."</a></span>\n";
      $user_prefs->unsetQueryOptions();
      echo "<br />";
      echo '<span class="view-option">';
      if ($this->getDisplayOption("show_all_comments") == "true")
      {
         $msg = $this->getDisplayOption('per_story');
         $user_prefs->setQueryOption("show_all_comments", "false");
      }
      else
      {
         $msg = $this->getDisplayOption('all_comments');
         $user_prefs->setQueryOption("show_all_comments", "true");
      }
      echo '<A class="view-option" href="'.$this->getFullLink().'">'.$msg."</a></span>\n";
      echo "<br />";

      // These preferences can only be used by editors anyhow.
      // echo("no_comment_text " .  $this->getDisplayOption('no_comment_text') ."<BR>");
      // echo("view_comment_text " .  $this->getDisplayOption('view_comment_text') ."<BR>");
      if($editor_session->isSessionOpen() && $editor_session->editor->allowedReadAccessTo("viewhiddencontent"))
      {
          echo '<span class="view-option">';
          if ($this->getDisplayOption("show_comment_text") == "true")
          {
             $msg = $this->getDisplayOption('no_comment_text');
             $user_prefs->setQueryOption("show_comment_text", "false");
          }
          else
          {
             $msg = $this->getDisplayOption('view_comment_text');
             $user_prefs->setQueryOption("show_comment_text", "true");
          }
          echo '<A class="view-option" href="'.$this->getFullLink().'">'.$msg."</a></span>\n";
          echo "<br />";
      }

      $user_prefs->unsetQueryOptions();
      $this->writeSavePrefsLink();
      echo "</div>";
   }

   function writeOtherViewLinks()
   {
      $archlink = $this->getSimpleLink($this->site_settings['archiveobject']);
      $comlink = $this->getSimpleLink($this->site_settings['newswireobject']);
      $gallink = $this->getSimpleLink($this->site_settings['galleryobject']);
      $featlink = $this->getSimpleLink($this->site_settings['featureobject']);
      ?>
      <A class="sitenavigation" href="<?=$archlink?>">view stories by date</A>&nbsp; | &nbsp;
      <A class="sitenavigation" href="<?=$comlink?>">view newswire</A>&nbsp; | &nbsp;
      <A class="sitenavigation" href="<?=$featlink?>">view features</A>&nbsp; | &nbsp;
      <A class="sitenavigation" href="<?=$gallink?>">view images</a>
      <?
   }


   function writeCommentsHeader()
   {
      global $pageFilterText, $system_config;
      if(isset($pageFilterText) && $pageFilterText != "")
      {
         $txt = $pageFilterText." ".$this->getDisplayOption('title');
      }
      else
      {
         $txt = $this->getDisplayOption('title');
      }
      $this->writeCommentViewOptions();
      echo "<H3>$txt</H3>";
      echo "<p class='intro'>";
      echo $this->getDisplayOption('introtext')."</p>";
      //$this->writeViewChoiceBar();

      // If this is a publish comment and the publish comment delay is in effect, then tell users.
      if($system_config->publish_comment_delay > 0 ) {
          echo("<p><strong>Note:</strong> Publish Delay of " .$system_config->publish_comment_delay ." minutes is in effect.</p>");
      }

   }

   function processRequest()
   {
      parent::processRequest();
      SharedMemoryUpdate("comments");
      $this->display();
   }

   function display($res = 0)
   {
      $this->writeCommentsHeader();
      $this->writeLatestCommentTable();
      $this->writeDisplayFooter();
   }

}
?>
