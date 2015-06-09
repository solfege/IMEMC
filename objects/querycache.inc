<?php

/************************************************************************/
/* Oscailt                                                         */
/* Indepenent Media Centre Content Management System                  */
/* ====================================================================   */
/* Copyright (c)2003-2005 by Independent Media Centre Ireland           */
/* http://www.indymedia.ie                                              */
/* Development List: oscailt@lists.indymedia.org                        */
/* See contributions.txt for the list of contributors                   */
/*                                                                      */
/* This program is free software. You can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation. http://www.gnu.org/copyleft/gpl.html   */
/*                                                               */
/* This program is distributed in the hope that it will be useful,     */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of       */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       */
/* GNU General Public License for more details.                        */
/************************************************************************/


class QueryCache
{
   var $new_cache = true;
   var $sql_cache_invalid=array(
	  0 => true,
	  1 => false,
	  2 => false,
	  3 => false,
	  4 => false,
	  5 => false,
       );

   // New functions that have been added to give a 2 minute time out when the database is missing
   function isDatabaseMissing()
   {
   	global $system_config, $path_prefix;
   	$file = $path_prefix.$system_config->query_cache_store."db.flag";
   	if(file_exists($file))
   	{
   		if(time() - filemtime($file) > 120)
   		{
   			unlink($file);
			// May as well check the db cache directory exists too.
                        // $this->checkCacheDirs();
   		}
   		else
   		{
   			return true;
   		}
   	}
   	return false;
   }

   function databaseMissing()
   {
      global $system_config, $path_prefix;
      $file = $path_prefix.$system_config->query_cache_store."db.flag";
      touch($file);
   }

   function checkCacheDirs($return_error=false,$create_dir=false, $return_path_list =false)
   {
      global $path_prefix, $system_config;

      $base_msg = "Unable to create db cache directory ";
      $ret_str = array();
      $ret_paths = array();

      // The new cache still requires this directory.

      $cache_dir = $path_prefix. $system_config->query_cache_store;
      if (file_exists($cache_dir) === FALSE) {
          $dh=opendir($cache_dir);
	  if ($dh == null) {
              if (!mkdir($cache_dir, $system_config->default_writable_directory_permissions)) {
	          logMessage($base_msg.$cache_dir,"sitelog.txt");
	          if ($return_error == true) $ret_str[] = $base_msg . $cache_dir;
              } else {
	          if ($return_path_list == true) $ret_paths[] = $cache_dir;
	          if ($return_error == true) $ret_str[] = "Created directory " . $cache_dir;
              }
          } else {
              closedir($dh);
	  }
      } 

      // If new cache is in use check those directories.

      if ($this->new_cache == true) {
          for ($cache_group = 1;$cache_group <= 5; $cache_group++) {
	      $cache_dir = $path_prefix.$system_config->query_cache_store.$cache_group."/";
	      if (!file_exists($cache_dir)) {
                  if (!mkdir($cache_dir, $system_config->default_writable_directory_permissions)) {
	              logMessage($base_msg.$cache_dir,"sitelog.txt");
	              if ($return_error == true) $ret_str[] = $base_msg . $cache_dir;
                  } else {
	              if ($return_path_list == true) $ret_paths[] = $cache_dir;
	              if ($return_error == true) $ret_str[] = "Created directory " . $cache_dir;
	          }
	      }
	  }
      }

      // If this is true, it has higher precendence over return_errors
      if ($return_path_list == true) return $ret_paths;

      return $ret_str;
   }

   function getCachedData($query,$cachetype, $cache_group =2)
   {
      global $system_config,$path_prefix, $sql_debug;
      $md = md5($query);

      if($cachetype == 'sql')
      {
         if($this->new_cache == true)
             $file = $path_prefix.$system_config->query_cache_store.$cache_group."/".$md.".".$cachetype.$cache_group."cache";
	 else
             $file = $path_prefix.$system_config->query_cache_store.$md.".".$cachetype.$cache_group."cache";
      }
      else
      {
         $file = $path_prefix.$system_config->query_cache_store.$md.".".$cachetype."cache";
      }

      if (file_exists($file))
      {
         $results = unserialize(implode('',file($file)));
         if($sql_debug) echo "<b>Cache hit:</b> $file<br>";
         return $results;
      }
      else return null;
   }

