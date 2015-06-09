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
require_once('objects/indyruntime/attachmentbearingdisplay.inc');
require_once('objects/memorymgmt.inc');

class articleDisplay extends attachmentBearingDisplay
{
   var $contains_hidden_items = false;
   var $story_title = NULL;
   var $hide_reasons = NULL;

   function articleDisplay ($obj_id)
   {
      $this->attachmentBearingDisplay($obj_id);
   }

   function setValidOptions()
   {
      $this->valid_item_options = array(
         "attachment" => array('clip', "edit", "hide", "unhide", "delete", "featurize", "unfeaturize", "swap"),
         "story" => array('clip',  "upgrade", "edit", "hide", "unhide", "lock", "unlock", "delete", "stick", "unstick", "release", "translate"),
         "feature" => array('clip', "edit", "hide", "unhide", "lock", "unlock","delete", "stick", "unstick", "release","translate"),
         "comment" => array("multi", 'clip', 'upgrade', "edit", "hide", "unhide", "delete", "release")
      );
   }

   function writePrintBox(&$story)
   {
      global $graphics_store;
      if(!isset($_GET['print_page']))
      {
         // Adding rel=nofollow to links to stop google-bot following them.
         //$save_query = $this->query->copy();
         $link = $this->getFilterLink();
         $link = addLinkParameters($link, array("print_page=true"));
         ?>
            <span class='storysummary-detail'>
            <a class='category-text' target="_blank" href="<?=$link?>" rel="nofollow"><img class='icon' src="<?=$graphics_store?>printerfriendly.gif">printable version</a>
            </span>
         <?
         $link = $this->getFullLink();
         $link = addLinkParameters($link, array("include_comments=true", "print_page=true"));
         ?>
            <span class='storysummary-detail'>
            <a class='category-text' target="_blank" href="<?=$link?>" rel="nofollow"><img  class='icon' src="<?=$graphics_store?>printerfriendly.gif">with comments</a>
            </span>
         <?
      }
   }

   function writeSocialBookmarks(&$story)
   {
      global $system_config, $graphics_store;

      if(isset($_GET['print_page'])) return;
      if($story->hidden) return;

      $encoded_url = urlencode("http://".$system_config->site_url."/article/". $story->story_id); 
      // Bookmarks for: 1) Digg, 2) Del.Icio.Us, 3) Furl, 4) Reddit, 5) Technorati
      ?>
      <div>
      <a href="http://digg.com/submit?phase=2&url=<?=$encoded_url?>&title=<?php echo urlencode($story->story_title);?>"><img src="<?=$graphics_store?>/digg.png" /> Digg this</a> &nbsp;
      <a href="http://del.icio.us/post?v=2&url=<?=$encoded_url?>&title=<?php echo urlencode($story->story_title);?>" title="Post to del.icio.us"><img src="<?=$graphics_store?>/delicious.png" /> del.icio.us</a> &nbsp;
      <a href="http://www.furl.net/storeIt.jsp?t=<?php echo urlencode($story->story_title);?>&u=<?=$encoded_url?>"><img src="<?=$graphics_store?>/furl.png" /> Furl</a> &nbsp;
      <a href="http://reddit.com/submit?url=<?=$encoded_url?>&title=<?php echo urlencode($story->story_title);?>"><img src="<?=$graphics_store?>/reddit.png" /> Reddit</a> &nbsp;
      <a href="http://tecnorati.com/faves/?add=<?=$encoded_url?>"><img src="<?=$graphics_store?>/technorati.png" /> Technorati</a> &nbsp;
      </div>
      <?
   }

   function writeFeedBox(&$story)
   {
      global $graphics_store;
      writeHelpJS();
      if($story->hidden) return;
      $rss_feed_id = $this->getDisplayOption("rss_article_feed");
      $atom_feed_id = $this->getDisplayOption("atom_article_feed");

      if(isset($_GET['print_page'])) return;

      if($atom_feed_id > 0)
      {
         $atom_link = $this->getSimpleLink($atom_feed_id);
         $atom_link = addLinkParameters($atom_link, array("story_id=$story->story_id"));
      $atom_txt = htmlspecialchars($this->getDisplayOption("atom_text"), ENT_QUOTES);
      echo "<span class='storysummary-detail'>";
         echo " <a class='category-text' href='$atom_link'><img class='icon' src='".$graphics_store."atomcommentsbutton.png' alt='atom feed of comments' />$atom_txt</a>";
      echo "</span>";
      }
      if($rss_feed_id > 0)
      {
         $rss_link = $this->getSimpleLink($rss_feed_id);
         $rss_link = addLinkParameters($rss_link, array("story_id=$story->story_id"));
         $rss_txt = htmlspecialchars($this->getDisplayOption("rss_text"), ENT_QUOTES);
        echo "<span class='storysummary-detail'>";
         echo "<a class='category-text' href='$rss_link'><img class='icon' src='".$graphics_store."rsscommentsbutton.png' alt='rss feed of comments' />$rss_txt</a>";
        echo "</span>";
      }
      if($rss_feed_id > 0 || $atom_feed_id > 0)
      {
           $feed_help = $this->getDisplayOption("feed_article_help");
           $help_html = getHelpHTML($feed_help, false, true);
         echo $help_html;
      }
   }

