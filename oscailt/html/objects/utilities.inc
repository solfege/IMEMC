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
/* DB Connectivity                                                    */
/************************************************************************/
function connectToDatabase()
{
   global $dbconn;
   global $dbhost;
   global $dbuname;
   global $dbpass;
   global $dbname;
   global $sql_debug;
   global $query_cache;
   if($sql_debug) echo("<BR>connecting to database<BR>");
   if(is_object($query_cache) && $query_cache->isDatabaseMissing())
   {
   	$dbconn = null;
   }
   else
   {
	$dbconn = sql_connect($dbhost, $dbuname, $dbpass, $dbname);
	if (($dbconn == null) || ($dbconn == false))
	{
		$query_cache->databaseMissing();
		// Making assumption we are using MySQL when reporting the error.
		reportError("Database Error - failed to connect to database. Error: ".mysql_error());
	}
   }
}

function disconnectFromDatabase()
{
   global $dbconn,$query_cache,$sql_debug;
   if($dbconn != NULL)
   {
      if($sql_debug) echo("<BR>disconnecting from database<BR>");
      sql_logout($dbconn);
      SharedMemoryUpdate("DbOpens");
   } else {
      // Null. Db was never opened
      SharedMemoryUpdate("DbNoOpens");
   }

   // Clear HTML cache from here too. RSS cache flag really used for html cache clear.
   if($query_cache->sql_cache_invalid[1]==true || $query_cache->sql_cache_invalid[2]==true || $query_cache->rss_cache_invalid==true ) 
   {
      clearHtmlCache();
   }

   for($cache_grp = 1; $cache_grp <= 5; $cache_grp++)
   {
      if($query_cache->sql_cache_invalid[$cache_grp]==true) $query_cache->clearCache("sql", $cache_grp);
   }
}


/************************************************************************/
/* Used to figure out whether the object needs a html header            */
/************************************************************************/
function isHTMLPage()
{
   global $oscailt_page_setting;
   if(isset($oscailt_page_setting["obj_type"]) && $oscailt_page_setting["obj_type"] == "FEEDEXPORT")
   {
      return false;
   }
   return true;
}


/************************************************************************/
/* General Purpose Data Fiddling stuff                                  */
/************************************************************************/
function stripslashes_deep($value)
{
   $value = is_array($value) ? array_map('stripslashes_deep', $value) : stripslashes($value);
   return $value;
}

function markTime($msg)
{
   global $time_list;
   $time_list[] = array($msg, microtime());
}

function getMicrotimeDiffAsSecs($a, $b)
{
   return number_format(((substr($b,0,9)) + (substr($b,-10)) - (substr($a,0,9)) - (substr($a,-10))),4);
}

function printTimingData($msg)
{
   global $time_list, $performance_test;
   $time_list[] = array($msg, microtime());
   if($performance_test == 1 or count($time_list) == 2)
   {
       $total_t = getMicrotimeDiffAsSecs($time_list[0][1], $time_list[count($time_list)-1][1]);
       echo "<br><center><small><small>".$time_list[count($time_list)-1][0]." -> $total_t seconds</small></small></center>";
       return;
   }
   elseif(count($time_list) > 2)
   {
      echo "<div style='width: 600px; margin: 2em auto 2em auto;'>";
      for($i = 0; $i < count($time_list); $i++)
      {
         if($i == 0)
         {
            echo "<table class='admin' align='center'><tr class='admin'><th class='admin'>Stage</th><th class='admin'>stage time</th><th class='admin'>Total Time</th></tr>";
         }
         else
         {
            list($msg, $mt) = $time_list[$i];
            list($d, $first_mt) = $time_list[0];
            list($x, $last_mt) = $time_list[$i-1];
            $tot_t = getMicrotimeDiffAsSecs($first_mt, $mt);
            $stage_t = getMicrotimeDiffAsSecs($last_mt, $mt);
            echo "<tr class='admin'><td class='admin'>$msg</td><td class='admin'>$stage_t secs</td><td class='admin'>$tot_t secs</td></tr>";
            if($i == (count($time_list)-1))
            {
               echo "</table>";
            }
         }
      }
      echo "</div>";
   }
}

function echobig($string, $bufferSize = 8192)
{
  if($bufferSize <= 0) $bufferSize = 8192;
  for ($chars=strlen($string)-1,$start=0;$start <= $chars;$start += $bufferSize) {
   echo substr($string,$start,$bufferSize);
  }
}

function isRelativePath($str)
{
   if($str{0} == '/' or $str{0} == '\\' or substr($str, 1, 2) == ':\\') return false;
   return true;
}

function isAbsolutePath($str)
{
   if($str{0} == '/' or $str{0} == '\\' or substr($str, 1, 2) == ':\\') return true;
   if(realpath($str) == $str) return true;
   return false;
}

function isAbsoluteURL($str)
{
   foreach(array("http://", "ftp://", "https://", "itms://") as $p)
   {
      if(strtolower(substr($str, 0, strlen($p))) == $p) return true;
   }
   return false;
}

function isLocalPath($str)
{
   global $system_config;
   if(isAbsoluteURL($str))
   {
      if(strpos($str, $system_config->site_url) !== false)
      {
         return true;
      }
      return false;
   }
   if(isRelativeUrl($str)) return true;
   elseif(isAbsolutePath($str)) return true;
   return true;
}

function isRelativeUrl($str)
{
   if(stristr($str, "://") !== false) return false;
   return isRelativePath($str);
}

function getAbsolutePathAsRelative($inputFile)
{
   $doc_root = $_SERVER['DOCUMENT_ROOT'];
   return str_replace($doc_root, "", $inputFile);
}

function findLocalLinkedImages($txt, $offset="")
{
   $local_links = array();
   $pattern = '#<img[^>]+src\s*=\s*[\'\"]([^\'\"]+?)[\'\"]#is';
   preg_match_all($pattern, $txt, $matches);
   foreach($matches[1] as $m)
   {
      if(isLocalPath($offset.$m) and file_exists($offset.$m))
      {
         $local_links[] = $m;
      }
   }
   return $local_links;
}

function makeLinksRelative($txt, $make_all_relative=false)
{
   global $system_config;
   if($make_all_relative)
   {
      $pattern = '#(http|https)(://)'.$system_config->site_url.'/#i';
      $txt = preg_replace($pattern, '', $txt);
   }
   else
   {
      $pattern = '#(<a[^>]+href\s*=\s*[\'\"])(http|https)://'.$system_config->site_url.'/([^\'\"]+[\'\"])#si';
      $txt = preg_replace($pattern, '$1$3', $txt);
   }

   return $txt;
}

function stripOscailtMacros($txt, $allowed_list = array())
{
   $patterns = array();
   if(!in_array("OSCAILTIMAGE", $allowed_list))
   {
      $patterns[] = "@OSCAILTIMAGE\((\S+)\s+(\d+)\s+(\d+)\s*(\w*)\)@si";
   }
   if(!in_array("OSCAILTOBJECT", $allowed_list))
   {
      $patterns[] = "@OSCAILTOBJECT\((\d+)\s*([\w\-]*)\s*\)@si";
   }
   return preg_replace($patterns, "", $txt);
}

function substituteOscailtMacros($txt, $use_absolute_url=true)
{
   return substituteObjectMacro(substituteImageMacro($txt, $use_absolute_url));
}

function substituteImageMacro($txt, $use_absolute_url=true, $story_images=null)
{
   global $system_config, $graphics_store;
   $pattern = "@OSCAILTIMAGE\((\S+)\s+(\d+)\s+(\d+)\s*(\w*)\)@si";
   preg_match_all($pattern, $txt, $matches);

   if(count($matches[1]) > 0)
   {
      $reps = array();
      $origs = $matches[0];
      for($i = 0; $i< count($matches[1]); $i++)
      {
         $file = $matches[1][$i];
         $width = $matches[2][$i];
         $height = $matches[3][$i];
         $align = $matches[4][$i];
         if($align != "")
         {
            $align = strtolower($align{0});
            if($align == 'r') $align = "align='right'";
            elseif($align == 'l') $align = "align='left'";
            else $align = "";
         }

         if($file == 'DUMMY')
         {
            if($use_absolute_url)
            {
               $reps[] = "<img $align class='feature' src='http://".$system_config->site_url."/graphics/feature-dummy.jpg' alt='dummy image for feature'>";
            }
            else
            {
               $reps[] = "<img $align class='feature' src='".$graphics_store."feature-dummy.jpg' alt='dummy image for feature'>";
            }
         }
	 else if(preg_match("/embedaudio[:=\(]/i",$file) > 0 )
         {
	    // Format OSCAILTIMAGE(embedaudio:01=URL_to_mp3 1 1) or OSCAILTIMAGE(embedaudio:URL_to_mp3)
            $audioId = substr($file,11,2);
	    if ($audioId == "01") $audioId = substr($file,14);
	    else $audioId = substr($file,11);

            $reps[] = "<BR />" . getEmbeddedAudioHTML(true, $audioId);
         }
	 else if(strpos($file,"embedvideo=") !== false || strpos($file,"embedvideo:") !== false )
         {
            // Format to parse is: embedvideo=XX=VideoId width height   -Can't have : -colon
	    // where XX is the type. Must be 2 digits for the parsing here. Later can split on the =
            $videoType = (int) substr($file,11,2); 
            $videoId = substr($file,14);
            $reps[] = "<BR />" . htmlspecialchars(getEmbeddedVideoHTML($videoType, $videoId,$width,$height));
         }
         else
         {
            if(count($story_images) > 0 ) {
	        if (preg_match("/image\d{1,2}/i", $file) > 0 ) {
	            // Need to handle 2 digits but just one for now.
                    $t_index=substr($file,5,1);
                    $t_index--;
                    if ($t_index <= count($story_images) ) $file = $system_config->attachment_store . $story_images[$t_index]->attachment_file;
	        }
	    }
            if(strpos($file, $system_config->attachment_store_url) !== false)
            {
               $file = str_replace($system_config->attachment_store_url, $system_config->attachment_store, $file);
            }
            $imageConfig = new ImageConfig($width,$height,false,false,$system_config->max_feature_imagesize,array(0xFF, 0xFF, 0xFF),0,0,0,0,0);
            $tool = new ImageTool();
            $url_info = $tool->getTransformedImageURL($file,$imageConfig);
            $reps[] = getTransformedImageHTML($url_info, $use_absolute_url, $align);
         }
      }
      return str_replace($origs, $reps, $txt);
   }
   return $txt;
}