   function getCachedTime($query,$cachetype, $cache_group = 2)
   {
      global $system_config,$path_prefix;

      if($cachetype == 'sql')
      {
         if($this->new_cache == true)
             $file = $path_prefix.$system_config->query_cache_store.$cache_group."/".md5($query).".".$cachetype.$cache_group."cache";
         else
             $file = $path_prefix.$system_config->query_cache_store.md5($query).".".$cachetype.$cache_group."cache";
      }
      else
      {
         $file = $path_prefix.$system_config->query_cache_store.md5($query).".".$cachetype."cache";
      }

      if (file_exists($file))
      {
         return filemtime($file);
      }
      return null;
   }

   function touchCachedData($query,$cachetype, $cache_group = 2)
   {
      global $system_config,$path_prefix;
      if($cachetype == 'sql')
      {
         if($this->new_cache == true)
             $file = $path_prefix.$system_config->query_cache_store.$cache_group."/".md5($query).".".$cachetype.$cache_group."cache";
	 else
             $file = $path_prefix.$system_config->query_cache_store.md5($query).".".$cachetype.$cache_group."cache";
      }
      else
      {
         $file = $path_prefix.$system_config->query_cache_store.md5($query).".".$cachetype."cache";
      }

      if (file_exists($file))
      {
         touch($file);
      }
      return null;
   }

   function deleteCachedData($query,$cachetype, $cache_group=2)
   {
      global $system_config,$path_prefix;
      if($cachetype == 'sql')
      {
         if($this->new_cache == true)
             $file = $path_prefix.$system_config->query_cache_store.$cache_group."/".md5($query).".".$cachetype.$cache_group."cache";
	 else
             $file = $path_prefix.$system_config->query_cache_store.md5($query).".".$cachetype.$cache_group."cache";
      }
      else
      {
         $file = $path_prefix.$system_config->query_cache_store.md5($query).".".$cachetype."cache";
      }

      if (file_exists($file))
      {
         unlink($file);
      }
   }

   function cacheData($query,$data,$cachetype,$cache_group = 2)
   {
      global $system_config,$path_prefix;

      if($cachetype == 'sql')
      {
         if($this->new_cache == true)
             $file = $path_prefix.$system_config->query_cache_store.$cache_group."/".md5($query).".".$cachetype.$cache_group."cache";
	 else
             $file = $path_prefix.$system_config->query_cache_store.md5($query).".".$cachetype.$cache_group."cache";
      }
      else
      {
         $file = $path_prefix.$system_config->query_cache_store.md5($query).".".$cachetype."cache";
      }

      $OUTPUT = serialize($data);
      // open file with Write permission
      $fp = fopen($file,"w");
      // locking file
      if (flock($fp, LOCK_EX))
      {
         fputs($fp, $OUTPUT);
         // unlocking file
         flock($fp, LOCK_UN);
      }
      /*else //windows 98 doesn't support locking
      {
         fputs($fp, $OUTPUT);
      }*/
      fclose($fp);
   }