   function writeReportPostIcon(&$item, $contact_id, $is_comment=false)
   {
      global $system_config, $graphics_store;
      if($item->hidden or isset($_GET['print_page'])) return;
      $lnk = $this->getSimpleLink($contact_id);
      $txt = $this->getDisplayOption("report_post_text");
      $subj = $this->getDisplayOption("report_post_subject");
      if($is_comment)
      {
         $action_bit = "#comment_id=".$item->comment_id;
         $subj .= ": Comment $item->comment_id with title: $item->comment_title";
         $msg_subj = $subj . urlencode("\n") . "Parent Story title: " . $this->story_title;
      }
      else
      {
         $action_bit = "";
         $subj .= ": Story $item->story_id with title: $item->story_title";
         $msg_subj = $subj;
      }
      $hide_lnk = "http://".$system_config->site_url."/index.php?obj_id=".$this->obj_id;
      $hide_lnk .= "&story_id=$item->story_id&".$action_bit;
      $hide_lnk .= "\n\nEnter your reason here - please do not remove the above link as it will allow an editor to easily remove the offending content";
      //$hide_lnk = addLinkParameters($hide_lnk, array("story_id=$item->story_id", $action_bit));
      $lnk = addLinkParameters($lnk, array("subject=$subj", "message=".$msg_subj.urlencode("\n".$hide_lnk)));
      echo "<a href='$lnk'>";
      echo "<img class='icon' alt='$txt' title='$txt' src='".$graphics_store."report.gif'>";
      echo "</a>";
   }

   function display($res = 0)
   {
      global $system_config, $editor_session;
      if($this->query->comment_id!=null && $editor_session->isSessionOpen()) // editors are allowed view comments on separate page to enable copying and moving of attachments
      {
         $cls = "comment_navig";
         $comment = new Comment();
         $comment->comment_id = $this->query->comment_id;
         if($comment->load() == false)
         {
            writeFailureMessage("Comment $comment->comment_id: ".$this->getDisplayOption("emptyresultmsg"), $this->getDisplayOption("no_comment_with_id_message"));
            return;
         }
         $this->query->story_id=$comment->story_id;
         $this->writeComment($comment,true);
         $backup_comment_id=$this->query->comment_id;
         $this->query->comment_id=NULL;
         $lnk_dest = $this->getFilterLink();
         echo "<P class=$cls><A href='$lnk_dest'>".$this->getDisplayOption('back_to_article_message')."</A></P>";
         $this->query->comment_id=$backup_comment_id;
         if($editor_session->isSessionOpen())
         {
            $editor_session->clipboard->writeClipboard($this->query);
         }
         return;
      }
      if($this->query->story_id==null)
      {
         writeFailureMessage($this->getDisplayOption("emptyresultmsg"), $this->getDisplayOption("user_error_message_noid"));
         return;
      }
      $story = new Story();
      $story->story_id = $this->query->story_id;
      if($story->load() === false)
      {
         writeFailureMessage("Story ".$story->story_id.": ".$this->getDisplayOption("emptyresultmsg"), $this->getDisplayOption("no_story_with_id_message"));
         return;
      }
      global $pageTopic, $pageRegion, $pageType, $pageLanguage, $oscailt_page_setting;
      $pageTopic=$story->getTopic();
      $pageRegion=$story->getRegion();
      $pageType=$story->getType();
      $pageLanguage=$story->getLanguage();
      $oscailt_page_setting['page_author'] = $story->author_name;
      //need to check to see if story has a bad topic/type/region/language
      if($this->storyFallsOutsideFilters($story))
      {
         writeFailureMessage($this->getDisplayOption("emptyresultmsg"), $this->getDisplayOption("notviewable_message"));
         return;
      }
      if($story->hidden==false || ($editor_session->isSessionOpen() && $editor_session->editor->allowedReadAccessTo("viewhiddencontent")))
      {
	 SharedMemoryUpdate("article");
	 UpdateArticleTracker($story->story_id);
         $this->writeArticleHeader($story);

         $comments_written = $this->writeStoryWithComments($story);
         //if($comments_written == 0)
         //{
         //   echo "<hr>";
         //}
         if($editor_session->canEditContent())
         {
            $possible_actions = array("clip", "hide", "unhide", "delete");
            if($this->getDisplayOption('condense_comments') != 'true')
            {
               $this->writeMultiEditOptions("comment", $possible_actions, $comments_written);
            }
            $editor_session->clipboard->writeClipboard($this->query);
         }
         $this->writeArticleFooter($story);
      }
      else
      {
         writeFailureMessage($this->getDisplayOption("emptyresultmsg"), $this->getDisplayOption('hiddenmessage'));
      }
   }

   function storyAuthorNameIncluded(&$story)
   {
      if(count($this->author_names) == 0)
      {
         return true;
      }
      foreach($this->author_names as $auth_name)
      {
         $p = "";
         similar_text(strtoupper($auth_name), strtoupper($story->author_name), $p);
         if($p > 90) return true;
      }
      return false;
   }

   function storyFallsOutsideFilters(&$story)
   {
      global $topicList, $languageList, $regionList, $typeList;
      //this is where we integrate the user filters with object set ones
      //do it through the query
      if(!((empty($this->topic_id) or in_array($story->topic_id, $this->topic_id)) and
         (empty($this->region_id) or in_array($story->region_id, $this->region_id)) and
         (empty($this->type_id) or in_array($story->type_id, $this->type_id)) and
         (empty($this->language_id) or in_array($story->language_id, $this->language_id)) and
         $this->storyAuthorNameIncluded($story)))
      {
         return true;
      }
      global $pageTopic, $pageLanguage, $pageRegion, $pageType, $editor_session;
      if(!($editor_session->isSessionOpen() and $editor_session->editor->allowedReadAccessTo("viewprotectedcategories"))and ($pageTopic->excluded or $pageRegion->excluded or $pageLanguage->excluded or $pageType->excluded))
      {
         return true;
      }
      return false;
   }

