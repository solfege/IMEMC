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
/* A set of functions for doing various levels of html scrubbing        */
/************************************************************************/
require_once("config/markupconfig.php");

function canUseHTML()
{
   global $editor_session;
   if($editor_session->isSessionOpen() and ($editor_session->editor->allowedWriteAccessTo("usehtml") or $editor_session->editor->allowedWriteAccessTo("usefreehtml") or $editor_session->editor->allowedWriteAccessTo("usecode"))) return true;
   return false;
}

function canUseFreeHTML()
{
   global $editor_session;
   if($editor_session->isSessionOpen() and ($editor_session->editor->allowedWriteAccessTo("usefreehtml") or $editor_session->editor->allowedWriteAccessTo("usecode"))) return true;
   return false;
}

function canUseCode()
{
   global $editor_session;
   if($editor_session->isSessionOpen() and $editor_session->editor->allowedWriteAccessTo("usecode")) return true;
   return false;
}


function findContentProblems($content, $use_html, $to_html=true)
{
   global $editor_session, $system_config, $evil_tags, $basic_tags, $approved_tags, $public_tags;
   $problems = array();
   //check for evil stuff first, just so that we can log it before stripping it.
   if(!$system_config->force_correct_html_input)
   {
      return $problems;
   }
   if(!canUseCode())
   {
      if(containsEvilTags($content))
      {
         $problems[] = "Evil HTML tags or attributes were present in your input. Oscailt feels that this may have been an attempt to penetrate its security and has logged the incident to its security log for investigation.  Please do not attempt to use embedded code or scripts in your contributions";
      }
   }
   //white list checks from severe to mild
   $allowed_tags = array();
   $unclosed_tags = array();
   if(!$use_html or !$editor_session->isSessionOpen())
   {
      if($to_html) $allowed_tags = $public_tags;
      if($content != stripTagList($content, $allowed_tags))
      {
         if($editor_session->isSessionOpen() && count($allowed_tags) > 0)
         {
            $msg= "Forbidden HTML Tags were present in your input. If you want to publish HTML you must check the HTML box above the field. If you do not check the box, the only tags that you are allowed to use are: ".htmlspecialchars('<'.implode(">, <",$allowed_tags).'>', ENT_QUOTES).".";
            $msg .= " Oscailt has removed all other tags from the text.  Check it to make sure it is okay and submit again.";
         }
         elseif(count($allowed_tags) == 0)
         {
            $msg = "HTML Tags were present in your input. You are not allowed use HTML in this field.";
         }
         else
         {
            $msg = "Forbidden HTML Tags were present. The only tags that you are allowed to use are: ".htmlspecialchars('<'.implode(">, <",$allowed_tags).'>', ENT_QUOTES).".";
            $msg .= " Oscailt has removed all other tags from the text.  Check it to make sure it is okay and submit again.";
         }

         $problems[] = $msg;
         return $problems;
      }
   }
   elseif(!canUseHTML())
   {
      $allowed_tags = array_unique(array_merge($basic_tags, $public_tags));
      if($content != stripTagList($content, $allowed_tags))
      {
         $problems[] = "Forbidden HTML Tags were present. The only tags that you are allowed to use are: ".htmlspecialchars('<'.implode(">, <",$allowed_tags).'>', ENT_QUOTES)."."." Oscailt has removed all other tags from the text.  Check it to make sure it is okay and submit again.";
;      }
   }
   elseif(!canUseFreeHTML())
   {
      $t_tags = array_unique(array_merge($basic_tags, $public_tags));
      $allowed_tags = array_unique(array_merge($t_tags, $approved_tags));
      if($content != stripTagList($content, $allowed_tags))
      {
         $problems[] = "Forbidden HTML Tags were present. The only tags that you are allowed to use are: ".htmlspecialchars('<'.implode(">, <",$allowed_tags).'>', ENT_QUOTES)."."." Oscailt has removed all other tags from the text.  Check it to make sure it is okay and submit again.";

      }
   }

   //if we already have problems don't bother with closing tags etc.
   if(count($problems) > 0) return $problems;

   $unclosed_tags = listUnclosedTags($content, $allowed_tags);
   if(count($unclosed_tags) > 0)
   {
      $problems[] = "Your HTML is incorrectly formatted.  You need to close all your tags properly so that your tags do not affect the rest of the page.  Please correct the following errors: ".htmlspecialchars(implode(",",$unclosed_tags));
   }
   if($system_config->allow_bb_code)
   {
      $messed_up_bb_tags = listMessedUpBBCode($content);
      if(count($messed_up_bb_tags) > 0)
      {
         $problems[] = "Your BB Code is incorrectly formatted. Please correct the following errors: ".htmlspecialchars(implode(",",$messed_up_bb_tags));
      }
   }
   return $problems;
}