function substituteObjectMacro($txt)
{
   global $page_style;
   $pattern =  "@OSCAILTOBJECT\((\d+)\s*([\w\-]*)\s*\)@si";
   preg_match_all($pattern, $txt, $matches);
   if(count($matches[1]) > 0)
   {
      $reps = array();
      $origs = $matches[0];
      for($i = 0; $i< count($matches[1]); $i++)
      {
         $obj_id = $matches[1][$i];
         $obj_style = $matches[2][$i];
         $obj_file = getObjectCacheIndexFile($obj_id);
         ob_start();
         if(file_exists($obj_file))
         {
            if($obj_style != "")
            {
               $save_style = $page_style;
               $page_style = $obj_style;
            }
            include_once($obj_file);
            if($obj_style != "")
            {
               $page_style = $save_style;
            }
         }
         $reps[] = ob_get_contents();
         ob_end_clean();
      }
      return str_replace($origs, $reps, $txt);
   }
   return $txt;
}


function getTransformedImageHTML($img_info, $use_abs, $align="")
{
   global $pseudo_directory_offset, $system_config;
   if(!$img_info) return "";
   $img_url = $img_info[0];
   $full_file_url = $img_info[1];
   if(!isAbsoluteURL($img_url) && isRelativePath($img_url))
   {
      if($use_abs)
      {
         $img_url = "http://".$system_config->site_url."/".$img_url;
      }
      else
      {
         $img_url = $pseudo_directory_offset.$img_url;
      }
   }
   if(!isAbsoluteURL($full_file_url) && isRelativePath($full_file_url))
   {
      if($use_abs)
      {
         $full_file_url = "http://".$system_config->site_url."/".$full_file_url;
      }
      else
      {
         $full_file_url = $pseudo_directory_offset.$full_file_url;
      }
   }
   if(($full_file_url != $img_url))
   {
      $a = "<a href='$full_file_url'>";
      $a_e = "</a>";
   }
   else
   {
      $a = "";
      $a_e = "";
   }
   $tag = "$a<img $align $img_info[2] class='feature' src='$img_url' alt='featured image'>$a_e";
   return $tag;
}



function serialiseArrayParams(&$arr, $lvl = 0)
{
   if($lvl > 10) return '"Recursion Limit"';
   $str = 'Array(';
   $first = true;
   foreach(array_keys($arr) as $k)
   {
      if($first)
      {
         $first = false;
      }
      else
      {
         $str .= ', ';
      }
      $str .= '"'.$k.'" => ';
      if(is_array($arr[$k]))
      {
         $str .= serialiseArrayParams($arr[$k], ++$lvl);
      }
      else
      {
         $str .= '"'.addslashes($arr[$k]).'"';
      }
   }
   $str .= ')';
   return $str;
}

function remote_file_size ($url)
{
   $head = "";
   $url_p = parse_url($url);
   $host = $url_p["host"];
   $path = $url_p["path"];

   $fp = fsockopen($host, 80, $errno, $errstr, 20);
   if(!$fp)
   {
      return false;
   }
   else
   {
       fputs($fp, "HEAD ".$url." HTTP/1.1\r\n");
       fputs($fp, "HOST: dummy\r\n");
       fputs($fp, "Connection: close\r\n\r\n");
       $headers = "";
       while (!feof($fp)) {
           $headers .= fgets ($fp, 128);
       }
   }
   fclose ($fp);
   $return = false;
   $arr_headers = explode("\n", $headers);
   foreach($arr_headers as $header) {
      $s = "Content-Length: ";
      if(substr(strtolower ($header), 0, strlen($s)) == strtolower($s)) {
          $return = substr($header, strlen($s));
          break;
      }
   }
   return $return;
}


/************************************************************************/
/* Basic Language Stuff & filtering based on file extensions            */
/************************************************************************/
function getPageLanguageOptions($my_lang, $available_langs, $id)
{
   global $languageList, $userLanguage, $pseudo_directory_offset, $OSCAILT_SCRIPT, $system_config;
   if(!$system_config->show_page_translations_box) return;
   echo "<div class='page-language-options'>";
   if(!in_array($userLanguage->getLanguageCodePrefix(), $available_langs))
   {
      echo "<P class='page-language-options'>This page has not been translated into ".$userLanguage->language_name." yet.</p>";
   }
   echo "This page can be viewed in<br />";
   foreach($available_langs as $av_lang)
   {
      $lang = $languageList->getLanguageByPrefix($av_lang);
      $lang_name = $lang->language_name;
      if($av_lang == $my_lang)
      {
         echo "<span class='page-language-selected'>$lang_name</span> ";
      }
      else
      {
         $lnk = $pseudo_directory_offset.$OSCAILT_SCRIPT."?obj_id=$id"."&amp;userlanguage=$av_lang";
         echo "<span class='page-language-option'><a class='page-language-option' href='$lnk'>$lang_name</a></span> ";
      }
   }
   echo "</div>";
}

function compareToUserLanguage($candidate)
{
   global $userLanguage;
   $uprefix = $userLanguage->getLanguageCodePrefix();
   if(substr($candidate, 0, strlen($uprefix)) == $uprefix){
      return true;
   }
   return false;
}

function getFileByLanguage($dir, $fname, $prefix = "", $extension='.xml')
{
   global $userLanguage, $languageList;
   if($prefix == "") $prefix = $userLanguage->getLanguageCodePrefix();
   //first try the explicit one..
   $objfile = $dir.$fname.'_'.$prefix.$extension;
   if( !file_exists( $objfile ) )
   {
      //fall back on the standard..
      $objfile = $dir.$fname.$extension;
      if( !file_exists( $objfile ) )
      {
         return false;
      }
   }
   return $objfile;
}

function findFile($path,&$filename,$isImage)
{
   $exists = false;
   if($isImage)
   {
      if(file_exists($path.$filename.".gif"))
      {
         $filename = $filename.".gif";
         return true;
      }
      if(file_exists($path.$filename.".jpg"))
      {
         $filename = $filename.".jpg";
         return true;
      }
      if(file_exists($path.$filename.".png"))
      {
         $filename = $filename.".png";
         return true;
      }
      //default assume gif
      $filename = $filename.".gif";
      return false;
   }
   else
   {
      return file_exists($path.$filename);
   }
   return false;
}

function getFilteredFile($path,$filename,$isImage)
{
   global $pageRegion,$pageTopic,$userLanguage;
   if($pageRegion!=null && $pageTopic!=null && $userLanguage!=null)
   {
      $file=strtolower($pageRegion->getStrippedName())."_".strtolower($pageTopic->getStrippedName())."_".strtolower($userLanguage->getStrippedCode())."_".$filename;
      if(findFile($path,$file,$isImage)) return $file;
      $file=strtolower($pageTopic->getStrippedName())."_".strtolower($pageRegion->getStrippedName())."_".strtolower($userLanguage->getStrippedCode())."_".$filename;
      if(findFile($path,$file,$isImage)) return $file;
   }
   if($pageRegion!=null && $pageTopic!=null)
   {
      $file=strtolower($pageRegion->getStrippedName())."_".strtolower($pageTopic->getStrippedName())."_".$filename;
      if(findFile($path,$file,$isImage)) return $file;
      $file=strtolower($pageTopic->getStrippedName())."_".strtolower($pageRegion->getStrippedName())."_".$filename;
      if(findFile($path,$file,$isImage)) return $file;
   }
   if($pageRegion!=null && $userLanguage!=null)
   {
      $file=strtolower($pageRegion->getStrippedName())."_".strtolower($userLanguage->getStrippedCode())."_".$filename;
      if(findFile($path,$file,$isImage)) return $file;
   }
   if($pageRegion!=null)
   {
      $file=strtolower($pageRegion->getStrippedName())."_".$filename;
      if(findFile($path,$file,$isImage)) return $file;
   }
   if($pageTopic!=null && $userLanguage!=null)
   {
      $file=strtolower($pageTopic->getStrippedName())."_".strtolower($userLanguage->getStrippedCode())."_".$filename;
      if(findFile($path,$file,$isImage)) return $file;
   }
   if($pageTopic!=null)
   {
      $file=strtolower($pageTopic->getStrippedName())."_".$filename;
      if(findFile($path,$file,$isImage)) return $file;
   }
   if($userLanguage!=null)
   {
      $file=strtolower($userLanguage->getStrippedCode())."_".$filename;
      if(findFile($path,$file,$isImage)) return $file;
   }
   if($isImage)
   {
      findFile($path,$filename,$isImage);// get correct file ending
      return $filename;
   }
   else return $filename;
}


