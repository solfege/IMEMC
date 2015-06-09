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
require_once('displayobject.inc');
require_once('objects/magpie/rss_fetch.inc');
require_once('objects/contentcleansing.inc');
require_once('objects/feed_utilities.inc');

class feedimportDisplay extends displayObject
{
   var $headlines_retrieved = false;
   var $cache_images = false;
   var $style_prefix = 'storysummary';
   var $style_prefix_alt = 'storysummary-alternative';
   var $allowed_tags = array();
   var $allowed_attributes = array();
   var $max_word_length;

   function feedimportDisplay ($obj_id)
   {
      global $system_config;
      $this->displayObject($obj_id);
      if(!defined('MAGPIE_CACHE_ON')) define('MAGPIE_CACHE_ON', true);
      if(!defined('MAGPIE_CACHE_DIR')) define('MAGPIE_CACHE_DIR', $system_config->rss_cache);
      if(!defined('MAGPIE_CACHE_AGE'))
      {
         define('MAGPIE_CACHE_AGE', $system_config->rss_query_cache_expiry);
      }
      if(!defined('MAGPIE_FETCH_TIME_OUT')) define('MAGPIE_FETCH_TIME_OUT',$system_config->rss_fetch_timeout);
   }

   function populateAllowedTagList()
   {
      global $approved_tags, $public_tags, $approved_attributes, $basic_tags;
      if($this->getDisplayOption("striptags") == 'true')
      {
         $this->allowed_attributes = false;
      }
      else
      {
         $t = array_unique(array_merge($public_tags, $basic_tags));
         $this->allowed_tags = array_unique(array_merge($approved_tags, $t));
         $this->allowed_attributes = array();
         foreach(array_keys($approved_attributes) as $a)
         {
            foreach($approved_attributes[$a] as $att)
            {
               if($att != 'style' && $att != 'class')
               {
                  if(!isset($this->allowed_attributes[$a])) $this->allowed_attributes[$a] = array();
                  $this->allowed_attributes[$a][] = $att;
               }
            }
         }
      }
   }

   /************************************************************************/
   /* retrieving and processing information from the rss object
   /************************************************************************/
   function getChannelField(&$rss, $f1, $f2= false, $strip=true)
   {
      $result_str = false;
      if($f2)
      {
         if(isset($rss->channel[$f1][$f2]))
         {
            $result_str = $rss->channel[$f1][$f2];
         }
      }
      else
      {
         if(isset($rss->channel[$f1]))
         {
            $result_str = $rss->channel[$f1];
         }
      }
      if($result_str === false) return false;
      else return $this->processRSSData($result_str, $strip);
   }

   //extracts the data from the rss object...
   function getItemField(&$item, $f1, $f2=false, $strip=true)
   {
      $result_str = false;
      if($f2)
      {
         if(isset($item[$f1][$f2]))
         {
            $result_str = $item[$f1][$f2];
         }
      }
      else
      {
         if(isset($item[$f1]))
         {
            $result_str = $item[$f1];
         }
      }
      if($result_str === false) return false;
      else return $this->processRSSData($result_str, $strip);
   }

   function processRSSData($result_str, $strip_tags = true)
   {
      if($this->getDisplayOption("convertfromutf8") == 'true')
      {
         $result_str == utf8_decode($result_str);
      }
      if($strip_tags)
      {
         $result_str = strip_tags($result_str);
      }
      return $result_str;
   }


   function parseW3CDate($w3cDT)
   {
      //http://www.w3.org/TR/NOTE-datetime
      //1997-07-28T19:25:30.45+01:00
      if (strcasecmp('Z',substr($w3cDT,-1,1))==0)
      {
         $tzd='+00:00';
         $myDate=substr($w3cDT,0,-1);
      }
      else
      {
         $i=strrpos($w3cDT,'+');
         if ($i<1) $i=strrpos($w3cDT,'-');
         if (($i>15)&&(strlen($w3cDT)==($i+6)))
         {
            $tzd=substr($w3cDT,-6);
            $myDate=substr($w3cDT,0,-6);
         }
         else
         {
            $tzd='+00:00';
            $myDate=substr($w3cDT,0,10);
         }
      }
      $lns=strlen($myDate);
      $year=($lns>=4)?substr($myDate,0,4):1970;
      $month=($lns>=7)?substr($myDate,5,2):1;
      $day=($lns>=10)?substr($myDate,8,2):1;
      $hour=($lns>=13)?substr($myDate,11,2):0;
      $minute=($lns>=16)?substr($myDate,14,2):0;
      $second=($lns>=19)?substr($myDate,17,2):0;
      $sign=($tzd[0]=='+')?-1:1;
      $tzdHour=substr($tzd,1,2);
      $tzdMinute=substr($tzd,-2);
      $hour+=($tzdHour*$sign);
      $minute+=($tzdMinute*$sign);
      return gmmktime($hour,$minute,$second,$month,$day,$year);
   }