function containsForbiddenTags($content)
{
   global $system_config, $public_tags;
   $save_bb_setting = $system_config->allow_bb_code;
   $save_public_tags = $public_tags;
   $public_tags = array('br');
   $system_config->allow_bb_code = false;
   if($content == unprocessRichContentField($content, true))
   {
      $res = false;
   }
   else
   {
      $res = true;
   }
   $system_config->allow_bb_code = $save_bb_setting;
   $public_tags = $save_public_tags;
   return $res;
}

//five different levels of stripping
function processRichContentField($unclean, $use_html, $to_html=true)
{
   global $editor_session, $system_config, $evil_tags, $basic_tags, $approved_tags, $basic_attributes, $approved_attributes, $public_tags, $public_attributes;
   if(!$to_html)
   {
      $allowed_tags = array();
      $allowed_attrs = array();
   }
   elseif(!$use_html or !$editor_session->isSessionOpen())
   {
      $allowed_tags = $public_tags;
      $allowed_attrs = $public_attributes;
   }
   elseif(!canUseHTML())
   {
      $allowed_tags = array_unique(array_merge($basic_tags, $public_tags));
      $allowed_attrs = $basic_attributes;
   }
   elseif(!canUseFreeHTML())
   {
      $allowed_attrs = $approved_attributes;
      $t_tags = array_unique(array_merge($basic_tags, $public_tags));
      $allowed_tags = array_unique(array_merge($t_tags, $approved_tags));
   }
   elseif(!canUseCode())
   {
      $clean = stripEvilTags($unclean);
      $clean = stripEvilAttributes($clean);
      $clean = compactWhiteSpace($clean);
   }
   else
   {
      $clean = compactWhiteSpace($unclean);
   }
   if(!$use_html or !canUseFreeHTML())
   {
      $clean = stripTagList($unclean, $allowed_tags, $allowed_attrs);
      if($to_html)
      {
         $clean = compactWhiteSpace($clean);
      }
      if($system_config->allow_bb_code && $to_html)
      {
         $clean = processBBCode($clean);
      }
      if(!in_array("br", $allowed_tags) && $to_html)
      {
         $clean = nl2br($clean);
      }
      if(!in_array("a", $allowed_tags) && $to_html)
      {
         $clean = get_shortened_tag($clean, $system_config->max_unshortened_tag_length);
      }
   }
   if(!canUseHTML() or $system_config->allow_oscailt_macros == false)
   {
      $clean = stripOscailtMacros($clean);
   }
   return $clean;
}

//takes the field from its storage format and formats it for the form
function unprocessRichContentField($content, $use_html, $to_html = true)
{
   global $editor_session, $system_config, $basic_tags, $approved_tags, $public_tags;
   if($to_html and (!$use_html or !$editor_session->isSessionOpen()))
   {
      $content = str_replace("<br />\n","\n",$content);
   }
   if($to_html && $system_config->allow_bb_code and !canUseHTML())
   {
      $content = unprocessBBCode($content);
   }
   if(!$to_html)
   {
      $allowed_tags = array();
      $content = stripTagList($content, $allowed_tags);
   }
   elseif(!$use_html or !$editor_session->isSessionOpen())
   {
      $allowed_tags = $public_tags;
      if(!in_array('a', $allowed_tags))
      {
         $content = stripShortenedTags($content);
      }
      $content = stripTagList($content, $allowed_tags);
   }
   elseif(!canUseHTML())
   {
      $allowed_tags = array_unique(array_merge($public_tags, $basic_tags));
      $content = stripTagList($content, $allowed_tags);
   }
   elseif(!canUseFreeHTML())
   {
      $allowed_tags = array_unique(array_merge($public_tags, $basic_tags));
      $allowed_tags = array_unique(array_merge($allowed_tags, $approved_tags));
      $content = stripTagList($content, $allowed_tags);
   }
   return $content;
}