/************************************************************************/
/* Adding stuff to the meta tags in the html header                     */
/************************************************************************/
function addToPageDescription($str)
{
   global $pageDescription;
   if(isset($pageDescription) && $pageDescription != "")
   {
      $pageDescription = strip_tags($str)." ".$pageDescription;
   }
   else
   {
      $pageDescription = strip_tags($str, ENT_QUOTES);
   }
}

function addToPageKeywords($str)
{
   global $pageKeywords;
   if(isset($pageKeywords) && $pageKeywords != "")
   {
      $pageKeywords .= strip_tags($str)." ".$pageKeywords;
   }
   else
   {
      $pageKeywords = strip_tags($str);
   }
}

function addToPageTitle($str)
{
   global $pageTitle;
   if(isset($pageTitle) && $pageTitle != "")
   {
      $pageTitle = strip_tags($str)." - $pageTitle";
   }
   else
   {
      $pageTitle = strip_tags($str);
   }
}
function setPageTitle($str)
{
   global $pageTitle;
   $pageTitle = strip_tags($str);
}

function loadSiteKeywords()
{
   global $site_keywords, $system_config;
   $site_keywords = $system_config->site_keywords;
   return $site_keywords;
}


/************************************************************************/
/* Error Reporting and Logging etc.                                     */
/************************************************************************/
function checkForError($result)
{
   if(!$result)
   {
      global $dbconn, $last_sql_query;
      reportError("database error : ".mysql_errno()." - ".mysql_error());
      reportError("Last database query : ". $last_sql_query);
      return false;
   }
   return true;
}

function convertIPToNumber($ip_addr)
{
    
    list($p4, $p3, $p2, $p1) = explode(".",$ip_addr);

    $p2 = $p2 << 8 ;
    $p3 = $p3 << 16;
    $p4 = $p4 << 24;

    return ($p4+$p3+$p2+$p1);
}
function convertNumberToIP($ip_number)
{
    $p4 = $ip_number & 0xff000000;
    $p3 = $ip_number & 0x00ff0000;
    $p2 = $ip_number & 0x0000ff00;
    $p1 = $ip_number & 0x000000ff;

    $p2 = $p2 >> 8 ;
    $p3 = $p3 >> 16;
    $p4 = $p4 >> 24;

    return ($p4.".".$p3.".".$p2.".".$p1);
}

function logAction($item, $item_id, $item_type, $item_action, $action_reason="")
{
   global $system_config,$editor_session;
   if($editor_session->isSessionOpen())
   {
      $nm = $editor_session->editor->editor_name;
   }
   else
   {
      $nm = "Unknown User";
   }
   if(isset($_REQUEST['action_reason']) && trim($_REQUEST['action_reason']) != "")
   {
      $reason = trim($_REQUEST['action_reason']);
   }
   else
   {
      if ($action_reason != "")
          $reason = $action_reason;
      else
          $reason = "<not specified>";
   }
   $msg = $nm.":\t".$item_action.":\t".$item_type.":\t".$item_id.":\t".$reason;
   if ($item_type == "IP" && ($item_action == "ban" || $item_action == "unban")) {
       $item_id = convertIPToNumber($item_id);
   }
   logMessage($msg,'actionlog.txt');
   // if ($item_type == "IP-Monitor" || $item_action == "site-recache" || $item_id == "all-images" || $item_id == "all-objects" || $item_id == "all-type") return;
   writeEditorialAction($item, $item_id, $item_type, $item_action, $nm, $reason);
}
function writeEditorialAction($item, $item_id, $item_type, $item_action, $editorname, $action_reason)
{
   global $prefix, $dbconn;

   $actiontime ="FROM_UNIXTIME(".time().")";
   // Clean up item id and make sure it is not longer than 20 chars as it is defined as varchar(20)
   $item_id = trim($item_id);
   if (strlen($item_id) > 20) $item_id = substr($item_id,0,20);

   $action_reason = addslashes($action_reason);

   if (strrpos($item_id,",") === false)
   {
       $sql_string = "INSERT INTO ".$prefix."_editorial_actions values (NULL,".$actiontime.",'".$editorname."','". $item_action."','".$item_type."','".$action_reason."', '".$item_id."',NULL)";
   } else {
       list($primary_id, $secondary_id) = explode(",",$item_id);
       $sql_string = "INSERT INTO ".$prefix."_editorial_actions values (NULL,".$actiontime.",'".$editorname."','". $item_action."','".$item_type."','".$action_reason."', '".$primary_id."',".$secondary_id.")";
   }

   // echo("STRING: ".$sql_string."<BR>");
   checkForError(sql_query($sql_string, $dbconn,1));
   $db_result = mysql_insert_id($dbconn);

}
function getEditorialReasons($in_list, $is_story = true)
{
    global $dbconn, $prefix;
	
    // This part could be moved to a generic editorial_actions utils part and could be reused for coments
    if ($is_story == true ) {
       $ed_query = "SELECT content_id,editor_name,editor_reason from ".$prefix."_editorial_actions where action='hide' AND (content_type='story' OR content_type='feature') AND content_id in (";
    } else {
       $ed_query = "SELECT secondary_id,editor_name,editor_reason from ".$prefix."_editorial_actions where action='hide' AND content_type='comment' AND secondary_id in (";
    }
    $in_list= substr($in_list,0, (strlen($in_list)-1));
    if ($is_story == true ) {
        $ed_query .= $in_list . ") ORDER BY content_id";
    } else {
        $ed_query .= $in_list . ") ORDER BY secondary_id";
    }
    // echo("query: ".$ed_query."<BR>");

    $reasons_array = array();
    $result = sql_query($ed_query, $dbconn, 1);
    checkForError($result);
    if(sql_num_rows( $result ) > 0)
    {
       for ($k=0; $k<sql_num_rows( $result ); $k++)
       {
           // Well in the case of comments the story id is really the comment id
           list($t_story_id, $t_name, $t_reason) = sql_fetch_row($result, $dbconn);
	   // echo("Setting id " . $t_story_id ." to " .$t_reason."<BR>");
	   if ($t_story_id != null ) $reasons_array[$t_story_id] = $t_name . ": " .$t_reason;
       }
    }

    return $reasons_array;
}
function getAnnotations($in_list, $is_story = true)
{
    global $dbconn, $prefix;
	
    // This part could be moved to a generic editorial_actions utils part and could be reused for coments
    // These queries only ever get called when a user is logged in.
    if ($is_story == true ) {
       $ed_query = "SELECT content_id,editor_name,editor_reason from ".$prefix."_editorial_actions where action='annotate' AND (content_type='story' OR content_type='feature') AND content_id in (";
    } else {
       $ed_query = "SELECT secondary_id,editor_name,editor_reason from ".$prefix."_editorial_actions where (action='annotate' OR (action='promote' AND editor_reason !='<not specified>')) AND content_type='comment' AND secondary_id in (";
    }
    $in_list= substr($in_list,0, (strlen($in_list)-1));
    if ($is_story == true ) {
        $ed_query .= $in_list . ") ORDER BY content_id";
    } else {
        $ed_query .= $in_list . ") ORDER BY secondary_id";
    }

    $reasons_array = array();
    $result = sql_query($ed_query, $dbconn, 1);
    checkForError($result);
    if(sql_num_rows( $result ) > 0)
    {
       for ($k=0; $k<sql_num_rows( $result ); $k++)
       {
           // Well in the case of comments the story id is really the comment id
           list($t_story_id, $t_name, $t_reason) = sql_fetch_row($result, $dbconn);
	   // echo("Setting id " . $t_story_id ." to " .$t_reason."<BR>");
	   if ($t_story_id != null ) {
		if (isset($reasons_array[$t_story_id])) $reasons_array[$t_story_id] .= "<BR>".$t_name . ": " .$t_reason;
		else $reasons_array[$t_story_id] = $t_name . ": " .$t_reason;
	   }
       }
    }

    return $reasons_array;
}
function reportInfo($info_message)
{
    reportError($info_message, true);
}
function reportError($error_message, $info_only = false)
{
   global $system_config, $graphics_store;
   if($system_config->user_error_reporting >= 2)
   {
      if($info_only == true)
      {
         echo("<p align=center><B>". $error_message."</B></p>");
      }
      else
      {
         ?>
         <p align=center>
         <img src="<?=$graphics_store?>warning.gif" align=middle><BR><BR>
         <B><?=$error_message?></B><BR>
         <?
         if($system_config->sql_query_caching_enabled)
         {
            ?><small>(cached errors will attempt auto recovery within <?=$system_config->sql_query_caching_healing_timeout/60?> minutes)</small><?
         }
         ?>
         <BR><BR>
         The site may be undergoing difficulties due to heavy load please try again later!
         </p>
         <?
      }
   }
   elseif($system_config->user_error_reporting > 0 && $info_only == false)
   {
      writeFailureMessage("Internal Error", "The site may be undergoing difficulties due to heavy load please try again later!");
   }

   if($system_config->user_error_reporting >= 2 || $info_only == false) logMessage($error_message,"sitelog.txt");
}


