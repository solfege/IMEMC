<?
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
/* Class to represent a basic 'ad' - a picture with a title and a link
/************************************************************************/
require_once("objects/attachment.inc");

class indyPictureboxObject extends indyBasicObject
{

   var $display_options = array(
     "Picture Inline",
     "Picture",
     "With Caption",
     "With Title",
     "With Caption And Title"
   );

   function indyPictureboxObject ($a, $b)
   {
      $this->indyBasicObject($a, $b);
    }

   function getDefaultDisplayMode($depth)
   {
      return "Picture";
   }

   function getHTML($cls, $depth, &$obj_set, $caller_settings, $live=false)
   {
      global $system_config, $pseudo_directory_offset;

      list($wrap, $display) = $this->getDisplayMode($caller_settings, $depth);
      $cls_str = ($cls == "") ? "" : "class='$cls'";
      list($h, $h_e) = $this->getHeaderTags($caller_settings, $depth, $cls_str, true);

      $title = $this->getMeta('title');
      if($title != "") $title_html = $h.$title.$h_e;
      else $title_html = "";
      $rheight = $this->getMeta('resizeheight');
      $rwidth = $this->getMeta('resizewidth');
      $maxsize = $this->getMeta('maxfilesize');
      if($this->getMeta('forcesize') == 'on') $forcesize = 'true';
      else $forcesize = 'false';
      if($this->getMeta("usefeaturedphoto") == 'on')
      {
         if($display == "With Caption" or $display == "With Caption And Title")
         {
            $show_caption = "true";
         }
         else
         {
            $show_caption = "false";
         }

         //$image_thumbnail = $system_config->newswire_bar_thumbnail;
         //$featured_image = $system_config->featured_image;
         if(!$this->loadSiteObject($obj_set))
         {
            $ret .= "INTERNAL ERROR";
            return $ret;
         }
         $nw_id = $this->getMeta("newswireobject");
         if($nw_id == "")
         {
            $st = $this->site_object->getSiteSettings();
            $nw_id = $st['newswireobject'];
         }
         $ret = $this->getHeadHTML($cls_str, $wrap);
         if($display == "With Title" or $display == "With Caption And Title")
         {
            $ret .= $title_html;
         }
         $ret .= '<?php writeFeaturedPhoto("'.$nw_id.'", "'.$rheight.'", "'.$rwidth.'", "'.$maxsize.'", '.$forcesize.', '.$show_caption.'); ?>';
         $ret .= $this->getFootHTML($cls_str, $wrap);
         return $ret;
      }
      else
      {
         $caption = $this->getMeta('caption');
         $img_plainurl = $this->getMeta('image');
         if($this->getMeta("filterfile") == 'on')
         {
            $parts = pathinfo($img_plainurl);
            $file_bit = $parts['basename'];
            $path_bit = $parts['dirname']."/";
            $file_extension = strrchr($file_bit, ".");
            if($file_extension=="") $file_without_extension=$file_bit;
            else $file_without_extension = substr($file_bit, 0, strrpos ( $file_bit, "."));
            $image_URL = '<?php
            $fname = getFilteredFile("'.$path_bit.'", "'.$file_without_extension.'", true);
            echo getResizedImageURL("'.$path_bit.'$fname", '.$rheight.', '.$rwidth.', '.$forcesize.', '.$maxsize.'); '
            .'?>';
         }
         else
         {
            $image_URL = '<?php echo'." getResizedImageURL(\"$img_plainurl\", $rheight, $rwidth, $forcesize, $maxsize); ".'?>';
         }
         $image_link = $this->getMeta('linkdestination');
         if(isRelativePath($image_link) && !(isAbsoluteURL($image_link)))
         {
            $image_link = '<?php echo $pseudo_directory_offset; ?>'.$image_link;
         }
      }
      $image_html = "<img class='$cls"."-thumbnail' src='$image_URL' alt='".htmlspecialchars($caption)."' title='".htmlspecialchars($caption)."' />";
      if($image_link != "")
      {
         $image_html =   "<a href='$image_link' $cls_str>$image_html</a>";
      }
      $ret = $this->getHeadHTML($cls_str, $wrap);

      if($display != "Picture Inline")
      {
         $image_html = "<P $cls_str>$image_html</P>";
      }

      if($display == "Picture" or $display == "Picture Inline")
      {
         $ret .= $image_html;
      }
      elseif($display == "With Caption")
      {
         $caption_cls_str = "class='$cls"."-caption'";
         $ret .= $image_html."<P $caption_cls_str>$caption</P>";
      }
      elseif($display == "With Title")
      {
         $ret .= $title_html.$image_html;
      }
      elseif($display == "With Caption And Title")
      {
         $caption_cls_str = "class='$cls"."-caption'";
         $ret .= $title_html.$image_html."<P $caption_cls_str>$caption</P>";
      }
      else $ret .= "INTERNAL ERROR 4022";
      $ret .= $this->getFootHTML($cls_str, $wrap);
      return $ret;
   }

}


?>