function stripShortenedTags($content)
{
   $tag = "a";
   $content = preg_replace('@<'.$tag.'[^>]*href=\s*[\"\']([^\s]*)[\"\'][^>]*>.*?</'.$tag.'>@si', "\\1", $content);
   return $content;
}

function listMessedUpBBCode($clean)
{
   global $allowed_bb_code, $system_config;
   $problems = array();
   $try_bb_code = array_map('strtoupper', $allowed_bb_code);
   foreach($try_bb_code as $bb_tag)
   {
      $clean = strtoupper($clean);
      $stripped = strip_tags($clean);//in case its hidden in html attributes
      $open_matches = array();
      $close_matches = array();
      $opening_tag_count = preg_match_all('@\['.$bb_tag.'([^\]]*?)\]@si', $stripped, $open_matches);
      $closing_tag_count = preg_match_all('@\[/'.$bb_tag.'\s*?\]@si', $stripped, $close_matches);
      if($opening_tag_count != $closing_tag_count)
      {
         if($closing_tag_count > $opening_tag_count)
         {
            $problems[] = "Closing Tag [/$bb_tag] with no matching opening tag (x".($closing_tag_count-$opening_tag_count).")";
         }
         else
         {
            $problems[] = "Open Tag [$bb_tag] with no matching closing tag (x".($opening_tag_count-$closing_tag_count).")";
         }
         return $problems;
      }
      elseif($opening_tag_count == 0) continue;
      $start_pos = -1;
      $end_pos = -1;
      //now check that there is no crap in between
      for($i = 0; $i < count($open_matches[0]); $i++)
      {
         $start_pos = strpos($clean, $open_matches[0][$i], $start_pos+1);
         $end_pos = strpos($clean, $close_matches[0][$i], $end_pos+1);
         if($start_pos === false or $end_pos === false)
         {
            $problems[] = "BBCode [$bb_tag] embedded in HTML - This is not allowed";
            return $problems;
         }
         elseif($end_pos < ($start_pos+strlen($open_matches[0][$i])))
         {
            $problems[] = "BBCode [$bb_tag] incorrectly specified - the start tag needs to come before the end tag!";
            return $problems;
         }
         $filling = trim(substr($clean, $start_pos+strlen($open_matches[0][$i]),$end_pos-($start_pos+strlen($open_matches[0][$i]))));
         //look for embedded html
         if(strlen($filling) == 0)
         {
            $problems[] = "BB Tag: [$bb_tag] has empty contents. Tags with empty contents don't do anything except take up space.";
            return $problems;
         }
         $tag_matches = array();
         if(preg_match('@<(/?\w+)@', $filling, $tag_matches))
         {
            $problems[] = "HTML Tag: <".$tag_matches[1][0]."> embedded in BB Tags - This is not allowed";
            return $problems;
         }
         //look for incorrectly embedded bbcode
         if($bb_tag === 'LIST')
         {
            $list_tags = array();
            $embedded_count = preg_match_all('@\[(/?[\w\*][^\]]*?)\]@si', $filling, $list_tags);
            if($embedded_count == 0)
            {
               $problems[] = "No List Entries found in BB List. Empty Lists are not allowed.  To place an entry in a list, use the [*] tag";
               return $problems;
            }
            foreach($list_tags[1] as $m)
            {
               if(trim($m) != '*')
               {
                  $problems[] = "BB Tag: [".$m."] embedded in BB List - This is not allowed";
               }
            }
         }
         else
         {
            if(strlen($filling) > $system_config->maximum_allowed_bbed_text)
            {
               $problems[] = "You have embedded too much text inside the BB Tag: [".$bb_tag."] you are only allowed to tag up to $system_config->maximum_allowed_bbed_text characters in one section.  The feature is for highlighting small chunks of text, not huge screeds.";
            }
            if($bb_tag === 'QUOTE')//allowed embedded i's and b's but numbers must match
            {
               $m = array();
               $open_tags = preg_match_all('@\[[ib]\s*?\]@si', $filling, $m);
               $close_tags = preg_match_all('@\[/[ib]\s*?\]@si', $filling, $m);
               if($open_tags != $close_tags)
               {
                  $problems[] = "You have mismatched BB Tags inside BB quotes. You can use the [i] and [b] tags inside quoted text, but you can't embed them and you must close your tags properly by using [/i] and [/b] after the text that you want highlighted.";
                  return $problems;
               }
               $embed_bb_code = array_map('strtoupper', $allowed_bb_code);
               foreach($embed_bb_code as $embed_bb_tag)
               {
                  $t = strtoupper($embed_bb_tag);
                  if($t == 'I' or $t == 'B') continue;
                  elseif(preg_match('@\[/?'.$t.'\b@si', $filling))
                  {
                     $problems[] = "You have embedded the BB Tag [$t] inside the tag [QUOTE]. This is not allowed.";
                     return $problems;
                  }
               }
            }
            else
            {
               $embed_bb_code = array_map('strtoupper', $allowed_bb_code);
               foreach($embed_bb_code as $embed_bb_tag)
               {
                  if((strpos($filling, '['.$embed_bb_tag) !== false) or (strpos($filling, '[/'.$embed_bb_tag) !== false))
                  {
                     $problems[] = "You have embedded the BB Tag [$embed_bb_tag] inside the tag [$bb_tag]. This is not allowed.";
                     return $problems;
                  }
               }
            }

         }
      }
   }
   return $problems;
}

