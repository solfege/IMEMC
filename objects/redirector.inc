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

class storyTitleCache
{
   var $id_map;
   var $title_map;

   function getTitleString($story_id)
   {
      return false;
   }

   function getStoryID($story_title)
   {
      return false;
   }
}

$storyTitleList = new storyTitleCache();

class Redirector
{
   var $id;
   var $names = array();
   var $type = "Redirector";

   function Redirector($id, $names){
      $this->id = $id;
      $this->names = $names;
   }

   function isSynonym(&$fields)
   {
      if(count($this->names) == 0) return false;
      if(in_array($fields[0], $this->names))
      {
         return true;
      }
      return false;
   }

   function getRedirectorString($cls)
   {
      $array_str = serialiseArrayParams($this->names);
      $str = '$redirects[] = new '.$cls.'('.$this->id.', '.$array_str.')';
      return $str;
   }

   function getPrimaryRedirect()
   {
      if(count($this->names) > 0)
      {
         return $this->names[0];
      }
      return false;
   }

   function getAdditionalSynonyms()
   {
      $syns = $this->names;
      if(count($syns) > 1)
      {
         array_shift($syns);
         return $syns;
      }
      return false;
   }

   function getSerialisedPHP()
   {
      $cls = "Redirector";
      return $this->getRedirectorString($cls);
   }

   function updateQuery($extra_str)
   {}
}


class ArticleRedirector extends Redirector
{
   function ArticleRedirector($id, $names)
   {
      $this->Redirector($id, $names);
      $this->type = "ArticleRedirector";
   }

   function getSerialisedPHP()
   {
      $cls = "ArticleRedirector";
      return $this->getRedirectorString($cls);
   }

   function isSynonym(&$fields)
   {
      if(parent::isSynonym($fields))
      {
         if(isset($_REQUEST['story_id']))
         {
            return true;
         }
         elseif(count($fields) > 1)
         {
            //assume it is the extra_str...
            $parts = explode("#", $fields[1]);
            $id = cleanseNumericalQueryField($parts[0]);
            if($id > 0)
            {
               $_REQUEST['story_id'] = $id;
               return true;
            }
            else
            {
               //look up the cache by title...
               $id = $this->getStoryIDFromTitle($parts[0]);
               if($id > 0)
               {
                  $_REQUEST['story_id'] = $id;
                  return true;
               }
            }
         }
      }
      return false;
   }

   function getStoryIDFromTitle($story_title)
   {
      global $storyTitleList;
      return $storyTitleList->getStoryID($story_title);
   }

   function getStoryTitleLink($story_id)
   {
      global $storyTitleList;
      return false;//$storyTitleList->getTitleString($story_id);
   }
}

class FeedRedirector extends Redirector
{
   function FilteredRedirector($id, $names)
   {
      $this->Redirector($id, $names);
      $this->type = "FeedRedirector";
   }

   function getSerialisedPHP()
   {
      $cls = "FeedRedirector";
      return $this->getRedirectorString($cls);
   }
}

class FilteredRedirector extends Redirector
{
   function FilteredRedirector($id, $names)
   {
      $this->Redirector($id, $names);
      $this->type = "FilteredRedirector";
   }

   function getSerialisedPHP()
   {
      $cls = "FilteredRedirector";
      return $this->getRedirectorString($cls);
   }

   function isSynonym(&$fields)
   {
      if(parent::isSynonym($fields))
      {
         array_shift($fields);
         mapPathToFilters($fields);
         return true;
      }
      return false;
   }
}

// This allows 2nd level directory matches but first must match too.
class DocumentRedirector extends Redirector
{
   var $level2_entries = array();

   function DocumentRedirector($id, $names, $second_level=null)
   {
      $this->Redirector($id, $names);
      $this->type = "DocumentRedirector";
      if ($second_level != null)
         $this->level2_entries = $second_level;
   }

   function getRedirectorString($cls)
   {
      $array_str = serialiseArrayParams($this->names);
      $dir_array_str = serialiseArrayParams($this->level2_entries);
      $str = '$redirects[] = new '.$cls.'('.$this->id.', '.$array_str.','.$dir_array_str.')';
      return $str;
   } 

   function getSerialisedPHP()
   {
      $cls = "DocumentRedirector";
      return $this->getRedirectorString($cls);
   }

   function isSynonym(&$fields)
   {
      if(parent::isSynonym($fields))
      {
          // If more parameters were passed then handle them.
          if(count($fields) == 1) return true;
          if(count($fields) > 3) return false;

	  // This is the 2nd field that we are matching on. Doing 3 levels would be much much harder.
	  // echo("trying to match ".$fields[0]."/".$fields[1]."<BR>");

          if(in_array($fields[1], array_keys($this->level2_entries)) )
          {
             $this->id= $this->level2_entries[$fields[1]];
             return true;
          }
          return false;
      }
      return false;
   }
}


class RedirectList
{
   var $redirects = array();

   function load($force=false,$f=null)
   {
      global $system_config;

      if ($force == true) $this->redirects = null;
      if(!$force && count($this->redirects) > 0) return;
      if($f === null)
      {
         $file_path = $system_config->redirection_cache_store;
      }
      else
      {
         $file_path = $f;
      }
      if(file_exists($file_path))
      {
	 // $infob = file($file_path);
	 // foreach($infob as $b) { echo("array elem ".$b."<br>"); }

         $fcontents = implode("", file($file_path));
         $redirects = array();
         ob_start();
         eval(' ?> '.trim($fcontents).' <?');
         ob_end_clean();
         if(count($redirects) > 0)
         {
             foreach($redirects as $r)
             {
                $this->redirects[$r->id] = $r;
             }
         }
      }
   }

   function save($f = null)
   {
      global $system_config;
      if($f === null)
      {
         $file = $system_config->redirection_cache_store;
      }
      else
      {
         $file = $f;
      }
      if(file_exists($file))
      {
         copy($file, $file.'.'.time());//important maybe?
      }
      $fp = fopen($file,"w");
      if (flock($fp, LOCK_EX))
      {
         fputs($fp, '<?php '."\n");
         foreach($this->redirects as $one_redirect)
         {
            // locking file
            if ($one_redirect != null)
                fputs($fp, $one_redirect->getSerialisedPHP().";\n");

         }
         fputs($fp, '?>'."\n");
         // unlocking file
         flock($fp, LOCK_UN);
         fclose($fp);
      }
      else echo "<P>failed to get lock on redirect cache file $file</p>";
   }

   function removeRedirect($id)
   {
      foreach(array_keys($this->redirects) as $k)
      {
         if($this->redirects[$k]->id == $id)
         {
            unset($this->redirects[$k]);
            return;
         }
      }
   }

   function add(&$red)
   {
      $this->redirects[$red->id] = $red;
   }

   function containsRedirectString($nm)
   {
      foreach($this->redirects as $red)
      {
         if(in_array($nm, $red->names))
         {
            return true;
         }
      }
      return false;
   }

   function getRedirect($id)
   {

      if(!isset($this->redirects[$id])) return false;
        return $this->redirects[$id];
      /*foreach($this->redirects as $red)
      {
         if($red->id == $id) return $red;
      }

      return false;*/
   }
}

?>