   function displayPreview($item, $item_type)
   {
      global $system_config, $editor_session;
      $this->query = new StoryQuery();
      $this->onwards_query = new StoryQuery();
      if($item->time_posted == null or $item->time_posted == 0)
      {
         $item->time_posted = time();
      }
      if($item_type == 'comment')
      {
         // $this->writeComment($item,false);
         // The two zeros would be defaulted anyhow.
         $this->writeComment($item,true, 0, 0, false);
      }
      else
      {
         $con = $item->story_content;
         // $comments_written = $this->writeStory($item,false,false);
         $comments_written = $this->writeStory($item,true,false, false);
         $item->story_content = $con;
      }
   }

   function writeArticleFooter($story)
   {
      $this->writeStoryNavigation($story);
   }

   function writeStoryNavigation(&$story)
   {
      if(!isset($_REQUEST['print_page']))
      {
         echo '<div class="site-navigation">';

         $backup_story_id=$this->query->story_id;
         $this->query->story_id=NULL;

         $back_text = $this->getDisplayOption('back_to_newswire_message');
         echo "<span class='site-navigation'>";
         echo "<A href='".$this->getFilterLink($this->site_settings['newswireobject'])."'>$back_text</A></span>";
         echo "</div>";
         $this->query->obj_id = $this->obj_id;
         $this->query->story_id=$backup_story_id;
      }
   }

   function writeArticleHeader(&$story)
   {
      $this->writeDisplayHeader();
   }


   /**********************************************
   /* Functions to write each section of the story summary
   /* These are seperated out to allow them to be more easily configurable
   /* Input: story reference
   /* Output: to screen
   /* Transforms: writes information about story depending on settings..
   //********************************************/
   function writeTranslationInfo(&$story)
   {
      // If there are any translations list them and link to them.
      $t_links_list = $this->getTranslatedLinks($story->story_id,true);
      if ($t_links_list == "") return;

      echo "<p align='right'>";
      foreach($t_links_list as $t_links) {
          echo $t_links ."\n";
      }
      echo "</p>\n";
   }

   function writeStoryTitle(&$story)
   {
      // Copy the title into this object.
      $this->story_title = $story->story_title;
      addToPageTitle($story->story_title);
      echo "<H1 class='article-title'>".$this->getItemHighlights($story).$story->story_title."</H1>\n";

   }

   function writeStoryFooter(&$story)
   {
      echo '<div class="article-footer">';
      if(!isset($_REQUEST['print_page']))
      {
         if($this->getDisplayOption('include_comments') == 'true' and !isset($_GET['include_comments']))
         {
            $this->writeAddCommentLink($story);
         }
         echo "<span class='storysummary-details'>";
         $this->writePrintBox($story);
         if($this->getDisplayOption('include_comments') == 'true' and !isset($_GET['include_comments']))
         {
            $this->writeFeedBox($story);
         }
         echo "</span>";
      }
      echo '</div>';
      if(!isset($_REQUEST['print_page']))
      {
         if($this->getDisplayOption('social_bookmark') == 'true')
         {
            $this->writeSocialBookmarks($story);
         }
      }
   }

   function writeStorySubtitle(&$story)
   {
      addToPageDescription($story->story_subtitle);
      echo "<p class='article-subtitle'>$story->story_subtitle</p>\n";
   }

   function getDisplayableEmailAddress($addy)
   {
      if(! isValidEmailAddress($addy) ) return false;
      //is valid -> substitutions
      $bits = explode('@', $addy);
      $other_bits = explode('.', $bits[1]);
      $ret_str = $bits[0]. ' at ' . array_shift($other_bits);
      foreach($other_bits as $bit)
      {
         $ret_str .= ' dot '.$bit;
      }
      return $ret_str;
   }

   function writeEndDetails(&$story)
   {
      $opts = $this->getDisplayOption('enddetails');
      if(!empty($opts) && count($opts) > 0)
      {
         echo "<div class='article-details'>\n";
         echo $this->writeStoryDetails($story, $opts);
         echo "</div>\n";
      }
   }

   function writeSubtitleDetails(&$story)
   {
      $opts = $this->getDisplayOption('subtitledetails');
      if(!empty($opts) && count($opts) > 0)
      {
         echo "<div class='article-subtitledetails'>\n";
         echo $this->writeStoryDetails($story, $opts);
         $contact_id = $this->getDisplayOption("content_reporting_contact");
         if($contact_id > 0)
         {
            $this->writeReportPostIcon($story, $contact_id);
         }
         echo "</div>\n";
      }
      else
      {
         $contact_id = $this->getDisplayOption("content_reporting_contact");
         if($contact_id > 0)
         {
            echo "<div class='report-post'>";
            $this->writeReportPostIcon($story, $contact_id);
            echo "</div>";
         }
      }

   }