   function parseRSSDate($dateString)
   {
      global $system_config;
      // check for normal date of some format
      $timestamp=strtotime($dateString);
      if($timestamp>0) return $timestamp+$system_config->timezone_offset;
      // check for old IS08601:1998 date
      $timestamp=strtotime(str_replace("T"," ",$dateString));
      if($timestamp>0) return $timestamp+$system_config->timezone_offset;
      // check for valid W3C date
      $timestamp=$this->parseW3CDate($dateString);
      if($timestamp>0) return $timestamp+$system_config->timezone_offset;
      return false;
   }


    /************************************************************************/
    /* retrieving the header data about the feed itself
    /************************************************************************/
   function getFeedLanguage(&$rss)
   {
      $lang = $this->getChannelField($rss, 'dc', 'language');
      if($lang === false)
      {
         $lang = $this->getChannelField($rss, 'language');
      }
      return $lang;
   }

   function getFeedTitle(&$rss)
   {
      if($this->getDisplayOption("title") != "")
      {
         $txt = $this->getDisplayOption("title");
      }
      else
      {
         $txt = $this->getChannelField($rss, 'title');
      }
      return $txt;
   }

   function getFeedDate(&$rss)
   {
      global $system_config;
      $ret = "";
      $format = $this->getDisplayOption('dateformat');
      if($format == "") $format = $system_config->default_strftime_format;
      $time_str = $this->getChannelField($rss,'dc','date');
      if($time_str === false)
      {
         $time_str = $this->getChannelField($rss,'pubDate');
      }
      if($time_str !== false)
      {
         $item_time=$this->parseRSSDate($time_str);
         if($item_time !== false)
         {
            $ret =  strftime($format, $item_time);
         }
      }
      return $ret;
   }

   function getFeedDescription(&$rss)
   {
      $txt = $this->getChannelField($rss, 'description');
      if($txt === false)
      {
         $txt = $this->getChannelField($rss, 'subtitle');
      }
      return $txt;
   }

   function getFeedImage(&$rss)
   {
      $img_html = "";
      // Bugfix Dec 2007. Added isset as if rss was not valid, it was generating an error in Apache error log.
      if(isset($rss->image) and $rss->image != null)
      {
         $img_url = $this->getItemField($rss->image, 'url');
         $img_alt = $this->getItemField($rss->image, 'title');
         $img_link = $this->getItemField($rss->image, 'link');
         $img_html = $this->getImageHTML($img_url, $img_alt, $img_link);
      }
      else
      {
         $img_url = $this->getChannelField($rss, 'logo');
         if($img_url)
         {
            $img_alt = $this->getChannelField($rss, 'title');
            $img_link = $this->getChannelField($rss, 'link');
            $img_html = $this->getImageHTML($img_url, $img_alt, $img_link);
         }

      }
      return $img_html;
   }

   /************************************************************************/
   /* retrieving the data about an item in the feed
   /************************************************************************/
   function getItemLink(&$item)
   {
      $lnk = $this->getItemField($item, 'link');
      //seems to be a bug in global..
      if(strpos($lnk, "http://www.indymedia.org/or/") !== false)
      {
         $lnk = $this->getItemField($item, "dc", "source");
      }
      return $lnk;
   }


   function getItemDate(&$item)
   {
      global $system_config;
      $ret = "";
      $format = $this->getDisplayOption('dateformat');
      if($format == "") $format = $system_config->default_strftime_format;
      $time_str = $this->getItemField($item,'dc','date');
      if($time_str === false)
      {
         $time_str = $this->getItemField($item,'pubdate');
      }
      if($time_str !== false)
      {
         $item_time=$this->parseRSSDate($time_str);
         if($item_time !== false)
         {
            $ret = strftime($format, $item_time);
         }
      }
      return $ret;
   }

