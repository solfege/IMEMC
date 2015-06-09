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
/* Class that makes a content object appear like a data object for       */
/* inclusion in containers.                                             */
/************************************************************************/

require_once("objects/comment.inc");
require_once("objects/attachment.inc");
require_once("objects/story.inc");

class indyContentObject extends indyBasicObject
{
   var $display_options = array();
   var $pseu = '<?php echo $pseudo_directory_offset; ?>';

   var $container_settables = array(
      "title" => 'text',
      "hsize" => 'select',
   );

   function indyContentObject ($a, $b)
   {
      $this->indyBasicObject($a, $b);
      if($b == 'story')
      {
         $this->display_options = array("Title", "Title And Details", "Short Summary", "Short Summary And Details", "Full Summary", "Full Story");
      }
      elseif($b == 'comment')
      {
         $this->display_options = array("Title", "Title And Details", "Summary", "Full Comment");
      }
      elseif($b == 'attachment')
      {
         $this->display_options = array("Link", "Thumbnail", "Thumbnail And Title", "Full Contents");
      }
   }

   function getDefaultDisplayMode($depth)
   {
      if($this->getType() == 'story')
      {
         if($depth == FULL_DEPTH) $display = "Full Story";
         elseif($depth == SUMMARY_DEPTH)  $display = "Full Summary";
         elseif($depth > LONG_LIST_DEPTH)  $display = "Short Summary And Details";
         elseif($depth == LONG_LIST_DEPTH) $display = "Title And Details";
         else $display = "Title";
      }
      if($this->getType() == 'comment')
      {
         if($depth == FULL_DEPTH) $display = "Full Comment";
         elseif($depth == SUMMARY_DEPTH)  $display = "Summary";
         elseif($depth > LONG_LIST_DEPTH)  $display = "Title And Details";
         else $display = "Title";
      }
      if($this->getType() == 'attachment')
      {
         if($depth == FULL_DEPTH) $display = "Full Contents";
         elseif($depth == SUMMARY_DEPTH)  $display = "Thumbnail";
         else $display = 'Link';
      }
       return $display;
   }

   function getAttachmentLink($obj_id, $story_id, $comment_id, $attachment_id)
   {
      global $redirectList, $system_config;
      $atts = false;
      if($comment_id != null)
      {
         $extra = 'condense_comments=false&amp;comment_limit=0#attachment'.$attachment_id;
         $atts = true;
      }
      else
      {
         $extra = '#attachment'.$attachment_id;
      }
      if($system_config->use_friendly_urls)
      {
         $red = $redirectList->getRedirect($obj_id);
         if($red !== false)
         {
            $prim = $red->getPrimaryRedirect();
            if($prim !== false)
            {
               if($atts) $extra = '?'.$extra;
               return $this->pseu.$prim.'/'.$story_id.$extra;
            }
         }
      }
      if($atts) $extra = '&amp;'.$extra;
      return $this->page."?obj_id=$obj_id&amp;story_id=$story_id".$extra;
   }

   function getCommentLink($obj_id, $story_id, $comment_id)
   {
      global $redirectList, $system_config;
      if($system_config->use_friendly_urls)
      {
         $red = $redirectList->getRedirect($obj_id);
         if($red !== false)
         {
            $prim = $red->getPrimaryRedirect();
            if($prim !== false)
            {
               return $this->pseu.$prim.'/'.$story_id.'?condense_comments=false&amp;comment_limit=0#comment'.$comment_id;
            }
         }
      }
      return $this->page."?obj_id=$obj_id&amp;story_id=$story_id&amp;condense_comments=false&amp;comment_limit=0#comment".$comment_id;
   }

   function getStoryLink($obj_id, $story_id)
   {
      global $redirectList, $system_config;
      if($system_config->use_friendly_urls)
      {
         $red = $redirectList->getRedirect($obj_id);
         if($red !== false)
         {
            $prim = $red->getPrimaryRedirect();
            if($prim !== false)
            {
               return $this->pseu.$prim.'/'.$story_id;
            }
         }
      }
      return $this->page."?obj_id=$obj_id&amp;story_id=$story_id";
   }