function writeFailureMessage($act_txt, $msg)
{
   global $graphics_store, $suppress_page_insets;
   $suppress_page_insets = true;
   $icon = "<p align='center'><img src='$graphics_store"."warning.gif' alt='warning' style='border: #000 1pt solid'></p>";
   echo "<div class='user-message'>\n";
   echo $icon;
   echo "<h3 align='center' class='publish-header'>$act_txt</h3>\n";
   echo "<p>$msg</p>\n";
   echo "</div>\n<br>";
}

function logSecurityMessage($message)
{
   global $editor_session, $system_config;
   if($editor_session->isSessionOpen())
   {
      $message = $editor_session->editor->editor_name.":\t".$message;
   }
   else
   {
      $message = "Unknown User:\t".$message;
   }
   if($system_config->security_ip_recording >= $system_config->security_recording_level)
   {
      $message = $_SERVER['REMOTE_ADDR'].":\t".$message;
   }
   else
   {
      $message = "IP not recorded:\t".$message;
   }
   logMessage($message, "securitylog.txt");
}
function logSpammerMessage($message, $logFilename="spamlog.txt", $normal_mode = true)
{
   global $system_config;

   if($system_config->security_ip_recording >= $system_config->security_recording_level)
   {
      $ip_string = $_SERVER['REMOTE_ADDR'];
   }
   else
   {
      $ip_string = "IP not recorded";
   }

   // In normal mode it gets time stamp. The other mode is a special formatted mode and timestamp
   // is already in it.
   if ($normal_mode == true) {
       $message = $ip_string . ":\t" .$message;
       logMessage($message, $logFilename);
   } else {
       // Since message is formatted, it is hard to know whether to stick the IP too or not.
       logEmail($message,$logFilename);
   }
   // logMessage($message, "spamlog.txt");
}

function logMessage($message,$file, $add_timestamp=true)
{
   global $system_config,$path_prefix;
   if($system_config->logging_enabled==false) return;
   $file = $path_prefix.$system_config->log_store.$file;

   if ($add_timestamp == true)
       $OUTPUT = "\r\n".date("D, d M Y H:i:s",(time()+$system_config->timezone_offset))."\r\n".$message."\r\n";
   else
       $OUTPUT = $message."\r\n";
   $fp = fopen($file,"a"); // open file with append permission
   fputs($fp, $OUTPUT);
   fclose($fp);
}

function logEmail($message,$file)
{
   global $system_config,$path_prefix;
   if($system_config->logging_enabled==false) return;
   $file = $path_prefix.$system_config->log_store.$file;
   $fp = fopen($file,"a"); // open file with append permission
   fputs($fp, $message);
   fclose($fp);
}


/************************************************************************/
/* Filtering out nasties                                                */
/************************************************************************/
function cleanseTitleField($field)
{
   $c_field = strip_tags(trim($field));
   $nasties = '/[,;"\']/';
   $replace = '';
   return preg_replace($nasties, $replace, $c_field);
}

function cleanseIP($ip)
{
   if(preg_match("/(\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3})/", $ip, $matches))
      return $matches[0];
   return false;
}

function cleanseNumericalQueryField($field, $min=0)
{
   // removes any non numeric chars which may be an attempt to
   // insert malicious sql code
   $field = preg_replace("|[^0-9](.*)|","",$field);
   if($field < $min or (!is_numeric($field)))
   {
      $field = $min;
   }
   return $field;
}

function cleanseFormFieldFiller($txt)
{
    return str_replace(array("'","\"","& ", "<", ">"),array("&#039;","&quot;","&amp; ", "&lt;", "&gt;"), $txt);
}






/************************************************************************/
/* Finding the object cache files                                       */
/************************************************************************/
function getObjectCacheDir($id)
{
   global $system_config;
   $cacheroot = $system_config->html_cache_store;
   $directory_prefix = "objectstore";
   $directory_num = (int)($id / $system_config->objects_per_cache_directory);
   $cache_dir = $cacheroot.$directory_prefix."_".$directory_num."/";
   return $cache_dir;
}

function getObjectCacheIndexFile($id)
{
   $dir = getObjectCacheDir($id);
   $f = $id.'.inc';
   return $dir.$f;
}

/************************************************************************/
/* One day all the admin pages will be a nice object hierarchy.         */
/* For now they have to make do with common functions for page drawing  */
/************************************************************************/
function writeAdminSectionHeader($title, $msg, $cols)
{
   echo "<TR><TH class=admin colspan=$cols><big>$title</big><br /><small>$msg</small></TH></TR>\n";
}

function writeAdminPageFooter()
{
   echo "</div>";
}

function writeAdminPageHeader($title, $context, $intro, $filter="")
{
global $userLanguage;

   if ( isset($userLanguage) && $userLanguage->getLanguageCodePrefix() != "en")
   {
      $title .= " (".$userLanguage->getLanguageCodePrefix().")";
   }

   ?>
   <div class="sitebuilder-header">
      <h1 class="sitebuilder-header"> <?=$title?> </h1>
      <P class='sitebuilder-intro'><?=$intro?></P>
   </div>
   <?
   if($filter != ""){
      ?>
      <div class='sitebuilder-filter'><?=$filter?></div>
      <?
   }
   echo "<div class='sitebuilder-contents'>\n";
   if($context != "")
   {
      echo $context;
   }
}


function writeMenuHeader()
{
?>
   <table class="admin">
<?
}

function writeMenuSpacer()
{
?>
   <div class="object-listing-spacer">&nbsp;</div>
<?
}

function writeMenuFooter($cols, $show_logo=true, $filling = "")
{
   if($show_logo)
   {
   ?>
      <tr class="admin"><th colspan=<?=$cols?> class="admin"><?=$filling?><img class="right" src="graphics/adminicon.gif">&nbsp;</th></tr>
      </table>
   <?
   }
   else
   {
   ?>
      <tr class="admin"><th colspan=<?=$cols?> class="admin"><?=$filling?></th></tr>
      </table>
   <?
   }
}

function writeMenuSectionHeader($title, $msg, $cols)
{
   echo "<TR><TH class=admin colspan=$cols><big>$title</big><br /><small>$msg</small></TH></TR>\n";
}

function op_r($thing)
{
   echo "<PRE>";
   print_r($thing);
   echo "</PRE>";
}


function writeAdminHeader($extra_url="", $extra_urlmsg="",$extraUrlArray = null)
{
   global $OSCAILT_SCRIPT, $system_config;
   require_once("objects/indyobjects/indyobject.inc");
   require_once("objects/indyobjects/indyitemset.inc");
   $uni_options = new indyItemSet();
   $page_parts = explode('.', $OSCAILT_SCRIPT);
   $item_type = $page_parts[0];
   $admin_lnk = '<a class="admin-context" href="admin.php">';
   $my_lnk = "<a class='admin-context' href='$OSCAILT_SCRIPT'>";

   if($uni_options->load($system_config->xml_store, "universal_config_options") !== false)
   {
      $admin_str = $uni_options->getMeta('title');
      $item = $uni_options->getItemByName($item_type);
      if($item !== false)
      {
         $title = $item->getMeta('title');
         $intro = $item->getMeta('description');
         // $intro .= "<p align='center'> " . strftime($system_config->default_strftime_format, time() + $system_config->timezone_offset) . "</p>";
         //$intro .= $item->getMeta('description');
         //$icon = $item->getMeta('icon');
      }
   }
   if(!isset($admin_str))
   {
      $admin_str = "Oscailt Administration Home";
   }
   if(!isset($title))
   {
      $title = "Oscailt Configuration";
      $intro = "Choose Your Settings from the options below";
   }
   $contexts = array();
   $contexts[] = $admin_lnk.$admin_str."</a>\n";
   $contexts[] = $my_lnk.$title."</a>\n";
   if ( strlen($extra_url) > 0 )
   {
       $contexts[] = "<a href=\"" . $extra_url . "\">" . $extra_urlmsg ."</a>\n";
   }
   if ($extraUrlArray != null) {
      foreach($extraUrlArray as $urlAddr => $urlName)
      {
         $contexts[] = "<a href=\"" . $urlAddr . "\">" . $urlName ."</a>\n";
      }
   }

   $context = getAdminContextHTML($contexts);
   writeAdminPageHeader($title, $context, $intro);

}

function getAdminContextHTML(&$contexts)
{
   $ctxt_str = "";
   if(count($contexts > 0))
   {
      $ctxt_str = "<UL class='context'>\n";
      foreach($contexts as $ctxt)
      {
         $ctxt_str .= "<LI class='context'>$ctxt</LI>\n";
      }
      $ctxt_str .= "</UL>\n";
   }
   return $ctxt_str;
}