   function getItemAuthor(&$item, $cls_str)
   {
      $author_str = $this->getItemField($item,'dc', 'creator');
      if($author_str === false)
      {
         $author_str = $this->getItemField($item, "author_name");
         $creator_link = $this->getItemField($item, "author_uri");
      }
      if($author_str === false)
      {
         $author_str = $this->getItemField($item, 'author');
      }
      if($author_str !== false)
      {
         $creator_link = $this->getItemField($item, 'dc', 'publisher');
         if($creator_link === false)
         {
            $creator_link = $this->getItemField($item, 'source');
         }
         if($creator_link !== false)
         {
            $author_str = "<A $cls_str href='$creator_link'>$author_str</A>";
         }
      }
      return $author_str;
   }

   function getItemLanguage(&$item)
   {
      $lang = $this->getItemField($item, 'dc', 'language');
      if($lang === false)
      {
         $lang = $this->getItemField($item, 'language');//should not exist
      }

      return $lang;
   }

   function getItemDescription(&$item, $strip = true)
   {
      $str = $this->getItemField($item, 'description', false, $strip);
      if($str === false)
      {
         $str = $this->getItemField($item, 'summary', false, $strip);
      }
      return $str;
   }

   function getExtraContent(&$item, $strip=true)
   {
      $extra = trim($this->getItemField($item,'atom_content', false, $strip));
      if(!$extra)
      {
         $extra = trim($this->getItemField($item,'content', 'encoded', $strip));
      }
      return $extra;
   }

   function cleanItemContent($desc, $stripped=true, $base_lnk = "")
   {
      $patterns = array();
      $replacements = array();
      if($this->getDisplayOption("striptags") == 'true')
      {
         $desc = nl2br($desc);
      }
      else
      {
         //bug in indymedia.org code
         $desc = str_replace("/icon/icon/", "/icon/", $desc);
      }
      //need to shorten anything that is too long (except if it is inside a tag)
      $patterns[] = '/(?!<.*?)([^\s<>]{'.$this->max_word_length.',})(?![^<>]*?>)/ie';
      $replacements[] = "'<span class=\"shortened\" title=\"\\1 \">'.substr('\\1',0,".($this->max_word_length-3).").'...</span> '";
      $desc = preg_replace($patterns, $replacements, $desc);
      if(!$stripped)
      {
         $desc = stripTagList($desc, $this->allowed_tags, $this->allowed_attributes);
         $desc = $this->localiseImportedLinks($desc, $base_lnk);
         //$desc = preg_replace("@<img@sie", "'<IMG class='.getImageClass().' '", $desc);
         //if($this->allowed_attributes != false) $desc = stripEvilAttributes($desc);
      }
      return $desc;
   }