function unprocessBBCode($clean)
{
   //return $clean;
   $clean = preg_replace('@<CITE\s*class=[\'"]?BBINPUT[\'"]?>(.*?)</CITE>@si', "[QUOTE]\\1[/QUOTE]", $clean);
   $clean = preg_replace('@<UL\s*class=[\'"]?BBINPUT[\'"]?>(.*?)</UL>@si', "[LIST]\\1\n[/LIST]", $clean);
   $clean = preg_replace('@<STRONG\s*class=[\'"]?BBINPUT[\'"]?>(.*?)</STRONG>@si', "[B]\\1[/B]", $clean);
   $clean = preg_replace('@<EM\s*class=[\'"]?BBINPUT[\'"]?>(.*?)</EM>@si', "[I]\\1[/I]", $clean);
   $clean = preg_replace('@<LI\s*class=[\'"]?BBINPUT[\'"]?>(.*?)</LI>@si', "\n[*]\\1", $clean);
   return $clean;
}

function processBBCode($clean)
{
   $problems = listMessedUpBBCode($clean);
   if(count($problems) > 0)
   {
      //echo "<P>".htmlspecialchars($clean)."</P>";
   }
   global $allowed_bb_code, $system_config;
   //$problems = array();
   $try_bb_code = array_map('strtoupper', $allowed_bb_code);
   foreach($try_bb_code as $bb_tag)
   {
      if($bb_tag == 'LIST')
      {
         $pattern = '@(\[LIST\b[^\]]*?\])\s*(\[\*[^\]]*?\].*?)(\[/LIST\b[^\]]*?\])@sie';
         $clean = preg_replace($pattern, "'<UL class=\"BBINPUT\">'.processListBB('\\2').'</UL>'", $clean);
      }
      else
      {
         $patterns = array('@\['.$bb_tag.'\b[^\]]*?\]@si', '@\[/'.$bb_tag.'\b[^\]]*?\]@si');
         if($bb_tag == 'I') $r = 'em';
         elseif($bb_tag == 'B') $r = 'strong';
         elseif($bb_tag == 'QUOTE') $r = 'cite';
         $replacements = array("<$r class='BBINPUT'>", "</$r>");
         $clean = preg_replace($patterns, $replacements, $clean);
      }
   }
   return $clean;
}

function processListBB($str)
{
   $rtn = "";
   //echo "<P>processing $str</P>";
   $matches = preg_split('@[\*[^\]]*?\]\s*@', $str);
   foreach($matches as $match)
   {
      $x = trim($match);
      if($x != "")
      {
         $rtn .= "<LI class='BBINPUT'>$x</LI>";
      }
   }
   return $rtn;
}