/************************************************************************/
/* Front Line Security - checks for banned ips and referrers.           */
/************************************************************************/
function isRequestBanned()
{
   global $system_config;
   $ref = isset($_SERVER['HTTP_REFERER']) ? $_SERVER['HTTP_REFERER']: "";
   $ip = $_SERVER['REMOTE_ADDR'];
   //do various matches and either
   $banned_ips = $system_config->banned_ips;
   $banned_referers = $system_config->banned_referers;
   if(strpos($banned_ips, $ip) !== false) return true;
   if(empty($ref)) return false;
   // get host name from URL
   preg_match("/^(\w{3,}:\/\/)?([^\/]+)/i",$ref, $matches);
   if(!isset($matches[2])) return false;
   $host = $matches[2];
   // get last two segments of host name
   preg_match("/[^\.\/]+\.[^\.\/]+$/", $host, $matches);
   if(!isset($matches[0])) return false;
   $domain = $matches[0];
   if(strpos($banned_referers, $domain) !== false)
   {
      return true;
   }
   return false;
}
function isRequestBannedBot()
{
   global $system_config;

   $ip = $_SERVER['REMOTE_ADDR'];
   // Check the spambot IP list
   //$banned_referers = $system_config->banned_referers;
   //$system_config->spam_ip_list = "127.0.0.3 127.0.0.2";

   if(strlen($system_config->spam_ip_list) == 0) return false;

   if(strpos($system_config->spam_ip_list, $ip) === false) return false;

   return false;
}

function redirectBannedRequest($status_code = 0)
{
   global $system_config;
   //maybe load up some type of mapping here
   if ($status_code == 0) {
       $redirect_url = $system_config->redirect_banned_url;
       if ($system_config->security_recording_level >= 1)
           logMessage("Banned IP ".$_SERVER['REMOTE_ADDR']. " redirect to ".$redirect_url,"sitelog.txt");
       SharedMemoryUpdate("SpamRedirect");
   } else {
       if ($system_config->security_recording_level >= 1)
          logMessage("Banned Bot IP ".$_SERVER['REMOTE_ADDR']. " returned status code: ".$status_code,"sitelog.txt");
       SharedMemoryUpdate("SpamBotBan");
   }
   disconnectFromDatabase();
   RemoveSemaphoreId();

   if ($status_code == 0) {
      sleep(1);
      Header("Location: ".$redirect_url);
   } else {
      header("HTTP/1.0 ".$status_code ." Not Found");
      header("Status: ".$status_code ." Not Found");
   }
   exit;

}


/************************************************************************/
/* Maps URL to object id and does some request rewriting                */
/************************************************************************/
function getRequestTargetObjectID()
{
   global $redirectList, $system_config, $pseudo_directory_offset, $graphics_store;

   $obj_id = null;
   // If we receive a very long request then it is probably a spammer so log it.
   if (strlen($_SERVER['REQUEST_URI']) > $system_config->max_uri_suspect_spam )
   {
       $msg = "Likely spammer request: Length = " . strlen($_SERVER['REQUEST_URI']);
       $msg .= ". 1st 150 bytes of REQUEST_URI: " . substr($_SERVER['REQUEST_URI'],0,150);
       logSpammerMessage($msg); 
   }
   if(isset($_REQUEST['obj_id']))
   {
      $obj_id = $_REQUEST['obj_id'];
      // If it has ../../ then default to front page.
      if (strstr($obj_id,"../") !== FALSE) {
          $obj_id = getFrontPageID();
          $_REQUEST['obj_id'] = $obj_id;
      }
   }
   if($system_config->support_v1_url_remaps && isset($_REQUEST['id']) && !isset($_REQUEST['story_id']))
   {
      $_REQUEST['story_id'] = cleanseNumericalQueryField($_REQUEST['id']);
   }
   if(isset($_REQUEST['redirect']))
   {
      if(strlen($_SERVER['REQUEST_URI']) > $system_config->max_uri_redirect_spam )
      {
          $obj_id = 10202; // indicates to caller to return empty page
          $_REQUEST['obj_id'] = $obj_id;
          SharedMemoryUpdate("SpamMaxUri");
      }
      else
      {
         $fields = explode("/", $_REQUEST['redirect']);
         $pseudo_directories = count($fields) - 1;

         for($i = 0; $i < $pseudo_directories; $i++)
         {
            $pseudo_directory_offset = "../" . $pseudo_directory_offset;
         }
         if($system_config->support_v1_url_remaps and $fields[0] == 'cgi-bin')
         {
            array_shift($fields);
         }

         if($obj_id == null)
         {
            foreach($redirectList->redirects as $r)
            {
               if($r->isSynonym($fields))
               {
                  $obj_id = $r->id;
                  break;
               }
            }
         }
         if($obj_id == null)
         {
            mapPathToFilters($fields);
         }
      }
   }
   else
   {
      // This catches the case where it is not a redirect. Will tidy up later.
      if(strlen($_SERVER['REQUEST_URI']) > $system_config->max_uri_redirect_spam )
      {
          $obj_id = 10203; // indicates to caller to return empty page
          $_REQUEST['obj_id'] = $obj_id;
          SharedMemoryUpdate("SpamMaxUri");
      }
   }

   if($obj_id == null)
   {
      // And if it is still NULL, then the entry was not found in the friendly URLs so we can use
      // a more restritive criteria if we want to send back a less CPU intensive page.
      if(strlen($_SERVER['REQUEST_URI']) > $system_config->max_uri_redirect_spam )
      {
          $obj_id = 10204; // indicates to caller to return empty page
          $_REQUEST['obj_id'] = $obj_id;
      } else {
          $obj_id = getFrontPageID();
          $_REQUEST['obj_id'] = $obj_id;
      }
   }
   $graphics_store = $pseudo_directory_offset.$graphics_store;
   return $obj_id;
}

function getFrontPageID()
{
   global $system_config;
   $id = 0;
   if(isset($_REQUEST['user_frontpage']))
   {
      $id=cleanseNumericalQueryField($_REQUEST['user_frontpage']);
   }
   if($id <= 0)
   {
      $id = $system_config->front_page_id;
   }
   return $id;
}

function getHtmlCachePage()
{
    // Need to handle preferences to get correct page.
    global $system_config, $userLanguage;
    $lang_code = "_".$userLanguage->getLanguageCodePrefix();

    $t_query_string= str_replace("redirect=","",$_SERVER['QUERY_STRING'] );
    if (strlen(TRIM($_SERVER['QUERY_STRING'])) == 0) $t_query_string= "index";
    //$t_query_string .= $lang_code;
    $t_string = "";
    if ($t_query_string == "comments") {
         if (isset($_REQUEST['show_all_comments']) && $_REQUEST['show_all_comments'] == 'true') {
            $t_string = "_sac";
         } else {
            $t_string = "_no_sac";
         } 
         if (isset($_REQUEST['show_comment_titles']) && $_REQUEST['show_comment_titles'] == 'true') {
            $t_string .= "_sct";
         } else {
            $t_string .= "_no_sct";
         } 
    } elseif ($t_query_string == "events") {
	 if (isset($_REQUEST['event_display'])) {
	     if ($_REQUEST['event_display'] == 'weekly') {
                 $t_string = "_wk";
	     } elseif ($_REQUEST['event_display'] == 'biweekly') {
                 $t_string = "_biwk";
	     } else {
                 $t_string = "_list";
	     }
         } else {
            $t_string = "_list";
         } 
    }

    $t_query_string .= $lang_code . $t_string;

    return $system_config->html_cache_store. $t_query_string.".html";
}
function getHtmlCacheArray()
{
    return array("openwire", "archives", "comments", "newswire", "features", "events", "gallery", "about_us", "other_press", "breaking_news" );
}
function isPageHtmlCached($target_indyobject_id)
{
    // Should check caching is enabled before calling this.
    //echo "query [".$_SERVER['QUERY_STRING'] . "]";
    if (strlen($_SERVER['QUERY_STRING']) == 0 && $target_indyobject_id == getFrontPageID() ) {
        return true;
    }
    // If set then do not cache either
    if ( isset($_REQUEST['fontsizeinc']) && $_REQUEST['fontsizeinc'] != 0) return false;
    
    // .htaccess file will redirect most URLs into the form: redirect=xxxx
    $t_query_string= str_replace("redirect=","",$_SERVER['QUERY_STRING'] );

    if (in_array($t_query_string, getHtmlCacheArray()) ) {
        return true;
    }

    return false;

}
function clearHtmlCache($reportinfo=false)
{
    global $system_config;

    foreach (getHtmlCacheArray() as $each_file ) {

        $t_path = $system_config->html_cache_store. $each_file."*.html";
        foreach (glob($t_path) as $t_file) {
            if (!is_dir($t_file)) {
                if ($reportinfo == true) echo("Deleting file: ".$t_file ."<BR>");
                unlink($t_file);
            }
        }
    }

}