   function localiseImportedLinks($desc, $base_lnk)
   {
      $urlinfo=parse_url($base_lnk);
      //determine if file is local or url, and generate transformed file name
      if(!isset($urlinfo["scheme"]) or !isset($urlinfo["host"]))
      {
         return $desc;
      }
      $base_site = $urlinfo["scheme"]."://".$urlinfo["host"];
      $reg_ex_img = '@(<img[^>]*?)\s+src\s*=\s*[\'"]([^\'"]+)[\'"]([^>]*?>)@si';
      $matches = array();
      $replacements = array();
      preg_match_all($reg_ex_img, $desc, $matches);
      //set up array of tag -> tag string..
      if($matches and is_array($matches) and count($matches[0]) > 0)
      {
         for($i = 0; $i<count($matches[0]); $i++)
         {
            $remoteinfo=parse_url($matches[2][$i]);
            if(!isset($remoteinfo["scheme"]) or !isset($remoteinfo["host"]))
            {
               if(!isset($replacements[$matches[0][$i]]))
               {
                  if(isAbsolutePath($matches[2][$i]))
                  {
                     //echo($matches[2][$i] ." detected as absolute path");
                     $replacements[$matches[0][$i]] = $matches[1][$i]." SRC='".$base_site.$matches[2][$i]."' ".$matches[3][$i];
                  }
                  else
                  {
                     echo($matches[2][$i] ." detected as relative path");
                     $base_bits = explode("/", $urlinfo['path']);
                     array_pop($base_bits);
                     $j = 0;
                     while(substr($matches[2][$i], $j, 3) == "../" and count($base_bits) > 0)
                     {
                        array_pop($base_bits);
                        $j += 3;
                     }
                     $replacements[$matches[0][$i]] = $matches[1][$i]." SRC='".$base_site.implode("/", $base_bits)."/".substr($matches[2][$i], $j)."' ".$matches[3][$i];
                     //echo "<P>mapping ".htmlspecialchars($matches[0][$i])." to ".htmlspecialchars($replacements[$matches[0][$i]])."</p>";
                  }
               }
            }
         }
      }
      $reg_ex_img = '@(<a[^>]*?)\s+href\s*=\s*[\'"]([^\'"]+)[\'"]([^>]*?>)@si';
      $matches = array();
      preg_match_all($reg_ex_img, $desc, $matches);
      //set up array of tag -> tag string..
      if($matches and is_array($matches) and count($matches[0]) > 0)
      {
         for($i = 0; $i<count($matches[0]); $i++)
         {
            $remoteinfo=parse_url($matches[2][$i]);
            if(!isset($remoteinfo["scheme"]) or !isset($remoteinfo["host"]))
            {
               if(!isset($replacements[$matches[0][$i]]))
               {
                  if(strcasecmp('mailto:', substr($matches[2][$i], 0, 7)) == 0) continue;
                  if(isAbsolutePath($matches[2][$i]))
                  {
                     $replacements[$matches[0][$i]] = $matches[1][$i]." HREF='".$base_site.$matches[2][$i]."' ".$matches[3][$i];
                     //echo "<P>mapping ".htmlspecialchars($matches[0][$i])." to ".htmlspecialchars($replacements[$matches[0][$i]])."</p>";

                  }
                  else
                  {
                     $base_bits = explode("/", $urlinfo['path']);
                     array_pop($base_bits);
                     $j = 0;
                     while(substr($matches[2][$i], $j, 3) == "../" and count($base_bits) > 0)
                     {
                        array_pop($base_bits);
                        $j += 3;
                     }
                     $replacements[$matches[0][$i]] = $matches[1][$i]." HREF='".$base_site.implode("/", $base_bits)."/".substr($matches[2][$i], $j)."' ".$matches[3][$i];
                     //echo "<P>mapping ".htmlspecialchars($matches[0][$i])." to ".htmlspecialchars($replacements[$matches[0][$i]])."</p>";
                  }
               }
            }
         }
      }
      //op_r($replacements);
      if(count($replacements) > 0)
      {
         $desc = str_replace(array_keys($replacements), array_values($replacements), $desc);
      }
      return $desc;
   }

   function isInLanguageFilter(&$item)
   {
      $code = $this->getDisplayOption("language_codes");
      $all_codes = explode(",", trim($code));
      if(count($all_codes) > 0)
      {
         foreach(array_keys($all_codes) as $k)
         {
            $all_codes[$k] = strtolower(trim(substr($all_codes[$k], 0, 2)));
         }
         $lang = strtolower(trim(substr($this->getItemLanguage($item), 0, 2)));
         if(!$lang)
         {
            if(in_array('ot', $all_codes)) return true;
         }
         elseif(in_array($lang, $all_codes)) return true;
         else return false;
      }
      return true;
   }

   function getRepublishOptions(&$item)
   {
      global $editor_session, $graphics_store;
      if(($editor_session->isSessionOpen() && $this->getDisplayOption("show_republish") == 'true') or ($this->getDisplayOption("show_republish") == 'true' and $this->getDisplayOption("allowuser_republish") == 'true'))
      {
         $lnk = htmlspecialchars($this->getSimpleLink($this->site_settings['publishobject']));
         $rel_link = $this->getItemLink($item);
         $author = $this->getItemField($item,'dc', 'creator');
         if($author === false)
         {
            $author = $this->getItemField($item, 'author');
         }
         $descr = $this->getItemField($item, 'description', false, false);
         if($this->getDisplayOption("republish_full"))
         {
            $extra = $this->getExtraContent($item, false);
            if($extra) $descr .= $extra;
         }
         //$descr = $descr;//$this->cleanItemContent($descr);
         $descr = $this->localiseImportedLinks($descr, $this->getItemLink($item));
         $title = $this->getItemField($item, 'title');
         $summary = htmlspecialchars($this->getDisplayOption("republish_summary_prefill"), ENT_QUOTES);
         $promote_text = htmlspecialchars($this->getDisplayOption("promote_text"), ENT_QUOTES);
         echo "<form method='post' style='display: inline; margin: 0;' action='$lnk'>";
         echo "<input type=hidden name='story_summary' value='$summary'>";
         echo "<input type=hidden name='story_content_html' value='on'>";

         if($rel_link !== false)
         {
            $rel_link = htmlspecialchars($rel_link, ENT_QUOTES);
             echo "<input type=hidden name='related_link' value='$rel_link'>";
         }
         if($title !== false)
         {
            $title = htmlspecialchars($title, ENT_QUOTES);
            echo "<input type=hidden name='story_title' value='$title'>";
         }
         if($descr !== false)
         {
            $descr = htmlspecialchars($descr, ENT_QUOTES);
            echo "<input type=hidden name='story_content' value='$descr'>";
         }
         echo "<input title='$promote_text' type=image src='$graphics_store"."upgrade.gif' name='republish' value='Promote'>";
         echo "</form>";
      }
   }