function compactWhiteSpace($clean)
{
   //first get rid of multiple non-breaking spaces
   $clean = str_replace("\r\n", "\n", $clean);
   $clean = str_replace("\r", "\n", $clean);
   $clean = preg_replace('/[^\S\n]{1,}/', " ", $clean);
   //then get rid of multiple blank lines.
   $clean = preg_replace('/\n\s+\n/', "\n\n", $clean);
   return $clean;
}




function listUnclosedTags($content, $allowed_tags)
{
   global $system_config;
   $unc = array();
   foreach($allowed_tags as $tag)
   {
      if($tag == 'br' or $tag == 'img' or $tag == 'hr') continue;
      elseif(!$system_config->enforce_strict_tag_closing && ($tag == 'p' or $tag == 'li' or $tag=='td' or $tag == 'tr' or $tag == 'dd' or $tag == 'dt' or $tag == 'thead' or $tag == 'tbody' or $tag == 'tfoot')) continue;
      $start_matches = array();
      $end_matches = array();
      $open_count = preg_match_all('@<'.$tag.'\b[^>]*?>@si', $content, $start_matches);
      $close_count = preg_match_all('@</'.$tag.'\b[^>]*?>@si', $content, $end_matches);
      if($open_count != $close_count)
      {
         if($close_count > $open_count)
         {
            $unc[] = "(".($close_count-$open_count).") Closing Tag </$tag>) with no matching opening tag";
         }
         else
         {
            $unc[] = "(".($open_count-$close_count).") Open Tag <$tag> with no matching closing tag";
         }
      }

   }
   return $unc;
}

//this is a bottle-neck
function stripTagList($unclean, $tags, $attrs = false)
{
   global $performance_test;
   $tag_str = '<'.implode("><",$tags).'>';
   //while($unclean != strip_tags($unclean, $tag_str))
   //{
      $unclean = strip_tags($unclean, $tag_str);
      $unclean = stripEvilAttributes($unclean);
   //}
   if($attrs !== false)
   {
      $replacements = array();
      $contained_tags = getContainedTagList($unclean);
      foreach($contained_tags as $tag => $occurrences)
      {
         //echo "<P>processing tag $tag ".htmlspecialchars(implode(":::", $occurrences))."</P>";
         if(!isset($attrs[$tag]) or count($attrs[$tag]) == 0)
         {
            foreach($occurrences as $one_oc)
            {
               if(!isset($replacements[$one_oc]) && strcasecmp($one_oc, "<$tag>") != 0)
               {
                  $replacements[$one_oc] = "<$tag>";
               }
            }
         }
         else
         {
            $full_strip = false;
            $attr_list = $attrs[$tag];
            foreach($occurrences as $one_oc)
            {
               if(!isset($replacements[$one_oc]) && strcasecmp($one_oc, "<$tag>") != 0)
               {
                  //$replacements[$one_oc] = stripNonApprovedAttributes($one_oc, $tag, $attr_list);
                  $full_strip = true;
                  break;
               }
            }
            if($full_strip)
            {
               //echo "<P>stripping attributes from $tag: ".implode("::", $attr_list)."</P>";
               $unclean = stripNonApprovedAttributes($unclean, $tag, $attr_list);
            }
         }
      }
      if(count($replacements) > 0)
      {
         $unclean = str_replace(array_keys($replacements), array_values($replacements), $unclean);
      }
      //foreach($replacements as $orig => $noo)
      //{
      //   echo "<P>replacing $orig with $noo</P>";
      //   $unclean = str_replace($orig, $noo, $unclean);
      //}
      /*foreach($tags as $tag)
      {
         if(!isset($attrs[$tag])) $attr_list = array();
         else $attr_list = $attrs[$tag];
         if(!is_array($attr_list)) $attr_list = array();
         $unclean = stripNonApprovedAttributes($unclean, $tag, $attr_list);
      }*/

   }
   if($performance_test > 7) markTime("tags stripped from content");
   return $unclean;
}

function getContainedTagList($html_str)
{
   $tags = array();
   $reg_ex = '@<([\w]+)[^>]*>@s';
   $matches = array();
   preg_match_all($reg_ex, $html_str, $matches);
   //set up array of tag -> tag string..
   if($matches and is_array($matches) and count($matches[1]) > 0)
   {
      for($i = 0; $i<count($matches[0]); $i++)
      {
         if(!isset($tags[$matches[1][$i]]))
         {
            $tags[$matches[1][$i]] = array();
         }
         $tags[$matches[1][$i]][] = $matches[0][$i];
      }
/*      foreach($matches[0] as $m)
      {
         if(!isset($tags[$m[1]]))
         {
            $tags[$m[1]] = array();
         }
         $tags[$m[1]] = $m[0];
      }*/
   }
   return $tags;
}