   function clearCacheDirectory($cache_dir, $cachetype, $cache_group)
   {
      $cachetype_group = ".".$cachetype.$cache_group."cache";
      $cache_file_count =0;

      $dh=opendir($cache_dir);

      if ($dh === FALSE or $dh == null) {
          reportError("Couldn't open query cache directory: ".$cache_dir);
	  return 0;
      }

      $sql_cache = true;
      if($cachetype == "rss" or $cachetype == "oml") $sql_cache = false;

      while($file=readdir($dh))
      {
         if(!is_dir($cache_dir.$file))
         {
            if($sql_cache == false) {
               unlink($cache_dir."/".$file);
               $cache_file_count++;
            } else {
               if($cache_group > 0)
	       {
                  if(stristr($file, $cachetype_group) !=false)
                  {
                     unlink($cache_dir."/".$file);
                     $cache_file_count++;
                  }
	       }
	       else 
	       {
                  # For the new cache, we never set the cache_group = 0 when calling this function.
                  if ($this->new_cache == false) 
	          {
                      # If = 0 clear all... but 2 contain most files
                      if( (stristr($file, $cachetype."2cache") !=false) or (stristr($file, $cachetype."1cache") !=false) or (stristr($file, $cachetype."3cache") !=false) or (stristr($file, $cachetype."4cache") !=false ) or (stristr($file, $cachetype."5cache") !=false ))
                      {
                         unlink($cache_dir."/".$file);
                         $cache_file_count++;
	              }
	          }
               }
            }
         }
      }
      closedir($dh);

      return $cache_file_count;
   }

   # We have to handle the old and new director structures for the db cache. The old has all the files
   # in the same directory while the new puts them in sub-directories with cachegroup as the dirname.
   # Note: rss and oml caches should have group numbers. Ideally they should be called with zero.
   function clearCache($cachetype, $cache_group = 2)
   {
      global $system_config,$path_prefix;
      if($cachetype == 'rss')
      {
         $cache_dir = $path_prefix.$system_config->rss_cache;
      }
      else
      {
         if($cachetype == 'sql' && $this->new_cache == true && $cache_group > 0)
             $cache_dir = $path_prefix.$system_config->query_cache_store.$cache_group."/";
         else
             $cache_dir = $path_prefix.$system_config->query_cache_store;
      }
      # Must put here otherwise will be undefined for RSS. No changes for RSS cache dir structure
   
      $cache_file_count =0;

      if ($this->new_cache == true && $cachetype == 'sql') {
          if($cache_group == 0) 
	  {
	      for ($i_cache_group = 1;$i_cache_group <= 5; $i_cache_group++) 
	      {
                  $cache_dir = $path_prefix.$system_config->query_cache_store.$i_cache_group."/";
                  $t_total = $this->clearCacheDirectory($cache_dir, "sql", $i_cache_group);
                  $cache_file_count = $cache_file_count + $t_total;
	      }
          } 
	  else 
	  {
              $cache_file_count = $this->clearCacheDirectory($cache_dir, $cachetype, $cache_group);
          }
      } else {
          // Handles old style and oml cache clear.
          $cache_file_count = $this->clearCacheDirectory($cache_dir, $cachetype, $cache_group);
      }


      if($cachetype == 'sql')
      {
         if($cache_group == 0)
         {
             $this->sql_cache_invalid[1]=false;
             $this->sql_cache_invalid[2]=false;
             $this->sql_cache_invalid[3]=false;
             $this->sql_cache_invalid[4]=false;
             $this->sql_cache_invalid[5]=false;
         }
         else
         {
             $this->sql_cache_invalid[$cache_group]=false;
         }
	 reportInfo("sql cache cleared. Group: ".$cache_group." No of files was: ".$cache_file_count);
      }
      else
      {
	 reportInfo($cachetype." cache cleared. No of files was: ".$cache_file_count);
      }
   }

   function invalidateSQLCache($cache_group =2)
   {

      // In some circumstances (very rare), you may not want to break the cache, so if you pass 0 in
      // the original call, then we handle it here. Besides there is no entry for zero.
      if ($cache_group == 0) return;

      $this->sql_cache_invalid[$cache_group]=true;

      # Now apply the dependency rules between caches for clearing them.
      # For 2 and 3 do nothing extra.
      if ($cache_group == 1 )
      {
         $this->sql_cache_invalid[2]=true;
         $this->sql_cache_invalid[3]=true;
         $this->sql_cache_invalid[4]=true;
      } 
      else if ($cache_group == 4 )
      {
         $this->sql_cache_invalid[1]=true;
         // $this->sql_cache_invalid[2]=true;
         $this->sql_cache_invalid[3]=true;
      } 

   }
}
?>