   function getImageHTML($src, $alt, $lnk)
   {
      global $page_style, $system_config, $pseudo_directory_offset;
      if($this->cache_images)
      {
         $tool = new ImageTool();
         $src_info = $tool->getTransformedImageURL($src, $system_config->rss_bar_thumbnail);
         if($src_info == null) return false;
         $src = $pseudo_directory_offset.$src_info[0];
      }
      if($alt=== false) $alt = "no title";
      $extra_attr = "";
      if($this->getDisplayOption("max_img_width") > 0)
      {
         $extra_attr = 'width="'.$this->getDisplayOption("max_img_width").'"';
      }
      $img_html = "<IMG class='$page_style"."-thumbnail' $extra_attr src='$src' alt='$alt'>";
      if($lnk !== false)
      {
         $img_html = "<A HREF='$lnk' class='$page_style'>$img_html</A>";
      }
      return $img_html;
   }


   /************************************************************************/
   /* Functions for writing the main page
   /************************************************************************/
   function processRequest()
   {
      global $system_config,$editor_session, $performance_test;
      parent::processRequest();
      $this->populateAllowedTagList();
      $this->max_word_length = $this->getDisplayOption('max_word');
      if($this->max_word_length < 10) $this->max_word_length = 60;
      $this->display_options['maxhlength'] = 0;
      $url = $this->getDisplayOption("url");

      if($performance_test > 3) markTime("rss feed about to be fetched");
      $rss = $this->handleRssFetch($url);
      if($performance_test > 3) markTime("rss feed fetched");

      // Bugfix Dec 2007. Added check for false as fetch_rss can return this. It was generating an error in Apache error log.
      if($rss == false)
      {
         $this->writeEmptyResultMessage();
      }
      elseif(count($rss->items) == 0)
      {
         $this->writeFeedHeaderInfo($rss);
         $this->writeEmptyResultMessage();
      }
      else
      {
         $this->writeFeedHeaderInfo($rss);
         if($this->getDisplayOption('pagelimit') <= 0)
         {
            $limit = 100;
         }
         else
         {
            $limit = $this->getDisplayOption('pagelimit');
         }
         $i = 0;
         $j = 0;
         while($i < count($rss->items) && $j < $limit)
         {
            $item =& $rss->items[$i];
            if($this->writeHeadlineSummary($item, $j+1))
            {
               if($performance_test > 7) markTime("Wrote RSS Item $i ($j written)");
               $j++;
            }
            $i++;
         }
      }
      $this->writePageFooterInfo($rss);
      if($performance_test > 4) markTime("Finished Processing RSS page");
   }

   function writePageFooterInfo(&$rss)
   {
      $title = $this->getFeedTitle($rss);
      $lnk = $this->getChannelField($rss, 'link');
      if($lnk && $title)
      {
         echo "<p><a href='$lnk'>$title >></a></p>";
      }
   }