function filterApprovedAttributes($tag_txt, $tag_name, $attrs)
{
   return $tag_txt;
}

function stripNonApprovedAttributes($msg,$tag,$attrs)
{
   //$msg = stripEvilAttributes($msg);
   $lengthfirst = 0;
   while (strstr(substr($msg,$lengthfirst),"<$tag ")!="")
   {
      $rtn = "<$tag";
      $imgstart = $lengthfirst + strpos(substr($msg,$lengthfirst), "<$tag ");
      $partafterwith = substr($msg,$imgstart);
      $img = substr($partafterwith,0,strpos($partafterwith,">")+1);
      if(strlen($img) == 0) continue;
      foreach($attrs as $attr)
      {
         //echo "<P>trying attribut $attr in ".htmlspecialchars($img)."</P>";
         $matches = array();
         if(preg_match('@\b'.$attr.'\b\s*=\s*([\w\"\'].*)@si', $img, $matches))
         {
            $trailing_bit = $matches[1];
            if($trailing_bit{0} == '"')
            {
               $end = strpos($trailing_bit, '"', 1);
               //echo "<b>end is $end</b>";
               if($end !== false) $rtn .= " $attr=".substr($trailing_bit, 0, $end+1);
            }
            elseif($trailing_bit{0} == "'")
            {
               $end = strpos($trailing_bit, "'", 1);
               if($end !== false) $rtn .= " $attr=".substr($trailing_bit, 0, $end+1);
            }
            else
            {
               if(preg_match('@\w+@', $trailing_bit, $matches))
               {
                  $rtn .= " $attr".'="'.$matches[0].'"';
               }
            }

         }
      }
      $rtn .= ">";
      $partafter = substr($partafterwith,strpos($partafterwith,">")+1);
      $msg = substr($msg,0,$imgstart).$rtn.$partafter;
      $lengthfirst = $imgstart+3;
   }
   return $msg;
}

//black_list_scrubbing
function stripEvilTags($unclean)
{
   global $evil_tags;
   $count = 0;
   //foreach ($evil_tags as $tag)
   //{
      $tag = "(".implode('|', $evil_tags).")";
      $unclean = preg_replace('@<'.$tag.'[^>]*?>@si', "<!-- evil tag removed -->", $unclean);
      $unclean = preg_replace('@</'.$tag.'[^>]*?>@si', "<!-- evil tag end removed -->", $unclean);
   //}
   return $unclean;
}

function stripEvilAttributes($unclean)
{
   global $evil_attributes;
   $att = implode('|', $evil_attributes);
   //foreach($evil_attributes as $att)
   //{
      //echo "<P>stripping $att</p>";
      $unclean = preg_replace('@(<[^>]*?)\b('.$att.')\b([^>]*?>)@si', '\1\3', $unclean);
   //}
   return $unclean;
}


function containsEvilTags($unclean)
{
   global $system_config, $evil_tags, $code_delimiters, $evil_attributes;
   $matches = 0;
   foreach($evil_tags as $tag)
   {

      if((preg_match('@<'.$tag.'[^>]*?>@si', $unclean)))
      {
         if($system_config->security_recording_level > 0)
         {
            logSecurityMessage("Attempt to use evil tag: $tag");
         }
         return true;
      }
   }
   foreach($code_delimiters as $delim)
   {
      if(strpos($unclean, $delim) !== false)
      {
         if($system_config->security_recording_level > 0)
         {
            logSecurityMessage("Attempt to use code delimiter: $delim");
         }
         return true;
      }
   }
   foreach($evil_attributes as $att)
   {
      if((preg_match('@<[^>]*?'.$att.'[^>]*?>@si', $unclean)))
      {
         if($system_config->security_recording_level > 0)
         {
            logSecurityMessage("Attempt to use evil attribute: $att");
         }
         return true;
      }
   }
   return false;
}


function stripUnclosedTags($unclean, $tag_list)
{}

?>