function mapPathToFilters(&$paths)
{
   global $userLanguage, $pageFilterText, $pageType, $pageTopic, $pageLanguage, $pageRegion, $regionList, $typeList, $languageList, $topicList;
   foreach($paths as $extra)
   {
      if($extra == "") continue;
      if($pageType!=null && $pageRegion!=null && $pageTopic!=null && $userLanguage!=null) break;

      //if($extra == 'mayday2004') $extra = 'summitmobilisations';//hack for ie topic rename
      if($pageRegion==null)
      {
	 if (isset($userLanguage)) {
             $pageRegion = $regionList->getRegionByName($extra,$userLanguage->language_id);
             // if it is null then maybe it is a translated language so look it up that way
             // Should be able to turn this 2nd level lookup on and off because it may add a
             // fair amount of overhead
             if ($pageRegion == null) {
                 $pageRegion = $regionList->getRegionByAnyLangName($extra);
             }
	 } else {
             $pageRegion = $regionList->getRegionByName($extra);
	 }
         if(is_object($pageRegion)) $_REQUEST['region_id'] = $pageRegion->region_id;
      }

      if($pageTopic==null)
      {
	 if (isset($userLanguage)) {
             $pageTopic = $topicList->getTopicByName($extra,$userLanguage->language_id);
             // if it is null then maybe it is a translated language so look it up that way
             // Should be able to turn this 2nd level lookup on and off because it may add a
             // fair amount of overhead
             if ($pageTopic == null) {
                 $pageTopic = $topicList->getTopicByAnyLangName($extra);
             }
	 } else {
             $pageTopic = $topicList->getTopicByName($extra);
	 }
	 if(is_object($pageTopic)) $_REQUEST['topic_id'] = $pageTopic->topic_id;
      }
      if($pageLanguage==null)
      {
         $pageLanguage = $languageList->getLanguageByName($extra);
	 if(is_object($pageLanguage)) $_REQUEST['language_id'] = $pageLanguage->language_id;
      }
      if($userLanguage==null)
      {
         $userLanguage = $languageList->getLanguageByCode($extra);
	 if(is_object($userLanguage)) $_REQUEST['language_id'] = $userLanguage->language_id;
      }

      if($pageType==null)
      {
	 if (isset($userLanguage)) {
             $pageType = $typeList->getTypeByName($extra,$userLanguage->language_id);
             // if it is null then maybe it is a translated language so look it up that way
             // Should be able to turn this 2nd level lookup on and off because it may add a
             // fair amount of overhead
             if ($pageType == null) {
                 $pageType = $typeList->getTypeByAnyLangName($extra);
             }
	 } else {
             $pageType = $typeList->getTypeByName($extra);
	 }
	 if(is_object($pageType)) $_REQUEST['type_id'] = $pageType->type_id;
      }
      $pageFilterText = "";

      makePageFilterText();
   }
}

function makePageFilterText()
{
   global $pageRegion, $pageTopic, $pageType, $pageLanguage, $pageFilterText;
   if(isset($pageRegion)) $pageFilterText=$pageRegion->region_name. " / ";
   if(isset($pageTopic)) $pageFilterText=$pageFilterText.$pageTopic->topic_name." / ";
   if(isset($pageType)) $pageFilterText=$pageFilterText.$pageType->type_name." / ";
   if(isset($pageLanguage)) $pageFilterText=$pageFilterText.$pageLanguage->language_name." / ";
}

/************************************************************************/
/* Some basic data validation functions for common usages               */
/************************************************************************/
function isValidEmailAddress($addy)
{
   if (empty($addy)) return false;
   if (!preg_match('/^[a-zA-Z0-9&\'\.\-_\+]+\@[a-zA-Z0-9\.-]+\.+[a-zA-Z]{2,6}$/', $addy))
   {
      return false;
   }
   return true;
}

function isValidURL($url)
{
   $url = trim($url);
   if(trim($url) == "") return false;
   $matches = array();
   //echo("Testing URL [".$url."]<BR>");
   // if(preg_match("!((itms|http|https|ftp):/{2}[\w]{2,}\.[\w\.]{2,}[/\w\-\.\,\;\+\:\~\?\&\=\#\%]*[/\w\-\?\&\=\#]+)!e", $url, $matches)) {
      #                                 http://ucl-saguenay.blogspot.com/
   if(preg_match("!((itms|http|https|ftp):/{2}[\w\-]{2,}\.[\w\-\.]{2,}[/\w\-\.\,\;\+\:\~\?\&\=\#\%]*[/\w\-\?\&\=\#]+)!e", $url, $matches)) {
      //echo("returning URL [".$matches[0]."]<BR>");
      return $matches[0];
   }
   return false;
}

function getURLLinkString($url, $cls_str, $txt="")
{
   if(!($url = isValidURL($url))) return "";
   if(trim($txt) == "")
   {
      return getDisplayableURL($url);
   }
   else return "<A $cls_str HREF='".htmlspecialchars($url, ENT_QUOTES)."'>$txt</A>";
   //return $str;
}

function get_shortened_tag($url,$chr_limit = 30,$add = '...')
{
   if($chr_limit <= 0) $chr_limit = 30;
   $offset = 5;
   $url = str_replace("&amp;", "&", $url);
   return preg_replace("!((itms|http|https|ftp):/{2}[\w\-]{2,}\.[\w\-\.]{2,}[/\w\-\%\.\?\&\,\;\+\:\~\=\#]*[/\w\-\?\&\=\#]+)!e", "'<a href=\"'.htmlspecialchars('\\1', ENT_QUOTES).'\" title=\"'.htmlspecialchars('\\1', ENT_QUOTES).'\">'.(strlen('\\1')>=$chr_limit ? substr('\\1',0,$chr_limit-$offset).'$add'.substr('\\1', -$offset):'\\1').'</a>'", $url);
}

function getDisplayableURL($url)
{
   global $system_config;
   return get_shortened_tag($url, $system_config->max_unshortened_tag_length);
}

function makeStringOkForFilenames($str)
{
   return preg_replace("[^a-z0-9._]", "", str_replace(" ", "_", str_replace("%20", "_", strtolower(trim($str)))));
}

/************************************************************************/
/* Common image manipulation functions                                  */
/************************************************************************/
function writeFeaturedPhoto($id, $h, $w, $fs, $force_size, $show_caption)
{
   global $system_config, $pseudo_directory_offset, $page_style, $redirectList, $OSCAILT_SCRIPT;
   require_once("objects/attachment.inc");
   require_once("objects/story.inc");
   //$image_thumbnail = $system_config->newswire_bar_thumbnail;
   //writeFeaturedPhoto("'.$nw_id.'", "'.$rheight.'", "'.$rwidth.'", "'.$maxsize.'", '.$forcesize.', '.$show_caption.');
   if($fs == 0) $fs = 1000000;
   $image_thumbnail = new ImageConfig($w, $h,$force_size,$force_size,$fs,array(0xFF, 0xFF, 0xFF),0,0,0,0,0);
   $featured_image = $system_config->featured_image;
   if($featured_image)
   {
      $image = new Attachment();
      $image->attachment_id=$featured_image;
      $image->load();
      $image_info = $image->getTransformedImageURL($image_thumbnail);
      if(!$image_info) return;
      $image_URL = $pseudo_directory_offset.htmlspecialchars($image_info[0]);
      if($system_config->use_friendly_urls)
      {
         $red = $redirectList->getRedirect($id);
         if($red !== false)
         {
            $prim = $red->getPrimaryRedirect();
            if($prim !== false)
            {
               $image_link = $pseudo_directory_offset.$prim.'/'.$image->story_id;
            }
         }
      }
      if(!$system_config->use_friendly_urls or $red === false or $prim === false)
      {
         $image_link= $pseudo_directory_offset.$OSCAILT_SCRIPT."?obj_id=$id&amp;story_id=".$image->story_id;
      }
      if(strlen($image->description)>0) $caption=$image->description;
      else
      {
         $story = new Story();
         $story->story_id=$image->story_id;
         $story->load();
         $caption=$story->story_title;
      }
      $img = "<img class='$page_style"."-thumbnail' ".$image_info[2]." src='$image_URL' alt='".htmlspecialchars($caption)."' title='".htmlspecialchars($caption)."' />";
      echo "<a class='$page_style' href='$image_link'>$img</a>";
      if($show_caption)
      {
         $caption_cls_str = "class='$page_style"."-thumbnail-caption'";
         echo "<P $caption_cls_str>$caption</P>";
      }
    }
}

function getResizedImageURL($url, $rheight, $rwidth, $forcesize, $maxfsize, $add_pseudo=true)
{
   global $system_config, $pseudo_directory_offset;
   if($rheight == 0) $forceheight = false;
   else $forceheight = $forcesize;
   if($rwidth == 0) $forcewidth = false;
   else $forcewidth = $forcesize;
   if($maxfsize == 0) $maxfsize = 2048;
   $imageConfig = new ImageConfig($rheight,$rwidth,$forceheight,$forcewidth,$maxfsize,array(0xFF, 0xFF, 0xFF),0,0,0,0,0);
   $tool = new ImageTool();
   $url_info = $tool->getTransformedImageURL($url,$imageConfig);
   if($add_pseudo) return $pseudo_directory_offset.$url_info[0];
   else return $url_info[0];
}

/************************************************************************/
/* User Preference Controls with universal application                  */
/*               (preference object will hold these soon                */
/************************************************************************/
function getFontSizeControl($cls_str)
{
   $link=$_SERVER['REQUEST_URI'];
   $link = preg_replace("/(\?|&)fontsizeinc=[0-9]*/","",$link);
   if(stristr($link,"?")!=false) $link=$link."&";
   else $link=$link."?";
   $link = htmlspecialchars($link, ENT_QUOTES);
   echo "<small>";
   echo "[ text size ";
   if(isset( $_REQUEST['fontsizeinc'] ) && $_REQUEST['fontsizeinc']>2)
   {
      echo "<A $cls_str href='$link"."fontsizeinc=0'>normal</A>&nbsp;|&nbsp;";
      echo "<A $cls_str href='$link"."fontsizeinc=".($_REQUEST['fontsizeinc']-2)."'>&lt;&lt;</A>&nbsp;|&nbsp;";
   }
   else if(isset( $_REQUEST['fontsizeinc'] ) && $_REQUEST['fontsizeinc']>0)
   {
      echo "<A $cls_str href='$link"."fontsizeinc=".($_REQUEST['fontsizeinc']-2)."'>normal</A>&nbsp;|&nbsp;";
   }
   if(isset($_REQUEST['fontsizeinc'])) $bigger = $_REQUEST['fontsizeinc'] + 2;
   else $bigger = 2;
   echo "<A $cls_str href='$link"."fontsizeinc=".$bigger."'>&gt;&gt;</A> ]";
   echo "</small>";
}