   function writeFeedHeaderInfo(&$rss)
   {
      $title = ($this->getDisplayOption('includetitle') == 'true') ? $this->getFeedTitle($rss) : false;
      $lang = ($this->getDisplayOption('includelanguage') == 'true') ? $this->getFeedLanguage($rss) : false;
      $img_tag = ($this->getDisplayOption('includephoto') == 'true') ? $this->getFeedImage($rss) : false;
      $desc = ($this->getDisplayOption('includedescription') == 'true') ? $this->getFeedDescription($rss): false;
      $dte = ($this->getDisplayOption('includedate') == true) ? $this->getFeedDate($rss) : false;
      if($title or $lang or $img_tag or $desc or $dte)
      {
         echo "<div class='site-navigation'>";
         if($img_tag) echo "<div class='right'>$img_tag</div>";
         if($title) echo  "<span class='navigation-element'><b>$title</b></span>";
         if($lang) echo " <span class='navigation-element'>($lang)</span>";
         if($title or $lang) echo "<br />";
         if($dte) echo " <span class='navigation-element'>$dte</span>";
         if($desc) echo  " <span class='navigation-element'>$desc</span>";
         echo "</div>";
      }
   }

   function writeHeadlineSummary(&$item, $item_number=1)
   {
      if($this->getDisplayOption("language_filter") == 'true')
      {
         if(!$this->isInLanguageFilter($item)) return false;
      }
      if(($item_number % 2) == 1)
      {
         $style = $this->style_prefix;
      }
      else
      {
         $style = $this->style_prefix_alt;
      }

      echo "<div class='$style'>\n";
      $this->writeItemTitle($item, $style);
      $this->writeItemSubtitle($item, $style);
      $this->writeItemContent($item, $style);
      $this->writeItemFooter($story, $style);
      echo "</div>\n";
      return true;
   }


   function writeItemTitle(&$item, $style)
   {
      global $graphics_store;
      $title = $this->getItemField($item, 'title');
      if($title !== false)
      {
         echo "<div class='$style"."-title'>";
         $lnk = $this->getItemLink($item);
         echo $this->getRepublishOptions($item);
         if ($this->getDisplayOption('articleincludeicons') == 'true')
         {
            echo("<IMG class='icon' alt='offsite link' src='$graphics_store"."importedicon.gif' border=0> ");
         }
         if($lnk !== false)
         {
            echo "<A class='$style"."-title' href='$lnk'>$title</A> ";
         }
         else echo "<B>$title</B> ";
         echo "</div>";
      }
   }

   function writeItemSubtitle(&$item, $style)
   {

      $dte = ($this->getDisplayOption('articleincludedate') == 'true') ? $this->getItemDate($item) : false;
      $author = ($this->getDisplayOption('articleincludeauthor') == 'true') ? $this->getItemAuthor($item, "class='$this->style_prefix'") : false;
      $lang = ($this->getDisplayOption('articleincludelanguage') == 'true') ? $this->getItemLanguage($item) : false;
      if($dte or $author or $lang)
      {
         echo "<div class='$style"."-subtitle'>";
         echo "<span class='$style"."-details'>\n";
         if($author) echo "<span class='$style"."-detail'>$author</span>";
         if($author and $dte) echo " - ";
         if($dte) echo "<span class='$style"."-detail'>$dte</span>";
         if($lang) echo " <span class='$style"."-detail'>($lang)</span>";
         echo "</span>";
         echo "</div>";
      }
   }


   function writeItemContent(&$item, $style)
   {
      if($this->getDisplayOption('articleincludedescription') != 'true')
      {
         return;
      }
      if($this->getDisplayOption("striptags") == 'true')
      {
         $strip = true;
      }
      else
      {
         $strip = false;
      }

      $desc = trim($this->getItemDescription($item, $strip));
      if($this->getDisplayOption('articleincludecontent') == 'true')
      {
         $extra = $this->getExtraContent($item, $strip);
         if($extra)
         {
            $desc .= $extra;
         }
      }
      if($desc)
      {
         $desc = $this->cleanItemContent($desc, $strip, $this->getItemLink($item));
         echo "<div class='$style"."-content'>$desc";
         echo "</div>";
      }
   }

   function writeItemFooter(&$item, $style)
   {
      echo "<div class='$style"."-footer'>\n";
      echo "</div>";
   }