   function writeStoryDetails(&$story, $opts)
   {
      global $system_config, $graphics_store;
      echo "<span class='article-details'>\n";
      foreach($opts as $opt)
      {
         if($opt == "br")
         {
            echo "<br />";
            continue;
         }
         if($opt == 'attachments')
         {
            $this->writeAttachmentDetails($story);
         }
         elseif($opt == 'comments')
         {
            $this->writeCommentDetails($story);
         }
         elseif($opt == 'author_contact')
         {
            if($story->author_email == null and $story->author_address == null and $story->author_phone == null) continue;
            $em = $this->getDisplayableEmailAddress($story->author_email);
            if($em != "")
            {
               echo "<span class='article-detail'>";
               if($system_config->use_icons_for_author_details)
               {
                  echo "<img class='icon' src='".$graphics_store."email.gif' alt='author email'> ";
               }
               else
               {
                  echo "email ";
               }
               echo "<a href=\"mailto:$em\">$em</a>";
               echo "</span>";
            }
            if($story->author_address!=null)
            {
               echo "<span class='article-detail'>";
               if($system_config->use_icons_for_author_details)
               {
                  echo "<img class='icon' src='".$graphics_store."address.gif' alt='author address'> ";
               }
               else
               {
                  echo "address ";
               }
               echo $story->author_address;
               echo "</span>";
            }
            if($story->author_phone!=null)
            {
               echo "<span class='article-detail'>";
               if($system_config->use_icons_for_author_details)
               {
                  echo "<img class='icon' src='".$graphics_store."phone.gif' alt='author phone'> ";
               }
               else
               {
                  echo "address ";
               }
               echo $story->author_phone;
               echo "</span>";            }
            echo "</span>";
         }
         elseif($opt == 'author')
         {
            echo "<span class='article-detail'>";
            if($system_config->use_icons_for_author_details)
            {
               echo "<img class='icon' src='".$graphics_store."person.gif' alt='author'> by ";
            }
            else
            {
               echo "by ";
            }
            echo $story->author_name;
            if($story->author_organisation!=null)
            {
               echo(" - ".$story->author_organisation);
            }
            echo "</span>";
         }
         elseif($opt == 'context')
         {
            echo "<span class='article-detail'>";
            if($system_config->use_icons_for_author_details)
            {
               echo "<img class='icon' src='".$graphics_store."star.gif' alt='category'> ";
            }
            $this->writeCategoryContext($story);
            echo "</span>";
         }
         elseif($opt == 'date')
         {
            echo "<span class='article-detail'>";
            $this->writePublicationDate($story);
            echo "</span>";
         }
      }
      echo "</span>\n";
   }



   function writeStoryBody(&$story, $showAttachments)
   {
      global $pseudo_directory_offset, $editor_session;
      if($showAttachments)
      {
         $images = $story->getImageAttachments();
         // if we have a story we place first image after summary
         // if we have a feature we put them all at bottom so as not to interfere with summary formatting
         if($story->type_id!=1)
         {
            if(count($images)>0)
            {
               if(strstr($story->story_summary.$story->story_content.$story->story_content_extra,$images[0]->getURL())==false)
               {
                  ?>
                  <P class='photo'>
                  <a name="attachment<?=$images[0]->attachment_id?>"></a>
                  <?
                  echo $this->getTransformedImageTag($images[0], 'summary-image');
                  $descr_str = $this->getItemHighlights($images[0], true);
                  $descr_str .= $images[0]->description;
                  if(strlen($descr_str)>0)
                  {
                     echo "<br>$descr_str";
                  }
                  ?><BR><?
		  if (count($images) <= 1 ) echo $this->getItemOptions($images[0],"attachment",false);
		  else
		  {
                      $id_array = array();
		      // We pass in the index (1 to 5) of this attachments. It will be excluded from the list.
		      // Later we exclude the element that is the given attachment.
                      for($i=0;$i < count($images);$i++) {
                          $id_array[] = $images[$i]->attachment_id;
                      }
		      echo $this->getItemOptions($images[0],"attachment",false, "'editor-attachment-option'", $id_array);
                  }
                  ?></P><?
               }
            }
         }
      }
      ?>
      <P class="article"><?=$story->story_content?> <?=$story->story_content_extra?></P>
      <?
      $this->writeEndDetails($story);
      $this->writeRelatedLink($story, 'article-related-link');
      if($showAttachments)
      {
         $this->writeAttachmentList($story,false,false);
         if($editor_session->canEditContent())
         {
            $this->writeAttachmentWarnings($story);
         }
      }
   }

   function writeStoryIntro(&$story)
   {
      addToPageDescription($story->story_summary);
      if($story->hide_summary_on_story_page==false)
      {
         ?><blockquote class="article-intro"><?=$story->story_summary?></blockquote><?
      }
   }


   function writePublicationDate(&$story)
   {
      global $system_config, $graphics_store;
      $date_format = $this->getDisplayOption('storydate_format');
      if($date_format == "") $date_format = $system_config->default_strftime_format;
      if($system_config->use_icons_for_author_details)
      {
         echo "<img class='icon' src='".$graphics_store."date.gif' alt='author'> ";
      }
      else
      {
         echo "published ";
      }
      echo strftime("$date_format",$story->time_posted+$system_config->timezone_offset);
   }

   function writeCategoryContext(&$story)
   {
      $n_query = new StoryQuery();
      $topic=$story->getTopic();
      $region=$story->getRegion();
      $type=$story->getType();
      $n_query->topic_id = $topic->topic_id;
      $topic_link = $this->getFilterLink($this->site_settings['newswireobject'], false, $n_query);
      $n_query->topic_id = null;
      $n_query->region_id = $region->region_id;
      $region_link = $this->getFilterLink($this->site_settings['newswireobject'], false, $n_query);
      $n_query->region_id = null;
      $n_query->type_id = $type->type_id;
      $type_link = $this->getFilterLink($this->site_settings['newswireobject'], false, $n_query);
      if(!isset($_REQUEST['print_page']))
      {
         ?>
         <a class=category-text href="<?=$region_link?>"><?=strtolower($region->region_name)?></a> |
         <a class=category-text href="<?=$topic_link?>"><?=strtolower($topic->topic_name)?></a> |
         <a class=category-text href="<?=$type_link?>"><?=strtolower($type->type_name)?></a>
         <?
      }
      else
      {
         ?>
         <?=strtolower($region->region_name)?> |
         <?=strtolower($topic->topic_name)?> |
         <?=strtolower($type->type_name)?>
         <?
      }
      //$this->onwards_query=$save_query;
   }