function getUserFPControl($cls_str)
{
   global $oscailt_page_setting;
   $page_id = $oscailt_page_setting['obj_id'];
   $link=$_SERVER['REQUEST_URI'];
   $link = preg_replace("/(\?|&)fontsizeinc=[0-9]*/","",$link);
   $link = preg_replace("/(\?|&)userlanguage=[a-z]{2}/","",$link);
   if(stristr($link,"?")!=false) $link=$link."&";
   else $link=$link."?";
   $link = htmlspecialchars($link, ENT_QUOTES);
   $my_link = $link."user_frontpage=$page_id&amp;save_prefs=true";
   echo "<small>";
   echo "<a $cls_str href='$my_link'>make this your home page</a>";
}

function addLinkParameters(&$lnk, $params)
{
   if(count($params) <= 0) return $lnk;
   if(!is_array($params)) $params = array($params);
   $extra = "";
   $x = strrchr($lnk, '?');
   if($x === false)
   {
      $extra = '?' . implode("&", $params);
   }
   else
   {
      $extra = '&' . implode("&", $params);
   }

   $lnk .= htmlspecialchars($extra, ENT_QUOTES);
   return $lnk;
}


function getUserLanguageControl($cls_str)
{
   global $languageList, $userLanguage;
   $langs = $languageList->getfLanguages();
   if(count($langs) <= 1) return;
   $link=$_SERVER['REQUEST_URI'];
   $link = preg_replace("/(\?|&)fontsizeinc=[0-9]*/","",$link);
   $link = preg_replace("/(\?|&)userlanguage=[a-z]{2}/","",$link);
   if(stristr($link,"?")!=false) $link=$link."&";
   else $link=$link."?";
   $link = htmlspecialchars($link, ENT_QUOTES);
   echo "<small>";
   echo "[ Language ";
   $first = false;
   foreach($langs as $l)
   {
      $my_link = $link;
      if(!$first)   $first = true;
      else echo " | ";
      if(compareToUserLanguage($l->getLanguageCodePrefix()))
      {
         echo "<b $cls_str>".$l->getLanguageCodePrefix()."</b>";
      }
      else
      {
         $my_link.="userlanguage=".$l->getLanguageCodePrefix();
         echo "<a $cls_str href='$my_link'>".$l->getLanguageCodePrefix()."</a>";
      }
   }
   $my_link = $link."userlanguage=".$userLanguage->getLanguageCodePrefix()."&amp;save_prefs=true";
   echo " | <a $cls_str href='$my_link'>save preference</a> ]";
   echo "</small>";
}

function getUserPreferenceControls()
{
   global $page_style;
   $cls_str = "class='$page_style'";
   getUserLanguageControl($cls_str);
   echo " ";
   getUserFPControl($cls_str);
   echo " ";
   getFontSizeControl($cls_str);

}

//Two unfortunate things.
//1. setting locale may not be thread safe
//         so if you are experiencing random and weird changes of language
//         you might want to disable this or use some sort of locking mechanism
//2. there is no universal standard for locale file names (well none accepted by windows),
//       so you may have to try a good few to get a hit.
function setClosestLocale()
{
   global $userLanguage;

   $locales_to_try = array();
   $lcode = $userLanguage->language_code;
   $locales_to_try[] = $lcode;
   $locales_to_try[] = $lcode.'.ISO_8859-1';
   $prefix = $userLanguage->getLanguageCodePrefix();
   $suffix = $userLanguage->getLanguageCodeSuffix();
   $alternative_lcode = $prefix.'_'.$suffix;
   $locales_to_try[] = $alternative_lcode ;
   $locales_to_try[] = $alternative_lcode.'.ISO_8859-1';
   if($suffix != "") $locales_to_try[] = $prefix;
   if($suffix != "") $locales_to_try[] = $suffix;
   foreach($locales_to_try as $loc)
   {
      if(setlocale(LC_ALL, $loc) !== false)
      {
         return;
      }
   }
   //failed based on configured language, now try based on prefix alone
   if($prefix == 'en')
   {
      $locales_to_try[] = 'english';
      $locales_to_try[] = 'English';
      $locales_to_try[] = 'en_UK';
      $locales_to_try[] = 'en_US';
      $locales_to_try[] = 'en-UK';
      $locales_to_try[] = 'en-US';
      $locales_to_try[] = 'en_UK.ISO_8859-1';
      $locales_to_try[] = 'en_US.ISO_8859-1';
      $locales_to_try[] = 'eng_UK';
      $locales_to_try[] = 'eng_US';
   }
   elseif($prefix == 'fr')
   {
      $locales_to_try[] = 'french';
      $locales_to_try[] = 'French';
      $locales_to_try[] = 'fr_FR';
      $locales_to_try[] = 'fra_FRA';
      $locales_to_try[] = 'fr_FR.ISO_8859-1';
      $locales_to_try[] = 'france';
      $locales_to_try[] = 'France';
   }
   elseif($prefix == 'it')
   {
      $locales_to_try[] = 'italian';
      $locales_to_try[] = 'Italian';
      $locales_to_try[] = 'it_IT';
      $locales_to_try[] = 'it_IT.ISO_8859-1';
      $locales_to_try[] = 'ita_ITA';
      $locales_to_try[] = 'italy';
      $locales_to_try[] = 'Italy';
   }
   elseif($prefix == 'de')
   {
      $locales_to_try[] = 'de_DE';
      $locales_to_try[] = 'German';
      $locales_to_try[] = 'german';
      $locales_to_try[] = 'de-DE';
      $locales_to_try[] = 'ge';
      $locales_to_try[] = 'de_DE.ISO_8859-1';
   }
   elseif($prefix == 'es')
   {
      $locales_to_try[] = 'es_ES';
      $locales_to_try[] = 'Spanish';
      $locales_to_try[] = 'spanish';
      $locales_to_try[] = 'es-ES';
      $locales_to_try[] = 'es_ES.ISO_8859-1';
   }
   elseif($prefix == 'ga')
   {
      $locales_to_try[] = 'ga_IE';
      $locales_to_try[] = 'Irish';
      $locales_to_try[] = 'irish';
      $locales_to_try[] = 'ga-IE';
      $locales_to_try[] = 'ga_IE.ISO_8859-1';
   }
   //add in any other locales that you want to support
   foreach($locales_to_try as $loc)
   {
      if(setlocale(LC_ALL, $loc) !== false)
      {
         return;
      }
   }
}

function getHelpHTML($txt, $strip = true, $pop_up = false, $make_wide=false)
{
   global $graphics_store;
   $txt = trim($txt);
   if($txt == "") return "";
   $img="help_icon.gif";
   if($strip)
   {
      $txt = htmlspecialchars(trim($txt), ENT_QUOTES);
      $txt = nl2br($txt);
      $alt = $txt;
   }
   else
   {
      $alt = htmlspecialchars(strip_tags($txt), ENT_QUOTES);
   }

   $n = "ot".str_replace(" ", "-", rand().microtime());
   $extra_str = "";

   if($make_wide == true && $pop_up == true) $extra_str = 'position: absolute; width: 550px;';
   else if($pop_up) $extra_str = 'position: absolute; width: 400px;';

   $a_str = " <a href=\"#\" onclick='javascript:toggleHelp(\"$n\"); return false;'>";
   $img_str = "<img style='vertical-align: middle;' border=0 src='$graphics_store"."$img' alt='$alt' title='$alt' />";
   $img_str2 = "<img style='vertical-align: middle;' border=0 src='$graphics_store"."$img' alt='close help' title='close help' />";
   $html_str = $a_str.$img_str."</a> ";
   $hidden_str = "<div id='$n' class='help-popup' style='display: none; $extra_str'>";
   $hidden_str .= $a_str.$img_str2."</a> $txt <br /><small>$a_str"."hide help</a></small></div>";
   $html_str .= $hidden_str;
   return $html_str;
}

function writeStrftimeHelp()
{
   echo "locale sensitive strftime format: use the following codes (non-exhaustative list):
          %a - abbreviated weekday name  /  %A - full weekday name
          %b - abbreviated month name / %B - full month name
          %d - day of the month as a decimal number (range 01 to 31)
          %H - hour as a decimal number using a 24-hour clock (range 00 to 23)
          %m - month as a decimal number (range 01 to 12)
          %M - minute as a decimal number
          %S - second as a decimal number
          %y - year as a decimal number without a century / %Y - year as a decimal number including the century

          For example, the string %a %b %d, %Y %H:%M might produce Sun Jan 1, 2005 23:12";
}

function writeHelpJS()
{
   ?>
   <script type="text/javascript" language="javascript">
   <!--
      function toggleHelp(id)
      {
        dis = document.getElementById(id).style.display;
        if(dis == 'none')
        {
          document.getElementById(id).style.display = 'block';
        }
        else
        {
          document.getElementById(id).style.display = 'none';
        }
      }
   // -->
   </script>
   <?
}