   function getHTML($cls, $depth, &$content_obj, &$s, $obj_id, $site_id)
   {
      global $system_config, $regionList, $topicList, $typeList, $languageList;

      $hidden = false;
      if($content_obj->hidden > 0) $hidden = true;
      list($wrap, $display) = $this->getDisplayMode($s, $depth);
      $cls_str = ($cls == "") ? "" : "class='$cls'";

       list($h, $h_e) = $this->getHeaderTags($caller_settings, $depth, $cls_str, true);
      if($this->getType() == 'story')
      {
         $content_obj->splitFeatureContent();
         $reg = $regionList->getRegionByID($content_obj->region_id, $content_obj->language_id);
         $tpc = $topicList->getTopicByID($content_obj->topic_id, $content_obj->language_id);
         $lang = $languageList->getLanguageByID($content_obj->language_id);
         $typ = $typeList->getTypeByID($content_obj->type_id, $content_obj->language_id);
         $lnk = $this->getStoryLink($obj_id, $content_obj->story_id);
         $title = $content_obj->story_title;
         if($content_obj->type_id == $system_config->event_type_id)
         {
            $format = "%a %b %d %y";
            $extra = " <SPAN class='$cls"."eventdate'>";
            $extra .= strftime($format, $content_obj->event_time+$system_config->timezone_offset);
            $extra.= "</SPAN> ";
            $title = $extra.$title;
         }
         $title = "<a href='$lnk' $cls_str>$title</a>";
         $title_str = $h.$title.$h_e;
         $detail_str = $this->getStoryDetails($content_obj, $cls);
         $subtitle = trim($content_obj->story_subtitle);
         $summary = trim($content_obj->story_summary);
         if($subtitle != "") $short_summary_str = $subtitle;
         else $short_summary_str = substr($summary, 0,80);
         $content = trim($content_obj->story_content.$content_obj->story_content_extra);
         if($display == "Title")
         {
            $content_str = $title_str;
         }
         elseif($display == "Title And Details")
         {
            $content_str = $title_str." ".$detail_str;
         }
         elseif($display == "Short Summary")
         {
            $content_str = $title_str."<p class='$cls'>".$short_summary_str."</P>";
         }
         elseif($display == "Short Summary And Details")
         {
            $content_str = $title_str." ".$detail_str."<p class='$cls'>".$short_summary_str."</P>";
         }
         elseif($display == "Full Summary")
         {
            if($subtitle != "") $subtitle .= "<br />";
            $content_str = $title_str." ".$detail_str."<p class='$cls'>$subtitle".$summary."</P>";
         }
         else
         {
            if($subtitle != "") $subtitle .= "<br />";
            $content_str = $title_str." ".$detail_str."<p class='$cls'>$subtitle".$summary."</P>";
            $content_str .= "<P $cls>$content</P>";
         }
      }
      elseif($this->getType() == 'comment')
      {
         $comment_id = NULL;
         $parent_story = new Story();
         $parent_story->story_id = $content_obj->story_id;
         if(!$parent_story->load())
         {
            return "Error Loading Story";
         }
         if($parent_story->hidden > 0) $hidden = true;
         $reg = $regionList->getRegionByID($parent_story->region_id, $parent_story->language_id);
         $tpc = $topicList->getTopicByID($parent_story->topic_id, $parent_story->language_id);
         $lang = $languageList->getLanguageByID($parent_story->language_id);
         $typ = $typeList->getTypeByID($parent_story->type_id, $parent_story->language_id);
         $lnk = $this->getCommentLink($obj_id, $content_obj->story_id, $content_obj->comment_id);
         $title = $content_obj->comment_title;
         $title_str = "$h<a href='$lnk' $cls_str>$title</a>$h_e";
         $detail_str = $this->getCommentDetails($content_obj, $cls);
         $content = $content_obj->comment_content;
         $summary_str = strip_tags(substr($content, 0, 200));
         if($display == "Title")
         {
            $content_str = $title_str;
         }
         elseif($display == "Title And Details")
         {
            $content_str = $title_str." ".$detail_str;
         }
         elseif($display == "Summary")
         {
            $content_str =$title_str." ".$detail_str."<p class='$cls'>".$summary_str."</P>";
         }
         else
         {
            $content_str =$title_str." ".$detail_str."<p class='$cls'>".$content."</P>";
         }
      }
      elseif($this->getType() == 'attachment')
      {
         $parent_comment_id = $content_obj->comment_id;
         $title = trim($content_obj->description);
         if($parent_comment_id == null)
         {
            $parent_story_id = $content_obj->story_id;
         }
         else
         {
            $parent_comment = new Comment();
            $parent_comment->comment_id = $parent_comment_id;
            $parent_comment->load();
            if($title == "") $title = $parent_comment->comment_title;
            if($parent_comment->hidden > 0) $hidden = true;
            $parent_story = new Story();
            $parent_story_id = $parent_comment->story_id;
         }
         $parent_story = new Story();
         $parent_story->story_id = $parent_story_id;
         if(!$parent_story->load())
         {
            return "Error Loading Story";
         }
         if($title == "") $title = $parent_story->story_title;
         if($parent_story->hidden > 0) $hidden = true;
         $reg = $regionList->getRegionByID($parent_story->region_id, $parent_story->language_id);
         $tpc = $topicList->getTopicByID($parent_story->topic_id, $parent_story->language_id);
         $lang = $languageList->getLanguageByID($parent_story->language_id);
         $typ = $typeList->getTypeByID($parent_story->type_id, $parent_story->language_id);

         $lnk = $this->getAttachmentLink($obj_id, $parent_story->story_id, $parent_comment_id, $content_obj->attachment_id);
         $title_tag = "<a href='$lnk' $cls_str>$title</a>";
         $title_str = $h.$title_tag.$h_e;
         if(!$content_obj->image or $display == 'Link')
         {
            $content_str = $title_str;
         }
         elseif($display == "Thumbnail" or $display == "Thumbnail And Title")
         {
            $image_thumbnail = $system_config->newswire_bar_thumbnail;
            $image_info = $content_obj->getTransformedImageURL($image_thumbnail);
            if($image_info)
            {
               $image_URL = '<?php echo $pseudo_directory_offset; ?>'.htmlspecialchars($image_info[0]);
               $image_html = "<img class='$cls"."-thumbnail' ".$image_info[2]." src='$image_URL' alt='".htmlspecialchars($title)."' title='".htmlspecialchars($title)."' />";
               if($display == "Thumbnail") $content_str = "<a href='$lnk' border='0'>".$image_html."</a>";
               else $content_str = $image_html.$title_str;
            }
            else
            {
               return "";
            }
         }
         elseif($display == "Full Contents")
         {
            $image_URL = '<?php echo $pseudo_directory_offset; ?>'.$content_obj->getURL();
            $image_html = "<img src='$image_URL' alt='".htmlspecialchars($title)."' title='".htmlspecialchars($title)."' />";
            $content_str = $image_html.$title_str;
         }
      }

      else return "no type ".$this->getType();

      $ret_str = "";
      if($hidden)
      {
         $ret_str .= '<?php global $editor_session; if($editor_session->isSessionOpen()) { ?>';
      }
      if($reg->excluded or $tpc->excluded or $lang->excluded or $typ->excluded)
      {
         $ret_str .= '<?php global $editor_session;  if($editor_session->isSessionOpen() && $editor_session->editor->allowedReadAccessTo("viewprotectedcategories")) { ?>'."\n";
      }
      $ret_str .= $this->getHeadHTML($cls_str, $wrap);
      $ret_str .= $content_str;
      $ret_str .= $this->getFootHTML($cls_str, $wrap);
      if($reg->excluded or $tpc->excluded or $lang->excluded or $typ->excluded)
      {
         $ret_str .= '<?php } ?>'."\n";
      }
      if($hidden)
      {
         $ret_str .= '<?php } ?>'."\n";
      }
      return $ret_str;
   }