   function writeEventInfo(&$story)
   {
      global $system_config;
      if($story->type_id==$system_config->event_type_id)
      {
         $topic=$story->getTopic();
         $region=$story->getRegion();
         $type=$story->getType();
         $intro_str = "$region->region_name - $type->type_name";

         echo '<div class="article-eventinfo">';
         $edate_format = $this->getDisplayOption('event_date_format');
         if($edate_format == "") $edate_format = ' '.$system_config->default_strftime_format;
         echo "$intro_str<br /><span class='article-eventdate'>";
         echo strftime($edate_format,$story->event_time+$system_config->timezone_offset);
         if (strftime("%H%M",$story->event_time +$system_config->timezone_offset) != "0000") 
	 {
             echo '<br>Start Time: '. strftime("%I:%M %p",$story->event_time +$system_config->timezone_offset);
         }
         echo '</span></div>';
      }
   }

   function writeStory(&$story,$showAttachments,$show_options=true)
   {
      global $system_config,$editor_session;
      $story->splitFeatureContent();
      //saves for php5
      $con_e = $story->story_content_extra;
      echo "<div class='article'>\n";
      if($show_options)
      {
         $options = $this->getItemOptions($story,"story",false, 0);
         if($options != "") echo $options;
      }
      $story->correctLinkDirectoryOffset();
      if($system_config->allow_oscailt_macros)
      {
         $story->applyOscailtMacros();
      }
      $this->writeEventInfo($story);
      $this->writeStoryTitle($story);
      if($system_config->show_page_translations_box == true)
      {
         $this->writeTranslationInfo($story);
      }
      //if($showAttachments)
      $this->writeSubtitleDetails($story);
      $this->writeStorySubtitle($story);
      $this->writeStoryIntro($story);
      $this->writeStoryBody($story, $showAttachments);
      echo "</div>\n";
      if($showAttachments)
      {
         $this->writeStoryFooter($story);
      }
      $story->story_content_extra = $con_e;
   }



   function writeCommentAttachmentList(&$comment)
   {
      $image = $comment->getImageAttachments();
      $video = $comment->getVideoAttachments();
      $audio = $comment->getAudioAttachments();
      $embed_video = $comment->getEmbeddedVideoAttachments();
      $embed_audio = $comment->getEmbeddedAudioAttachments();
      $miscellaneous = $comment->getMiscellaneousAttachments();
      $this->writeAttachmentLinks($comment,false,$image,"image",0);
      if(count($video)>0 || count($audio)>0 ||count($miscellaneous)>0) echo("<P>");
      $this->writeAttachmentLinks($comment,false,$video,"video",0);
      $this->writeAttachmentLinks($comment,false,$audio,"audio",0);
      $this->writeAttachmentLinks($comment,false,$miscellaneous,"misc",0);

      // Displaying comment embedded video and audio.
      $this->writeAttachmentLinks($comment,false,$embed_video,"video",0);
      $this->writeAttachmentLinks($comment,false,$embed_audio,"audio",0);
   }

   function writeStoryWithComments(&$story)
   {
      $this->writeStory($story,true);
      if($this->getDisplayOption('include_comments') == 'true' and (!(isset($_REQUEST['print_page']) && !isset($_GET['include_comments']))))
      {
         return $this->writeComments($story,true);
      }
      else return 0;
   }

   function writeCommentTitle(&$comment, $comment_number)
   {
      # Generate a HTML table row with the comment title as a URL. Add author and time too.
      global $system_config;
      if($comment->hidden) $this->contains_hidden_items = true;
      $lnk = $this->getFilterLink();
      $extras = array("&condense_comments=false");
      if(isset($_REQUEST['comment_offset']) and cleanseNumericalQueryField($_REQUEST['comment_offset']) >= 1)
      {
         $extras[] = 'comment_offset='.cleanseNumericalQueryField($_REQUEST['comment_offset']);
      }
      $lnk = addLinkParameters($lnk, $extras);
      $lnk .= "#comment".$comment->comment_id;

      echo "<TR class='storycomments'>";
      echo "<TD class='storycomments'>&nbsp;<SMALL>".($comment_number+1)."&nbsp;</TD>\n";
      echo "<TD class='storycomments'>&nbsp;";
      echo $this->getItemHighlights($comment, true);
      echo "<A name='comment".$comment->comment_id."' href='$lnk'>";
      echo trim($comment->comment_title)."</a> &nbsp;</TD>\n";
      echo "<TD class='storycomments'>&nbsp; ".trim($comment->author_name)."&nbsp;</TD>\n";
      echo "<TD class='storycomments'>&nbsp;  <I> ";
      $date_format = $this->getDisplayOption('commentdate_format');
      if($date_format == "") $date_format = $system_config->default_strftime_format;
      echo strftime($date_format,$comment->time_posted+$system_config->timezone_offset);
      echo "</I>&nbsp;</TD></TR>\n";
   }

   function writeAddCommentLink(&$story)
   {
      global $editor_session, $graphics_store, $oscailt_page_setting;
      $site_id = $oscailt_page_setting['site_id'];
      if(!$story->locked || ($editor_session->isSessionOpen() && $editor_session->editor->allowedReadAccessTo("commentonlocked", $site_id)))
      {
         echo '<div class="addcomment">'."\n";
         $publink = $this->getSimpleLink($this->site_settings['publishobject']);
         $publink = addLinkParameters($publink, array('story_id='.$story->story_id, "section=addcomment"));
         # added rel=nofollow to tell googlebot not to follow it.
         echo '<a class="addcomments" href="'.$publink.'" rel="nofollow">'.$this->getDisplayOption('add_comment_message').'</a>';
         echo '</div>';
      }
      else
      {
         $lck_img = "<img alt='locked' src='$graphics_store"."lock.gif'>";
         $this->writeUserMessage($lck_img." ".$this->getDisplayOption('lockedmessage'));
      }
   }