function getAttachmentFileFromCacheFile($c_file)
{
   global $system_config, $pseudo_directory_offset;
   $ret = $c_file;
   if($pseudo_directory_offset != "" && strpos($ret, $pseudo_directory_offset) === 0)
   {
      $ret = substr($ret, strlen($pseudo_directory_offset));
   }
   if(strpos($ret, "http://".$system_config->site_url."/") === 0)
   {
      $ret = substr($ret, strlen("http://".$system_config->site_url."/"));
   }
   if(strpos($ret, $system_config->image_cache."/local/") === 0)
   {
      $ret = substr($ret, strlen($system_config->image_cache."/local/"));
   }
   else
   {
      return false;
   }
   $pathinfo = pathinfo($ret);
   $dir_bit = $pathinfo['dirname'];
   $base_bit = $pathinfo['basename'];
   $basename_we = substr($base_bit, 0, -(strlen($pathinfo['extension']) + ($pathinfo['extension'] == '' ? 0 : 1)));
   //remove the prefix bit from base bit..
   $parts = explode("_", $basename_we);
   //$parts = explode("_", $base_bit);
   if(count($parts) < 10) return false;
   $resolved_name = "";
   for($i = 10; $i<count($parts); $i++)
   {
      $resolved_name .= $parts[$i];
      if($i > 9 && $i<(count($parts)-1)) $resolved_name .= "_";
   }
   $fbase = $dir_bit."/".$resolved_name;
   if(file_exists($fbase.".jpg")) return $pseudo_directory_offset.$fbase.".jpg";
   elseif(file_exists($fbase.".gif")) return $pseudo_directory_offset.$fbase.".gif";
   elseif(file_exists($fbase.".png")) return $pseudo_directory_offset.$fbase.".png";
   elseif(file_exists($fbase.".bmp")) return $pseudo_directory_offset.$fbase.".bmp";
   else return false;
}

function debugOp($thing)
{
   ob_start();
   op_r($thing);
   $x = ob_get_contents();
   ob_end_clean();
   $f = "debug.html";
   $fh = fopen($f, "a");
   fputs($fh, $x);
   fclose($fh);
}

// This will return a string for an input time difference and format it as
// x hrs y min z secs ago. Currently used by view logs and editor status.
// The input time difference is an integer of seconds
function getTimeAgoString($time_diff, $trunc_secs =false)
{
	
   if ($time_diff < 60 )
   {
       $t_str = $time_diff . " secs ago";
   }
   elseif ($time_diff < 3600 )
   {
       // Less than one hour ago.
       $t_min = (int) $time_diff / 60;
       $t_min = floor($t_min);
       $t_sec = $time_diff - ( (int) (60 * $t_min));
       $t_str = $t_min . " mins " . $t_sec . " secs ago";
   }
   elseif ($time_diff < 86400 )
   {
       // Less than one day ago.
       $t_hr = (int) $time_diff / 3600;
       $t_hr = floor($t_hr);
       $time_diff_left = $time_diff - ($t_hr * 3600);

       $t_min = (int) $time_diff_left / 60;
       $t_min = floor($t_min);

       // If true, then do not add secs ago to the string as it is over 1 hr anyhow.
       if ($trunc_secs == true) { 
           $t_str = $t_hr . " hrs " . $t_min . " mins ago";
       } else {
           $t_sec = $time_diff_left - (60 * $t_min);
           $t_str = $t_hr . " hrs " . $t_min . " mins " . $t_sec . " secs ago";
       }
   }
   else
   {
       $t_day = (int) $time_diff / 86400;
       $t_day = floor($t_day);
       if ($t_day == 1 ) $t_str = $t_day . " day ago";
       else $t_str = $t_day . " days ago";
   }

   return $t_str;
}

function getMemUsage()
{
   if (function_exists('memory_get_usage')) {
       return memory_get_usage();
   }

   if (substr(PHP_OS,0,3) == 'WIN') {

       $output = array();
       exec('tasklist /FI "PID eq ' .getmypid() . '" /FO LIST', $output);
       return preg_replace('/[^0-9]/', '', $output[5] ) * 1024;
   }

   return 0;
}

// Debug function to dump the input array.
function DebugDumpArray($InputArray, $offset_str="")
{
   if ($InputArray == null ) {
       echo("Array is null <br>");
       return;
   }
   if (Count($InputArray) == 0 ) {
       echo("Array is empty <br>");
       return;
   }

   foreach ($InputArray as $t_key => $t_value)
   {
      if (!is_array($t_value)) {
          echo($offset_str."Key: ".$t_key. " value: ".$t_value ."<BR>");
      } else {
          DebugDumpArray($t_value,"--- ");
      }
   }
}

function getReminderDateSelect()
{
      global $system_config;

      $default_date = time()+$system_config->timezone_offset;

      $selected_min = date("i",$default_date);
      $selected_hr  = date("H",$default_date);
      $selected_day = date("j",$default_date);
      $selected_month = date("n",$default_date);
      $selected_year = date("Y",$default_date);
      $current_year = date("Y",time()+$system_config->timezone_offset);

      $str = '<select name="event_time_day">';
      $str .= "\n";

      for ($i=1; $i<=31; $i++)
      {
         $str .= '<option';
         if($i==$selected_day) $str .= " selected";
         $str.= ' value="'.$i.'">'.$i.'</option>';
      }
      $str .= "</select>\n";
      $str .= '<select name="event_time_month">';
      $str .= "\n";

      // Make it day 2 in mktime, to get over any complications with timezone etc
      for ($i=1; $i<=12; $i++)
      {
         $str .= "<option ";
         if($i==$selected_month) $str .= "selected ";
         $str .= "value='$i'>". strftime("%B", mktime (0,0,0,$i,2,$selected_year))."</option>\n";
      }
      $str .= "</select>\n";

      $str .= '<select name="event_time_year">';
      $str .= "\n";

      for ($i=$current_year; $i<=$current_year+1; $i++)
      {
          $str .= "<option ";
          if($i==$selected_year) $str .= "selected ";
          $str.= ' value="'.$i.'">'.$i.'</option>';
      }
      $str .= "</select>\n";
      $str .= "&nbsp;";

      // Add in the start time: hour and minutes. Make 00:00 the default and if not changed then ignore
      $start_time = '<select name="event_time_hr">';
      $start_time .= "\n";

      if ($selected_hr == 0 ) $start_time .= "<option selected value='-1'>Hour</option>";
      for ($i=0; $i <= 23; $i++)
      {
            $start_time .= "<option ";
	    $ihr = $i;
            if ($selected_hr > 0 ) {
                if($i==$selected_hr) $start_time .= "selected ";
            }
	    if ($ihr < 10 ) $ihr = "0" . $ihr;
            $start_time.= ' value="'.$i.'">'.$ihr.'</option>';
      }
      $start_time .= "</select>\n";

      $start_time .= '<select name="event_time_min">';
      $start_time .= "\n";

      if ($selected_min == 0 ) $start_time .= "<option selected value='-1'>Mins</option>";
      for ($i=0; $i < 12; $i++)
      {
            $start_time .= "<option ";
	    $imin = $i*5;
            if ($selected_min > 0 ) {
                if($selected_min == ($i*5)) $start_time .= "selected ";
            }
	    if ($imin < 10 ) $imin = "0" . $imin;
            $start_time.= ' value="'.$imin.'">'.$imin.'</option>';
      }
      $start_time .= "</select>\n";
      $str .= $start_time;

      return $str;
}

function getGoToHTML($DisplayData=false)
{
    global $system_config, $redirectList;
    // This is a lot of work but Italians use artico instead of article, so have to do the lookup just in case.
    // Note the redirects should already be loaded.
    $article_primary = "article";
    $redirectList->load();
    foreach($redirectList->redirects as $redirect)
    {
       if ($redirect->type  == "ArticleRedirector") {
           $article_primary = $redirect->getPrimaryRedirect();
	   break;
       }
    }
    $http_mode = isset($_SERVER['HTTPS']) ? "https" : "http";

    $t_html = '<FORM name="gotoform" action="'.$http_mode.'://'.$system_config->site_url.'/index.php" onsubmit="return changeActionDest()" method=POST>';
    $t_html .= '<small><input type=submit name=goto_btn value="Goto Article (id)"></small>';
    $t_html .= '<input type=text name=story_id value="" size=8 >';
    $t_html .= '</FORM>';

    $t_html .= '<script type="text/javascript" language="Javascript">';
    $t_html .= 'function changeActionDest()';
    $t_html .= '{';
    $t_html .= '    var new_location;';
    $t_html .= '    var t_str = document.gotoform.story_id.value;';
    $t_html .= '    t_str = t_str.replace(/^\s+/g, \'\');';
    $t_html .= '    document.gotoform.story_id.value=t_str;';
    $t_html .= '    new_location = "'.$http_mode.'://"+"'.$system_config->site_url.'"+"/'.$article_primary.'/"+t_str;';
    $t_html .= '    document.gotoform.action=new_location;';
    $t_html .= '    return true;';
    $t_html .= '}';
    $t_html .= ' </script>';

    if ($DisplayData == true) echo $t_html;
    else return $t_html;

}
function generatePassword()
{
      $t_password = "";
      for ($ipass = 0; $ipass < 8; $ipass++) {
	  $t_choice = rand(0,2);
	  if ($t_choice == 0 ) {
              $t_password .= chr(65+rand(0,25));
	  } else if ($t_choice == 1 ) {
              // lowercase
              $t_password .= chr(97+rand(0,25));
	  } else {
              $t_password .= rand(0,9);
	  }
      }
      return $t_password;
}

?>