   function getStoryDetails(&$story, $cls)
   {
      global $system_config;

      $detail_str = "";

      $format = $system_config->default_strftime_format;
      $detail_str .= "<SPAN class='$cls"."date'>";
      $detail_str .= strftime($format, $story->time_posted+$system_config->timezone_offset);
      $detail_str .= "</SPAN>";

      $detail_str .= " <SPAN class='$cls"."author'>by $story->author_name";
      $detail_str .=  "</SPAN>";
      $org = trim($story->author_organisation);
      if($org != "")
      {
         $detail_str .= " <SPAN class='$cls"."organisation'>$org";
         $detail_str .=  "</SPAN>";
      }
      return $detail_str;
   }


   function getCommentDetails(&$comment, $cls)
   {
      global $system_config;

      $detail_str = "";

      $format = $system_config->default_strftime_format;
      $detail_str .= "<SPAN class='$cls"."date'>";
      $detail_str .= strftime($format, $comment->time_posted+$system_config->timezone_offset);
      $detail_str .= "</SPAN>";

      $detail_str .= " <SPAN class='$cls"."author'>by $comment->author_name";
      $detail_str .=  "</SPAN>";
      $org = trim($comment->author_organisation);
      if($org != "")
      {
         $detail_str .= " <SPAN class='$cls"."organisation'>$org";
         $detail_str .=  "</SPAN>";
      }
      return $detail_str;
   }


}
?>