   function writeCommentsHeader(&$story, &$comment_list, $page_num)
   {
      global $user_prefs, $editor_session;
      if(count($comment_list) > 0)
      {
         echo '<a name="comments"></a><div class="comments-section">';
         echo '<div class="comments-header">'."\n";

         if(!isset($_REQUEST['print_page']))
         {
            echo '<div class="comments-viewoptions">';
            $this->writeCommentViewOptions($comment_list, $page_num);
            echo '</div>';
         }

         $show_str = "";
         $showing = $this->getDisplayOption('comment_limit');
         if($showing == 0 or $page_num == 0)
         {
            $count_str = count($comment_list);
         }
         else
         {
            if($this->getDisplayOption("comment_order") == 'asc')
            {
               $count_str = count($comment_list) - (($page_num-1) * $showing);
               if((($page_num) * $showing) > count($comment_list))
               {
                  $count_str .= " - 1";
               }
               else
               {
                  $count_str .= " - ".(count($comment_list) - (($page_num) * $showing));
               }
            }
            else
            {
               $count_str = (($page_num-1) * $showing) + 1;
               if((($page_num) * $showing) > count($comment_list))
               {
                  $count_str .= " - ".count($comment_list);
               }
               else
               {
                  $count_str .= " - ".((($page_num) * $showing));
               }
            }
         }
         echo '<H3 class="comments-header">'.$this->getDisplayOption('comments_title');
         echo "<span class='comments-showing'>";
         echo " ($count_str of ".count($comment_list).")";
         if($editor_session->isSessionOpen())
         {
             echo " &nbsp; ". $story->getNumberHiddenComments() . " hidden ";
         }

         echo "</span>";
         echo "</h3>";
         if($this->getDisplayOption("condense_comments") == "true")
         {
            $show_com_nums = false;
         }
         else
         {
            $show_com_nums = true;
         }
         $limits = $this->writeCommentNavigation($comment_list, $page_num, $show_com_nums);
         echo '</div>';

      }
      else $limits = 0;
      return $limits;
   }

   function writeCommentViewOptions(&$comment_list, $page_num)
   {
      global $user_prefs;
      if(isset($_REQUEST['print_page'])) return;

      $cls_str = "class='comment-navigation'";
      //print_r($user_prefs);
      //all of our non-standard view stuff should be added to links....

      if ($this->getDisplayOption("comment_order") == "asc")
      {

         $msg = $this->getDisplayOption('oldest_first_message');
         $user_prefs->setQueryOption('comment_order', "desc");
         echo "<A $cls_str href='".$this->getFullLink()."#comments'>$msg</a><br />";
         $user_prefs->unsetQueryOptions();
      }
      else
      {
         $msg = $this->getDisplayOption('latest_first_message');
         $user_prefs->setQueryOption('comment_order', "asc");
         echo "<A $cls_str href='".$this->getFullLink()."#comments'>$msg</a><br />";
         $user_prefs->unsetQueryOptions();
      }
      if ($this->getDisplayOption("condense_comments") != 'true')
      {
         $msg = $this->getDisplayOption('titles_only_message');
         $user_prefs->setQueryOption('condense_comments', "true");
         echo "<A $cls_str href='".$this->getFullLink()."#comments'>$msg</a><br />";
         $user_prefs->unsetQueryOptions();
      }
      else
      {
         $user_prefs->setQueryOption('condense_comments', "false");
         $msg = $this->getDisplayOption('full_comments_message');
         echo "<A $cls_str href='".$this->getFullLink()."#comments'>$msg</a><br />";
         $user_prefs->unsetQueryOptions();
      }
      $this->writeSavePrefsLink();
   }

   function writeCommentNavigation(&$comment_list, $page_num, $show_comment_nums=true)
   {
      global $user_prefs;
      if(count($comment_list)<=0) return;
      if($page_num == 0)
      {
         $start = 0;
         $end = count($comment_list);
         $pages = 0;
      }
      else
      {
         $num1 = count($comment_list)-1;
         $num2 = $this->getDisplayOption('comment_limit');
         $pages = (int)($num1 / $num2) + 1;
         if($this->getDisplayOption("comment_order") == 'asc')
         {
            $end = count($comment_list) - (($page_num-1) * $this->getDisplayOption('comment_limit'));
            //last page
            if($pages == $page_num)
            {
               $start = 0;
            }
            else
            {
               $start = $end - $this->getDisplayOption('comment_limit');
            }
         }
         else
         {
            $start = (($page_num-1) * $this->getDisplayOption('comment_limit'));
            //last page
            if($pages == $page_num)
            {
               $end = count($comment_list);
            }
            else
            {
               $end = $start + $this->getDisplayOption('comment_limit');
            }
         }
      }
      if($pages > 1)
      {
         $lnkbase = $this->getFullLink();
         echo "<span class='page-navigation'>".$this->getDisplayOption('comments_page_text') ."</strong>\n";
         for($k=1; $k<=$pages; $k++)
         {
            $lnk = $lnkbase;
            if($k == $page_num) echo "<b>$k</b>  ";
            else
            {
               $offset = (($k-1) * $this->getDisplayOption('comment_limit')) + 1;
               $lnk = addLinkParameters($lnk, array("comment_offset=$offset")).'#comments';
               echo " <a class='comment-navigation' href='$lnk'> $k</a> ";
            }
         }
         echo "</span>\n";
         $user_prefs->unsetQueryOptions();
      }
      if($show_comment_nums)
      {
         if($pages > 1) echo "&nbsp; - &nbsp;";
         echo "<span class='comment-navigation'>".$this->getDisplayOption('comments_number_text') ."\n";
         if($this->getDisplayOption("comment_order") == 'asc')
         {
            for($j=$end-1;$j>=$start && $j<count($comment_list);$j--)
            {
            ?>
               <a class='comment-navigation' href="#comment<?=$comment_list[$j]->comment_id?>"><?=$j+1?></a>
            <?
            }
         }
         else
         {
            for($j=$start;$j<$end && $j<count($comment_list);$j++)
            {
            ?>
               <a class='comment-navigation' href="#comment<?=$comment_list[$j]->comment_id?>"><?=$j+1?></a>
            <?
            }
         }
         echo "</span>";
      }
      else
      {
         echo "&nbsp;";
      }
      return array($start, $end, $pages);

   }