   function handleRssFetch($url)
   {
      global $encode_charset;

      if($this->getDisplayOption("pre_parse_utf8") == 'true') {
          $pre_parse_utf8 = true;
          $encode_charset = "utf8";
      } else {
          $pre_parse_utf8 = false;
      }

      $tFeedStatusObj = checkFeedStatus($this->obj_id, $this->site_settings['siteobject']);

      if ($tFeedStatusObj->retries <= 3 || $tFeedStatusObj->feed_status != "Okay" || $tFeedStatusObj->feed_status == "Okay") {
          $rssObj = new FetchRSS();
          $rss = $rssObj->fetch_rss( $url, $pre_parse_utf8 );
          $tFeedStatusObj->feed_url = $url;
          $prev_status = $tFeedStatusObj->feed_status;
          $tFeedStatusObj->feed_status = $rssObj->status;
          $tFeedStatusObj->last_error = $rssObj->fetch_error;
	  // The $rss is of type MagpieRSS so we access the data directly
	  if ($rssObj->status == "Okay") {
              // So if state changed, then can reset retry counter
              if ($prev_status != "Okay") $tFeedStatusObj->retries = 0;
              $tFeedStatusObj->feed_type = $rss->feed_type. " ". $rss->feed_version;
          }
          $tFeedStatusObj->last_retry = time();
          $tFeedStatusObj->retries++;
      } else {
          // All info will be in tFeedStatusObj
          $tFeedStatusObj->retries++;
          $rss = false;
      }

      if ($rss == false) $this->writeFetchStatusInfo($FeedStatusObj);

      // if (($tFeedStatusObj->retries <= 4 && $tFeedStatusObj->feed_status == "Okay") || ($tFeedStatusObj->feed_status != "Okay"))
      if ($tFeedStatusObj->retries <= 4)
      {
          updateFeedStatus($tFeedStatusObj);
      }

      return $rss; 
   } 

    /************************************************************************/
    /* Functions for writing the column
    /************************************************************************/
   function retrieveHeadlines()
   {
      global $system_config, $performance_test;
      $this->setDisplayOptions();
      $this->populateAllowedTagList();
      $this->max_word_length = $this->getDisplayOption('max_word');
      if($this->max_word_length < 10) $this->max_word_length = 30;
      $url = $this->getDisplayOption("url");

      if($performance_test > 3) markTime("rss feed about to be fetched");
      $rss = $this->handleRssFetch($url);
      if($performance_test > 3) markTime("rss feed fetched");

      // Bugfix Dec 2007. Added check for false as fetch_rss can return this. It was generating an error in Apache error log.
      if($rss == false)
      {
	 if ($this->getDisplayOption('emptyresultmsg') != "") $this->writeEmptyResultMessage();
      }
      elseif(count($rss->items) == 0)
      {
         $this->writeColumnFeedHeader($rss);
         $this->writeEmptyResultMessage();
      }
      else
      {
         $this->writeColumnFeedHeader($rss);
         if($this->getDisplayOption('pagelimit') <= 0)
         {
            $limit = 100;
         }
         else
         {
            $limit = $this->getDisplayOption('pagelimit');
         }
         $i = 0;
         $j = 0;
         if($performance_test > 6) markTime("writing headlines");
         while($i < count($rss->items) && $j < $limit)
         {
            $item =& $rss->items[$i];
            //if($j == 6){ $j++; continue; }
            if($this->writeColumnHeadline($item, $j+1))
            {
               $j++;
               if($performance_test > 7) markTime("headline $j written");
            }
            $i++;
         }
      }

      if($performance_test > 6) markTime("headlines written");
      $this->writeColumnFooter($rss);
   }

   function writeColumnHeadline(&$item, $item_number)
   {
      global $system_config, $page_style, $graphics_store;
      if(($item_number % 2) == 1)
      {
         $style = $page_style;
      }
      else
      {
         $style= $page_style."-alternative";
      }

      //if(isset($item['dc']['language']) && $item['dc']['language']=="el") return false;
      //if(isset($item['dc']['creator']) && $item['dc']['creator']=="Thessaloniki") return false;
      //if(isset($item['dc']['creator']) && $item['dc']['creator']=="Athens") return false;
      if($this->getDisplayOption("language_filter") == 'true')
      {
         if(!$this->isInLanguageFilter($item)) return false;
      }
      $title = $this->getItemField($item, 'title');
      $lnk = $this->getItemLink($item);
      if(!$lnk or !$title) return false;
      if($this->getDisplayOption('maxhlength') > 0 && (strlen($title) > $this->getDisplayOption('maxhlength')))
      {
         $title = substr($title, 0, $this->getDisplayOption('maxhlength')) . "...";
      }
      echo "<p class='$style'>\n";
      echo $this->getRepublishOptions($item);
      if ($this->getDisplayOption('articleincludeicons') == 'true')
      {
         echo("<IMG class='icon' alt='offsite link' src='$graphics_store"."importedicon.gif' border=0> ");
      }
      echo "<A class='$style' href='$lnk'>$title</A> ";
      $this->writeColumnItemDetails($item, $style, $lnk);
      echo "</P>";
      return true;

   }