   function getPageNum(&$comment_list)
   {
      $page_size = $this->getDisplayOption('comment_limit');
      if($page_size == 0 or $page_size > count($comment_list)) return 0;
      if(!isset($_REQUEST['comment_offset']) or $_REQUEST['comment_offset'] <= 1) return 1;
      else return ceil(($_REQUEST['comment_offset']/$page_size));
   }

   function writeCommentTable(&$comment_list, $limits)
   {
      if($limits[1] >= count($comment_list))
      {
         $limits[1] = count($comment_list);
      }
      ?><P><table width="100%" class="storycomments">
      <TR class=storycomments>
      <TH class=storycomments>&nbsp;#&nbsp;</th>
      <TH class=storycomments>&nbsp;Title&nbsp;</th>
      <TH class=storycomments>&nbsp;Author&nbsp;</th>
      <TH class=storycomments>&nbsp;Date&nbsp;</th>
      </tr>
      <?
      if($this->getDisplayOption("comment_order") == "asc")
      {
         for($j=$limits[1] - 1; $j >= $limits[0] ;$j--)
         {
            $this->writeCommentTitle($comment_list[$j], $j);
         }
      }
      else
      {
         for($j=$limits[0];$j<$limits[1];$j++)
         {
            $this->writeCommentTitle($comment_list[$j], $j);
         }
      }
      ?> <tr><td colspan=4><br></td></tr> </table><br>  <?

   }

   function writeComments(&$story,$showAttachments)
   {
      global $editor_session;
      # Added functionality to display comment summaries in a table instead.
      $comment_list = $story->getComments();
      $page_num = $this->getPageNum($comment_list);
      $limits = $this->writeCommentsHeader($story, $comment_list, $page_num);
      $written_comments = 0;
      if(count($comment_list) <= 0) return;

      if ($this->getDisplayOption("condense_comments") == "true" && !isset($_REQUEST['print_page']))
      {
         $this->writeCommentTable($comment_list, $limits);
      }
      else
      {
         // Get the list of editorial reasons for hides for hidden comments.
	 $this->hide_reasons = $this->getHideReasonsArray($comment_list);
         # Stick the 'Add comments' here, because we don't want users to add comments
         # if they haven't read the existing ones. It encourages trolls.
         if($this->getDisplayOption("comment_order") == "asc")
         {
            for($j=$limits[1] - 1; $j >= $limits[0] ;$j--)
            {
               $this->writeComment($comment_list[$j],$showAttachments, ++$written_comments, $j+1);
            }
         }
         else
         {
            for($j=$limits[0];$j<$limits[1];$j++)
            {
               $this->writeComment($comment_list[$j],$showAttachments, ++$written_comments, $j+1);
            }
         }
      }
      if(!isset($_REQUEST['print_page']))
      {
         $this->writeCommentsFooter($story, $comment_list, $page_num);
      }
      return $written_comments;
   }

   function writeComment(&$comment,$showAttachments, $comment_num=0, $comment_number_id=0, $show_options=true)
   {
      global $system_config,$editor_session, $graphics_store;
      if($comment->hidden) $this->contains_hidden_items = true;
      if($showAttachments && $show_options==true) echo $this->getItemOptions($comment,"comment",false, $comment_num);

      // Show editorial hide reason if hidden
      if ($comment->hidden == true ) {
         if ($this->hide_reasons != null && isset($this->hide_reasons[$comment->comment_id]) ) {
             $http_mode = isset($_SERVER['HTTPS']) ? "https" : "http";
             $logs_url ="<a href='".$http_mode."://".$system_config->site_url."/viewsitelog.php?log_type=action_db&filter_id=".$comment->story_id."'>logs</a>";
             $this->writeUserMessage("<b>Hide Reason: (".$logs_url.")</b> " . $this->hide_reasons[$comment->comment_id] );
         }
      }

      if($comment->hidden) { 
           echo '<div class="comment_hidden">'; 
      }
      elseif($system_config->publish_comment_delay > 0 and ($comment->time_posted > (time() - 60*$system_config->publish_comment_delay)) ) {
          // Give comments still inside the delay time a different background color. Only eds can see them.
          echo '<div class="comment_delay">';
      }
      else echo '<div class="comment">';

      echo '<div class="comment-title">';
      if($comment_number_id > 0 and $this->getDisplayOption('show_comment_numbers') == 'true')
      {
         echo "<span class='comment-number'>[$comment_number_id]</span> ";
      }
      // editors are allowed view comments on separate page to enable copying and moving of attachments
      if($editor_session->isSessionOpen())
      {
         $this->onwards_query->comment_id=$comment->comment_id;
         echo $this->getItemHighlights($comment, true);
      }
      if($editor_session->canEditContent())
      {
         echo $comment->comment_id." ";
         echo "<a class='comment-title' name='comment".$comment->comment_id."' href='".$this->getFullLink()."&comment_id=".$comment->comment_id."'>$comment->comment_title</a>";
         $this->query->comment_id=null;
      }
      else
      {
         echo "<a name='comment".$comment->comment_id."'>$comment->comment_title</a>";
      }
      echo "</div>\n";

      echo "<span class='article-details'>";
      echo "<span class='article-detail'>";
      if($system_config->use_icons_for_author_details)
      {
         echo "<img class='icon' src='".$graphics_store."person.gif' alt='author'> ";
      }
      echo "by $comment->author_name";
      if($comment->author_organisation!=null)
      {
         echo(" - ".$comment->author_organisation);
      }
      echo "</span>";
      $date_format = $this->getDisplayOption('commentdate_format');
      if($date_format == "") $date_format = "%c";
      echo "<span class='article-detail'>";
      if($system_config->use_icons_for_author_details)
      {
         echo "<img class='icon' src='".$graphics_store."date.gif' alt='publication date'> ";
      }
      echo "<EM>".strftime($date_format,$comment->time_posted+$system_config->timezone_offset);
      echo "</EM></span>";

      $em = $this->getDisplayableEmailAddress($comment->author_email);
      if($comment->author_email!=null)
      {
         echo "<span class='article-detail'>";
         if($system_config->use_icons_for_author_details)
         {
            echo "<img class='icon' src='".$graphics_store."email.gif' alt='author email'> ";
         }
         echo("<a href=\"mailto:$em\">$em</a>");
         echo "</span>";
      }
      if($comment->author_address!=null)
      {
         echo "<span class='article-detail'>";
         if($system_config->use_icons_for_author_details)
         {
            echo "<img class='icon' src='".$graphics_store."address.gif' alt='author address'> ";
         }
         else echo "address ";
         echo("$comment->author_address");
         echo "</span>";

      }
      if($comment->author_phone!=null)
      {
         echo "<span class='article-detail'>";
         if($system_config->use_icons_for_author_details)
         {
            echo "<img class='icon' src='".$graphics_store."phone.gif' alt='author phone'> ";
         }
         else echo "phone ";
         echo("$comment->author_phone");
         echo "</span>";

      }
      $contact_id = $this->getDisplayOption("content_reporting_contact");
      if($contact_id > 0)
      {
         $this->writeReportPostIcon($comment, $contact_id, true);
      }
      echo "</span>";
      echo "<P class='comment'>$comment->comment_content</P>";
      if($showAttachments) $this->writeCommentAttachmentList($comment);
      if($comment->related_link!=null)
      {
         $this->writeRelatedLink($comment, "article-related-link");
      }
      echo "</div>\n";
   }

   function writeCommentsFooter(&$story, &$comment_list, $page_num)
   {
      global $user_prefs;
      if(count($comment_list) > 5)
      {
         echo '<div class="comments-header">'."\n";
         $show_str = "";
         if(count($comment_list) > 5)
         {
            if(isset($user_prefs->non_default_preferences['comment_limit']))
            {
               $orig_limit = $user_prefs->non_default_preferences['comment_limit'];
               unset($user_prefs->non_default_preferences['comment_limit']);
            }
            $lnk = $this->getFullLink()."#comments";
            $show_str = "<form style='display: inline; margin: 0' name='pagesize' action='$lnk' method='POST'>";
            $show_str .= "<select name='comment_limit' class='comments-pagesize' onChange='document.pagesize.submit()'>";
            $i = 5;
            while($i < count($comment_list))
            {
               if($this->getDisplayOption('comment_limit') != 0 && ($this->getDisplayOption('comment_limit') > ($i-5)) && $this->getDisplayOption('comment_limit') < ($i+5))
               {
                  $show_str .= "<option value='$i' selected>$i</option>";
               }
               else
               {
                  $show_str .= "<option value='$i'>$i</option>";
               }
               $i += 5;
            }
            if($this->getDisplayOption('comment_limit') == 0)
            {
               $show_str .= "<option value='0' selected>all</option>";
            }
            else
            {
               $show_str .= "<option value='0'>all</option>";
            }
            $show_str .= "</select></form>";
            if(isset($orig_limit)) $user_prefs->non_default_preferences['comment_limit'] = $orig_limit;
         }
         if($show_str != "")
         {
            $msg = $this->getDisplayOption('set_comment_pagesize');
            echo "<div class='comments-viewoptions'>$msg $show_str</div>&nbsp;";
         }

         $this->writeCommentNavigation($comment_list, $page_num, false);
         echo '</div>';
      }
      echo "</div>";
      $this->writeCommentEnd($story);
   }

   function writeCommentEnd(&$story)
   {
      echo '<div class="article-comments-footer">';
      if(!isset($_REQUEST['print_page']))
      {
         $this->writeAddCommentLink($story);
         $this->writeHiddenItemsNotice($story->getNumberOfComments(), $story->getNumberHiddenComments() );
      }
      echo '</div>';
   }

   function writeHiddenItemsNotice($total_comments, $number_hidden)
   {
      global $graphics_store;
      if($this->contains_hidden_items and $this->getDisplayOption("hidden_comments_note_text") != '')
      {
         echo "<span class='storysummary-details'>";
         echo "<img src='".$graphics_store."report.gif' alt='hidden comments' class='icon'>";
         echo $this->getDisplayOption("hidden_comments_note_text");
	 $percentage = floor(100 * $number_hidden / $total_comments);
         echo "<strong>";
         echo " " . $number_hidden . " (" . $percentage ."%) of " . $total_comments;
         echo "</strong>";
         echo "</span>";
      }
      else
      {
         echo "&nbsp;";
      }
   }

   function getHideReasonsArray($comment_list)
   {
      global $editor_session;

      // See if any stories are hidden and if so, get the list of reasons for the hides
      $reasons_array = null;
      if(($editor_session->isSessionOpen() && $editor_session->editor->allowedReadAccessTo("viewhiddencontent")))
      {
	 $hide_cnt=0;
	 $in_list="";
         for($k=0;$k < count($comment_list);$k++)
         {
               $comment=$comment_list[$k];
	       if ($comment->hidden != true) continue;
	       $hide_cnt++;
               $in_list .= "'".$comment->comment_id ."',";
         }
	 if ($hide_cnt > 0 ) {
               $reasons_array = getEditorialReasons($in_list, false);
         }
      }
      return $reasons_array;
   }

}
?>