   function writeColumnItemDetails(&$item, $style, $base_lnk)
   {
      $dte = ($this->getDisplayOption('articleincludedate') == 'true') ? $this->getItemDate($item) : false;
      $author = ($this->getDisplayOption('articleincludeauthor') == 'true') ? $this->getItemAuthor($item, "class='$style'") : false;
      $lang = ($this->getDisplayOption('articleincludelanguage') == 'true') ? $this->getItemLanguage($item) : false;
      if($this->getDisplayOption("striptags") == 'true')
      {
         $stripped = true;
      }
      else
      {
         $stripped = false;
      }
      $desc = ($this->getDisplayOption('articleincludedescription') == 'true') ? trim($this->getItemDescription($item, $stripped)) : false;
      if($dte or $author or $lang)
      {
         if($dte) echo " <span class='$style"."date'>$dte</span>";
         if($dte and $author) echo " | ";
         if($author) echo "<span class='$style"."date'>$author</span>";
         if(($dte or $author) and $lang) echo " | ";
         if($lang) echo "<span class='$style"."-language'>$lang</span>";
         if($desc)
         {
            $desc = $this->cleanItemContent($desc, $stripped, $base_lnk);
            echo "<br />$desc";
         }
      }

   }

   function writeFetchStatusInfo($FeedStatusObj)
   {
      global $system_config;
      echo "<div class='site-navigation'>";
      echo "<div class='left'>".$FeedStatusObj->feed_status . " for  <small>". $FeedStatusObj->feed_url . ".</small></div>";
      echo "<div class='left'>Last Retry ".strftime($system_config->default_strftime_format, $FeedStatusObj->last_retry);
      echo "</div>";
      echo "</div>";
   }

   function writeColumnFeedHeader(&$rss)
   {
      global $page_style;
      if($this->getDisplayOption('includetitle') != 'true') return;

      $title = $this->getFeedTitle($rss);
      $title = "<a class='$page_style' href='".$this->getSimpleLink()."'>$title</a>";
      $lang = ($this->getDisplayOption('includelanguage') == 'true') ? $this->getFeedLanguage($rss) : false;
      $img_tag = ($this->getDisplayOption('includephoto') == 'true') ? $this->getFeedImage($rss) : false;
      $desc = ($this->getDisplayOption('includedescription') == 'true') ? $this->getFeedDescription($rss): false;
      $dte = ($this->getDisplayOption('includedate') == 'true') ? $this->getFeedDate($rss) : false;
      if($title or $lang or $img_tag or $desc or $dte)
      {
         if($title)
         {
            $hsize = $this->getDisplayOption('titlesize');
            if($hsize == 0) $hsize = 4;
            echo "<h".$hsize." class='$page_style'>$title</h".$hsize.">";
         }
         if($lang or $dte)
         {
            echo "<P class='$page_style"."-context'>";
            if($lang) echo " <span class='$page_style"."-context'>$lang</span>";
            if($lang != "" and $dte != "") echo " | ";
            if($dte) echo " <span class='$page_style"."-context'>$dte</span>";
         }
         if($img_tag)
         {
            echo "</P><DIV class='$page_style"."-thumbnail'>";
            echo $img_tag;
            echo "</DIV><P class='$page_style"."-context'>";
         }
         if($desc) echo " <span class='$page_style"."-context'>$desc</span>";
         echo "</P>";
      }
   }

   function writeColumnFooter(&$rss)
   {
      global $page_style;
      $title = $this->getFeedTitle($rss);
      $lnk = $this->getChannelField($rss, 'link');
      if($lnk && $title)
      {
         echo "<p><small><a class='$page_style' href='$lnk'>$title >></a></small></P>";
      }
   }

}

function getImageClass()
{
   static $i = 0;
   if($i == 0)
   {
      $i = 1;
      return "rightsummarypic";
   }
   else
   {
      $i = 0;
      return "leftsummarypic";
   }
}
?> 
